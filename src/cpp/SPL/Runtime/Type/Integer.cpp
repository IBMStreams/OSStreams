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

#include <SPL/Runtime/Type/Integer.h>
#include <limits>

using namespace SPL;

std::istream& SPL::operator>>(std::istream& istr, boolean& x)
{
    char c;
    istr >> c;
    if (c == 't') {
        std::string rest = "rue";
        for (size_t i = 0, iu = rest.size(); i < iu; ++i) {
            if (istr.peek() != rest[i]) {
                istr.setstate(std::ios_base::failbit);
                return istr;
            }
            istr.get(c);
        }
        x = true;
    } else if (c == 'f') {
        std::string rest = "alse";
        for (size_t i = 0, iu = rest.size(); i < iu; ++i) {
            if (istr.peek() != rest[i]) {
                istr.setstate(std::ios_base::failbit);
                return istr;
            }
            istr.get(c);
        }
        x = false;
    } else {
        istr.setstate(std::ios_base::failbit);
    }
    return istr;
}

std::istream& SPL::operator>>(std::istream& istr, int8& x)
{
    int16_t y;
    istr >> y;
    if (istr) {
        if (y > std::numeric_limits<int8_t>::max() || y < std::numeric_limits<int8_t>::min()) {
            istr.setstate(std::ios_base::failbit);
        } else {
            x = static_cast<uint8>(y);
        }
    }
    return istr;
}

std::istream& SPL::operator>>(std::istream& istr, uint8& x)
{
    uint16_t y;
    istr >> y;
    if (istr) {
        if (y > std::numeric_limits<uint8_t>::max()) {
            istr.setstate(std::ios_base::failbit);
        } else {
            x = static_cast<uint8>(y);
        }
    }
    return istr;
}
