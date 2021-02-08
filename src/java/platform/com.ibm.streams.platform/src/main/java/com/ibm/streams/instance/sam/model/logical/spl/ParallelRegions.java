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

package com.ibm.streams.instance.sam.model.logical.spl;

import com.ibm.streams.instance.sam.exception.SAMInternalErrorException;
import com.ibm.streams.instance.sam.model.ModelDumper;
import java.math.BigInteger;
import java.util.HashMap;
import java.util.Map;

public class ParallelRegions {

  private final transient Map<BigInteger, ParallelRegion> parallelRegionMap = new HashMap<>();
  private final transient Map<BigInteger, ParallelRegion> parallelOperatorMap = new HashMap<>();

  public Map<BigInteger, ParallelRegion> getParallelRegionMap() {
    return parallelRegionMap;
  }

  public Map<BigInteger, ParallelRegion> getParallelOperatorMap() {
    return parallelOperatorMap;
  }

  public boolean isEmpty() {
    return parallelRegionMap.isEmpty();
  }

  public boolean isParallelOperator(BigInteger operIndex) {
    if (!parallelOperatorMap.containsKey(operIndex)) {
      return false;
    }
    return true;
  }

  public ParallelRegion getParallelRegion(BigInteger operIndex) throws SAMInternalErrorException {
    if (!parallelOperatorMap.containsKey(operIndex)) {
      throw SPLLogicalModel.traceAndReturnException(
          "Internal error: parallelRegion called for non-parallel operator");
    }
    return parallelOperatorMap.get(operIndex);
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.line("<parallelRegions>");
    dumper.indent();
    dumper.append(parallelRegionMap);
    dumper.outdent();
    dumper.line("</parallelRegions>");
    return dumper.toString();
  }
}
