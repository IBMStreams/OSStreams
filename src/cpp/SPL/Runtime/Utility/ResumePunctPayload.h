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

#ifndef SPL_RUNTIME_RESUME_PAYLOAD_H
#define SPL_RUNTIME_RESUME_PAYLOAD_H

#include <SPL/Runtime/Utility/Payload.h>
#include <stdint.h>
#include <string>

namespace SPL {

/// ResumeMarker payload for @consistent
/// This payload applies to a ResumeMarker only
/// For internal use only
class ResumePunctPayload : public Payload
{
  public:
    /// Payload name - "rs"
    static const std::string name;

    /// Constructor - for deserialization purposes
    ResumePunctPayload() {}

    /// Constructor
    /// @param seqID sequence identifier
    ResumePunctPayload(int64_t seqID, int64_t retiredID)
      : _seqID(seqID)
      , _retiredID(retiredID)
    {}

    /// clone myself
    ///@return new copy
    ResumePunctPayload* clone() const;

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

    /// Return the retired sequence identifier
    /// @return retired sequence ID
    int64_t getRetiredID() const { return _retiredID; }

    size_t getSize() const { return (sizeof(_seqID) + sizeof(_retiredID)); }

  private:
    int64_t _seqID;
    int64_t _retiredID;
};

};

#endif /*SPL_RUNTIME_RESUME_PAYLOAD_H*/
