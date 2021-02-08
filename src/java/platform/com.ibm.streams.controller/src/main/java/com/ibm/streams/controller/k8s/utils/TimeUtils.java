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

package com.ibm.streams.controller.k8s.utils;

import io.fabric8.kubernetes.api.model.HasMetadata;
import java.time.Instant;
import java.time.ZoneId;
import java.time.format.DateTimeFormatter;

public class TimeUtils {

  private static TimeUtils instance;
  private DateTimeFormatter formatter;

  private TimeUtils() {
    formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd'T'HH:mm:ss'Z'").withZone(ZoneId.of("UTC"));
  }

  public static synchronized TimeUtils getInstance() {
    if (instance == null) instance = new TimeUtils();
    return instance;
  }

  public Instant parse(String str) {
    return Instant.from(formatter.parse(str));
  }

  public String toString(Instant instant) {
    return formatter.format(instant);
  }

  public static boolean happenedBefore(HasMetadata meta, Instant ts) {
    /*
     * Check if the timestamp is valid.
     */
    if (meta.getMetadata().getCreationTimestamp() == null) {
      return false;
    }
    /*
     * Convert and compare the timestamp
     */
    Instant cTime = getInstance().parse(meta.getMetadata().getCreationTimestamp());
    return cTime.isBefore(ts);
  }

  public static int happenedBefore(String tsA, String tsB) {
    Instant cTimeA = getInstance().parse(tsA);
    Instant cTimeB = getInstance().parse(tsB);
    return cTimeA.compareTo(cTimeB);
  }
}
