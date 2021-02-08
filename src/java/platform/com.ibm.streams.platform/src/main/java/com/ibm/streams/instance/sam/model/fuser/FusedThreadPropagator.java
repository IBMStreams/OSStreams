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

package com.ibm.streams.instance.sam.model.fuser;

import com.ibm.streams.instance.sam.SAMStaticContext;
import com.ibm.streams.instance.sam.model.topology.Annotation;
import com.ibm.streams.instance.sam.model.topology.AnnotationKeyValue;
import com.ibm.streams.instance.sam.model.topology.ParallelChannel;
import com.ibm.streams.instance.sam.model.topology.Splitter;
import com.ibm.streams.instance.sam.model.topology.TopologyNode;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeConnection;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeInputPort;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeOutputPort;
import java.math.BigInteger;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

public class FusedThreadPropagator {

  private FusedContainer fused;
  private Map<BigInteger, Set<BigInteger>> threadsPerOperator =
      new HashMap<>(); // operator index -> set of thread ids
  private Map<BigInteger, Set<BigInteger>> threadsPerPeOutputPort =
      new HashMap<>(); // pe output port index -> set of thread ids
  private Map<BigInteger, Map<BigInteger, Set<BigInteger>>> threadsPerThreadedPort =
      new HashMap<>(); // operator index -> (operator input port index -> set of thread ids)
  private Set<BigInteger> visited =
      new HashSet<>(); // keeps track of which operators we have visited

  public FusedThreadPropagator(FusedContainer fused) {
    this.fused = fused;
  }

  private void resetAndInitialize() {
    threadsPerOperator.clear();
    threadsPerPeOutputPort.clear();
    threadsPerThreadedPort.clear();

    // Initialize thread id groups by going over all operators (TopologyNodes) and their input
    // ports.
    for (BigInteger nodeIndex : fused.getNodeIndexes()) {
      threadsPerOperator.put(nodeIndex, new HashSet<BigInteger>());
      threadsPerThreadedPort.put(nodeIndex, new HashMap<BigInteger, Set<BigInteger>>());
      TopologyNode tn =
          SAMStaticContext.getTopologyTracker()
              .getTopologyNode(fused.getFusedApplicationId(), nodeIndex);
      for (BigInteger iportIndex : tn.getInputPorts().keySet()) {
        threadsPerThreadedPort.get(nodeIndex).put(iportIndex, new HashSet<BigInteger>());
      }
    }

    // Initialize thread id groups by going over all all PE output ports (FusedContainerOutputPort).
    for (FusedContainerOutputPort fcop : fused.getContainerOutputPorts().values()) {
      threadsPerPeOutputPort.put(fcop.getIndex(), new HashSet<BigInteger>());
    }
  }

  // This algorithm was adapted from SPL/Core/OperatorThreadPropagator.
  //
  // The basic idea is that we need to completley redo the single-threadedness calcuation on
  // all operators, PE output ports, and threaded ports. There are four sources of threads:
  //   1. PE input ports.
  //   2. Source operators.
  //   3. Threaded ports.
  //   4. Mutlithreaded operators.
  // This algorithm finds all threads in the PE, and propogates those threads down to each
  // port that the thread will see in execution. We record the threads we've seen for all operators,
  // PE output ports, and threaded ports. After propogating all threads to all ports, we look at the
  // threads we've seen, and set the single-threadedness for that port accordingly.
  public void propagateThreadsToPorts() {
    BigInteger tid =
        BigInteger.valueOf(-1); // first valid tid is 0; we always add one before using it

    resetAndInitialize();

    // For each PE input port, bump the thread count for the operator's downstream
    for (FusedContainerInputPort fcip : fused.getContainerInputPorts().values()) {
      tid = tid.add(BigInteger.ONE);
      TopologyNodeInputPort tnip = fcip.getTopologyNodeInputPort();
      if (tnip.getThreadedPort() == null) {
        bumpDownstreamThreadCount(tnip.getNodeIndex(), tid, false);
      } else {
        threadsPerThreadedPort.get(tnip.getNodeIndex()).get(tnip.getIndex()).add(tid);
      }
    }

    // For each operator
    for (BigInteger nodeIndex : fused.getNodeIndexes()) {
      TopologyNode tn =
          SAMStaticContext.getTopologyTracker()
              .getTopologyNode(fused.getFusedApplicationId(), nodeIndex);

      // Is this operator a source?
      if (tn.getInputPorts().size() == 0 && tn.getOutputPorts().size() > 0) {
        tid = tid.add(BigInteger.ONE);
        bumpDownstreamThreadCount(tn.getIndex(), tid, true);
      }

      // If the operator is multithreaded, it should be encoded in its single-threadedness on
      // outputs.
      if (!tn.getDeploymentDesc().isSingleThreadedOnOutputs()) {
        tid = tid.add(BigInteger.ONE);
        bumpDownstreamThreadCount(tn.getIndex(), tid, true);
      }

      // Check input ports for threaded ports.
      for (TopologyNodeInputPort tnip : tn.getInputPorts().values()) {
        if (tnip.getThreadedPort() == null) {
          if (possibleDynamicThreadingModel(tn)) {
            // If the threading model is not explicitly set to manual, potentially a seperate
            // ScheduleThread will work
            // on this input port.
            tid = tid.add(BigInteger.ONE);
            bumpDownstreamThreadCount(tn.getIndex(), tid, false);
          }
        } else {
          tid = tid.add(BigInteger.ONE);
          bumpDownstreamThreadCount(tn.getIndex(), tid, false);
        }
      }
    }

    // We have now accounted for all threads. Now we need to count the threads
    // we've seen, and change single-threadedness accordingly.

    // Mark the operator instances based on the thread count
    for (BigInteger nodeIndex : fused.getNodeIndexes()) {
      TopologyNode tn =
          SAMStaticContext.getTopologyTracker()
              .getTopologyNode(fused.getFusedApplicationId(), nodeIndex);
      boolean singleOnInput =
          (threadsPerOperator.get(nodeIndex).size() <= 1)
              && (!possibleDynamicThreadingModel(tn) || tn.getInputPorts().size() <= 1);
      tn.getDeploymentDesc().setSingleThreadedOnInputs(singleOnInput);
      tn.getDeploymentDesc()
          .setSingleThreadedOnOutputs(
              singleOnInput && tn.getDeploymentDesc().isSingleThreadedOnOutputs());
    }

    // Mark the pe output ports based on the thread count
    for (FusedContainerOutputPort fcop : fused.getContainerOutputPorts().values()) {
      boolean singleOnOutput = (threadsPerPeOutputPort.get(fcop.getIndex()).size() <= 1);
      fcop.getTopologyNodeOutputPort().setSingleThreadedOnOutput(singleOnOutput);
    }

    // Mark the threaded ports based on the thread count
    for (Map.Entry<BigInteger, Map<BigInteger, Set<BigInteger>>> i :
        threadsPerThreadedPort.entrySet()) {
      TopologyNode tn =
          SAMStaticContext.getTopologyTracker()
              .getTopologyNode(fused.getFusedApplicationId(), i.getKey());
      for (Map.Entry<BigInteger, Set<BigInteger>> j : i.getValue().entrySet()) {
        TopologyNodeInputPort tnip = tn.getInputPorts().get(j.getKey());
        if (tnip.getThreadedPort() != null) {
          boolean singleThreaded = (j.getValue().size() <= 1);
          tnip.getThreadedPort().setSingleThreadedOnInput(singleThreaded);
        }
      }
    }
  }

  private boolean possibleDynamicThreadingModel(TopologyNode tn) {
    for (Annotation annot : tn.getAnnotations()) {
      if (annot.getTag().equals("threading")) {
        for (AnnotationKeyValue kv : annot.getKeyValues()) {
          // If the threading model is not explicitly set to manual, potentially dynamic threading
          // will be used due to
          // the default auto threading model
          if (kv.getKey().equals("model") && kv.getValue().toString().equals("manual")) {
            return false;
          }
        }
      }
    }
    return true;
  }

  private boolean definiteDynamicThreadingModel(TopologyNode tn) {
    for (Annotation annot : tn.getAnnotations()) {
      if (annot.getTag().equals("threading")) {
        for (AnnotationKeyValue kv : annot.getKeyValues()) {
          // If the threading model is explicitly set to dynamic, scheduled port will be used for
          // sure
          if (kv.getKey().equals("model") && kv.getValue().toString().equals("dynamic")) {
            return true;
          }
        }
      }
    }
    return false;
  }

  private void bumpDownstreamThreadCount(BigInteger opIndex, BigInteger tid, boolean ownedThread) {
    visited.clear();
    recursivelyBumpDownstreamThreadCount(opIndex, tid, ownedThread);
  }

  private void recursivelyBumpDownstreamThreadCount(
      BigInteger opIndex, BigInteger tid, boolean ownedThread) {
    // Avoid infinite recursion due to cycles.
    if (visited.contains(opIndex)) {
      return;
    }

    // If this operator does not own the thread, then we're seeing it for the first time.
    if (!ownedThread) {
      visited.add(opIndex);
      threadsPerOperator.get(opIndex).add(tid);
    }

    // Walk all of this operator's output ports.
    TopologyNode tn =
        SAMStaticContext.getTopologyTracker()
            .getTopologyNode(fused.getFusedApplicationId(), opIndex);
    for (TopologyNodeOutputPort tnop : tn.getOutputPorts().values()) {
      // If this output port exports a stream, then we created a PE output port for it.
      boolean threadLeavesPeNormally = (tnop.getExport() != null);

      // Walk all of this output port's connections.
      for (TopologyNodeConnection tnc : tnop.getOperConnections()) {
        TopologyNodeInputPort tnip = tnc.getDestination().getInputPort();
        BigInteger dstOpIndex = tnc.getDestination().getOwningOperator().getIndex();
        BigInteger dstPortIndex = tnip.getIndex();

        // Is the destination operator inside this PE?
        if (fused.getNodeIndexes().contains(dstOpIndex)) {
          assert fused.getIntraContainerConnections().contains(tnc);
          if (tnip.getThreadedPort() == null) {
            TopologyNode dstTN =
                SAMStaticContext.getTopologyTracker()
                    .getTopologyNode(fused.getFusedApplicationId(), dstOpIndex);
            if (!definiteDynamicThreadingModel(dstTN))
              recursivelyBumpDownstreamThreadCount(dstOpIndex, tid, false);
          } else {
            threadsPerThreadedPort.get(dstOpIndex).get(dstPortIndex).add(tid);
          }
        } else {
          assert fused.getInterContainerConnections().contains(tnc);
          if (tnc.getSplitterIndex() != null && tnc.getChannelIndex() != null) {
            // If our connection is for a splitter, we have to handle this here. Normally, there is
            // one PE output port for an operator output port, even if it has multiple connections.
            // But for splitters, we create a new PE output port for each connection.
            Splitter splitter = tnop.getPortSplittersMap().get(tnc.getSplitterIndex());
            ParallelChannel channel = splitter.getParallelChannels().get(tnc.getChannelIndex());
            FusedContainerOutputPort fcop = fused.getFusedContainerOutputPort(channel);
            threadsPerPeOutputPort.get(fcop.getIndex()).add(tid);
          } else {
            threadLeavesPeNormally = true;
          }
        }
      }

      // We need to remember how many threads reach PE output ports
      if (threadLeavesPeNormally) {
        FusedContainerOutputPort fcop = fused.getFusedContainerOutputPort(tnop);
        threadsPerPeOutputPort.get(fcop.getIndex()).add(tid);
      }
    }
  }
}
