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

#include <SPL/Runtime/ProcessingElement/PECheckpointService.h>

#include <SAM/augmentedApplicationModel.h>
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Operator/Operator.h>
#include <SPL/Runtime/Operator/OperatorContextImpl.h>
#include <SPL/Runtime/Operator/OperatorImpl.h>
#include <SPL/Runtime/Operator/OperatorState.h>
#include <SPL/Runtime/Operator/OperatorTracker.h>
#include <SPL/Runtime/Operator/State/CheckpointContextImpl.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/operations.hpp>

using namespace SPL;
using namespace Distillery;
using namespace Streams;
using namespace std;
using namespace std::tr1;
namespace bf = boost::filesystem;

PECheckpointService::PECheckpointService(PEImpl& pe)
  : pe_(pe)
{
    stringstream dirName;
    dirName.imbue(locale::classic());
    dirName << pe_.getCheckpointDirectory() << "/pe" << pe_.getPEModel().id() << ".ckpt";
    ckptDirName_ = dirName.str();
    ckptDirCreated_ = false;
}

void PECheckpointService::saveState(OperatorImpl& oper, NetworkByteBuffer const& opstate)
{
    APPTRC(L_DEBUG, "Started dumping operator state.", SPL_PE_DBG);

    OperatorState ckpt(false); // do not take ownership
    ckpt.addOperatorState(const_cast<NetworkByteBuffer&>(opstate).getPtr(),
                          opstate.getContentSize());

    AutoMutex am(ckptMutex_);
    APPTRC(L_DEBUG, "Starting to dump operator state to disk...", SPL_PE_DBG);

    if (ckptDirCreated_ == false) {
        createCheckpointingDirectory();
    }

    string const& filenm = getCkptTempFileName(oper);
    ofstream ckptFile(filenm.c_str(), ios_base::binary);
    try {
        ckpt.saveState(ckptFile);
    } catch (DistilleryException const& e) {
        APPTRC(L_ERROR,
               "Error saving operator state to file '" << filenm << "', "
                                                       << "reason: '" << e.getExplanation()
                                                       << "'. Skipping...",
               SPL_PE_DBG);
        return;
    }
    ckptFile.flush();
    ckptFile.close();

    // do an fsync on the file
    FILE* cfRaw = fopen(filenm.c_str(), "a");
    fsync(fileno(cfRaw));
    fclose(cfRaw);

    if (0 != std::rename(getCkptTempFileName(oper).c_str(), getCkptMainFileName(oper).c_str())) {
        APPTRC(L_ERROR,
               "Error renaming operator state file, reason: '" << RuntimeUtility::getErrorNoStr()
                                                               << "'. Skipping...",
               SPL_PE_DBG);
    }
    APPTRC(L_DEBUG, "Completed dumping operator state.", SPL_PE_DBG);
}

void PECheckpointService::synchronousCheckpoint(OperatorImpl& oper,
                                                NetworkByteBuffer const& opstate)
{
    string const& opname = oper.getContextImpl().getName();
    APPTRC(L_DEBUG, "Operator " << opname << " started checkpointing synchronously.", SPL_PE_DBG);
    saveState(oper, opstate);
    APPTRC(L_DEBUG, "Operator " << opname << " completed checkpointing synchronously.", SPL_PE_DBG);
}

void PECheckpointService::initTimeouts()
{
    vector<OperatorImpl*> const& operators = pe_.getOperators();
    vector<OperatorImpl*>::const_iterator it;
    double interval = 0, currtime = getWallClockTime();
    for (it = operators.begin(); it != operators.end(); ++it) {
        OperatorImpl& oper = **it;
        if (oper.getContextImpl().getCheckpointingInterval() > 0) {
            interval = oper.getContextImpl().getCheckpointingInterval();
            pair<OperatorImpl*, double> timeout(&oper, currtime + interval);
            timeoutQueue_.push(timeout);
        }
    }
}

static void logSaveException(PEImpl& pe, const string& opName)
{
    SPLLOGMSG(L_ERROR, SPL_RUNTIME_EXCEPTION_IN_SPL_OPER_GET_CKPT_LOG(pe.getPEId(), opName),
              SPL_PE_DBG);
}

void PECheckpointService::timedCheckpoint()
{
    initTimeouts();
    while (!pe_.getShutdownRequested()) {
        if (timeoutQueue_.empty()) {
            APPTRC(L_DEBUG, "Timeout queue is empty. Stopping checkpointing thread...", SPL_PE_DBG);
            return;
        }

        OperatorImpl* expiringOp = timeoutQueue_.top().first;
        double expiringTimeout = timeoutQueue_.top().second;
        timeoutQueue_.pop();

        double timeout = expiringTimeout - getWallClockTime();
        if (timeout > 0.0) {
            APPTRC(L_DEBUG, "Waking up in " << timeout << " seconds.", SPL_PE_DBG);
            pe_.blockUntilShutdownRequest(timeout);
            if (pe_.getShutdownRequested()) {
                break;
            }
            APPTRC(L_DEBUG, "Slept for " << timeout << " seconds.", SPL_PE_DBG);
        }

        double now = getWallClockTime();
        expiringTimeout += expiringOp->getContextImpl().getCheckpointingInterval();
        while (expiringTimeout < now) {
            APPTRC(L_ERROR,
                   "Skipping checkpoint for op: '" << expiringOp->getContextImpl().getName()
                                                   << "', running late.",
                   SPL_PE_DBG);
            expiringTimeout += expiringOp->getContextImpl().getCheckpointingInterval();
        }

        string const& opname = expiringOp->getContextImpl().getName();
        OperatorTracker::setCurrentOperator(expiringOp->getIndex());
        try {
            if (expiringOp->getContextImpl().getStateHandler() != NULL) {
                APPTRC(L_DEBUG, "Acquiring state of operator '" << opname << "'...", SPL_PE_DBG);
                CheckpointContextImpl* ckptContext =
                  expiringOp->getContextImpl().getCheckpointContextImpl();
                ckptContext->createCheckpoint();
            } else {
                NetworkByteBuffer opstate;
                expiringOp->getOperator().getCheckpoint(opstate);
                saveState(*expiringOp, opstate);
            }
            expiringTimeout =
              getWallClockTime() + expiringOp->getContextImpl().getCheckpointingInterval();
            timeoutQueue_.push(make_pair(expiringOp, expiringTimeout));
        } catch (DistilleryException const& e) {
            pe_.setFailedOperator(opname);
            logSaveException(pe_, opname);
            APPTRC(L_ERROR, SPL_RUNTIME_EXCEPTION_IN_SPL_OPER_GET_CKPT(opname, e.getExplanation()),
                   SPL_OPER_DBG);
            pe_.shutdown();
        } catch (std::exception const& e) {
            pe_.setFailedOperator(opname);
            logSaveException(pe_, opname);
            APPTRC(L_ERROR, SPL_RUNTIME_EXCEPTION_IN_SPL_OPER_GET_CKPT(opname, e.what()),
                   SPL_OPER_DBG);
            pe_.shutdown();
        } catch (...) {
            pe_.setFailedOperator(opname);
            logSaveException(pe_, opname);
            APPTRC(
              L_ERROR,
              SPL_RUNTIME_EXCEPTION_IN_SPL_OPER_GET_CKPT(opname, SPL_RUNTIME_UNKNOWN_EXCEPTION),
              SPL_OPER_DBG);
            pe_.shutdown();
        }
        OperatorTracker::resetCurrentOperator();
    }
}

static void logRestoreException(PEImpl& pe, const string& opName)
{
    SPLLOGMSG(L_ERROR, SPL_RUNTIME_EXCEPTION_IN_SPL_OPER_RESTORE_CKPT(pe.getPEId(), opName),
              SPL_PE_DBG);
}

void PECheckpointService::restorePEState()
{
    APPTRC(L_DEBUG, "Restoring state from checkpoint...", SPL_PE_DBG);
    initCheckpointingFileNames();
    vector<OperatorImpl*> const& operators = pe_.getOperators();
    vector<OperatorImpl*>::const_iterator it;
    for (it = operators.begin(); it != operators.end(); ++it) {
        OperatorImpl& oper = **it;
        string const& opname = oper.getContextImpl().getName();
        if (!oper.getContextImpl().isCheckpointingOn()) {
            continue;
        }

        bool restored = false;
        // check if we are using the new checkpoint scheme
        if (oper.getContextImpl().getStateHandler() != NULL) {
            try {
                CheckpointContextImpl* ckptContext =
                  oper.getContextImpl().getCheckpointContextImpl();
                ckptContext->restoreCheckpoint();
                restored = true;
            } catch (DataStoreException const& e) {
                pe_.setFailedOperator(opname);
                logRestoreException(pe_, opname);
                APPTRC(L_ERROR,
                       SPL_RUNTIME_EXCEPTION_IN_SPL_OPER_RESTORE_CKPT(opname, e.getExplanation()),
                       SPL_OPER_DBG);
                pe_.shutdown();
            }
        } else {
            OperatorState ckpt;
            string const& filenm = getCkptMainFileName(oper);
            ifstream ckptFile(filenm.c_str(), ios_base::binary);
            APPTRC(L_DEBUG, "Started restoration of state for operator '" << opname << "'.",
                   SPL_PE_DBG);
            try {
                ckpt.restoreState(ckptFile);
            } catch (DistilleryException const& e) {
                APPTRC(L_DEBUG,
                       "Error recovering state from file '" << filenm << "', "
                                                            << "reason: '" << e.getExplanation()
                                                            << "'. Skipping restoration...",
                       SPL_PE_DBG);
                throw;
            }
            ckptFile.close();

            NetworkByteBuffer opstate(
              static_cast<unsigned char*>(const_cast<void*>(ckpt.getOperatorState(0))),
              ckpt.getOperatorStateSize(0));
            OperatorTracker::setCurrentOperator(oper.getIndex());

            try {
                oper.getOperator().restoreCheckpoint(opstate);
                restored = true;
            } catch (DistilleryException const& e) {
                pe_.setFailedOperator(opname);
                logRestoreException(pe_, opname);
                APPTRC(L_ERROR,
                       SPL_RUNTIME_EXCEPTION_IN_SPL_OPER_RESTORE_CKPT(opname, e.getExplanation()),
                       SPL_OPER_DBG);
                pe_.shutdown();
            } catch (std::exception const& e) {
                pe_.setFailedOperator(opname);
                logRestoreException(pe_, opname);
                APPTRC(L_ERROR, SPL_RUNTIME_EXCEPTION_IN_SPL_OPER_RESTORE_CKPT(opname, e.what()),
                       SPL_OPER_DBG);
                pe_.shutdown();
            } catch (...) {
                pe_.setFailedOperator(opname);
                logRestoreException(pe_, opname);
                APPTRC(L_ERROR,
                       SPL_RUNTIME_EXCEPTION_IN_SPL_OPER_RESTORE_CKPT(
                         opname, SPL_RUNTIME_UNKNOWN_EXCEPTION),
                       SPL_OPER_DBG);
                pe_.shutdown();
            }
            OperatorTracker::resetCurrentOperator();
        }
        if (restored) {
            APPTRC(L_DEBUG, "Completed restoration of operator '" << opname << "'.", SPL_PE_DBG);
        } else {
            APPTRC(L_DEBUG, "Cannot complete restoration of operator '" << opname << "'.",
                   SPL_PE_DBG);
        }
    }
}

void PECheckpointService::start()
{
    initCheckpointingFileNames();
    this->create();
}

void* PECheckpointService::run(void*)
{
    APPTRC(L_DEBUG, "Checkpoint thread starting...", SPL_PE_DBG);
    registerThread("Checkpoint");
    OperatorTracker::init();
    timedCheckpoint();
    OperatorTracker::finalize();
    APPTRC(L_DEBUG, "Checkpointing thread exiting...", SPL_PE_DBG);
    return NULL;
}

void PECheckpointService::createCheckpointingDirectory()
{
    APPTRC(L_DEBUG, "Checkpoint directory is '" << ckptDirName_ << "'", SPL_PE_DBG);
    bf::path ckptDir = ckptDirName_;
    try {
        bf::create_directory(ckptDir.branch_path());
        bf::create_directory(ckptDirName_);
        ckptDirCreated_ = true;
    } catch (bf::filesystem_error const& e) {
        APPTRC(L_ERROR,
               "Creation of checkpoint directory '" << ckptDirName_ << "' failed, "
                                                    << "reason: '" << e.what() << "'.",
               SPL_PE_DBG);
        THROW(SPLRuntime, e.what());
    }
}

void PECheckpointService::initCheckpointingFileNames()
{
    vector<OperatorImpl*> const& opers = pe_.getOperators();
    vector<OperatorImpl*>::const_iterator it = opers.begin();
    for (it = opers.begin(); it != opers.end(); ++it) {
        OperatorImpl& oper = **it;
        stringstream temp, ckpt;
        ckpt.imbue(locale::classic());
        temp << ckptDirName_ << "/tmp" << oper.getIndex() << ".ckpt";
        ckpt << ckptDirName_ << "/op" << oper.getIndex() << ".ckpt";
        CkptFiles files;
        files.tempFile = temp.str();
        files.mainFile = ckpt.str();
        ckptFileNames_[oper.getIndex()] = files;
    }
}

string const& PECheckpointService::getCkptMainFileName(OperatorImpl& oper)
{
    return ckptFileNames_[oper.getIndex()].mainFile;
}

string const& PECheckpointService::getCkptTempFileName(OperatorImpl& oper)
{
    return ckptFileNames_[oper.getIndex()].tempFile;
}
