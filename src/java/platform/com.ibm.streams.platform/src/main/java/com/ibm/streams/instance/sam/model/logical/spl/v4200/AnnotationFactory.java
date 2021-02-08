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

package com.ibm.streams.instance.sam.model.logical.spl.v4200;

import com.ibm.streams.instance.sam.model.logical.spl.LogicalAnnotationPrimitiveValue;
import com.ibm.streams.instance.sam.model.logical.spl.SPLLogicalModel;
import com.ibm.streams.instance.sam.model.topology.Annotation;
import com.ibm.streams.instance.sam.model.topology.AnnotationKeyValue;
import com.ibm.streams.instance.sam.model.topology.AnnotationListValue;
import com.ibm.streams.instance.sam.model.topology.AnnotationPrimitiveValue;
import com.ibm.streams.instance.sam.model.topology.AnnotationTupleValue;
import com.ibm.streams.instance.sam.model.topology.AnnotationValue;
import com.ibm.streams.platform.services.v4200.SplAnnotationValueType;
import java.util.ArrayList;
import java.util.List;

public class AnnotationFactory {

  public static Annotation createAnnotation(
      String tag,
      boolean isInternal,
      List<SplAnnotationValueType> splValues,
      SPLLogicalModel model) {
    List<AnnotationKeyValue> keyValues = new ArrayList<>();
    if (tag.compareTo("view") == 0) {
      // We have to deal specially with the view annotation in order to
      // get it to conform to the new structure
      for (SplAnnotationValueType annotValue : splValues) {
        String key = annotValue.getKey();
        if (key.compareTo("filter") == 0) {
          List<AnnotationKeyValue> filterKeyValues = new ArrayList<>();
          String filter = annotValue.getValue();
          filter = filter.substring(1, filter.length() - 1);
          String[] keyVals = filter.split(",");
          for (String s : keyVals) {
            String[] keyVal = s.split("=");
            assert (keyVal.length == 2);
            String filterKey = keyVal[0].trim();
            String filterValue = keyVal[1].trim();
            filterValue = filterValue.substring(1, filterValue.length() - 1);
            filterKeyValues.add(
                new AnnotationKeyValue(filterKey, new AnnotationPrimitiveValue(filterValue)));
          }
          AnnotationTupleValue v = new AnnotationTupleValue(filterKeyValues);
          keyValues.add(new AnnotationKeyValue(key, v));
        } else if (key.compareTo("attributes") == 0) {
          List<AnnotationValue> attrs = new ArrayList<>();
          String attrValue = annotValue.getValue();
          attrValue = attrValue.substring(1, attrValue.length() - 1);
          String[] attributes = attrValue.split(",");

          for (String s : attributes) {
            String attr = s.trim();
            // Strip off the surrounding quotes
            attr = attr.substring(1, attr.length() - 1);
            attrs.add(new AnnotationPrimitiveValue(attr));
          }
          AnnotationValue value = new AnnotationListValue(attrs);
          keyValues.add(new AnnotationKeyValue(key, value));
        } else {
          AnnotationPrimitiveValue value =
              new LogicalAnnotationPrimitiveValue(
                  annotValue.getValue(), annotValue.getSubmissionTimeValueIndex(), model);
          keyValues.add(new AnnotationKeyValue(key, value));
        }
      }
    } else if (tag.compareTo("consistentRegionEntry") == 0) {
      ArrayList<AnnotationValue> autonomousPorts = new ArrayList<>();
      for (SplAnnotationValueType annotValue : splValues) {
        String key = annotValue.getKey();
        if (key.compareTo("autonomousInputPort") == 0) {
          autonomousPorts.add(
              new LogicalAnnotationPrimitiveValue(
                  annotValue.getValue(), annotValue.getSubmissionTimeValueIndex(), model));
        } else {
          AnnotationValue value =
              new LogicalAnnotationPrimitiveValue(
                  annotValue.getValue(), annotValue.getSubmissionTimeValueIndex(), model);
          keyValues.add(new AnnotationKeyValue(key, value));
        }
      }
      if (autonomousPorts.size() > 0) {
        keyValues.add(
            new AnnotationKeyValue(
                "autonomousInputPort", new AnnotationListValue(autonomousPorts)));
      }
    } else {
      for (SplAnnotationValueType annotValue : splValues) {
        String key = annotValue.getKey();
        AnnotationValue value =
            new LogicalAnnotationPrimitiveValue(
                annotValue.getValue(), annotValue.getSubmissionTimeValueIndex(), model);
        keyValues.add(new AnnotationKeyValue(key, value));
      }
    }
    return new Annotation(tag, isInternal, keyValues);
  }
}
