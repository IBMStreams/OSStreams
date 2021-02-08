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

package com.ibm.streams.controller.executor;

import java.util.Properties;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class DefaultExecutor implements IExecutor {

  private static final Logger LOGGER = LoggerFactory.getLogger(DefaultExecutor.class);

  public void setProperties(Properties properties) {}

  public void execute(EExecutionCommand command, Runnable fun) {
    LOGGER.debug("Execute {}", command.name());
    fun.run();
  }
}
