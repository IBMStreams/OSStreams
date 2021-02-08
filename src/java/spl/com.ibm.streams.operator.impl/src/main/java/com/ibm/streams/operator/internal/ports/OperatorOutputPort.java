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

package com.ibm.streams.operator.internal.ports;

import com.ibm.streams.operator.Attribute;
import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.StreamingOutput;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.eventtime.EventTimeContext;
import com.ibm.streams.operator.internal.eventtime.EventTimeContextImpl;
import com.ibm.streams.operator.internal.eventtime.Watermark;
import com.ibm.streams.operator.internal.ports.actions.GetExportProperties;
import com.ibm.streams.operator.internal.ports.actions.RemoveExportProperties;
import com.ibm.streams.operator.internal.ports.actions.SetExportProperties;
import com.ibm.streams.operator.internal.ports.actions.WatermarkAction;
import com.ibm.streams.operator.internal.runtime.AbstractStream;
import com.ibm.streams.operator.internal.runtime.api.Adapter;
import com.ibm.streams.operator.internal.runtime.api.SPLRuntime;
import com.ibm.streams.operator.meta.CollectionType;
import com.ibm.streams.operator.types.RString;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.Callable;

/**
 * Single output port implementation with different functionality driven by the RuntimeStream
 * handler used to action stream events. Used for compile time and runtime (all connection
 * combinations).
 */
public class OperatorOutputPort<T extends OutputTuple> extends AbstractStream
    implements StreamingOutput<T>, WatermarkSender {

  private final ExportType exportType;
  private RuntimeStream<Tuple> handler;
  private final TupleFactory<T> tupleFactory;

  public OperatorOutputPort(
      int port,
      ExportType exportType,
      String name,
      StreamSchema schema,
      int pePortNumber,
      RuntimeStream<Tuple> handler,
      TupleFactory<T> tupleFactory) {
    super(port, name, schema, pePortNumber);
    this.exportType = exportType;
    this.handler = handler;
    this.tupleFactory = tupleFactory;
  }

  /** An StreamingOutput that is not connected to a PE port (and hence not exported) */
  public OperatorOutputPort(
      int port,
      String name,
      StreamSchema schema,
      RuntimeStream<Tuple> handler,
      TupleFactory<T> tupleFactory) {
    super(port, name, schema, NOT_CONNECTED_TO_PE_PORT);
    this.exportType = ExportType.NOT_EXPORTED;
    this.handler = handler;
    this.tupleFactory = tupleFactory;
  }

  /*
   * Tuple submission methods.
   */

  /**
   * Validate the incoming tuple has the correct schema. When this class creates the tuple, there is
   * no need for the schema to be checked.
   */
  private final void checkSchema(Tuple tuple) {
    final StreamSchema tupleSchema = tuple.getStreamSchema();
    if (!getStreamSchema().equals(tupleSchema)) {
      throw new UnsupportedOperationException(
          new Message(Key.SPL_RUNTIME_TUPLE_TYPE_MISMATCH, getStreamSchema(), tupleSchema)
              .getLocalizedMessageAndPrefix());
    }
  }

  @Override
  public final Callable<Boolean> deferredSubmit(final T tuple) {
    checkSchema(tuple);
    return new Callable<Boolean>() {

      public Boolean call() throws Exception {
        handler.tuple(tuple);
        return false;
      }
    };
  }

  @Override
  public boolean submit(T tuple) throws Exception {
    checkSchema(tuple);
    handler.tuple(tuple);
    return false;
  }

  @Override
  public boolean submit(Tuple tuple) throws Exception {
    checkSchema(tuple);
    handler.tuple(tuple);
    return false;
  }

  /** Utility method to submit from a varargs list. */
  @Override
  public final boolean submitAsTuple(Object... attributeValues) throws Exception {
    final Tuple tuple = getStreamSchema().getTuple(attributeValues);
    handler.tuple(tuple);
    return false;
  }
  /** Utility method to submit from a map. */
  @Override
  public final boolean submitMapAsTuple(Map<String, ?> map) throws Exception {
    final Tuple tuple = getStreamSchema().getTuple(map);
    handler.tuple(tuple);
    return false;
  }
  /** Utility method to submit from a list. */
  @Override
  public final boolean submitListAsTuple(List<?> list) throws Exception {
    final Tuple tuple = getStreamSchema().getTuple(list);
    handler.tuple(tuple);
    return false;
  }

  @Override
  public T newTuple() {
    return tupleFactory.newTuple();
  }

  /*
   ** Punctuation handling
   */
  /**
   * Punctuate the output stream with the given mark. Delivery of the mark to the underlying
   * physical stream is handled by the sub-class's deliver method.
   *
   * @param mark Punctuation to send, if mark is NON_EXISTENT then no action is taken.
   */
  public final boolean punctuate(Punctuation mark) throws Exception {
    if (mark == null) throw new NullPointerException();
    if (mark == Punctuation.NON_EXISTENT) return false;
    handler.mark(mark);
    return false;
  }

  /*
   * Exported port handling.
   */

  protected final void checkExportedByProperties() {
    if (getExportType() != StreamingOutput.ExportType.BY_PROPERTIES)
      throw new IllegalStateException(
          new Message(
                  Key.SPL_RUNTIME_EXPORT_NOT_BY_PROPERTIES,
                  getName(),
                  getExportType(),
                  ExportType.BY_PROPERTIES)
              .getLocalizedMessageAndPrefix());
  }

  @Override
  public void sendWatermark(Watermark wm) {
    executeAction(new WatermarkAction(wm));
  }

  @Override
  public final ExportType getExportType() {
    return exportType;
  }

  @Override
  public final boolean isExported() {
    return getExportType() != ExportType.NOT_EXPORTED;
  }

  @Override
  public void removeStreamProperties(String... name) {
    checkExportedByProperties();
    if (name == null) throw new NullPointerException();
    for (String n : name) if (n == null) throw new NullPointerException();
    executeAction(new RemoveExportProperties(name));
  }

  @Override
  public final void addStreamProperties(Map<String, ?> properties) {
    setStreamProperties(properties, false);
  }

  @Override
  public final void setStreamProperties(Map<String, ?> properties) {
    setStreamProperties(properties, true);
  }

  private final void setStreamProperties(Map<String, ?> properties, boolean set) {
    checkExportedByProperties();
    executeAction(new SetExportProperties(properties, set));
  }

  @Override
  public Tuple getStreamPropertiesAsTuple() {
    checkExportedByProperties();
    return executeAction(new GetExportProperties());
  }

  /** Executes a StreamAction wrapping any exception in a RuntimeException. */
  private <A> A executeAction(StreamAction<A> action) {
    try {
      return handler.action(action);
    } catch (RuntimeException re) {
      throw re;
    } catch (Exception e) {
      throw new RuntimeException(e);
    }
  }

  /** Convert the exported stream properties object from a tuple to a Map. */
  @SuppressWarnings("unchecked")
  @Override
  public final Map<String, Object> getStreamProperties() {
    Tuple tuple = getStreamPropertiesAsTuple();
    Map<String, Object> properties = new HashMap<String, Object>();

    for (Attribute attribute : tuple.getStreamSchema()) {
      String name = attribute.getName();
      Object value;

      switch (attribute.getType().getMetaType()) {
        case RSTRING:
          value = tuple.getString(name);
          break;
        case LIST:
          switch (((CollectionType) attribute.getType()).getElementType().getMetaType()) {
            case RSTRING:
              {
                List<RString> lrs = (List<RString>) tuple.getList(name);
                String[] sa = new String[lrs.size()];
                for (int i = 0; i < sa.length; i++) sa[i] = lrs.get(i).getString();
                value = sa;
                break;
              }
            default:
              value = tuple.getObject(name);
              break;
          }
          break;
        default:
          value = tuple.getObject(name);
          break;
      }
      properties.put(name, value);
    }

    return properties;
  }

  /** Initialize watermark generation, if this output port requires it. */
  public void initializeWatermarkSource() {
    if (SPLRuntime.isJavaOperatorActive()) {
      Adapter adapter = SPLRuntime.getActive();
      if (adapter != null) {
        OperatorContext context = adapter.getContext();
        EventTimeContextImpl etc =
            (EventTimeContextImpl) context.getOptionalContext(EventTimeContext.class);
        if (etc != null) {
          if ((etc.getMode() == EventTimeContext.WatermarkMode.OUTPUT_EVENT_TIME)
              && etc.isOutputPortEnabled(getPortNumber())) {
            WatermarkSource source = new WatermarkSource(etc, handler);
            handler = source;
          }
        }
      }
    }
  }

  private class WatermarkSource implements RuntimeStream<Tuple> {

    public WatermarkSource(EventTimeContextImpl eventTimeContext, RuntimeStream<Tuple> handler) {
      this.eventTimeContext = eventTimeContext;
      this.handler = handler;
    }

    @Override
    public void tuple(Tuple tuple) throws Exception {
      handler.tuple(tuple);
      eventTimeContext.tuple(tuple, getPortNumber());
    }

    @Override
    public void mark(Punctuation mark) throws Exception {
      handler.mark(mark);
    }

    @Override
    public <A> A action(StreamAction<A> action) throws Exception {
      return handler.action(action);
    }

    private EventTimeContextImpl eventTimeContext;
    private RuntimeStream<Tuple> handler;
  }
}
