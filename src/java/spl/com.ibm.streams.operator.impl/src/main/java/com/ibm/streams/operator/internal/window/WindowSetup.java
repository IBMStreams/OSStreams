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

package com.ibm.streams.operator.internal.window;

import com.ibm.streams.operator.Attribute;
import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.internal.window.delta.DeltaPolicy;
import com.ibm.streams.operator.window.StreamWindow;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.util.List;

/**
 * Create a StreamWindow instance for a configured input port. The parameter set for an operator
 * invocation will contain a parameter of name '[window].N]' where N is the port number for every
 * windowed input port. The parameter has multiple values with the first being the type (as an
 * ordinal of the enum) and the second being the partition count. The remaining values depend on the
 * type of the window.
 */
public class WindowSetup {

  /**
   * Return an instance of a Stream window definition.
   *
   * @param <T> Tuple type
   * @param port Input port
   * @param info Window information from the parameters.
   * @return A StreamWindow instance.
   */
  public static <T> StreamWindow<T> getWindow(
      OperatorContext context, StreamingInput<T> port, List<String> info) {

    if (info.isEmpty()) return new InputPortWindow<T, Object, Object>(context, port);

    int iType = Integer.valueOf(info.get(0));
    boolean isPartitioned = "1".equals(info.get(1));
    StreamWindow.Type type = StreamWindow.Type.values()[iType];

    StreamWindow.Policy evictPolicy = getPolicy(info, 2);

    if (type == StreamWindow.Type.TUMBLING)
      return getTumblingWindow(context, port, info, isPartitioned, evictPolicy);
    else return getSlidingWindow(context, port, info, isPartitioned, evictPolicy);
  }

  /**
   * Parameter layout for a tumbling window.
   *
   * <UL>
   *   <LI>0 - Integer form of type 1=TUMBLING,2=SLIDING
   *   <LI>1 - "0" not partitioned, "1" is partitioned.
   *   <LI>2 - Eviction policy - Java enum ordinals match Perl values.
   *   <LI>Remaining info specific to eviction policy
   * </UL>
   *
   * @param info
   * @return
   */
  private static <T> InputPortWindow<T, ?, ?> getTumblingWindow(
      final OperatorContext context,
      final StreamingInput<T> port,
      final List<String> info,
      final boolean isPartitioned,
      final StreamWindow.Policy evictPolicy) {

    switch (evictPolicy) {
      case TIME:
        // item 3 is period of the window in seconds as a String.
        String period = info.get(3);
        return new InputPortWindow<T, String, Object>(
            context, port, isPartitioned, evictPolicy, period, info, 4);
      case COUNT:
        // item 3 is size of the window as an integer
        int size = getCountPolicySize(info, 3);
        return new InputPortWindow<T, Integer, Object>(
            context, port, isPartitioned, evictPolicy, size, info, 4);
      case PUNCTUATION:
        // No more info.
        return new InputPortWindow<T, Object, Object>(
            context, port, isPartitioned, evictPolicy, null, info, 3);
      case DELTA:
        {
          // item 3 is attribute name
          // item 4 is delta value
          DeltaPolicy<?, ? extends WindowedTuple<?>> evictDelta = getDeltaPolicy(port, info, 3);
          return new InputPortWindow<T, DeltaPolicy<?, ? extends WindowedTuple<?>>, Object>(
              context, port, isPartitioned, evictPolicy, evictDelta, info, 5);
        }
    }

    return null;
  }

  /** Get a delta policy from two items: position: attribute name position+1: delta value */
  private static DeltaPolicy<?, ? extends WindowedTuple<?>> getDeltaPolicy(
      StreamingInput<?> port, List<String> info, int position) {
    String attributeName = info.get(position);
    int dot = attributeName.lastIndexOf('.');
    if (dot != -1) attributeName = attributeName.substring(dot + 1);
    String delta = info.get(position + 1);
    Attribute attribute = port.getStreamSchema().getAttribute(attributeName);
    if (attribute == null)
      throw new IllegalStateException(
          new Message(Key.SPL_RUNTIME_WINDOW_ATTR_FOR_DELTA_POLICY_NOT_FOUND, attributeName)
              .getLocalizedMessageAndPrefix());
    return DeltaPolicy.getPolicy(attribute, delta);
  }

  private static StreamWindow.Policy getPolicy(List<String> info, int position) {
    int iPolicy = Integer.valueOf(info.get(position));
    return StreamWindow.Policy.values()[iPolicy];
  }

  private static int getCountPolicySize(List<String> info, int position) {
    return Integer.valueOf(info.get(position));
  }

  /**
   * Parameter layout for a tumbling window.
   *
   * <UL>
   *   <LI>0 - Integer form of type 1=TUMBLING,2=SLIDING
   *   <LI>1 - Integer partition count
   *   <LI>2 - Eviction policy - Java enum ordinals match Perl values.
   *   <LI>Eviction policy items
   *   <LI>N - Trigger policy
   *   <LI>Trigger policy items
   * </UL>
   *
   * @param info
   * @return
   */
  private static <T> InputPortWindow<T, ?, ?> getSlidingWindow(
      final OperatorContext context,
      final StreamingInput<T> port,
      final List<String> info,
      final boolean isPartitioned,
      final StreamWindow.Policy evictPolicy) {

    switch (evictPolicy) {
      case COUNT:
        {
          int evictSize = getCountPolicySize(info, 3);
          StreamWindow.Policy triggerPolicy = getPolicy(info, 4);
          switch (triggerPolicy) {
            case COUNT:
              {
                int triggerSize = getCountPolicySize(info, 5);
                return new InputPortWindow<T, Integer, Integer>(
                    context,
                    port,
                    StreamWindow.Type.SLIDING,
                    isPartitioned,
                    evictPolicy,
                    evictSize,
                    triggerPolicy,
                    triggerSize,
                    info,
                    6);
              }
            case TIME:
              {
                String triggerPeriod = info.get(5);
                return new InputPortWindow<T, Integer, String>(
                    context,
                    port,
                    StreamWindow.Type.SLIDING,
                    isPartitioned,
                    evictPolicy,
                    evictSize,
                    triggerPolicy,
                    triggerPeriod,
                    info,
                    6);
              }
            case DELTA:
              {
                DeltaPolicy<?, ? extends WindowedTuple<?>> triggerDelta =
                    getDeltaPolicy(port, info, 5);
                return new InputPortWindow<T, Integer, DeltaPolicy<?, ? extends WindowedTuple<?>>>(
                    context,
                    port,
                    StreamWindow.Type.SLIDING,
                    isPartitioned,
                    evictPolicy,
                    evictSize,
                    triggerPolicy,
                    triggerDelta,
                    info,
                    7);
              }
          }
        }
      case TIME:
        {
          String evictPeriod = info.get(3);
          StreamWindow.Policy triggerPolicy = getPolicy(info, 4);
          switch (triggerPolicy) {
            case COUNT:
              {
                int triggerSize = getCountPolicySize(info, 5);
                return new InputPortWindow<T, String, Integer>(
                    context,
                    port,
                    StreamWindow.Type.SLIDING,
                    isPartitioned,
                    evictPolicy,
                    evictPeriod,
                    triggerPolicy,
                    triggerSize,
                    info,
                    6);
              }
            case TIME:
              {
                String triggerPeriod = info.get(5);
                return new InputPortWindow<T, String, String>(
                    context,
                    port,
                    StreamWindow.Type.SLIDING,
                    isPartitioned,
                    evictPolicy,
                    evictPeriod,
                    triggerPolicy,
                    triggerPeriod,
                    info,
                    6);
              }
            case DELTA:
              {
                DeltaPolicy<?, ? extends WindowedTuple<?>> triggerDelta =
                    getDeltaPolicy(port, info, 5);
                return new InputPortWindow<T, String, DeltaPolicy<?, ? extends WindowedTuple<?>>>(
                    context,
                    port,
                    StreamWindow.Type.SLIDING,
                    isPartitioned,
                    evictPolicy,
                    evictPeriod,
                    triggerPolicy,
                    triggerDelta,
                    info,
                    7);
              }
          }
        }
      case DELTA:
        {
          DeltaPolicy<?, ? extends WindowedTuple<?>> evictDelta = getDeltaPolicy(port, info, 3);
          StreamWindow.Policy triggerPolicy = getPolicy(info, 5);
          switch (triggerPolicy) {
            case COUNT:
              {
                int triggerSize = getCountPolicySize(info, 6);
                return new InputPortWindow<T, DeltaPolicy<?, ? extends WindowedTuple<?>>, Integer>(
                    context,
                    port,
                    StreamWindow.Type.SLIDING,
                    isPartitioned,
                    evictPolicy,
                    evictDelta,
                    triggerPolicy,
                    triggerSize,
                    info,
                    7);
              }
            case TIME:
              {
                String triggerPeriod = info.get(6);
                return new InputPortWindow<T, DeltaPolicy<?, ? extends WindowedTuple<?>>, String>(
                    context,
                    port,
                    StreamWindow.Type.SLIDING,
                    isPartitioned,
                    evictPolicy,
                    evictDelta,
                    triggerPolicy,
                    triggerPeriod,
                    info,
                    7);
              }
            case DELTA:
              {
                DeltaPolicy<?, ? extends WindowedTuple<?>> triggerDelta =
                    getDeltaPolicy(port, info, 6);
                return new InputPortWindow<
                    T,
                    DeltaPolicy<?, ? extends WindowedTuple<?>>,
                    DeltaPolicy<?, ? extends WindowedTuple<?>>>(
                    context,
                    port,
                    StreamWindow.Type.SLIDING,
                    isPartitioned,
                    evictPolicy,
                    evictDelta,
                    triggerPolicy,
                    triggerDelta,
                    info,
                    8);
              }
          }
        }
    }
    return null;
  }
}
