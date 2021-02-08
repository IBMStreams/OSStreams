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

package com.ibm.streams.controller.k8s.pods;

import io.fabric8.kubernetes.api.model.Volume;
import io.fabric8.kubernetes.api.model.VolumeBuilder;

public class PersistentVolumeClaimBinder extends VolumeMountBinder {
  protected final String claimName;
  protected final boolean readOnly;

  public PersistentVolumeClaimBinder(String name, String claimName) {
    super(name);
    this.claimName = claimName;
    this.readOnly = false;
  }

  @Override
  public Volume build() {
    return new VolumeBuilder()
        .withName(name)
        .withNewPersistentVolumeClaim(claimName, readOnly)
        .build();
  }
}
