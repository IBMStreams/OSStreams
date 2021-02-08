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

#include <SPL/Runtime/Type/Timestamp.h>

void SPL::timestamp::deserialize(std::istream& istr)
{
    char c;
    istr >> c;
    if (!istr) {
        return;
    }
    if (c != '(') {
        istr.setstate(std::ios_base::failbit);
        return;
    }
    istr >> secs_;
    if (!istr) {
        return;
    }
    istr >> c;
    if (!istr) {
        return;
    }
    if (c != ',') {
        istr.setstate(std::ios_base::failbit);
        return;
    }
    istr >> nanosecs_;
    if (!istr) {
        return;
    }
    istr >> c;
    if (!istr) {
        return;
    }
    if (c == ')') {
        machineId_ = 0;
        return;
    }
    if (c != ',') {
        istr.setstate(std::ios_base::failbit);
        return;
    }
    istr >> machineId_;
    if (!istr) {
        return;
    }
    istr >> c;
    if (!istr) {
        return;
    }
    if (c != ')') {
        istr.setstate(std::ios_base::failbit);
        return;
    }
}
