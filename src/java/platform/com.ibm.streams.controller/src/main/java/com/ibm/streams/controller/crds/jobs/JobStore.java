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

package com.ibm.streams.controller.crds.jobs;

import java.math.BigInteger;
import java.util.Optional;
import java.util.concurrent.ConcurrentHashMap;

public class JobStore extends ConcurrentHashMap<Object, Job> {

  public void put(Job job) {
    put(job.getMetadata().getName(), job);
  }

  public boolean hasJobWithName(String name) {
    return values().stream().anyMatch(j -> j.getMetadata().getName().equals(name));
  }

  public Optional<Job> getJobWithName(String name) {
    return values().stream().filter(j -> j.getMetadata().getName().equals(name)).findAny();
  }

  public Optional<Job> getJobWithNameAndGenerationId(String name, String generationId) {
    return getJobWithNameAndGenerationId(name, new BigInteger(generationId));
  }

  public Optional<Job> getJobWithNameAndGenerationId(String name, BigInteger generationId) {
    return values().stream()
        .filter(j -> j.getMetadata().getName().equals(name))
        .filter(j -> j.getSpec().getGenerationId() != null)
        .filter(j -> j.getSpec().getGenerationId().equals(generationId))
        .findAny();
  }

  public Optional<Job> getJobWithNameAndUid(String name, String uid) {
    return values().stream()
        .filter(j -> j.getMetadata().getName().equals(name))
        .filter(j -> j.getMetadata().getUid().equals(uid))
        .findAny();
  }

  public Optional<Job> getJobWithId(BigInteger id) {
    return values().stream().filter(j -> j.getSpec().getId().equals(id)).findAny();
  }
}
