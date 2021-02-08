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

package com.ibm.streams.operator.internal.eventtime;

import static com.ibm.streams.operator.internal.logtrace.Setup.opTrace;

import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.StreamingOutput;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.eventtime.EventTimeContext;
import com.ibm.streams.operator.internal.pemodel.PEModel;
import com.ibm.streams.operator.internal.runtime.api.OperatorAdapter;
import com.ibm.streams.operator.internal.runtime.api.OperatorSetup;
import com.ibm.streams.operator.logging.LogLevel;
import com.ibm.streams.operator.types.Timestamp;
import com.ibm.streams.platform.services.AnnotationAttributeValueType;
import com.ibm.streams.platform.services.AnnotationType;
import com.ibm.streams.platform.services.OperInstanceIncomingConnectionType;
import com.ibm.streams.platform.services.OperInstanceInputPortType;
import com.ibm.streams.platform.services.OperInstanceOutputPortType;
import com.ibm.streams.platform.services.PhysOperInstanceType;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class EventTimeContextImpl implements EventTimeContext {

  /** The name of the custom metric reporting the current watermark value of the operator. */
  public static final String WATERMARK_METRIC_NAME = "currentWatermark";

  public EventTimeContextImpl(
      OperatorAdapter adapter, PEModel model, int operatorIndex, OperatorSetup setup) {
    this.adapter = adapter;
    this.operatorId = operatorIndex;
    this.setup = new EventTimeContextSetup(model, operatorIndex);
    this.watermark = Watermark.ZeroTimestamp;
    modeSet = false;
    initializeIsWindowed(model, setup);
    buildSenders();
  }

  @Override
  public void setMode(WatermarkMode mode) {
    watermarkMode = mode;
    modeSet = true;
  }

  @Override
  public WatermarkMode getMode() {
    return watermarkMode;
  }

  // This is the method for advancing the watermark if the
  // mode is INPUT_WATERMARK.
  // If the watermark mode is INPUT_WATERMARK, check whether the
  // input watermark cause the operator's watermark to advance.
  // If it does, advance the watermark.
  public void receive(final Watermark wm, StreamingInput<Tuple> port) {
    if (watermarkMode == EventTimeContext.WatermarkMode.INPUT_WATERMARK) {
      Timestamp ts = receiver.receive(wm, port.getPortNumber());
      if (ts != Watermark.ZeroTimestamp) {
        setWatermark(ts);
      }
    }
  }

  // This is the method for advancing the watermark if the mode
  // is OUTPUT_EVENT_TIME
  // Handle incoming tuple.  If the mode is OUTPUT_EVENT_TIME,
  // extract the event-time from the tuple, and update the
  // watermark if needed.
  public void tuple(Tuple tuple, int portNumber) {
    if (watermarkMode == EventTimeContext.WatermarkMode.OUTPUT_EVENT_TIME) {
      Timestamp ts = getters[portNumber].getEventTime(tuple);
      Timestamp wm = wmGenerator.getWatermarkFor(ts);
      setWatermark(wm);
    }
  }

  // This is the method for advancing the watermark if the mode
  // is MANUAL.  It can also be used with other watermark modes.
  @Override
  public synchronized void setWatermark(Timestamp wm) {
    if (wm.after(watermark)) {
      watermark = wm;
      wmGenerator.setWatermark(wm);
    }
  }

  // Called after the operator has been initialized had the opportunity
  // to call setMode.
  public void initialize() {
    initializeMode();
    buildReceiver();
    buildSenders();
    buildGetters();
    buildWatermarkGenerator();
  }

  // Called after the last tuple has been processed.
  public void finalize() {
    // Submit a Watermark with the maximum value to
    // trigger any remaining timeInterval windows
    wmGenerator.setWatermark(Watermark.MaxTimestamp);
  }

  private void initializeMode() {
    // If the operator has an @eventTime annnotation,
    // the default mode is OUTPUT_EVENT_TIME.
    // Otherwise, it is INPUT_WATERMARK.
    PhysOperInstanceType physOper = setup.model.getOperatorInstance(setup.operatorIndex);

    boolean isAnnotated =
        physOper.getSplAnnotation().stream()
            .map(annotation -> annotation.getTag())
            .anyMatch(tag -> tag.equals("eventTime"));

    if (!modeSet) {
      setMode(
          isAnnotated
              ? EventTimeContext.WatermarkMode.OUTPUT_EVENT_TIME
              : EventTimeContext.WatermarkMode.INPUT_WATERMARK);
    }

    // Warn if conflicting settings
    if (setup.inputConnections.size() == 0
        && watermarkMode == EventTimeContext.WatermarkMode.INPUT_WATERMARK) {
      opTrace.log(
          LogLevel.WARN,
          new Message(Key.SPL_RUNTIME_JAVA_INPUTWATERMARK_NO_INPUT_PORTS, watermarkMode)
              .getLocalizedMessageAndPrefix());
    } else if (isAnnotated && watermarkMode == EventTimeContext.WatermarkMode.OUTPUT_EVENT_TIME) {
      opTrace.log(
          LogLevel.WARN,
          new Message(Key.SPL_RUNTIME_JAVA_WATERMARK_LAG_IGNORED, watermarkMode)
              .getLocalizedMessageAndPrefix());
    } else {
      opTrace.log(LogLevel.INFO, "EventTimeContext mode " + watermarkMode.toString());
    }
  }

  private static class EventTimeContextSetup {
    public PEModel model;
    public int operatorIndex;
    public String attribute;
    public String resolution;
    public String type;
    public double lag;
    public double gap;
    public List<ConnectionId> inputConnections;
    public List<Integer> outputPorts;

    public EventTimeContextSetup(PEModel model, int operatorIndex) {
      this.model = model;
      this.operatorIndex = operatorIndex;
      this.outputPorts = new ArrayList();
      this.inputConnections = new ArrayList();

      PhysOperInstanceType opInstance = model.getOperatorInstance(operatorIndex);
      boolean lagSeen = false;
      boolean gapSeen = false;
      List<AnnotationType> annotations = opInstance.getAnnotation();
      for (AnnotationType annotation : annotations) {
        if (ET_CONTEXT_ANNOTATION.equals(annotation.getTag())) {
          List<AnnotationAttributeValueType> attrs = annotation.getTuple().getAttr();
          for (AnnotationAttributeValueType attrValue : attrs) {
            final String key = attrValue.getKey();
            final String value = attrValue.getValue();
            if (ET_CONTEXT_ATTRIBUTE.equals(key)) {
              attribute = value;
            } else if (ET_CONTEXT_RESOLUTION.equals(key)) {
              resolution = value;
            } else if (ET_CONTEXT_TYPE.equals(key)) {
              type = value;
            } else if (ET_CONTEXT_GAP.equals(key)) {
              double newGap = Double.parseDouble(value);
              if (!gapSeen || newGap > gap) {
                gap = newGap;
                gapSeen = true;
              }
            } else if (ET_CONTEXT_LAG.equals(key)) {
              double newLag = Double.parseDouble(value);
              if (!lagSeen || newLag > lag) {
                lag = newLag;
                lagSeen = true;
              }
            } else if (ET_CONTEXT_INPUT_CONNECTIONS.equals(key)) {
              // Will be a comma-separated list starting and
              // ending with square brackets, like:
              // "[]", "[[1,0,0]]", or "[[1,2,3],[4,3,2]]"

              // If it is only two characters long, it is the
              // empty list.  Otherwise, remove the first
              // and last two characters, and split at '],['.
              // That will give an array of comma-separated
              // triples.

              if (value.length() == 2) {
                // do nothing.
              } else {
                // inputConnections = new ArrayList<ConnectionId>();

                String strippedValue = value.substring(2, value.length() - 2);

                // split at "],[".
                String[] triples = strippedValue.split("\\],\\[");
                for (String triple : triples) {
                  String[] connectionParts = triple.split(",");

                  assert (connectionParts.length == 3);
                  int inputPortIndex = Integer.valueOf(connectionParts[0]);
                  int connectedOperatorIndex = Integer.valueOf(connectionParts[1]);
                  int outputPortIndex = Integer.valueOf(connectionParts[2]);
                  ConnectionId connection =
                      new ConnectionId(connectedOperatorIndex, outputPortIndex, inputPortIndex);
                  inputConnections.add(connection);
                }
              }
            } else if (ET_CONTEXT_OUTPUT_PORTS.equals(key)) {
              outputPorts.add(Integer.valueOf(value));
            }
          }
        }
      }
    }
  }

  private void buildSenders() {

    List<OperInstanceOutputPortType> outputPorts =
        setup.model.getOperatorInstance(setup.operatorIndex).getOutputPorts().getOutputPort();
    int portCount = outputPorts.size();
    senders = new WatermarkSender[portCount];
    for (OperInstanceOutputPortType outputPort : outputPorts) {
      int portIndex = outputPort.getIndex().intValue();
      senders[portIndex] = new WatermarkSender();
      if (!setup.outputPorts.contains(portIndex)) {
        senders[portIndex].setInactive();
      }
    }
  }

  private void buildWatermarkGenerator() {
    double lag = isWindowed ? 0.0 : setup.lag;
    double gap = setup.gap;

    wmGenerator = new WatermarkGenerator(this, lag, gap);
  }

  private void buildReceiver() {
    if (watermarkMode == EventTimeContext.WatermarkMode.INPUT_WATERMARK) {
      receiver = new WatermarkReceiver(setup);
    } else {
      receiver = null;
    }
  }

  private void buildGetters() {
    if (watermarkMode == EventTimeContext.WatermarkMode.OUTPUT_EVENT_TIME) {
      List<OperInstanceOutputPortType> outputPorts =
          setup.model.getOperatorInstance(setup.operatorIndex).getOutputPorts().getOutputPort();
      int portCount = outputPorts.size();
      getters = new EventTimeGetter[portCount];
      for (OperInstanceOutputPortType outputPort : outputPorts) {
        int portIndex = outputPort.getIndex().intValue();
        if (setup.outputPorts.contains(portIndex)) {
          getters[portIndex] = new EventTimeGetter(setup.attribute, setup.type, setup.resolution);
        } else {
          getters[portIndex] = new EventTimeGetter();
        }
      }
    } else {
      getters = null;
    }
  }

  /** This submits a watermark onto specified port. */
  private boolean submit(Timestamp wm, StreamingOutput<OutputTuple> port) {
    int portIndex = port.getPortNumber();
    if (wm.before(senders[portIndex].watermark) || senders[portIndex].isInactive()) {
      return false;
    }

    senders[portIndex].watermark = wm;
    Watermark watermark = new Watermark(wm, operatorId, portIndex);
    if (port instanceof com.ibm.streams.operator.internal.ports.WatermarkSender) {
      ((com.ibm.streams.operator.internal.ports.WatermarkSender) port).sendWatermark(watermark);
    }
    return true;
  }

  /** This submits a watermark to all ports which have a stream with an event-time attribute. */
  // Used by WatermarkGenerator
  public void submit(Timestamp wm) {
    if (!wm.equals(Watermark.MaxTimestamp)) {
      adapter
          .getContext()
          .getMetrics()
          .getCustomMetric(WATERMARK_METRIC_NAME)
          .setValue(wm.getTime());
    }
    List<StreamingOutput<OutputTuple>> ports = adapter.getExecutionContext().getOutputPorts();
    for (StreamingOutput<OutputTuple> port : ports) {
      submit(wm, port);
    }
  }

  @Override
  public synchronized Timestamp getWatermark() {
    return watermark;
  }

  private void initializeIsWindowed(PEModel model, OperatorSetup setup) {
    PhysOperInstanceType physOper = model.getOperatorInstance(operatorId);
    List<OperInstanceInputPortType> portDefs = physOper.getInputPorts().getInputPort();

    isWindowed =
        portDefs.stream()
            .mapToInt(portDef -> portDef.getIndex().intValue())
            .mapToObj(
                port -> setup.getParameters().getParameterValues(OperatorAdapter.OP_WINDOW + port))
            .anyMatch(windowInfo -> !windowInfo.isEmpty());
  }

  // Used by OperatorOutputPort
  public boolean isOutputPortEnabled(int outputPort) {
    return !(senders[outputPort].isInactive());
  }

  private EventTimeContextSetup setup;
  private Timestamp watermark;

  private int operatorId;
  private boolean isWindowed;
  private WatermarkSender[] senders;
  private WatermarkReceiver receiver;
  private EventTimeGetter[] getters;
  private WatermarkGenerator wmGenerator;
  private WatermarkMode watermarkMode;
  private boolean modeSet = false;

  private OperatorAdapter adapter;

  private static class ConnectionId {
    public int operatorIndex;
    public int oportIndex;
    public int iportIndex;

    public ConnectionId(int operatorIndex, int oportIndex, int iportIndex) {
      this.operatorIndex = operatorIndex;
      this.oportIndex = oportIndex;
      this.iportIndex = iportIndex;
    }

    @Override
    public boolean equals(Object ot) {
      if (ot == null || !(ot instanceof ConnectionId)) return false;
      ConnectionId other = (ConnectionId) ot;
      return other.operatorIndex == operatorIndex
          && other.oportIndex == oportIndex
          && other.iportIndex == iportIndex;
    }

    @Override
    public int hashCode() {
      int s = 17;
      s = 37 * s + operatorIndex;
      s = 37 * s + oportIndex;
      s = 37 * s + iportIndex;
      return s;
    }

    @Override
    public String toString() {
      return "(" + operatorIndex + ", " + oportIndex + ", " + iportIndex + ")";
    }
  }

  private class WatermarkSender {
    public WatermarkSender() {
      watermark = Watermark.ZeroTimestamp;
    }

    public boolean isInactive() {
      return watermark.equals(Watermark.MaxTimestamp);
    }

    public Timestamp watermark;

    public void setInactive() {
      watermark = Watermark.MaxTimestamp;
    }
  }

  private class WatermarkReceiver {

    public WatermarkReceiver(EventTimeContextSetup setup) {
      currentWatermark = Watermark.ZeroTimestamp;

      List<ConnectionId> enabledConnections = setup.inputConnections;

      List<OperInstanceInputPortType> inputPorts =
          setup.model.getOperatorInstance(operatorId).getInputPorts().getInputPort();
      for (OperInstanceInputPortType inputPort : inputPorts) {
        List<OperInstanceIncomingConnectionType> intraPeConnections =
            inputPort.getIntraPeConnection();
        for (OperInstanceIncomingConnectionType intraPeConnection : intraPeConnections) {
          ConnectionId thisConnection =
              new ConnectionId(
                  intraPeConnection.getOperInstanceIndex().intValue(),
                  intraPeConnection.getOportIndex().intValue(),
                  inputPort.getIndex().intValue());
          if (enabledConnections.contains(thisConnection)) {
            initStorage(thisConnection);
          }
        }

        List<OperInstanceIncomingConnectionType> interPeConnections =
            inputPort.getInterPeConnection();
        for (OperInstanceIncomingConnectionType interPeConnection : interPeConnections) {
          ConnectionId thisConnection =
              new ConnectionId(
                  interPeConnection.getOperInstanceIndex().intValue(),
                  interPeConnection.getOportIndex().intValue(),
                  inputPort.getIndex().intValue());
          if (enabledConnections.contains(thisConnection)) {
            initStorage(thisConnection);
          }
        }
      }
    }

    public synchronized Timestamp receive(Watermark wm, int iportIndex) {
      ConnectionId id = new ConnectionId(wm.getOperatorIndex(), wm.getPortIndex(), iportIndex);
      Timestamp existingTimestamp = watermarkMap.get(id);
      if (existingTimestamp == null) {
        throw new IllegalArgumentException(
            new Message(
                    Key.SPL_RUNTIME_JAVA_INVALID_WATERMARK_SOURCE,
                    wm.getOperatorIndex(),
                    wm.getPortIndex())
                .getLocalizedMessageAndPrefix());
      }
      Timestamp inputTimestamp = wm.getTimestamp();
      if (inputTimestamp.after(existingTimestamp)) {
        // Advance watermark for the stream.
        watermarkMap.put(id, inputTimestamp);

        Timestamp minWatermark = getMinWatermark();
        if (currentWatermark.before(minWatermark)) {
          currentWatermark = minWatermark;

          // return advanced watermark
          return currentWatermark;
        }
      } else if (inputTimestamp.before(existingTimestamp)) {
        throw new RuntimeException(
            new Message(
                    Key.SPL_RUNTIME_JAVA_RETROGRADE_WATERMARK,
                    inputTimestamp,
                    existingTimestamp,
                    wm.getOperatorIndex(),
                    wm.getPortIndex())
                .getLocalizedMessageAndPrefix());
      }
      return Watermark.ZeroTimestamp;
    }

    public Timestamp getWatermark() {
      return currentWatermark;
    }

    private void initStorage(ConnectionId inputConnection) {
      watermarkMap.put(inputConnection, Watermark.ZeroTimestamp);
    }

    private Timestamp getMinWatermark() {
      return watermarkMap.values().stream()
          .min(Timestamp::compareTo)
          .orElse(Watermark.ZeroTimestamp);
    }

    private Map<ConnectionId, Timestamp> watermarkMap = new HashMap<ConnectionId, Timestamp>();
    private Timestamp currentWatermark;
  }

  // This class is responsible for extracting the event-time value from
  // an output tuple, and converting it to a timestamp.
  private static class EventTimeGetter {
    private enum EventTimeType {
      NONE,
      TIMESTAMP,
      UINT64,
      INT64
    }

    public EventTimeGetter(String attribute, String type, String resolution) {
      this.attribute = attribute.split("\\.");
      if (type.equals("timestamp")) {
        this.type = EventTimeType.TIMESTAMP;
      } else if (type.equals("uint64")) {
        this.type = EventTimeType.UINT64;
      } else {
        assert (type.equals("int64"));
        this.type = EventTimeType.INT64;
      }

      if (resolution.equals("Milliseconds")) {
        secondsToTicksMultiplier = 1000L;
        nanosToTicksDivisor = 1_000_000L;
      } else if (resolution.equals("Microseconds")) {
        secondsToTicksMultiplier = 1_000_000L;
        nanosToTicksDivisor = 1000L;
      } else { // Nanoseconds
        assert (resolution.equals("Nanoseconds"));
        secondsToTicksMultiplier = 1_000_000_000L;
        nanosToTicksDivisor = 1L;
      }
    }

    public EventTimeGetter() {
      this.attribute = new String[0];
      this.type = EventTimeType.NONE;
      this.secondsToTicksMultiplier = 1L;
      this.nanosToTicksDivisor = 1L;
    }

    public Timestamp getEventTime(final Tuple tuple) {
      Tuple refTuple = tuple;

      for (int index = 0; index < attribute.length - 1; ++index) {
        // Get the tuple.
        refTuple = refTuple.getTuple(attribute[index]);
      }

      switch (type) {
        case TIMESTAMP:
          {
            Timestamp val = refTuple.getTimestamp(attribute[attribute.length - 1]);
            // ignore resolution
            return val;
          }
        case INT64:
          {
            long ticks = refTuple.getLong(attribute[attribute.length - 1]);
            return toTimestamp(ticks);
          }
        case UINT64:
          {
            long ticks = refTuple.getLong(attribute[attribute.length - 1]);
            return toTimestamp(ticks);
          }
        case NONE:
        default:
          {
            return Watermark.ZeroTimestamp;
          }
      }
    }

    private Timestamp toTimestamp(long ticks) {
      return new Timestamp(
          ticks / secondsToTicksMultiplier,
          (int) ((ticks % secondsToTicksMultiplier) * nanosToTicksDivisor));
    }

    private final String[] attribute;
    private final EventTimeType type;
    private final long secondsToTicksMultiplier;
    private final long nanosToTicksDivisor;
  } // class EventTimeGetter

  // some constants used to read the event time context annotation
  public static final String ET_CONTEXT_ANNOTATION = "eventTimeContext";
  private static final String ET_CONTEXT_ATTRIBUTE = "attribute";
  private static final String ET_CONTEXT_RESOLUTION = "resolution";
  private static final String ET_CONTEXT_TYPE = "type";
  private static final String ET_CONTEXT_LAG = "lag";
  private static final String ET_CONTEXT_GAP = "minimumGap";
  private static final String ET_CONTEXT_INPUT_CONNECTIONS = "inputConnections";
  private static final String ET_CONTEXT_OUTPUT_PORTS = "outputPorts";
}
