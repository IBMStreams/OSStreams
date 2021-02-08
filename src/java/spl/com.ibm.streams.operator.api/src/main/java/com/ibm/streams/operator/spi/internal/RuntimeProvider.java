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

package com.ibm.streams.operator.spi.internal;

import com.ibm.streams.operator.OperatorContext;
import java.util.ServiceLoader;

public abstract class RuntimeProvider {

  /** Bundle for this jar, com.ibm.streams.operator.jar */
  // private static final String BUNDLE_NAME = "com.ibm.streams.operator.spi.internal.messages";
  // //$NON-NLS-1$
  // public static final ResourceBundle bundle = ResourceBundle.getBundle(BUNDLE_NAME);

  public static final RuntimeProvider provider;

  static {
    ServiceLoader<RuntimeProvider> loader =
        ServiceLoader.load(RuntimeProvider.class, RuntimeProvider.class.getClassLoader());
    // Assume this is always a single provider.
    RuntimeProvider srp = null;
    for (RuntimeProvider p : loader) srp = p;
    provider = srp;
  }

  public abstract OperatorContext getCurrentContext();
}
