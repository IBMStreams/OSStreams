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
 * Implementation of SPL::CheckpointDeletionWorkItem class
 */

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Operator/State/CheckpointDeletionWorkItem.h>
#include <SPL/Runtime/Operator/State/CheckpointLogTrace.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <boost/lexical_cast.hpp>

using namespace SPL;
using namespace std;

CheckpointDeletionWorkItem::CheckpointDeletionWorkItem(
  CheckpointContextImpl& ckptContext,
  const int64_t& id,
  const std::string& opName,
  CheckpointContextImpl::DeleteCallback callback,
  void* clientData)
  : ckptContext_(ckptContext)
  , id_(id)
  , opName_(opName)
  , callback_(callback)
  , clientData_(clientData)
{}

CheckpointDeletionWorkItem::~CheckpointDeletionWorkItem() {}

void CheckpointDeletionWorkItem::satisfy()
{
    bool isSuccess;
    // delete the given checkpoint
    SPLCKPTTRC(L_DEBUG, opName_, "Deleting checkpoint (" << id_ << ") asynchronously ...");
    try {
        ckptContext_.deleteCheckpoint(id_);
        isSuccess = true;
        SPLCKPTTRC(L_DEBUG, opName_, "Deleted checkpoint (" << id_ << ") successfully.");
    } catch (DataStoreException const& e) {
        SPLCKPTLOGMSG(L_ERROR, opName_,
                      "Cannot delete checkpoint with Sequence ID " +
                        boost::lexical_cast<std::string>(id_),
                      e.getExplanation());
        SPLCKPTTRCMSG(L_ERROR, opName_,
                      "Cannot delete checkpoint with Sequence ID " +
                        boost::lexical_cast<std::string>(id_),
                      e.getExplanation());
        isSuccess = false;
    }
    if (callback_ != NULL) {
        try {
            (*callback_)(id_, isSuccess, clientData_);
        } catch (std::exception const& e) {
            SPLCKPTTRC(L_DEBUG, opName_,
                       "When deleting checkpoint "
                         << id_ << ", caught exception thrown from checkpoint deletion callback: "
                         << e.what());
        } catch (...) {
            SPLCKPTTRC(L_DEBUG, opName_,
                       "When deleting checkpoint "
                         << id_
                         << ", caught unknown exception thrown from checkpoint deletion callback");
        }
    }
}
