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

import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.OperatorContext.ContextCheck;
import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.StreamingOutput;
import com.ibm.streams.operator.compile.OperatorContextChecker;
import com.ibm.streams.operator.model.OutputPortSet;
import com.ibm.streams.operator.model.OutputPortSet.WindowPunctuationOutputMode;
import com.ibm.streams.operator.model.OutputPorts;
import com.ibm.streams.operator.model.Parameter;
import com.ibm.streams.operator.model.PrimitiveOperator;
import com.ibm.streams.operator.samples.patterns.ProcessTupleProducer;
import com.ibm.streams.operator.state.ConsistentRegionContext;
import java.util.Enumeration;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

/**
 * Sample source operator using a {@code process()} method. Operator that reads the Java System
 * property set and submits an output tuple for each system property. A single output port is
 * assumed with attributes {@code name} for the property name, {@code value} for its value and
 * {@code tags} for a set of tags set from parameters.
 *
 * <p>The class handles two parameters to provide a simple tagging scheme. This is used to
 * demonstrate parameter handling. <br>
 * If the parameter <code>tagged</code> is set then any tuple for a system property that starts with
 * <code>tagged</code>'s value will have its <code>tags</code> attribute set to the value of the
 * <code>tags</code> parameter.
 *
 * <p>The parameter <code>initDelay</code> is inherited from the parent class TupleProducer.
 *
 * <p>This operator provided as the sample Java primitive operator <br>
 * {@code com.ibm.streams.javaprimitivesamples.sources.SystemPropertySource} <br>
 * in the sample {@code JavaOperators} toolkit located in: <br>
 * {@code $STREAMS_INSTALL/samples/spl/feature/JavaOperators}
 *
 * @see com.ibm.streams.operator.samples.patterns.TupleProducer
 */
@PrimitiveOperator(description = "Submits an output tuple for each Java system property.")
@OutputPorts({
  @OutputPortSet(
      cardinality = 1,
      windowPunctuationOutputMode = WindowPunctuationOutputMode.Generating,
      description =
          "Port requiring `name` and `value` attributes representing a system property. Optional `tags` attribute containing tags for the property")
})
public class SystemPropertySource extends ProcessTupleProducer {

  private String tagged;

  private Set<String> tags;

  /** {@inheritDoc} */
  @Override
  public void initialize(OperatorContext context) throws Exception {
    super.initialize(context);
  }

  /**
   * If parameter {@code tagged} is set then {@code tags} is required.
   *
   * @param checker Checker for this invocation
   */
  @ContextCheck
  public static void checkTaggedParameters(OperatorContextChecker checker) {
    checker.checkDependentParameters("tagged", "tags");
    OperatorContext context = checker.getOperatorContext();
    if (context.getParameterNames().contains("tagged")) {
      checker.checkRequiredAttributes(context.getStreamingOutputs().get(0), "tags");
    }
  }

  /** Check the output attributes name and value are present. */
  @ContextCheck
  public static void checkOuputAttributes(OperatorContextChecker checker) {
    OperatorContext context = checker.getOperatorContext();
    checker.checkRequiredAttributes(context.getStreamingOutputs().get(0), "name", "value");
  }

  /**
   * Set the tagged parameter.
   *
   * @param tagged Prefix indicated tagged system properties.
   */
  @Parameter(
      optional = true,
      description = "Prefix for property names to be tagged with the value of `tags`.")
  public void setTagged(String tagged) {
    this.tagged = tagged;
  }

  /**
   * Set the tags parameter.
   *
   * @param tags Tags to be associated with tagged properties.
   */
  @Parameter(
      optional = true,
      description =
          "Tags to set in `tags` output attribute when the property name starts with the value of `taggged`.")
  public void setTags(List<String> tags) {
    this.tags = new HashSet<String>(tags);
  }

  @Override
  protected void process() throws Exception {

    final ConsistentRegionContext crc =
        getOperatorContext().getOptionalContext(ConsistentRegionContext.class);

    // makeConsistent requires a permit is held.
    if (crc != null) crc.acquirePermit();

    while (!Thread.interrupted()) {

      getPropertiesAndSubmit();
      if (crc == null) {
        // Finite set of properties so send a final mark.
        getOutput(0).punctuate(Punctuation.FINAL_MARKER);
        break;
      } else {
        // Make the region consistent.
        if (crc.makeConsistent()) {
          break;
        }
        // else resubmit the tuples by continuing the while statement.
      }
    }
    if (crc != null) crc.releasePermit();
  }

  /** Iterate over all the system properties submitting a tuple for each name value pair. */
  protected void getPropertiesAndSubmit() throws Exception {

    final StreamingOutput<OutputTuple> out = getOutput(0);
    for (Enumeration<?> e = System.getProperties().propertyNames(); e.hasMoreElements(); ) {
      String name = (String) e.nextElement();
      String value = System.getProperty(name);

      OutputTuple tuple = out.newTuple();

      tuple.setString("name", name);
      tuple.setString("value", value);

      // Add the simple tagging in, if set.
      if (tagged != null) {
        if (name.startsWith(tagged)) tuple.setSet("tags", tags);
      }

      out.submit(tuple);
    }

    // Make the set of tuples a window.
    out.punctuate(Punctuation.WINDOW_MARKER);
  }
}
