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
 * \file IncrementalCkptInfo.h \brief Definition of SPL::IncrementalCkptIntervalSetter class
 */
#ifndef SPL_RUNTIME_OPERATOR_STATE_INCREMENTAL_CKPT_INTERVAL_SETTER_H
#define SPL_RUNTIME_OPERATOR_STATE_INCREMENTAL_CKPT_INTERVAL_SETTER_H

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Operator/State/IncrementalCkptInfo.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <boost/config.hpp>
#include <stdint.h>

namespace SPL {
/// \brief The class that represents the algoritmn to determine incremental checkpointing interval,
/// i.e., the number of delta checkpoints between two base checkpoints. For example, when the
/// incremental checkpointing interval is 2, there are two delta checkpoints between two base
/// checkpoints. When the incremental checkpointing interval is 0, incremental checkpointing is
/// disabled.
class DLL_PUBLIC IncrementalCkptIntervalSetter
{
  public:
    /// Default constructor
    IncrementalCkptIntervalSetter();

    /// Destructor
    virtual ~IncrementalCkptIntervalSetter();

    /// Initialize and enable the algorithm
    /// @param param opaque parameter to initialize the algorithm; the default implementation
    /// interpret this parameter as a pointer to a double value which specifies the limit on ratio
    /// of aggregated consecutive delta checkpoints size and base checkpoint size; a value of 0.0
    /// would disable incremental checkpointing; any negative value is not allowed and cause
    /// a DataStoreExcpetion to be thrown; a NULL pointer cause DEFAULT_SIZE_RATIO to be used.
    /// @throws DataStoreExcpetion if there is any error in initialization or if the parameter is
    /// illegal
    virtual void initialize(void* param);

    /// Pass information about the checkpointing operation just fnished to the algorithm
    /// @param ckptInfo information about a checkpointing operation
    void addCheckpointInfo(const IncrementalCkptInfo& ckptInfo);

    /// Callback function to be called by addCheckpointInfo() for a base checkpoint
    /// @param ckptInfo information about a checkpointing operation
    virtual void onAddingBaseCheckpointInfo(const IncrementalCkptInfo& ckptInfo);

    /// Callback function to be called by addCheckpointInfo() for a delta checkpoint
    /// @param ckptInfo information about a checkpointing operation
    virtual void onAddingDeltaCheckpointInfo(const IncrementalCkptInfo& ckptInfo);

    /// Determine the incremental checkpointing interval
    /// @return the incremental checkpointing interval
    uint32_t determineInterval();

    /// Callback funciton to be called by determineInterval()
    /// @return the incremental checkpointing interval
    virtual uint32_t onDetermineInterval();

    /// Reset the algorithm right after resetting the operator
    /// @param interval incremental checkpointing interval to be used
    /// @return the incremental checkpointing interval
    uint32_t reset(const uint32_t interval);

    /// Return whether the checkpoint to make should be a base checkpoint or delta checkpoint
    /// @return true if the checkpoint to make should be a base checkpoint, false otherwise
    bool isBaseCheckpoint() const;

    /// Return whether to enable logging
    /// @return true to enable logging, false to disable logging
    bool shouldEnableLogging() const;

    /// Constants
    const static uint32_t INITIAL_INTERVAL = 2; // initial incremental checkpointing interval
    const static uint32_t PROBE_LIMIT = 10;     // re-enable incremental ckpt after this many
                                                // consecutive base checkpoints
    BOOST_CONSTEXPR_OR_CONST
    static double DEFAULT_SIZE_RATIO = 1.0; // default ratio of aggregated size of delta checkpoints
                                            // to base checkpoint size
    BOOST_CONSTEXPR_OR_CONST
    static double LOGGING_PENALTY = 1.1; // adjustment factor to account for logging cost

#ifndef DOXYGEN_SKIP_FOR_USERS
  private:
    uint32_t interval_;   // current incremental checkpointing interval
    uint32_t numCkpts_;   // number of checkpoints made since last base checkpoint (including last
                          // base checkpoint)
    uint32_t probeLimit_; // re-enable incremental ckpt after this many consecutive base checkpoints
    uint32_t probeCount_; // number of consecutive base checkpoints
    double sizeRatio_;    // limit on ratio of aggregated size of delta checkpoints to base
                          // checkpoint size
    double baseCkptSize_; // size of base checkpoint
    double sumDeltaCkptSize_;         // aggregated size of delta checkpoints
    double weightedSumDeltaCkptSize_; // weighted aggregated size of delta checkpoints
#endif
};
} // namespace SPL

#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_RUNTIME_OPERATOR_STATE_INCREMENTAL_CKPT_INTERVAL_SETTER_H
