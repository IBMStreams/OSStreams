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

#include <UTILS/DistilleryExceptionCode.h>
#include <UTILS/SerializationBuffer.h>

UTILS_NAMESPACE_USE
using namespace std;

const string DistilleryExceptionCode::NO_MESSAGE_ID = "NoMessageId";

DistilleryExceptionCode::DistilleryExceptionCode(const string& messageId)
  : _msgId(messageId){};

DistilleryExceptionCode::DistilleryExceptionCode(const DistilleryExceptionCode& ec)
  : _msgId(ec._msgId){};

DistilleryExceptionCode::DistilleryExceptionCode(SerializationBuffer& data)
  : _msgId(data.getSTLString()){};

void DistilleryExceptionCode::serialize(SerializationBuffer& s) const
{
    s.addSTLString(_msgId);
}

void DistilleryExceptionCode::print(ostream& o) const
{
    o << _msgId;
}

DistilleryExceptionCode::~DistilleryExceptionCode(void) throw() {}

ostream& UTILS_NAMESPACE::operator<<(ostream& o, const DistilleryExceptionCode& i)
{
    i.print(o);
    return o;
}
