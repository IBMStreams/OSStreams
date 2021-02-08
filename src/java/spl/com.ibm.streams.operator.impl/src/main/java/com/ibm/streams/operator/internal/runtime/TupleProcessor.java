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

package com.ibm.streams.operator.internal.runtime;

import com.ibm.streams.operator.StreamingData.Punctuation;

/**
 * API to provide the glue between code that produces tuples and the Operator implementation that
 * will process them.
 *
 * <p>Any implementation of this class must set up the correct thread context class loader before
 * calling the user code.
 *
 * <p>Implementations are thread-safe in that multiple threads may call any method concurrently and
 * will result in concurrent calls to the user's Operator implementation. The Operator
 * implementation must itself be thread-safe.
 *
 * @param <S> StreamingInput
 * @param <T> Tuple
 */
public interface TupleProcessor<S, T> {

  /** Process a tuple that arrived on the port. */
  public void processTuple(S port, T tuple) throws Exception;

  /** Process a punctuation mark that arrived on the port. */
  public void processPunctuation(S port, Punctuation mark) throws Exception;
}
