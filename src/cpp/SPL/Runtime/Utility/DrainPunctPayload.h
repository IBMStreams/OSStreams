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

#ifndef SPL_RUNTIME_DRAIN_PAYLOAD_H
#define SPL_RUNTIME_DRAIN_PAYLOAD_H

#include <SPL/Runtime/Utility/Payload.h>
#include <stdint.h>
#include <string>

namespace SPL {

/// DrainMarker payload for @consistent
/// This payload applies to a DrainMarker only
/// For internal use only
class DrainPunctPayload : public Payload
{
  public:
    /// Payload name - "dr"
    static const std::string name;

    /// Constructor - for deserialization purposes
    DrainPunctPayload() {}

    /// Constructor
    /// @param seqID sequence identifier
    DrainPunctPayload(int64_t seqID)
      : _seqID(seqID)
    {}

    /// clone myself
    ///@return new copy
    DrainPunctPayload* clone() const;

    /// Serialize the payload (binary)
    /// @param buf serialization buffer to use
    void serialize(NativeByteBuffer& buf) const;

    /// Deserialize the container (binary)
    /// @param buf serialization buffer to use
    void deserialize(NativeByteBuffer& buf);

    /// Serialize the payload (binary)
    /// @param buf serialization buffer to use
    void serialize(NetworkByteBuffer& buf) const;

    /// Deserialize the container (binary)
    /// @param buf serialization buffer to use
    void deserialize(NetworkByteBuffer& buf);

    /// Return the sequence identifier
    /// @return sequence ID;
    int64_t getSequenceID() const { return _seqID; }

    size_t getSize() const { return (sizeof(_seqID)); }

  private:
    int64_t _seqID;
};

};

#endif /*SPL_RUNTIME_DRAIN_PAYLOAD_H*/
