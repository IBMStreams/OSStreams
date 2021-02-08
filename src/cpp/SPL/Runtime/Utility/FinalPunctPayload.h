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

#ifndef SPL_RUNTIME_FINAL_PUNCT_PAYLOAD_H
#define SPL_RUNTIME_FINAL_PUNCT_PAYLOAD_H

#include <SPL/Runtime/Utility/Payload.h>
#include <stdint.h>
#include <string>

namespace SPL {

/// FinalPunct Payload
/// For internal use only
class FinalPunctPayload : public Payload
{
  public:
    /// Payload name - "finalPunct"
    static const std::string name;

    /// Constructor - for deserialization purposes
    FinalPunctPayload() {}

    /// Constructor
    ///
    FinalPunctPayload(uint64_t peID, uint32_t opIndex, uint32_t portIndex)
      : _peID(peID)
      , _opIndex(opIndex)
      , _portIndex(portIndex)
    {}

    /// clone myself
    ///@return new copy
    FinalPunctPayload* clone() const;

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

    /// Set the peID
    /// @param peID ID
    void setPeID(uint64_t peID) { _peID = peID; }

    /// Return the peID
    /// @return pe ID;
    uint32_t getPeID() const { return _peID; }

    /// Set the opIndex
    /// @param opIndex operator ID
    void setOpIndex(uint32_t opIndex) { _opIndex = opIndex; }

    /// Return the operator index
    /// @return Operator Index
    uint32_t getOpIndex() const { return _opIndex; }

    /// Set the port index
    /// @param portIndex port index
    void setPortIndex(uint64_t portIndex) { _portIndex = portIndex; }

    /// Return the port index
    /// @return the port index
    uint64_t getPortIndex() const { return _portIndex; }

  private:
    uint64_t _peID;
    uint32_t _opIndex;
    uint32_t _portIndex;
};

};

#endif /*SPL_RUNTIME_FINAL_PUNCT_PAYLOAD_H*/
