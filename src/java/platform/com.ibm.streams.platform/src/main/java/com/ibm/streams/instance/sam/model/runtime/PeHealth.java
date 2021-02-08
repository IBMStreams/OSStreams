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

public class PeHealth {

  private OutgoingConnectionHealthSummary requiredConnectionsSummary;
  private boolean isHealthy;
  private PeHealthSummary peHealth;
  private OutgoingConnectionHealthSummary optionalConnectionsSummary;

  public PeHealth() {}

  public OutgoingConnectionHealthSummary getRequiredConnectionsSummary() {
    return requiredConnectionsSummary;
  }

  public void setRequiredConnectionsSummary(
      OutgoingConnectionHealthSummary requiredConnectionsSummary) {
    this.requiredConnectionsSummary = requiredConnectionsSummary;
  }

  public OutgoingConnectionHealthSummary getOptionalConnectionsSummary() {
    return optionalConnectionsSummary;
  }

  public void setOptionalConnectionsSummary(
      OutgoingConnectionHealthSummary optionalConnectionsSummary) {
    this.optionalConnectionsSummary = optionalConnectionsSummary;
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
    result =
        prime * result
            + ((optionalConnectionsSummary == null) ? 0 : optionalConnectionsSummary.hashCode());
    result = prime * result + ((peHealth == null) ? 0 : peHealth.hashCode());
    result =
        prime * result
            + ((requiredConnectionsSummary == null) ? 0 : requiredConnectionsSummary.hashCode());
    return result;
  }

  @Override
  public boolean equals(Object obj) {
    if (this == obj) return true;
    if (obj == null) return false;
    if (getClass() != obj.getClass()) return false;
    PeHealth other = (PeHealth) obj;
    if (isHealthy != other.isHealthy) return false;
    if (optionalConnectionsSummary != other.optionalConnectionsSummary) return false;
    if (peHealth != other.peHealth) return false;
    if (requiredConnectionsSummary != other.requiredConnectionsSummary) return false;
    return true;
  }
}
