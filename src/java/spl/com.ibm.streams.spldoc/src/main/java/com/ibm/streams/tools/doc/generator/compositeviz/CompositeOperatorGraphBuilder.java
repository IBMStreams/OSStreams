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

import com.ibm.xmlns.prod.streams.spl.source.code.CompositeBodyType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeDefinitionType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeHeadType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeInputsType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeOutputsType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositePortType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputStreamType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputsType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationType;
import java.util.ArrayList;
import org.eclipse.emf.common.util.EList;

public class CompositeOperatorGraphBuilder {
  private CompositeDefinitionType compOper_ = null;

  public CompositeOperatorGraphBuilder(CompositeDefinitionType compOper) {
    compOper_ = compOper;
  }

  public CompositeOperatorGraph build() {
    CompositeOperatorGraph graph = null;
    { // setup head
      CompositeHeadType head = compOper_.getCompositeHead();
      graph = new CompositeOperatorGraph(head.getName());
      ArrayList<String> compIPorts = new ArrayList<String>();
      CompositeInputsType inputs = head.getInputs();
      if (inputs != null) {
        for (CompositePortType iport : inputs.getIport()) compIPorts.add(iport.getName());
      }
      ArrayList<String> compOPorts = new ArrayList<String>();
      CompositeOutputsType outputs = head.getOutputs();
      if (outputs != null) {
        for (CompositePortType oport : outputs.getOport()) compOPorts.add(oport.getName());
      }
      graph.setPorts(compIPorts, compOPorts);
    }
    { // setup body
      CompositeBodyType body = compOper_.getCompositeBody();
      // A composite may not have a graph.  If so, just return
      if (body.getGraph() == null) {
        return null;
      }
      for (OperatorInvocationType oper : body.getGraph().getOperatorInvocation()) {
        OperatorInvocationHeadType head = oper.getOperatorInvocationHead();
        EList<OperatorInvocationOutputType> outputs = null;
        OperatorInvocationOutputsType outputsDef = head.getOutputs();
        if (outputsDef != null) outputs = head.getOutputs().getOutput();
        String name = head.getInvocationAlias();
        if (name == null && outputs != null && outputs.size() > 0)
          name = outputs.get(0).getStreamName();
        String kind = oper.getOperatorInvocationHead().getOperatorName();
        OperatorInvocation opInvoke = new OperatorInvocation(name, kind);
        ArrayList<ArrayList<String>> inNames = new ArrayList<ArrayList<String>>();
        if (head.getInputs() != null) {
          EList<OperatorInvocationInputType> inputs = head.getInputs().getInput();
          for (OperatorInvocationInputType iport : inputs) {
            ArrayList<String> ins = new ArrayList<String>();
            for (OperatorInvocationInputStreamType stream : iport.getIstream())
              ins.add(stream.getName());
            inNames.add(ins);
          }
        }
        opInvoke.addInputPorts(inNames);
        ArrayList<String> outNames = new ArrayList<String>();
        if (outputsDef != null) {
          for (OperatorInvocationOutputType oport : outputs) outNames.add(oport.getStreamName());
        }
        opInvoke.addOutputPorts(outNames);
        graph.addOperatorInvocation(opInvoke);
      }
    }
    graph.buildUpOperatorConnections();
    return graph;
  }
}
