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

package com.ibm.streams.controller.k8s.pods.pes;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_DOCKER_GID;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_DOCKER_UID;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_REDIS_CONFIG_ANNOTATION_KEY;

import com.ibm.streams.controller.crds.hostpools.HostPool;
import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.crds.pes.ProcessingElement;
import com.ibm.streams.controller.crds.pes.ProcessingElementSpec;
import com.ibm.streams.controller.k8s.pods.ConfigMapVolumeBinder;
import com.ibm.streams.controller.k8s.pods.HostPathVolumeBinder;
import com.ibm.streams.controller.k8s.pods.KubernetesLabels;
import com.ibm.streams.controller.k8s.pods.NodeAffinityMaker;
import com.ibm.streams.controller.k8s.pods.PersistentVolumeClaimBinder;
import com.ibm.streams.controller.k8s.pods.PodAffinityLabels;
import com.ibm.streams.controller.k8s.pods.PodAntiAffinityLabels;
import com.ibm.streams.controller.k8s.pods.SecretVolumeBinder;
import com.ibm.streams.controller.k8s.pods.VolumeMountBinder;
import com.ibm.streams.controller.utils.CommonEnvironment;
import io.fabric8.kubernetes.api.model.Affinity;
import io.fabric8.kubernetes.api.model.Capabilities;
import io.fabric8.kubernetes.api.model.ConfigMap;
import io.fabric8.kubernetes.api.model.Container;
import io.fabric8.kubernetes.api.model.EnvVar;
import io.fabric8.kubernetes.api.model.ExecAction;
import io.fabric8.kubernetes.api.model.LocalObjectReference;
import io.fabric8.kubernetes.api.model.ObjectMeta;
import io.fabric8.kubernetes.api.model.PodDNSConfig;
import io.fabric8.kubernetes.api.model.PodDNSConfigOptionBuilder;
import io.fabric8.kubernetes.api.model.PodSecurityContext;
import io.fabric8.kubernetes.api.model.PodSpec;
import io.fabric8.kubernetes.api.model.Probe;
import io.fabric8.kubernetes.api.model.SecurityContext;
import io.fabric8.kubernetes.api.model.Volume;
import io.fabric8.kubernetes.api.model.VolumeMount;
import io.fabric8.kubernetes.client.KubernetesClient;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.Optional;
import java.util.stream.Collectors;
import lombok.var;
import org.apache.commons.validator.routines.InetAddressValidator;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class PodSpecBuilder {

  private static final Logger logger = LoggerFactory.getLogger(PodSpecBuilder.class);

  private final KubernetesClient client;
  private final ProcessingElementSpec peSpec;
  private final ObjectMeta peMeta;
  private final CommonEnvironment env;
  private final String jobName;
  private final VolumeMountBinder peConfigVolume;
  private final VolumeMountBinder jobConfigVolume;
  private final String redisConfig;
  private final Optional<VolumeMountBinder> dataVolume;
  private final Optional<VolumeMountBinder> checkpointVolume;
  private final Optional<VolumeMountBinder> appPropertiesSecretVolume;
  private final Optional<VolumeMountBinder> restrictedPropertiesSecretVolume;
  private final PodAffinityLabels podAffinities;
  private final PodAntiAffinityLabels podAntiAffinities;
  private final KubernetesLabels isolationLabels;
  private final Optional<NodeAffinityMaker> nodeAffinities;

  public PodSpecBuilder(
      KubernetesClient client,
      Job job,
      ProcessingElement pe,
      ConfigMap cm,
      HostPool hp,
      CommonEnvironment env) {
    this.client = client;
    this.peSpec = pe.getSpec();
    this.peMeta = pe.getMetadata();
    this.env = env;
    this.jobName = peMeta.getLabels().get(STREAMS_JOB_LABEL_KEY);
    this.peConfigVolume =
        new ConfigMapVolumeBinder("pe-config-volume", cm.getMetadata().getName())
            .addMount("/etc/config/pe");
    this.jobConfigVolume =
        new ConfigMapVolumeBinder("job-config-volume", jobName).addMount("/etc/config/job");
    this.podAffinities = new PodAffinityLabels();
    this.podAntiAffinities = new PodAntiAffinityLabels();
    this.isolationLabels = new KubernetesLabels("hostisolation");
    /*
     * Redis configuration. Can be null.
     */
    this.redisConfig = job.getMetadata().getAnnotations().get(STREAMS_REDIS_CONFIG_ANNOTATION_KEY);
    /*
     * Data volume.
     */
    if (peSpec.getDataVolumeClaim() != null && peSpec.getDataVolumeClaim().getName() != null) {
      var c = peSpec.getDataVolumeClaim();
      var v =
          new PersistentVolumeClaimBinder("data-volume", c.getName())
              .addMount("/data", c.getSubPath());
      this.dataVolume = Optional.of(v);
    } else if (peSpec.getDataPath() != null) {
      var v = new HostPathVolumeBinder("data-volume", peSpec.getDataPath()).addMount("/data");
      this.dataVolume = Optional.of(v);
    } else if (env.getDataPath().isPresent()) {
      var v = new HostPathVolumeBinder("data-volume", env.getDataPath().get()).addMount("/data");
      this.dataVolume = Optional.of(v);
    } else {
      this.dataVolume = Optional.empty();
    }
    /*
     * Checkpoint volume.
     */
    var ckptStore = peSpec.getCheckpointDataStore();
    if (ckptStore != null && ckptStore.getFileSystem() != null) {
      if (ckptStore.getFileSystem().getVolumeClaim() != null) {
        var c = ckptStore.getFileSystem().getVolumeClaim();
        var b = new PersistentVolumeClaimBinder("checkpoint-volume", c.getName());
        var v = b.addMount("/checkpoint", c.getSubPath());
        this.checkpointVolume = Optional.of(v);
      } else if (ckptStore.getFileSystem().getPath() != null) {
        var b = new HostPathVolumeBinder("checkpoint-volume", ckptStore.getFileSystem().getPath());
        var v = b.addMount("/checkpoint");
        this.checkpointVolume = Optional.of(v);
      } else if (env.getCheckpointPath().isPresent()) {
        var b = new HostPathVolumeBinder("checkpoint-volume", env.getCheckpointPath().get());
        var v = b.addMount("/checkpoint");
        this.checkpointVolume = Optional.of(v);
      } else {
        this.checkpointVolume = Optional.empty();
      }
    } else {
      this.checkpointVolume = Optional.empty();
    }
    /*
     * App Restricted properties secret volume.
     */
    if (peSpec.getAppPropertiesSecret() != null) {
      var v =
          new SecretVolumeBinder("app-properties-secret-volume", peSpec.getAppPropertiesSecret());
      v.addMount("/etc/config/app");
      this.appPropertiesSecretVolume = Optional.of(v);
    } else {
      this.appPropertiesSecretVolume = Optional.empty();
    }
    /*
     * Restricted properties secret volume.
     */
    if (peSpec.getRestrictedPropertiesSecret() != null) {
      var v =
          new SecretVolumeBinder(
                  "restricted-properties-secret-volume", peSpec.getRestrictedPropertiesSecret())
              .addMount("/etc/config/restricted");
      this.restrictedPropertiesSecretVolume = Optional.of(v);
    } else {
      this.restrictedPropertiesSecretVolume = Optional.empty();
    }
    /*
     * Host colocations.
     */
    if (!peSpec.getPlacement().getCoLocations().isEmpty()) {
      podAffinities.addSymmetric("hostcolocation");
      peSpec
          .getPlacement()
          .getCoLocations()
          .forEach(co -> podAffinities.addSymmetricLabel("hostcolocation", co));
    }
    /*
     * Host exlocations.
     */
    if (!peSpec.getPlacement().getExLocations().isEmpty()) {
      podAntiAffinities.addSymmetric("hostexlocation");
      peSpec
          .getPlacement()
          .getExLocations()
          .forEach(ex -> podAntiAffinities.addSymmetricLabel("hostexlocation", ex));
    }
    /*
     * Host isolation labels and anti-affinities. Isolation labels are so that PEs which have an
     * anti-affinity spec due to their isolation request will not execute with us. And if this PE
     * has an isolations request, we add our own anti-affinity spec.
     */
    if (!peSpec.getPlacement().getIsolationLabels().isEmpty()) {
      peSpec.getPlacement().getIsolationLabels().forEach(isolationLabels::add);
    }
    if (peSpec.getPlacement().getIsolation() != null) {
      podAntiAffinities
          .addAsymmetric("hostisolation")
          .addAsymmetricLabel("hostisolation", peSpec.getPlacement().getIsolation());
    }
    /*
     * Hostpools.
     */
    if (peSpec.getPlacement().getHostPoolLocation() != null) {
      var inPoolIndex = peSpec.getPlacement().getHostPoolLocation().getInpoolIndex();
      /*
       * Hostpools with explicit hosts are not tagged, and tagged hostpools do not specify
       * explicit hosts. Hence, we can safely consider both cases separately.
       */
      if (!hp.getSpec().getHostpool().getHosts().isEmpty()) {
        // TODO: handle explicitly named hostpools
        nodeAffinities = Optional.empty();
      } else if (!hp.getSpec().getHostpool().getTags().isEmpty()) {
        if (inPoolIndex == null) {
          nodeAffinities = Optional.of(new NodeAffinityMaker());
          hp.getSpec().getHostpool().getTags().forEach(tag -> nodeAffinities.get().add(tag));
        } else {
          // TODO: handle when a user requested a specific host in a tagged pool, for
          // example `host(myHostPool[2])`.
          nodeAffinities = Optional.empty();
        }
      } else {
        nodeAffinities = Optional.empty();
      }
    } else {
      nodeAffinities = Optional.empty();
    }
  }

  private List<VolumeMount> buildVolumeMounts() {
    List<VolumeMount> mounts = new ArrayList<>();
    mounts.addAll(peConfigVolume.getMounts());
    mounts.addAll(jobConfigVolume.getMounts());
    dataVolume.ifPresent(v -> mounts.addAll(v.getMounts()));
    checkpointVolume.ifPresent(v -> mounts.addAll(v.getMounts()));
    appPropertiesSecretVolume.ifPresent(v -> mounts.addAll(v.getMounts()));
    restrictedPropertiesSecretVolume.ifPresent(v -> mounts.addAll(v.getMounts()));
    return mounts;
  }

  private List<Volume> buildVolumes() {
    List<Volume> volumes = new ArrayList<>();
    volumes.add(peConfigVolume.build());
    volumes.add(jobConfigVolume.build());
    dataVolume.ifPresent(v -> volumes.add(v.build()));
    checkpointVolume.ifPresent(v -> volumes.add(v.build()));
    appPropertiesSecretVolume.ifPresent(v -> volumes.add(v.build()));
    restrictedPropertiesSecretVolume.ifPresent(v -> volumes.add(v.build()));
    return volumes;
  }

  private List<EnvVar> buildContainerEnvironment() {
    /*
     * Build the default environment.
     */
    var result =
        Arrays.stream(
                new String[][] {
                  {"STREAMS_DOMAIN_ID", "kubernetes"},
                  {"STREAMS_INITIAL_TRACE_LEVEL", peSpec.getRuntimeTraceLevel()},
                  {"STREAMS_INSTALL", env.getStreamsInstall()},
                  {"STREAMS_INSTANCE_ID", peMeta.getNamespace()},
                  {"STREAMS_JOB_NAME", jobName},
                  {"STREAMS_NS", "K8S:"},
                  {"STREAMS_PE_ID", peSpec.getId().toString()},
                  {"STREAMS_PE_LAUNCH_COUNT", Integer.toString(peSpec.getLaunchCount())},
                  {"STREAMS_PE_NAME", peMeta.getName()},
                  {"STREAMS_PE_USE_VALGRIND", Boolean.toString(peSpec.isRunWithValgrind())},
                  {"STREAMS_SAB_NAME", peSpec.getBundleName()},
                  {"STREAMS_TRANSPORT_SECURITY_TYPE", peSpec.getTransportSecurityType()}
                })
            .map(e -> new EnvVar(e[0], e[1], null))
            .collect(Collectors.toCollection(LinkedList::new));
    /*
     * Check if we need to append the trace level override. By default, the runtime:
     * 1. Uses the trace update in the AADL. If not present,
     * 2. Uses the trace value from the environment variable below. If not present,
     * 3. Uses L_ERROR.
     */
    if (peSpec.getAppTraceLevel() != null) {
      result.add(new EnvVar("STREAMS_APP_TRACE_LEVEL", peSpec.getAppTraceLevel(), null));
    }
    /*
     * Append the checkpoint configuration.
     */
    var ckptStore = peSpec.getCheckpointDataStore();
    if (ckptStore != null) {
      if (ckptStore.getFileSystem() != null) {
        var configuration = "{ \"Dir\" : \"/checkpoint\" }";
        result.add(new EnvVar("STREAMS_CHECKPOINT_REPOSITORY", "fileSystem", null));
        result.add(new EnvVar("STREAMS_CHECKPOINT_REPOSITORY_CONFIGURATION", configuration, null));
      } else if (ckptStore.getRedis() != null && this.redisConfig != null) {
        result.add(new EnvVar("STREAMS_CHECKPOINT_REPOSITORY", "redis", null));
        result.add(new EnvVar("STREAMS_CHECKPOINT_REPOSITORY_CONFIGURATION", redisConfig, null));
      } else if (env.getCheckpointPath().isPresent()) {
        var configuration = "{ \"Dir\" : \"/checkpoint\" }";
        result.add(new EnvVar("STREAMS_CHECKPOINT_REPOSITORY", "fileSystem", null));
        result.add(new EnvVar("STREAMS_CHECKPOINT_REPOSITORY_CONFIGURATION", configuration, null));
      }
    }
    return result;
  }

  private Probe buildLivenessProbe() {
    ExecAction a = new ExecAction();
    a.setCommand(Arrays.asList("cat", "/tmp/live"));
    Probe p = new Probe();
    p.setExec(a);
    p.setInitialDelaySeconds(peSpec.getInitialProbeDelayInSeconds());
    p.setPeriodSeconds(peSpec.getProbeIntervalInSeconds());
    p.setFailureThreshold(peSpec.getProbeFailureThreshold());
    return p;
  }

  private String getBuildType() {
    /*
     * Grab the build type from the environment.
     */
    String buildType = env.getBuildType();
    /*
     * Check if the user has overriden it.
     */
    if (peSpec.buildType().isPresent()) {
      buildType = peSpec.buildType().get();
    }
    /*
     * If valgrind is requested, enforce "debug".
     */
    if (peSpec.isRunWithValgrind()) {
      if (!buildType.equals("debug")) {
        logger.warn("PEs are set to run with Valgrind, forcing the PE build type to \"debug\"");
        buildType = "debug";
      }
    }
    return buildType.toLowerCase();
  }

  private String getContainerImage() {
    var type = getBuildType();
    return peSpec.getImage() != null
        ? peSpec.getImage()
        : String.format(
            "%s/%s/streams-runtime:6.%s", env.getRegistry(), env.getRegistryNameSpace(), type);
  }

  private List<Container> buildContainers() {
    Container c = new Container();
    c.setName("pe");
    c.setImage(getContainerImage());
    c.setImagePullPolicy(peSpec.getImagePullPolicy());
    c.setEnv(buildContainerEnvironment());
    c.setArgs(
        Collections.singletonList(
            env.getStreamsInstall() + "/system/impl/bin/streams-k8s-bootstrap"));
    c.setVolumeMounts(buildVolumeMounts());
    c.setLivenessProbe(buildLivenessProbe());
    c.setResources(peSpec.getResources());
    /*
     * Enable SYS_PTRACE if requested.
     */
    if (peSpec.isAllowPtrace() || peSpec.isRunWithValgrind()) {
      var caps = new Capabilities();
      caps.setAdd(Arrays.asList("SYS_ADMIN", "SYS_PTRACE"));
      var sc = new SecurityContext();
      sc.setCapabilities(caps);
      c.setSecurityContext(sc);
    }
    return Collections.singletonList(c);
  }

  /*
   * See: https://blog.quentin-machu.fr/2018/06/24/5-15s-dns-lookups-on-kubernetes/
   */
  private PodDNSConfig buildPodDnsConfig() {
    var singleRequestReopen =
        new PodDNSConfigOptionBuilder().withName("single-request-reopen").build();
    var dnsConfig = new PodDNSConfig();
    dnsConfig.setOptions(Collections.singletonList(singleRequestReopen));
    return dnsConfig;
  }

  private List<LocalObjectReference> buildImagePullSecrets() {
    LocalObjectReference s = new LocalObjectReference();
    s.setName(peSpec.getImagePullSecret());
    return Collections.singletonList(s);
  }

  /*
   * If host is a valid IP address, returns name for that node. Otherwise,
   * returns input string. If host is a valid IP address, but that IP
   * address is not in any node, throws exception.
   */
  private String mapIpAddressToNodeName(String host) {
    var validator = InetAddressValidator.getInstance();
    if (validator.isValidInet4Address(host) || validator.isValidInet6Address(host)) {
      return client.nodes().list().getItems().stream()
          .filter(
              n -> n.getStatus().getAddresses().stream().anyMatch(a -> a.getAddress().equals(host)))
          .findFirst()
          .orElseThrow(IllegalArgumentException::new)
          .getMetadata()
          .getName();
    }
    return host;
  }

  public PodSpec build() {
    /*
     * Define the Pod's security context.
     */
    var sc = new PodSecurityContext();
    sc.setRunAsUser(STREAMS_DOCKER_UID);
    sc.setRunAsGroup(STREAMS_DOCKER_GID);
    /*
     * Build the pod spec. We use a restart policy of "Never" as the pod controller
     * takes care of that itself to track the restart count in the AADL.
     */
    PodSpec ps = new PodSpec();
    ps.setSecurityContext(sc);
    ps.setContainers(buildContainers());
    ps.setDnsConfig(buildPodDnsConfig());
    ps.setRestartPolicy("Never");
    ps.setVolumes(buildVolumes());
    /*
     * Set the image pull secret if it has been defined.
     */
    if (peSpec.getImagePullSecret() != null) {
      ps.setImagePullSecrets(buildImagePullSecrets());
    }
    /*
     * Handle all placement concerns. This includes setting a node name, pod affinities
     * and pod anti-affinities and any necessary new labels.
     */
    peSpec.getPlacement().host().ifPresent(h -> ps.setNodeName(mapIpAddressToNodeName(h)));
    if (!podAffinities.isEmpty()
        || !podAntiAffinities.isEmpty()
        || !isolationLabels.getLabels().isEmpty()
        || nodeAffinities.isPresent()) {
      var affinities = new Affinity();
      var labels = peMeta.getLabels();
      labels.putAll(isolationLabels.getLabels());
      if (!podAffinities.isEmpty()) {
        labels.putAll(podAffinities.getLabels());
        affinities.setPodAffinity(podAffinities.build());
      }
      if (!podAntiAffinities.isEmpty()) {
        labels.putAll(podAntiAffinities.getLabels());
        affinities.setPodAntiAffinity(podAntiAffinities.build());
      }
      nodeAffinities.ifPresent(na -> affinities.setNodeAffinity(na.build()));
      ps.setAffinity(affinities);
      peMeta.setLabels(labels);
    }
    return ps;
  }
}
