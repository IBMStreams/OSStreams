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

package com.ibm.streams.operator.internal.runtime.api;

import com.ibm.streams.operator.ProcessingElement;
import com.ibm.streams.operator.internal.management.MXBeanControl;
import com.ibm.streams.operator.internal.management.NotifyInfo;
import com.ibm.streams.operator.management.OperatorManagement;
import com.ibm.streams.operator.management.ProcessingElementMXBean;
import java.net.UnknownHostException;
import java.util.Map;
import javax.management.MBeanNotificationInfo;
import javax.management.Notification;
import javax.management.ObjectName;

/**
 * Simply wrap the ProcessingElement, converting none MXBean types to MXBean (open type) types (e.g.
 * File to String).
 */
final class PEMXBeanImpl extends NotifyInfo implements ProcessingElementMXBean {

  private final ProcessingElement pe;

  static PEMXBeanImpl registerForPE(ProcessingElement pe) {
    // The ProcessingElementMXBean
    final ObjectName peName = OperatorManagement.getPEName();
    PEMXBeanImpl bean = new PEMXBeanImpl(pe);
    MXBeanControl.platformRegister(bean, peName, true);
    return bean;
  }

  private PEMXBeanImpl(ProcessingElement pe) {
    this.pe = pe;
  }

  @Override
  public MBeanNotificationInfo[] getNotificationInfo() {
    MBeanNotificationInfo trace =
        new MBeanNotificationInfo(
            new String[] {TRACE_LEVEL},
            Notification.class.getName(),
            "SPL Trace facility notifications");

    return new MBeanNotificationInfo[] {trace};
  }

  @Override
  public String getJobGroup() {
    return pe.getJobGroup();
  }

  @Override
  public String getJobId() {
    return pe.getJobId().toString();
  }

  @Override
  public String getJobName() {
    return pe.getJobName();
  }

  @Override
  public String getPEId() {
    return pe.getPEId().toString();
  }

  @Override
  public String getOutputDirectory() {
    return pe.getOutputDirectory().getAbsolutePath();
  }

  @Override
  public String getDataDirectory() {
    return pe.hasDataDirectory() ? pe.getDataDirectory().getAbsolutePath() : null;
  }

  @Override
  public String getApplicationDirectory() {
    return pe.getApplicationDirectory().getAbsolutePath();
  }

  @Override
  public String getApplicationName() {
    return pe.getApplicationName();
  }

  @Override
  public String getApplicationScope() {
    return pe.getApplicationScope();
  }

  @Override
  public boolean isStandalone() {
    return pe.isStandalone();
  }

  @Override
  public int getRelaunchCount() {
    return pe.getRelaunchCount();
  }

  @Override
  public String getInstanceId() {
    return pe.getInstanceId();
  }

  @Override
  public String getConfiguredHostName() throws UnknownHostException {
    return pe.getConfiguredHost().getCanonicalHostName();
  }

  @Override
  public String getDomainId() {
    return pe.getDomainId();
  }

  @Override
  public Map<String, String> getApplicationConfiguration(String name) {
    return pe.getApplicationConfiguration(name);
  }
}
