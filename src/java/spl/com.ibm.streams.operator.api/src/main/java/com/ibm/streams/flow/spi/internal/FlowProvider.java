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

package com.ibm.streams.flow.spi.internal;

import com.ibm.streams.flow.declare.OperatorGraph;
import com.ibm.streams.flow.javaprimitives.JavaTestableGraph;
import java.util.ServiceLoader;

/** Service provider interface for com.ibm.streams.flow. Internal to Streams. */
public abstract class FlowProvider {

  public static final FlowProvider provider;

  static {
    ServiceLoader<FlowProvider> loader =
        ServiceLoader.load(FlowProvider.class, FlowProvider.class.getClassLoader());
    // Assume this is always a single provider.
    FlowProvider fp = null;
    for (FlowProvider p : loader) fp = p;
    provider = fp;
  }

  /** Create a new OperatorGraph to contain a flow's definition. */
  public abstract OperatorGraph newOperatorGraph();

  /** Create a new JavaTestableGraph for testing Java primitives. */
  public abstract JavaTestableGraph newJavaTestableGraph(OperatorGraph declaredGraph)
      throws Exception;
}
