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

#include <SPL/Runtime/Operator/State/ConsistentRegionContextImpl.h>

#include <SPL/Runtime/Operator/Port/OperatorInputPortImpl.h>
#include <SPL/Runtime/ProcessingElement/PEConsistentRegionService.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <iostream>

using namespace SPL;

// This is the internal controller consistent region mbean.
#define CONTROL_DOMAIN "com.ibm.streams.control.reserved:"
#define CONSISTENT_REGION_NAME_TYPE ",type=consistentController"

const std::string ConsistentRegionContextImpl::drainMethod_("drain");
const std::string ConsistentRegionContextImpl::resetRegionMethod_("reset");
const std::string ConsistentRegionContextImpl::startOperatorSubscribedMethod_(
  "startOperatorSubscribed");

static SPL::Meta::Type::Value const typesRstringInt64[2] = { SPL::Meta::Type::RSTRING,
                                                             SPL::Meta::Type::INT64 };
std::vector<SPL::Meta::Type::Value> ConsistentRegionContextImpl::rstringInt64Types_(
  typesRstringInt64,
  typesRstringInt64 + sizeof(typesRstringInt64) / sizeof(typesRstringInt64[0]));

static SPL::Meta::Type::Value const typesRstring[1] = { SPL::Meta::Type::RSTRING };
std::vector<SPL::Meta::Type::Value> ConsistentRegionContextImpl::rstringTypes_(
  typesRstring,
  typesRstring + sizeof(typesRstring) / sizeof(typesRstring[0]));

ConsistentRegionContextImpl::ConsistentRegionContextImpl(ADL::annotationType const& annot,
                                                         PEImpl& pe,
                                                         OPModel const& opmod,
                                                         CheckpointContextImpl* ckptContext,
                                                         uint32_t opIndex)
  : name_(CONSISTENT_REGION)
  , isStartRegion_(false)
  , isEndRegion_(false)
  , isTriggerOperator_(false)
  , drainTimeout_(1.0)
  , resetTimeout_(1.0)
  , opIndex_(opIndex)
  , opInstanceName_(opmod.name())
  , pe_(pe)
  , sequenceID_(1)
  , resetAttempt_(-1)
  , retiredID_(-1)
  , nonBlockingEnabled_(false)
  , caEvHandler_(pe, this, ckptContext)
  , ckptContext_(ckptContext)
  , peCRegionService_(pe.getConsistentRegionService())
{

    uint32_t numInputPorts = opmod.inputPorts().inputPort().size();

    consistentInputPort_.assign(numInputPorts, true);
    assert(annot.tag() == CC_REGION_ENTRY_ANNOTATION);

    ADL::annotationType::tuple_type tuple = annot.tuple();
    ADL::annotationTupleValueType::attr_const_iterator attrIt;

    ADL::annotationTupleValueType::attr_sequence const& attributes = tuple.attr();

    // construct aspect for trace messages, as OperatorTracker is not working yet
    // at this point of operator initialization
    std::string aspect = "#splapptrc," + pe.extraAspectInfo() + opmod.name();

    for (attrIt = attributes.begin(); attrIt != attributes.end(); attrIt++) {
        ADL::annotationAttributeValueType const& attr = *attrIt;
        if (attr.key() == CC_AUTONOMOUS_INPUT_PORT) {
            assert(attr.list().present());
            ADL::annotationListValueType const& list = attr.list().get();
            for (ADL::annotationListValueType::listEntry_const_iterator
                   listIt = list.listEntry().begin(),
                   listItEnd = list.listEntry().end();
                 listIt != listItEnd; ++listIt) {
                ADL::annotationListEntryValueType const& listEntry = *listIt;
                assert(listEntry.value().present());
                std::istringstream listVal(listEntry.value().get());
                uint32_t portIndex;
                listVal >> portIndex;
                consistentInputPort_[portIndex] = false;
                APPTRC(L_DEBUG,
                       "Found autonomous input port for operator " << opmod.name()
                                                                   << ": port index: " << portIndex,
                       aspect + "," SPL_OPER_DBG);
            }
        } else {
            assert(attr.value().present());
            std::istringstream val(attr.value().get());
            if (attr.key() == CC_IS_TRIGGER_OPERATOR) {
                val >> std::boolalpha >> isTriggerOperator_;
            } else if (attr.key() == CC_IS_START_REGION) {
                val >> std::boolalpha >> isStartRegion_;
                // Start of regions do not receive Cut and Restore Markers,
                // so all its input ports are autonomous
                consistentInputPort_.assign(numInputPorts, false);
            } else if (attr.key() == CC_IS_END_REGION) {
                val >> std::boolalpha >> isEndRegion_;
            } else if (attr.key() == CC_REGION_INDEX) {
                val >> regionIndex_;
                std::stringstream buf;
                // ObjectName must be in its canonical form
                // keys in lexical order
                // so index followed by type
                buf << CONTROL_DOMAIN << "index=" << regionIndex_ << CONSISTENT_REGION_NAME_TYPE;
                jcpObjectName_ = buf.str();
            } else if (attr.key() == CC_LOGICAL_INDEX) {
                // no action
            } else if (attr.key() == CC_DRAIN_TIMEOUT) {
                val >> drainTimeout_;
            } else if (attr.key() == CC_RESET_TIMEOUT) {
                val >> resetTimeout_;
            } else {
                APPTRC(L_ERROR,
                       "Error parsing AADL for operator " << opmod.name() << ": " << attr.key(),
                       aspect + "," SPL_OPER_DBG);
            }
        }
    }

    std::stringstream asp;
    asp << "consistentRegion_" << regionIndex_;
    aspect_ = asp.str();
    caEvHandler_.setAspect(aspect_);

    findConsistentControlPort(opmod, aspect + "," + aspect_);

    mustReset_ = (pe.getRelaunchCount() != 0) ? true : false;
    if (mustReset_) {
        sequenceID_ = -1;
    }

    peCRegionService_.registerConsistentOperator(*this);
}

void ConsistentRegionContextImpl::findConsistentControlPort(OPModel const& opmod,
                                                            std::string const& aspect)
{
    hasConsistentControlPort_ = false;

    // All input ports of an operator that start the region are oblivious to Cut and Restore markers
    if (isStartOfRegion()) {
        return;
    }

    for (uint32_t i = 0; i < opmod.inputPorts().inputPort().size(); i++) {
        APPTRC(L_DEBUG,
               "Operator " << opmod.name() << " input port index "
                           << opmod.inputPorts().inputPort()[i].index(),
               aspect);
        APPTRC(L_DEBUG,
               "Operator " << opmod.name() << " input port isControl "
                           << opmod.inputPorts().inputPort()[i].isControl(),
               aspect);
        if (isConsistentInputPort(i) && opmod.inputPorts().inputPort()[i].isControl().present() &&
            opmod.inputPorts().inputPort()[i].isControl().get()) {
            APPTRC(L_DEBUG, "Operator " << opmod.name() << " has consistent control port " << i,
                   aspect);
            hasConsistentControlPort_ = true;
            return;
        }
    }
}

std::vector<OperatorInputPort*> ConsistentRegionContextImpl::getInputPorts() const
{
    return consistentInputPortPtrs_;
}

void ConsistentRegionContextImpl::setOperatorImpl(OperatorImpl* op)
{
    for (uint32_t i = 0; i < op->getNumberOfInputPorts(); i++) {
        if (isConsistentInputPort(i)) {
            SPLAPPTRC(L_DEBUG, "Adding consistent input port " << i, aspect_);
            consistentInputPortPtrs_.push_back(&(op->getInputPortAt(i)));
        }
    }

    caEvHandler_.setOperatorImpl(op);

    // Start thread only after OperatorImpl is set
    if (isStartOfRegion()) {
        caEvHandler_.create();
    }
}

void ConsistentRegionContextImpl::resetRegion()
{
    pe_.getPlatform().reset(regionIndex_, opInstanceName_);
}

void ConsistentRegionContextImpl::drainCompleted()
{
    SPLAPPTRC(L_TRACE, "drainCompleted() started", aspect_);
    Distillery::AutoMutex am(mutex_);

    peCRegionService_.drainCompleted(regionIndex_, sequenceID_);
    resetAttempt_ = -1;
    SPLAPPTRC(L_TRACE, "drainCompleted() ended", aspect_);
}

void ConsistentRegionContextImpl::checkpointCompleted(int64_t seqId)
{
    SPLAPPTRC(L_TRACE, "checkpointCompleted() started", aspect_);
    Distillery::AutoMutex am(mutex_);

    peCRegionService_.checkpointCompleted(regionIndex_, seqId, opIndex_);
    SPLAPPTRC(L_TRACE, "checkpointCompleted() ended", aspect_);
}

void ConsistentRegionContextImpl::resetCompleted(int64_t seqId, int32_t resetAttempt)
{
    SPLAPPTRC(L_TRACE, "resetCompleted() started", aspect_);
    Distillery::AutoMutex am(mutex_);

    peCRegionService_.resetCompleted(regionIndex_, seqId, resetAttempt);
    // Only update state variables, if this reset attempt is the same.
    // The values might be different because this method is called by a
    // thread in the checkpointing thread pool. These values might be
    // changed by the processing thread when a new ResetMarker is
    // processed.
    if (seqId == sequenceID_ && resetAttempt == resetAttempt_) {
        mustReset_ = false;
    }

    SPLAPPTRC(L_TRACE, "resetCompleted() ended", aspect_);
}

void ConsistentRegionContextImpl::drain()
{
    pe_.getPlatform().drain(regionIndex_, opInstanceName_, sequenceID_);
}

void ConsistentRegionContextImpl::enableNonBlockingCheckpoint()
{
    nonBlockingEnabled_ = caEvHandler_.enableNonBlockingCheckpoint();
    if (nonBlockingEnabled_) {
        // an operator is counted as using non-blocking checkpointing only if
        // enableNonBlockingCheckpoint() is called and the operator has no state variable.
        // Otherwise it's counted as an ordinary operator with blocking checkpointing
        peCRegionService_.enableNonBlockingCheckpoint(regionIndex_, opIndex_);
    } else {
        SPLAPPTRC(
          L_WARN,
          "Non-blocking checkpointing is disabled because the operator has logic state variables",
          aspect_);
    }
}

enum ConsistentRegionContext::DrainResult ConsistentRegionContextImpl::makeConsistentNonBlocking()
{
    return caEvHandler_.makeConsistentNonBlocking();
}

void ConsistentRegionContextImpl::forwardNotification(Notification const& n)
{
    caEvHandler_.handleNotification(n);
}
