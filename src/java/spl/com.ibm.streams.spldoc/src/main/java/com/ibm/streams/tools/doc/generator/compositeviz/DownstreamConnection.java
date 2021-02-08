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

package com.ibm.streams.tools.doc.generator.compositeviz;

public class DownstreamConnection {
  DownstreamConnection(OperatorInvocation op, int iport) {
    iport_ = iport;
    op_ = op;
  }

  OperatorInvocation getOperatorInvocation() {
    return op_;
  }

  int getInputPort() {
    return iport_;
  }

  private int iport_ = 0;
  private OperatorInvocation op_ = null;
}
