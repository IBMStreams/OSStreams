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

/**
 * A stream connection between an input and output port.
 *
 * @since InfoSphere&reg; Streams Version 3.1
 */
public interface StreamConnection {

  /**
   * The declaration for the input port that will receive tuples and punctuation marks.
   *
   * @return input port that receives tuples and punctuation marks
   */
  public InputPortDeclaration getInput();

  /**
   * The declaration for the output port that will submit tuples and punctuation marks.
   *
   * @return output port that submits tuples and punctuation marks
   */
  public OutputPortDeclaration getOutput();
}
