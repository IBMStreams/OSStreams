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

import com.ibm.streams.operator.Operator;
import com.ibm.streams.operator.state.Checkpoint;
import com.ibm.streams.operator.state.StateHandler;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

/**
 * Abstract class that registers listeners with a {@link StreamWindow}. to perform partitioned state
 * management. The class may safely be used with non-partitioned windows. When used with a
 * non-partitioned window the partition key is a {@code java.lang.Integer} with value zero. <br>
 * Sub-classes extend this class to proved state changes driven by window events. Use of this class
 * allows the focus on the the logic related to events rather that managing per-partition state.
 *
 * <p>The ordering guarantees for {@link StreamWindowListener} are guaranteed ensuing that changes
 * made by any previous <code>handleEvent</code> are visible.
 *
 * <p>For a partitioned window an event of type {@link StreamWindowEvent.Type#FINAL} will be
 * generated for every active partition. If final processing is required once all partitions have
 * had their {@code FINAL} event handled then it must be performed in the {@link
 * Operator#processPunctuation(com.ibm.streams.operator.StreamingInput,
 * com.ibm.streams.operator.StreamingData.Punctuation)} method of the enclosing operator.
 *
 * <p>This class implements {@link StateHandler} to persist the operator's window state on a
 * checkpoint. The persisted state includes the {@link #getPartitionState(Object) state for each
 * partition} by serializing the state object, thus the state object type {@code S} must implement
 * {@code java.io.Serializable}. <br>
 * When an instance is created it is automatically {@link
 * com.ibm.streams.operator.OperatorContext#registerStateHandler(StateHandler) registered} as a
 * {@code StateHandler} for the operator. <br>
 * Operators must ensure that instances of {@code StateHandler} and {@code StreamWindowListener} are
 * registered in a consistent order to ensure the order of information written to a checkpoint and
 * read from a checkpoint upon reset is consistent.
 *
 * @param S Type of the state.
 * @param T Tuple type.
 */
public abstract class StatefulWindowListener<S, T>
    implements StreamWindowListener<T>, StateHandler {

  private Map<Object, S> partionedStates = Collections.synchronizedMap(new HashMap<Object, S>());

  private Set<Object> insertsOccurred = Collections.synchronizedSet(new HashSet<Object>());

  private Set<Object> evictionsOccurred = Collections.synchronizedSet(new HashSet<Object>());

  private Set<Object> seenInitialFull = Collections.synchronizedSet(new HashSet<Object>());

  private final StreamWindow<T> window;

  /**
   * Creates and registers a {@link StreamWindowListener} for the passed in window. This instance
   * must not be registered as a {@link StreamWindowListener} with the {@link StreamWindow}, as an
   * internal private {@link StreamWindowListener} is created and registered by this constructor.
   * Creates states for each partition using newState().
   *
   * @param window Window state is being managed for.
   */
  protected StatefulWindowListener(final StreamWindow<T> window) {

    this.window = window;

    if (window.getType() == StreamWindow.Type.NOT_WINDOWED)
      throw new IllegalStateException(this.getClass().getName() + " : " + window.getType());

    if (!supportsWindow())
      throw new IllegalStateException(
          this.getClass().getName() + ".supportsWindow() == false, window: " + window);

    window.registerListener(
        new StreamWindowListener<T>() {

          @Override
          public void handleEvent(StreamWindowEvent<T> event) throws Exception {
            final Object partition = event.getPartition();

            switch (event.getType()) {
              case INSERTION:
                insertsOccurred.add(partition);
                StatefulWindowListener.this.handleEvent(event);
                break;

              case EVICTION:
                try {
                  StatefulWindowListener.this.handleEvent(event);
                } finally {
                  if (window.getType() == StreamWindow.Type.TUMBLING) {
                    partionedStates.put(
                        partition, getInitializedState(partition, getPartitionState(partition)));
                    insertsOccurred.remove(partition);
                    return;
                  } else {
                    evictionsOccurred.add(partition);
                  }
                }
                break;

              case TRIGGER:
                try {
                  StatefulWindowListener.this.handleEvent(event);
                } finally {
                  insertsOccurred.remove(partition);
                  evictionsOccurred.remove(partition);
                }
                break;

              case INITIAL_FULL:
                synchronized (StatefulWindowListener.this) {
                  seenInitialFull.add(partition);
                }
                // fall-through
              case FINAL:
                StatefulWindowListener.this.handleEvent(event);
                break;

              case PARTITION_EVICTION:
                try {
                  StatefulWindowListener.this.handleEvent(event);
                } finally {
                  partionedStates.remove(partition);
                  insertsOccurred.remove(partition);
                  evictionsOccurred.remove(partition);
                  seenInitialFull.remove(partition);
                }
                break;
            }
          }
        },
        false);
  }

  /**
   * Get the window this object is handling.
   *
   * @return The window this object is handling.
   */
  public final StreamWindow<T> getWindow() {
    return window;
  }

  /**
   * Return true if this listener supports the window type described by StreamWindow. This
   * implementation returns true. Sub-classes may override this method to indicate limited window
   * support.
   *
   * @return True.
   */
  protected boolean supportsWindow() {
    return true;
  }

  /**
   * Get an initialized state object for the given partition. Sub-classes may simply return a new
   * object or re-initialize the passed in object if it is not null. The first call for both window
   * types will be from the constructor.
   *
   * <p>For tumbling windows getInitializedState() will be called for every EVICTION event after the
   * call to the sub-class's handleEvent(). <br>
   * For sliding windows getInitializedState() will only be called at construction time.
   *
   * @param partition Partition for the state.
   * @param state The previous state or null if this is the first call.
   * @return An initialized state object.
   */
  protected abstract S getInitializedState(Object partition, S state);

  /**
   * Return the state for the given partition.
   *
   * @param partition
   * @return State for the partition.
   */
  protected final S getPartitionState(Object partition) {
    S state = partionedStates.get(partition);

    // Since partitions can be dynamic, state is only created
    // initially when a sub-class requires it.
    if (state == null) {
      state = getInitializedState(partition, null);
      partionedStates.put(partition, state);
    }
    return state;
  }

  /**
   * Has this partition had a tuple inserted into it. <br>
   * For tumbling windows this flag is reset to false after an eviction event. <br>
   * For sliding windows this flag is reset to false after a trigger event.
   *
   * <p>After a partition eviction this flag is set to false.
   *
   * @param partition
   * @return True if inserts have occurred for the partition, false otherwise.
   */
  protected final boolean insertionsOccurred(final Object partition) {
    return insertsOccurred.contains(partition);
  }

  /**
   * For a sliding window has this partition had tuples evicted. <br>
   * For tumbling windows this flag is always false. <br>
   * For sliding windows this flag is reset to false after a trigger event.
   *
   * <p>After a partition eviction this flag is set to false.
   *
   * @param partition
   * @return True if evictions have occurred for the partition, false otherwise.
   */
  protected final boolean evictionsOccurred(final Object partition) {
    return evictionsOccurred.contains(partition);
  }

  /**
   * For a sliding window return true if an INITIAL_FULL event has been received for the specified
   * partition.
   *
   * <p>After a partition eviction this flag is set to false.
   *
   * @param partition Partition to check
   * @return True if INITIAL_FULL event has been received for the partition, false otherwise.
   */
  protected boolean seenInitialFull(Object partition) {
    return seenInitialFull.contains(partition);
  }

  /** {@inheritDoc} */
  @Override
  public void close() throws IOException {
    // TODO Auto-generated method stub

  }
  /** {@inheritDoc} */
  @Override
  public void drain() throws Exception {
    // TODO Auto-generated method stub

  }
  /** {@inheritDoc} */
  @Override
  public void checkpoint(Checkpoint checkpoint) throws Exception {
    final ObjectOutputStream ckptOut = checkpoint.getOutputStream();
    ckptOut.writeObject(evictionsOccurred);
    ckptOut.writeObject(insertsOccurred);
    ckptOut.writeObject(partionedStates);
    ckptOut.writeObject(seenInitialFull);
  }
  /** {@inheritDoc} */
  @Override
  public void resetToInitialState() throws Exception {
    evictionsOccurred.clear();
    insertsOccurred.clear();
    partionedStates.clear();
    seenInitialFull.clear();
  }
  /** {@inheritDoc} */
  @SuppressWarnings("unchecked")
  @Override
  public void reset(Checkpoint checkpoint) throws Exception {
    ObjectInputStream ckptIn = checkpoint.getInputStream();
    evictionsOccurred = (Set<Object>) ckptIn.readObject();
    insertsOccurred = (Set<Object>) ckptIn.readObject();
    partionedStates = (Map<Object, S>) ckptIn.readObject();
    seenInitialFull = (Set<Object>) ckptIn.readObject();
  }

  /** {@inheritDoc} */
  @Override
  public void retireCheckpoint(long id) throws Exception {
    // TODO Auto-generated method stub
  }

  /** {@inheritDoc} */
  @Override
  public void prepareForNonBlockingCheckpoint(long id) throws Exception {}

  /** {@inheritDoc} */
  @Override
  public void regionCheckpointed(long id) throws Exception {}
}
