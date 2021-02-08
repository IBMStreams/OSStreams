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

package com.ibm.streams.controller.utils;

import static junit.framework.TestCase.assertTrue;
import static org.junit.Assert.assertEquals;

import lombok.var;
import org.junit.Test;

public class ObjectUtilsTest {

  @Test
  public void pathRemovalTest() {
    var e0 = new ObjectUtilsEntity();
    var e1 = new ObjectUtilsEntity();
    e0.getValues().put("svc", "pe");
    var diff = ObjectUtils.diff(e0, e1);
    assertTrue(diff.isPresent());
    assertEquals("[{\"op\":\"remove\",\"path\":\"/values/svc\"}]", diff.get());
  }
}
