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

package com.ibm.streams.controller.crds.pes;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.crds.pes.instance.IProcessingElementCoordinator;
import lombok.var;
import org.microbean.kubernetes.controller.AbstractEvent;

public class MockProcessingElementCoordinator implements IProcessingElementCoordinator {

  @Override
  public void updatePeConnectivity(ProcessingElement pe, Connectivity connectivity) {
    pe.getSpec().setConnectivity(connectivity);
  }

  @Override
  public void incrementPeLaunchCount(ProcessingElement pe) {
    pe.getSpec().setLaunchCount(pe.getSpec().getLaunchCount() + 1);
  }

  @Override
  public void touchPe(Job job, ProcessingElement pe) {
    var generation = job.getSpec().getGenerationId();
    pe.getMetadata().getLabels().put(STREAMS_GENERATION_LABEL_KEY, generation.toString());
  }

  @Override
  public void updatePeContentIdAndIncrementLaunchCount(
      Job job, ProcessingElement pe, String contentId) {
    var generation = job.getSpec().getGenerationId();
    pe.getMetadata().getLabels().put(STREAMS_GENERATION_LABEL_KEY, generation.toString());
    pe.getSpec().setContentId(contentId);
    pe.getSpec().setLaunchCount(pe.getSpec().getLaunchCount() + 1);
  }

  public void onAddition(AbstractEvent<? extends ProcessingElement> event) {}

  public void onModification(AbstractEvent<? extends ProcessingElement> event) {}

  public void onDeletion(AbstractEvent<? extends ProcessingElement> event) {}
}
