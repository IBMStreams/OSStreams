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

#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/BitOperationUtils.h>

using namespace std;
UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE

uint64_t UTILS_NAMESPACE::getInt64BitSet(unsigned int position)
{
    if (position < 64) {
        return LONG_ONE << position;
    } else {
        return LONG_ZERO;
    }
}

void UTILS_NAMESPACE::setBitSet4Position(uint64_t& num, unsigned int position)
{
    SPCDBG(L_DEBUG, "Enter: set bit " << position << " of number " << num, CORE_UTILS);
    uint64_t bitSet = getInt64BitSet(position);
    SPCDBG(L_DEBUG, "Got Int64BitSet number: " << bitSet, CORE_UTILS);
    num = num | bitSet;
    SPCDBG(L_DEBUG, "Number after setting the bit: " << num, CORE_UTILS);
}

bool UTILS_NAMESPACE::isBitSet4Position(const uint64_t& num, unsigned int position)
{
    uint64_t bitSet = getInt64BitSet(position);
    return ((num & bitSet) == bitSet);
}

void UTILS_NAMESPACE::setBitSetWithRange(uint64_t& num, unsigned int range)
{
    SPCDBG(L_DEBUG, "Enter: set range " << range << " of number " << num, CORE_UTILS);
    for (unsigned int i = 0; i < range; i++) {
        uint64_t bitSet = getInt64BitSet(i);
        SPCDBG(L_TRACE, "Got Int64BitSet number: " << bitSet, CORE_UTILS);
        num = num | bitSet;
    }
    SPCDBG(L_DEBUG, "Number after setting a range of bit: " << num, CORE_UTILS);
}

bool UTILS_NAMESPACE::isAllBitSetWithRange(const uint64_t& num, unsigned int range)
{
    for (unsigned int i = 0; i < range; i++) {
        uint64_t bitSet = getInt64BitSet(i);
        if ((num & bitSet) != bitSet) {
            return false;
        }
    }
    return true;
}

void UTILS_NAMESPACE::setAllBits(uint64_t& num)
{
    num = ~LONG_ZERO;
}

bool UTILS_NAMESPACE::isAllBitsSet(uint64_t& num)
{
    if ((num ^ LONG_ZERO) == num) {
        return true;
    } else {
        return false;
    }
}
