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

#ifndef SPL_RUNTIME_OPERATOR_CONSISTENT_REGION_CONTEXT_H
#define SPL_RUNTIME_OPERATOR_CONSISTENT_REGION_CONTEXT_H

/*!
 * \file ConsistentRegionContext.h \brief Definition of the SPL::ConsistentRegionContext class.
 */
#include <SPL/Runtime/Utility/Visibility.h>
#include <SPL/Runtime/Operator/OptionalContext.h>
#include <SPL/Runtime/Operator/Port/OperatorInputPort.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>

/// Macro for the @consistent SPL annotation.
#define CONSISTENT "consistent"

#include <stdint.h>
#include <vector>
#include <tr1/memory>

namespace SPL
{
    /*! \brief This is an optional context that can be acquired by using the
     * CONSISTENT_REGION string defined in OptionalContext. An operator has a
     * ConsistentRegionContext when it belongs to a consistent region.
     *
     * One can use the \@consistent annotation to define a consistent region. Consistent
     * regions enable an application to achieve guaranteed tuple processing.
     */

    class ConsistentRegionContext :
        public OptionalContext
    {
    public:
#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Destructor
        virtual ~ConsistentRegionContext() {}
#endif

        /// Checks if the operator is a start of a consistent region.
        /// @return true if the operator is a start of a consistent region, false otherwise
        virtual bool isStartOfRegion() const = 0;

        /// Checks if the operator is an end of a consistent region
        /// @return true if the operator is an end of a consistent region, false otherwise
        virtual bool isEndOfRegion() const = 0;

        /// Checks if the operator is the trigger operator of a consistent region. An operator
        /// is the trigger operator if it is the start operator of an operator-driven region.
        /// @return true if the operator is the trigger operator of a consistent region, false otherwise
        virtual bool isTriggerOperator() const = 0;

        /// Checks if the operator input port is part of the consistent region. An input port is
        /// part of a consistent region if it consumes streams from other operators that are
        ///  also in the consistent region.
        /// @param port input port index
        /// @return true if the port is part of the consistent region, false otherwise
        /// @throws SPLRuntimeInvalidIndexException when the index is out of bounds
        virtual bool isConsistentInputPort(uint32_t port) const = 0;

        /// Gets the input ports that participate in the consistent region
        /// @return vector with pointers to all input ports that participate in the consistent region
        virtual std::vector<OperatorInputPort *> getInputPorts() const = 0;

        /// Get the region index
        /// @return region index
        virtual int32_t getIndex() const = 0;

        /// Get the canonical form of the ObjectName of the @c ConsistentRegionMXBean
        /// associated with the current consistent region.  The name can be used to
        /// identify the source of notifications to subscribe to.
        /// @return canonical form for the @c ConsistentRegionMXBean ObjectName
        virtual std::string const & getConsistentRegionMXBeanName() const = 0;

        /// This method returns the current sequence identifier of a consistent
        /// region. It indicates the id that a drain or a reset should be
        /// associated to. After operator startup, this method returns 1.
        /// On operator restart, the method returns -1 until it has reset.
        /// \n
        /// When establishing a consistent state, the sequence ID is strictly increasing.
        /// When resetting, the sequence ID refers to a sequence ID of a prior successfully
        /// established consistent state.
        /// @return current sequence id of a drain or reset, or -1 if the method is
        /// accessed after the operator restarts due to a PE crash but before the operator
        /// resets
        virtual int64_t getSequenceId() const = 0;

        /// Returns the current number of attempts of resetting a consistent region. If
        /// a drain is completed after a reset, this method returns -1.
        /// @return last reset attempt, or -1 if a drain has been successfully processed
        /// after a reset
        virtual int32_t getResetAttempt() const = 0;

        /// Requests the @c ConsistentRegionMXBean to reset the consistent region
        /// to the point of the last successful drain. If the region is already
        /// being reset, a new reset attempt is triggered.
        /// This method can be used by operators when detecting an operator-specific transient
        /// failure (e.g., handling an exception that prevents the successful establishment of
        /// a consistent state). If the exception happens while holding consistent region
        /// permits, this method must be invoked while holding the permit.
        virtual void resetRegion() = 0;

        /// Acquires the permit required to do operator state modification and tuple or punctuation submission in a
        /// consistent region.
        /// \n
        /// Once the permit is acquired, no consistent state is established or
        /// reset until the permit is released.
        /// If a consistent state is being established or reset, this method blocks until
        /// the process completes successfuly.
        /// \n
        /// Multiple threads are allowed to acquire
        /// permits at the same time. Operator backgrounds threads (e.g., threads running
        /// Operator::process(uint32_t)) that perform tuple or punctuation submission must always acquire permits
        /// prior to submission. Permits are automatically acquired by the SPL runtime when processing
        /// tuples (e.g., Operator::process(Tuple &, uint32_t)), punctuations (Operator::process(Punctuation const &, uint32_t)), and
        /// window library events.
        virtual void acquirePermit() = 0;

        /// Releases a previously acquired permit.
        /// Releasing a permit indicates the successful completion of operator state modification and
        /// tuple or punctuation submission, unless the resetRegion() method has been invoked prior to the release
        /// of the permit.
        virtual void releasePermit() = 0;

        /// Called by start operators of a region to initiate the establishment of a consistent state.
        /// If the region is operator driven, and the operator is the trigger operator of the region,
        /// draining of the region starts immediately. If the region is periodic, the calling thread blocks
        /// until the SPL runtime finishes the next periodic consistent state. If the operator needs to submit
        /// more tuples prior to the start of a consistent state, such tuples can be submitted in the
        /// StateHandler::drain() method.
        /// \n
        /// To successfully execute makeConsistent(), the calling thread must currently hold a permit. If multiple
        /// threads call this method at the same time, only a single consistent state will be established.
        /// \n
        /// The method blocks the calling thread and returns when the region has successfully established
        /// or reset a consistent state or when there is a shutdown request.
        /// \n
        /// Note that calling makeConsistent() in the start operator would block the tuple flow until all operators
        /// in the consistent region have finished checkpointing, including those operator which enable non-blocking
        /// checkpointing. In order to achieve non-blocking checkpointing, please use makeConsistentNonBlocking()
        /// in the start operator.
        /// \n
        /// Since Streams&reg; Version 4.2.0, before makeConsistent() returns true, the SPL Runtime invokes
        /// start operator's StateHandler::regionCheckpointed() callback.
        /// \n
        /// @return true if the region become consistent without any reset, false otherwise.
        /// @throws SPLRuntimeOperatorException if the operator is not a start operator of the region.
        virtual bool makeConsistent() = 0;

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Indicates if the marker should be forwarded downstream
        /// @param punct punctuation
        /// @param port input port
        /// @return true, if the marker must be forwarded downstream, false otherwise
        virtual bool mustForwardMarker(Punctuation const & punct, uint32_t port) = 0;

        /// Informs the runtime that the marker has been forwarded downstream
        /// @param punct punctuation
        /// @param port input port
        virtual void forwardedMarker(Punctuation const & punct, uint32_t port) = 0;

        /// Finishes processing the marker
        /// @param punct punctuation
        /// @param port input port
        virtual void postMarkerProcessing(Punctuation const & punct, uint32_t port) = 0;
#endif

        /// Gets the drain timeout of the consistent region
        /// @return timeout specified drain timeout for the region
        /// @since InfoSphere&reg; Streams Version 4.0.1
        virtual double getDrainTimeout() = 0;

        /// Gets the reset timeout of the consistent region
        /// @return timeout specified reset timeout for the region
        /// @since InfoSphere&reg; Streams Version 4.0.1
        virtual double getResetTimeout() = 0;

        /// Enables non-blocking checkpointing. This method must be invoked
        /// from the operator constructor. Non-blocking checkpointing enables
        /// an operator to checkpoint its state while new tuples are processed.
        /// This should be used only when the operator implements techniques
        /// that guarantee that the operator state saved on the checkpoint()
        /// call is consistent with processing all tuples prior to the drain()
        /// call even after tuple processing is resumed. One such technique
        /// is user-level copy-on-write of operator state.
        /// @since Streams&reg; Version 4.2.0
        virtual void enableNonBlockingCheckpoint() = 0;

        /// Return value of makeConsistentNonBlocking() call
        /// @since Streams&reg; Version 4.2.0
        enum DrainResult {
            COMPLETED, // all operators have finished checkponting
            CHECKPOINT_PENDING, // all operators that do blocking checkpointing have finished checkpointing, and
                                // all operators that do non-blocking checkpointing have finished preparation
                                // for non-blocking checkpointing, but at least one operator still has non-blocking
                                // checkpointing ongoing
            FAILED     // there is failure & reset or shutdown during the draining and checkpointing process
        };

        /// Called by start operators of a region to initiate the establishment of a consistent state.
        /// If the region is operator driven, and the operator is the trigger operator of the region,
        /// draining of the region starts immediately. If the region is periodic, the calling thread blocks
        /// until the SPL runtime starts the next periodic consistent state. If the operator needs to submit
        /// more tuples prior to the start of a consistent state, such tuples can be submitted in the
        /// StateHandler::drain() method.
        /// \n
        /// To successfully execute makeConsistentNonBlocking(), the thread must currently hold a permit. If multiple
        /// threads call this method at the same time, only a single consistent state will be established.
        /// \n
        /// The makeConsistentNonBlocking() function has similar functionality as makeConsistent(), but should be
        /// preferred when the consistent region contains operator(s) with non-blocking checkpoointing enabled.
        /// \n
        /// If a consistent region has no operator which enables non-blocking checkpointing, then the
        /// makeConsistentNonBlocking() returns COMPLETED when a consistent state has successfully established, or
        /// FAILED when there is a shutdown request or there is a failure and the region has been resetted.
        /// So if the consistent region has no operator with non-blocking checkpointing enabled,
        /// makeConsistentNonBlocking() returning COMPLETED is  equivalent to makeConsistent() returning true, and
        /// makeConsistentNonBlocking() returning FAILED is the same as makeConsistent() returning false.
        /// \n
        /// If a consistent region has any operator which enabls non-blocking checkpointing, then the
        /// makeConsistentNonBlocking() may also return CHECKPOINT_PENDING, indicating that all blocking
        /// operators have finished checkpointing and all non-blocking operators have finished preparation
        /// but there is still non-blocking checkpointing ongoing. The start operator can resume tuple
        /// processing upon the return of CHECKPOINT_PENDING. In comparison, if makeConsistent() is used when
        /// the consistent region has any operator with non-blocking checkpointing enabled, the makeConsistent()
        /// call would not return until all operator have finished checkpointing (including those non-blocking
        /// ones).
        /// \n
        /// If makeConsistentNonBlocking() returns COMPLETED (no matter whether there is non-blocking operator in the
        /// region), a consistent state has been successfully established, and it is safe to resume tuple processing
        /// and retire any tuples between this checkpoint and the previous checkpoint. The previous checkpoint has
        /// also been retired by the SPL Runtime via calling StateHandler::retireCheckpoint().
        /// Before letting makeConsistentNonblocking() returning COMPLETED, the SPL Runtime also invokes start
        /// operator's StateHandler::regionCheckpointed() callback.
        /// \n
        /// If makeConsistentNonBlocking() returns CHECKPOINT_PENDING, it is not safe to retire tuples between this
        /// checkpoint and the previous checkpoint, and the SPL Runtime has not yet retired the previous
        /// checkpoint; however, it is ok to resume tuple processing after CHECKPOINT_PENDING is returned.
        /// When all the pending checkpoints are complete, the SPL Runtime invokes start operator's
        /// StateHandler::regionCheckpointed() callback.
        /// \n
        /// makeConsistentNonBlocking() and makeConsistent() cannot be used in mix.
        /// \n
        /// @return COMPLETED if all operators have finished checkponting, return CHECKPOINT_PENDING if all operators
        /// that do blocking checkpointing have finished checkpointing, andall operators that do non-blocking
        /// checkpointing have finished preparation for non-blocking checkpointing, but at least one operator still
        /// has non-blocking checkpointing ongoing, return FAILED if there is failure & reset or shutdown request
        /// during the draining and checkpointing process
        /// @throws SPLRuntimeOperatorException if the operator is not a start operator of the region.
        /// @since Streams&reg; Version 4.2.0
        virtual enum DrainResult makeConsistentNonBlocking() = 0;
    };

    /*! \brief Class that provides an RAII handler for consistent region permits.
     *
     *  Acquires and releases a consistent region permit using a passing ConsistentRegionContext.
     *
     *  \code{.cpp}
     *  {
     *      ConsistentRegionPermit cr(myContext);
     *      AutoMutex am(myLock)
     *      // Code that manipulates state
     *      // Code that submits a tuple
     *  }
     *  \endcode
     */
    class DLL_PUBLIC ConsistentRegionPermit
    {
    public:
        /// Constructor
        /// @param context operator's ConsistentRegionContext
        ConsistentRegionPermit(ConsistentRegionContext * context)
          : context_(context) {
            if (context_ != NULL) {
                context_->acquirePermit();
            }
        }

        /// Destructor
        ~ConsistentRegionPermit() {
            if (context_ != NULL) {
                context_->releasePermit();
            }
        }
    private:
        ConsistentRegionContext *context_;
    };

}

#endif // SPL_RUNTIME_COMMON_CONSISTENT_REGION_CONTEXT_H
