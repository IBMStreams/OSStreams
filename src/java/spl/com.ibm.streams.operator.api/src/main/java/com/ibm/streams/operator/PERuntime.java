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

package com.ibm.streams.operator;

import com.ibm.streams.operator.spi.internal.RuntimeProvider;

/**
 * Utility methods to get information about the current processing element and operator context.
 * Allows methods that are executing as SPL native Java functions to access operator context.
 *
 * @since InfoSphere&reg; Streams Version 4.0
 */
public class PERuntime {

  /**
   * Get the processing element for the current operator.
   *
   * @return ProcessingElement hosting this Operator instance.
   */
  public static ProcessingElement getPE() {
    return getCurrentContext().getPE();
  }

  /**
   * Get the operator context for the current operator.
   *
   * @return OperatorContext for the current Operator instance.
   */
  public static OperatorContext getCurrentContext() {
    return RuntimeProvider.provider.getCurrentContext();
  }
}
