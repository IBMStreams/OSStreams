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

package com.ibm.streams.instance.sam.model.logical.spl;

import com.ibm.streams.instance.sam.exception.SAMInternalErrorException;
import java.math.BigInteger;

public class PrimitiveExportInputPort extends PrimitivePort {

  public PrimitiveExportInputPort(
      BigInteger index, ExportOperator owningPrimitiveOp, SPLLogicalModel model) {
    super(index, owningPrimitiveOp, model);
  }

  public PrimitiveExportInputPort(
      PrimitiveExportInputPort other, ExportOperator owningPrimitiveOp) {
    super(other, owningPrimitiveOp);
  }

  public void addReverseConnections(BigInteger channelIndex) throws SAMInternalErrorException {
    throw SPLLogicalModel.traceAndReturnException("Internal error: unexpected condition");
  }
}
