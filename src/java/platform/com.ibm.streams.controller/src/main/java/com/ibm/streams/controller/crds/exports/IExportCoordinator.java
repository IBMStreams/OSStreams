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

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.events.IEventConsumerDelegate;
import com.ibm.streams.platform.services.StreamPropertiesType;
import com.ibm.streams.platform.services.StreamPropertyNamesType;
import com.ibm.streams.platform.services.StreamPropertyType;

public interface IExportCoordinator extends IEventConsumerDelegate<Export> {

  void addExportProperty(Export exp, StreamPropertyType property);

  void addExportProperties(Export exp, StreamPropertiesType properties);

  void setExportProperty(Export exp, StreamPropertyType property);

  void setExportProperties(Export exp, StreamPropertiesType properties);

  void removeExportProperty(Export exp, String name);

  void removeExportProperties(Export exp, StreamPropertyNamesType names);

  void updateExport(Job job, Export exp);
}
