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

package com.ibm.streams.controller.instance.rest.providers;

import com.ibm.streams.platform.services.ObjectFactory;
import com.ibm.streams.platform.services.StreamPropertyNamesType;
import java.io.IOException;
import java.io.OutputStream;
import java.lang.annotation.Annotation;
import java.lang.reflect.Type;
import javax.ws.rs.Produces;
import javax.ws.rs.WebApplicationException;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.MultivaluedMap;
import javax.ws.rs.ext.MessageBodyWriter;
import javax.ws.rs.ext.Provider;
import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Marshaller;

@Provider
@Produces(MediaType.APPLICATION_XML)
public class StreamPropertyNamesTypeWriter implements MessageBodyWriter<StreamPropertyNamesType> {

  private JAXBContext context = JAXBContext.newInstance(StreamPropertyNamesType.class);
  private Marshaller marshaller = context.createMarshaller();
  private ObjectFactory factory = new ObjectFactory();

  public StreamPropertyNamesTypeWriter() throws JAXBException {}

  @Override
  public boolean isWriteable(
      Class<?> aClass, Type type, Annotation[] annotations, MediaType mediaType) {
    return aClass == StreamPropertyNamesType.class;
  }

  @Override
  public void writeTo(
      StreamPropertyNamesType streamPropertyNamesType,
      Class<?> aClass,
      Type type,
      Annotation[] annotations,
      MediaType mediaType,
      MultivaluedMap<String, Object> multivaluedMap,
      OutputStream outputStream)
      throws IOException, WebApplicationException {
    try {
      marshaller.marshal(factory.createStreamPropertyNames(streamPropertyNamesType), outputStream);
    } catch (JAXBException e) {
      e.printStackTrace();
      throw new IOException(e.getMessage());
    }
  }
}
