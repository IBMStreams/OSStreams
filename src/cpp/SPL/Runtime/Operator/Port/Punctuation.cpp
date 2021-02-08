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

#include <SPL/Runtime/Operator/Port/Punctuation.h>

#include <SPL/Runtime/Utility/DrainPunctPayload.h>
#include <SPL/Runtime/Utility/PayloadContainer.h>
#include <SPL/Runtime/Utility/ResetPunctPayload.h>
#include <SPL/Runtime/Utility/WatermarkPunctPayload.h>

#include <iostream>

using namespace std;
using namespace SPL;

Punctuation::~Punctuation()
{
    delete payload_;
}

void Punctuation::setPayloadContainer(Punctuation const& rhs)
{
    if (rhs.payload_) {
        payload_ = new PayloadContainer(*rhs.payload_);
    } else {
        payload_ = NULL;
    }
}

void Punctuation::setPayloadContainer(PayloadContainer* payload)
{
    if (payload == payload_) {
        return;
    }
    delete payload_;
    payload_ = payload;
}

int32_t Punctuation::getResetAttempt() const
{
    if (value_ != Punctuation::ResetMarker) {
        return -1;
    }
    assert(payload_ != NULL);
    ResetPunctPayload* rpayload =
      static_cast<ResetPunctPayload*>(payload_->find(ResetPunctPayload::name));
    assert(rpayload != NULL);
    return rpayload->getResetAttempt();
}

Punctuation& Punctuation::operator=(Punctuation const& rhs)
{
    if (this != &rhs) {
        value_ = rhs.value_;
        delete payload_;
        setPayloadContainer(rhs);
    }
    return *this;
}

ostream& SPL::operator<<(ostream& ostr, Punctuation const& punct)
{
    switch (punct.value_) {
        case Punctuation::WindowMarker:
            ostr << "WindowMarker";
            break;
        case Punctuation::FinalMarker:
            ostr << "FinalMarker";
            break;
        case Punctuation::SwitchMarker:
            ostr << "SwitchMarker";
            break;
        case Punctuation::DrainMarker: {
            // TODO: For now, print DrainMarker payload here.
            assert(punct.getPayloadContainer() != NULL);
            DrainPunctPayload* cpayload = static_cast<DrainPunctPayload*>(
              punct.getPayloadContainer()->find(DrainPunctPayload::name));
            assert(cpayload != NULL);
            int64_t seqID = cpayload->getSequenceID();

            ostr << "DrainMarker " << seqID;
            break;
        }
        case Punctuation::ResetMarker: {
            // TODO: For now, print ResetMarker payload here.
            assert(punct.getPayloadContainer() != NULL);
            ResetPunctPayload* rpayload = static_cast<ResetPunctPayload*>(
              punct.getPayloadContainer()->find(ResetPunctPayload::name));
            assert(rpayload != NULL);
            int64_t seqID = rpayload->getSequenceID();
            int32_t resetAttempt = rpayload->getResetAttempt();

            ostr << "ResetMarker " << seqID << " " << resetAttempt;
            break;
        }
        case Punctuation::WatermarkMarker: {
            // TODO Create virtual Payload::serialize(ostream&) and implement for descendants
            assert(punct.getPayloadContainer() != NULL);
            WatermarkPunctPayload* payload = static_cast<WatermarkPunctPayload*>(
              punct.getPayloadContainer()->find(WatermarkPunctPayload::name));
            assert(payload != NULL);
            ostr << "WatermarkMarker ";
            payload->serialize(ostr);
            break;
        }
        case Punctuation::ResumeMarker:
            ostr << "ResumeMarker";
            break;
        case Punctuation::Invalid:
            ostr << "InvalidMarker";
            break;
        default:
            assert(!"cannot happen");
    }
    return ostr;
}

istream& SPL::operator>>(istream& istr, Punctuation& punct)
{
    std::string token;
    istr >> token;
    if (token == "WindowMarker") {
        punct = Punctuation::WindowMarker;
    } else if (token == "FinalMarker") {
        punct = Punctuation::WindowMarker;
    } else if (token == "SwitchMarker") {
        punct = Punctuation::SwitchMarker;
    } else if (token == "DrainMarker") {
        punct = Punctuation::DrainMarker;
    } else if (token == "ResetMarker") {
        punct = Punctuation::ResetMarker;
    } else if (token == "ResumeMarker") {
        punct = Punctuation::ResumeMarker;
        // TODO Punctuation::WatermarkMarker and payload deserialization
    } else {
        punct = Punctuation::Invalid;
    }
    return istr;
}
