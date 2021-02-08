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

package com.ibm.streams.application.models.spl.logical;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import java.util.List;

public class SPLLogicalModel {

  private final List<TupleTypeDef> tupleTypeDefs = null;
  private final List<CompositeOperDefinition> compositeOperDefs = null;
  private final List<PrimitiveOperDefinition> primitiveOperDefs = null;
  private final List<ParallelRegion> parallelRegions = null;
  private final List<Toolkit> toolkits = null;
  private final List<URI> sourceUris = null;

  private final CompositeOperator mainComposite = null;

  private SPLLogicalModel() {}

  // A method used for testing
  public String toJSON() {
    GsonBuilder gsonBuilder = new GsonBuilder();
    gsonBuilder.serializeNulls();
    // gsonBuilder.setPrettyPrinting();
    gsonBuilder.registerTypeAdapter(CompositeOperator.class, new CompositeOperatorSerializer());
    gsonBuilder.registerTypeAdapter(PrimitiveOperator.class, new PrimitiveOperatorSerializer());
    gsonBuilder.registerTypeAdapter(TupleType.class, new TupleTypeSerializer());
    gsonBuilder.registerTypeAdapter(PrimitiveType.class, new PrimitiveTypeSerializer());
    gsonBuilder.registerTypeAdapter(MapType.class, new MapTypeSerializer());
    gsonBuilder.registerTypeAdapter(SetType.class, new SetTypeSerializer());
    gsonBuilder.registerTypeAdapter(ListType.class, new ListTypeSerializer());
    gsonBuilder.registerTypeAdapter(OptionalType.class, new OptionalTypeSerializer());
    gsonBuilder.registerTypeAdapter(Annotation.class, new AnnnotationSerializer());
    gsonBuilder.registerTypeAdapter(
        PrimitiveAnnotationValue.class, new PrimitiveAnnotationValueSerializer());
    gsonBuilder.registerTypeAdapter(ListAnnotationValue.class, new ListAnnotationValueSerializer());
    gsonBuilder.registerTypeAdapter(
        TupleAnnotationValue.class, new TupleAnnotationValueSerializer());
    gsonBuilder.registerTypeAdapter(PrimitiveInputPort.class, new PrimitiveInputPortSerializer());
    gsonBuilder.registerTypeAdapter(PrimitiveOutputPort.class, new PrimitiveOutputPortSerializer());
    Gson gson = gsonBuilder.create();
    return gson.toJson(this);
  }

  public static SPLLogicalModel fromJSON(String json1) {
    GsonBuilder gsonBuilder = new GsonBuilder();
    gsonBuilder.serializeNulls();
    gsonBuilder.registerTypeAdapter(
        com.ibm.streams.application.models.spl.logical.PrimitiveOperator.class,
        new PrimitiveOperatorDeserializer());
    gsonBuilder.registerTypeAdapter(
        com.ibm.streams.application.models.spl.logical.Type.class, new TypeDeserializer());
    gsonBuilder.registerTypeAdapter(AnnotationValue.class, new AnnotationValueDeserializer());
    gsonBuilder.registerTypeAdapter(Annotation.class, new AnnotationDeserializer());
    Gson gson = gsonBuilder.create();
    return gson.fromJson(
        json1, com.ibm.streams.application.models.spl.logical.SPLLogicalModel.class);
  }

  public List<TupleTypeDef> getTupleTypeDefs() {
    return tupleTypeDefs;
  }

  public List<CompositeOperDefinition> getCompositeOperDefs() {
    return compositeOperDefs;
  }

  public List<PrimitiveOperDefinition> getPrimitiveOperDefs() {
    return primitiveOperDefs;
  }

  public CompositeOperator getMainComposite() {
    return mainComposite;
  }

  public List<ParallelRegion> getParallelRegions() {
    return parallelRegions;
  }

  public List<Toolkit> getToolkits() {
    return toolkits;
  }

  public List<URI> getSourceURIs() {
    return sourceUris;
  }
}
