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

import com.ibm.distillery.utils.DistilleryException;
import com.ibm.streams.admin.internal.api.StreamsException;
import com.ibm.streams.admin.internal.api.StreamsMessage;
import com.ibm.streams.admin.internal.api.trace.ConsoleLogger;
import com.ibm.streams.admin.internal.api.trace.LogManager;
import com.ibm.streams.admin.internal.api.trace.OutputStreamTraceLogger;
import com.ibm.streams.admin.internal.api.trace.StreamsLogger.StreamsLogLevel;
import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;
import com.ibm.streams.admin.internal.api.trace.TraceManager;
import com.ibm.streams.instance.sam.exception.SubmissionTimeErrorException;
import com.ibm.streams.instance.sam.model.Job;
import com.ibm.streams.instance.sam.model.Pe;
import com.ibm.streams.instance.sam.model.composer.ContainerSpecification;
import com.ibm.streams.instance.sam.model.fuser.FusedApplication;
import com.ibm.streams.instance.sam.model.logical.LogicalModel;
import com.ibm.streams.instance.sam.model.logical.LogicalModelFactory;
import com.ibm.streams.instance.sam.model.runtime.RuntimeJobComposites;
import com.ibm.streams.instance.sam.model.runtime.RuntimeJobConfig;
import com.ibm.streams.instance.sam.model.topology.TopologyApplication;
import com.ibm.streams.messages.MessageUtilities;
import com.ibm.streams.messages.StreamsRuntimeMessagesKey;
import com.ibm.streams.platform.services.AugmentedPEType;
import com.ibm.streams.sch.composer.ComposingInstructionFactory;
import com.ibm.streams.sch.composer.ComposingInstructions;
import com.ibm.streams.sch.composer.IEUComposer;
import java.math.BigInteger;
import java.util.List;
import java.util.Locale;
import java.util.Properties;
import java.util.SortedSet;
import java.util.TreeSet;

public class StandaloneOperations {

  public static String createStandalonePe(
      String adlString, Properties submitParms, StreamsLogLevel logLevel, Level traceLevel) {
    ConsoleLogger logger = new ConsoleLogger();
    logger.setLevel(logLevel);
    LogManager.setLogger(logger);
    OutputStreamTraceLogger tracer = new OutputStreamTraceLogger(System.err);
    TraceManager.setLevel(traceLevel);
    TraceManager.setTraceLogger(tracer);
    SAMStaticContext.init();
    String serializedXML = null;
    Locale locale = Locale.getDefault();
    try {
      ObjectTracker tracker = new ObjectTracker();
      SAMStaticContext.setObjectTracker(tracker);
      SAMStaticContext.setStandaloneMode(true, false);
      Properties cfgs = new Properties();
      try {
        LogicalModel logicalModel =
            LogicalModelFactory.createLogicalModel(adlString, submitParms, cfgs);
        if (logicalModel.getSubmissionTimeWarning() != null) {
          for (String extraValue : logicalModel.getSubmissionTimeWarning().getExtraValues()) {
            String msg =
                MessageUtilities.getMessageAndPrefix(
                    locale,
                    StreamsRuntimeMessagesKey.Key.SubmissionParameterExtraWarning,
                    extraValue);
            System.out.println(msg);
          }
        }
        TopologyApplication topologyModel = logicalModel.generateTopology(cfgs);
        ComposingInstructions compInst = ComposingInstructionFactory.createFuseToOne();
        IEUComposer composer = new IEUComposer(topologyModel, null);
        composer.buildComposerModel(true);
        SortedSet<ContainerSpecification> containers =
            new TreeSet<>(composer.composeContainerSpecs(compInst));
        FusedApplication fused = new FusedApplication(topologyModel, containers);
        Job job =
            new Job(
                fused,
                "",
                "",
                "default",
                new RuntimeJobConfig(adlString, null, null),
                new RuntimeJobComposites(logicalModel.toJSON()),
                false);
        Pe pe = job.getPe(BigInteger.ZERO);
        AugmentedPEType xml = pe.toXml();
        serializedXML = SAMClientHelper.serializeAugmentedPEType(xml);
      } catch (DistilleryException e) {
        throw new StreamsException(e);
      }
    } catch (SubmissionTimeErrorException sse) {
      for (String ambiguousValue : sse.getAmbiguousList()) {
        String msg =
            MessageUtilities.getMessageAndPrefix(
                locale, StreamsRuntimeMessagesKey.Key.SAMSubmissionParameterError, ambiguousValue);
        System.err.println(msg);
      }
      for (String extraValue : sse.getExtraValues()) {
        String msg =
            MessageUtilities.getMessageAndPrefix(
                locale, StreamsRuntimeMessagesKey.Key.SubmissionParameterExtraWarning, extraValue);
        System.out.println(msg);
      }
      for (String dupValue : sse.getDuplicatesList()) {
        String msg =
            MessageUtilities.getMessageAndPrefix(
                locale, StreamsRuntimeMessagesKey.Key.SubmissionParameterDuplicateError, dupValue);
        System.err.println(msg);
      }
      for (String missingValue : sse.getMissingList()) {
        String msg =
            MessageUtilities.getMessageAndPrefix(
                locale,
                StreamsRuntimeMessagesKey.Key.SubmissionParameterMissingError,
                missingValue);
        System.err.println(msg);
      }
    } catch (StreamsException e) {
      List<List<StreamsMessage>> listOfLists = e.getNestedMessages();
      if (listOfLists != null) {
        for (List<StreamsMessage> list : listOfLists) {
          for (StreamsMessage msg : list) {
            System.err.println(msg.getMessageText());
          }
        }
      }
    } finally {
      SAMStaticContext.shutdown();
    }
    return serializedXML;
  }
}
