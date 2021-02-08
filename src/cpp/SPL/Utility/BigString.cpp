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

#include <SPL/Utility/BigString.h>
#include <UTILS/SupportFunctions.h>

using namespace SPL;

bool BigString::operator==(const BigString& oth) const
{
    if (sig_ != oth.sig_) {
        return false;
    }
    return str_ == oth.str_;
}

void BigString::recomputeSignature()
{
    Distillery::computeBase64SHA1Digest(str_.c_str(), str_.size(), sig_);
}
