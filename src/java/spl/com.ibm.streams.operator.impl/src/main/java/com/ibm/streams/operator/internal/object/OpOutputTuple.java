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

package com.ibm.streams.operator.internal.object;

import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.internal.runtime.Schema;

/** OutputTuple object implementation for the com.ibm.streams.operator package. */
public final class OpOutputTuple extends ObjectOutputTuple implements OutputTuple {

  private static final long serialVersionUID = -7908034389295913228L;

  public OpOutputTuple(Schema schema) {
    super(schema);
  }

  /**
   * For each attribute in this tuple's schema see if the other tuple contains a matching Attribute
   * and if so set this tuple's attribute from the other tuple's attribute.
   */
  @Override
  public final void assign(final Tuple sourceTuple) {
    getSchema().copyIntoValues(values, sourceTuple);
  }
}
