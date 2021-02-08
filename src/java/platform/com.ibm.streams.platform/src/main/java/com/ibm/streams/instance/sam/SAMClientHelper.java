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

package com.ibm.streams.instance.sam;

import com.ibm.distillery.utils.DistilleryExceptionCode;
import com.ibm.distillery.utils.exc.SerializationException;
import com.ibm.streams.platform.services.AugmentedPEType;
import java.util.Arrays;
import java.util.Vector;

public final class SAMClientHelper {

  public static String serializeAugmentedPEType(AugmentedPEType t) throws SerializationException {
    try {
      SAMXmlHelper<AugmentedPEType> helper = new SAMXmlHelper<AugmentedPEType>();
      return helper.marshal(SAMXmlHelper.newObjf().createAugmentedPE(t));
    } catch (Exception e) {
      throw getSerializationException(t, e);
    }
  }

  public static SerializationException getSerializationException(Object t, Throwable cause) {
    return new SerializationException(
        cause,
        new DistilleryExceptionCode("CDISR1394E"),
        new Vector<String>(Arrays.asList(String.valueOf(t), cause.getMessage())));
  }
}
