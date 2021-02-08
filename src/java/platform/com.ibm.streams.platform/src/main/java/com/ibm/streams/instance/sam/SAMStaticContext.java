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

package com.ibm.streams.instance.sam;

import com.ibm.streams.admin.internal.api.StreamsException;
import com.ibm.streams.admin.internal.api.trace.LogManager;
import com.ibm.streams.messages.MessageKey;

public class SAMStaticContext {

  private static volatile boolean _shutdownRequested;
  private static volatile boolean _stopped;
  private static volatile boolean _restartPEOnResourceFailure;
  private static volatile int _restartPeRetryCountResetPeriod;
  private static volatile int _maxPERestartCount;
  private static volatile int _restartPesOnResourceFailureWaitTime;
  private static int _waitTimeOfRestartPEsAfterResourcesRestarted;
  private static boolean _isStandalone;
  private static String _testDataDir;
  private static boolean _errorTestMode;
  private static ObjectTracker _objectTracker;
  private static boolean _isRecoveryEnabled;
  private static volatile boolean _isInRecovery;
  private static volatile boolean _isRecovered;
  private static String _previousJobExecStreamsVersion;

  private static final String ASPECT_PREFIX = ":::";
  private static final String SAM_ASP = ASPECT_PREFIX + "SAM";
  private static final String[] SAM_ONLY_ASPECT = new String[] {SAM_ASP};
  private static final String[] STANDALONE_ASPECT = new String[] {"standalone"};

  public static void init() {
    _shutdownRequested = false;
    _stopped = false;
    _restartPEOnResourceFailure = false;
    _restartPeRetryCountResetPeriod = 300;
    _maxPERestartCount = 10;
    _restartPesOnResourceFailureWaitTime = 30;
    _waitTimeOfRestartPEsAfterResourcesRestarted = 30;
    _isStandalone = false;
    _testDataDir = "testSupport/sam/dataDir/";
    _errorTestMode = false;
    _objectTracker = null;
    _isRecoveryEnabled = true;
    _isInRecovery = false;
    _isRecovered = false;
    _previousJobExecStreamsVersion = null;
  }

  public static String getPreviousJobExecStreamsVersion() {
    return _previousJobExecStreamsVersion;
  }

  public static void setPreviousJobExecStreamsVersion(String previousJobExecStreamsVersion) {
    _previousJobExecStreamsVersion = previousJobExecStreamsVersion;
  }

  public static void setIsInRecovery(boolean isInRecovery) {
    _isInRecovery = isInRecovery;
  }

  public static boolean isInRecovery() {
    return _isInRecovery;
  }

  public static void setIsRecovered(boolean isRecovered) {
    _isRecovered = isRecovered;
  }

  public static boolean isRecovered() {
    return _isRecovered;
  }

  public static boolean isRecoveryEnabled() {
    return _isRecoveryEnabled;
  }

  public static void setStopped() {
    _stopped = true;
  }

  public static boolean isStopped() {
    return _stopped;
  }

  public static void setShutdownRequested() {
    _shutdownRequested = true;
  }

  public static boolean wasShutdownRequested() {
    return _shutdownRequested;
  }

  public static boolean getRestartPEOnResourceFailure() {
    return _restartPEOnResourceFailure;
  }

  public static void setRestartPEOnResourceFailure(Boolean restart) {
    _restartPEOnResourceFailure = restart;
  }

  public static boolean isSAMShutdown() {
    return (isStopped() || wasShutdownRequested());
  }

  public static int getWaitTimeOfRestartPEsAfterResourcesRestarted() {
    return _waitTimeOfRestartPEsAfterResourcesRestarted;
  }

  public static boolean isRoutingEnabled() {
    // todo
    return true;
  }

  public static String getViewServerLabel() {
    return "com.ibm.streams.viewserver";
  }

  public static void setStandaloneMode(boolean mode, boolean isRecoveryEnabled)
      throws StreamsException {
    _isStandalone = mode;
    _isRecoveryEnabled = isRecoveryEnabled;
  }

  public static void shutdown() {}

  // For unittest
  public static void setTestDataDir(String dataDir) {
    _testDataDir = dataDir;
  }

  public static String getTestDataDir() {
    return _testDataDir;
  }

  public static boolean isStandaloneMode() {
    return _isStandalone;
  }

  // For unittest with error condition
  public static void setErrorTestMode(boolean mode) {
    _errorTestMode = mode;
  }

  public static boolean isErrorTestMode() {
    return _errorTestMode;
  }

  public static ObjectTracker getObjectTracker() {
    return _objectTracker;
  }

  // for recovery
  public static void setObjectTracker(ObjectTracker objectTracker) {
    _objectTracker = objectTracker;
  }

  public static FusedTracker getFusedTracker() {
    return getObjectTracker();
  }

  public static TopologyTracker getTopologyTracker() {
    return getObjectTracker();
  }

  public static IDGeneratorFactory getIDGeneratorFactory() {
    return getObjectTracker();
  }

  public static void productLog(MessageKey msgKey, Object... replacements) {
    productLog(msgKey, STANDALONE_ASPECT, replacements);
  }

  public static void productLog(MessageKey msgKey, String[] aspects, Object... replacements) {
    if (msgKey != null) {
      String msgType = msgKey.getMessageId().substring(9, 10);
      if (msgType.equals("E")) {
        LogManager.getLogger().logError(msgKey, aspects, replacements);
      } else if (msgType.equals("I")) {
        LogManager.getLogger().logInfo(msgKey, aspects, replacements);
      } else if (msgType.equals("W")) {
        LogManager.getLogger().logWarning(msgKey, aspects, replacements);
      }
    }
  }

  public static int getRestartPeRetryCountResetPeriod() throws StreamsException {
    return _restartPeRetryCountResetPeriod;
  }

  public static void setRestartPeRetryCountResetPeriod(int period) throws StreamsException {
    _restartPeRetryCountResetPeriod = period;
  }

  public static int getMaximumPERestartCount() throws StreamsException {
    return _maxPERestartCount;
  }

  public static void setMaximumPERestartCount(int count) throws StreamsException {
    _maxPERestartCount = count;
  }

  public static int getRestartPesOnResourceFailureWaitTime() throws StreamsException {
    return _restartPesOnResourceFailureWaitTime;
  }

  public static void setRestartPesOnResourceFailureWaitTime(int period) throws StreamsException {
    _restartPesOnResourceFailureWaitTime = period;
  }

  public static boolean isSAMWatcherRegistrationManager() {
    // no notifications for standalone
    return false;
  }
}
