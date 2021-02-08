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

/*
 * Implementation of SPL::Checkpoint class
 */

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Function/TimeFunctions.h>
#include <SPL/Runtime/Operator/State/Checkpoint.h>
#include <SPL/Runtime/Operator/State/CheckpointBatch.h>
#include <SPL/Runtime/Operator/State/CheckpointBufferCache.h>
#include <SPL/Runtime/Operator/State/CheckpointNaming.h>
#include <SPL/Runtime/Operator/State/DataStoreByteBuffer.h>
#include <SPL/Runtime/Operator/State/DataStoreEntry.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/Operator/State/IncrementalCheckpointIndex.h>
#include <assert.h>
#include <exception>
#include <limits>
#include <memory>
#include <string.h>
#include <utility>
#include <vector>

using namespace std;
using namespace SPL;

Checkpoint::Checkpoint(DataStoreEntry* storeEntry,
                       const int64_t& id,
                       const bool mode,
                       const uint64_t& size)
  : entry_(storeEntry)
  , buffer_(NULL)
  , id_(id)
  , mode_(mode)
  , state_(INPROGRESS)
  , checkpointCounter_(0)
  , normBuffer_(NULL)
  , deltaBuffer_(NULL)
  , indexBuffer_(NULL)
  , baseID_(id)
  , incrementalCkptInterval_(1)
  , enableLogging_(false)
  , deAllocBufferCache_(true)
  , indexBufferCache_(NULL)
  , deltaBufferCache_(NULL)
  , batch_(NULL)
{
    assert(storeEntry != NULL);

    initialize(size, 0, 0);
}

Checkpoint::Checkpoint(DataStoreEntry* storeEntry,
                       const int64_t& id,
                       const bool mode,
                       const int64_t& baseID,
                       const int64_t& ckptCount,
                       const uint32_t incrCkptInterval,
                       const bool enableLogging,
                       const uint64_t& normSize,
                       const int64_t& deltaSize,
                       const int64_t& indexSize,
                       CheckpointBatch* batch)
  : entry_(storeEntry)
  , buffer_(NULL)
  , id_(id)
  , mode_(mode)
  , state_(INPROGRESS)
  , checkpointCounter_(ckptCount)
  , normBuffer_(NULL)
  , deltaBuffer_(NULL)
  , indexBuffer_(NULL)
  , baseID_(baseID)
  , incrementalCkptInterval_(incrCkptInterval)
  , enableLogging_(enableLogging)
  , deAllocBufferCache_(true)
  , indexBufferCache_(NULL)
  , deltaBufferCache_(NULL)
  , batch_(batch)
{
    assert(storeEntry != NULL);

    initialize(normSize, deltaSize, indexSize);
}

void Checkpoint::cleanup()
{
    delete normBuffer_;
    normBuffer_ = NULL;
    if (mode_ == true) {
        delete deltaBuffer_;
        deltaBuffer_ = NULL;
        delete indexBuffer_;
        indexBuffer_ = NULL;
    }
    delete indexBufferCache_;
    indexBufferCache_ = NULL;
    delete deltaBufferCache_;
    deltaBufferCache_ = NULL;
    if (batch_) {
        batch_->setState(CheckpointBatch::ERROR);
    }
}

void Checkpoint::initialize(const int64_t& normSize,
                            const int64_t& deltaSize,
                            const int64_t& indexSize)
{
    DataStoreByteBuffer::Options options;
    options.mode =
      (mode_ == true) ? DataStoreByteBuffer::BB_MODE_WRITE : DataStoreByteBuffer::BB_MODE_READ;
    options.chunkSize = entry_->getDefaultChunkSize();
    options.truncate = false;
    options.startOffset = 0;
    DataStoreUpdateBatch* batch = batch_ ? batch_->getBatchImpl() : NULL;
    try {
        // open a Data Store Byte Buffer for Checkpoint data
        options.totalSize = normSize;
        normBuffer_ = entry_->openByteBuffer(CheckpointNaming::getDataKey(id_), options, batch);
        assert(normBuffer_);
        buffer_ = normBuffer_; // default to non-incremetnal checkpoint
        if (mode_ == true) {   // checkpointing
            options.totalSize = deltaSize;
            deltaBuffer_ =
              entry_->openByteBuffer(CheckpointNaming::getIncrementalDataKey(id_), options, batch);
            assert(deltaBuffer_);
            options.totalSize = indexSize;
            indexBuffer_ =
              entry_->openByteBuffer(CheckpointNaming::getIncrementalIndexKey(id_), options, batch);
            assert(indexBuffer_);
        } else { // restoring
            uint8_t metaSize;
            const char* meta = normBuffer_->getUserMetaData(metaSize);
            if (meta != NULL && metaSize == 21) {
                if (meta[0] == 1) {
                    // this checkpoint has incremental checkpoint data
                    indexBufferCache_ = new CheckpointBufferCache(entry_);
                    deltaBufferCache_ = new CheckpointBufferCache(entry_);
                    indexBuffer_ = indexBufferCache_->getBuffer(
                      CheckpointNaming::getIncrementalIndexKey(id_), 0);
                    assert(indexBuffer_);
                    deltaBuffer_ =
                      deltaBufferCache_->getBuffer(CheckpointNaming::getIncrementalDataKey(id_), 0);
                    assert(deltaBuffer_);
                }
                memcpy(&baseID_, meta + 1, sizeof(int64_t));
                memcpy(&checkpointCounter_, meta + 9, sizeof(int64_t));
                memcpy(&incrementalCkptInterval_, meta + 17, sizeof(uint32_t));
            } else {
                THROW_CHAR(DataStore, "Cannot read meta-data");
            }
        }
    } catch (DataStoreException const& e) {
        cleanup();
        THROW_NESTED(DataStore, "Cannot create Checkpoint instance", e);
    } catch (std::exception const& e) {
        cleanup();
        THROW(DataStore, "Cannot create Checkpoint instance: received exception: " << e.what());
    } catch (...) {
        cleanup();
        THROW(DataStore, "Cannot create Checkpoint instance: received unknown exception");
    }
}

Checkpoint::~Checkpoint()
{
    bool hasNormBuffer = false;
    bool hasDeltaBuffer = false;
    bool hasIndexBuffer = false;
    if (normBuffer_) {
        delete normBuffer_;
        hasNormBuffer = true;
    }
    if (mode_ == false) {
        if (deAllocBufferCache_ == true) {
            if (indexBufferCache_) {
                delete indexBufferCache_;
            }
            if (deltaBufferCache_) {
                delete deltaBufferCache_;
            }
        }
        return;
    }
    if (deltaBuffer_) {
        delete deltaBuffer_;
        hasDeltaBuffer = true;
    }
    if (indexBuffer_) {
        delete indexBuffer_;
        hasIndexBuffer = true;
    }
    if (state_ != FINISHED) {
        // delete the checkpoint data with temporary key
        try {
            APPTRC(L_DEBUG, "Try to delete temporary checkpoint data ...", SPL_CKPT);
            if (hasNormBuffer) {
                entry_->removeByteBuffer(CheckpointNaming::getDataKey(id_));
            }
            if (hasDeltaBuffer) {
                entry_->removeByteBuffer(CheckpointNaming::getIncrementalDataKey(id_));
            }
            if (hasIndexBuffer) {
                entry_->removeByteBuffer(CheckpointNaming::getIncrementalIndexKey(id_));
            }
            APPTRC(L_DEBUG, "Temporary checkpoint data is deleted successfully.", SPL_CKPT);
        } catch (...) {
        }
    }
}

// for checkpointing/restoring primitive types
#define CHECKPOINT_ADD_FUNCTION(type, method)                                                      \
    void Checkpoint::add##method(type value)                                                       \
    {                                                                                              \
        try {                                                                                      \
            buffer_->add##method(value);                                                           \
        } catch (DataStoreException const& e) {                                                    \
            exceptionHandler(e, "Checkpointing failed");                                           \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
    type Checkpoint::get##method() { return buffer_->get##method(); }

CHECKPOINT_ADD_FUNCTION(char, Char)
CHECKPOINT_ADD_FUNCTION(bool, Bool)
CHECKPOINT_ADD_FUNCTION(uint8_t, UInt8)
CHECKPOINT_ADD_FUNCTION(int8_t, Int8)
CHECKPOINT_ADD_FUNCTION(uint16_t, UInt16)
CHECKPOINT_ADD_FUNCTION(uint32_t, UInt32)
CHECKPOINT_ADD_FUNCTION(uint64_t, UInt64)
CHECKPOINT_ADD_FUNCTION(int16_t, Int16)
CHECKPOINT_ADD_FUNCTION(int32_t, Int32)
CHECKPOINT_ADD_FUNCTION(int64_t, Int64)
CHECKPOINT_ADD_FUNCTION(float, Float)
CHECKPOINT_ADD_FUNCTION(double, Double)
CHECKPOINT_ADD_FUNCTION(long double, LongDouble)

#undef CHECKPOINT_ADD_FUNCTION

void Checkpoint::addNTStr(char const* value)
{
    try {
        buffer_->addNTStr(value);
    } catch (DataStoreException const& e) {
        exceptionHandler(e, "Checkpointing failed");
    }
}

char* Checkpoint::getNTStr()
{
    return buffer_->getNTStr();
}

void Checkpoint::addSTLString(std::string const& value)
{
    try {
        buffer_->addSTLString(value);
    } catch (DataStoreException const& e) {
        exceptionHandler(e, "Checkpointing failed");
    }
}

void Checkpoint::getSTLString(std::string& value)
{
    buffer_->getSTLString(value);
}

std::string Checkpoint::getSTLString()
{
    return buffer_->getSTLString();
}

void Checkpoint::addSPLString(RSTRING_BB_TYPE const& value)
{
    try {
        buffer_->addSPLString(value);
    } catch (DataStoreException const& e) {
        exceptionHandler(e, "Checkpointing failed");
    }
}

void Checkpoint::getSPLString(RSTRING_BB_TYPE& value)
{
    buffer_->getSPLString(value);
}

RSTRING_BB_TYPE
Checkpoint::getSPLString()
{
    return buffer_->getSPLString();
}

void Checkpoint::addUnicodeString(USTRING_BB_TYPE const& value)
{
    try {
        buffer_->addUnicodeString(value);
    } catch (DataStoreException const& e) {
        exceptionHandler(e, "Checkpointing failed");
    }
}

USTRING_BB_TYPE
Checkpoint::getUnicodeString()
{
    return buffer_->getUnicodeString();
}

void Checkpoint::addBlob(const void* blob, const uint32_t blobsize)
{
    try {
        buffer_->addBlob(blob, blobsize);
    } catch (DataStoreException const& e) {
        exceptionHandler(e, "Checkpointing failed");
    }
}

unsigned char* Checkpoint::getBlob(uint32_t& size)
{
    return buffer_->getBlob(size);
}

void Checkpoint::addPointer(void const* ptr)
{
    try {
        buffer_->addPointer(ptr);
    } catch (DataStoreException const& e) {
        exceptionHandler(e, "Checkpointing failed");
    }
}

void* Checkpoint::getPointer()
{
    return buffer_->getPointer();
}

void Checkpoint::addCharSequence(char const* chbuf, uint32_t chbufsize)
{
    try {
        buffer_->addCharSequence(chbuf, chbufsize);
    } catch (DataStoreException const& e) {
        exceptionHandler(e, "Checkpointing failed");
    }
}

char* Checkpoint::getCharSequence(uint32_t& sizeTillEOB)
{
    return buffer_->getCharSequence(sizeTillEOB);
}

char* Checkpoint::getFixedCharSequence(uint32_t mysize)
{
    return buffer_->getFixedCharSequence(mysize);
}

void Checkpoint::getFixedCharSequence(char* buffer, uint64_t bsize, uint64_t& returnSize)
{
    return buffer_->getFixedCharSequence(buffer, bsize, returnSize);
}

uint32_t Checkpoint::getContentSize() const
{
    uint64_t totalSize;
    getContentSize(totalSize);
    if (totalSize <= std::numeric_limits<unsigned int>::max()) {
        return uint32_t(totalSize);
    } else {
        THROW(DataStore, "getContentSize() failed: size (" << totalSize
                                                           << ") larger than 32-bit integer limit");
    }
}

void Checkpoint::getContentSize(uint64_t& size) const
{
    size = getNormDataSize() + getIncrementalDataSize() + getIncrementalIndexSize();
}

void Checkpoint::finishCheckpointing()
{
    if (mode_ == true) {
        if (state_ == ERROR) { // there has been error
            THROW(DataStore, "Cannot finish checkpointing due to previous error");
        }
        try {
            // flush buffer for the new Checkpoint data
            char meta[21];
            meta[0] = 0;
            uint64_t indexBufferSize = 0;
            indexBuffer_->getContentSize(indexBufferSize);
            if (indexBufferSize > 0) {
                indexBuffer_->finishWrite();
                deltaBuffer_->finishWrite();
                meta[0] = 1;
            }
            memcpy(meta + 1, &baseID_, sizeof(int64_t));
            memcpy(meta + 9, &checkpointCounter_, sizeof(int64_t));
            memcpy(meta + 17, &incrementalCkptInterval_, sizeof(uint32_t));
            normBuffer_->finishWrite(meta, 21);
            state_ = FINISHED;
        } catch (DataStoreException const& e) {
            exceptionHandler(e, "Cannot finish checkpointing");
        } catch (std::exception const& e) {
            state_ = ERROR;
            THROW(DataStore, "Cannot finish checkpointing: received exception: " << e.what());
        } catch (...) {
            state_ = ERROR;
            THROW(DataStore, "Cannot finish checkpointing: received unknown exception");
        }
    } else {
        THROW_CHAR(DataStore, "finishCheckpointing() cannot be called in restore mode");
    }
}

double Checkpoint::getTimestamp() const
{
    if (!mode_) {
        SPL::timestamp myTime = buffer_->getCreateTime();
        return myTime.getSeconds() + double(myTime.getNanoseconds()) / 1e9;
    }
    THROW_CHAR(DataStore, "getTimestamp() cannot be called when checkpointing is in progress");
}

void Checkpoint::exceptionHandler(DataStoreException const& e, string const& msg)
{
    if (batch_) {
        batch_->setState(CheckpointBatch::ERROR);
    }
    state_ = ERROR;
    THROW_NESTED(DataStore, msg, e);
}

Checkpoint::Checkpoint(Checkpoint& ckpt, const int64_t& id, const uint64_t& offset)
  : entry_(ckpt.entry_)
  , buffer_(NULL)
  , id_(id)
  , mode_(ckpt.mode_)
  , state_(INPROGRESS)
  , checkpointCounter_(1)
  , normBuffer_(NULL)
  , deltaBuffer_(NULL)
  , indexBuffer_(NULL)
  , baseID_(ckpt.getBaseID())
  , incrementalCkptInterval_(ckpt.getIncrementalCheckpointInterval())
  , enableLogging_(false)
  , deAllocBufferCache_(false)
  , indexBufferCache_(ckpt.indexBufferCache_)
  , deltaBufferCache_(ckpt.deltaBufferCache_)
{
    assert(ckpt.mode_ == false);

    try {
        // open a Data Store Byte Buffer for Checkpoint data
        deltaBuffer_ =
          deltaBufferCache_->getBuffer(CheckpointNaming::getIncrementalDataKey(id), offset);
        assert(deltaBuffer_);
        buffer_ = deltaBuffer_;
        indexBuffer_ = indexBufferCache_->getBuffer(CheckpointNaming::getIncrementalIndexKey(id));
        assert(indexBuffer_);
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "Cannot create Checkpoint instance", e);
    } catch (std::exception const& e) {
        THROW(DataStore, "Cannot create Checkpoint instance: received exception: " << e.what());
    } catch (...) {
        THROW(DataStore, "Cannot create Checkpoint instance: received unknown exception");
    }
}

uint64_t Checkpoint::getNormDataSize() const
{
    uint64_t size = 0;
    if (normBuffer_) {
        normBuffer_->getContentSize(size);
    }
    return size;
}

uint64_t Checkpoint::getIncrementalDataSize() const
{
    uint64_t size = 0;
    if (deltaBuffer_) {
        deltaBuffer_->getContentSize(size);
    }
    return size;
}

uint64_t Checkpoint::getIncrementalIndexSize() const
{
    uint64_t size = 0;
    if (indexBuffer_) {
        indexBuffer_->getContentSize(size);
    }
    return size;
}

void Checkpoint::setIncrementalMode()
{
    buffer_ = deltaBuffer_;
}

void Checkpoint::unsetIncrementalMode()
{
    buffer_ = normBuffer_;
}

bool Checkpoint::isIncrementalMode() const
{
    return (buffer_ == deltaBuffer_);
}

DataStoreByteBuffer* Checkpoint::getIndexBuffer(const int64_t& id, const uint64_t& offset)
{
    if (indexBufferCache_) {
        return indexBufferCache_->getBuffer(CheckpointNaming::getIncrementalIndexKey(id), offset);
    } else {
        return NULL;
    }
}

DataStoreByteBuffer* Checkpoint::getIndexBuffer(const int64_t& id)
{
    if (indexBufferCache_) {
        return indexBufferCache_->getBuffer(CheckpointNaming::getIncrementalIndexKey(id));
    } else {
        return NULL;
    }
}

void Checkpoint::setOCursor(uint64_t offset)
{
    buffer_->setOCursor(offset);
}

bool Checkpoint::isBase() const
{
    return baseID_ == id_;
}

int64_t Checkpoint::getBaseID() const
{
    return baseID_;
}

int64_t Checkpoint::getCheckpointCount() const
{
    return checkpointCounter_;
}

uint32_t Checkpoint::getIncrementalCheckpointInterval() const
{
    return incrementalCkptInterval_;
}

bool Checkpoint::shouldEnableLogging() const
{
    return enableLogging_;
}
