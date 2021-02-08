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

package com.ibm.streams.instance.sam.model.topology;

import static com.google.common.base.Preconditions.checkNotNull;

import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.Validator;
import com.ibm.streams.instance.sam.persistence.IXmlSerializer;
import com.ibm.streams.platform.services.SharedObjectType;

public class SharedObject implements Validator, IXmlSerializer<SharedObjectType> {

  private String wrapper;
  private String sharedObjectUri;

  public SharedObject(String sharedObjectUri, String wrapper) {
    this.sharedObjectUri = sharedObjectUri;
    this.wrapper = wrapper;
  }

  public String getWrapper() {
    return wrapper;
  }

  public void setWrapper(String wrapper) {
    this.wrapper = wrapper;
  }

  public String getSharedObjectUri() {
    return sharedObjectUri;
  }

  public void setSharedObjectUri(String sharedObjectUri) {
    this.sharedObjectUri = sharedObjectUri;
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.line(
        "<sharedObject sharedObjectUri=\"" + sharedObjectUri + "\" wrapper=\"" + wrapper + "\"/>");
    return dumper.toString();
  }

  public SharedObjectType toXml() {
    SharedObjectType xml = new SharedObjectType();
    xml.setWrapper(wrapper); // TODO: Remove
    xml.setSharedObjectUri(sharedObjectUri);
    return xml;
  }

  public void validate() throws Exception {
    checkNotNull(wrapper, "SharedObject - Wrapper not set");
    checkNotNull(sharedObjectUri, "SharedObject - SharedObjectUri not set");
  }
}
