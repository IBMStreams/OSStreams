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

package com.ibm.streams.operator.internal.runtime.api;

import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.OptionalContext;
import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.StreamingOutput;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.internal.pemodel.PEModel;
import com.ibm.streams.operator.metrics.Metric;
import com.ibm.streams.operator.metrics.Metric.Kind;
import com.ibm.streams.operator.metrics.OperatorMetrics;
import com.ibm.streams.operator.state.StateHandler;
import java.net.MalformedURLException;
import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * Represents an operator that is not a Java primitive operator, i.e. a CPP primitive or a Custom
 * operator. In this case a Java function may execute in the custom logic of the operator but still
 * require access to the full Java environment.
 */
class ModelOperator extends BaseOpAdapter {

  private final OperatorContext context;
  private final OperatorInfo mgmt;
  private final ExecutionContext ec;

  public static Adapter addModelOperator(
      SPLRuntime runtime, int operatorIndex, ExecutionContext ec) {
    return new ModelOperator(runtime.getModel(), operatorIndex, ec);
  }

  private ModelOperator(PEModel model, int operatorIndex, final ExecutionContext ec) {
    super(model, operatorIndex);
    this.ec = ec;
    context =
        new OpContext(getInvokeInfo(), this) {

          @Override
          public int getNumberOfStreamingInputs() {
            throw new UnsupportedOperationException();
          }

          @Override
          public List<StreamingInput<Tuple>> getStreamingInputs() {
            throw new UnsupportedOperationException();
          }

          @Override
          public int getNumberOfStreamingOutputs() {
            throw new UnsupportedOperationException();
          }

          @Override
          public List<StreamingOutput<OutputTuple>> getStreamingOutputs() {
            throw new UnsupportedOperationException();
          }

          @Override
          public List<String> getParameterValues(String parameterName) {
            throw new UnsupportedOperationException();
          }

          @Override
          public Set<String> getParameterNames() {
            throw new UnsupportedOperationException();
          }

          @Override
          public void registerStateHandler(StateHandler handler) {
            throw new UnsupportedOperationException();
          }

          @Override
          public OperatorMetrics getMetrics() {
            return new OperatorMetrics() {

              @Override
              public Metric createCustomMetric(
                  final String name, final String description, final Kind kind) {
                return new Metric() {

                  @Override
                  public String getName() {
                    return name;
                  }

                  @Override
                  public String getDescription() {
                    return description;
                  }

                  @Override
                  public Kind getKind() {
                    return kind;
                  }

                  @Override
                  public long getValue() {
                    // TODO Auto-generated method stub
                    return 0;
                  }

                  @Override
                  public void setValue(long value) {
                    // TODO Auto-generated method stub

                  }

                  @Override
                  public void incrementValue(long delta) {
                    // TODO Auto-generated method stub

                  }

                  @Override
                  public void increment() {
                    // TODO Auto-generated method stub

                  }
                };
              }

              @Override
              public Metric getCustomMetric(String name) {
                throw new UnsupportedOperationException();
              }

              @Override
              public Map<String, Metric> getCustomMetrics() {
                throw new UnsupportedOperationException();
              }

              @Override
              public Metric getInputPortMetric(int port, InputPortMetric name) {
                throw new UnsupportedOperationException();
              }

              @Override
              public Metric getOutputPortMetric(int port, OutputPortMetric name) {
                throw new UnsupportedOperationException();
              }

              @Override
              public Metric getOperatorMetric(SystemMetric name) {
                throw new UnsupportedOperationException();
              }
            };
          }

          @Override
          public void addClassLibraries(String[] libraries) throws MalformedURLException {

            throw new UnsupportedOperationException();
          }

          @Override
          public <A extends OptionalContext> A getOptionalContext(Class<A> contextType) {
            return optionalContext(contextType);
          }
        };

    mgmt = new OperatorInfo(this);
    mgmt.registerMXBeans();
  }

  @Override
  public boolean isJavaOp() {
    return false;
  }

  @Override
  public ExecutionContext getExecutionContext() {
    return ec;
  }

  @Override
  public void notifyCompleter() {
    // TODO Auto-generated method stub

  }

  @Override
  public ClassLoader getClassLoader() {
    return this.getClass().getClassLoader();
  }

  @Override
  public String getOperatorClassName() {
    throw new IllegalStateException();
  }

  @Override
  public boolean backgroundException(Thread t, Throwable e) {
    // TODO Auto-generated method stub
    return false;
  }

  @Override
  public OperatorContext getContext() {
    return context;
  }

  @Override
  public OperatorInfo getManagement() {
    return mgmt;
  }
}
