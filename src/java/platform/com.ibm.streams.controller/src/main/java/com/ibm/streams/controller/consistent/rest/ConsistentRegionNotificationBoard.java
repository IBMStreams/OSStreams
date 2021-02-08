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

package com.ibm.streams.controller.consistent.rest;

import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import java.math.BigInteger;
import java.util.concurrent.ConcurrentHashMap;

@JsonDeserialize
public class ConsistentRegionNotificationBoard
    extends ConcurrentHashMap<BigInteger, ConsistentRegionNotifications> {
  public void add(BigInteger peId, Integer regionId, ConsistentRegionNotification notification) {
    computeIfAbsent(peId, k -> new ConsistentRegionNotifications()).put(regionId, notification);
  }

  public void clear(BigInteger peId, Integer regionId) {
    if (containsKey(peId)) {
      if (get(peId).containsKey(regionId)) {
        get(peId).remove(regionId);
      }

      if (get(peId).size() == 0) {
        remove(peId);
      }
    }
  }
}
