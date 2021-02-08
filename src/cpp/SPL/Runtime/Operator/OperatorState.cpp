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

#include <SPL/Runtime/Operator/OperatorState.h>

#include <SPL/Runtime/Common/RuntimeException.h>
#include <UTILS/SupportFunctions.h>
#include <UTILS/auto_array.h>

#include <cassert>
#include <limits.h>

using namespace SPL;
using namespace Distillery;
using namespace std;

OperatorState::OperatorState(bool own /*=true*/)
  : own_(own)
  , numOfOperators_(0)
  , totalStateSize_(0)
  , totalCheckpointSize_(0)
{
    timestamp_.tv_sec = 0;
    timestamp_.tv_usec = 0;
}

OperatorState::~OperatorState()
{
    clear();
}

void OperatorState::clear()
{
    if (own_) {
        for (uint32_t i = 0; i < numOfOperators_; ++i) {
            if (operatorStateSizes_[i] != 0) {
                delete[](char*) operatorStates_[i];
                operatorStates_[i] = NULL;
            }
        }
    }
    release();
}

void OperatorState::release()
{
    totalStateSize_ = 0;
    numOfOperators_ = 0;
    totalCheckpointSize_ = 0;

    timestamp_.tv_sec = 0;
    timestamp_.tv_usec = 0;

    operatorStates_.clear();
    operatorStateSizes_.clear();
}

uint32_t OperatorState::getTotalStateSize() const
{
    return totalStateSize_;
}

uint32_t OperatorState::getNumOfOperators() const
{
    return numOfOperators_;
}

// Signature is computed over the state and the control data
void OperatorState::generateSignature(string& signature)
{
    if (numOfOperators_ == 0) {
        THROW_CHAR(SPLRuntimeInvalidCheckpoint,
                   "Cannot compute checkpoint signature. Invalid state size.");
    }

    Distillery::auto_array<char> wholeCheckpointPtr(new char[totalCheckpointSize_]);
    char* wholeCheckpoint = wholeCheckpointPtr.get();
    memcpy(wholeCheckpoint, &totalCheckpointSize_, sizeof(totalCheckpointSize_));
    wholeCheckpoint += sizeof(totalCheckpointSize_);
    memcpy(wholeCheckpoint, &timestamp_, sizeof(timestamp_));
    wholeCheckpoint += sizeof(timestamp_);
    memcpy(wholeCheckpoint, &numOfOperators_, sizeof(numOfOperators_));
    wholeCheckpoint += sizeof(numOfOperators_);
    for (uint32_t i = 0; i < numOfOperators_; ++i) {
        memcpy(wholeCheckpoint, &operatorStateSizes_[i], sizeof(operatorStateSizes_[i]));
        wholeCheckpoint += sizeof(operatorStateSizes_[i]);
    }
    for (uint32_t i = 0; i < numOfOperators_; ++i) {
        if (operatorStateSizes_[i] != 0) {
            memcpy(wholeCheckpoint, operatorStates_[i], operatorStateSizes_[i]);
            wholeCheckpoint += operatorStateSizes_[i];
        }
    }

    signature.clear();
    wholeCheckpoint = wholeCheckpointPtr.get();
    computeBase64SHA1Digest(wholeCheckpoint, totalCheckpointSize_, signature);
}

bool OperatorState::isValidCheckpoint(char* signature, char* checkpoint)
{
    string newSignature;
    computeBase64SHA1Digest(checkpoint, totalCheckpointSize_, newSignature);
    if (0 != strncmp(signature, newSignature.data(), newSignature.size())) {
        return false;
    }
    return true;
}

#define SIGNATURE_SIZE 28
#define MAX_CHECKPOINT_SIZE (INT_MAX)

void OperatorState::saveState(ofstream& cp)
{
    if (!cp || !cp.is_open()) {
        THROW_CHAR(SPLRuntimeInvalidCheckpoint, "Invalid reference to checkpoint file");
    }

    if (numOfOperators_ == 0) {
        THROW_CHAR(SPLRuntimeInvalidCheckpoint, "Invalid number of operators");
    }

    gettimeofday(&timestamp_, NULL);

    // Required so we can check the signature against the whole data, does not include signature
    totalCheckpointSize_ = sizeof(totalCheckpointSize_) + sizeof(timestamp_) +
                           sizeof(numOfOperators_) + (sizeof(uint32_t) * numOfOperators_) +
                           totalStateSize_;

    if (totalCheckpointSize_ > MAX_CHECKPOINT_SIZE) {
        THROW_CHAR(SPLRuntimeInvalidCheckpoint, "Invalid checkpoint size");
    }

    string signature;
    generateSignature(signature);

    cp.exceptions(ofstream::eofbit | ofstream::failbit | ofstream::badbit);
    try {
        cp.seekp(ios_base::beg);
        cp.write((char*)signature.data(), signature.size());
        cp.write((char*)&totalCheckpointSize_, sizeof(totalCheckpointSize_));
        cp.write((char*)&timestamp_, sizeof(timestamp_));
        cp.write((char*)&numOfOperators_, sizeof(numOfOperators_));
        for (uint32_t i = 0; i < numOfOperators_; ++i) {
            cp.write((char*)&operatorStateSizes_[i], sizeof(operatorStateSizes_[i]));
        }
        for (uint32_t i = 0; i < numOfOperators_; ++i) {
            if (operatorStateSizes_[i] != 0) {
                cp.write((char*)operatorStates_[i], operatorStateSizes_[i]);
            }
        }
        cp.flush();
    } catch (std::exception const& e) {
        THROW(SPLRuntime, "Error during checkpoint save, details: '" << e.what() << "'");
    }
}

void OperatorState::restoreState(ifstream& cp)
{
    clear();

    // logic to read the state from the file
    if (!cp || !cp.is_open()) {
        THROW_CHAR(SPLRuntimeInvalidCheckpoint, "Invalid reference to checkpoint file");
    }

    cp.seekg(ios_base::beg);

    // Check signature
    char signature[SIGNATURE_SIZE];
    cp.read(signature, sizeof(signature));
    cp.read((char*)&totalCheckpointSize_, sizeof(totalCheckpointSize_));

    if (totalCheckpointSize_ > MAX_CHECKPOINT_SIZE) {
        THROW_CHAR(SPLRuntimeInvalidCheckpoint, "Invalid checkpoint size");
    }

    Distillery::auto_array<char> checkpointPtr(new char[totalCheckpointSize_]);
    char* checkpoint = checkpointPtr.get();

    cp.exceptions(ifstream::eofbit | ifstream::failbit | ifstream::badbit);
    uint32_t readCheckpointSize = 0;
    try {
        cp.seekg(sizeof(signature));
        cp.read(checkpoint, totalCheckpointSize_);
        readCheckpointSize = cp.gcount();
    } catch (std::exception const& e) {
        THROW(SPLRuntime, "Error during checkpoint restore, details: '" << e.what() << "'");
    }

    if (readCheckpointSize != totalCheckpointSize_) {
        THROW_CHAR(SPLRuntimeInvalidCheckpoint, "Checkpoint expected size and read size differ");
    }

    if (!isValidCheckpoint(signature, checkpoint)) {
        THROW_CHAR(SPLRuntimeInvalidCheckpoint, "Checkpoint signature is invalid");
    }

    char const* rawmem = checkpoint + sizeof(totalCheckpointSize_);
    memcpy(&timestamp_, rawmem, sizeof(timestamp_));
    rawmem += sizeof(timestamp_);
    memcpy((void*)&numOfOperators_, rawmem, sizeof(numOfOperators_));
    rawmem += sizeof(numOfOperators_);

    for (uint32_t i = 0; i < numOfOperators_; ++i) {
        uint32_t opsize;
        memcpy(&opsize, rawmem, sizeof(opsize));
        rawmem += sizeof(opsize);
        operatorStateSizes_.push_back(opsize);
        totalStateSize_ += opsize;
    }

    for (uint32_t i = 0; i < numOfOperators_; ++i) {
        char* opstate;
        if (operatorStateSizes_[i] != 0) {
            opstate = new char[operatorStateSizes_[i]];
            memcpy(opstate, rawmem, operatorStateSizes_[i]);
            rawmem += operatorStateSizes_[i];
            operatorStates_.push_back(opstate);
        } else {
            operatorStates_.push_back(NULL);
        }
    }
}

void OperatorState::addOperatorState(void const* state, uint32_t size)
{
    if (size > MAX_CHECKPOINT_SIZE) {
        THROW_CHAR(SPLRuntimeInvalidCheckpoint, "Illegal checkpoint size");
    }

    operatorStateSizes_.push_back(size);
    operatorStates_.push_back(state);

    ++numOfOperators_;
    totalStateSize_ += size;
}

static void checkIndex(uint32_t index, uint32_t size)
{
    if (index >= size) {
        THROW(SPLRuntimeInvalidIndex,
              "Operator index '" << index << "' is out of range [0.." << size << "]");
    }
}

void const* OperatorState::getOperatorState(uint32_t index) const
{
    checkIndex(index, numOfOperators_);
    return operatorStates_[index];
}

uint32_t OperatorState::getOperatorStateSize(uint32_t index) const
{
    checkIndex(index, numOfOperators_);
    return operatorStateSizes_[index];
}
