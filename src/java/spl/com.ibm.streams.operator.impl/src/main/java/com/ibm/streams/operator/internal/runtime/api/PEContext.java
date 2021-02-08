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

package com.ibm.streams.operator.internal.runtime.api;

import java.util.Map;

/** Context information for the {@code PE}'s execution context. */
public interface PEContext {

  int getCurrentOperator();

  /** Returns when shutdown has been requested. */
  void waitForShutdown();

  /** Return domain credentials. */
  String[] getDomainCredentials();

  /**
   * Get the application configuration specified by {@code name}. This method returns an
   * unmodifiable map which provides users with read-only access. An empty map is returned if the
   * configuration is not found.
   *
   * @param name the configuration name
   * @return the configuration
   */
  Map<String, String> getApplicationConfiguration(String name);
}
