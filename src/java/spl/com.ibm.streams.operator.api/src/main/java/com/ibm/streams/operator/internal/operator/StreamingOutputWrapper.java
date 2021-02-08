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

package com.ibm.streams.operator.internal.operator;

import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.StreamingOutput;
import com.ibm.streams.operator.Tuple;
import java.util.List;
import java.util.Map;
import java.util.concurrent.Callable;

public abstract class StreamingOutputWrapper<T> implements StreamingOutput<T> {

  private final StreamingOutput<OutputTuple> output;

  public StreamingOutputWrapper(StreamingOutput<OutputTuple> output) {
    this.output = output;
  }

  public final boolean punctuate(Punctuation mark) throws Exception {
    return getRealOutput().punctuate(mark);
  }

  public final String getName() {
    return getRealOutput().getName();
  }

  public final int getPortNumber() {
    return getRealOutput().getPortNumber();
  }

  public final StreamSchema getStreamSchema() {
    return getRealOutput().getStreamSchema();
  }

  @Override
  public final boolean isConnectedToPEPort() {
    return getRealOutput().isConnectedToPEPort();
  }

  @Override
  public final int getPEPortNumber() {
    return getRealOutput().getPEPortNumber();
  }

  public final StreamingOutput<OutputTuple> getRealOutput() {
    return output;
  }

  public final Callable<Boolean> deferredSubmit(T tuple) {
    return getRealOutput().deferredSubmit(unwrap(tuple));
  }

  public boolean submit(T tuple) throws Exception {
    return getRealOutput().submit(unwrap(tuple));
  }

  public boolean submit(Tuple tuple) throws Exception {
    return getRealOutput().submit(tuple);
  }

  @Override
  public final boolean submitAsTuple(Object... attributeValues) throws Exception {
    return getRealOutput().submitAsTuple(attributeValues);
  }

  @Override
  public final boolean submitListAsTuple(List<?> list) throws Exception {
    return getRealOutput().submitListAsTuple(list);
  }

  @Override
  public final boolean submitMapAsTuple(Map<String, ?> map) throws Exception {
    return getRealOutput().submitMapAsTuple(map);
  }

  protected abstract OutputTuple unwrap(T tuple);

  @Override
  public final com.ibm.streams.operator.StreamingOutput.ExportType getExportType() {
    return getRealOutput().getExportType();
  }

  @Override
  public final boolean isExported() {
    return getRealOutput().isExported();
  }

  @Override
  public final Map<String, Object> getStreamProperties() {
    return getRealOutput().getStreamProperties();
  }

  @Override
  public final Tuple getStreamPropertiesAsTuple() {
    return getRealOutput().getStreamPropertiesAsTuple();
  }

  @Override
  public final void setStreamProperties(Map<String, ?> properties) {
    getRealOutput().setStreamProperties(properties);
  }

  @Override
  public final void addStreamProperties(Map<String, ?> properties) {
    getRealOutput().addStreamProperties(properties);
  }

  @Override
  public final void removeStreamProperties(String... name) {
    getRealOutput().removeStreamProperties(name);
  }
}
