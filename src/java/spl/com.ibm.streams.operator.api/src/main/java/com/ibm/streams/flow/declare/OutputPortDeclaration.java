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

import com.ibm.streams.operator.model.OutputPortSet;

/**
 * Declaration of output port for an {@link OperatorInvocation}.
 *
 * @since InfoSphere&reg; Streams Version 3.1
 */
public interface OutputPortDeclaration extends PortDeclaration<OutputPortDeclaration> {

  /**
   * Connect this output port to input ports.
   *
   * @param inputPorts Input ports to connect to.
   * @return this
   * @exception IllegalStateException At least one of the input ports has a different schema.
   */
  public OutputPortDeclaration connect(InputPortDeclaration... inputPorts);

  /**
   * Return the port set for this port if the operator class had an {@link OutputPortSet}
   * annotation.
   *
   * @return Port set for this port, or null if the operator class was not annotated.
   * @since InfoSphere&reg; Streams Version 3.2
   */
  public OutputPortSet getPortSet();
}
