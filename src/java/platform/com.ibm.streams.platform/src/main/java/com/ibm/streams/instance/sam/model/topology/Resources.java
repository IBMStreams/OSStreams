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

import static com.ibm.streams.instance.sam.model.ValidatorHelper.checkContentsNotNull;
import static com.ibm.streams.instance.sam.model.ValidatorHelper.validateIfPresent;

import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.Validator;
import com.ibm.streams.instance.sam.persistence.IXmlSerializer;
import com.ibm.streams.platform.services.CoLocationType;
import com.ibm.streams.platform.services.ExLocationType;
import com.ibm.streams.platform.services.HostLocationType;
import com.ibm.streams.platform.services.ResourcesType;
import java.math.BigInteger;
import java.util.Map;
import java.util.Objects;
import java.util.Set;

public class Resources implements Validator, IXmlSerializer<ResourcesType> {

  private String hostLocation; // May be null
  private PoolLocation poolLocation; // May be null
  private Set<String> coLocations;
  private Set<String> exLocations;
  private Boolean isolation;

  public Resources(
      String hostLocation,
      PoolLocation poolLocation,
      Set<String> resourceCoLocations,
      Set<String> resourceExLocations,
      Boolean isolation) {
    this.hostLocation = hostLocation;
    this.poolLocation = poolLocation;
    this.coLocations = resourceCoLocations;
    this.exLocations = resourceExLocations;
    this.isolation = isolation;
  }

  public Resources copy() {

    return null;
  }

  public Resources() {}

  public String getHostLocation() {
    return hostLocation;
  }

  public void setHostLocation(String hostLocation) {
    this.hostLocation = hostLocation;
  }

  public PoolLocation getPoolLocation() {
    return poolLocation;
  }

  public void setPoolLocation(PoolLocation poolLocation) {
    this.poolLocation = poolLocation;
  }

  public Set<String> getCoLocations() {
    return coLocations;
  }

  public void setCoLocations(Set<String> coLocations) {
    this.coLocations = coLocations;
  }

  public Set<String> getExLocations() {
    return exLocations;
  }

  public void setExLocations(Set<String> exLocations) {
    this.exLocations = exLocations;
  }

  public void setIsolation(boolean isolation) {
    this.isolation = Boolean.valueOf(isolation);
  }

  public boolean isIsolation() {
    return isolation.booleanValue();
  }

  public void transferOwnership(Map<BigInteger, BigInteger> hostpoolIndexFix) {
    if (poolLocation != null) {
      BigInteger poolIndex = poolLocation.getPoolIndex();
      if (hostpoolIndexFix.containsKey(poolIndex)) {
        poolLocation.setPoolIndex(hostpoolIndexFix.get(poolIndex));
      }
    }
  }

  public String toDotGraph() {
    StringBuilder str = new StringBuilder();
    if (hostLocation != null) str.append("H:" + hostLocation);
    if (poolLocation != null) {
      str.append(" P:" + poolLocation.getPoolIndex());
      if (poolLocation.getInpoolIndex() != null) str.append("," + poolLocation.getInpoolIndex());
      str.append(" ");
    }
    if (coLocations != null && coLocations.size() > 0) str.append(" C:" + coLocations);
    if (exLocations != null && exLocations.size() > 0) str.append(" X:" + exLocations);
    if (isolation != null && isolation) str.append("I");
    return str.toString();
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.startLine("<resources");
    if (isolation != null) {
      dumper.append(" isolation=\"" + isolation + "\"");
    }
    dumper.append(">").end();
    dumper.indent();
    if (poolLocation != null) {
      dumper.append(poolLocation.toString());
    }
    if (hostLocation != null) {
      dumper.line("<hostLocation>" + hostLocation + "</hostLocation>");
    }
    if (coLocations.size() > 0) {
      for (String coloc : coLocations) {
        dumper.line("<coLocation colocId=\"" + coloc + "\"/>");
      }
    }
    if (exLocations.size() > 0) {
      for (String exloc : exLocations) {
        dumper.line("<exLocation exlocId=\"" + exloc + "\"/>");
      }
    }
    dumper.outdent();
    dumper.line("</resources>");
    return dumper.toString();
  }

  public ResourcesType toXml() {
    ResourcesType xml = new ResourcesType();
    if (getHostLocation() != null && !(getHostLocation().isEmpty())) {
      HostLocationType hlt = new HostLocationType();
      hlt.setHostname(getHostLocation());
      xml.setHostLocation(hlt);
    }
    if (getPoolLocation() != null) xml.setPoolLocation(getPoolLocation().toXml());
    if (getCoLocations() != null && getCoLocations().size() > 0)
      for (String coloc : getCoLocations()) {
        CoLocationType clt = new CoLocationType();
        clt.setColocId(coloc);
        xml.getCoLocation().add(clt);
      }
    if (getExLocations() != null && getExLocations().size() > 0)
      for (String exloc : getExLocations()) {
        ExLocationType elt = new ExLocationType();
        elt.setExlocId(exloc);
        xml.getExLocation().add(elt);
      }
    if (isolation != null) {
      xml.setIsolation(isolation);
    }

    return xml;
  }

  public void validate() throws Exception {
    // Host Location is optional
    validateIfPresent(poolLocation);
    checkContentsNotNull(coLocations);
    checkContentsNotNull(exLocations);
    // Isolation is optional

  }

  // for change detection
  @Override
  public int hashCode() {
    return Objects.hash(hostLocation, poolLocation, coLocations, exLocations, isolation);
  }
}
