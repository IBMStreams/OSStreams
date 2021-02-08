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

package com.ibm.streams.operator.internal.ports;

import com.ibm.streams.operator.StreamingData.Punctuation;

/**
 * Invokes a Runnable if an exception is thrown processing a tuple or a punctuation mark.
 *
 * @param <T>
 */
public class RuntimeStreamExceptionHandler<T> extends RuntimeStreamForwarder<T> {

  private final Runnable onException;

  public RuntimeStreamExceptionHandler(Runnable onException, RuntimeStream<T> destination) {
    super(destination);
    this.onException = onException;
  }

  @Override
  public void tuple(T tuple) throws Exception {
    try {
      super.tuple(tuple);
    } catch (Exception e) {
      onException.run();
      throw e;
    }
  }

  @Override
  public void mark(Punctuation mark) throws Exception {
    try {
      super.mark(mark);
    } catch (Exception e) {
      onException.run();
      throw e;
    }
  }
}
