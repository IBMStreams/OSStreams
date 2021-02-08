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

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_NAME_ANNOTATION_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_SCOPE_ANNOTATION_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;

import com.ibm.streams.controller.crds.exports.Export;
import com.ibm.streams.controller.crds.imports.Import;
import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.instance.sam.model.InputPort;
import com.ibm.streams.instance.sam.model.OutputPort;
import com.ibm.streams.instance.sam.model.topology.ImportedStreams;
import com.ibm.streams.instance.sam.model.topology.NameBasedImport;
import com.ibm.streams.instance.sam.model.topology.PropertyBasedImport;
import com.ibm.streams.platform.services.ImportedStreamsType;
import java.io.IOException;
import java.math.BigInteger;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.Optional;
import lombok.var;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ImportExportCommons {

  private static final Logger LOGGER = LoggerFactory.getLogger(ImportExportCommons.class);

  private static boolean checkApplicationName(Export exp, NameBasedImport imp) {
    return Optional.ofNullable(imp.getApplicationName())
        .map(e -> e.equals(exp.getMetadata().getAnnotations().get(STREAMS_APP_NAME_ANNOTATION_KEY)))
        .orElse(true);
  }

  private static boolean checkApplicationScope(Export exp, NameBasedImport imp) {
    return Optional.ofNullable(imp.getApplicationScope())
        .map(
            e -> e.equals(exp.getMetadata().getAnnotations().get(STREAMS_APP_SCOPE_ANNOTATION_KEY)))
        .orElse(true);
  }

  static boolean checkApplicationScope(Export exp, PropertyBasedImport imp) {
    return Optional.ofNullable(imp.getApplicationScope())
        .map(
            e -> e.equals(exp.getMetadata().getAnnotations().get(STREAMS_APP_SCOPE_ANNOTATION_KEY)))
        .orElse(true);
  }

  static boolean matchAnyNamedStream(Export exp, Import imp) {
    return imp.getSpec().getStreams().getNameBasedImports().values().stream()
        .filter(v -> ImportExportCommons.checkApplicationName(exp, v))
        .filter(v -> ImportExportCommons.checkApplicationScope(exp, v))
        .anyMatch(v -> v.getStreamName().equals(exp.getSpec().getStream().getName()));
  }

  public static String getExportName(Job job, BigInteger peId, OutputPort op) {
    return job.getMetadata().getName() + "-" + peId + "-" + op.getId();
  }

  public static String getImportName(Job job, BigInteger peId, InputPort ip) {
    return job.getMetadata().getName() + "-" + peId + "-" + ip.getId();
  }

  public static String getPeNameForExport(Export exp) {
    String jobName = exp.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
    BigInteger peId = exp.getSpec().getPeId();
    return jobName + "-" + peId;
  }

  public static String getPeNameForImport(Import imp) {
    String jobName = imp.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
    BigInteger peId = imp.getSpec().getPeId();
    return jobName + "-" + peId;
  }

  public static String getLabelForExport(Export exp) {
    String peName = getPeNameForExport(exp);
    BigInteger portId = exp.getSpec().getPortId();
    return peName + "-" + portId;
  }

  public static String getLabelForImport(Import imp) {
    String peName = getPeNameForImport(imp);
    BigInteger portId = imp.getSpec().getPortId();
    return peName + "-" + portId;
  }

  public static ImportedStreams getImportedStreams(ImportedStreamsType importedStreamsType) {
    var ip = new ImportedStreams();
    /*
     * Convert the ImportStreamsType into ImportedStreams.
     */
    ip.setFilter(importedStreamsType.getFilter());
    if (importedStreamsType.getPropertyBasedImport() != null) {
      var pbi = importedStreamsType.getPropertyBasedImport();
      var value =
          new PropertyBasedImport(
              pbi.getSubscription(),
              pbi.getImportOperName(),
              pbi.getIndex(),
              pbi.getApplicationScope());
      ip.setPropertyBasedImport(value);
    }
    importedStreamsType
        .getNameBasedImport()
        .forEach(
            nbi -> {
              var value =
                  new NameBasedImport(
                      nbi.getApplicationName(),
                      nbi.getImportOperName(),
                      nbi.getStreamName(),
                      nbi.getApplicationScope());
              ip.getNameBasedImports().put(nbi.getIndex(), value);
            });
    return ip;
  }

  static void sendSubscriptionNotification(DatagramSocket socket, String target, String content) {
    /*
     * Check if the socket is valid.
     */
    if (socket == null) {
      return;
    }
    /*
     * Send the datagram notification. It is not possible to do partial reads with UDP, so
     * we need to make sure that the buffer on the other end is big enough.
     */
    try {
      byte[] buffer = content.getBytes();
      InetAddress address = InetAddress.getByName(target);
      DatagramPacket packet = new DatagramPacket(buffer, buffer.length, address, 10000);
      socket.send(packet);
      LOGGER.debug("Routing notification sent to " + target);
    } catch (IOException ignored) {
    }
  }
}
