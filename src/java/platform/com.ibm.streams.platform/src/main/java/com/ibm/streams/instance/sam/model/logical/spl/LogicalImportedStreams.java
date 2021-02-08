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

import static com.ibm.streams.spl.expressions.SPLMeta.SPLInt64;
import static com.ibm.streams.spl.expressions.SPLMeta.SPLValue;

import com.ibm.streams.instance.sam.exception.SAMInternalErrorException;
import com.ibm.streams.instance.sam.model.topology.ImportedStreams;
import com.ibm.streams.instance.sam.model.topology.NameBasedImport;
import com.ibm.streams.instance.sam.model.topology.PropertyBasedImport;
import com.ibm.streams.spl.expressions.SPLEvalException;
import com.ibm.streams.spl.expressions.SPLExpressionEval;
import com.ibm.streams.spl.expressions.SPLInvalidExpressionException;
import java.math.BigInteger;
import java.util.HashMap;
import java.util.Map;

public class LogicalImportedStreams extends ImportedStreams {
  public LogicalImportedStreams() {
    super();
  }

  public LogicalImportedStreams(LogicalImportedStreams other) {
    super(other);
  }

  private String tryEvaluate(String exp, Map<String, SPLValue> symbols)
      throws SAMInternalErrorException {
    if (exp.isEmpty()) return exp;

    String res = null;
    try {
      res = SPLExpressionEval.evaluate(exp, symbols);
    } catch (SPLInvalidExpressionException e) {
      throw SPLLogicalModel.traceAndReturnException(
          "SPL invalid expression exception: " + e.toString());
    } catch (SPLEvalException e) {
      throw SPLLogicalModel.traceAndReturnException(
          "SPL expression eval exception: " + e.toString());
    }
    return res;
  }

  public void setImportOperName(String operName) {
    for (Map.Entry<BigInteger, NameBasedImport> entry : getNameBasedImports().entrySet()) {
      NameBasedImport nbi = entry.getValue();
      nbi.setImportOperName(operName);
    }
    if (getPropertyBasedImport() != null) {
      getPropertyBasedImport().setImportOperName(operName);
    }
  }

  public void evaluate(
      BigInteger localChannelIndex,
      BigInteger localMaxChannels,
      BigInteger globalChannelIndex,
      BigInteger globalMaxChannels)
      throws SAMInternalErrorException {
    Map<String, SPLValue> intrinsics = new HashMap<String, SPLValue>();
    intrinsics.put("getLocalChannel", new SPLInt64(localChannelIndex.longValue()));
    intrinsics.put("getChannel", new SPLInt64(globalChannelIndex.longValue()));
    intrinsics.put("getLocalMaxChannels", new SPLInt64(localMaxChannels.longValue()));
    intrinsics.put("getMaxChannels", new SPLInt64(globalMaxChannels.longValue()));

    for (Map.Entry<BigInteger, NameBasedImport> entry : getNameBasedImports().entrySet()) {
      NameBasedImport nbi = entry.getValue();
      String streamName = SPLExpressionEval.intrinsicsSubstitution(nbi.getStreamName(), intrinsics);
      entry.setValue(
          new NameBasedImport(
              nbi.getApplicationName(),
              nbi.getImportOperName(),
              streamName,
              nbi.getApplicationScope()));
    }

    PropertyBasedImport pbi = getPropertyBasedImport();
    if (pbi != null) {
      pbi.setSubscription(tryEvaluate(pbi.getSubscription(), intrinsics));
    }

    if (getFilter() != null) {
      setFilter(tryEvaluate(getFilter(), intrinsics));
    }
  }
}
