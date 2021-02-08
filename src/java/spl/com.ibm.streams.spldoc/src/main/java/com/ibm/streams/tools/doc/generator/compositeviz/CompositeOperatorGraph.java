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

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;

public class CompositeOperatorGraph {
  CompositeOperatorGraph(String compositeName) {
    compositeName_ = compositeName;
  }

  public void setPorts(List<String> inPortNames, List<String> outPortNames) {
    inPortNames_.addAll(inPortNames);
    outPortNames_.addAll(outPortNames);
    for (int i = 0; i < inPortNames_.size(); ++i)
      downstreamsInside_.add(new ArrayList<DownstreamConnection>());
    for (int i = 0; i < outPortNames_.size(); ++i)
      upstreamsInside_.add(new ArrayList<UpstreamConnection>());
  }

  public void addOperatorInvocation(OperatorInvocation op) {
    operators_.put(op.getName(), op);
  }

  public void buildUpOperatorConnections() {
    buildCompositePortConnections();
    buildOperatorInvocationConnections();
  }

  private void buildCompositePortConnections() {
    for (OperatorInvocation op : operators_.values()) {
      for (int iport = 0; iport < op.getNumberOfInputPorts(); ++iport) {
        for (String istream : op.getInputPortStreams(iport)) {
          for (int ciport = 0; ciport < inPortNames_.size(); ++ciport) {
            String ciStream = inPortNames_.get(ciport);
            if (ciStream.equals(istream)) {
              downstreamsInside_.get(ciport).add(new DownstreamConnection(op, iport));
              break;
            }
          }
        }
      }
      for (int oport = 0; oport < op.getNumberOfOutputPorts(); ++oport) {
        String ostream = op.getOutputPortStream(oport);
        for (int coport = 0; coport < outPortNames_.size(); ++coport) {
          String coStream = outPortNames_.get(coport);
          if (coStream.equals(ostream)) {
            upstreamsInside_.get(coport).add(new UpstreamConnection(op, oport));
            break;
          }
        }
      }
    }
  }

  private void buildOperatorInvocationConnections() {
    HashMap<String, UpstreamConnection> ups = new HashMap<String, UpstreamConnection>();
    for (OperatorInvocation op : operators_.values()) {
      for (int oport = 0; oport < op.getNumberOfOutputPorts(); ++oport) {
        String ostream = op.getOutputPortStream(oport);
        ups.put(ostream, new UpstreamConnection(op, oport));
      }
    }

    for (OperatorInvocation op : operators_.values()) {
      for (int iport = 0; iport < op.getNumberOfInputPorts(); ++iport) {
        for (String istream : op.getInputPortStreams(iport)) {
          if (ups.containsKey(istream)) {
            UpstreamConnection up = ups.get(istream);
            OperatorInvocation uop = up.getOperatorInvocation();
            int uoport = up.getOutputPort();
            op.addUpstreamConnection(iport, uop, uoport);
            uop.addDownstreamConnection(uoport, op, iport);
          }
        }
      }
    }
  }

  public String getCompositeName() {
    return compositeName_;
  }

  public int getNumberOfInputPorts() {
    return inPortNames_.size();
  }

  public int getNumberOfOutputPorts() {
    return outPortNames_.size();
  }

  public List<DownstreamConnection> getInputPortDownstreamOpsInside(int iport) {
    return downstreamsInside_.get(iport);
  }

  public List<UpstreamConnection> getOutputPortUpstreamOpsInside(int oport) {
    return upstreamsInside_.get(oport);
  }

  public OperatorInvocation getOperatorInvocation(String name) {
    return operators_.get(name);
  }

  public Collection<OperatorInvocation> getOperatorInvocations() {
    return operators_.values();
  }

  public ArrayList<String> getInputPortNames() {
    return inPortNames_;
  }

  public String getInputPortName(int iport) {
    return inPortNames_.get(iport);
  }

  public ArrayList<String> getOutputPortNames() {
    return outPortNames_;
  }

  public String getOutputPortName(int oport) {
    return outPortNames_.get(oport);
  }

  private String compositeName_;
  private ArrayList<String> inPortNames_ = new ArrayList<String>();
  private ArrayList<String> outPortNames_ = new ArrayList<String>();
  private ArrayList<ArrayList<DownstreamConnection>> downstreamsInside_ =
      new ArrayList<ArrayList<DownstreamConnection>>();
  private ArrayList<ArrayList<UpstreamConnection>> upstreamsInside_ =
      new ArrayList<ArrayList<UpstreamConnection>>();
  private LinkedHashMap<String, OperatorInvocation> operators_ =
      new LinkedHashMap<String, OperatorInvocation>();
}
