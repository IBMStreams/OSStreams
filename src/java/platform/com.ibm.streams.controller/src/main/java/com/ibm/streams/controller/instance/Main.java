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

package com.ibm.streams.controller.instance;

import com.ibm.streams.controller.executor.DefaultExecutor;
import com.ibm.streams.controller.executor.IExecutor;
import com.ibm.streams.controller.executor.Utils;
import com.ibm.streams.controller.instance.rest.StreamsResourceConfig;
import com.ibm.streams.controller.k8s.utils.TimeUtils;
import com.ibm.streams.controller.utils.KubernetesEnvironment;
import io.fabric8.kubernetes.api.model.ConfigMap;
import io.fabric8.kubernetes.client.DefaultKubernetesClient;
import io.fabric8.kubernetes.client.KubernetesClient;
import io.fabric8.kubernetes.client.KubernetesClientException;
import java.io.IOException;
import java.time.Instant;
import java.util.Map;
import java.util.Optional;
import java.util.Properties;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import javax.ws.rs.core.UriBuilder;
import lombok.var;
import org.apache.log4j.Level;
import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;
import org.glassfish.jersey.jdkhttp.JdkHttpServerFactory;
import org.slf4j.bridge.SLF4JBridgeHandler;

public class Main {

  private static final Lock lock = new ReentrantLock();
  private static final Condition terminated = lock.newCondition();

  private static final int port = 10000;
  private static final String host = "http://0.0.0.0/";

  private static IExecutor loadExecutor(KubernetesClient client, String myNs) {
    IExecutor executor;
    try {
      ConfigMap cm =
          client.configMaps().inNamespace(myNs).withName("instance-operator-failure-configs").get();
      if (cm != null) {
        var props = new Properties();
        Map<String, String> data = cm.getData();
        if (data.containsKey("Injected") && data.get("Injected").equals("true")) {
          executor =
              Utils.instantiate(
                  "com.ibm.streams.controller.executor.DefaultExecutor", IExecutor.class);
        } else {
          executor =
              Utils.instantiate(
                  "com.ibm.streams.controller.executor.PropertyExecutor", IExecutor.class);
          data.put("Injected", "true");
          props.putAll(data);
          executor.setProperties(props);
          ConfigMap newCM = new ConfigMap();
          newCM.setMetadata(cm.getMetadata());
          newCM.setData(data);
          client.configMaps().inNamespace(myNs).createOrReplace(newCM);
        }
      } else {
        executor =
            Utils.instantiate(
                "com.ibm.streams.controller.executor.DefaultExecutor", IExecutor.class);
      }
    } catch (Exception e) {
      e.printStackTrace();
      executor = new DefaultExecutor();
    }
    return executor;
  }

  private static Instant getStartInstant(KubernetesClient client, String ns, String name)
      throws InterruptedException {
    while (true) {
      /*
       * Make sure the pod exists.
       */
      var myPod = client.pods().inNamespace(ns).withName(name).get();
      if (myPod == null) {
        Thread.sleep(100);
        continue;
      }
      /*
       * Make sure the pod has containers.
       */
      var statuses = myPod.getStatus().getContainerStatuses();
      if (statuses == null) {
        Thread.sleep(100);
        continue;
      }
      /*
       * Make sure the pod has our container.
       */
      var status = statuses.stream().filter(cs -> cs.getName().equals("controller")).findFirst();
      if (!status.isPresent()) {
        Thread.sleep(100);
        continue;
      }
      /*
       * Make sure that our container is running.
       */
      var state = status.get().getState();
      if (state == null || state.getRunning() == null) {
        Thread.sleep(100);
        continue;
      }
      /*
       * Return our start time.
       */
      return TimeUtils.getInstance().parse(state.getRunning().getStartedAt());
    }
  }

  public static void main(String[] args) {
    /*
     * Register a shutdown hook.
     */
    Runtime.getRuntime()
        .addShutdownHook(
            new Thread(
                () -> {
                  lock.lock();
                  terminated.signal();
                  lock.unlock();
                }));
    /*
     * Send JUL log events to SLF4J.
     */
    SLF4JBridgeHandler.removeHandlersForRootLogger();
    SLF4JBridgeHandler.install();
    /*
     * Set the JUL loggers' to the minimum verbosity.
     */
    var juls = java.util.logging.LogManager.getLogManager().getLoggerNames();
    while (juls.hasMoreElements()) {
      var logger = java.util.logging.Logger.getLogger(juls.nextElement());
      logger.setLevel(java.util.logging.Level.OFF);
    }
    /*
     * Main try block.
     */
    try {
      /*
       * Grab the controller namespace and pod name environment variable.
       */
      var myName =
          Optional.ofNullable(System.getenv("MY_POD_NAME")).orElseThrow(RuntimeException::new);
      var myNs =
          Optional.ofNullable(System.getenv("MY_POD_NAMESPACE")).orElseThrow(RuntimeException::new);
      /*
       * Grab a new Kube client.
       */
      var client = new DefaultKubernetesClient();
      /*
       * Create the environment.
       */
      var env = new KubernetesEnvironment(client);
      /*
       * Update the loggers.
       */
      var loggers = LogManager.getCurrentLoggers();
      while (loggers.hasMoreElements()) {
        Logger logger = (Logger) loggers.nextElement();
        logger.setLevel(Level.toLevel(env.getStreamsLogLevel()));
      }
      /*
       * Grab the timestamp when the container starts running. Note that this is NOT the creation
       * timestamp. The pod creation timestamp is problematic because if the pod restarts due to
       * container failure the creation timestamp stays the same.
       */
      var cTime = getStartInstant(client, myNs, myName);
      /*
       * Create the Streams instance.
       */
      var instance = new StreamsInstance(client, loadExecutor(client, myNs), myNs, env, cTime);
      /*
       * Create the REST HTTP server.
       */
      var baseUri = UriBuilder.fromUri(host).port(port).build();
      var config = new StreamsResourceConfig(instance, myNs);
      JdkHttpServerFactory.createHttpServer(baseUri, config);
      /*
       * Start the Job FSM and the controllers
       */
      lock.lock();
      instance.start();
      terminated.await();
      lock.unlock();
      /*
       * Close the controllers upon termination.
       */
      instance.close();
    } catch (IOException | KubernetesClientException | InterruptedException e) {
      e.printStackTrace();
    }
  }
}
