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

package com.ibm.streams.operator.samples.sources;

import com.ibm.streams.operator.Attribute;
import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.model.OutputPortSet;
import com.ibm.streams.operator.model.OutputPortSet.WindowPunctuationOutputMode;
import com.ibm.streams.operator.model.OutputPorts;
import com.ibm.streams.operator.model.Parameter;
import com.ibm.streams.operator.model.PrimitiveOperator;
import com.ibm.streams.operator.samples.patterns.PollingSingleTupleProducer;
import com.ibm.streams.operator.samples.state.Counter;
import com.ibm.streams.operator.state.Checkpoint;
import com.ibm.streams.operator.state.DelegateStateHandler;
import java.util.Random;
import java.util.logging.Logger;

/**
 * Sample source operator that creates a tuple with random values on its single output port
 * according to the defined period. <br>
 * Accepts an optional int64 parameter {@code seed} as the seed for the random value generator.
 *
 * <p>This operator provided as the sample Java primitive operator <br>
 * {@code com.ibm.streams.javaprimitivesamples.sources.RandomBeacon} <br>
 * in the sample {@code JavaOperators} toolkit located in: <br>
 * {@code $STREAMS_INSTALL/samples/spl/feature/JavaOperators}
 */
@PrimitiveOperator(
    description =
        "Sample source operator that creates a tuple with random values on its single output port according to the defined period.")
@OutputPorts({
  @OutputPortSet(
      cardinality = 1,
      windowPunctuationOutputMode = WindowPunctuationOutputMode.Generating,
      description = "Port that produces the generated tuples with random attribute values")
})
public class RandomBeacon extends PollingSingleTupleProducer {

  protected Random rand;
  private long seed = System.currentTimeMillis();

  @Parameter(optional = true, description = "Seed for the random number generator.")
  public void setSeed(long seed) {
    this.seed = seed;
  }

  public long getSeed() {
    return seed;
  }
  /**
   * Take the seed for the random number generator from the parameter {@code seed} if it exists,
   * otherwise use the current time. Logs the seed to allow replay of generated values.
   */
  @Override
  public synchronized void initialize(OperatorContext context) throws Exception {
    super.initialize(context);
    rand = new Random(getSeed());
    Logger.getAnonymousLogger().info("Random(seed)=" + getSeed());
  }

  /**
   * Set each attribute to a random value and then return true to submit the tuple.
   *
   * <p>A sub-class may override this method to set specific attributes while generating random
   * values for the remaining attributes. An example is provided in {@link
   * RandomBeaconWithTimestamp}.
   */
  @Override
  protected synchronized boolean fetchSingleTuple(final OutputTuple tuple) throws Exception {

    for (Attribute attribute : tuple.getStreamSchema()) {

      Object value = attribute.getType().randomValue(rand);

      tuple.setObject(attribute.getName(), value);
    }

    // Always submit the tuple
    return true;
  }

  @Override
  protected DelegateStateHandler<Counter> getStateHandler(Counter counter) {
    return new DelegateStateHandler<Counter>(counter) {
      @Override
      public synchronized void resetToInitialState() throws Exception {
        super.resetToInitialState();
        rand = new Random(getSeed());
      }

      @Override
      public synchronized void checkpoint(Checkpoint checkpoint) throws Exception {
        super.checkpoint(checkpoint);
        checkpoint.getOutputStream().writeObject(rand);
      }

      @Override
      public synchronized void reset(Checkpoint checkpoint) throws Exception {
        super.reset(checkpoint);
        rand = (Random) checkpoint.getInputStream().readObject();
      }
    };
  }
}
