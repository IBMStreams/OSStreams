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

import com.ibm.streams.operator.AbstractOperator;
import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.Tuple;

/**
 * Abstract operator that forces all tuple processing to be through window event handling.
 *
 * @see StreamWindowListener
 */
public class AbstractWindowOperator extends AbstractOperator {

  /**
   * {@inheritDoc}
   *
   * <p>This implementation validates every input port is windowed.
   *
   * <p>Sub-classes <b>must</b> register a {@link StreamWindowListener} for each input port using
   * {@link StreamWindow#registerListener(StreamWindowListener, boolean)}.
   */
  public void initialize(OperatorContext context) throws Exception {
    super.initialize(context);
    if (context.getNumberOfStreamingInputs() == 0)
      throw new IllegalStateException(getClass().getName() + ": getNumberOfStreamingInputs() == 0");

    for (StreamingInput<Tuple> port : context.getStreamingInputs())
      if (port.getStreamWindow().getType() == StreamWindow.Type.NOT_WINDOWED)
        throw new IllegalStateException(port.getName() + ": " + port.getStreamWindow().getType());
  }

  /**
   * Final method that does nothing, all tuple processing will be handled through the registered
   * instances of {@link StreamWindowListener}.
   */
  public final void process(StreamingInput<Tuple> stream, Tuple tuple) {}
  /**
   * Empty method, punctuation processing is handled through the registered instances of {@link
   * StreamWindowListener}.
   *
   * <p>Sub-classes may need to override this to perform any processing for {@link
   * Punctuation#FINAL_MARKER} marks, when such marks are not automatically propagated to the output
   * ports.
   */
  public void processPunctuation(StreamingInput<Tuple> stream, Punctuation mark) throws Exception {}
}
