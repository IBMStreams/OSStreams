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
 * \file CheckpointDeletionWorkItem.h \brief Definition of SPL::CheckpointDeletionWorkItem class
 */
#ifndef SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_DELETION_WORK_ITEM_H
#define SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_DELETION_WORK_ITEM_H

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Operator/State/CheckpointContextImpl.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <UTILS/WorkerThread.h>
#include <string>

namespace SPL {
/// \brief The class that represents a work item to be executed by PE's thread pool to
/// delete a Checkpoint asynchronously
class DLL_PUBLIC CheckpointDeletionWorkItem : public UTILS_NAMESPACE::WorkItem
{
  public:
    /// Constructor
    /// @param ckptContext handle to CheckpointContextImpl
    /// @param id Sequence ID of the checkpoint to be deleted
    /// @param opName name of operator
    /// @param callback function to be called after deletion
    /// @param clientData opaque parameter to pass to callback function
    CheckpointDeletionWorkItem(CheckpointContextImpl& ckptContext,
                               const int64_t& id,
                               const std::string& opName,
                               CheckpointContextImpl::DeleteCallback callback,
                               void* clientData);

    /// Destructor
    ~CheckpointDeletionWorkItem();

    /// The function which does the real work
    virtual void satisfy();

#ifndef DOXYGEN_SKIP_FOR_USERS
  private:
    CheckpointContextImpl& ckptContext_;
    int64_t id_;
    std::string opName_;
    CheckpointContextImpl::DeleteCallback callback_;
    void* clientData_;
#endif
};
} // namespace SPL

#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_DELETION_WORK_ITEM_H
