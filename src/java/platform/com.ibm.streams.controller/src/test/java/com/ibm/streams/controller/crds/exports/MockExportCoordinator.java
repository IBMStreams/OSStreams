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

package com.ibm.streams.controller.crds.exports;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.instance.sam.SamUtilFunctions;
import com.ibm.streams.instance.sam.model.topology.StreamProperty;
import com.ibm.streams.platform.services.StreamPropertiesType;
import com.ibm.streams.platform.services.StreamPropertyNamesType;
import com.ibm.streams.platform.services.StreamPropertyType;
import java.util.stream.Collectors;
import lombok.var;
import org.microbean.kubernetes.controller.AbstractEvent;

public class MockExportCoordinator implements IExportCoordinator {

  @Override
  public void addExportProperty(Export exp, StreamPropertyType property) {
    /*
     * Check if the export is by properties.
     */
    if (exp.getSpec().getType() != EExportedStreamType.ByProperties) {
      throw new IllegalArgumentException("Export has no properties");
    }
    /*
     * Build the properties maps.
     */
    var epMap =
        exp.getSpec().getStream().getProperties().stream()
            .collect(Collectors.toMap(StreamProperty::getName, e -> e));
    /*
     * Check that no new property exists and add the value.
     */
    if (epMap.containsKey(property.getName())) {
      throw new IllegalArgumentException("Property already exist: " + property.getName());
    }
    /*
     * Duplicate the spec and add the values.
     */
    var p = SamUtilFunctions.getStreamPropertyFromStreamPropertyType(property);
    exp.getSpec().getStream().getProperties().add(p);
  }

  @Override
  public void addExportProperties(Export exp, StreamPropertiesType properties) {
    /*
     * Check if the export is by properties.
     */
    if (exp.getSpec().getType() != EExportedStreamType.ByProperties) {
      throw new IllegalArgumentException("Export has no properties");
    }
    /*
     * Build the properties maps.
     */
    var epMap =
        exp.getSpec().getStream().getProperties().stream()
            .collect(Collectors.toMap(StreamProperty::getName, e -> e));
    /*
     * Check that no new property exists and add the value.
     */
    for (var prop : properties.getProperty()) {
      if (epMap.containsKey(prop.getName())) {
        throw new IllegalArgumentException("Property already exist: " + prop.getName());
      }
    }
    /*
     * Duplicate the spec and add the values.
     */
    var p = SamUtilFunctions.getStreamPropertyFromStreamPropertiesType(properties);
    exp.getSpec().getStream().getProperties().addAll(p);
  }

  @Override
  public void setExportProperty(Export exp, StreamPropertyType property) {
    /*
     * Check if the export is by properties.
     */
    if (exp.getSpec().getType() != EExportedStreamType.ByProperties) {
      throw new IllegalArgumentException("Export has no properties");
    }
    /*
     * Build the properties maps.
     */
    var epMap =
        exp.getSpec().getStream().getProperties().stream()
            .collect(Collectors.toMap(StreamProperty::getName, e -> e));
    /*
     * Check that all new properties exist and overwrite the value.
     */
    if (!epMap.containsKey(property.getName())) {
      throw new IllegalArgumentException("Unknown property: " + property.getName());
    }
    var p = SamUtilFunctions.getStreamPropertyFromStreamPropertyType(property);
    epMap.put(property.getName(), p);
    /*
     * Duplicate the spec and overwrite the values.
     */
    exp.getSpec().getStream().getProperties().clear();
    exp.getSpec().getStream().getProperties().addAll(epMap.values());
  }

  @Override
  public void setExportProperties(Export exp, StreamPropertiesType properties) {
    /*
     * Check if the export is by properties.
     */
    if (exp.getSpec().getType() != EExportedStreamType.ByProperties) {
      throw new IllegalArgumentException("Export has no properties");
    }
    /*
     * Build the properties maps.
     */
    var epMap =
        exp.getSpec().getStream().getProperties().stream()
            .collect(Collectors.toMap(StreamProperty::getName, e -> e));
    /*
     * Check that all new properties exist and overwrite the value.
     */
    for (var prop : properties.getProperty()) {
      if (!epMap.containsKey(prop.getName())) {
        throw new IllegalArgumentException("Unknown property: " + prop.getName());
      }
      var p = SamUtilFunctions.getStreamPropertyFromStreamPropertyType(prop);
      epMap.put(prop.getName(), p);
    }
    /*
     * Duplicate the spec and overwrite the values.
     */
    exp.getSpec().getStream().getProperties().clear();
    exp.getSpec().getStream().getProperties().addAll(epMap.values());
  }

  @Override
  public void removeExportProperty(Export exp, String name) {
    /*
     * Check if the export is by properties.
     */
    if (exp.getSpec().getType() != EExportedStreamType.ByProperties) {
      throw new IllegalArgumentException("Export has no properties");
    }
    /*
     * Remove the property.
     */
    if (!exp.getSpec().getStream().getProperties().removeIf(e -> e.getName().equals(name))) {
      throw new IllegalArgumentException("Unknown property: " + name);
    }
    if (exp.getSpec().getStream().getProperties().isEmpty()) {
      exp.getSpec().getStream().setProperties(null);
    }
  }

  @Override
  public void removeExportProperties(Export exp, StreamPropertyNamesType names) {
    names.getName().forEach(n -> removeExportProperty(exp, n.getName()));
  }

  @Override
  public void updateExport(Job job, Export exp) {
    exp.getMetadata()
        .getLabels()
        .put(STREAMS_GENERATION_LABEL_KEY, job.getSpec().getGenerationId().toString());
  }

  @Override
  public void onAddition(AbstractEvent<? extends Export> event) {}

  @Override
  public void onModification(AbstractEvent<? extends Export> event) {}

  @Override
  public void onDeletion(AbstractEvent<? extends Export> event) {}
}
