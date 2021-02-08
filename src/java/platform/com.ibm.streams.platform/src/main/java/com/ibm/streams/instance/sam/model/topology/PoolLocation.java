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
import com.ibm.streams.platform.services.PoolLocationType;
import java.math.BigInteger;
import org.apache.commons.lang.builder.EqualsBuilder;
import org.apache.commons.lang.builder.HashCodeBuilder;

public class PoolLocation implements Validator, IXmlSerializer<PoolLocationType> {

  private BigInteger inpoolIndex; // May be null
  private BigInteger poolIndex;

  @Override
  public boolean equals(Object obj) {
    if (obj == null) {
      return false;
    }
    if (obj == this) {
      return true;
    }
    if (obj.getClass() != getClass()) {
      return false;
    }
    PoolLocation rhs = (PoolLocation) obj;
    return new EqualsBuilder()
        .append(inpoolIndex, rhs.inpoolIndex)
        .append(poolIndex, rhs.poolIndex)
        .isEquals();
  }

  @Override
  public int hashCode() {
    return new HashCodeBuilder().append(inpoolIndex).append(poolIndex).toHashCode();
  }

  public PoolLocation() {}

  public PoolLocation(BigInteger poolIndex, BigInteger indexInPool) {
    this.poolIndex = poolIndex;
    this.inpoolIndex = indexInPool;
  }

  public PoolLocation(PoolLocation loc) {
    this.poolIndex = loc.poolIndex;
    this.inpoolIndex = loc.inpoolIndex;
  }

  public BigInteger getInpoolIndex() {
    return inpoolIndex;
  }

  public void setInpoolIndex(BigInteger inpoolIndex) {
    this.inpoolIndex = inpoolIndex;
  }

  public void setPoolIndex(BigInteger poolIndex) {
    this.poolIndex = poolIndex;
  }

  public BigInteger getPoolIndex() {
    return poolIndex;
  }

  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.startLine("<poolLocation poolIndex=\"" + poolIndex + "\"");
    if (inpoolIndex != null) {
      dumper.append(" inPoolIndex= \"" + inpoolIndex + "\"");
    }
    dumper.append("/>").end();
    return dumper.toString();
  }

  public PoolLocationType toXml() {
    PoolLocationType xml = new PoolLocationType();
    xml.setPoolIndex(getPoolIndex());
    xml.setInpoolIndex(getInpoolIndex());
    return xml;
  }

  public void validate() throws Exception {
    checkNotNull(poolIndex);
    // Inpool Index is optional
  }
}
