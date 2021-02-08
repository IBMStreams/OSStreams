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
import com.ibm.streams.instance.sam.persistence.IXmlSerializer;
import com.ibm.streams.platform.services.DependenciesType;
import com.ibm.streams.platform.services.ExecutableType;
import com.ibm.streams.platform.services.LibraryDependencyType;
import java.math.BigInteger;
import java.util.List;

public class Executable implements IXmlSerializer<ExecutableType> {

  private String wrapper;
  private String executableUri;
  private String arguments;
  private List<BigInteger> dependencies;
  private String digest;
  private boolean dynamicLoad;

  public Executable(String wrapper, String executableUri) {
    this.wrapper = wrapper;
    this.executableUri = executableUri;
  }

  public String getWrapper() {
    return wrapper;
  }

  public void setWrapper(String wrapper) {
    this.wrapper = wrapper;
  }

  public String getExecutableUri() {
    return executableUri;
  }

  public void setExecutableUri(String executableUri) {
    this.executableUri = executableUri;
  }

  public String getArguments() {
    return arguments;
  }

  public void setArguments(String arguments) {
    this.arguments = arguments;
  }

  public List<BigInteger> getDependencies() {
    return dependencies;
  }

  public void setDependencies(List<BigInteger> dependencies) {
    this.dependencies = dependencies;
  }

  public String getDigest() {
    return digest;
  }

  public void setDigest(String digest) {
    this.digest = digest;
  }

  public boolean isDynamicLoad() {
    return dynamicLoad;
  }

  public void setDynamicLoad(boolean dynamicLoad) {
    this.dynamicLoad = dynamicLoad;
  }

  public ExecutableType toXml() {
    ExecutableType exec = new ExecutableType();
    exec.setWrapper(getWrapper());
    exec.setExecutableUri(getExecutableUri());
    exec.setArguments(getArguments());

    DependenciesType dt = new DependenciesType();
    for (BigInteger dependency : getDependencies()) {
      LibraryDependencyType ldt = new LibraryDependencyType();
      ldt.setLibIndex(dependency);
      dt.getDependency().add(ldt);
    }
    exec.setDependencies(dt);

    exec.setDigest(getDigest());
    exec.setDynamicLoad(isDynamicLoad());
    return exec;
  }

  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.line(
        "<Executable wrapper=\"" + wrapper + "\" executableUri=\"" + executableUri + "\"/>");
    return dumper.toString();
  }
}
