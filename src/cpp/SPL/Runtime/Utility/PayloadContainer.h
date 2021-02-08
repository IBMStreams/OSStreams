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

#ifndef SPL_RUNTIME_PAYLOAD_CONTAINER_H
#define SPL_RUNTIME_PAYLOAD_CONTAINER_H

#include <SPL/Runtime/Utility/Visibility.h>
#include <string>
#include <vector>

namespace SPL {
class Payload;
class NetworkByteBuffer;
class NativeByteBuffer;

/// Holder class to hold mutiple Payloads in a tuple.
/// For internal use only
class DLL_PUBLIC PayloadContainer
{
  public:
    /// Contructor
    PayloadContainer() {}

    /// Contructor
    /// @param rhs Container to copy
    PayloadContainer(const PayloadContainer& rhs);

    /// Destructor
    ~PayloadContainer();

    /// Add a payload to the container -- takes ownership of payload
    /// @param name Payload kind
    /// @param payload Payload to add
    /// @return 'true' if added, 'false' if name already present
    bool add(const std::string& name, Payload& payload);

    /// find a payload in the container
    /// @param name Payload to locate
    /// @return payload if matching payload found, or NULL
    Payload* find(const std::string& name) const;

    /// remove a payload from the container -- give up ownership
    /// @param name Payload to remove
    /// @return payload if matching payload found, or NULL
    Payload* remove(const std::string& name);

    /// remove a payload from the container -- give up ownership
    /// @param payload Payload to remove
    void remove(const Payload& payload);

    /// Serialize the payload container (binary)
    /// @param buf serialization buffer to use
    void serialize(NativeByteBuffer& buf) const;

    /// Deserialize the payload container (binary)
    /// @param buf serialization buffer to use
    void deserialize(NativeByteBuffer& buf);

    /// Serialize the payload container (binary)
    /// @param buf serialization buffer to use
    void serialize(NetworkByteBuffer& buf) const;

    /// Deserialize the payload container (binary)
    /// @param buf serialization buffer to use
    void deserialize(NetworkByteBuffer& buf);

    /// Is the list of payloads empty?
    /// @return 'true' if there are no payloads
    bool empty() const { return _payloads.empty(); }

    /// Create a known payload
    /// @param name Payload kind
    /// @return new uninitialized Payload
    /// @throws SPLRuntimePayloadException
    static Payload& createPayload(const std::string& name);

  private:
    std::vector<std::pair<std::string, Payload*> > _payloads;
};
};

#endif /*SPL_RUNTIME_PAYLOAD_CONTAINER_H*/
