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

#ifndef SPL_RUNTIME_OPERATOR_STATE_HANDLER_H
#define SPL_RUNTIME_OPERATOR_STATE_HANDLER_H

#include <stdint.h>
#include <SPL/Runtime/Operator/State/Checkpoint.h>

namespace SPL
{
    /*! \brief Class that serves as a handler for operator state. The handler is
     *  used when the operator is checkpointing or is in a consistent region.
     *
     *  If an operator is an instance of StateHandler then it is automatically
     *  registered as a state handler, otherwise an operator registers an instance
     *  of StateHandler using OperatorContext::registerStateHandler(StateHandler &).
     *  Operators that need to manage state are encouraged to always utilize a StateHandler
     *  by either extending StateHandler or registering a state handler.
     *
     *  StateHandler implementations should be careful to honor the contract of this
     *  interface to allow use in future scenarios. For example, no assumptions
     *  should be made about the operator being in a consistent region or checkpointing,
     *  and tuple submission should only occur from drain().
     *
     *  When periodic checkpointing is enabled, the SPL Runtime will call checkpoint(Checkpoint&)
     *  for the registered state handler at the configured period. With operator driven
     *  checkpointing, the operator call CheckpointContext::createCheckpoint() to create
     *  a checkpoint, that will result in a callback to checkpoint(Checkpoint&) for the
     *  registered handler. These callbacks occur on the thread that originated the create checkpoint
     *  request.
     *  After PE restart, the SPL Runtime will reset the operator to a persisted state by calling
     *  reset(Checkpoint&) or resetToInitialState().
     *
     *  In a consistent region, the methods drain() and checkpoint(Checkpoint&) are periodically
     *  invoked, according
     *  to the consistent region configuration (periodic or operator-driven). On a drain(),
     *  the operator must finish any outstanding processing related to tuples processed prior
     *  to the drain() call. On a checkpoint(Checkpoint&), the operator can persist its state to the
     *  checkpointing backed. The persisted state is guaranteed to be consistent with the state
     *  of all other operators in the consistent region. After a failure of any operator in the region,
     *  the SPL Runtime will reset the operator to the last successfully persisted consistent state
     *  by calling reset(Checkpoint&) or resetToInitialState().
     *
     *  If the operator code enables non-blocking checkpoint (via invoking
     *  ConsistentRegionContext::enableNonBlockingCheckpoint()), the prepareForNonBlockingCheckpoint(int64_t)
     *  method is invoked by the SPL Runtime after the drain() method. The tuple flow is resumed after
     *  the return of prepareForNonBlockingCheckpoint(int64_t) method. Meanwhile the SPL Runtime delegates an
     *  internal thread to execute the operator’s checkpoint(Checkpoint&) method in the background. Background
     *  checkpointing is internally tracked by the SPL Runtime.
     *
     *  For a start operator of a consistent region, the regionCheckpointed(int64_t) method is called once all
     *  the operators in the consistent region is drained an checkpointed, including those operators
     *  performing non-blocking checkpointing.
     */
    class StateHandler
    {
    public:
#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Destructor
        virtual ~StateHandler() {}
#endif

        /// Drain any outstanding processing. Once this method
        /// returns, the operator is indicating that it has drained
        /// all outstanding processing for all tuples seen on input
        /// ports.
        /// The method may: (i) submit tuples to output ports,
        /// (ii) interact with external systems, (iii) wait for
        /// background activity to drain outstanding work.
        /// For a consistent region this method is called
        /// once all this operator's data (non-control)
        /// input ports in the region have been drained.
        /// This method can be called concurrently with tuple
        /// and punctuation processing from autonomous ports and
        /// tuples from consistent control ports.
        virtual void drain() {}

        /// Checkpoint the operator state. Serializing the operator state
        /// to the Checkpoint reference ensures that the operator state is
        /// persisted to the Streams instance checkpoint backend.
        /// \n
        /// If the operator
        /// is in a consistent region, the state is consistent with the checkpointed
        /// state of other operators in the same consistent region.
        /// If the operator is in a consistent region, this method is called
        /// after all consistent input ports (control and non-control)
        /// have been fully processed. It is guaranteed that no tuples or
        /// punctuations are processed by consistent or autonomous ports when
        /// this method is invoked.
        /// \n
        /// No new tuples should be submitted during a checkpoint.
        /// @param ckpt reference to the checkpoint being created
        virtual void checkpoint(Checkpoint & ckpt) {}

        /// Reset the operator state to a previously persisted operator state.
        /// Deserializing the operator from the checkpoint reference ensures
        /// that the operator state is restored from the Streams instance
        /// checkpoint backend.
        /// \n
        /// If the operator is in a consistent region,
        /// the restored state is consistent with the restored state of
        /// other operators in the same consistent region. It is guaranteed that
        ///  no tuples or punctuations are processed by consistent or autonomous
        /// ports when this method is invoked.
        /// \n
        /// No new tuples should be submitted during a reset.
        /// @param ckpt reference to the checkpoint being reset
        virtual void reset(Checkpoint & ckpt) {}

        /// Retires a checkpoint with an given sequence identifier. When a checkpoint
        /// is retired, it means that the checkpoint with the provided sequence
        /// identifier will no longer be used on a reset in the lifetime of the job.
        /// There is no guarantee that this method is called for every retired checkpoint.
        /// \n
        /// If the operator is in a consistent region and a checkpoint with
        /// a given id is retired, any checkpoint with an identifier equal or lower
        /// than the provided id is also retired.
        /// This method can be called concurrently with tuple
        /// and punctuation processing from autonomous and consistent ports.
	/// @param id checkpoint sequence id
        virtual void retireCheckpoint(int64_t id) {}

        /// Requests the operator to set its state to its state after
        /// Operator::allPortsReady() is invoked.
        /// \n
        /// If the operator is in a consistent region, it is guaranteed that
        /// no tuples or punctuations are processed
        /// by consistent or autonomous ports when this method is invoked.
        /// \n
        /// No new tuples should be submitted during a reset.
        virtual void resetToInitialState() {}

        /// Prepare the operator for non-blocking checkpointing.
        /// This method is executed only when the operator enables non-blocking
        /// checkpointing with the ConsistentRegionContext::enableNonBlockingCheckpoint()
        /// method. This method is guaranteed to execute after StateHandler::drain() and
        /// before StateHandler::checkpoint(). It is also guaranteed to execute before
        /// the consistent region resumed tuple processing.
	/// @param id checkpoint sequence id
        /// @since Streams&reg; Version 4.2.0
        virtual void prepareForNonBlockingCheckpoint(int64_t id) {}

        /// Notify that the whole consistent region has been drained and checkpointed.
        /// When all operators in a consistent region have completed checkpointing (including
        /// non-blocking checkpointing), the regionCheckpointed() method is called to notify
        /// each start operator of the region.
        /// \n
        /// This method is called only for start operators of a consistent region.
        /// \n
	/// @param id checkpoint sequence id
        /// @since Streams&reg; Version 4.2.0
        virtual void regionCheckpointed(int64_t id) {}
    };
}

#endif
