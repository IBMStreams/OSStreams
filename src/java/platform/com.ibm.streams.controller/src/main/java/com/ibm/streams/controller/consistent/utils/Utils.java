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

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;

import com.ibm.streams.controller.crds.crs.ConsistentRegion;
import com.ibm.streams.controller.crds.pes.Connectivity;
import com.ibm.streams.controller.crds.pes.ProcessingElement;
import com.ibm.streams.controller.crds.pes.ProcessingElementStore;
import com.ibm.streams.controller.instance.jobs.JobUtils;
import com.ibm.streams.controller.k8s.pods.PodStore;
import io.fabric8.kubernetes.api.model.Pod;
import java.math.BigInteger;
import java.util.Optional;
import java.util.Set;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class Utils {

  private static final Logger logger = LoggerFactory.getLogger(Utils.class);

  public static RegionInfo checkRegion(
      ConsistentRegion cr, ProcessingElementStore peStore, PodStore podStore, String jobId) {
    Set<BigInteger> pesInRegion = cr.getSpec().getPesInRegion();
    String jobName = cr.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);

    RegionInfo regionInfo = new RegionInfo();

    for (BigInteger p : pesInRegion) {
      String peName = JobUtils.getProcessingElementName(jobName, p.toString());
      Optional<ProcessingElement> pe = peStore.getPeWithNameAndJobId(peName, jobId);
      Integer launchCount;
      if (pe.isPresent()) {
        launchCount = pe.get().getSpec().getLaunchCount();
        if (!pe.get().getSpec().getConnectivity().equals(Connectivity.Full)) {
          regionInfo.setRegionHealthy(false);
        }

        if (launchCount > 1) {
          regionInfo.setCleanStart(false);
        }

        Optional<Pod> pod =
            podStore.getPodWithJobNameAndId(
                jobName, p, launchCount, pe.get().getMetadata().getUid());
        if (pod.isPresent()) {
          logger.info(
              "pe " + p + " phase " + pod.get().getStatus().getPhase() + " restart " + launchCount);
          if (!pod.get().getStatus().getPhase().equals("Running")) {
            regionInfo.setRegionHealthy(false);
          }
        } else {
          regionInfo.setHealthUnknown(true);
        }
      } else {
        regionInfo.setHealthUnknown(true);
      }
    }
    return regionInfo;
  }
}
