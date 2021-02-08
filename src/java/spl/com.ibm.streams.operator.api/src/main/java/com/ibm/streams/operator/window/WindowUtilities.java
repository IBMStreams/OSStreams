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

package com.ibm.streams.operator.window;

import com.ibm.streams.operator.Attribute;
import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.Tuple;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * Utility methods for windowed input ports.
 *
 * @see StreamWindow
 * @since InfoSphere&reg; Streams Version 2.0.0.3
 */
public abstract class WindowUtilities {

  /**
   * Register a {@code StreamWindowPartitioner} for a {@code StreamWindow} where partitioning is
   * defined by attributes of the input tuples.
   *
   * <p>Any attribute used for partitioning must be one of the following types: {@code
   * Type.MetaType#BSTRING}, {@code Type.MetaType#RSTRING}, {@code Type.MetaType#USTRING}, {@code
   * Type.MetaType#BOOLEAN}, {@code Type.MetaType#INT8}, {@code Type.MetaType#INT16}, {@code
   * Type.MetaType#INT32}, {@code Type.MetaType#INT64}, {@code Type.MetaType#UINT8}, {@code
   * Type.MetaType#UINT16}, {@code Type.MetaType#UINT32}, {@code Type.MetaType#UINT64}.
   *
   * @param window Window being partitioned.
   * @param attributeNames Names of the attributes to partition by.
   * @exception IllegalArgumentException Invalid attributes names or types of attributes.
   * @since InfoSphere&reg; Streams Version 2.0.0.3
   */
  public static void registerAttributePartitioner(
      final StreamWindow<Tuple> window, final String... attributeNames) {
    if (attributeNames == null || attributeNames.length == 0) throw new IllegalArgumentException();

    StreamSchema schema = window.getInputPort().getStreamSchema();

    for (String attributeName : attributeNames) {
      Attribute attr = schema.getAttribute(attributeName);
      if (attr == null) throw new IllegalArgumentException(attributeName);

      switch (attr.getType().getMetaType()) {
        case USTRING:
        case RSTRING:
        case BSTRING:
        case BOOLEAN:
        case INT8:
        case INT16:
        case INT32:
        case INT64:
        case UINT8:
        case UINT16:
        case UINT32:
        case UINT64:
          break;
        default:
          throw new IllegalArgumentException(
              attr.getType().getLanguageType() + " " + attributeName);
      }
    }

    // Single attribute
    if (attributeNames.length == 1) {
      final int attributeIndex = schema.getAttributeIndex(attributeNames[0]);

      window.registerPartitioner(
          new StreamWindowPartitioner<Tuple, Object>() {

            @Override
            public Object getPartition(Tuple tuple) {
              return tuple.getObject(attributeIndex);
            }
          });
      return;
    }

    // Multiple attributes.
    final int[] indexes = new int[attributeNames.length];
    for (int i = 0; i < indexes.length; i++)
      indexes[i] = schema.getAttributeIndex(attributeNames[i]);

    window.registerPartitioner(
        new StreamWindowPartitioner<Tuple, List<Object>>() {

          @Override
          public List<Object> getPartition(Tuple tuple) {
            final List<Object> attrs = new ArrayList<Object>(indexes.length);
            for (int i = 0; i < indexes.length; i++) attrs.add(tuple.getObject(indexes[i]));
            return Collections.unmodifiableList(attrs);
          }
        });
  }
}
