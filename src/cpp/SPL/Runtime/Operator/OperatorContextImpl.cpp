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

#include <SPL/Runtime/Operator/OperatorContextImpl.h>

#include <NAM/NAM_NameService.h>
#include <SAM/SAMInterfaceTypes.h>
#include <SAM/SAMNamespaceDefs.h>
#include <SAM/SAMTypes.h>
#include <SAM/applicationModel.h>
#include <SAM/augmentedApplicationModel.h>
#include <SAM/physicalApplicationModel.h>
#include <SPL/Runtime/Common/RuntimeAnnotations.h>
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Function/SPLCast.h>
#include <SPL/Runtime/Operator/CatchContext.h>
#include <SPL/Runtime/Operator/EventTime/EventTimeContext.h>
#include <SPL/Runtime/Operator/Operator.h>
#include <SPL/Runtime/Operator/OperatorMetricsImpl.h>
#include <SPL/Runtime/Operator/Port/StreamPropertyXML.h>
#include <SPL/Runtime/Operator/Port/SubscriptionExpression.h>
#include <SPL/Runtime/Operator/State/CheckpointContext.h>
#include <SPL/Runtime/Operator/State/CheckpointContextImpl.h>
#include <SPL/Runtime/Operator/State/ConsistentRegionContextImpl.h>
#include <SPL/Runtime/Operator/ThreadingModelContext.h>
#include <SPL/Runtime/ProcessingElement/PE.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/Type/ValueHandle.h>
#include <UTILS/RegEx.h>

#include <cassert>
#include <cxxabi.h>

using namespace SPL;
using namespace Distillery;

OperatorContextImpl::OperatorContextImpl(uint32_t index, PEImpl& pe, OperatorImpl& oper)
  : index_(index)
  , pe_(pe)
  , peHandle_(pe)
  , stateHandler_(NULL)
  , operatorMetricsHandler_(NULL)
{
    const PEModel& pemod = pe.getPEModel();
    const OPModel& opmod = pemod.operInstances().operInstance()[index];
    const PhysOPModel& physOpmod = static_cast<const PhysOPModel&>(opmod);

    operModel_ = &opmod;
    name_ = opmod.name();
    logicalName_ = physOpmod.logicalName();
    className_ = opmod.class_();

    singleThreadedOnInputs_ = opmod.singleThreadedOnInputs();
    singleThreadedOnOutputs_ = opmod.singleThreadedOnOutputs();

    if (physOpmod.parallelRegions().present()) {
        const PhysParallelRegions& parallelRegions = physOpmod.parallelRegions().get();
        for (PhysParallelRegions::parallelRegion_const_iterator
               it1 = parallelRegions.parallelRegion().begin(),
               it1End = parallelRegions.parallelRegion().end();
             it1 != it1End; ++it1) {
            localChannels_.push_back(it1->localChannelIndex());
            globalChannels_.push_back(it1->globalChannelIndex());
            localMaxChannels_.push_back(it1->localMaxChannels());
            globalMaxChannels_.push_back(it1->globalMaxChannels());
        }
    } else {
        globalChannels_.push_back(-1);
        localChannels_.push_back(-1);
        localMaxChannels_.push_back(0);
    }

    numOfInputPorts_ = opmod.inputPorts().inputPort().size();
    numOfOutputPorts_ = opmod.outputPorts().outputPort().size();

    for (uint32_t oiidx = 0; oiidx < numOfInputPorts_; ++oiidx) {
        uint32_t piidx;
        bool exist = pe.getInputPortIndex(piidx, index, oiidx);
        if (exist) {
            opIp2PeIp_[oiidx] = piidx;
        }
    }

    for (uint32_t ooidx = 0; ooidx < numOfOutputPorts_; ++ooidx) {
        uint32_t poidx;
        bool exist = pe.getOutputPortIndex(poidx, index, ooidx);
        if (exist) {
            opOp2PeOp_[ooidx] = poidx;
        }
    }

    using namespace xmlns::prod::streams::application;
    runtimeConstantsType::runtimeConstant_const_iterator it;
    const runtimeConstantsType::runtimeConstant_sequence& rcs =
      opmod.runtimeConstants().runtimeConstant();
    for (it = rcs.begin(); it != rcs.end(); it++) {
        const runtimeConstantsType::runtimeConstant_type& rc = *it;
        runtimeConstants_[rc.name()] = rc.value();
    }

    ckptInterval_ = -1;
    checkpointing_ = false;
    if (opmod.checkpointing().present()) {
        checkpointing_ = true;
        const checkpointingType& c = opmod.checkpointing().get();
        if (c.policy() == checkpointPolicyType::periodic) {
            ckptInterval_ = c.period().get();
        }
    }

    CheckpointContext* checkpointContext = NULL;
    if (pe.isStandalone() == false && checkpointing_ == true) {
        // this covers operators outside consistent region and with checkpoint config
        CheckpointContext::Kind ckptKind = CheckpointContext::operatorDriven;
        const checkpointingType& c = opmod.checkpointing().get();
        if (c.policy() == checkpointPolicyType::periodic) {
            ckptKind = CheckpointContext::periodic;
        }
        checkpointContext = new CheckpointContext(
          ckptKind, ckptInterval_, new CheckpointContextImpl(oper.getOperator(), opmod));
        optContextMap_[checkpointContext->getName()] = checkpointContext;
    }

    OperatorMetricsImpl* metrics = new OperatorMetricsImpl(*this, pe);
    opmHandle_ = metrics;
    opm_.reset(metrics);

    // Operator annotations
    PhysOPModel const& physOPModel = static_cast<const PhysOPModel&>(opmod);

    PADL::physOperInstanceType::annotation_const_iterator annotIt;
    PADL::physOperInstanceType::annotation_sequence const& annots = physOPModel.annotation();
    for (annotIt = annots.begin(); annotIt != annots.end(); annotIt++) {
        const ADL::annotationType& annot = *annotIt;
        if (annot.tag() == CC_REGION_ENTRY_ANNOTATION) {
            ConsistentRegionContextImpl* ccContext = new ConsistentRegionContextImpl(
              annot, pe, opmod, new CheckpointContextImpl(oper.getOperator(), opmod), index);
            optContextMap_[ccContext->getName()] = ccContext;
        } else if (annot.tag() == CATCH_ANNOTATION) {
            CatchContext* catchContext = new CatchContext(annot);
            optContextMap_[catchContext->getName()] = catchContext;
        } else if (annot.tag() == THREADING_ANNOTATION) {
            ThreadingModelContext* threadingContext = new ThreadingModelContext(annot);
            optContextMap_[threadingContext->getName()] = threadingContext;
        } else if (annot.tag() == ET_CONTEXT_ANNOTATION) {
            optContextMap_[EventTimeContext::NAME] = new EventTimeContext(annot, *this, oper);
        }
    }
}

OperatorContextImpl::~OperatorContextImpl()
{
    for (std::tr1::unordered_map<std::string, OptionalContext*>::iterator it =
           optContextMap_.begin();
         it != optContextMap_.end(); ++it) {
        delete it->second;
    }
}

CheckpointContextImpl* OperatorContextImpl::getCheckpointContextImpl()
{
    std::tr1::unordered_map<std::string, OptionalContext*>::iterator iter =
      optContextMap_.find(CHECKPOINT);
    if (iter != optContextMap_.end()) {
        CheckpointContext* checkpointContext = static_cast<CheckpointContext*>(iter->second);
        return checkpointContext->getImpl();
    }
    iter = optContextMap_.find(CONSISTENT_REGION);
    if (iter != optContextMap_.end()) {
        ConsistentRegionContextImpl* crContext =
          static_cast<ConsistentRegionContextImpl*>(iter->second);
        return crContext->getCheckpointContextImpl();
    } else {
        return NULL;
    }
}

void OperatorContextImpl::getRuntimeConstantValue(ValueHandle& val, string const& name)
{
    std::tr1::unordered_map<std::string, std::string>::const_iterator it;
    it = runtimeConstants_.find(name);
    assert(it != runtimeConstants_.end());
    std::stringstream ostr(it->second);
    ostr.imbue(locale::classic());
    val.deserializeWithNanAndInfs(ostr);
}

string const& OperatorContextImpl::getClassName() const
{
    return className_;
}

void OperatorContextImpl::setSubscriptionExpression(uint32_t opIIdx,
                                                    const SubscriptionExpression& subExpr) const
{
    if (pe_.isStandalone()) {
        APPTRC(L_DEBUG, "Running in standalone mode -- this call is ignored", SPL_OPER_DBG);
    } else {
        APPTRC(L_DEBUG,
               "Setting subscription expression '"
                 << subExpr << "' for operator's input port #" << opIIdx
                 << " (PE's iportid: " << getPeIPortId(opIIdx) << ")...",
               SPL_OPER_DBG);
        ADL::importedStreamsType subscription;
        auto_ptr<ADL::propertyBasedImportType> import(new ADL::propertyBasedImportType());
        import->subscription(subExpr.toString());
        import->importOperName(name_);
        import->index(0); // the index is only really meaningful in the ADL itself, so we just use
                          // 0; see defect 31902
        subscription.propertyBasedImport().set(import);
        try {
            pe_.getPlatform().setSubscription(getPeIPortId(opIIdx), subscription);
        } catch (DistilleryException const& e) {
            THROW_NESTED(SPLRuntimeSubscription, e.getExplanation(), e);
        }
    }
}

std::string OperatorContextImpl::getSubscription(uint32_t opIIdx) const
{
    if (pe_.isStandalone()) {
        APPTRC(L_DEBUG, "Running in standalone mode -- this call is ignored", SPL_OPER_DBG);
        return "";
    }
    APPTRC(L_DEBUG,
           "Getting subscription expression for operator's input port #"
             << opIIdx << " (PE's iportid: " << getPeIPortId(opIIdx) << ")...",
           SPL_OPER_DBG);
    ADL::importedStreamsType subscription;
    try {
        pe_.getPlatform().getSubscription(getPeIPortId(opIIdx), subscription);
    } catch (DistilleryException const& e) {
        THROW_NESTED(SPLRuntimeSubscription, e.getExplanation(), e);
    }
    assert(subscription.propertyBasedImport().present());
    return subscription.propertyBasedImport().get().subscription();
}

void OperatorContextImpl::setFilterExpression(uint32_t opIIdx, string const& filterExpr)
{
    if (pe_.isStandalone()) {
        APPTRC(L_DEBUG, "Running in standalone mode -- this call is ignored", SPL_OPER_DBG);
        return;
    }
    APPTRC(L_DEBUG,
           "Setting filter expression '" << filterExpr << "' for operator's input port #" << opIIdx
                                         << " (PE's iportid: " << getPeIPortId(opIIdx) << ")...",
           SPL_OPER_DBG);
    try {
        pe_.getPlatform().setFilter(getPeIPortId(opIIdx), filterExpr);
    } catch (DistilleryException const& e) {
        THROW_NESTED(SPLRuntimeFilter, e.getExplanation(), e);
    }
}

void OperatorContextImpl::removeFilterExpression(uint32_t opIIdx)
{
    if (pe_.isStandalone()) {
        APPTRC(L_DEBUG, "Running in standalone mode -- this call is ignored", SPL_OPER_DBG);
        return;
    }
    APPTRC(L_DEBUG,
           "Removing filter expression for operator's input port #"
             << opIIdx << " (PE's iportid: " << getPeIPortId(opIIdx) << ")...",
           SPL_OPER_DBG);
    try {
        pe_.getPlatform().setFilter(getPeIPortId(opIIdx), "");
    } catch (DistilleryException const& e) {
        THROW_NESTED(SPLRuntimeFilter, e.getExplanation(), e);
    }
}

string OperatorContextImpl::getFilterExpression(uint32_t opIIdx) const
{
    if (pe_.isStandalone()) {
        APPTRC(L_DEBUG, "Running in standalone mode -- this call is ignored", SPL_OPER_DBG);
        return "";
    }
    APPTRC(L_DEBUG,
           "Getting filter expression for operator's input port #"
             << opIIdx << " (PE's iportid: " << getPeIPortId(opIIdx) << ")...",
           SPL_OPER_DBG);
    string filter;
    try {
        pe_.getPlatform().getFilter(getPeIPortId(opIIdx), filter);
    } catch (DistilleryException const& e) {
        THROW_NESTED(SPLRuntimeFilter, e.getExplanation(), e);
    }
    return filter;
}

void OperatorContextImpl::registerStateHandler(StateHandler& handler)
{
    Distillery::AutoMutex am(mutex_);
    if (pe_.isStandalone()) {
        APPTRC(L_DEBUG, "Running in standalone mode -- this call is ignored", SPL_OPER_DBG);
        return;
    }
    stateHandler_ = &handler;
    std::tr1::unordered_map<std::string, OptionalContext*>::iterator iter =
      optContextMap_.find(CONSISTENT_REGION);
    if (iter != optContextMap_.end()) {
        ConsistentRegionContextImpl* crContext =
          static_cast<ConsistentRegionContextImpl*>(iter->second);
        crContext->registerStateHandler(handler);
    }
}

void OperatorContextImpl::getStreamProperties(uint32_t opOIdx, StreamPropertyCollection& spc) const
{
    if (pe_.isStandalone()) {
        APPTRC(L_DEBUG, "Running in standalone mode -- this call is ignored", SPL_OPER_DBG);
    } else {
        APPTRC(L_DEBUG,
               "Getting properties for operator's output port #"
                 << opOIdx << " (PE's oportid: " << getPeOPortId(opOIdx) << ")...",
               SPL_OPER_DBG);
        ADL::streamPropertiesType properties;
        SAM_INTERFACE::streamPropertyNamesType propertyNames;
        propertyNames.scope(SAM_INTERFACE::streamPropertyNamesScopingType::all);
        try {
            pe_.getPlatform().getStreamProperties(getPeOPortId(opOIdx), propertyNames, properties);
        } catch (DistilleryException const& e) {
            THROW_NESTED(SPLRuntimeStreamProperty, e.getExplanation(), e);
        }
        fromXML(properties, spc);
    }
}

static string join(vector<string> const& vals, string const& sep)
{
    ostringstream ostr;
    ostr.imbue(locale::classic());
    for (size_t i = 0, iu = vals.size(); i < iu; ++i) {
        if (i) {
            ostr << sep;
        }
        ostr << vals[i];
    }
    return ostr.str();
}

void OperatorContextImpl::getStreamProperties(uint32_t opOIdx,
                                              vector<string> const& names,
                                              StreamPropertyCollection& spc) const
{
    if (pe_.isStandalone()) {
        APPTRC(L_DEBUG, "Running in standalone mode -- this call is ignored", SPL_OPER_DBG);
    } else {
        APPTRC(L_DEBUG,
               "Getting properties [" << join(names, ",") << "] for operator's output port #"
                                      << opOIdx << " (PE's oportid: " << getPeOPortId(opOIdx)
                                      << ")...",
               SPL_OPER_DBG);
        SAM_INTERFACE::streamPropertyNamesType propertyNames;
        propertyNames.scope(SAM_INTERFACE::streamPropertyNamesScopingType::listed);
        vector<string>::const_iterator it;
        for (it = names.begin(); it != names.end(); ++it) {
            auto_ptr<SAM_INTERFACE::streamPropertyNameType> name(
              new SAM_INTERFACE::streamPropertyNameType());
            name->name(*it);
            propertyNames.name().push_back(name);
        }
        ADL::streamPropertiesType properties;
        try {
            pe_.getPlatform().getStreamProperties(getPeOPortId(opOIdx), propertyNames, properties);
        } catch (DistilleryException const& e) {
            THROW_NESTED(SPLRuntimeStreamProperty, e.getExplanation(), e);
        }
        fromXML(properties, spc);
    }
}

void OperatorContextImpl::setStreamProperties(uint32_t opOIdx,
                                              StreamPropertyCollection const& spc) const
{
    if (pe_.isStandalone()) {
        APPTRC(L_DEBUG, "Running in standalone mode -- this call is ignored", SPL_OPER_DBG);
    } else {
        APPTRC(L_DEBUG,
               "Setting property collection " << spc << " for operator's output port #" << opOIdx
                                              << " (PE's oportid: " << getPeOPortId(opOIdx)
                                              << ")...",
               SPL_OPER_DBG);
        auto_ptr<ADL::streamPropertiesType> sspc = toXML(spc);
        try {
            pe_.getPlatform().setStreamProperties(getPeOPortId(opOIdx), *sspc);
        } catch (DistilleryException const& e) {
            THROW_NESTED(SPLRuntimeStreamProperty, e.getExplanation(), e);
        }
    }
}

void OperatorContextImpl::addStreamProperties(uint32_t opOIdx,
                                              StreamPropertyCollection const& spc) const
{
    if (pe_.isStandalone()) {
        APPTRC(L_DEBUG, "Running in standalone mode -- this call is ignored", SPL_OPER_DBG);
    } else {
        APPTRC(L_DEBUG,
               "Adding property collection " << spc << " for operator's output port #" << opOIdx
                                             << " (PE's oportid: " << getPeOPortId(opOIdx)
                                             << ")...",
               SPL_OPER_DBG);
        auto_ptr<ADL::streamPropertiesType> sspc = toXML(spc);
        try {
            pe_.getPlatform().addStreamProperties(getPeOPortId(opOIdx), *sspc);
        } catch (DistilleryException const& e) {
            THROW_NESTED(SPLRuntimeStreamProperty, e.getExplanation(), e);
        }
    }
}

void OperatorContextImpl::removeStreamProperties(uint32_t opOIdx, vector<string> const& spc) const
{
    if (pe_.isStandalone()) {
        APPTRC(L_DEBUG, "Running in standalone mode -- this call is ignored", SPL_OPER_DBG);
    } else {
        APPTRC(L_DEBUG,
               "Removing properties [" << join(spc, ",") << "]"
                                       << " for operator's output port #" << opOIdx
                                       << " (PE's oportid: " << getPeOPortId(opOIdx) << ")...",
               SPL_OPER_DBG);
        SAM_INTERFACE::streamPropertyNamesType names;
        names.scope(SAM_INTERFACE::streamPropertyNamesScopingType::listed);
        vector<string>::const_iterator it;
        for (it = spc.begin(); it != spc.end(); ++it) {
            auto_ptr<SAM_INTERFACE::streamPropertyNameType> name(
              new SAM_INTERFACE::streamPropertyNameType());
            name->name(*it);
            names.name().push_back(name);
        }
        try {
            pe_.getPlatform().removeStreamProperties(getPeOPortId(opOIdx), names);
        } catch (DistilleryException const& e) {
            THROW_NESTED(SPLRuntimeStreamProperty, e.getExplanation(), e);
        }
    }
}

void OperatorContextImpl::getStreamProperty(uint32_t opOIdx,
                                            string const& name,
                                            StreamProperty& sp) const
{
    if (pe_.isStandalone()) {
        APPTRC(L_DEBUG, "Running in standalone mode -- this call is ignored", SPL_OPER_DBG);
    } else {
        APPTRC(L_DEBUG,
               "Getting property '" << sp.getName() << "' for operator's output port #" << opOIdx
                                    << " (PE's oportid: " << getPeOPortId(opOIdx) << ")...",
               SPL_OPER_DBG);
        SAM_INTERFACE::streamPropertyNameType propertyName;
        propertyName.name(name);
        ADL::streamPropertyType property;
        try {
            pe_.getPlatform().getStreamProperty(getPeOPortId(opOIdx), propertyName, property);
        } catch (DistilleryException const& e) {
            THROW_NESTED(SPLRuntimeStreamProperty, e.getExplanation(), e);
        }
        sp = fromXML(property);
    }
}

bool OperatorContextImpl::hasStreamProperty(uint32_t opOIdx, std::string const& sp) const
{
    if (pe_.isStandalone()) {
        APPTRC(L_DEBUG, "Running in standalone mode -- this call is ignored", SPL_OPER_DBG);
    } else {
        APPTRC(L_DEBUG,
               "Checking the existence of property '"
                 << sp << "' for operator's output port #" << opOIdx
                 << " (PE's oportid: " << getPeOPortId(opOIdx) << ")...",
               SPL_OPER_DBG);
        SAM_INTERFACE::streamPropertyNameType propertyName;
        propertyName.name(sp);
        ADL::streamPropertyType property;
        try {
            pe_.getPlatform().getStreamProperty(getPeOPortId(opOIdx), propertyName, property);
        } catch (DistilleryException const& e) {
            return false;
        }
    }

    return true;
}

void OperatorContextImpl::setStreamProperty(uint32_t opOIdx, StreamProperty const& sp) const
{
    if (pe_.isStandalone()) {
        APPTRC(L_DEBUG, "Running in standalone mode -- this call is ignored", SPL_OPER_DBG);
    } else {
        APPTRC(L_DEBUG,
               "Setting property '" << sp.getName() << "' for operator's output port #" << opOIdx
                                    << " (PE's oportid: " << getPeOPortId(opOIdx) << ")...",
               SPL_OPER_DBG);
        auto_ptr<ADL::streamPropertyType> property = toXML(sp);
        try {
            pe_.getPlatform().setStreamProperty(getPeOPortId(opOIdx), *property);
        } catch (DistilleryException const& e) {
            THROW_NESTED(SPLRuntimeStreamProperty, e.getExplanation(), e);
        }
    }
}

void OperatorContextImpl::addStreamProperty(uint32_t opOIdx, StreamProperty const& sp) const
{
    if (pe_.isStandalone()) {
        APPTRC(L_DEBUG, "Running in standalone mode -- this call is ignored", SPL_OPER_DBG);
    } else {
        APPTRC(L_DEBUG,
               "Adding property '" << sp.getName() << "' for operator's output port #" << opOIdx
                                   << " (PE's oportid: " << getPeOPortId(opOIdx) << ")...",
               SPL_OPER_DBG);
        auto_ptr<ADL::streamPropertyType> property = toXML(sp);
        try {
            pe_.getPlatform().addStreamProperty(getPeOPortId(opOIdx), *property);
        } catch (DistilleryException const& e) {
            THROW_NESTED(SPLRuntimeStreamProperty, e.getExplanation(), e);
        }
    }
}

void OperatorContextImpl::removeStreamProperty(uint32_t opOIdx, string const& sp) const
{
    if (pe_.isStandalone()) {
        APPTRC(L_DEBUG, "Running in standalone mode -- this call is ignored", SPL_OPER_DBG);
    } else {
        APPTRC(L_DEBUG,
               "Removing property '" << sp << "' for operator's output port #" << opOIdx
                                     << " (PE's oportid: " << getPeOPortId(opOIdx) << ")...",
               SPL_OPER_DBG);
        SAM_INTERFACE::streamPropertyNameType name;
        name.name(sp);
        try {
            pe_.getPlatform().removeStreamProperty(getPeOPortId(opOIdx), name);
        } catch (DistilleryException const& e) {
            THROW_NESTED(SPLRuntimeStreamProperty, e.getExplanation(), e);
        }
    }
}

bool OperatorContextImpl::getPeIPortIndex(uint32_t opIPortIndex, uint32_t& peIPortIndex) const
{
    if (opIp2PeIp_.count(opIPortIndex) == 0) {
        return false;
    }
    peIPortIndex = opIp2PeIp_.find(opIPortIndex)->second;
    return true;
}

bool OperatorContextImpl::getPeOPortIndex(uint32_t opOPortIndex, uint32_t& peOPortIndex) const
{
    if (opOp2PeOp_.count(opOPortIndex) == 0) {
        return false;
    }
    peOPortIndex = opOp2PeOp_.find(opOPortIndex)->second;
    return true;
}

uint32_t OperatorContextImpl::getPeOPortId(uint32_t opOPortIndex) const
{
    uint32_t peOPortIndex = 0;
#ifndef NDEBUG
    bool hasPePort =
#endif // NDEBUG
      getPeOPortIndex(opOPortIndex, peOPortIndex);
    assert(hasPePort);
    return PEImpl::instance().getOutputPortId(peOPortIndex);
}

uint32_t OperatorContextImpl::getPeIPortId(uint32_t opIPortIndex) const
{
    uint32_t peIPortIndex = 0;
#ifndef NDEBUG
    bool hasPePort =
#endif // NDEBUG
      getPeIPortIndex(opIPortIndex, peIPortIndex);
    assert(hasPePort);
    return PEImpl::instance().getInputPortId(peIPortIndex);
}

uint32_t OperatorContextImpl::getGlobalIndex() const
{
    return operModel_->index();
}

uint32_t OperatorContextImpl::getGlobalDefinitionIndex() const
{
    return operModel_->definitionIndex();
}

OptionalContext* OperatorContextImpl::getOptionalContext(std::string const& name) const
{
    if (optContextMap_.find(name) != optContextMap_.end()) {
        return optContextMap_.find(name)->second;
    } else {
        return NULL;
    }
}
