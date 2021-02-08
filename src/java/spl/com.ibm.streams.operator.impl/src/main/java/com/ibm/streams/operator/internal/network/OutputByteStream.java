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

package com.ibm.streams.operator.internal.network;

import com.ibm.streams.operator.internal.ports.RuntimeStream;
import java.nio.ByteBuffer;

/**
 * Handler that handles encoded tuples and in addition provides the mechanism to get a byte buffer.
 */
public interface OutputByteStream extends RuntimeStream<ByteBuffer> {
  ByteBuffer getByteBuffer(int size);
}
