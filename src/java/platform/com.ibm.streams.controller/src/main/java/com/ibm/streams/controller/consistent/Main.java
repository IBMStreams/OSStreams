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

package com.ibm.streams.controller.consistent;

import com.ibm.streams.controller.consistent.rest.ConsistentRegionNotificationBoard;
import com.ibm.streams.controller.consistent.rest.ConsistentRegionResourceConfig;
import com.ibm.streams.controller.consistent.utils.ConsistentRegionOperatorMetric;
import com.ibm.streams.controller.crds.cros.ConsistentRegionOperator;
import com.ibm.streams.controller.crds.cros.ConsistentRegionOperatorFactory;
import com.ibm.streams.controller.crds.cros.ConsistentRegionOperatorList;
import com.ibm.streams.controller.crds.cros.DoneableConsistentRegionOperator;
import com.ibm.streams.controller.crds.crs.ConsistentRegionFactory;
import com.ibm.streams.controller.crds.crs.ConsistentRegionStore;
import com.ibm.streams.controller.crds.crs.consistent.ConsistentRegionController;
import com.ibm.streams.controller.crds.pes.ProcessingElementStore;
import com.ibm.streams.controller.crds.pes.consistent.ProcessingElementController;
import com.ibm.streams.controller.k8s.pods.PodStore;
import com.ibm.streams.controller.k8s.pods.crs.PodController;
import io.fabric8.kubernetes.client.DefaultKubernetesClient;
import io.fabric8.kubernetes.client.KubernetesClient;
import io.fabric8.kubernetes.client.KubernetesClientException;
import java.io.IOException;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import javax.ws.rs.core.UriBuilder;
import lombok.var;
import org.apache.log4j.Level;
import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;
import org.glassfish.jersey.jdkhttp.JdkHttpServerFactory;
import org.slf4j.bridge.SLF4JBridgeHandler;

public class Main {

  private static final Lock lock = new ReentrantLock();
  private static final Condition terminated = lock.newCondition();

  private static final int port = 10000;
  private static final String host = "http://0.0.0.0/";

  public static void main(String[] args) {
    /*
     * Register a shutdown hook.
     */
    Runtime.getRuntime()
        .addShutdownHook(
            new Thread(
                () -> {
                  lock.lock();
                  terminated.signal();
                  lock.unlock();
                }));
    /*
     * Send JUL log events to SLF4J.
     */
    SLF4JBridgeHandler.removeHandlersForRootLogger();
    SLF4JBridgeHandler.install();
    /*
     * Set the JUL loggers' to the minimum verbosity.
     */
    var juls = java.util.logging.LogManager.getLogManager().getLoggerNames();
    while (juls.hasMoreElements()) {
      var logger = java.util.logging.Logger.getLogger(juls.nextElement());
      logger.setLevel(java.util.logging.Level.OFF);
    }
    /*
     * Main try block.
     */
    try {
      /*
       * Grab the controller namespace environment variable.
       */
      var myNs = System.getenv("MY_POD_NAMESPACE");
      if (myNs == null) {
        myNs = "default";
      }

      var jobName = System.getenv("STREAMS_JOB_NAME");
      var logLevel = System.getenv("STREAMS_LOG_LEVEL");
      var jobId = System.getenv("STREAMS_JOB_ID");
      var generationId = System.getenv("STREAMS_GENERATION_ID");
      var numRegions = Integer.parseInt(System.getenv("NUM_CONSISTENT_REGIONS"));

      /*
       * Grab a new Kube client.
       */
      final KubernetesClient client = new DefaultKubernetesClient();

      /*
       * Update the loggers.
       */
      var loggers = LogManager.getCurrentLoggers();
      while (loggers.hasMoreElements()) {
        Logger logger = (Logger) loggers.nextElement();
        logger.setLevel(Level.toLevel(logLevel));
      }

      /*
       * Create the consistent region store and factory
       */
      var crStore = new ConsistentRegionStore();
      var crFactory = new ConsistentRegionFactory(client);

      ConsistentRegionOperatorFactory croFactory = new ConsistentRegionOperatorFactory(client);
      String croName = jobName + "-consistent-region-operator";
      ConsistentRegionOperator consistentRegionOperator =
          client
              .customResources(
                  croFactory.getContext(),
                  ConsistentRegionOperator.class,
                  ConsistentRegionOperatorList.class,
                  DoneableConsistentRegionOperator.class)
              .inNamespace(myNs)
              .withName(croName)
              .get();
      boolean isRestart = false;
      if (consistentRegionOperator.getSpec().isHasStarted()) {
        isRestart = true;
      } else {
        croFactory.updateConsistentRegionOperator(consistentRegionOperator, true);
      }

      /*
       * Create the processing element store
       */
      var peStore = new ProcessingElementStore();

      /*
       * Create the pod store and factory.
       */
      var podStore = new PodStore();

      /*
       * Create the consistent region controllers (one per job that has consistent regons).
       */
      var consistentRegionOperatorMetric = new ConsistentRegionOperatorMetric();

      var crNotificationBoard = new ConsistentRegionNotificationBoard();
      var crController =
          new ConsistentRegionController(
              isRestart,
              client,
              crFactory,
              crStore,
              peStore,
              podStore,
              crNotificationBoard,
              consistentRegionOperatorMetric,
              myNs,
              jobName,
              jobId,
              numRegions);
      var peMonitor = new ProcessingElementController(client, crController, peStore, myNs, jobName);
      var podMonitor = new PodController(client, crController, podStore, peStore, myNs, jobName);
      var baseUri = UriBuilder.fromUri(host).port(port).build();
      var config =
          new ConsistentRegionResourceConfig(
              crController, crNotificationBoard, jobName, crStore, numRegions);
      JdkHttpServerFactory.createHttpServer(baseUri, config);

      /*
       * Start the controllers.
       */
      lock.lock();
      crController.start();
      peMonitor.start();
      podMonitor.start();
      terminated.await();
      lock.unlock();
      /*
       * Close the controllers upon termination.
       */
      crController.close();
      peMonitor.close();
      podMonitor.close();
    } catch (IOException | KubernetesClientException | InterruptedException e) {
      e.printStackTrace();
    }
  }
}
