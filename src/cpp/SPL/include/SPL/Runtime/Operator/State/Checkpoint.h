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
 * \file Checkpoint.h \brief Definition of the SPL::Checkpoint class.
 */

#ifndef SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_H
#define SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_H

#include <SPL/Runtime/Serialization/ByteBuffer.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <string>
#include <tr1/unordered_map>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Type/TypeHelper.h>
#include <SPL/Runtime/Utility/Visibility.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

namespace SPL
{
#ifndef DOXYGEN_SKIP_FOR_USERS
    /// Forward declaration
    class CheckpointContextImpl;
    class DataStoreByteBuffer;
    class DataStoreEntry;
    class CheckpointBatch;
    class CheckpointBufferCache;
#endif

    /// \brief Class that represents an individual checkpoint i.e. a snapshot
    /// of the state of the operator when the checkpoint is created.
    ///
    /// A checkpoint can be logically defined as a key-value pair where the key is
    /// a unique sequence id (within the scope of the operator) and the value is the state
    /// of the operator in serialized form. Each operator may keep one or more
    /// checkpoints of their state on some backend data store.
    ///
    /// In the case of a failure and the subsequent restart, one of the saved checkpoints can
    /// be used to restore the operator to a state prior to the failure thereby minimizing
    /// data loss. Obviously, any state changes made between the last checkpoint and the failure
    /// time cannot be recovered.
    ///
    /// The lifecycle of a Checkpoint instance is as follows during checkpointing:
    /// 1. A checkpointing operation is initiated. This can happen in 2 ways:
    ///	   a. Operator code initiates an operator-driven, synchronous checkpoint by calling
    ///       CheckpointContext::createCheckpoint()
    ///    b. The SPL runtime requests a checkpoint during periodic checkpointing or Consistent Cut processing.
    /// 2. A new Checkpoint instance is created.
    /// 3. If a registered StateHandler is present, the StateHandler::checkpoint() function is called. Operator
    ///    state data are serialized into the Checkpoint instance.
    /// 4. Logical state variables (if there is any) of the operator are serialized into the Checkpoint instance.
    /// 5. The Checkpointed data are committed to backend data store and the Checkpoint instance is destroyed.
    ///
    /// During the restore process, the lifecycle of a Checkpoint instance is as follows:
    /// 1. A new Checkpoint instance is created.
    /// 2. If a registered StateHandler is present, the StateHandler::reset() function is called. Opeartor state
    ///    data are de-serialized and restored from the Checkpoint instance.
    /// 3. Logical state variables (if there is any) of the operator are de-serialized from the Checkpoint instance.
    /// 4. The Checkpoint instance is destroyed.
    ///
    /// The Checkpoint class provides addType() and getType() methods for serializing and deserializing various
    /// data types. In addition, there are overloaded operator<< and >> functions to serlalize and deserialize
    /// data of primitive, STL, and SPL types to and from a Checkpoint instance.
    ///
    /// The SPL Runtime internally transfers the checkpoint data to and from a backend data store which
    /// is specified by domain or instance level checkpointRepository and checkpointRepositoryConfiguration
    /// properties.
    class DLL_PUBLIC Checkpoint : public ByteBuffer<Checkpoint>
    {
    public:
#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Constructor
        /// User should not create Checkpoint instance directly; instead the SPL Runtime will internally
        /// create a Checkppoint instance and pass it to operator's checkpoint() and reset() callbacks.
        /// @param storeEntry Data Store Entry to contain this Checkppoint
        /// @param id Sequence ID of this Checkpoint
        /// @pram mode true for checkpointing, false for restoring
        /// @param size the size of the checkpoint data in Bytes. This can be approximate and is used as
        /// a hint to determine internal buffer size
        Checkpoint(DataStoreEntry * storeEntry, const int64_t & id, const bool mode, const uint64_t & size = 0);
#endif

        /// Destructor
        ~Checkpoint();

        /// Get the sequence identifier of this Checkpoint
        /// @return the sequence id of this Checkpoint
        int64_t getSequenceId() const
        {
            return id_;
        }

        /// Returns the time at which the checkpoint is created. This can only be called at restore time.
        /// @returns the checkpoint creation time as a double value in seconds since the Epoch
        /// @throws DataStoreException if the Checkpoint is used for checkpointing
        double getTimestamp() const;

        /// Add/serialize a character to the Checkpoint
        /// @param value a character
        /// @throws DataStoreException if the Checkpoint is used for restoring or
        /// if the given data cannot be added to Checkpoint
        void addChar(char value);

        /// Extract/de-serialize a character from the Checkpoint
        /// @return a character
        /// @throws DataStoreException if the Checkpoint is used for checkpointing or
        /// if a character cannot be extracted from Checkpoint
        char getChar();

        /// Add/serialize a boolean to the Checkpoint
        /// @param value a boolean
        /// @throws DataStoreException if the Checkpoint is used for restoring or
        /// if the given data cannot be added to Checkpoint
        void addBool(bool value);

        /// Extract/de-serialize a boolean from the Checkpoint
        /// @return a boolean
        /// @throws DataStoreException if the Checkpoint is used for checkpointing or
        /// if a boolean cannot be extracted from Checkpoint
        bool getBool();

        /// Add/serialize a uint8_t to the Checkpoint
        /// @param value a uint8_t
        /// @throws DataStoreException if the Checkpoint is used for restoring or
        /// if the given data cannot be added to Checkpoint
        void addUInt8(uint8_t value);

        /// Extract/de-serialize a uint8_t from the Checkpoint
        /// @return a uint8_t
        /// @throws DataStoreException if the Checkpoint is used for checkpointing or
        /// if a uint8_t cannot be extracted from Checkpoint
        uint8_t getUInt8();

        /// Add/serialize an int8_t to the Checkpoint
        /// @param value an int8_t
        /// @throws DataStoreException if the Checkpoint is used for restoring or
        /// if the given data cannot be added to Checkpoint
        void addInt8(int8_t value);

        /// Extract/de-serialize an int8_t from the Checkpoint
        /// @return an int8_t
        /// @throws DataStoreException if the Checkpoint is used for checkpointing or
        /// if an int8_t cannot be extracted from Checkpoint
        int8_t getInt8();

        /// Add/serialize a uint16_t to the Checkpoint
        /// @param value a uint16_t
        /// @throws DataStoreException if the Checkpoint is used for restoring or
        /// if the given data cannot be added to Checkpoint
        void addUInt16(uint16_t value);

        /// Extract/de-serialize a uint16_t from the Checkpoint
        /// @return a uint16_t
        /// @throws DataStoreException if the Checkpoint is used for checkpointing or
        /// if a uint16_t cannot be extracted from Checkpoint
        uint16_t getUInt16();

        /// Add/serialize an int16_t to the Checkpoint
        /// @param value an int16_t
        /// @throws DataStoreException if the Checkpoint is used for restoring or
        /// if the given data cannot be added to Checkpoint
        void addInt16(int16_t value);

        /// Extract/de-serialize an int16_t from the Checkpoint
        /// @return an int16_t
        /// @throws DataStoreException if the Checkpoint is used for checkpointing or
        /// if an int16_t cannot be extracted from Checkpoint
        int16_t getInt16();

        /// Add/serialize a uint32_t to the Checkpoint
        /// @param value a uint32_t
        /// @throws DataStoreException if the Checkpoint is used for restoring or
        /// if the given data cannot be added to Checkpoint
        void addUInt32(uint32_t value);

        /// Extract/de-serialize a uint32_t from the Checkpoint
        /// @return a uint32_t
        /// @throws DataStoreException if the Checkpoint is used for checkpointing or
        /// if a uint32_t cannot be extracted from Checkpoint
        uint32_t getUInt32();

        /// Add/serialize an int32_t to the Checkpoint
        /// @param value an int32_t
        /// @throws DataStoreException if the Checkpoint is used for restoring or
        /// if the given data cannot be added to Checkpoint
        void addInt32(int32_t value);

        /// Extract/de-serialize an int32_t from the Checkpoint
        /// @return an int32_t
        /// @throws DataStoreException if the Checkpoint is used for checkpointing or
        /// if an int32_t cannot be extracted from Checkpoint
        int32_t getInt32();

        /// Add/serialize a uint64_t to the Checkpoint
        /// @param value a uint64_t
        /// @throws DataStoreException if the Checkpoint is used for restoring or
        /// if the given data cannot be added to Checkpoint
        void addUInt64(uint64_t value);

        /// Extract/de-serialize a uint64_t from the Checkpoint
        /// @return a uint64_t
        /// @throws DataStoreException if the Checkpoint is used for checkpointing or
        /// if a uint64_t cannot be extracted from Checkpoint
	uint64_t getUInt64();

        /// Add/serialize an int64_t to the Checkpoint
        /// @param value an int64_t
        /// @throws DataStoreException if the Checkpoint is used for restoring or
        /// if the given data cannot be added to Checkpoint
        void addInt64(int64_t value);

        /// Extract/de-serialize an int64_t from the Checkpoint
        /// @return an int64_t
        /// @throws DataStoreException if the Checkpoint is used for checkpointing or
        /// if an int64_t cannot be extracted from Checkpoint
        int64_t getInt64();

        /// Add/serialize a float to the Checkpoint
        /// @param value a single-precision floating point number
        /// @throws DataStoreException if the Checkpoint is used for restoring or
        /// if the given data cannot be added to Checkpoint
        void addFloat(float value);

        /// Extract/de-serialize a float from the Checkpoint
        /// @return a single-precision floating point number
        /// @throws DataStoreException if the Checkpoint is used for checkpointing or
        /// if a float cannot be extracted from Checkpoint
        float getFloat();

        /// Add/serialize a double to the Checkpoint
        /// @param value a double-precision floating point number
        /// @throws DataStoreException if the Checkpoint is used for restoring or
        /// if the given data cannot be added to Checkpoint
        void addDouble(double value);

        /// Extract/de-serialize a double from the Checkpoint
        /// @return a double-precision floating point number
        /// @throws DataStoreException if the Checkpoint is used for checkpointing or
        /// if a double cannot be extracted from Checkpoint
        double getDouble();

        /// Add/serialize a long double to the Checkpoint
        /// @param value a long double floating point number
        /// @throws DataStoreException if the Checkpoint is used for restoring or
        /// if the given data cannot be added to Checkpoint
        void addLongDouble(long double value);

        /// Extract/de-serialize a long double from the Checkpoint
        /// @return a long double floating point number
        /// @throws DataStoreException if the Checkpoint is used for checkpointing or
        /// if a long double cannot be extracted from Checkpoint
        long double getLongDouble();

        /// Add/serialize a NULL-terminated string to the Checkpoint
        /// @param value a NULL terminated string
        /// @throws DataStoreException if the Checkpoint is used for restoring or
        /// if the given data cannot be added to Checkpoint
        void addNTStr(char const * value);

        /// Extract/de-serialize a NULL-terminated string from the Checkpoint.
        /// User is responsible for de-allocate the returned memory with delete[].
        /// @return a pointer to a NULL-terminated string
        /// @throws DataStoreException if the Checkpoint is used for checkpointing or
        /// if a NULL-terminated string cannot be extracted from  the Checkpoint
        char * getNTStr();

        /// Add/serialize an STL string to the Checkpoint
        /// @param value an STL string
        /// @throws DataStoreException if the Checkpoint is used for restoring or
        /// if the given data cannot be added to Checkpoint
        void addSTLString(std::string const & value);

        /// Extract/de-serialize an STL string from the Checkpoint
        /// @param value return an STL string extracted from the Checkpoint
        /// @throws DataStoreException if the Checkpoint is used for checkpointing or
        /// if an STL string cannot be extracted from Checkpoint
        void getSTLString(std::string & value);

        /// Extract/de-serialize an STL string from the Checkpoint
        /// @return a STL string extracted from the Checkpoint
        /// @throws DataStoreException if the Checkpoint is used for checkpointing or
        /// if an STL string cannot be extracted from Checkpoint
        std::string getSTLString();

        /// Add/serialize an SPL string to the Checkpoint
        /// @param value an SPL string
        /// @throws DataStoreException if the Checkpoint is used for restoring or
        /// if the given data cannot be added to Checkpoint
        void addSPLString(RSTRING_BB_TYPE const & value);

        /// Extract/de-serialize an SPL string from the Checkpoint
        /// @param value return an SPL string extracted from the Checkpoint
        /// @throws DataStoreException if the Checkpoint is used for checkpointing or
        /// if an SPL string cannot be extracted from Checkpoint
        void getSPLString(RSTRING_BB_TYPE & value);

        /// Extract/de-serialize an SPL string from the Checkpoint
        /// @return an SPL string extracted from the Checkpoint
        /// @throws DataStoreException if the Checkpoint is used for checkpointing or
        /// if an SPL string cannot be extracted from Checkpoint
        RSTRING_BB_TYPE getSPLString();

        /// Add/serialize a ustring to the Checkpoint
        /// @param value a ustring
        /// @throws DataStoreException if the Checkpoint is used for restoring or
        /// if the given data cannot be added to Checkpoint
        void addUnicodeString(USTRING_BB_TYPE const & value);

        /// Extract/de-serialize a ustring from the Checkpoint
        /// @return a ustring
        /// @throws DataStoreException if the Checkpoint is used for checkpointing or
        /// if a ustring cannot be extracted from Checkpoint
        USTRING_BB_TYPE getUnicodeString();

        /// Checkpoint an opaque blob
        /// @param blob a pointer to an opaque blob
        /// @param blobsize the opaque blob size
        /// @throws a DataStoreException if the Checkpoint is used for restoring or
        /// if a blob cannot be added to the Checkpoint
        void addBlob(const void * blob, const uint32_t blobsize);

        /// Get an opaque blob from the Checkpoint.
        /// User is responsible for de-allocating the returned memory with delete[].
        /// @param size the size of the returned opaque blob
        /// @return a pointer to an opaque blob
        /// @throws a DataStoreException if the Checkpoint is used for checkpointing or
        /// if an blob cannot be extracted from the Checkpoint
        unsigned char * getBlob(uint32_t & size);

        /// Add/serialize a pointer to the Checkpoint
        /// @param value a void* pointer
        /// @return the Checkpoint instance
        /// @throws DataStoreException if the Checkpoint is used for restoring or
        /// if the given data cannot be added to Checkpoint
        void addPointer(void const * ptr);

        /// Extract/de-serialize a pointer from the Checkpoint
        /// @return a pointer from Checkpoint
        /// @throws DataStoreException if the Checkpoint is used for checkpointing or
        /// if a pointer cannot be extracted from Checkpoint
        void * getPointer();

        /// Add/serialize a sequence of characters to the Checkpoint.
        /// NOTE: the size of the sequence is not serialized (for that use addBlob).
        /// @param chbuf a pointer to a sequence of characters
        /// @param chbufsize the size for the sequence of characters
        /// @throws DataStoreException if the Checkpoint is used for restoring or
        /// if the given data cannot be added to Checkpoint
        void addCharSequence(char const * chbuf, uint32_t chbufsize);

        /// Extract/de-serialize a sequence of charaters from the Checkppint.
        /// The sequence begins at the output cursor position and ends at the end
        /// of the Checkpoint.
        /// User is responsible for de-allocating memory for the returned data with delete[].
        /// @param sizeTillEOB this is an out-parameter that will store the size
        /// of the returned char sequence
        /// @return a pointer to sequence of characters
        /// @throws DataStoreException if the Checkpoint is used for checkpointing or
        /// if a sequence of charaters cannot be extracted from Checkpoint
        char * getCharSequence(uint32_t & sizeTillEOB);

        /// Extract/de-serialize a fixed-size sequence of characters from
        /// the Checkpoint -- the sequence begins at the output cursor position.
        /// User is responsible for de-allocating memory for the returned data with delete[].
        /// @param mysize the size of the sequence to extract
        /// @return a pointer to sequence of characters
        /// @throws DataStoreException if the Checkpoint is used for checkpointing or
        /// if a sequence of charaters cannot be extracted from Checkpoint
        char * getFixedCharSequence(uint32_t mysize);

        /// Extract/de-serialize a sequence of charaters from the Checkpoint into a
        /// user-provided buffer. The sequence begins at the output cursor position.
        /// @param buffer user provided buffer (of size bsize).
        /// @param bsize the size of the sequence to read
        /// @param returnSize return the size of the actually retrieved sequence
        /// @throws DataStoreException if the Checkpoint is used for checkpointing or
        /// if a sequence of charaters cannot be extracted from Checkpoint
        void getFixedCharSequence(char * buffer, uint64_t bsize, uint64_t & returnSize);

        /// Obtain the size of the content current in the Checkpoint
        /// @return the size (in Bytes) of the content current in the Checkpoint
        /// @throws DataStoreException if the size cannot be determined is larger than
        /// the maximum limit of uint32_t
        uint32_t getContentSize() const;

        /// Obtain the size of the current content in the Checkpoint
        /// @param size return the size (in Bytes) of the current content in the Checkpoint
        /// @throws a DataStoreException if the size cannot be determined
        void getContentSize(uint64_t & size) const;

        /// Obtain the pointer for the internal buffer (Not supported)
        /// @return a unsigned char* pointer to the internal buffer
        /// @throws DataStoreException
        unsigned char * getPtr()
        {
            THROW_CHAR(DataStore, "getPtr() is not supported");
        }

        /// Obtain the (const) pointer for the internal buffer (Not supported)
        /// @return a unsigned char* pointer to the internal buffer
        /// @throws DataStoreException
        unsigned char const * getPtr() const
        {
            THROW_CHAR(DataStore, "getPtr() is not supported");
        }

        /// Obtain number of serialized bytes remaining from the location
        /// pointed by the output cursor till the end of the buffer (Not supported)
        /// @throws DataStoreException
        uint32_t getNRemainingBytes() const
        {
            THROW_CHAR(DataStore, "getNRemainingBytes() is not supported");
        }

        /// Get the internal buffer output cursor (Not supported)
        /// @return current output cursor buffer cursor
        /// @throws DataStoreException
        uint32_t getOCursor() const
        {
            THROW_CHAR(DataStore, "getOCursor() is not supported");
        }

        /// Extract/de-serialize data of type T from the Checkpoint.
        /// The type T must be de-serializable from Checkpoint.
        /// @return the extracted data
        /// @throws DataStoreException if the Checkpoint is used for checkpointing or
        /// if data cannot be extracted from Checkpoint
        template <class T> T get()
        {
            T value;
            *this >> value;
            return value;
        }

        friend class CheckpointContextImpl;

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Mark the end of Checkpointing operation.
        /// This function is for internal use. Operator code should not call this function.
        /// @throws DataStoreException if the Checkpoint is used for restoring or if
        /// any error happens
        void finishCheckpointing();
#endif

#ifndef DOXYGEN_SKIP_FOR_USERS
    private:
        /// Handle exception caught during checkpointing/restore, including aborting ongoing
        /// operations, recording trace message, and throwing an exception which nests
        /// the exception being caught.
        /// @param e the exception being caught
        /// @param msg message to be associated with the exception
        /// @throws DataStoreException
        void exceptionHandler(DataStoreException const & e, std::string const & msg);

        /// State of checkpointing
        enum CheckpointState {
            INPROGRESS = 0,
            ERROR = 1,
            FINISHED = 2
        };

        DataStoreEntry * entry_;        // Data Store Entry on backend store
        DataStoreByteBuffer * buffer_;  // Data Store Byte Buffer on backend store for checkpoint data
        int64_t id_;                    // sequence ID of this Checkpoint
        bool mode_;                     // true for checkpointing; false for restoring
        enum CheckpointState state_;    // checkpointing state
        std::string dataKey_;           // key used to point to checkpoint data in backend store (deprecated)
        int64_t checkpointCounter_;     // internal counter for how many number of checkpoints have been taken

        /// Constructor (only used for resetting from incremental checkpoint)
        /// @param ckpt current checkpoint being resetted
        /// @param id checkpoint sequence ID
        /// @param offset starting offset
        /// @throws DataStoreException if a Checkpoint instance cannot be created
        Checkpoint(Checkpoint & ckpt, const int64_t & id, const uint64_t & offset);

        /// Constructor
        /// @param storeEntry Data Store Entry to contain this Checkppoint
        /// @param id Sequence ID of this Checkpoint
        /// @pram mode true for checkpointing, false for restoring
        /// @param baseID sequence ID of base checkpoint
        /// @param ckptCount internal checkpoint ID
        /// @param incrCkptInterval incremental checkpointing interval
        /// @param enableLogging whether to enable logging after checkpointing
        /// @param normSize the size of the normal checkpoint data in Bytes.
        /// @param deltaSize the size of the incremental checkpoint data in Bytes.
        /// @param indexSize the size of the incremental checkpoint index data in Bytes.
        /// @param batch Checkpoint Batch handle
        /// @throws DataStoreException if there is any error
        Checkpoint(DataStoreEntry * storeEntry, const int64_t & id, const bool mode,
            const int64_t & baseID, const int64_t & ckptCount, const uint32_t incrCkptInterval, const bool enableLogging,
            const uint64_t & normSize, const int64_t & deltaSize, const int64_t & indexSize, CheckpointBatch * batch);

        /// Initialization
        /// @param normSize the size of the normal checkpoint data in Bytes.
        /// @param deltaSize the size of the incremental checkpoint data in Bytes.
        /// @param indexSize the size of the incremental checkpoint index data in Bytes.
        /// @throws DataStoreException if there is any error
        void initialize(const int64_t & normSize, const int64_t & deltaSize, const int64_t & indexSize);

        /// Get the size of normal checkpoint data
        /// @return the size of normal checkpoint data
        uint64_t getNormDataSize() const;

        /// Get the size of normal checkpoint data
        /// @return the size of incremental checkpoint data
        uint64_t getIncrementalDataSize() const;

        /// Get the size of normal checkpoint data
        /// @return the size of incremental checkpoint index
        uint64_t getIncrementalIndexSize() const;

        /// Set the Checkpoint to read/write data in incremental form
        void setIncrementalMode();

        /// Set the Checkpoint to read/write data in normal form
        void unsetIncrementalMode();

        /// Test if the Checkpoint is in incremental form
        /// @return true if the Checkpoint is in incremental form, false otherwise
        bool isIncrementalMode() const;

        /// Get index buffer
        /// @param id checkpoint sequence ID
        /// @param offset starting offset
        /// @throws DataStoreException if any error happens
        DataStoreByteBuffer * getIndexBuffer(const int64_t & id, const uint64_t & offset);
        DataStoreByteBuffer * getIndexBuffer(const int64_t & id);

        /// Set the output cursor to the given offset
        /// @param offset offset to change to
        void setOCursor(uint64_t offset);

        /// Return whether this checkpoint is in pure base form (only used for checkpointing)
        /// @return true if this checkpoint is in pure base form, return false otherwise
        bool isBase() const;

        /// Get base checkpoint sequence ID
        /// @return base checkpoint sequence ID
        int64_t getBaseID() const;

        /// Get checkpoint count
        /// @return checkpoint count
        int64_t getCheckpointCount() const;

        /// Get checkpoint count
        /// @return checkpoint count
        uint32_t getIncrementalCheckpointInterval() const;

        /// Return whether to enable logging after checkpointing
        /// @return true if logging should be enabled after checkpointing, false otherwise
        bool shouldEnableLogging() const;

        /// Clean up routine upon failure in constructor
        void cleanup();

        DataStoreByteBuffer * normBuffer_;         // Data Store Byte Buffer for normal non-incremental checkpoint data
        DataStoreByteBuffer * deltaBuffer_;        // Data Store Byte Buffer for incremental checkpoint data
        DataStoreByteBuffer * indexBuffer_;        // Data Store Byte Buffer for index to incremental checkpoint data
        int64_t baseID_;                           // base checkpoint ID on which this Checkpoint is dependent
        uint32_t incrementalCkptInterval_;         // incremental checkpointing interval
        bool enableLogging_;                       // whether to enable logging after checkpointing
        bool deAllocBufferCache_;                  // whether this Checkpoint should deallocate buffer caches
        CheckpointBufferCache * indexBufferCache_; // cache of retreived indices
        CheckpointBufferCache * deltaBufferCache_; // cache of retreived incremental data
        CheckpointBatch * batch_;                  // checkpoint batch handle

        friend class UpdateLog;
#endif
    };
} // namespace SPL

#include <SPL/Runtime/Operator/State/CheckpointHelpers.h>
#include <SPL/Runtime/Operator/State/IncrementalCheckpoint.h>

#endif // SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_H
