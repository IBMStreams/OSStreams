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
import com.ibm.streams.platform.services.NameBasedImportType;
import java.math.BigInteger;
import java.util.Objects;

public class NameBasedImport implements Validator {

  private String applicationName;
  private String importOperName;
  private String streamName;
  private String applicationScope;

  public NameBasedImport() {
    applicationScope = "Default";
  }

  public NameBasedImport(
      String applicationName, String importOperName, String streamName, String applicationScope) {
    this.applicationName = applicationName;
    this.importOperName = importOperName;
    this.streamName = streamName;
    this.applicationScope = applicationScope == null ? "Default" : applicationScope;
  }

  public void setApplicationName(String applicationName) {
    this.applicationName = applicationName;
  }

  public String getApplicationName() {
    return applicationName;
  }

  public void setApplicationScope(String applicationScope) {
    this.applicationScope = applicationScope == null ? "Default" : applicationScope;
  }

  public String getApplicationScope() {
    return applicationScope;
  }

  public void setStreamName(String streamName) {
    this.streamName = streamName;
  }

  public String getStreamName() {
    return streamName;
  }

  public void setImportOperName(String importOperName) {
    this.importOperName = importOperName;
  }

  public String getImportOperName() {
    return importOperName;
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.startLine(
        "<nameBasedImport importOperName=\""
            + importOperName
            + "\" streamName=\""
            + streamName
            + "\" applicationName=\""
            + applicationName
            + "\"");
    if (applicationScope != null) {
      dumper.append(" applicationScope=\"" + applicationScope + "\"");
    }
    dumper.append("/>").end();
    return dumper.toString();
  }

  public void validate() throws Exception {
    checkNotNull(applicationName);
    checkNotNull(importOperName);
    checkNotNull(streamName);
    // Application Scope can be null
  }

  public NameBasedImportType toXml(BigInteger index) {
    NameBasedImportType nameBasedImportType = new NameBasedImportType();
    nameBasedImportType.setApplicationName(getApplicationName());
    nameBasedImportType.setApplicationScope(getApplicationScope());
    nameBasedImportType.setImportOperName(getImportOperName());
    nameBasedImportType.setIndex(index);
    nameBasedImportType.setStreamName(getStreamName());
    return nameBasedImportType;
  }

  @Override
  public int hashCode() {
    return Objects.hash(applicationName, importOperName, streamName, applicationScope);
  }

  @Override
  public boolean equals(Object obj) {
    if (obj instanceof NameBasedImport) {
      NameBasedImport t = (NameBasedImport) obj;
      if (getApplicationScope() == null && t.getApplicationScope() == null) {
        return (getApplicationName().equals(t.getApplicationName())
            && getStreamName().equals(t.getStreamName())
            && getImportOperName().equals(t.getImportOperName()));
      } else if (getApplicationScope() != null && t.getApplicationScope() != null) {
        return (getApplicationName().equals(t.getApplicationName())
            && getStreamName().equals(t.getStreamName())
            && getImportOperName().equals(t.getImportOperName())
            && getApplicationScope().equals(t.getApplicationScope()));
      }
    }
    return false;
  }
}
