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

package com.ibm.streams.sch.composer;

import com.google.common.collect.Multiset;
import com.ibm.streams.instance.sam.model.topology.PoolLocation;
import java.math.BigInteger;
import java.util.List;
import java.util.Map;
import java.util.Set;

public interface FusingConstraintCriteria {

  //	public Set<String> 							getCompositeHostLocations();
  //
  public Set<PoolLocation> getCompositePoolLocations();

  public Set<HostSpec> getCompositeHostSpecs();

  public Set<HostSpec> getCompositeResolvedHostSpecs();

  public Map<String, Set<String>> getCompositeHostCoLocationGroupIds();

  public Map<String, Set<String>> getCompositeHostExLocationGroupIds();

  public Set<String> getCompositePartitionCoLocationGroupIds();

  public List<String> getCompositePartitionExLocationGroupIds();

  public List<Boolean> getCompositePartitionIsolations();

  public Map<String, Set<Set<String>>> getCompositeProvisionalLocations();

  public Map<String, Set<Set<String>>> getCompositeSoftProvisionalLocations();

  public Map<String, Multiset<Set<String>>> getCompositeSegregatedLocations();

  public Map<String, Multiset<Set<String>>> getCompositeSoftSegregatedLocations();

  public Multiset<BigInteger> getCompositeExclusiveHostPoolIndexes();

  public Set<Boolean> getCompositeRestartable();

  public Set<Boolean> getCompositeRelocatable();

  public Set<String> getCompositeNamesOfContributingNodes();

  public Set<ComposerCriteriaNode> getCompositeCcNodes();

  public String getId();

  public long getCpuUsage();

  public IEUComposer getComposer();;
}
