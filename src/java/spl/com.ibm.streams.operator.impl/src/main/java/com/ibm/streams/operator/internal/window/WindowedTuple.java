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

package com.ibm.streams.operator.internal.window;

import java.io.Serializable;

/**
 * Class that maintains the tuple in the window logically. When inserted the tuple is a reference to
 * a just inserted tuple, but during event processing the operator's StreamWindowListener can remove
 * the reference to the tuple from the window, while maintaining its position. So that for example a
 * window with eviction policy count(3) may have three WindowedTuple references, but all may have a
 * null for tuple. So the window logically contains three tuples, but with low storage overhead.
 */
public class WindowedTuple<T> implements Serializable {

  /** */
  private static final long serialVersionUID = 6467987834199168868L;

  // private static final WindowedTuple<?> NO_TUPLE = new WindowedTuple<Object>(null);

  private T tuple;

  protected WindowedTuple(T tuple) {
    synchronized (this) {
      this.tuple = tuple;
    }
  }

  public synchronized T getTuple() {
    return tuple;
  }

  synchronized void clear() {
    tuple = null;
  }

  /*
  private Object writeReplace() throws ObjectStreamException {
      return tuple == null ? NO_TUPLE : this;
  }

  private Object readResolve() throws ObjectStreamException {
      return tuple == null ? NO_TUPLE : this;
  }
  */
}
