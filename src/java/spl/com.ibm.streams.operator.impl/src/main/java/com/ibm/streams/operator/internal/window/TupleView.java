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

import java.util.Collection;
import java.util.Iterator;

class TupleView<T, W extends WindowedTuple<T>> implements Iterable<T> {

  final Collection<W> tuples;

  public TupleView(Collection<W> tuples) {
    this.tuples = tuples;
  }

  public TupleView<T, W> addTuple(W wt) {
    tuples.add(wt);
    return this;
  }

  @Override
  public Iterator<T> iterator() {

    return new Iterator<T>() {

      private final Iterator<W> iterator = tuples.iterator();
      private WindowedTuple<T> nextTuple;
      private WindowedTuple<T> actveTuple;

      @Override
      public boolean hasNext() {

        if (nextTuple != null) return true;

        while (iterator.hasNext()) {
          nextTuple = iterator.next();
          if (nextTuple.getTuple() != null) return true;
        }

        nextTuple = null;
        return false;
      }

      @Override
      public T next() {
        if (!hasNext()) throw new IllegalStateException();
        try {
          return (actveTuple = nextTuple).getTuple();
        } finally {
          nextTuple = null;
        }
      }

      @Override
      public void remove() {
        if (actveTuple == null) throw new IllegalStateException();

        actveTuple.clear();
        actveTuple = null;
      }
    };
  }
}
