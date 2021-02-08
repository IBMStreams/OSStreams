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

package com.ibm.streams.spl.messages;

/**
 * A Streams Message Key interface. This represents a message key used to identify a message in a
 * resource bundle generated from our XLIFF files. This interface is typically implemented by the
 * generated Key file.
 */
public interface MessageKey {

  /**
   * Get the associated message ID for this message key.
   *
   * @return The message identifier (e.g. CDISC0001I)
   */
  public String getMessageId();

  /**
   * Get the associated resource bundle name.
   *
   * @return The name of the resource bundle.
   */
  public String getResourceBundleName();
}
