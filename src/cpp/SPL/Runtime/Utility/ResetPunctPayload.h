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

#ifndef SPL_RUNTIME_RESET_PAYLOAD_H
#define SPL_RUNTIME_RESET_PAYLOAD_H

#include <SPL/Runtime/Utility/Payload.h>
#include <stdint.h>
#include <string>

namespace SPL {

/// ResetMarker payload for @consistentCut
/// This payload applies to a ResetMarker only
/// For internal use only
class ResetPunctPayload : public Payload
{
  public:
    /// Payload name - "rt"
    static const std::string name;

    /// Constructor - for deserialization purposes
    ResetPunctPayload() {}

    /// Constructor
    /// @param seqID Cut identifier
    ResetPunctPayload(int64_t seqID, int32_t resetAttempt)
      : _seqID(seqID)
      , _resetAttempt(resetAttempt)
    {}

    /// clone myself
    /// @return new copy
    ResetPunctPayload* clone() const;

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

    /// Return the cut identifier
    /// @return cut ID;
    int64_t getSequenceID() const { return _seqID; }

    /// Set the reset attempt
    /// @param resetAttempt retry attempt
    void setResetAttempt(int32_t resetAttempt) { _resetAttempt = resetAttempt; }

    /// Return the reset attempt of the punctuation
    /// @return reset  attempt
    int32_t getResetAttempt() const { return _resetAttempt; }

    /// Returns the size of they payload
    /// @return payload size
    size_t getSize() const { return (sizeof(_seqID) + sizeof(_resetAttempt)); }

  private:
    int64_t _seqID;
    int32_t _resetAttempt;
};

};

#endif /*SPL_RUNTIME_RESET_PAYLOAD_H*/
