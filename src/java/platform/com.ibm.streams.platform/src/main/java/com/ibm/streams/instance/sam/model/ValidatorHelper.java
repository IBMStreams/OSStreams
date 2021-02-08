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

package com.ibm.streams.instance.sam.model;

import static com.google.common.base.Preconditions.checkNotNull;
import static com.google.common.base.Preconditions.checkPositionIndex;

import java.util.Collection;

public final class ValidatorHelper {

  public static void validateIfPresent(Validator obj) throws Exception {
    if (obj != null) obj.validate();
  }

  public static void checkContentsNotNullAndValidate(Collection<? extends Validator> obj)
      throws Exception {
    checkNotNull(obj);
    for (Validator v : obj) {
      checkNotNull(v);
      v.validate();
    }
  }

  public static void checkContentsNotNull(Collection<?> obj) throws Exception {
    checkNotNull(obj);
    for (Object o : obj) {
      checkNotNull(o);
    }
  }

  public static void checkContentsPositionIndex(Collection<? extends Number> obj) {
    checkNotNull(obj);
    for (Number n : obj) checkPositionIndex(n.intValue(), obj.size());
  }

  public static void checkNotNullAndValidate(Validator obj) throws Exception {
    checkNotNull(obj);
    obj.validate();
  }

  public static void checkNotNullAndValidate(Validator obj, Object errorMessage) throws Exception {
    checkNotNull(obj, errorMessage);
    obj.validate();
  }

  public static void checkNotNullAndValidate(
      Validator obj, String errorMessageTemplate, Object... errorMessageArgs) throws Exception {
    checkNotNull(obj, errorMessageTemplate, errorMessageArgs);
    obj.validate();
  }
}
