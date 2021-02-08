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

package com.ibm.streams.operator.state;

import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.OptionalContext;
import java.io.IOException;

/**
 * Context information that allows operators to create or restore checkpoint data of their state
 * information.
 *
 * <p>Operator data can be checkpointed or restored in 2 ways:
 *
 * <OL>
 *   <LI>Periodic: An application specifies a {@code checkpoint: periodic} config option for an
 *       operator with a corresponding time interval in SPL.
 *   <LI>Operator-driven: Operator can request a synchronous, operator-driven checkpoint. The {@code
 *       checkpoint: operatorDriven} option is specified in SPL.
 * </OL>
 *
 * In each of the above scenarios, a valid {@code StateHandler} must be registered with the {@code
 * OperatorContext.registerStateHandler()} method. In the case of periodic checkpointing, the {@code
 * StateHandler.checkpoint()} method will be automatically called by the SPL Runtime at configured
 * interval. <br>
 * In case of operatorDriven checkpointing, the operator can request a checkpoint by calling the
 * createCheckpoint() method within this interface. <br>
 * When an operator is restarted, the {@code StateHandler.reset()} is automatically called by the
 * SPL Runtime to restore operator's state from checkpoint. <br>
 * Note that a checkpoint context is only available if checkpointing is enabled for an operator
 * otherwise the {@code OperatorContext.getOptionalContext(CheckpointContext.class)} call will
 * return null.
 *
 * @see OperatorContext#getOptionalContext(Class)
 * @since InfoSphere&reg; Streams Version 4.0
 */
public interface CheckpointContext extends OptionalContext {

  /**
   * TODO
   *
   * @since InfoSphere&reg; Streams Version 4.0
   */
  public enum Kind {
    PERIODIC,
    OPERATOR_DRIVEN
  }

  /**
   * Return the kind of the configured checkpointing.
   *
   * @return kind of the configured checkpointing.
   */
  Kind getKind();

  /**
   * Returns the checkpointing interval if periodic checkpointing is configured. This method returns
   * <0 if operatorDriven checkpointing is configured.
   *
   * @return the checkpointing interval for periodic checkpointing or <0 otherwise
   */
  public double getCheckpointingInterval();

  /**
   * Create a checkpoint. This will trigger a call to the registered {@code
   * StateHandler.checkpoint()} method.
   *
   * @return True if checkpointing is successful, false if checkpointing is not enabled.
   * @throws IOException if there is any error during creating a checkpoint.
   */
  public boolean createCheckpoint() throws IOException;

  /**
   * Return whether checkpointing/restore is enabled.
   *
   * @return True if checkpointing/restore is enabled, false otherwise.
   */
  public boolean isEnabled();
}
