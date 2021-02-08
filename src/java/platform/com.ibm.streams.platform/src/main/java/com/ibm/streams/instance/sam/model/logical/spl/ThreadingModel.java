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

package com.ibm.streams.instance.sam.model.logical.spl;

import com.ibm.streams.instance.sam.model.topology.Annotation;
import com.ibm.streams.instance.sam.model.topology.AnnotationKeyValue;
import com.ibm.streams.instance.sam.model.topology.AnnotationPrimitiveValue;
import java.util.ArrayList;
import java.util.List;

public class ThreadingModel {

  private final Annotation annotation;

  public ThreadingModel(Annotation annotation) {
    this.annotation = annotation;
  }

  @Override
  public String toString() {
    String model = "automatic";
    String threads = null;
    String elastic = null;
    List<AnnotationKeyValue> kvs = annotation.getKeyValues();
    for (AnnotationKeyValue kv : kvs) {
      if (kv.getKey().equals("model")) {
        model = kv.getValue().toString();
      } else if (kv.getKey().equals("threads")) {
        threads = kv.getValue().toString();
      } else if (kv.getKey().equals("elastic")) {
        elastic = kv.getValue().toString();
      }
    }

    StringBuilder sb = new StringBuilder();
    sb.append("<threading model=\"");
    sb.append(model);
    sb.append("\"");
    if (model.equals("dynamic")) {
      if (threads != null) {
        sb.append(" threads=\"");
        sb.append(threads);
        sb.append("\"");
      }
      if (elastic != null) {
        sb.append(" elastic=\"");
        sb.append(elastic);
        sb.append("\"");
      }
    }
    sb.append("/>");

    return sb.toString();
  }

  public Annotation getAnnotation() {
    return annotation;
  }

  public static ThreadingModel create(String model, String threads, String elastic) {
    List<AnnotationKeyValue> keyValues = new ArrayList<>();
    keyValues.add(new AnnotationKeyValue("model", new AnnotationPrimitiveValue(model)));

    if (threads != null) {
      keyValues.add(new AnnotationKeyValue("threads", new AnnotationPrimitiveValue(threads)));
    }

    if (elastic != null) {
      keyValues.add(new AnnotationKeyValue("elastic", new AnnotationPrimitiveValue(elastic)));
    }

    Annotation annot = new Annotation("threading", false, keyValues);
    return new ThreadingModel(annot);
  }
}
