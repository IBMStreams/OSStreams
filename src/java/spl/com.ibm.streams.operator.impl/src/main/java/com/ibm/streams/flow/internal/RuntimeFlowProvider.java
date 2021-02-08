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

package com.ibm.streams.flow.internal;

import com.ibm.streams.flow.declare.OperatorGraph;
import com.ibm.streams.flow.javaprimitives.JavaTestableGraph;
import com.ibm.streams.flow.spi.internal.FlowProvider;

public class RuntimeFlowProvider extends FlowProvider {

  @Override
  public OperatorGraph newOperatorGraph() {
    return new OpGraph();
  }

  @Override
  public JavaTestableGraph newJavaTestableGraph(OperatorGraph declaredGraph) throws Exception {
    return new GraphTester(declaredGraph);
  }
}
