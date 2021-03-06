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

package com.ibm.streams.application.models.spl.logical;

import java.math.BigInteger;

public class Toolkit {

  private final BigInteger index = null;
  private final String name = null;
  private final String uri = null;
  private final String version = null;

  private Toolkit() {}

  public BigInteger getIndex() {
    return index;
  }

  public String getName() {
    return name;
  }

  public String getUri() {
    return uri;
  }

  public String getVersion() {
    return version;
  }
}
