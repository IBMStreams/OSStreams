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
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;

public class CompositeOperatorDefinition {
  private final BigInteger index;
  private final String kind;
  private final BigInteger toolkitIndex;
  private final transient List<Annotation> annotations;
  private transient ThreadingModel threadingModel = null;

  public CompositeOperatorDefinition(BigInteger index, String kind, BigInteger toolkitIndex) {
    super();
    this.index = index;
    this.kind = kind;
    this.toolkitIndex = toolkitIndex;
    this.annotations = new ArrayList<>();
  }

  public void addAnnotation(Annotation annotation) {
    if (annotation.getTag().equals("threading")) {
      this.threadingModel = new ThreadingModel(annotation);
    }
    this.annotations.add(annotation);
  }

  public ThreadingModel getThreadingModel() {
    return threadingModel;
  }

  public List<Annotation> getAnnotations() {
    return annotations;
  }
}
