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

import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.topology.SplitterKind;
import java.math.BigInteger;
import java.util.List;

public class ParallelRegionSplitter {

  private final BigInteger iPortIndex;
  private final SplitterKind kind;
  private final String portSelector; // May be null
  private final List<String> attributes;

  public ParallelRegionSplitter(
      BigInteger iPortIndex, SplitterKind kind, String portSelector, List<String> attributes) {
    this.iPortIndex = iPortIndex;
    this.kind = kind;
    this.portSelector = portSelector;
    this.attributes = attributes;
  }

  public BigInteger getiPortIndex() {
    return iPortIndex;
  }

  public SplitterKind getKind() {
    return kind;
  }

  public String getPortSelector() {
    return portSelector;
  }

  public List<String> getAttributes() {
    return attributes;
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.startLine(
        "<splitter iPortIndex=\"" + iPortIndex + "\" kind=\"" + kind.toString() + "\"");
    if (portSelector != null) {
      dumper.append(" portSelector=\"" + portSelector + "\">").end();
      dumper.indent();
      for (String attr : attributes) {
        dumper.line("<attribute>" + attr + "</attribute>");
      }
      dumper.outdent();
    } else {
      dumper.append(">").end();
    }

    dumper.line("</splitter>");
    return dumper.toString();
  }
}
