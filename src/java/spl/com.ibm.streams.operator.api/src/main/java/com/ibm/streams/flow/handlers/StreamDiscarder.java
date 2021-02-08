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
 * Discards all tuples and marks.
 *
 * @param <T> Type for the tuples on the stream
 * @since InfoSphere&reg; Streams Version 3.1
 */
public final class StreamDiscarder<T> implements StreamHandler<T> {

  /** Do nothing with the tuple. */
  @Override
  public final void tuple(T tuple) throws Exception {}

  /** Do nothing with the mark. */
  @Override
  public final void mark(Punctuation mark) throws Exception {}
}
