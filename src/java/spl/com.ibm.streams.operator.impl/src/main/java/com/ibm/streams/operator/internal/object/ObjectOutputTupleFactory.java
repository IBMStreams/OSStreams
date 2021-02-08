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
import com.ibm.streams.operator.internal.ports.TupleFactory;
import com.ibm.streams.operator.internal.runtime.Schema;

public final class ObjectOutputTupleFactory implements TupleFactory<OutputTuple> {

  private final Schema schema;

  public static TupleFactory<OutputTuple> getTupleFactory(Schema schema) {
    return new ObjectOutputTupleFactory(schema);
  }

  private ObjectOutputTupleFactory(Schema schema) {
    this.schema = schema;
  }

  @Override
  public OutputTuple newTuple() {
    return new OpOutputTuple(schema);
  }
}
