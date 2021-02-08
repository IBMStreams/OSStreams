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

package com.ibm.streams.utils;

import java.util.concurrent.TimeUnit;
import java.util.function.Supplier;
import lombok.val;
import org.junit.Assert;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class Probe {

  private static final Logger LOGGER = LoggerFactory.getLogger(Probe.class);

  public static <T extends Comparable<T>> void watchUntil(
      long delayInSec, Supplier<T> fn, T value) {
    watchUntil(null, delayInSec, fn, value);
  }

  public static <T extends Comparable<T>> void watchUntil(
      String label, long delayInSec, Supplier<T> fn, T value) {
    try {
      for (int i = 0; i < delayInSec; i += 1) {
        val probe = fn.get();
        if (probe.compareTo(value) == 0) {
          return;
        }
        if (label == null) {
          LOGGER.debug("Actual: \"{}\", expected: \"{}\"", probe, value);
        } else {
          LOGGER.debug("{} - actual: \"{}\", expected: \"{}\"", label, probe, value);
        }
        TimeUnit.SECONDS.sleep(1);
      }
    } catch (InterruptedException ignored) {
    }
    Assert.fail("Probe " + label + " failed");
  }
}
