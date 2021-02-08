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

package com.ibm.streams.instance.sam.applicationModel;

import java.io.InputStream;
import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBElement;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Unmarshaller;

public class ApplicationModelFactory {

  private static JAXBContext jc4000;
  private static JAXBContext jc4200;

  public static synchronized com.ibm.streams.platform.services.v4000.ApplicationType
      createV4000ApplicationModel(InputStream iStream) throws JAXBException {
    if (jc4000 == null) {
      jc4000 = JAXBContext.newInstance(com.ibm.streams.platform.services.v4000.ObjectFactory.class);
    }
    Unmarshaller um = jc4000.createUnmarshaller();
    JAXBElement<com.ibm.streams.platform.services.v4000.ApplicationSetType> jbe =
        (JAXBElement<com.ibm.streams.platform.services.v4000.ApplicationSetType>)
            um.unmarshal(iStream);
    com.ibm.streams.platform.services.v4000.ApplicationSetType app = jbe.getValue();
    com.ibm.streams.platform.services.v4000.ApplicationType adlModel = app.getApplication().get(0);
    return adlModel;
  }

  public static synchronized com.ibm.streams.platform.services.v4200.ApplicationType
      createV4200ApplicationModel(InputStream iStream) throws JAXBException {
    if (jc4200 == null) {
      jc4200 = JAXBContext.newInstance(com.ibm.streams.platform.services.v4200.ObjectFactory.class);
    }
    Unmarshaller um = jc4200.createUnmarshaller();
    JAXBElement<com.ibm.streams.platform.services.v4200.ApplicationSetType> jbe =
        (JAXBElement<com.ibm.streams.platform.services.v4200.ApplicationSetType>)
            um.unmarshal(iStream);
    com.ibm.streams.platform.services.v4200.ApplicationSetType app = jbe.getValue();
    com.ibm.streams.platform.services.v4200.ApplicationType adlModel =
        app.getSplApplication().get(0);
    return adlModel;
  }
}
