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

import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.internal.ports.actions.TypedAction;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Set;

/**
 * When java threaded input ports are used, it some cases it is necessary to switch between the java
 * threaded port and a C++ threaded port. This class supports making that switch.
 *
 * <p>One or two alternate handlers are associated with this handler. If there is only one
 * alternate, tuples, punctuations, and actions are simply accepted and passed to the single
 * alternate.
 *
 * <p>If there are two alternates, one alternate should be the handler that receives tuples from the
 * java threaded port, and the other alternate should be the one that receives tuples from the c++
 * threaded port. Only one of these alternates is active at any time. On the active alternate, any
 * tuple, punctuation, or action received is immediately passed to the inner handler. On the other
 * alternate, input is blocked, until the alternates are switched.
 *
 * <p>To switch active alternates, simply send a {@link TypedAction#SwitchHandlers} action on the
 * stream of the currently active alternate. When the action is received from all the registered
 * output handlers, this handler will start processing input from the currently inactive input, and
 * start blocking the currently active input.
 *
 * @param <T> Tuple type
 */
public class SwitchingHandler<T> {

  private int activeAlternate = 0;
  private ArrayList<Alternate> alternates = new ArrayList<Alternate>(2);
  private boolean switching = false;

  private static class PortIdentifier {
    @Override
    public int hashCode() {
      final int prime = 31;
      int result = 1;
      result = prime * result + operatorIndex;
      result = prime * result + portIndex;
      return result;
    }

    @Override
    public boolean equals(Object obj) {
      if (this == obj) return true;
      if (obj == null) return false;
      if (getClass() != obj.getClass()) return false;
      PortIdentifier other = (PortIdentifier) obj;
      if (operatorIndex != other.operatorIndex) return false;
      if (portIndex != other.portIndex) return false;
      return true;
    }

    public PortIdentifier(int operatorIndex, int portIndex) {
      this.operatorIndex = operatorIndex;
      this.portIndex = portIndex;
    }

    @Override
    public String toString() {
      return "Operator " + operatorIndex + ", port " + portIndex;
    }

    int operatorIndex;
    int portIndex;
  }

  private Set<PortIdentifier> outputPorts = new HashSet<PortIdentifier>();
  private int switchCount = 0;

  public String opName;
  public int portNo;

  public SwitchingHandler(String opName, int portNo) {
    this.opName = opName;
    this.portNo = portNo;
  }

  @Override
  public String toString() {
    return "SwitchingHandler " + opName + ":" + portNo;
  }

  // TODO consider failing if there are already
  // two alternates.
  public RuntimeStream<T> addAlternate(RuntimeStream<T> handler) {
    Alternate alternate = new Alternate(handler);
    alternates.add(alternate);
    alternate.index = alternates.size() - 1;
    if (alternates.size() == 1) {
      alternates.get(0).activate();
    }
    return alternate;
  }

  // This assumes one or two alternates, which is not enforced elsewhere.
  private synchronized void switchAlternates() {
    if (switching && alternates.size() > 1) {
      alternates.get(activeAlternate).deactivate();
      activeAlternate = (activeAlternate + 1) % alternates.size();
      alternates.get(activeAlternate).activate();
      notifyAll();
    }
  }

  class Alternate implements RuntimeStream<T> {
    public int index = 0;
    private RuntimeStream<T> inner;
    private volatile boolean active = false;

    public Alternate(RuntimeStream<T> destination) {
      this.inner = destination;
    }

    void activate() {
      active = true;
    }

    public void deactivate() {
      active = false;
    }

    @Override
    public void tuple(T tuple) throws Exception {
      waitUntilActive();
      inner.tuple(tuple);
    }

    @Override
    public void mark(Punctuation mark) throws Exception {
      waitUntilActive();
      inner.mark(mark);
    }

    @Override
    public <A> A action(StreamAction<A> action) throws Exception {
      // Actions are always synchronous, so they should only be handled by the
      // active handler.  Inactive handlers should discard any actions.
      if (switching && !active) {
        return null;
      }

      if (action.equals(TypedAction.SwitchHandlers)) {
        if (++switchCount >= outputPorts.size()) {
          SwitchingHandler.this.switchAlternates();
        }
        return null;
      }

      A result = inner.action(action);
      return result;
    }

    private void waitUntilActive() throws InterruptedException {
      if (switching && !active) {
        synchronized (SwitchingHandler.this) {
          while (!active) {
            SwitchingHandler.this.wait();
          }
        }
      }
    }

    @Override
    public String toString() {
      return SwitchingHandler.this.toString() + " alternate " + index;
    }
  }

  /**
   * If a handler is an instance of this class, find and return the inner handler. Otherwise, just
   * return the handler.
   *
   * @param outerHandler Possible instance of this class
   * @return the inner handler, or the unchanged outer handler
   */
  public RuntimeStream<T> undecorate(RuntimeStream<T> outerHandler) {
    RuntimeStream<T> undecorated = outerHandler;
    if (outerHandler instanceof SwitchingHandler.Alternate) {
      undecorated = ((SwitchingHandler<T>.Alternate) outerHandler).inner;
    }
    return undecorated;
  }

  /**
   * Register an output handler whose output feeds this handler. When a switch is required, we do
   * not switch until the switch marker is received from all output handlers.
   *
   * @param operatorIndex the output handler's operator index
   * @param outputPortIndex the output handler's port index.
   */
  public synchronized void addHandlerOutput(int operatorIndex, int outputPortIndex) {
    outputPorts.add(new PortIdentifier(operatorIndex, outputPortIndex));
  }

  /** Enable the switching mode of this operator. */
  public synchronized void enableSwitch() {
    switching = true;
  }

  public synchronized int getActiveAlternate() {
    return this.activeAlternate;
  }

  public synchronized void setAlternate(int alternateIndex) {
    this.activeAlternate = alternateIndex;
    alternates.get(alternateIndex).activate();
  }
}
