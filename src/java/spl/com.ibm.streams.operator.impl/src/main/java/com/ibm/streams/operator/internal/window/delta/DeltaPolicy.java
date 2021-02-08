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

package com.ibm.streams.operator.internal.window.delta;

import com.ibm.streams.operator.Attribute;
import com.ibm.streams.operator.internal.window.WindowedTuple;
import com.ibm.streams.operator.types.Timestamp;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.io.Serializable;
import java.math.BigDecimal;
import java.math.MathContext;

/**
 * Implementation of a delta policy for an attribute. A delta policy maintains the attribiute's
 * value for every tuple in the window with an sub-class of WindowedTuple. This allows the tuple
 * reference to be discarded while maintaining the delta value required for window processing.
 *
 * @param <T> Tuple type.
 * @param <W> Windowed tuple type.
 */
public abstract class DeltaPolicy<T, W extends WindowedTuple<T>> implements Serializable {

  private static final long serialVersionUID = -6486538538195284660L;
  final int attributeIndex;

  DeltaPolicy(int attributeIndex) {
    this.attributeIndex = attributeIndex;
  }

  /** Return true of the windowed tuple is in the window defined by the new incoming tuple. */
  public abstract boolean isWindowedTupleInWindow(W windowedTuple, T tuple);

  /**
   * Check if the insertion of the tuple in the window fulfills the requirements to trigger the
   * sliding window. When a new tuple is received, if the value of its trigger attribute is larger
   * than that of the last tuple which triggered the window plus the trigger policy size, the window
   * is triggered again.
   *
   * @param tuple Incoming tuple.
   * @return True if the window needs to be trigger.
   */
  public abstract boolean checkTrigger(T tuple);

  /**
   * Get the WindowedTuple object for the tuple to be inserted into the window. This will contain
   * the attribute's value.
   */
  public abstract W getWindowedTuple(T tuple);

  public static DeltaPolicy<?, ? extends WindowedTuple<?>> getPolicy(
      Attribute attribute, String delta) {
    final int attributeIndex = attribute.getIndex();
    switch (attribute.getType().getMetaType()) {
      case INT8:
      case INT16:
      case INT32:
        return new IntDelta(attributeIndex, Integer.valueOf(delta));
      case INT64:
        return new LongDelta(attributeIndex, Long.valueOf(delta));
      case FLOAT32:
        return new FloatDelta(attributeIndex, Float.valueOf(delta));
      case FLOAT64:
        return new DoubleDelta(attributeIndex, Double.valueOf(delta));
      case DECIMAL32:
        return new DecimalDelta(
            attributeIndex, new BigDecimal(delta, MathContext.DECIMAL32), MathContext.DECIMAL32);
      case DECIMAL64:
        return new DecimalDelta(
            attributeIndex, new BigDecimal(delta, MathContext.DECIMAL64), MathContext.DECIMAL64);
      case DECIMAL128:
        return new DecimalDelta(
            attributeIndex, new BigDecimal(delta, MathContext.DECIMAL128), MathContext.DECIMAL128);
      case TIMESTAMP:
        {
          double deltaSeconds = Double.valueOf(delta);
          return new TimestampDelta(attributeIndex, Timestamp.getTimestamp(deltaSeconds));
        }
    }
    throw new UnsupportedOperationException(
        new Message(Key.SPL_RUNTIME_UNSUPPORTED_ATTR_FOR_DELTA_POLICY, attribute)
            .getLocalizedMessageAndPrefix());
  }
}
