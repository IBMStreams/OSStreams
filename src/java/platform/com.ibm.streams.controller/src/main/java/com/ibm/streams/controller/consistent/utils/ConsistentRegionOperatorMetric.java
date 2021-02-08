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

package com.ibm.streams.controller.consistent.utils;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_METRIC_PORT;

import io.prometheus.client.Gauge;
import io.prometheus.client.exporter.HTTPServer;
import io.prometheus.client.hotspot.DefaultExports;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import lombok.Getter;
import lombok.Setter;

public class ConsistentRegionOperatorMetric {
  @Getter @Setter private Map<EMetric, Gauge> metrics;
  @Getter private HTTPServer server;

  public ConsistentRegionOperatorMetric() {
    try {
      DefaultExports.initialize();
      server = new HTTPServer(STREAMS_METRIC_PORT);

      metrics = new HashMap<>();
      String label = "ConsistentRegion";
      metrics.put(
          EMetric.AVERAGE_DRAIN_TIME,
          Gauge.build()
              .name("average_drain_time_milliseconds")
              .help("average drain time for consistent region")
              .labelNames(label)
              .register());
      metrics.put(
          EMetric.AVERAGE_RESET_TIME,
          Gauge.build()
              .name("average_reset_time_milliseconds")
              .labelNames(label)
              .help("average reset time for consistent region")
              .register());
      metrics.put(
          EMetric.LAST_CONSISTENT_STATE_TIME,
          Gauge.build()
              .name("last_consistent_state_time_milliseconds")
              .labelNames(label)
              .help("last time reaching a consistent state")
              .register());
      metrics.put(
          EMetric.LAST_RESET_TIME,
          Gauge.build()
              .name("last_reset_time_milliseconds")
              .labelNames(label)
              .help("last time to reset a consistent region")
              .register());
      metrics.put(
          EMetric.LAST_COMPLETED_DRAIN_SEQ_ID,
          Gauge.build()
              .name("last_completed_drain_seq_id")
              .labelNames(label)
              .help("sequence id of the last completed drain")
              .register());
      metrics.put(
          EMetric.LAST_COMPLETED_RESET_SEQ_ID,
          Gauge.build()
              .name("last_completed_reset_seq_id")
              .labelNames(label)
              .help("sequence id of the last completed reset")
              .register());
      metrics.put(
          EMetric.STATE,
          Gauge.build()
              .name("state")
              .labelNames(label)
              .help("state of the consistent region")
              .register());
      metrics.put(
          EMetric.CURRENT_RESET_ATTEMPT,
          Gauge.build()
              .name("n_current_reset_attempts")
              .labelNames(label)
              .help("number of current reset attempts")
              .register());
    } catch (IOException e) {
      e.printStackTrace();
    }
  }

  public void updateMetric(Integer regionId, EMetric name, Long value) {
    if (metrics.containsKey(name)) {
      String label = "ConsistentRegion_" + regionId.toString();
      metrics.get(name).labels(label).set(value);
    }
  }
}
