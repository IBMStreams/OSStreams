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

package com.ibm.streams.instance.sam.model.runtime;

public class JobHealth {

  private boolean isHealthy;
  private PeHealthSummary peHealth;

  public JobHealth() {}

  @Override
  public String toString() {
    StringBuilder builder = new StringBuilder();
    builder.append("isHealthy=" + isHealthy);
    builder.append(" peHealth=" + peHealth);
    return builder.toString();
  }

  public boolean isHealthy() {
    return isHealthy;
  }

  public void setHealthy(boolean isHealthy) {
    this.isHealthy = isHealthy;
  }

  public PeHealthSummary getPeHealth() {
    return peHealth;
  }

  public void setPeHealth(PeHealthSummary peHealth) {
    this.peHealth = peHealth;
  }

  @Override
  public int hashCode() {
    final int prime = 31;
    int result = 1;
    result = prime * result + (isHealthy ? 1231 : 1237);
    result = prime * result + ((peHealth == null) ? 0 : peHealth.hashCode());
    return result;
  }

  @Override
  public boolean equals(Object obj) {
    if (this == obj) return true;
    if (obj == null) return false;
    if (getClass() != obj.getClass()) return false;
    JobHealth other = (JobHealth) obj;
    if (isHealthy != other.isHealthy) return false;
    if (peHealth != other.peHealth) return false;
    return true;
  }
}
