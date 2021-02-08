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

import com.ibm.streams.operator.StreamingData;
import java.util.Collection;

/**
 * Declaration of a port for an {@link OperatorInvocation}.
 *
 * @since InfoSphere&reg; Streams Version 3.1
 */
public interface PortDeclaration<T extends PortDeclaration<T>> extends StreamingData {

  /**
   * Get the operator invocation for this port.
   *
   * @return the operator invocation for this port
   */
  public OperatorInvocation<?> operator();

  /**
   * Is the port connected in the graph.
   *
   * @return true if the port is connected. Otherwise false.
   */
  public boolean isConnected();

  /**
   * Get the list of connections for this port.
   *
   * @return list of connections for the port
   */
  public Collection<StreamConnection> getConnections();
}
