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

package com.ibm.streams.flow.declare;

import com.ibm.streams.operator.Type.MetaType;
import com.ibm.streams.operator.model.InputPortSet;
import com.ibm.streams.operator.window.StreamWindow.Policy;
import com.ibm.streams.operator.window.StreamWindow.Type;
import java.util.concurrent.TimeUnit;

/**
 * Declaration of an input port for an {@link OperatorInvocation}. An input port is created as
 * {@link com.ibm.streams.operator.window.StreamWindow.Type#NOT_WINDOWED not windowed}.
 *
 * @since InfoSphere&reg; Streams Version 3.1
 */
public interface InputPortDeclaration extends PortDeclaration<InputPortDeclaration> {

  /**
   * Connect this input port to output ports.
   *
   * @param outputPorts Output ports to connect to.
   * @return this
   * @exception IllegalStateException At least one of the output ports has a different schema.
   */
  public InputPortDeclaration connect(OutputPortDeclaration... outputPorts);

  /**
   * Set this input port to be a sliding window. The existing window definitions are cleared, and
   * the eviction and trigger policies must be defined before the graph can be executed.
   *
   * @return this
   */
  public InputPortDeclaration sliding();

  /**
   * Set this input port to be a tumbling window. The existing window definitions are cleared, and
   * an eviction policy must be defined before the graph can be executed.
   *
   * @return this
   */
  public InputPortDeclaration tumbling();

  /**
   * Set the window for this input port to be partitioned.
   *
   * @return this
   * @exception IllegalStateException The window type of this input port is {@link
   *     Type#NOT_WINDOWED}
   */
  public InputPortDeclaration partitioned();

  /**
   * Set the type of the window eviction policy to be {@link Policy#COUNT}.
   *
   * @param count Set the number of tuples for the eviction policy.
   * @return this
   * @exception IllegalStateException The window type of this input port is {@link
   *     Type#NOT_WINDOWED}
   * @exception IllegalArgumentException {@code count} is less than zero
   */
  public InputPortDeclaration evictCount(int count);
  /**
   * Set the type of the window eviction policy to be {@link Policy#TIME}.
   *
   * @param seconds Eviction time period in seconds
   * @return this
   * @exception IllegalStateException The window type of this input port is {@link
   *     Type#NOT_WINDOWED}
   */
  public InputPortDeclaration evictTime(double seconds);

  /**
   * Set the type of the window eviction policy to be {@link Policy#TIME}.
   *
   * @param value Eviction time period.
   * @param unit Time unit for {@code value}.
   * @return this
   * @exception IllegalStateException The window type of this input port is {@link
   *     Type#NOT_WINDOWED}
   */
  public InputPortDeclaration evictTime(long value, TimeUnit unit);

  /**
   * Set the type of the window eviction policy to be {@link Policy#PUNCTUATION}.
   *
   * @return this
   * @exception IllegalStateException The window type of this input port is not {@link
   *     Type#TUMBLING}
   */
  public InputPortDeclaration evictPunctuation();

  /**
   * Set the type of the window eviction policy to be {@link Policy#DELTA}. <br>
   * If the type of the attribute is {@link MetaType#TIMESTAMP} then the type of {@code delta} must
   * be {@code Double}. Otherwise the object type of {@code value} must match the {@link
   * com.ibm.streams.operator.Type#getObjectType() object type} of the attribute.
   *
   * @param attribute Attribute name for the delta eviction policy
   * @param delta Delta value for the eviction policy
   * @return this
   * @exception IllegalStateException The window type of this input port is {@link
   *     Type#NOT_WINDOWED}
   */
  public InputPortDeclaration evictDelta(String attribute, Object delta);

  /**
   * Set the type of the window trigger policy to be {@link Policy#COUNT}.
   *
   * @param count Number of tuples
   * @return this
   * @exception IllegalStateException The window type of this input port is not {@link Type#SLIDING}
   */
  public InputPortDeclaration triggerCount(int count);
  /**
   * Set the type of the window trigger policy to be {@link Policy#TIME}.
   *
   * @param seconds Trigger time period in seconds
   * @return this
   * @exception IllegalStateException The window type of this input port is not {@link Type#SLIDING}
   */
  public InputPortDeclaration triggerTime(double seconds);

  /**
   * Set the type of the window trigger policy to be {@link Policy#TIME}.
   *
   * @param value Trigger time period
   * @param unit Time unit for {@code value}
   * @return this
   * @exception IllegalStateException The window type of this input port is not {@link Type#SLIDING}
   */
  public InputPortDeclaration triggerTime(long value, TimeUnit unit);

  /**
   * Set the type of the window trigger policy to be {@link Policy#DELTA}. <br>
   * If the type of the attribute is {@link MetaType#TIMESTAMP} then type of {@code delta} must be
   * {@code Double}, otherwise the object type of {@code value} must match the {@link
   * com.ibm.streams.operator.Type#getObjectType() object type} of the attribute.
   *
   * @param attribute Attribute name for the delta trigger policy
   * @param delta Delta value for the trigger policy
   * @return this
   * @exception IllegalStateException The window type of this input port is not {@link Type#SLIDING}
   */
  public InputPortDeclaration triggerDelta(String attribute, Object delta);

  /**
   * Set the window partition eviction policy to be age based. If a tuple is not inserted into a
   * partition for the given age-out period, the partition is deleted.
   *
   * @param seconds Partition age-out time in seconds
   * @return this
   * @exception IllegalStateException The window type of this input port is {@link
   *     Type#NOT_WINDOWED}
   * @exception IllegalStateException The window is not partitioned.
   * @exception IllegalArgumentException seconds is less than zero
   */
  public InputPortDeclaration partitionEvictionAge(double seconds);

  /**
   * Set the window partition eviction policy to be age based. If a tuple is not inserted into a
   * partition for the given age-out period, the partition is deleted.
   *
   * @param value Partition age-out time
   * @param unit Time unit for {@code value}
   * @return this
   * @exception IllegalStateException The window type of this input port is {@link
   *     Type#NOT_WINDOWED}
   * @exception IllegalStateException The window is not partitioned.
   * @exception @IllegalArgumentException {@code value} is less than or equal to zero
   */
  public InputPortDeclaration partitionEvictionAge(long value, TimeUnit unit);

  /**
   * Set the window partition eviction policy to partition count based. If the number of partitions
   * exceeds {@code count}, partitions are deleted until the partition count is equal to {@code
   * count}.
   *
   * @param count Number of partitions
   * @return this
   * @exception IllegalStateException The window type of this input port is {@link
   *     Type#NOT_WINDOWED}
   * @exception IllegalStateException The window is not partitioned.
   * @exception IllegalArgumentException {@code count} is less than or equal to zero
   */
  public InputPortDeclaration partitionEvictionCount(int count);

  /**
   * Set the window partition eviction policy to tuple count based. If the total number of tuples
   * across all the partitions exceeds the value of {@code count}, partitions are deleted until the
   * tuple count is less than or equal to the value of {@code count}.
   *
   * @param count Number of tuples
   * @return this
   * @exception IllegalStateException The window type of this input port is {@link
   *     Type#NOT_WINDOWED}
   * @exception IllegalStateException The window is not partitioned.
   * @exception IllegalArgumentException {@code count} is less than or equal to zero
   */
  public InputPortDeclaration partitionEvictionTupleCount(int count);

  /**
   * Return the port set for this port if the operator class had an {@link InputPortSet} annotation.
   *
   * @return Port set for this port, or null if the operator class was not annotated.
   * @since InfoSphere&reg; Streams Version 3.2
   */
  public InputPortSet getPortSet();

  public enum CongestionPolicy {
    WAIT,
    DROP_FIRST,
    DROP_LAST
  }

  /**
   * Set this input port to be a threaded port.
   *
   * @param congestionPolicy
   * @param queueSize
   * @return this
   * @since Streams Version 4.1
   */
  public InputPortDeclaration threaded(
      CongestionPolicy congestionPolicy, int queueSize, boolean singleThreadedOnInput);
}
