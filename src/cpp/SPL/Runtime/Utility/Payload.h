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

#ifndef SPL_RUNTIME_PAYLOAD_H
#define SPL_RUNTIME_PAYLOAD_H

#include <SPL/Runtime/Utility/Visibility.h>

namespace SPL {
class NativeByteBuffer;
class NetworkByteBuffer;

/// class for one Payload
/// For internal use only
class DLL_PUBLIC Payload
{
  public:
    /// Destructor
    virtual ~Payload() {}

    /// Clone the payload
    /// @return new copy
    virtual Payload* clone() const = 0;

    /// Serialize the payload (binary)
    /// @param buf serialization buffer to use
    virtual void serialize(NativeByteBuffer& buf) const = 0;

    /// Deserialize the container (binary)
    /// @param buf serialization buffer to use
    virtual void deserialize(NativeByteBuffer& buf) = 0;

    /// Serialize the payload (binary)
    /// @param buf serialization buffer to use
    virtual void serialize(NetworkByteBuffer& buf) const = 0;

    /// Deserialize the container (binary)
    /// @param buf serialization buffer to use
    virtual void deserialize(NetworkByteBuffer& buf) = 0;
};

};

#endif /*SPL_RUNTIME_PAYLOAD_H*/
