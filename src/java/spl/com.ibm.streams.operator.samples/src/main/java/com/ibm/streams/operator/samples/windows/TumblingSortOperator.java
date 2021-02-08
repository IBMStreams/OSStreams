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

package com.ibm.streams.operator.samples.windows;

import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.OperatorContext.ContextCheck;
import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.compile.OperatorContextChecker;
import com.ibm.streams.operator.model.InputPortSet;
import com.ibm.streams.operator.model.InputPortSet.WindowMode;
import com.ibm.streams.operator.model.InputPortSet.WindowPunctuationInputMode;
import com.ibm.streams.operator.model.OutputPortSet;
import com.ibm.streams.operator.model.OutputPortSet.WindowPunctuationOutputMode;
import com.ibm.streams.operator.model.Parameter;
import com.ibm.streams.operator.model.PrimitiveOperator;
import com.ibm.streams.operator.window.AbstractWindowOperator;
import com.ibm.streams.operator.window.StreamWindow;
import com.ibm.streams.operator.window.sort.TumblingWindowSort;
import java.util.Collections;
import java.util.List;

/**
 * Sample ascending sort operator for a tumbling window. Extends {@link AbstractWindowOperator} to
 * ensure all tuples and punctuation are handled by the {@link
 * com.ibm.streams.operator.window.StreamWindowListener StreamWindowListener}. <br>
 * {@link TumblingWindowSort} is used as the StreamWindowListener with the parameter <code>
 * sortAttributes</code> defining the set of attributes used for ordering. The first attribute in
 * the list is the most significant in the sort order. All the attributes must correspond to a
 * java.lang.Comparable object type. <br>
 * The optional boolean parameter <code>flushOnFinal</code> indicates if the remaining tuples in the
 * window should be flushed and submitted to the output port in sorted order when a final
 * punctuation mark is received. The default is false which means tuples remaining in the window are
 * discarded upon a final mark. Set to true to flush remaining tuples.
 *
 * <p>Requires a single tumbling windowed input port and a single output port.
 *
 * <p>This operator provided as the sample Java primitive operator <br>
 * {@code com.ibm.streams.javaprimitivesamples.windows.TumblingSort} <br>
 * in the sample {@code JavaOperators} toolkit located in: <br>
 * {@code $STREAMS_INSTALL/samples/spl/feature/JavaOperators}
 */
@PrimitiveOperator(
    name = "TumblingSort",
    description = "Sample ascending sort operator for a tumbling window.")
@InputPortSet(
    cardinality = 1,
    windowingMode = WindowMode.Windowed,
    windowPunctuationInputMode = WindowPunctuationInputMode.WindowBound)
@OutputPortSet(
    cardinality = 1,
    windowPunctuationOutputMode = WindowPunctuationOutputMode.Generating)
public class TumblingSortOperator extends AbstractWindowOperator {

  private boolean flushOnFinal = false;
  private List<String> sortAttributes = Collections.emptyList();

  /**
   * Perform a tumbling sort using the attributes defined by the SPL parameter sortAttributes.
   * Assumes the operator has a single input port and a single output port.
   */
  @Override
  public void initialize(OperatorContext context) throws Exception {
    super.initialize(context);

    StreamWindow<Tuple> window = getInput(0).getStreamWindow();

    new TumblingWindowSort(window, true, getSortAttributes(), getOutput(0), isFlushOnFinal());
  }

  /** Check the input port is tumbling. */
  @ContextCheck
  public static void requireTumblingWindow(final OperatorContextChecker checker) {
    final StreamingInput<Tuple> inputPort =
        checker.getOperatorContext().getStreamingInputs().get(0);
    checker.checkTumblingWindow(inputPort);
  }

  /**
   * Flush sort buffer when final punctuation mark is processed.
   *
   * @param flushOnFinal True to submit outstanding tuples, false to discard them.
   */
  @Parameter(
      optional = true,
      description = "Flush sort buffer when final punctuation mark is processed.")
  public void setFlushOnFinal(boolean flushOnFinal) {
    this.flushOnFinal = flushOnFinal;
  }

  /**
   * True if outstanding tuples are submitted when final markers are processed, false if they are
   * discarded.
   */
  public boolean isFlushOnFinal() {
    return flushOnFinal;
  }

  /**
   * Names of the attributes to perform the sort on. If not supplied then all comparable attributes
   * will be used as the sort key, with priority given to attributes with the lower index.
   */
  @Parameter(
      optional = true,
      description =
          "Names of the attributes to perform the sort on. If not supplied then all comparable attributes will be used as the sort key, with priority given to attributes with the lower index.")
  public void setSortAttributes(List<String> sortAttributes) {
    this.sortAttributes = sortAttributes;
  }

  public List<String> getSortAttributes() {
    return sortAttributes;
  }
}
