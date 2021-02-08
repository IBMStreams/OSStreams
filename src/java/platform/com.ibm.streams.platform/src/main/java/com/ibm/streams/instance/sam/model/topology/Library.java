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

import static com.google.common.base.Preconditions.checkNotNull;

import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.Validator;
import com.ibm.streams.instance.sam.persistence.IXmlSerializer;
import com.ibm.streams.platform.services.LibraryType;
import java.math.BigInteger;

public class Library implements Validator, IXmlSerializer<LibraryType> {

  private String version;
  private String name;
  private BigInteger index;

  public Library() {}

  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.line(
        "library<index=\""
            + index.toString()
            + "\"name= \""
            + name
            + "\" version=\""
            + version
            + "\"/>");
    return dumper.toString();
  }

  public BigInteger getIndex() {
    return index;
  }

  public void setIndex(BigInteger index) {
    this.index = index;
  }

  public String getVersion() {
    return version;
  }

  public void setVersion(String version) {
    this.version = version;
  }

  public String getName() {
    return name;
  }

  public void setName(String name) {
    this.name = name;
  }

  public LibraryType toXml() {
    LibraryType lib = new LibraryType();
    lib.setIndex(getIndex());
    lib.setName(getName());
    lib.setVersion(getVersion());
    return lib;
  }

  public void validate() throws Exception {
    checkNotNull(name);
    checkNotNull(index);
    // Version is optional
  }
}
