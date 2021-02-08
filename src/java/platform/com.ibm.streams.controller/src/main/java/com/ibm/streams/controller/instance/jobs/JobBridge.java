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

package com.ibm.streams.controller.instance.jobs;

import com.ibm.streams.admin.internal.api.StreamsException;
import com.ibm.streams.admin.internal.api.trace.StreamsLogger.StreamsLogLevel;
import com.ibm.streams.admin.internal.api.trace.TraceLogger;
import com.ibm.streams.controller.crds.jobs.FusionSpec;
import com.ibm.streams.controller.crds.jobs.JobSpec;
import com.ibm.streams.controller.instance.sam.Pipeline;
import com.ibm.streams.controller.utils.CommonEnvironment;
import com.ibm.streams.instance.sam.model.Job;
import com.ibm.streams.instance.sam.model.logical.LogicalModel;
import com.ibm.streams.sch.composer.ComposingInstructions;
import java.math.BigInteger;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import lombok.Getter;
import lombok.var;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/*
 * This class is a bridge between the CRD view of jobs and the
 * SAM view of jobs for job related operations that classes
 * other than just the JobController need to do.
 */
public class JobBridge {

  private static final Logger LOGGER = LoggerFactory.getLogger(JobBridge.class);
  /*
   * We synchronize access to the underlying SAM pipeline. It is stateful, and we have
   * to ensure that our interactions with it are atomic: when we create a job through
   * the pipeline, we must ensure that we get all the information out of the pipeline
   * that we need before leaving the synchornized method. Subsequent public calls to
   * synchronized methods will lose that state.
   */
  private final Pipeline sam;
  private final CommonEnvironment env;

  private static class ModelWrapper {
    @Getter private final Job job;
    @Getter private final LogicalModel logical;

    ModelWrapper(Job job, LogicalModel logical) {
      this.job = job;
      this.logical = logical;
    }
  }

  JobBridge(CommonEnvironment env) {
    this.sam = new Pipeline(StreamsLogLevel.WARN, TraceLogger.Level.WARN);
    this.env = env;
  }

  private ComposingInstructions resolveFusion(FusionSpec fusion) {
    if (fusion.legacy().orElse(false)) {
      return Pipeline.createLegacyFuse();
    }
    /*
     * We currently implement "automatic" through manual fusion; if we want "automatic",
     * we ask the environment how many PEs we should use and manually fuse that many.
     */
    int numPes = fusion.manual().orElse(env.getDefaultNumberOfPEs());
    if (numPes < 1) {
      throw new IllegalArgumentException("requested number of PEs is invalid: " + numPes);
    }
    return Pipeline.createManualFuse(numPes);
  }

  private synchronized ModelWrapper createModels(
      String name,
      String namespace,
      JobSpec spec,
      BigInteger jobId,
      Map<String, BigInteger> parWidths,
      boolean resetObjectTracker)
      throws StreamsException {
    /*
     * Grab the bundle.
     */
    var bundleLoader = env.getBundleLoader();
    var bundle = bundleLoader.load(spec.getBundle(), namespace);
    /*
     * Get the ADL from the SAB.
     */
    var adl = bundle.flatMap(env::getADL);
    if (!adl.isPresent()) {
      throw new StreamsException("Cannot get the ADL for SAB: " + spec.getBundle().getName());
    }
    /*
     * Create the logical model.
     */
    var cfgs = spec.configurationProperties();
    var subValues = spec.getSubmissionTimeValues();
    var logicalModel =
        sam.createLogicalModel(jobId, adl.get(), subValues, cfgs, resetObjectTracker);
    var fusion = resolveFusion(spec.getFusion());
    var jobModel = sam.createJob(name, adl.get(), logicalModel, fusion, cfgs, parWidths);
    return new ModelWrapper(jobModel, logicalModel);
  }

  public synchronized JobModelContext create(
      String name, String namespace, JobSpec spec, BigInteger jobId) throws Exception {
    /*
     * Create the models for this job for the first time. Since this is the first time, we use the
     * parallel widths from the original JobSpec, and we tell the SAM pipeline to start with a
     * fresh object tracker.
     */
    var models =
        createModels(
            name,
            namespace,
            spec,
            jobId,
            spec.getParallelRegionWidths(),
            Pipeline.RESET_OBJECT_TRACKER);
    return new JobModelContext(sam, jobId, models.getLogical(), models.getJob());
  }

  synchronized JobModelContext updateParallelWidth(
      String name,
      String namespace,
      JobSpec spec,
      String region,
      Map<BigInteger, Set<String>> peIdToOperators,
      Map<String, BigInteger> priorWidths,
      Map<String, BigInteger> currWidths)
      throws Exception {
    LOGGER.info("Updating parallel widths: {} -> {}", priorWidths, currWidths);
    /*
     * The old SAM code needs these sets, but we don't. We let kube determine which PEs/pods to
     * restart based on whether or not the AADL has changed. We may change the old SAM code
     * interface in the future to remove the need for these sets.
     */
    var changedOperators = new HashSet<String>();
    var addedOperators = new HashSet<String>();
    var removedOperators = new HashSet<String>();
    /*
     * The prior models contain the application as it is currently running, with the old width. The curr models
     * contain the application with the new width, and it has the structure that we want. But the operator indexes
     * don't match with those in the prior model, which means they don't match with what is currently running. We
     * perform the parallel diff and transfer so that the prior model contains both the correct indexes and the
     * modified parallel width.
     */
    var priorModels =
        createModels(
            name + "prior",
            namespace,
            spec,
            spec.getId(),
            priorWidths,
            Pipeline.RESET_OBJECT_TRACKER);
    var currModels =
        createModels(
            name + "curr", namespace, spec, spec.getId(), currWidths, Pipeline.KEEP_OBJECT_TRACKER);
    sam.transferParallelRegionDifference(
        priorModels.getJob(),
        currModels.getJob(),
        region,
        changedOperators,
        addedOperators,
        removedOperators);
    LOGGER.info("Added operators: {}", addedOperators);
    LOGGER.info("Removed operators: {}", removedOperators);
    LOGGER.info("Modified operators: {}", changedOperators);
    /*
     * The topology application in priorModels now contains the updated topology application. But, the job object
     * in the model still represents what is currently running. We need to re-fuse the application to get a new
     * job object that contains the PEs that we want to run after the width change.
     */
    var adl = priorModels.getJob().getConfig().getAdlString();
    var priorLogicalModel = priorModels.getLogical();
    var priorTopologyModel = priorModels.getJob().getTopologyApplication();
    var fusion = resolveFusion(spec.getFusion());
    var updatedJob =
        sam.createJob(name, adl, priorLogicalModel, priorTopologyModel, peIdToOperators, fusion);
    return new JobModelContext(sam, spec.getId(), priorLogicalModel, updatedJob);
  }

  public synchronized void close() {
    sam.close();
  }
}
