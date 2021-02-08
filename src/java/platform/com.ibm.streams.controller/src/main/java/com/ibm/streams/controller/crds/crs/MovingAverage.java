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

package com.ibm.streams.controller.crds.crs;

import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import java.util.ArrayDeque;
import lombok.Getter;
import lombok.Setter;

@JsonDeserialize
public class MovingAverage {
  @Getter @Setter private ArrayDeque<Long> samples;
  @Getter @Setter private int maxSize;
  @Getter @Setter private long total;

  public MovingAverage() {}

  public MovingAverage(int maxSize) {
    this.maxSize = maxSize;
    this.samples = new ArrayDeque<>(maxSize);
    this.total = 0;
  }

  public MovingAverage(MovingAverage ma) {
    this.samples = new ArrayDeque<>(ma.samples);
    this.maxSize = ma.maxSize;
    this.total = ma.total;
  }

  public void addSample(long sample) {
    long removedSample = 0;
    if (samples.size() == maxSize) {
      removedSample = samples.peekLast();
      samples.removeLast();
    }
    samples.addFirst(sample);

    total = total - removedSample + sample;
  }

  public float calcAverage() {
    return total / samples.size();
  }
}
