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

package com.ibm.streams.controller.instance.broker;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_SCOPE_ANNOTATION_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.ibm.streams.admin.internal.api.StreamsException;
import com.ibm.streams.controller.crds.exports.Export;
import com.ibm.streams.controller.crds.exports.ExportStore;
import com.ibm.streams.controller.crds.imports.Import;
import com.ibm.streams.controller.crds.imports.ImportStore;
import com.ibm.streams.controller.crds.jobs.JobStore;
import com.ibm.streams.controller.events.IEventConsumerDelegate;
import com.ibm.streams.controller.executor.EExecutionCommand;
import com.ibm.streams.controller.executor.IExecutor;
import com.ibm.streams.controller.state.subscription.SubscriptionBoard;
import com.ibm.streams.controller.utils.CommonEnvironment;
import com.ibm.streams.instance.sam.model.DynamicConnIpSubs;
import com.ibm.streams.instance.sam.model.DynamicConnOpProps;
import com.ibm.streams.instance.sam.model.topology.StreamProperty;
import io.fabric8.kubernetes.api.model.HasMetadata;
import java.math.BigInteger;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.function.BiConsumer;
import java.util.function.Function;
import java.util.stream.Collectors;
import lombok.var;
import org.microbean.kubernetes.controller.AbstractEvent;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ImportExportBroker implements IEventConsumerDelegate<HasMetadata> {

  private static final Logger LOGGER = LoggerFactory.getLogger(ImportExportBroker.class);

  private final SubscriptionBoard subscriptionBoard;
  private final JobStore jobStore;
  private final ImportStore importStore;
  private final ExportStore exportStore;
  private final IExecutor executor;
  private final String ns;
  private final CommonEnvironment env;
  private final ObjectMapper mapper;

  private DatagramSocket socket;

  public ImportExportBroker(
      SubscriptionBoard subscriptionBoard,
      JobStore jobStore,
      ImportStore importStore,
      ExportStore exportStore,
      IExecutor executor,
      String ns,
      CommonEnvironment env) {
    /*
     * Save parameters.
     */
    this.subscriptionBoard = subscriptionBoard;
    this.jobStore = jobStore;
    this.importStore = importStore;
    this.exportStore = exportStore;
    this.executor = executor;
    this.ns = ns;
    this.env = env;
    this.mapper = new ObjectMapper();
    /*
     * Create the datagram socket for notifications.
     */
    try {
      this.socket = new DatagramSocket();
    } catch (SocketException ex) {
      LOGGER.error("Cannot create a UDP socket, subscription notifications will be disabled.");
      this.socket = null;
    }
  }

  /*
   * Utility functions.
   */

  private boolean hasCompatibleFilterPolicy(Export exp, Import imp) {
    if (imp.getSpec().getStreams().getFilter() == null) {
      return true;
    }
    return exp.getSpec().getStream().isAllowFilter();
  }

  /*
   * Import match methods.
   */

  private List<Export> matchExportByStreamName(Import imp) {
    /*
     * Scan the exports for a match.
     */
    return exportStore.values().stream()
        .filter(v -> v.getSpec().getStream().getName() != null)
        .filter(v -> hasCompatibleFilterPolicy(v, imp))
        .filter(v -> ImportExportCommons.matchAnyNamedStream(v, imp))
        .collect(Collectors.toList());
  }

  private List<Export> matchExportBySubscription(Import imp) {
    /*
     * Build the subscription.
     */
    var sis = imp.getSpec().getStreams().getPropertyBasedImport();
    var subs =
        new DynamicConnIpSubs(BigInteger.ZERO, sis.getApplicationScope(), sis.getSubscription());
    /*
     * Scan the imports for a match.
     */
    return exportStore.values().stream()
        .filter(v -> v.getSpec().getStream().getProperties() != null)
        .filter(v -> ImportExportCommons.checkApplicationScope(v, sis))
        .filter(v -> hasCompatibleFilterPolicy(v, imp))
        .filter(
            v -> {
              /*
               * Grab the scope.
               */
              var scope = v.getMetadata().getAnnotations().get(STREAMS_APP_SCOPE_ANNOTATION_KEY);
              /*
               * Convert the export properties into ExportProperties.
               */
              var opProps =
                  new DynamicConnOpProps(
                      BigInteger.ZERO,
                      scope,
                      v.getSpec().getStream().getProperties().stream()
                          .collect(Collectors.toMap(StreamProperty::getName, Function.identity())));
              /*
               * Check if we have a match.
               */
              try {
                return subs.doesMatch(opProps, true);
              } catch (StreamsException ex) {
                return false;
              }
            })
        .collect(Collectors.toList());
  }

  private Map<String, List<Export>> matchExports(Import imp) {
    /*
     * Execute the proper matching based on the spec.
     */
    List<Export> exports = new LinkedList<>();
    if (imp.getSpec().getStreams().getNameBasedImports() != null) {
      exports.addAll(matchExportByStreamName(imp));
    }
    if (imp.getSpec().getStreams().getPropertyBasedImport() != null) {
      exports.addAll(matchExportBySubscription(imp));
    }
    /*
     * Convert into a job-keyed map.
     */
    Map<String, List<Export>> exportsPerJob = new HashMap<>();
    for (Export exp : exports) {
      String jobName = exp.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
      if (exportsPerJob.containsKey(jobName)) {
        exportsPerJob.get(jobName).add(exp);
      } else {
        List<Export> lst = new LinkedList<>();
        lst.add(exp);
        exportsPerJob.put(jobName, lst);
      }
    }
    /*
     * Filter-out exports with missing job.
     */
    return exportsPerJob.entrySet().stream()
        .filter(e -> jobStore.hasJobWithName(e.getKey()))
        .collect(Collectors.toMap(Map.Entry::getKey, Map.Entry::getValue));
  }

  /*
   * Export match methods.
   */

  private List<Import> matchImportByStreamName(Export exp) {
    /*
     * Scan the imports for a match.
     */
    return importStore.values().stream()
        .filter(v -> v.getSpec().getStreams().getNameBasedImports() != null)
        .filter(v -> hasCompatibleFilterPolicy(exp, v))
        .filter(v -> ImportExportCommons.matchAnyNamedStream(exp, v))
        .collect(Collectors.toList());
  }

  private List<Import> matchImportByProperties(Export exp) {
    /*
     * Grab the application scope.
     */
    var scope = exp.getMetadata().getAnnotations().get(STREAMS_APP_SCOPE_ANNOTATION_KEY);
    /*
     * Convert the export properties into ExportProperties.
     */

    var opProps =
        new DynamicConnOpProps(
            BigInteger.ZERO,
            scope,
            exp.getSpec().getStream().getProperties().stream()
                .collect(Collectors.toMap(StreamProperty::getName, Function.identity())));
    /*
     * Scan the imports for a match.
     */
    return importStore.values().stream()
        .filter(v -> v.getSpec().getStreams().getPropertyBasedImport() != null)
        .filter(v -> hasCompatibleFilterPolicy(exp, v))
        .filter(
            v ->
                ImportExportCommons.checkApplicationScope(
                    exp, v.getSpec().getStreams().getPropertyBasedImport()))
        .filter(
            v -> {
              var sis = v.getSpec().getStreams().getPropertyBasedImport();
              var subs =
                  new DynamicConnIpSubs(
                      BigInteger.ZERO, sis.getApplicationScope(), sis.getSubscription());
              /*
               * Check if we have a match.
               */
              try {
                return subs.doesMatch(opProps, true);
              } catch (StreamsException ex) {
                return false;
              }
            })
        .collect(Collectors.toList());
  }

  private Map<String, List<Import>> matchImports(Export exp) {
    /*
     * Execute the proper matching based on the spec.
     */
    List<Import> imports = new LinkedList<>();
    if (exp.getSpec().getStream().getName() != null) {
      imports.addAll(matchImportByStreamName(exp));
    }
    if (exp.getSpec().getStream().getProperties() != null) {
      imports.addAll(matchImportByProperties(exp));
    }
    /*
     * Convert into a job-keyed map.
     */
    Map<String, List<Import>> importsPerJob = new HashMap<>();
    for (Import imp : imports) {
      String jobName = imp.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
      if (importsPerJob.containsKey(jobName)) {
        importsPerJob.get(jobName).add(imp);
      } else {
        List<Import> lst = new LinkedList<>();
        lst.add(imp);
        importsPerJob.put(jobName, lst);
      }
    }
    /*
     * Filter-out exports with missing job.
     */
    return importsPerJob.entrySet().stream()
        .filter(e -> jobStore.hasJobWithName(e.getKey()))
        .collect(Collectors.toMap(Map.Entry::getKey, Map.Entry::getValue));
  }

  /*
   * Helper functions.
   */

  private void sendSubscriptionNotification(String target, String content) {
    ImportExportCommons.sendSubscriptionNotification(this.socket, target, content);
  }

  private void generatePayloadForProcessingElement(
      String name, BiConsumer<String, String> consumer) {
    try {
      var target = name + "." + this.ns;
      var content = mapper.writeValueAsString(subscriptionBoard.get(name));
      consumer.accept(target, content);
    } catch (JsonProcessingException ignored) {
    }
  }

  private void sendNotifications(
      Export exp, Map<String, List<Import>> imports, Collection<String> extra) {
    /*
     * Skip the event generation if not required.
     */
    if (imports.isEmpty() || !env.enableNotifications()) {
      return;
    }
    /*
     * Compute the list of target PEs.
     */
    var peSet =
        imports.values().stream()
            .flatMap(Collection::stream)
            .map(ImportExportCommons::getPeNameForImport)
            .collect(Collectors.toSet());
    peSet.add(ImportExportCommons.getPeNameForExport(exp));
    peSet.addAll(extra);
    /*
     * Send a notification to the PEs.
     */
    peSet.forEach(
        pe -> generatePayloadForProcessingElement(pe, this::sendSubscriptionNotification));
  }

  private void sendNotifications(
      Import imp, Map<String, List<Export>> exports, Collection<String> extra) {
    /*
     * Skip the event generation if not required.
     */
    if (exports.isEmpty() || !env.enableNotifications()) {
      return;
    }
    /*
     * Compute the list of target PEs.
     */
    var peSet =
        exports.values().stream()
            .flatMap(Collection::stream)
            .map(ImportExportCommons::getPeNameForExport)
            .collect(Collectors.toSet());
    peSet.add(ImportExportCommons.getPeNameForImport(imp));
    peSet.addAll(extra);
    /*
     * Send a notification to the PEs.
     */
    peSet.forEach(
        pe -> generatePayloadForProcessingElement(pe, this::sendSubscriptionNotification));
  }

  /*
   * Addition methods.
   */

  @Override
  public void onAddition(AbstractEvent<? extends HasMetadata> event) {
    var resource = event.getResource();
    if (resource instanceof Export) {
      executor.execute(EExecutionCommand.ADD_EXPORT_TO_BROKER, () -> onAddition((Export) resource));
    } else if (resource instanceof Import) {
      executor.execute(EExecutionCommand.ADD_IMPORT_TO_BROKER, () -> onAddition((Import) resource));
    }
  }

  private void onAddition(Export exp) {
    /*
     * Build the import list for the export.
     */
    var imports = matchImports(exp);
    /*
     * Update the subscription board.
     */
    imports.values().stream()
        .flatMap(Collection::stream)
        .forEach(
            imp -> {
              LOGGER.info(
                  "BROKER Export \"{}\" - Import \"{}\"",
                  exp.getMetadata().getName(),
                  imp.getMetadata().getName());
              subscriptionBoard.add(exp, imp);
            });
    /*
     * Send a notification to the owning PEs.
     */
    sendNotifications(exp, imports, Collections.emptyList());
  }

  private void onAddition(Import imp) {
    /*
     * Build the export list for the import.
     */
    var exports = matchExports(imp);
    /*
     * Update the subscription board.
     */
    exports.values().stream()
        .flatMap(Collection::stream)
        .forEach(
            exp -> {
              LOGGER.info(
                  "BROKER Import \"{}\" to Export \"{}\"",
                  imp.getMetadata().getName(),
                  exp.getMetadata().getName());
              subscriptionBoard.add(exp, imp);
            });
    /*
     * Send a notification to the owning PEs.
     */
    sendNotifications(imp, exports, Collections.emptyList());
  }

  /*
   * Modification methods.
   */

  @Override
  public void onModification(AbstractEvent<? extends HasMetadata> event) {
    var pre = event.getPriorResource();
    var cur = event.getResource();
    /*
     * Process the modification on the resource.
     */
    if (pre instanceof Export) {
      onModification((Export) pre, (Export) cur);
    } else if (pre instanceof Import) {
      onModification((Import) pre, (Import) cur);
    }
  }

  private void onModification(Export pre, Export cur) {
    /*
     * Clear the export from the subscription board and build the imports list.
     */
    var targets = subscriptionBoard.remove(cur);
    var imports = matchImports(cur);
    /*
     * Update the subscription board.
     */
    imports.values().stream()
        .flatMap(Collection::stream)
        .forEach(
            imp -> {
              LOGGER.info(
                  "BROKER Export \"{}\" - Import \"{}\"",
                  cur.getMetadata().getName(),
                  imp.getMetadata().getName());
              subscriptionBoard.add(cur, imp);
            });
    /*
     * Send a notification to the owning PEs.
     */
    sendNotifications(cur, imports, targets);
  }

  private void onModification(Import pre, Import cur) {
    /*
     * Clear the import entry from subscription board and build the exports list.
     */
    var targets = subscriptionBoard.remove(cur);
    var exports = matchExports(cur);
    /*
     * Register imports to existing exports.
     */
    exports.values().stream()
        .flatMap(Collection::stream)
        .forEach(
            exp -> {
              LOGGER.info(
                  "BROKER Import \"{}\" - Export \"{}\"",
                  cur.getMetadata().getName(),
                  exp.getMetadata().getName());
              subscriptionBoard.add(exp, cur);
            });
    /*
     * Send a notification to the owning PEs.
     */
    sendNotifications(cur, exports, targets);
  }

  /*
   * Deletion methods.
   */

  @Override
  public void onDeletion(AbstractEvent<? extends HasMetadata> event) {
    var resource = event.getResource();
    if (resource instanceof Export) {
      onDeletion((Export) resource);
    } else if (resource instanceof Import) {
      onDeletion((Import) resource);
    }
  }

  private void onDeletion(Export exp) {
    var targets = subscriptionBoard.remove(exp);
    var imports = matchImports(exp);
    sendNotifications(exp, imports, targets);
  }

  private void onDeletion(Import imp) {
    var targets = subscriptionBoard.remove(imp);
    var exports = matchExports(imp);
    sendNotifications(imp, exports, targets);
  }
}
