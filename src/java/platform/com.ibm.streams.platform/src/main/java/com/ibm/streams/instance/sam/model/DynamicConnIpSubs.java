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

import com.ibm.streams.admin.internal.api.StreamsException;
import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;
import com.ibm.streams.instance.sam.SAMStaticContext;
import com.ibm.streams.instance.sam.SamUtilFunctions;
import com.ibm.streams.messages.StreamsRuntimeMessagesKey;
import com.ibm.streams.spl.expressions.SPLEvalException;
import com.ibm.streams.spl.expressions.SPLExpressionEval;
import com.ibm.streams.spl.expressions.SPLInvalidExpressionException;
import com.ibm.streams.spl.expressions.SPLMeta.SPLValue;
import java.math.BigInteger;
import java.util.Map;

public class DynamicConnIpSubs {

  private BigInteger _iportId;
  private String _scope;
  private String _subscription;

  public DynamicConnIpSubs(BigInteger ipId, String scope, String exp) {
    _iportId = ipId;
    _scope = scope;
    _subscription = exp;
  }

  public BigInteger getIportId() {
    return _iportId;
  }

  public String getScope() {
    return _scope;
  }

  public String getSubscription() {
    return _subscription;
  }

  public void updateSubscription(String scope, String exp) {
    _scope = scope;
    _subscription = exp;
  }

  public boolean doesMatch(DynamicConnOpProps prop, boolean checkScope) throws StreamsException {
    if ((checkScope && !_scope.equals(prop.getScope())) || _subscription.isEmpty()) {
      return false;
    }
    String res = "";
    try {
      Map<String, SPLValue> splVals =
          SamUtilFunctions.getSPLValueMapFromStreamProperties(prop.getProperties().values());
      if (Trace.isEnabled(Level.DEBUG)) {
        Trace.logDebug("Evaluating subscription expression: " + _subscription);
      }
      res = SPLExpressionEval.evaluate(_subscription, splVals);
      if (Trace.isEnabled(Level.DEBUG)) {
        Trace.logDebug("Evaluation yielded: " + res);
      }
    } catch (SPLInvalidExpressionException e) {
      // This IS an error. The expression provided on the Import side could not be parsed,
      // which means that the Import side can never match with any Export side. That is
      // likely not the intended behavior, so log it as an error.
      Trace.logError(e.getMessage(), e);
      SAMStaticContext.productLog(
          StreamsRuntimeMessagesKey.Key.SAMInvalidSubscriptionExpression,
          _iportId.toString(),
          _subscription);
      return false;
    } catch (SPLEvalException e) {
      // Note that this is not an error - this means that the values provided by the
      // Export side are not valid with the expression provided by the Import side. But
      // that is allowable, expected behavior. It is still possible for another Export
      // to come along with values that do work with the Import. See defect 30888 for
      // discussion.
      return false;
    }

    return res.equalsIgnoreCase("true");
  }

  public String toString() {
    StringBuilder builder = new StringBuilder();
    builder.append("Input Port [").append(_iportId);
    builder.append("] Scope [").append(_scope);
    builder.append("] Subscription: ").append(_subscription);
    return builder.toString();
  }
}
