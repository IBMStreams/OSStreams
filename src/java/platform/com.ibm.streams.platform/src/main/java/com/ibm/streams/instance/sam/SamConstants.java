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

import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;

public class SamConstants {

  // Constants related to Job config (submitjob -C options)
  public static final String tracing = "tracing";
  public static final String traceValues[] = {"off", "error", "warn", "info", "debug", "trace"};
  public static final Set<String> allowableTraceSettings =
      new HashSet<String>(Arrays.asList(traceValues));
  public static final String data_directory = "data-directory";
  public static final String dataDirectory = "dataDirectory";
  public static final String preloadApplicationBundles = "preloadApplicationBundles";
  public static final String preload_enabled = "true";
  public static final String preload_disabled = "false";
  public static final String booleanValues[] = {"true", "false", "TRUE", "FALSE"};
  public static final Set<String> allowableBooleanSettings =
      new HashSet<String>(Arrays.asList(booleanValues));

  // Constants related to job deployment configurations
  public static final String fusionScheme = "fusionScheme";
  public static final String fusionScheme_legacy = "legacy";
  public static final String fusionScheme_automatic = "automatic";
  public static final String fusionScheme_manual = "manual";
  public static final String fusionSchemeValues[] = {
    fusionScheme_legacy, fusionScheme_automatic, fusionScheme_manual
  };
  public static final Set<String> allowableFusionSchemeValues =
      new HashSet<String>(Arrays.asList(fusionSchemeValues));
  public static final String fusionTargetPeCount = "fusionTargetPeCount";
  public static final String placementScheme = "placementScheme";
  public static final String placementScheme_legacy = "legacy";
  public static final String placementScheme_balancedJob = "balancedJob";
  public static final String placementScheme_balancedInstance = "balancedInstance";
  public static final String placementSchemeValues[] = {
    placementScheme_legacy, placementScheme_balancedJob, placementScheme_balancedInstance
  };
  public static final Set<String> allowablePlacementSchemeValues =
      new HashSet<String>(Arrays.asList(placementSchemeValues));
  public static final String threadingModel = "threadingModel";
  public static final String threadingModel_manual = "manual";
  public static final String threadingModel_automatic = "automatic";
  public static final String threadingModel_dynamic = "dynamic";
  public static final String threadingModel_dedicated = "dedicated";
  public static final String threadingModel_notSpecified = "notSpecified";
  public static final String threadingModelValues[] = {
    threadingModel_manual,
    threadingModel_automatic,
    threadingModel_dynamic,
    threadingModel_dedicated,
    threadingModel_notSpecified
  };
  public static final Set<String> allowableThreadingModelValues =
      new HashSet<String>(Arrays.asList(threadingModelValues));
  public static final String dynamicThreadingThreadCount = "dynamicThreadingThreadCount";
  public static final String dynamicThreadingElastic = "dynamicThreadingElastic";
  public static final String overrideResourceLoadProtection = "overrideResourceLoadProtection";
  public static final String jobResourceSharing = "jobResourceSharing";
  public static final String jobResourceSharing_sameJob = "sameJob";
  public static final String jobResourceSharing_sameUser = "sameUser";
  public static final String jobResourceSharing_sameInstance = "sameInstance";
  public static final String jobResourceSharingValues[] = {
    jobResourceSharing_sameJob, jobResourceSharing_sameUser, jobResourceSharing_sameInstance
  };
  public static final Set<String> allowableJobResourceSharingValues =
      new HashSet<String>(Arrays.asList(jobResourceSharingValues));
  public static final String fusionChangesProhibited = "fusionChangesProhibited";
  public static final String forcePeStopped = "forcePeStopped";
  public static final String forceUpdate = "forceUpdate";
  public static final String enableAutomaticAugmentation = "enableAutomaticAugmentation";
  public static final String proposedOperatorsPerResource = "proposedOperatorsPerResource";
  // parallelRegionConfig
  public static final String parallelRegionConfig = "parallelRegionConfig";
  public static final String fusionType = "fusionType";
  public static final String fusionType_channelIsolation = "channelIsolation";
  public static final String fusionType_channelExlocation = "channelExlocation";
  public static final String fusionType_noChannelInfluence = "noChannelInfluence";
  public static final String fusionTypeValues[] = {
    fusionType_channelIsolation, fusionType_channelExlocation, fusionType_noChannelInfluence
  };
  public static final Set<String> allowableFusionTypeValues =
      new HashSet<String>(Arrays.asList(fusionTypeValues));

  public static final String partitionIsolation = "partitionIsolation";
  public static final String partitionColocation = "partitionColocation";
  public static final String partitionExlocation = "partitionExlocation";
  public static final String listInfoOnly = "listInfoOnly";
  public static final String applicationOperatorConstraints = "applicationOperatorConstraints";
  public static final String resultingOperatorFusionSets = "resultingOperatorFusionSets";

  public static final String DISPLAY_NAME = "display";
  public static final String PARTIALLY_QUALIFIED_DISPLAY_NAME = "pe." + DISPLAY_NAME;
  public static final String QUALIFIED_DISPLAY_NAME =
      "streams::" + PARTIALLY_QUALIFIED_DISPLAY_NAME;
  public static final String DISPLAY_VAR_NAME = QUALIFIED_DISPLAY_NAME;

  public static final String consistentRegion = "consistentRegion";
}
