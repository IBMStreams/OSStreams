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

//----------------------------------------------------------------------------
//
// Title-
//       DistilleryExceptionCode.h
//
// Purpose-
//
//
//----------------------------------------------------------------------------
#ifndef _DISTILLERYEXCEPTIONCODES_H_
#define _DISTILLERYEXCEPTIONCODES_H_

#include <TRC/DistilleryDebug.h>
#include <UTILS/UTILSTypes.h>
#include <string>
#include <vector>

UTILS_NAMESPACE_BEGIN

/// forward declaration
class SerializationBuffer;

class DistilleryExceptionCode
{
  public:
    static const std::string NO_MESSAGE_ID;

    /// Constructor
    DistilleryExceptionCode(const std::string& messageId);

    /// Copy constructor
    /// @param ec Reference to another exception code
    DistilleryExceptionCode(const DistilleryExceptionCode& ec);

    /// Construct an exception code from a serialized version
    /// @param data Reference to the serilized buffer containing the exception code
    DistilleryExceptionCode(SerializationBuffer& data);

    /// Serialize an existing exception code to a buffer
    /// @param s Reference to the buffer when the exception code will be written
    void serialize(SerializationBuffer& s) const;

    /// Return the message id associated with this exception code, as
    /// supplied in the constuctor
    /// @return an std::string containing the message id
    inline const std::string& getMessageId(void) const { return _msgId; };

    /// Return the message id associated with this exception code, as
    /// supplied in the constuctor
    /// @return an std::string containing the message id
    void setMessageId(const std::string& id) { _msgId = id; };

    /// stream, include the backtrace
    void print(std::ostream& o) const;

    /// Destructor
    ~DistilleryExceptionCode(void) throw();

  protected:
    std::string _msgId;

    friend std::ostream& operator<<(std::ostream& o, const DistilleryExceptionCode& e);
};

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
