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

package com.ibm.streams.instance.sam.model.runtime;

import java.math.BigInteger;
import java.util.Objects;

public class RuntimeOperator {

  private final BigInteger _appId;
  private final BigInteger _index;
  private BigInteger _launchCount;

  public RuntimeOperator(BigInteger appId, BigInteger index) {
    _appId = appId;
    _index = index;
    _launchCount = BigInteger.ZERO;
  }

  public BigInteger getAppId() {
    return _appId;
  }

  public BigInteger getIndex() {
    return _index;
  }

  public BigInteger getLaunchCount() {
    return _launchCount;
  }

  public void setLaunchCount(BigInteger launchCount) {
    _launchCount = launchCount;
  }

  @Override
  public int hashCode() {
    return Objects.hash(_appId, _index, _launchCount);
  }
}
