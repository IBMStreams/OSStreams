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

package com.ibm.streams.operator.internal.compile;

import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.internal.ports.RuntimeStream;
import com.ibm.streams.operator.internal.ports.StreamAction;
import com.ibm.streams.operator.internal.ports.TupleFactory;

/**
 * Compile-time RuntimeStream and TupleFactory which just throws exceptions as the Stream does not
 * exist at compile time.
 */
class CompileTimeStream<T> implements RuntimeStream<T>, TupleFactory<T> {

  static final TupleFactory<OutputTuple> factory = new CompileTimeStream<OutputTuple>();
  static final RuntimeStream<Tuple> stream = new CompileTimeStream<Tuple>();

  private CompileTimeStream() {}

  @Override
  public void tuple(T tuple) throws Exception {
    throw CompileTimeState.runtimeOnly();
  }

  @Override
  public void mark(Punctuation mark) throws Exception {
    throw CompileTimeState.runtimeOnly();
  }

  @Override
  public <A> A action(StreamAction<A> action) throws Exception {
    throw CompileTimeState.runtimeOnly();
  }

  @Override
  public T newTuple() {
    throw CompileTimeState.runtimeOnly();
  }
}
