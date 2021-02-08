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
import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;
import com.ibm.streams.platform.services.StreamIdType;
import java.math.BigInteger;
import java.util.Objects;

public class StreamId {

  private BigInteger jobId = null;
  private BigInteger peId = null;
  private String operInstanceName = null;
  private BigInteger operInstanceIndex = null;
  private BigInteger operInstanceOutputPortIndex = null;

  public StreamId() {}

  public StreamId(BigInteger jobid, String opername, BigInteger operOPIndex) {
    jobId = jobid;
    operInstanceName = opername;
    operInstanceOutputPortIndex = operOPIndex;
  }

  public StreamId(JSONObject json) {
    jobId = new BigInteger((String) json.get("jobId"));
    if (json.get("peId") != null) {
      peId = new BigInteger((String) json.get("peId"));
    }
    operInstanceName = (String) json.get("operInstanceName");
    if (json.get("operInstanceIndex") != null) {
      operInstanceIndex = new BigInteger((String) json.get("operInstanceIndex"));
    }
    operInstanceOutputPortIndex = new BigInteger((String) json.get("operInstanceOutputPortIndex"));
  }

  public StreamId(StreamIdType streamIdType) {
    setJobId(streamIdType.getJobId().getId());
    setOperInstanceIndex(streamIdType.getOperInstanceIndex());
    setOperInstanceName(streamIdType.getOperInstanceName());
    setOperInstanceOPIndex(streamIdType.getOperInstanceOutputPortIndex());
    if (streamIdType.getPeId() != null) {
      setPeId(streamIdType.getPeId().getId());
    }
  }

  public JSONObject toJson() {
    JSONObject json = new JSONObject();
    json.put("jobId", jobId.toString());
    if (peId != null) {
      json.put("peId", peId.toString());
    }
    json.put("operInstanceName", operInstanceName);
    if (operInstanceIndex != null) {
      json.put("operInstanceIndex", operInstanceIndex.toString());
    }
    json.put("operInstanceOutputPortIndex", operInstanceOutputPortIndex.toString());
    return json;
  }

  @Override
  public int hashCode() {
    return Objects.hash(jobId, operInstanceName, operInstanceOutputPortIndex);
  }

  @Override
  public boolean equals(Object obj) {
    if (obj != null && (obj instanceof StreamId)) {
      StreamId id = (StreamId) obj;
      if (jobId.equals(id.getJobId())
          && operInstanceName.equals(id.getOperInstanceName())
          && operInstanceOutputPortIndex.equals(id.getOperInstanceOPIndex())) {
        return true;
      }
      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace(
            "Comparing "
                + toJson().toString()
                + " with streamID "
                + id.toJson().toString()
                + " return FALSE!");
      }
    }
    return false;
  }

  @Override
  public String toString() {
    StringBuilder builder = new StringBuilder();
    builder.append("jobId=" + jobId);
    builder.append(" peId=" + peId);
    builder.append(" operInstanceName=" + operInstanceName);
    builder.append(" operInstanceIndex=" + operInstanceIndex);
    builder.append(" operInstanceOutputPortIndex=" + operInstanceOutputPortIndex);
    return builder.toString();
  }

  public BigInteger getJobId() {
    return jobId;
  }

  public void setJobId(BigInteger jobid) {
    jobId = jobid;
  }

  public BigInteger getPeId() {
    return peId;
  }

  public void setPeId(BigInteger peid) {
    peId = peid;
  }

  public String getOperInstanceName() {
    return operInstanceName;
  }

  public void setOperInstanceName(String opername) {
    operInstanceName = opername;
  }

  public BigInteger getOperInstanceIndex() {
    return operInstanceIndex;
  }

  public void setOperInstanceIndex(BigInteger operIndex) {
    operInstanceIndex = operIndex;
  }

  public BigInteger getOperInstanceOPIndex() {
    return operInstanceOutputPortIndex;
  }

  public void setOperInstanceOPIndex(BigInteger operOPIndex) {
    operInstanceOutputPortIndex = operOPIndex;
  }
}
