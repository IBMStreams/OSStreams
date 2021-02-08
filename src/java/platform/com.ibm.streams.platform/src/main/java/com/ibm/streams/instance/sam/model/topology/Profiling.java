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

import com.ibm.streams.instance.sam.model.Validator;
import java.math.BigInteger;

public class Profiling implements Validator {

  private BigInteger bufferSize;
  private double samplingRate;

  public Profiling() {}

  public BigInteger getBufferSize() {
    return bufferSize;
  }

  public void setBufferSize(BigInteger bufferSize) {
    this.bufferSize = bufferSize;
  }

  public double getSamplingRate() {
    return samplingRate;
  }

  public void setSamplingRate(double samplingRate) {
    this.samplingRate = samplingRate;
  }

  public void validate() throws Exception {
    checkNotNull(samplingRate);
    checkNotNull(bufferSize);
  }
}
