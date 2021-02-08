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

package com.ibm.streams.instance.sam.model;

import com.google.common.collect.ImmutableList;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;

public class HostPePlacement {
  private String _hostId;
  private BigInteger _peId;
  private String _buildId;
  private List<String> _requiredHostTags;

  public HostPePlacement(String buildId, BigInteger peId, String hostId) {
    _requiredHostTags = new ArrayList<>();
    _buildId = buildId;
    _peId = peId;
    _hostId = hostId;
  }

  public String getHostId() {
    return _hostId;
  }

  public BigInteger getPeId() {
    return _peId;
  }

  public String getBuildId() {
    return _buildId;
  }

  public ImmutableList<String> getRequiredHostTags() {
    return ImmutableList.copyOf(_requiredHostTags);
  }

  public void addRequiredHostTag(String tag) {
    _requiredHostTags.add(tag);
  }

  @Override
  public String toString() {
    StringBuilder builder = new StringBuilder();
    builder.append("hostId=" + _hostId);
    builder.append(" peId=" + _peId);
    builder.append(" buildId=" + _buildId);
    builder.append(" requiredHostTags=" + _requiredHostTags);
    return builder.toString();
  }
}
