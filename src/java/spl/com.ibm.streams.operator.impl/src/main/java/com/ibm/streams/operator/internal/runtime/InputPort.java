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

package com.ibm.streams.operator.internal.runtime;

import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.internal.window.WindowSetup;
import com.ibm.streams.operator.window.StreamWindow;
import java.util.List;

/**
 * Concrete implementation of StreamingInput. StreamingInput is currently just a data structure with
 * no actions related to it. Tuples are processed by external agent (e.g. JNI glue code) creating
 * tuples and passing them to the OperatorAdapter.
 */
public final class InputPort<T> extends AbstractStream implements StreamingInput<T> {

  /** Import type for this input port. */
  private final ImportType importType;

  /**
   * Window definition. Never null, a non-windowed port has an object that is of type NOT_WINDOWED.
   */
  private final StreamWindow<T> window;

  /** Is this port a control port. */
  private final boolean isControl;

  public InputPort(
      OperatorContext context,
      int port,
      int typeOrdinal,
      String name,
      StreamSchema schema,
      int pePortNumber,
      List<String> windowInfo,
      boolean isControlPort) {
    super(port, name, schema, pePortNumber);
    importType = ImportType.values()[typeOrdinal];

    window = WindowSetup.getWindow(context, this, windowInfo);
    isControl = isControlPort;
  }

  @Override
  public ImportType getImportType() {
    return importType;
  }

  @Override
  public final boolean isImported() {
    return getImportType() != ImportType.NOT_IMPORTED;
  }

  @Override
  public StreamWindow<T> getStreamWindow() {
    return window;
  }

  @Override
  public boolean isControl() {
    return isControl;
  }
}
