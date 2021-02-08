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

package com.ibm.streams.instance.sam.model.runtime;

import com.api.json.JSONObject;
import com.ibm.streams.instance.sam.model.topology.TopologyTupleTypeType;

public class RuntimeStream {

  private StreamId streamId = null;
  private String operInstanceName = null;
  private String operInstanceOutputPortName = null;
  private String streamName = null;
  private boolean isLogical = false;

  private int numChannels = -1;
  private String logicalStreamName = null;
  private String logicalOperInstanceName = null;
  private int channelNum = -1;
  // <xs:element name="tupleType" type="base:tupleTypeType" minOccurs="0" maxOccurs="1"/>
  private TopologyTupleTypeType tupleType = null;

  public RuntimeStream() {}

  public RuntimeStream(JSONObject json) {
    streamId = new StreamId((JSONObject) json.get("streamId"));
    operInstanceName = (String) json.get("operInstanceName");
    operInstanceOutputPortName = (String) json.get("operInstanceOutputPortName");
    streamName = (String) json.get("streamName");
    isLogical = Boolean.parseBoolean((String) json.get("isLogical"));

    if (json.get("numChannels") != null) {
      numChannels = Integer.parseInt((String) json.get("numChannels"));
    }
    if (json.get("channelNum") != null) {
      channelNum = Integer.parseInt((String) json.get("channelNum"));
    }
    if (json.get("logicalStreamName") != null) {
      logicalStreamName = (String) json.get("logicalStreamName");
    }
    if (json.get("logicalOperInstanceName") != null) {
      logicalOperInstanceName = (String) json.get("logicalOperInstanceName");
    }
  }

  public JSONObject toJson() {
    JSONObject json = new JSONObject();

    json.put("streamId", streamId.toJson());
    json.put("operInstanceName", operInstanceName);
    json.put("operInstanceOutputPortName", operInstanceOutputPortName);
    json.put("streamName", streamName);
    json.put("isLogical", Boolean.toString(isLogical));

    if (numChannels != -1) {
      json.put("numChannels", Integer.toString(numChannels));
    }
    if (logicalStreamName != null) {
      json.put("logicalStreamName", logicalStreamName);
    }
    if (logicalOperInstanceName != null) {
      json.put("logicalOperInstanceName", logicalOperInstanceName);
    }
    if (channelNum != -1) {
      json.put("channelNum", Integer.toString(channelNum));
    }
    return json;
  }

  public StreamId getStreamId() {
    return streamId;
  }

  public void setStreamId(StreamId id) {
    streamId = id;
  }

  public String getOperInstanceName() {
    return operInstanceName;
  }

  public void setOperInstanceName(String name) {
    operInstanceName = name;
  }

  public String getOperInstanceOPortName() {
    return operInstanceOutputPortName;
  }

  public void setOperInstanceOPortName(String name) {
    operInstanceOutputPortName = name;
  }

  public String getStreamName() {
    return streamName;
  }

  public void setStreamName(String name) {
    streamName = name;
  }

  public boolean isLogical() {
    return isLogical;
  }

  public void setIsLogical(boolean logical) {
    isLogical = logical;
  }

  public int getNumChannels() {
    return numChannels;
  }

  public void setNumChannels(int num) {
    numChannels = num;
  }

  public String getLogicalStreamName() {
    return logicalStreamName;
  }

  public void setLogicalStreamName(String name) {
    logicalStreamName = name;
  }

  public String getLogicalOperInstanceName() {
    return logicalOperInstanceName;
  }

  public void setLogicalOperInstanceName(String name) {
    logicalOperInstanceName = name;
  }

  public int getChannelNum() {
    return channelNum;
  }

  public void setChannelNum(int num) {
    channelNum = num;
  }

  public TopologyTupleTypeType getTupleType() {
    return tupleType;
  }

  public void setTupleType(TopologyTupleTypeType ttype) {
    tupleType = ttype;
  }
}
