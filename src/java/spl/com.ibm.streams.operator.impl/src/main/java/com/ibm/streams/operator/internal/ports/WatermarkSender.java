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

package com.ibm.streams.operator.internal.ports;

import com.ibm.streams.operator.internal.eventtime.Watermark;

/**
 * This interface may optionally be implemented on a class that also implements {@link
 * com.ibm.streams.operator.StreamingInput StreamingInput}. If this interface is implemented, it
 * will be used to send watermark updates. Otherwise, watermark updates will be discarded.
 */
public interface WatermarkSender {
  /**
   * Send a watermark update.
   *
   * @param wm the watermark.
   */
  public void sendWatermark(Watermark wm);
}
