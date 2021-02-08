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

#ifndef BIT_OPERATION_UTILS
#define BIT_OPERATION_UTILS

#include <UTILS/UTILSTypes.h>
#include <stdint.h>

UTILS_NAMESPACE_BEGIN

const uint64_t LONG_ONE = 1;
const uint64_t LONG_ZERO = 0;

/// Get an int64_t representative for setting a specific bit position.
/// @param position the specific position that going to set
/// @return the int64_t representative that has that specific bit set.
uint64_t getInt64BitSet(unsigned int position);

/// Set a specific bit for a number
/// @param num the number to set bit with
/// @param position the position to set
void setBitSet4Position(uint64_t& num, unsigned int position);

/// Check if a specific bit of a number is set or not
/// @param num the number to check with
/// @param position the position
/// @return true if that bit is set.
bool isBitSet4Position(const uint64_t& num, unsigned int position);

/// Set a specific range of bits for a number
/// @param num the number to set bit with
/// @param range range of bits to set (starting from position 0)
void setBitSetWithRange(uint64_t& num, unsigned int range);

/// Check if all bits of a specific range for a number is set or not
/// @param num the number to check with
/// @param range range to check with (starting from position 0)
/// @return true if all bits of specific range are set.
bool isAllBitSetWithRange(const uint64_t& num, unsigned int range);

/// Set all 64 bits for a number
/// @param num number to set bits with.
void setAllBits(uint64_t& num);

/// Check if all bits of a number are set
/// @return true if all bits of a number are set.
bool isAllBitsSet(uint64_t& num);

UTILS_NAMESPACE_END

#endif
