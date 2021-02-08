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

#ifndef SPL_RUNTIME_PROCESSING_ELEMENT_PE_CHECKPOINT_SERVICE_H
#define SPL_RUNTIME_PROCESSING_ELEMENT_PE_CHECKPOINT_SERVICE_H

#include <SAM/SAMNamespaceDefs.h>
#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>
#include <SPL/Runtime/Utility/Thread.h>
#include <UTILS/Mutex.h>

#include <queue>
#include <string>
#include <unistd.h>
#include <vector>

namespace SPL {
class OperatorImpl;
class PEImpl;

/// Class that represents the PE's checkpointing service
class PECheckpointService : public Thread
{
  public:
    /// Constructor
    /// @param pe processing elemenet that hosts this service
    PECheckpointService(PEImpl& pe);

    /// Destructor
    ~PECheckpointService() {}

    /// Restore PE state
    void restorePEState();

    /// Checkpoint an operator synchronously
    /// @param oper operator to be checkpointed
    /// @param opstate operator state
    void synchronousCheckpoint(OperatorImpl& oper, NetworkByteBuffer const& opstate);

    void start();

  private:
    void initCheckpointingFileNames();
    void createCheckpointingDirectory();
    std::string const& getCkptMainFileName(OperatorImpl& oper);
    std::string const& getCkptTempFileName(OperatorImpl& oper);

    void* run(void*);
    void initTimeouts();
    void timedCheckpoint();
    void saveState(OperatorImpl& oper, NetworkByteBuffer const& opstate);

    struct OperatorTimeoutPairComparer
    {
        bool operator()(const std::pair<OperatorImpl*, double> lhs,
                        const std::pair<OperatorImpl*, double> rhs) const
        {
            return lhs.second > rhs.second;
        }
    };

    PEImpl& pe_;
    struct CkptFiles
    {
        std::string mainFile;
        std::string tempFile;
    };
    std::string ckptDirName_;
    std::tr1::unordered_map<uint32_t, CkptFiles> ckptFileNames_;
    std::priority_queue<std::pair<OperatorImpl*, double>,
                        std::vector<std::pair<OperatorImpl*, double> >,
                        OperatorTimeoutPairComparer>
      timeoutQueue_;
    std::deque<OperatorImpl*> userReqQueue_;
    Distillery::Mutex ckptMutex_;
    bool ckptDirCreated_;
};
};

#endif /* SPL_RUNTIME_PROCESSING_ELEMENT_PE_CHECKPOINT_SERVICE_H */
