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

package com.ibm.streams.operator.internal.compile;

import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import com.ibm.streams.spl.model.OperatorInstanceModelType;

abstract class CompileTimeState {

  private final CompileTimeSetup setup;

  CompileTimeState(CompileTimeSetup setup) {
    this.setup = setup;
  }

  final CompileTimeSetup getSetup() {
    return setup;
  }

  final String getProperty(String name) {
    return getSetup().getProperty(name);
  }

  final OperatorInstanceModelType getModel() {
    return getSetup().getModel();
  }

  static final IllegalStateException runtimeOnly() {
    return new IllegalStateException(
        new Message(Key.SPL_JAVA_CTS_RUNTIME_ONLY, "").getLocalizedMessageAndPrefix());
  }
}
