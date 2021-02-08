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

package com.ibm.streams.instance.sam.model.logical.spl;

import com.ibm.streams.instance.sam.exception.SAMInternalErrorException;
import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.topology.ImportedStreams;
import com.ibm.streams.instance.sam.model.topology.NameBasedImport;
import com.ibm.streams.instance.sam.model.topology.TopologyNode;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeInputPort;
import java.math.BigInteger;
import java.util.List;
import java.util.Map;

public class PrimitiveImportOutputPort extends PrimitivePort {

  public PrimitiveImportOutputPort(
      BigInteger index, ImportOperator owningImportOp, SPLLogicalModel model) {
    super(index, owningImportOp, model);
  }

  public PrimitiveImportOutputPort(
      PrimitiveImportOutputPort other, ImportOperator owningPrimitiveOp) {
    super(other, owningPrimitiveOp);
  }

  public void populateConnections(
      LogicalImportedStreams importedStreams,
      Map<BigInteger, TopologyNode> nodeMap,
      SPLLogicalModel model)
      throws SAMInternalErrorException {
    for (Connection conn : getConnections()) {
      List<TopologyNodeInputPort> iPorts = conn.getDestinationInputPort(nodeMap, model);
      for (TopologyNodeInputPort iPort : iPorts) {
        ImportedStreams is = iPort.getImports();
        is.setFilter(importedStreams.getFilter());
        if (importedStreams.getPropertyBasedImport() != null) {
          is.setPropertyBasedImport(importedStreams.getPropertyBasedImport());
        } else {
          Map<BigInteger, NameBasedImport> nbi = importedStreams.getNameBasedImports();
          if (!nbi.isEmpty()) {
            assert (nbi.size() == 1);
            Map<BigInteger, NameBasedImport> tnbi = is.getNameBasedImports();
            for (Map.Entry<BigInteger, NameBasedImport> entry : nbi.entrySet()) {
              tnbi.put(BigInteger.valueOf(tnbi.size()), entry.getValue());
            }
          }
        }
      }
    }
  }

  public void addReverseConnections(BigInteger channelIndex, SPLLogicalModel model)
      throws SAMInternalErrorException {
    for (Connection conn : getConnections()) {
      Connection mc = new Connection(getOwningOperator().getIndex(), getIndex(), PortKind.Output);
      conn.addConnection(mc, model);
    }
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.line("<outputPort>");
    dumper.indent();
    dumper.append(super.toString());
    dumper.outdent();
    dumper.line("</outputPort>");
    return dumper.toString();
  }
}
