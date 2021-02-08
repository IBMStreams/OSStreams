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
import com.ibm.streams.platform.services.PropertyBasedImportType;
import java.math.BigInteger;
import java.util.Objects;

public class PropertyBasedImport implements Validator {

  private String subscription;
  private String importOperName;
  private BigInteger index;
  private String applicationScope;

  public PropertyBasedImport() {
    applicationScope = "Default";
  }

  public PropertyBasedImport(
      String subscription, String importOperName, BigInteger index, String applicationScope) {
    this.subscription = subscription;
    this.importOperName = importOperName;
    this.index = index;
    this.applicationScope = applicationScope == null ? "Default" : applicationScope;
  }

  public PropertyBasedImport(PropertyBasedImport other) {
    this.subscription = other.subscription;
    this.importOperName = other.importOperName;
    this.index = other.index;
    this.applicationScope = other.applicationScope;
  }

  public String getApplicationScope() {
    return applicationScope;
  }

  public void setApplicationScope(String scope) {
    applicationScope = scope == null ? "Default" : scope;
  }

  public BigInteger getIndex() {
    return index;
  }

  public void setIndex(BigInteger index) {
    this.index = index;
  }

  public String getImportOperName() {
    return importOperName;
  }

  public void setImportOperName(String importOperName) {
    this.importOperName = importOperName;
  }

  public String getSubscription() {
    return subscription;
  }

  public void setSubscription(String subs) {
    subscription = subs;
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.startLine(
        "<propertyBasedImport index=\""
            + index
            + "\" importOperName=\""
            + importOperName
            + "\" subscription=\""
            + subscription
            + "\"");
    if (applicationScope != null) {
      dumper.append(" applicationScope=\"" + applicationScope + "\"");
    }
    dumper.append("/>").end();
    return dumper.toString();
  }

  public void validate() throws Exception {
    checkNotNull(subscription);
    checkNotNull(importOperName);
    checkNotNull(index);
    // ApplicationScope can be null

  }

  public PropertyBasedImportType toXml() {
    PropertyBasedImportType propertyBasedImportType = new PropertyBasedImportType();
    propertyBasedImportType.setApplicationScope(getApplicationScope());
    propertyBasedImportType.setImportOperName(getImportOperName());
    propertyBasedImportType.setIndex(getIndex());
    propertyBasedImportType.setSubscription(getSubscription());
    return propertyBasedImportType;
  }

  @Override
  public int hashCode() {
    return Objects.hash(subscription, importOperName, index, applicationScope);
  }

  @Override
  public boolean equals(Object obj) {
    if (obj instanceof PropertyBasedImport) {
      PropertyBasedImport t = (PropertyBasedImport) obj;
      if (getApplicationScope() == null && t.getApplicationScope() == null) {
        return (getIndex().equals(t.getIndex())
            && getImportOperName().equals(t.getImportOperName())
            && getSubscription().equals(t.getSubscription()));
      } else if (getApplicationScope() != null && t.getApplicationScope() != null) {
        return (getIndex().equals(t.getIndex())
            && getImportOperName().equals(t.getImportOperName())
            && getSubscription().equals(t.getSubscription())
            && getApplicationScope().equals(t.getApplicationScope()));
      }
    }
    return false;
  }
}
