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

package com.ibm.streams.instance.sam;

import com.ibm.streams.platform.services.ObjectFactory;
import java.io.ByteArrayOutputStream;
import java.io.StringReader;
import java.io.UnsupportedEncodingException;
import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBElement;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Marshaller;
import javax.xml.bind.Unmarshaller;
import javax.xml.transform.stream.StreamSource;

public class SAMXmlHelper<T> {

  private static JAXBContext jc = null;
  private static Unmarshaller unmarshaller = null;
  private static Marshaller marshaller = null;

  static synchronized JAXBContext initJc() throws JAXBException {
    if (jc == null) {
      jc = JAXBContext.newInstance(com.ibm.streams.platform.services.ObjectFactory.class);
      unmarshaller = jc.createUnmarshaller();
      marshaller = jc.createMarshaller();
      marshaller.setProperty(Marshaller.JAXB_FORMATTED_OUTPUT, new Boolean(true));
    }
    return jc;
  }

  public static com.ibm.streams.platform.services.ObjectFactory newObjf() {
    return new ObjectFactory();
  }

  public String marshal(JAXBElement<T> obj) throws JAXBException, UnsupportedEncodingException {
    return marshalAsBaos(obj).toString("UTF-8");
  }

  public ByteArrayOutputStream marshalAsBaos(JAXBElement<T> obj) throws JAXBException {
    synchronized (initJc()) {
      ByteArrayOutputStream baos = new ByteArrayOutputStream();
      marshaller.marshal(obj, baos);
      return baos;
    }
  }

  @SuppressWarnings("unchecked")
  public T unmarshal(String xmlstr) throws JAXBException {
    synchronized (initJc()) {
      JAXBElement<T> elem =
          (JAXBElement<T>) unmarshaller.unmarshal(new StreamSource(new StringReader(xmlstr)));
      return elem.getValue();
    }
  }
}
