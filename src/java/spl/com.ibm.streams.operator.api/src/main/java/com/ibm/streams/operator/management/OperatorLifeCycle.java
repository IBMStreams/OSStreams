/*
 * Copyright 2021 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.ibm.streams.operator.management;

import com.ibm.streams.operator.Operator;
import com.ibm.streams.operator.OperatorContext;
import java.io.Closeable;
import java.io.IOException;
import java.util.Map;
import java.util.Set;
import java.util.WeakHashMap;
import javax.management.InstanceNotFoundException;
import javax.management.ListenerNotFoundException;
import javax.management.Notification;
import javax.management.NotificationEmitter;
import javax.management.NotificationFilterSupport;
import javax.management.NotificationListener;

/**
 * Utility methods to interface with an operator's life cycle. Use of these methods allow code to
 * management their own life cycle, driver by an operator's life cycle without requiring explicit
 * calls from {@link Operator#allPortsReady()} or {@link Operator#shutdown()}. Thus code can be
 * setup from {@link Operator#initialize(OperatorContext)} and then respond to all ports ready or
 * shutdown by registering handlers using {@link #addAllPortsReadyAction(OperatorContext,
 * NotificationListener) addAllPortsReadyAction} or {@link #addShutdownAction(OperatorContext,
 * NotificationListener) addShutdownAction}. <br>
 * Notifications are through the operator's {@link
 * com.ibm.streams.operator.management.OperatorContextMXBean} registered in the platform
 * MBeanServer.
 *
 * @since InfoSphere&reg; Streams Version 3.2
 */
public class OperatorLifeCycle {

  /**
   * A map of listeners for Closeable objects. We maintain a single listener per context to help
   * ensure that Closeable objects that are discarded by the operator do not continue to consume
   * memory either through the closeable object or any notification listener. A WeakHashMap is used
   * to ensure we support operators that are themselves garbage collected, such as when running
   * multiple tests using the mock framework.
   */
  private static final Map<OperatorContext, CloseableNotificationListener> closeOnShutdowns =
      new WeakHashMap<OperatorContext, CloseableNotificationListener>();

  /**
   * Register to close a {@code Closeable} when the operator is shutdown. A weak reference is
   * maintained to {@code closeable} to avoid unbounded memory growth when an operator has some form
   * of <EM>reconnect after failure</EM> policy that results in new {@code Closeable} instances.
   * Therefore, an operator cannot rely on shutdown closing the instance if it discards its
   * reference to a {@code Closeable}. In such situations the operator should close the original
   * {@code Closeable} and invoke this method on the new {@code Closeable}. <br>
   * Any exception closing {@code closeable} is ignored.
   *
   * @param context Operator to be monitored for shutdown
   * @param closeable Object to be closed on shutdown
   */
  public static void closeOnShutdown(final OperatorContext context, final Closeable closeable) {

    CloseableNotificationListener listener;

    synchronized (closeOnShutdowns) {
      listener = closeOnShutdowns.get(context);
      if (listener == null) {
        listener = new CloseableNotificationListener();
        closeOnShutdowns.put(context, listener);
        addShutdownAction(context, listener);
      }
    }
    listener.addCloseable(closeable);
  }

  /** Listener that closes the set of Closeables it maintains. */
  private static class CloseableNotificationListener implements NotificationListener {

    private final Map<Closeable, Boolean> closeables = new WeakHashMap<Closeable, Boolean>();
    private boolean shutdown;

    @Override
    public void handleNotification(Notification notification, Object handback) {
      Set<Closeable> items;
      synchronized (this) {
        shutdown = true;
        items = closeables.keySet();
      }

      for (Closeable closeable : items) close(closeable);
    }

    /**
     * Add a closeable, if we have already been shutdown then immediately close it to honour the
     * contract of close on shutdown.
     */
    synchronized void addCloseable(final Closeable closeable) {
      if (shutdown) {
        close(closeable);
        return;
      }
      closeables.put(closeable, Boolean.FALSE);
    }

    private void close(final Closeable closeable) {
      try {
        closeable.close();
      } catch (IOException e) {;
      }
    }
  }

  /**
   * Register an action to be executed when an operator is shutdown.
   *
   * @param context Operator to be monitored for shutdown
   * @param listener Listener to be executed at shutdown
   * @see Operator#shutdown()
   * @see com.ibm.streams.operator.management.OperatorContextMXBean#SHUTDOWN
   */
  public static void addShutdownAction(OperatorContext context, NotificationListener listener) {
    try {
      addOneShotAction(context, OperatorContextMXBean.SHUTDOWN, listener);
    } catch (IllegalStateException e) {
      if (e.getCause() instanceof InstanceNotFoundException) {
        Notification fakeShutdown =
            new Notification(
                OperatorContextMXBean.SHUTDOWN, OperatorManagement.getName(context), -1);
        listener.handleNotification(fakeShutdown, null);
      }
      throw e;
    }
  }
  /**
   * Register an action to be executed when an operator is notified of port readiness.
   *
   * @param context Operator to be monitored for port readiness
   * @param listener Listener to be executed at port readiness
   * @see Operator#allPortsReady()
   * @see com.ibm.streams.operator.management.OperatorContextMXBean#ALL_PORTS_READY
   */
  public static void addAllPortsReadyAction(
      OperatorContext context, NotificationListener listener) {
    addOneShotAction(context, OperatorContextMXBean.ALL_PORTS_READY, listener);
  }

  /**
   * Register an action which will be removed once it has been executed, since the notification it
   * is waiting for is a one time event.
   */
  private static void addOneShotAction(
      OperatorContext context, String notificationType, final NotificationListener listener) {

    OperatorContextMXBean ocmx = OperatorManagement.getOperatorContextMXBean(context);
    final NotificationEmitter ocm = (NotificationEmitter) ocmx;

    final NotificationFilterSupport filter = new NotificationFilterSupport();
    filter.enableType(notificationType);

    final NotificationListener oneShotListener =
        new NotificationListener() {

          @Override
          public void handleNotification(Notification notification, Object handback) {
            listener.handleNotification(notification, handback);
            try {
              ocm.removeNotificationListener(this);
            } catch (ListenerNotFoundException e) {
            }
          }
        };

    try {
      ocm.addNotificationListener(oneShotListener, filter, null);
    } catch (java.lang.reflect.UndeclaredThrowableException ute) {
      if (ute.getCause() instanceof InstanceNotFoundException)
        throw new IllegalStateException(ute.getCause());
      throw ute;
    }
  }
}
