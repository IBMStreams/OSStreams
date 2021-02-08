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

package com.ibm.streams.flow.internal;

import java.util.Collection;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class Connections {

  public final class Connection {
    private final int outputOperatorIndex;
    private final int outputPortIndex;
    private final int inputOperatorIndex;
    private final int inputPortIndex;

    Connection(
        int outputOperatorIndex, int outputPortIndex, int inputOperatorIndex, int inputPortIndex) {
      this.outputOperatorIndex = outputOperatorIndex;
      this.outputPortIndex = outputPortIndex;
      this.inputOperatorIndex = inputOperatorIndex;
      this.inputPortIndex = inputPortIndex;
    }

    public int getOutputOperatorIndex() {
      return outputOperatorIndex;
    }

    public int getInputOperatorIndex() {
      return inputOperatorIndex;
    }

    public int getInputPortIndex() {
      return inputPortIndex;
    }

    public int getOutputPortIndex() {
      return outputPortIndex;
    }
  }

  private final Set<Connection> connections =
      Collections.synchronizedSet(new HashSet<Connection>());

  public void addConnections(
      int outputOperatorIndex, final int outputPortIndex, List<String> values) {
    if (values.isEmpty()) return;

    for (int i = 0; i < values.size(); i += 2) {
      int inputOperatorIndex = Integer.valueOf(values.get(i));
      int inputPortIndex = Integer.valueOf(values.get(i + 1));

      Connection conn =
          new Connection(
              outputOperatorIndex, outputPortIndex,
              inputOperatorIndex, inputPortIndex);
      connections.add(conn);
    }
  }

  public Collection<Connection> allConnections() {
    return Collections.unmodifiableCollection(connections);
  }
}
