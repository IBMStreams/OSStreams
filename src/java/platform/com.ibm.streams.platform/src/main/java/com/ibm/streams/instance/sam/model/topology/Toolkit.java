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

package com.ibm.streams.instance.sam.model.topology;

import com.ibm.streams.instance.sam.model.ModelDumper;
import java.math.BigInteger;

public class Toolkit {
  private final BigInteger index;
  private final String name;
  private final String uri;
  private final String version;

  public Toolkit(BigInteger index, String name, String uri, String version) {
    this.index = index;
    this.name = name;
    this.uri = uri;
    this.version = version;
  }

  public BigInteger getIndex() {
    return index;
  }

  public String getName() {
    return name;
  }

  public String getUri() {
    return uri;
  }

  public String getVersion() {
    return version;
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.line(
        "<toolkit index=\""
            + index
            + "\" name=\""
            + name
            + "\" uri=\""
            + uri
            + "\" version=\""
            + version
            + "\"/>");
    return dumper.toString();
  }
}
