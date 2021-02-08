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

package com.ibm.streams.controller.instance.rest;

import com.ibm.streams.controller.instance.IStreamsInstance;
import com.ibm.streams.controller.instance.rest.providers.ImportedStreamsTypeWriter;
import com.ibm.streams.controller.instance.rest.providers.StreamPropertiesTypeWriter;
import com.ibm.streams.controller.instance.rest.providers.StreamPropertyNamesTypeWriter;
import com.ibm.streams.controller.instance.rest.providers.StreamPropertyTypeWriter;
import com.ibm.streams.controller.instance.rest.services.EndpointsService;
import com.ibm.streams.controller.instance.rest.services.StateService;
import com.ibm.streams.controller.instance.rest.services.SubscriptionsService;
import org.glassfish.jersey.jackson.JacksonFeature;
import org.glassfish.jersey.server.ResourceConfig;

public class StreamsResourceConfig extends ResourceConfig {

  public StreamsResourceConfig(IStreamsInstance instance, String ns) {
    register(JacksonFeature.class);
    register(EndpointsService.class);
    register(SubscriptionsService.class);
    register(StateService.class);
    register(new StreamsBinder(instance, ns));
    register(IllegalArgumentExceptionProvider.class);
    register(NotFoundExceptionProvider.class);
    register(ImportedStreamsTypeWriter.class);
    register(StreamPropertyNamesTypeWriter.class);
    register(StreamPropertiesTypeWriter.class);
    register(StreamPropertyTypeWriter.class);
  }
}
