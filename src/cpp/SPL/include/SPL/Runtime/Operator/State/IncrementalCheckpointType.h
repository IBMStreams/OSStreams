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
 * \file IncrementalCheckpointType.h \brief Definition of the SPL::IncrementalCheckpointType class template.
 */

#ifndef SPL_RUNTIME_OPERATOR_STATE_INCREMENTAL_CHECKPOINT_TYPE_H
#define SPL_RUNTIME_OPERATOR_STATE_INCREMENTAL_CHECKPOINT_TYPE_H

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif
#include <SPL/Runtime/Operator/State/Checkpoint.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/Operator/State/UpdateLog.h>
#include <exception>
#include <stdint.h>

#ifndef DOXYGEN_SKIP_FOR_USERS

PUSH_DLL_PUBLIC
namespace SPL
{
    /// \brief Class that represents a data type which is capable of incremental checkpointing
    template<typename LogType = UpdateLog>
    class IncrementalCheckpointType
    {
    public:
        /// Constructor
        /// @param opaque paramter to initialize UpdateLog
        IncrementalCheckpointType(void * param) : log_(param), param_(param) { }

        /// Destructor
        virtual ~IncrementalCheckpointType() { }

        /// Serialize the data to Checkpoint
        /// @param ckpt a Checkpoint handle
        /// @throws DataStoreException if there is any error during serialization
        void serialize(Checkpoint & ckpt);

        /// Deserialize the data from Checkpoint
        /// @param ckpt a Checkpoint handle
        /// @throws DataStoreException if there is any error during deserialization
        void deserialize(Checkpoint & ckpt);

    protected:
        /// Simple RAII utility for Checkpoint
        class CheckpointPtr
        {
        public:
            /// Constructor
            /// @param ckpt Checkpoint handle
            CheckpointPtr(Checkpoint * ckpt) : ckptPtr_(ckpt) { }

            /// Destructor
            ~CheckpointPtr()
            {
                if (ckptPtr_ != NULL) {
                    delete ckptPtr_;
                }
            }

            /// overload -> operator
            Checkpoint & operator*() const
            {
                return *ckptPtr_;
            }

        private:
            Checkpoint * ckptPtr_;
        };

        /// Serialize the data in base format.
        /// This function should be implemented by derived classes.
        /// @param ckpt a Checkpoint handle
        virtual void serializeBase(Checkpoint & ckpt) = 0;

        /// deserialize the data in base format.
        /// This function should be implemented by derived classes.
        /// @param ckpt a Checkpoint handle
        virtual void deserializeBase(Checkpoint & ckpt) = 0;

        LogType log_;   // internal UpdateLog handle
        void * param_;  // opaque parameter for initializing UpdateLog
    };

    template<typename LogType>
    void IncrementalCheckpointType<LogType>::serialize(Checkpoint & ckpt)
    {
        try {
            log_.startCheckpoint(ckpt);
            if (log_.isBase() == false) {
                log_.serialize(ckpt, this);
            }
            else {
                serializeBase(ckpt);
                log_.onBaseCheckpoint(this);
            }
            log_.clear();
            log_.finishCheckpoint(ckpt);
        }
        catch (DataStoreException const & e) {
            THROW_NESTED(DataStore, "serialize() failed", e);
        }
        catch (std::exception const & e) {
            THROW(DataStore, "serialize() failed: received exception: " << e.what());
        }
        catch (...) {
            THROW(DataStore, "serialize() failed: received unknown exception");
        }
    }

    template<typename LogType>
    void IncrementalCheckpointType<LogType>::deserialize(Checkpoint & ckpt)
    {
        try {
            log_.startReset(ckpt);
            if (log_.isBase() == false) {
                // frist reset from base checkpoint
                {
                    CheckpointPtr baseCkpt(log_.getBaseCheckpoint());
                    deserializeBase(*baseCkpt);
                }
                // then reset from previous delta checkpoints in order
                while (log_.hasPreviousDeltaCheckpoint()) {
                    CheckpointPtr prevDeltaCkpt(log_.nextPreviousDeltaCheckpoint());
                    LogType prevLog(param_);
                    prevLog.deserializeAndApply(*prevDeltaCkpt, this);
                }
                // finally reset from current delta checkpoint
                log_.deserializeAndApply(ckpt, this);
                log_.clear();
            }
            else {
                deserializeBase(ckpt);
            }
            log_.finishReset(ckpt);
        }
        catch (DataStoreException const & e) {
            THROW_NESTED(DataStore, "deserialize() failed", e);
        }
        catch (std::exception const & e) {
            THROW(DataStore, "deserialize() failed: received exception: " << e.what());
        }
        catch (...) {
            THROW(DataStore, "deserialize() failed: received unknown exception");
        }
    }

    /// Overloaded operator << () function for serializing IncrementalCheckpointType
    /// @param ckpt a Checkpoint handle
    /// @param data data of IncrementalCheckpointType type to serialize
    /// @throws DataStoreException if there is any error during serialization
    template<typename LogType>
    Checkpoint & operator << (Checkpoint & ckpt, const IncrementalCheckpointType<LogType> & data)
    {
        const_cast<IncrementalCheckpointType<LogType> &>(data).serialize(ckpt);
        return ckpt;
    }

    /// Overloaded operator >> () function for deserializing IncrementalCheckpointType
    /// @param ckpt a Checkpoint handle
    /// @param data return deserialized data
    /// @throws DataStoreException if there is any error during deserialization
    template<typename LogType>
    Checkpoint & operator >> (Checkpoint & ckpt, IncrementalCheckpointType<LogType> & data)
    {
        data.deserialize(ckpt);
        return ckpt;
    }

    /// Overloaded operator << () function for serializing IncrementalCheckpointType
    /// @param ckpt an SPL::ByteBuffer<Checkpoint> handle
    /// @param data data of IncrementalCheckpointType type to serialize
    /// @throws DataStoreException if there is any error during serialization
    template<typename LogType>
    SPL::ByteBuffer<Checkpoint> & operator << (SPL::ByteBuffer<Checkpoint> & ckpt, const IncrementalCheckpointType<LogType> & data)
    {
        const_cast<IncrementalCheckpointType<LogType> &>(data).serialize(static_cast<Checkpoint&>(ckpt));
        return ckpt;
    }

    /// Overloaded operator >> () function for deserializing IncrementalCheckpointType
    /// @param ckpt an SPL::ByteBuffer<Checkpoint> handle
    /// @param data return deserialized data
    /// @throws DataStoreException if there is any error during deserialization
    template<typename LogType>
    SPL::ByteBuffer<Checkpoint> & operator >> (SPL::ByteBuffer<Checkpoint> & ckpt, IncrementalCheckpointType<LogType> & data)
    {
        data.deserialize(static_cast<Checkpoint&>(ckpt));
        return ckpt;
    }

} // namespace SPL
POP_DLL_PUBLIC

#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_RUNTIME_OPERATOR_STATE_INCREMENTAL_CHECKPOINT_TYPE_H
