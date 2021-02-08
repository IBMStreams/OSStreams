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

package com.ibm.streams.operator.window;

/**
 * StreamWindowPartitioner provides the mapping from an tuple being inserted into the window to a
 * partition.
 *
 * @param <T> Tuple type
 * @param <P> Partition type. The partition type must be suitable for use as a key in a Map.
 */
public interface StreamWindowPartitioner<T, P> {

  /**
   * Return the window's partition for the tuple.
   *
   * @param tuple Tuple being inserted into the window.
   * @return Partition for tuple.
   */
  public P getPartition(T tuple);
}
