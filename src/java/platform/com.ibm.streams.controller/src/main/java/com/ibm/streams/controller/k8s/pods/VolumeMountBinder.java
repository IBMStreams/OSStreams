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
import io.fabric8.kubernetes.api.model.VolumeMount;
import io.fabric8.kubernetes.api.model.VolumeMountBuilder;
import java.util.ArrayList;
import java.util.List;

public abstract class VolumeMountBinder {
  protected final String name;
  protected List<VolumeMount> mounts;

  public VolumeMountBinder(String name) {
    this.name = name;
    this.mounts = new ArrayList<VolumeMount>();
  }

  public VolumeMountBinder addMount(String mountPath) {
    mounts.add(new VolumeMountBuilder().withName(name).withMountPath(mountPath).build());
    return this;
  }

  public VolumeMountBinder addMount(String mountPath, String subPath) {
    mounts.add(
        new VolumeMountBuilder()
            .withName(name)
            .withMountPath(mountPath)
            .withSubPath(subPath)
            .build());
    return this;
  }

  public List<VolumeMount> getMounts() {
    return mounts;
  }

  public abstract Volume build();
}
