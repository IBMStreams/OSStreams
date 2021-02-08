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

package com.ibm.streams.controller.crds.prs;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;

import com.ibm.streams.controller.crds.jobs.Job;
import java.math.BigInteger;
import org.microbean.kubernetes.controller.AbstractEvent;

public class MockParallelRegionCoordinator implements IParallelRegionCoordinator {

  @Override
  public void setParallelRegionWidth(ParallelRegion pr, BigInteger width) {
    pr.getSpec().setWidth(width);
  }

  @Override
  public void updateParallelRegion(Job job, ParallelRegion pr) {
    pr.getMetadata()
        .getLabels()
        .put(STREAMS_GENERATION_LABEL_KEY, job.getSpec().getGenerationId().toString());
  }

  @Override
  public void onAddition(AbstractEvent<? extends ParallelRegion> event) {}

  @Override
  public void onModification(AbstractEvent<? extends ParallelRegion> event) {}

  @Override
  public void onDeletion(AbstractEvent<? extends ParallelRegion> event) {}
}
