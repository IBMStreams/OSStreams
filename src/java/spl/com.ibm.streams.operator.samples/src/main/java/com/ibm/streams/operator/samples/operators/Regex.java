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

package com.ibm.streams.operator.samples.operators;

import com.ibm.streams.operator.Attribute;
import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.Type;
import com.ibm.streams.operator.logging.TraceLevel;
import com.ibm.streams.operator.model.InputPortSet;
import com.ibm.streams.operator.model.InputPorts;
import com.ibm.streams.operator.model.Parameter;
import com.ibm.streams.operator.model.PrimitiveOperator;
import com.ibm.streams.operator.samples.patterns.TupleInTupleOut;
import java.util.List;
import java.util.logging.Logger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Sample operator that using regular expressions to filter tuples. This class extends the pattern
 * class {@code TupleInTupleOut} to implement a functor like behavior when a tuple is output if it
 * matches any regular expression.
 *
 * <p>A set of regular expression patterns is set by the parameter {@code patterns}. The operator
 * uses the matching capability provided by the standard Java package {@code java.util.regex}.
 *
 * <p>A tuple matches if any of its {@code ustring} attributes match any of the provided patterns.
 * If a tuple matches then an output tuple will be generated with its attributes set from the input
 * using {@link OutputTuple#assign(Tuple)}.
 *
 * <p>This operator provided as the sample Java primitive operator <br>
 * {@code com.ibm.streams.javaprimitivesamples.operators.RegexFilter} <br>
 * in the sample {@code JavaOperators} toolkit located in: <br>
 * {@code $STREAMS_INSTALL/samples/spl/feature/JavaOperators}
 *
 * @see java.util.regex.Pattern
 * @see OutputTuple#assign(Tuple)
 */
@PrimitiveOperator(
    name = "RegexFilter",
    description = "Sample operator that using regular expressions to filter tuples.")
@InputPorts(@InputPortSet(cardinality = 1))
public class Regex extends TupleInTupleOut {

  /** SPL Trace facility */
  private static final Logger trace = Logger.getLogger(Regex.class.getName());

  private Pattern[] compiledPatterns;
  private Matcher[] matchers;

  /** {@inheritDoc} */
  public void initialize(OperatorContext context) throws Exception {
    super.initialize(context);
  }

  /**
   * Set the patterns used to filter tuples. The parameter {@code patterns} sets the regular
   * expressions to match the incoming tuple against.
   *
   * @param patterns Regular expression patterns to be used to filter tuples.
   */
  @Parameter(description = "Set the regular expression patterns used to filter tuples.")
  public synchronized void setPatterns(List<String> patterns) {

    trace.log(TraceLevel.DEBUG, "patterns=" + patterns);

    // Compile the patterns and create a Matcher per pattern.
    // The matcher will be reset for each value to be matched in filter.
    compiledPatterns = new Pattern[patterns.size()];
    matchers = new Matcher[patterns.size()];
    for (int i = 0; i < compiledPatterns.length; i++) {
      compiledPatterns[i] = Pattern.compile(patterns.get(i));
      matchers[i] = compiledPatterns[i].matcher("");
    }
  }

  /**
   * Execute the regular expressions against any <code>ustring</code> in the incoming tuple. If any
   * match exists true is returned to indicate an outgoing tuple is to be sent using the <code>
   * transform</code> method.
   */
  @Override
  protected boolean filter(StreamingInput<Tuple> inputStream, Tuple tuple) {

    for (Attribute attr : inputStream.getStreamSchema()) {
      if (attr.getType().getMetaType() != Type.MetaType.USTRING) continue;

      String value = tuple.getString(attr.getName());

      for (Matcher matcher : getMatchers()) {
        if (matcher.reset(value).matches()) {
          return true;
        }
      }
    }
    return false;
  }

  /**
   * Get the list of Matchers compiled from the patterns. Synchronized to ensure that all threads
   * are guaranteed visibility to the correct value set by the synchronized method initialize.
   *
   * @return the matchers
   */
  private synchronized Matcher[] getMatchers() {
    return matchers;
  }

  /**
   * Simply assign the input tuple to the output tuple which will copy across the values for any
   * matching Attributes, regardless of position in the StreamSchema.
   */
  @Override
  protected boolean transform(StreamingInput<Tuple> inputStream, Tuple tuple, OutputTuple outTuple)
      throws Exception {

    outTuple.assign(tuple);
    return true;
  }
}
