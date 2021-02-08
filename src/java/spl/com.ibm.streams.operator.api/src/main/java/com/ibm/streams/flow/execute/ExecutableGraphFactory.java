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

package com.ibm.streams.flow.execute;

import com.ibm.streams.flow.declare.OperatorGraph;

/**
 * A factory for creating executable graphs of operators.
 *
 * @param <E> Type of executable graph
 * @since InfoSphere&reg; Streams Version 3.1
 */
public interface ExecutableGraphFactory<E extends ExecutableGraph<E>> {

  /**
   * Creates a new executable version of {@code operatorGraph}.
   *
   * @param operatorGraph The operator graph to be executed.
   * @return An executable operator graph.
   */
  public E executable(OperatorGraph operatorGraph) throws Exception;
}
