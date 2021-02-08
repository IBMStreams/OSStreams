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

package com.ibm.streams.controller.k8s.configmaps;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_LABEL_VALUE;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CONTENT_ID_ANNOTATION_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_PE_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_SVC_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_SVC_PE_LABEL_VALUE;

import com.ibm.streams.controller.events.GenericEventQueueConsumer;
import com.ibm.streams.controller.instance.jobs.JobUtils;
import com.ibm.streams.controller.k8s.utils.ConfigMapUtils;
import com.ibm.streams.platform.services.AugmentedPEType;
import io.fabric8.kubernetes.api.model.ConfigMap;
import io.fabric8.kubernetes.client.KubernetesClient;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.util.Optional;
import javax.xml.XMLConstants;
import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Unmarshaller;
import javax.xml.transform.stream.StreamSource;
import javax.xml.validation.SchemaFactory;
import lombok.var;
import org.microbean.kubernetes.controller.AbstractEvent;
import org.microbean.kubernetes.controller.Controller;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.xml.sax.SAXException;

public class ConfigMapController extends GenericEventQueueConsumer<ConfigMap> {

  private static final Logger LOGGER = LoggerFactory.getLogger(ConfigMapController.class);

  private final ConfigMapFactory factory;
  private final ConfigMapCache cache;
  private final Controller<ConfigMap> controller;

  private Unmarshaller unmarshaller;

  public ConfigMapController(
      KubernetesClient client,
      ConfigMapStore store,
      ConfigMapFactory factory,
      ConfigMapCache cache,
      String ns) {
    /*
     * Super constructor.
     */
    super(store);
    /*
     * Save the client and the store.
     */
    this.factory = factory;
    this.cache = cache;
    /*
     * Create the controller. NOTE(xrg) By design we are only watching ConfigMaps that pertain
     * to ProcessingElements. Any other ConfigMap are ignored.
     */
    controller =
        new Controller<>(
            client
                .configMaps()
                .inNamespace(ns)
                .withLabel(STREAMS_APP_LABEL_KEY, STREAMS_APP_LABEL_VALUE)
                .withLabel(STREAMS_SVC_LABEL_KEY, STREAMS_SVC_PE_LABEL_VALUE),
            this);
    /*
     * Create the unmarshaller for the AADLs.
     */
    try {
      var context = JAXBContext.newInstance(AugmentedPEType.class);
      var file = ConfigMapController.class.getResource("/schema/SAM/augmentedApplicationModel.xsd");
      var schemaFactory = SchemaFactory.newInstance(XMLConstants.W3C_XML_SCHEMA_NS_URI);
      var schema = schemaFactory.newSchema(file);
      this.unmarshaller = context.createUnmarshaller();
      this.unmarshaller.setSchema(schema);
    } catch (SAXException | JAXBException e) {
      e.printStackTrace();
      this.unmarshaller = null;
    }
  }

  private Optional<AugmentedPEType> parseAadlFrom(ConfigMap cm) {
    /*
     * Check if we have a marshaller to work with.
     */
    if (this.unmarshaller != null) {
      try {
        /*
         * Grab the AADL content.
         */
        var jobName = cm.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
        var peId = cm.getMetadata().getLabels().get(STREAMS_PE_LABEL_KEY);
        var peName = JobUtils.getProcessingElementName(jobName, peId);
        var xml = ConfigMapUtils.getAadl(cm);
        /*
         * Unmarshall the AADL.
         */
        var bis = new ByteArrayInputStream(xml.getBytes());
        var src = new StreamSource(bis);
        return Optional.of(unmarshaller.unmarshal(src, AugmentedPEType.class).getValue());
      } catch (JAXBException ignored) {
      }
    }
    /*
     * In any case of error, return an empty optional.
     */
    return Optional.empty();
  }

  @Override
  public void onAddition(AbstractEvent<? extends ConfigMap> event) {
    /*
     * Register the config map.
     */
    var cm = event.getResource();
    LOGGER.debug(
        "ADD: {}/{}",
        cm.getMetadata().getName(),
        cm.getMetadata().getAnnotations().get(STREAMS_CONTENT_ID_ANNOTATION_KEY));
    factory.registerConfigMapName(cm.getMetadata().getName());
    /*
     * Cache the AADL.
     */
    parseAadlFrom(cm).ifPresent(aadl -> cache.put(cm.getMetadata().getUid(), aadl));
  }

  @Override
  public void onModification(AbstractEvent<? extends ConfigMap> event) {
    /*
     * Print some information.
     */
    var pre = event.getPriorResource();
    var cur = event.getResource();
    LOGGER.debug(
        "MOD: {}/{}/{}",
        pre.getMetadata().getName(),
        pre.getMetadata().getAnnotations().get(STREAMS_CONTENT_ID_ANNOTATION_KEY),
        cur.getMetadata().getAnnotations().get(STREAMS_CONTENT_ID_ANNOTATION_KEY));
    /*
     * Update the AADL cache.
     */
    parseAadlFrom(cur).ifPresent(aadl -> cache.put(cur.getMetadata().getUid(), aadl));
  }

  /*
   * NOTE(xrg) We do not attempt to recreate the ConfigMap if it was mistakenly deleted.
   * We can try to think about a good way to handle this, but the logic may require too
   * much synchronization.
   */

  @Override
  public void onDeletion(AbstractEvent<? extends ConfigMap> event) {
    /*
     * Unregister the config map.
     */
    var cm = event.getResource();
    LOGGER.debug(
        "DEL: {}/{}",
        cm.getMetadata().getName(),
        cm.getMetadata().getAnnotations().get(STREAMS_CONTENT_ID_ANNOTATION_KEY));
    factory.unregisterConfigMapName(cm.getMetadata().getName());
    /*
     * Delete the entry from the cache.
     */
    cache.remove(cm.getMetadata().getUid());
  }

  public void start() throws IOException {
    controller.start();
  }

  public void close() throws IOException {
    controller.close();
  }
}
