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
import static com.ibm.streams.instance.sam.model.ValidatorHelper.checkContentsNotNull;

import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.Validator;
import com.ibm.streams.instance.sam.persistence.IXmlSerializer;
import com.ibm.streams.platform.services.HostType;
import com.ibm.streams.platform.services.HostpoolType;
import com.ibm.streams.platform.services.MembershipModeType;
import com.ibm.streams.platform.services.TagType;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;

public class Hostpool implements Validator, IXmlSerializer<HostpoolType> {

  private List<String> hosts = new ArrayList<>();
  private Set<String> tags = new HashSet<>();
  private String name;
  private BigInteger index;
  private MembershipMode membershipMode;
  private BigInteger size; // May be null

  public Hostpool() {
    // empty constructor needed for JSON serialization
  }

  public Hostpool(
      String name,
      BigInteger index,
      MembershipMode membershipMode,
      List<String> hosts,
      Set<String> tags,
      BigInteger size) {
    this.name = name;
    this.index = index;
    this.membershipMode = membershipMode;
    this.hosts = hosts;
    this.tags = tags;
    this.size = size;
  }

  public Hostpool(Hostpool other) {
    this(
        other.name,
        other.index,
        other.membershipMode,
        new ArrayList<String>(),
        new LinkedHashSet<String>(),
        other.size);
    hosts.addAll(other.getHosts());
    tags.addAll(other.getTags());
  }

  public List<String> getHosts() {
    return hosts;
  }

  public void setHosts(List<String> hosts) {
    this.hosts = hosts;
    // this.size = BigInteger.valueOf(hosts.size()); // Howard: May be
    // correct in this case but ...
  }

  public Set<String> getTags() {
    return tags;
  }

  public void setTags(Set<String> tags) {
    this.tags = tags;
  }

  public String getName() {
    return name;
  }

  public void setName(String name) {
    this.name = name;
  }

  public BigInteger getIndex() {
    return index;
  }

  public void setIndex(BigInteger index) {
    this.index = index;
  }

  public MembershipMode getMembershipMode() {
    return membershipMode;
  }

  public void setMembershipMode(MembershipMode membershipMode) {
    this.membershipMode = membershipMode;
  }

  public BigInteger getSize() {
    return size;
  }

  public void setSize(BigInteger size) {
    this.size = size;
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.startLine(
        "<hostpool index=\""
            + index
            + "\" name=\""
            + name
            + "\" membershipMode=\""
            + membershipMode.toString()
            + "\"");
    if (size != null) {
      dumper.append(" size=\"" + size.toString() + "\"");
    }
    if (hosts.size() == 0 && tags.size() == 0) {
      dumper.append("/>").end();
    } else {
      dumper.append(">").end();
      if (hosts.size() > 0) {
        dumper.indent();
        dumper.line("<hosts>");
        dumper.indent();
        for (String host : hosts) {
          dumper.line("<host>" + host + "</host>");
        }
        dumper.outdent();
        dumper.line("<hosts>");
        dumper.outdent();
      }
      if (tags.size() > 0) {
        dumper.line("<tags>");
        dumper.indent();
        for (String tag : tags) {
          dumper.line("<tag>" + tag + "</tag>");
        }
        dumper.outdent();
        dumper.line("</tags>");
      }
      dumper.line("</hostpool>");
    }
    return dumper.toString();
  }

  public HostpoolType toXml() {
    HostpoolType hpt = new HostpoolType();
    for (String host : hosts) {
      HostType ht = new HostType();
      ht.setHostname(host);
      hpt.getHost().add(ht);
    }
    for (String tag : tags) {
      TagType tt = new TagType();
      tt.setName(tag);
      hpt.getTag().add(tt);
    }
    hpt.setIndex(index);
    hpt.setMembershipMode(MembershipModeType.fromValue(membershipMode.toString()));
    hpt.setName(name);
    if (size != null) { // Howard: I added this check as I think size needs
      // to be able to be null
      hpt.setSize(size);
    }
    return hpt;
  }

  public void validate() throws Exception {
    checkContentsNotNull(hosts);
    checkContentsNotNull(tags);
    checkNotNull(name);
    checkNotNull(index);
    // Size is optional
    // TODO: Validate size against one of the collections?
    // TODO: Talk to Howard about size again.
  }

  @Override
  public int hashCode() {
    final int prime = 31;
    int result = 1;
    result = prime * result + ((hosts == null) ? 0 : hosts.hashCode());
    result = prime * result + ((index == null) ? 0 : index.hashCode());
    result = prime * result + ((membershipMode == null) ? 0 : membershipMode.hashCode());
    result = prime * result + ((name == null) ? 0 : name.hashCode());
    result = prime * result + ((size == null) ? 0 : size.hashCode());
    result = prime * result + ((tags == null) ? 0 : tags.hashCode());
    return result;
  }

  @Override
  public boolean equals(Object obj) {
    if (this == obj) return true;
    if (obj == null) return false;
    if (getClass() != obj.getClass()) return false;
    Hostpool other = (Hostpool) obj;
    if (hosts == null) {
      if (other.hosts != null) return false;
    } else if (!hosts.equals(other.hosts)) return false;
    if (index == null) {
      if (other.index != null) return false;
    } else if (!index.equals(other.index)) return false;
    if (membershipMode != other.membershipMode) return false;
    if (name == null) {
      if (other.name != null) return false;
    } else if (!name.equals(other.name)) return false;
    if (size == null) {
      if (other.size != null) return false;
    } else if (!size.equals(other.size)) return false;
    if (tags == null) {
      if (other.tags != null) return false;
    } else if (!tags.equals(other.tags)) return false;
    return true;
  }
}
