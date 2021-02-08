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

package com.ibm.streams.instance.sam.model.topology;

import java.math.BigInteger;

public interface ConnectionSource {

  // Returns true if the receiving object is an output port
  public boolean isOutputPort();

  // Returns true if the receiving object is a ParallelChannel
  public boolean isParallelChannel();

  // Return the output port that is the ultimate source
  public TopologyNode getOwningOperator();

  // Get the output port that is feeding this connection
  public TopologyNodeOutputPort getOutputPort();

  // Get the index of the output port feeding this connection
  public BigInteger getPortIndex();

  // Get the index of the operator feeding this connection
  public BigInteger getOperatorIndex();

  // Get the id of the application that owns the connection source
  public BigInteger getApplicationId();
}
