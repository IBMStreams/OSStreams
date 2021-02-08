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

package com.ibm.streams.flow.handlers;

import com.ibm.streams.operator.StreamingData.Punctuation;

/**
 * A generic processing interface for a stream of tuples and punctuation marks.
 *
 * @param <T> The type of the tuples on the stream
 * @since InfoSphere&reg; Streams Version 3.1
 */
public interface StreamHandler<T> {

  /**
   * Process a tuple.
   *
   * @param tuple The tuple.
   * @throws Exception Failure to process the tuple.
   */
  public void tuple(T tuple) throws Exception;

  /**
   * Process a punctuation mark.
   *
   * @param mark The punctuation mark.
   * @throws Exception Failure to process the punctuation mark.
   */
  public void mark(Punctuation mark) throws Exception;
}
