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

import com.ibm.streams.instance.sam.model.runtime.RoutingNotificationType;
import java.math.BigInteger;
import java.util.Arrays;

public class RoutingNotification {
  private BigInteger _opId;
  private ConnectionId _connId;
  private RoutingNotificationType _action;
  private String _ipnslabel;
  private BigInteger _iportIndex;
  private String _filterExpression;
  private BigInteger _routingOrder;

  public RoutingNotification(RoutingNotificationType action, OutputPort op, InputPort ip) {
    throw new RuntimeException("called RoutingNotification constructor");
  }

  public BigInteger getOutputPortId() {
    return _opId;
  }

  public ConnectionId getConnId() {
    return _connId;
  }

  public RoutingNotificationType getAction() {
    return _action;
  }

  public String getIpNsLabel() {
    return _ipnslabel;
  }

  public BigInteger getIPortIndex() {
    return _iportIndex;
  }

  public String getFilterExpression() {
    return _filterExpression;
  }

  public BigInteger getRoutingOrder() {
    return _routingOrder;
  }

  public void setRoutingOrder(BigInteger routingOrder) {
    _routingOrder = routingOrder;
  }

  @Override
  public String toString() {
    return Arrays.deepToString(
        new Object[] {
          _opId, _connId, _action, _ipnslabel, _iportIndex, _filterExpression, _routingOrder
        });
  }
}
