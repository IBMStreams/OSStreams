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

package com.ibm.streams.mock.server;

import io.fabric8.kubernetes.api.model.HasMetadata;
import io.fabric8.kubernetes.client.server.mock.KubernetesAttributesExtractor;
import io.fabric8.mockwebserver.crud.Attribute;
import io.fabric8.mockwebserver.crud.AttributeSet;
import io.fabric8.mockwebserver.crud.AttributeType;
import io.fabric8.zjsonpatch.internal.guava.Strings;
import java.util.Map;
import lombok.var;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class AttributesExtractor extends KubernetesAttributesExtractor {

  private static final Logger LOGGER = LoggerFactory.getLogger(AttributesExtractor.class);

  private static final String LABEL_KEY_PREFIX = "labels:";

  @Override
  public AttributeSet extract(HasMetadata o) {
    var attrs = new AttributeSet();
    LOGGER.debug(o.toString());
    /*
     * Extract the kind attribute.
     */
    if (!Strings.isNullOrEmpty(o.getKind())) {
      String value = o.getKind().toLowerCase();
      if (o.getApiVersion().startsWith("streams.ibm.com")) {
        switch (o.getKind().toLowerCase()) {
          case "export":
            {
              value = "streamsexport";
              break;
            }
          case "import":
            {
              value = "streamsimport";
              break;
            }
          case "hostpool":
            {
              value = "streamshostpool";
              break;
            }
          case "job":
            {
              value = "streamsjob";
              break;
            }
          case "processingelement":
            {
              value = "streamspe";
              break;
            }
          case "consistentregionoperator":
            {
              value = "streamscro";
              break;
            }
          case "testsuite":
            {
              value = "streamsts";
              break;
            }
          case "consistentregion":
            {
              value = "streamscr";
              break;
            }
          case "parallelregion":
            {
              value = "streamspr";
              break;
            }
        }
      }
      attrs = attrs.add(new Attribute("kind", value));
    }
    /*
     * Extract the name attribute.
     */
    if (!Strings.isNullOrEmpty(o.getMetadata().getName())) {
      attrs = attrs.add(new Attribute(NAME, o.getMetadata().getName()));
    }
    /*
     * Extract the namespace attribute.
     */
    if (!Strings.isNullOrEmpty(o.getMetadata().getNamespace())) {
      attrs = attrs.add(new Attribute(NAMESPACE, o.getMetadata().getNamespace()));
    }
    /*
     * Extract the labels attribute.
     */
    if (o.getMetadata().getLabels() != null) {
      for (Map.Entry<String, String> label : o.getMetadata().getLabels().entrySet()) {
        AttributeType type = AttributeType.WITH;
        if (label.getKey().equals("streams.ibm.com/generation")) {
          type = AttributeType.EXISTS;
        }
        attrs = attrs.add(new Attribute(LABEL_KEY_PREFIX + label.getKey(), label.getValue(), type));
      }
    }
    /*
     * Return the attributes.
     */
    LOGGER.debug(attrs.toString());
    return attrs;
  }
}
