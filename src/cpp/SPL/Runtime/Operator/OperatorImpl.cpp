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

#include <SPL/Runtime/Operator/OperatorImpl.h>

#include <SAM/SAMNamespaceDefs.h>
#include <SAM/augmentedApplicationModel.h>
#include <SPL/Runtime/Common/RuntimeAnnotations.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Operator/CatchContext.h>
#include <SPL/Runtime/Operator/EventTime/EventTimeContext.h>
#include <SPL/Runtime/Operator/Operator.h>
#include <SPL/Runtime/Operator/OperatorContextImpl.h>
#include <SPL/Runtime/Operator/OperatorThread.h>
#include <SPL/Runtime/Operator/OperatorTracker.h>
#include <SPL/Runtime/Operator/Port/ActiveQueue.h>
#include <SPL/Runtime/Operator/Port/OpInputPortDelegate.h>
#include <SPL/Runtime/Operator/Port/OperatorInputPortImpl.h>
#include <SPL/Runtime/Operator/Port/OperatorOutputPortImpl.h>
#include <SPL/Runtime/Operator/Port/ProcessSignal.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>
#include <SPL/Runtime/Operator/Port/SubmitSignal.h>
#include <SPL/Runtime/Operator/State/ConsistentRegionContext.h>
#include <SPL/Runtime/ProcessingElement/PE.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/ProcessingElement/ViewProperties.h>
#include <SPL/Runtime/Window/Window.h>

using namespace SPL;
using namespace std;
using namespace std::tr1;

typedef PEModel::tupleTypes_type::tupleType_type AADLTupleType;
typedef PEModel::tupleTypes_type::tupleType_sequence AADLTupleTypes;
static string tupleTypeName(AADLTupleType const& tuple)
{
    string name = "tuple<";
    for (size_t i = 0, iu = tuple.attribute().size(); i < iu; ++i) {
        if (i) {
            name += ",";
        }
        name += tuple.attribute()[i].type() + " " + tuple.attribute()[i].name();
    }
    return name + ">";
}

OperatorImpl::OperatorImpl(Operator& oper)
  : index_(OperatorTracker::getCurrentOperator())
  , toolkitIndex_(0)
  , oper_(oper)
  , pe_(PEImpl::instance())
  , isOptimized_(pe_.isOptimized())
  , numThreads_(0)
  , paramValues_(NULL)
  , watermarkFromOutputEventTime_(false)
  , connected_(0)
{
    OperatorContextImpl* context = new OperatorContextImpl(index_, pe_, *this);
    contextHandle_ = context;
    context_.reset(context);

    pe_.addOperator(*this); // context has to be there
    pe_.instantiateOperatorMetrics(*this);

    OPModel const& opModel = context->getOperatorModel();
    const PhysOPModel& physOpmod = static_cast<const PhysOPModel&>(opModel);
    PEModel const& peModel = PEImpl::instance().getPEModel();
    toolkitIndex_ = physOpmod.toolkitIndex();
    unordered_map<size_t, size_t> rawIdxMap;
    AADLTupleTypes const& types = peModel.tupleTypes().tupleType();
    for (size_t i = 0, iu = types.size(); i < iu; ++i) {
        rawIdxMap[types[i].index()] = i;
    }

    for (uint32_t i = 0, iu = context_->getNumberOfInputPorts(); i < iu; ++i) {
        OperatorInputPortImplPtr iptr(new OperatorInputPortImpl(*this, i));
        if (opModel.inputPorts().inputPort()[i].isControl().present()) {
            iptr->setControl(opModel.inputPorts().inputPort()[i].isControl().get());
        }
        iport_.push_back(iptr);
        AADLTupleType const& type =
          peModel.tupleTypes()
            .tupleType()[rawIdxMap[opModel.inputPorts().inputPort()[i].tupleTypeIndex()]];
        bool isMutable = opModel.inputPorts().inputPort()[i].isMutable();
        string name = tupleTypeName(type);
        ProcessSignalPtr signal(new TypedProcessSignal(*this, i, name, isMutable));
        OpInputPortDelegate<Punctuation const&> pcall(&oper, i);
        // Add a delegate for NativeByteBuffer which will be called for Java operators since
        // they accept NativeByteBuffers directly and do not need to create/serialize/deserialize
        // Tuples
        OpInputPortDelegate<NativeByteBuffer&> btcall(&oper, i);
        if (isMutable) {
            OpInputPortDelegate<Tuple&> mtcall(&oper, i);
            signal->connect(mtcall, pcall, btcall);
        } else {
            OpInputPortDelegate<Tuple const&> mtcall(&oper, i);
            signal->connect(mtcall, pcall, btcall);
        }
        process_.push_back(signal);
    }
    for (uint32_t i = 0, iu = context_->getNumberOfOutputPorts(); i < iu; ++i) {
        OperatorOutputPortImplPtr optr(new OperatorOutputPortImpl(*this, i));
        oport_.push_back(optr);

        const ADL::operInstanceOutputPortType& modelOport = opModel.outputPorts().outputPort()[i];
        AADLTupleType const& type =
          peModel.tupleTypes().tupleType()[rawIdxMap[modelOport.tupleTypeIndex()]];
        bool isMutable = modelOport.isMutable();
        string name = tupleTypeName(type);
        SubmitSignalPtr signal(new TypedSubmitSignal(*this, i, name, isMutable));
        submit_.push_back(signal);
        typeInfos_.push_back(&signal->getTypeId());
    }

    OptionalContext* opContext = context_->getOptionalContext(CONSISTENT_REGION);
    if (opContext != NULL) {
        ccContext_ = static_cast<ConsistentRegionContextImpl*>(opContext);
        // Setting pointer to operator impl later, as the context needs info from the operator
        // that is set after the instantiation
        ccContext_->setOperatorImpl(this);
    } else {
        ccContext_ = NULL;
    }

    CatchContext* catchContext = NULL;
    opContext = context_->getOptionalContext(CATCH_ANNOTATION);
    if (opContext != NULL) {
        catchContext = static_cast<CatchContext*>(opContext);
    }

    // Run try catch path if the operator is in a consistent region or
    // it has the @catch annotation
    mustRunTryCatchPath_ =
      (catchContext != NULL && catchContext->getExceptionKind() != NONE) || (ccContext_ != NULL);

    EventTimeContext* etCtx =
      dynamic_cast<EventTimeContext*>(context_->getOptionalContext(EventTimeContext::NAME));
    eventTimeContext_ = etCtx != NULL ? etCtx->impl_.get() : NULL;
}

OperatorImpl::~OperatorImpl() {}

static string getTupleTypeName(Tuple const& tuple)
{
    Meta::BaseType const& type = Meta::BaseType::makeType(tuple);
    return type.getName();
}

void OperatorImpl::portTypeCheck(Tuple const& tuple, uint32_t port)
{
    if (typeid(tuple) != *typeInfos_[port]) {
        SPLTRACEMSGANDTHROW(
          SPLRuntimeTypeMismatch, L_ERROR,
          SPL_RUNTIME_TUPLE_TYPE_DOES_NOT_MATCH_PORT_TYPE(
            getTupleTypeName(tuple), submit_[port]->getTupleType().getName(), port),
          SPL_OPER_DBG);
    }
}

void OperatorImpl::registerWindow(BaseWindow& window, uint32_t port)
{
    Distillery::AutoMutex am(windowMutex_);
    windows_[port].push_back(&window);
}

bool OperatorImpl::unregisterWindow(BaseWindow& window, uint32_t port)
{
    Distillery::AutoMutex am(windowMutex_);
    bool unregistered = false;
    WindowRegistry::iterator registryIter = windows_.find(port);
    if (registryIter != windows_.end()) {
        std::vector<BaseWindow*>& windowVector = registryIter->second;
        std::vector<BaseWindow*>::iterator windowIter =
          std::find(windowVector.begin(), windowVector.end(), &window);
        if (windowIter != windowVector.end()) {
            windowVector.erase(windowIter);
            unregistered = true;
        }
    }

    return unregistered;
}

void OperatorImpl::registerActiveQueue(ActiveQueue& queue, uint32_t port)
{
    Distillery::AutoMutex am(activeQueueMutex_);
    activeQueues_[port].push_back(&queue);
    context_->getMetricsImpl().setQueueSize(port, queue.getMaxQueueSize());
}

void OperatorImpl::shutdownWindowsOnPort(uint32_t port)
{
    APPTRC(L_DEBUG, "Shutting down windows on port " << port << "...", SPL_OPER_DBG);
    Distillery::AutoMutex am(windowMutex_);
    if (windows_.count(port) > 0) {
        vector<BaseWindow*>& pwins = windows_.find(port)->second;
        for (size_t i = 0, iu = pwins.size(); i < iu; ++i) {
            pwins[i]->shutdown();
        }
    }
    APPTRC(L_DEBUG, "Shut down windows on port " << port, SPL_OPER_DBG);
}

void OperatorImpl::drainActiveQueuesOnPort(uint32_t port)
{
    APPTRC(L_DEBUG, "Draining down active queues on port " << port << "...", SPL_OPER_DBG);
    Distillery::AutoMutex am(activeQueueMutex_);
    if (activeQueues_.count(port) > 0) {
        vector<ActiveQueue*>& pqueues = activeQueues_.find(port)->second;
        for (size_t i = 0, iu = pqueues.size(); i < iu; ++i) {
            pqueues[i]->drain();
        }
    }
    APPTRC(L_DEBUG, "Draining down active queues on port " << port, SPL_OPER_DBG);
}

void OperatorImpl::notifyScheduledQueuePortClosed()
{
    pe_.notifyScheduledQueuePortClosed();
}

// FIXME: I am not convinced it is okay that we don't grab windowMutex_ here. It
// may be the cause of some errors on shutdown. On the other hand, if we tried
// to grab the lock while joining the threads, we would be deadlocked with
// the thread calling shutdown on the windows. So fixing this may require do
// refactoring of how we protect the windows_ hash table. Note this also applies
// to how we access activeQueues_.
void OperatorImpl::joinWindowThreads()
{
    APPTRC(L_DEBUG, "Joining window threads...", SPL_OPER_DBG);
    unordered_map<uint32_t, vector<BaseWindow*> >::iterator it;
    for (it = windows_.begin(); it != windows_.end(); ++it) {
        vector<BaseWindow*>& pwins = it->second;
        for (size_t i = 0, iu = pwins.size(); i < iu; ++i) {
            pwins[i]->join();
        }
    }
    APPTRC(L_DEBUG, "Joined window threads", SPL_OPER_DBG);
}

void OperatorImpl::joinActiveQueueThreads()
{
    int dummy;
    APPTRC(L_DEBUG, "Joining active queue threads...", SPL_OPER_DBG);
    unordered_map<uint32_t, vector<ActiveQueue*> >::iterator it;
    for (it = activeQueues_.begin(); it != activeQueues_.end(); ++it) {
        vector<ActiveQueue*>& pqueues = it->second;
        for (size_t i = 0, iu = pqueues.size(); i < iu; ++i) {
            pqueues[i]->join(&dummy);
        }
    }
    APPTRC(L_DEBUG, "Joined active queue threads", SPL_OPER_DBG);
}

void OperatorImpl::joinOperatorThreads()
{
    APPTRC(L_DEBUG, "Joining operator threads", SPL_OPER_DBG);
    int dummy;
    for (size_t i = 0, iu = thread_.size(); i < iu; ++i) {
        thread_[i]->join(&dummy);
    }
    APPTRC(L_DEBUG, "Joined operator threads", SPL_OPER_DBG);
}

void OperatorImpl::allPortsReadyRaw()
{
    APPTRC(L_DEBUG, "Starting window threads...", SPL_OPER_DBG);
    OperatorTracker::setCurrentOperator(context_->getIndex());
    Distillery::AutoMutex am(windowMutex_);
    unordered_map<uint32_t, vector<BaseWindow*> >::iterator it;
    for (it = windows_.begin(); it != windows_.end(); ++it) {
        vector<BaseWindow*>& pwins = it->second;
        for (size_t i = 0, iu = pwins.size(); i < iu; ++i) {
            pwins[i]->start();
        }
    }
    APPTRC(L_DEBUG, "Started window threads", SPL_OPER_DBG);
    oper_.allPortsReady();
    if (ccContext_ != NULL) {
        APPTRC(L_DEBUG, "ccContext not null", SPL_OPER_DBG);
        ccContext_->allPortsReady();
    }
    OperatorTracker::resetCurrentOperator();
}

void OperatorImpl::prepareToShutdownRaw()
{
    {
        APPTRC(L_DEBUG, "Calling shutdown on windows...", SPL_OPER_DBG);
        OperatorTracker::setCurrentOperator(context_->getIndex());
        Distillery::AutoMutex am(windowMutex_);
        unordered_map<uint32_t, vector<BaseWindow*> >::iterator it;
        for (it = windows_.begin(); it != windows_.end(); ++it) {
            vector<BaseWindow*>& pwins = it->second;
            for (size_t i = 0, iu = pwins.size(); i < iu; ++i) {
                pwins[i]->shutdown();
            }
            APPTRC(L_DEBUG, "Called shutdown on windows", SPL_OPER_DBG);
        }
        OperatorTracker::resetCurrentOperator();
    }
    {
        APPTRC(L_DEBUG, "Calling shutdown on active queues...", SPL_OPER_DBG);
        Distillery::AutoMutex am(activeQueueMutex_);
        unordered_map<uint32_t, vector<ActiveQueue*> >::iterator it;
        for (it = activeQueues_.begin(); it != activeQueues_.end(); ++it) {
            vector<ActiveQueue*>& pqueues = it->second;
            for (size_t i = 0, iu = pqueues.size(); i < iu; ++i) {
                pqueues[i]->shutdown();
            }
            APPTRC(L_DEBUG, "Called shutdown on active queues", SPL_OPER_DBG);
        }
    }
    {
        APPTRC(L_DEBUG, "Calling shutdown handlers for the operator...", SPL_OPER_DBG);
        typedef vector<pair<ShutdownFunction, void*> >::iterator iter;
        Distillery::AutoMutex am(shutdownFuncMutex_);
        for (iter it = shutdownFunc_.begin(); it != shutdownFunc_.end(); ++it) {
            (*it->first)(it->second);
        }
        APPTRC(L_DEBUG, "Called shutdown handlers for the operator", SPL_OPER_DBG);
    }
    oper_.prepareToShutdown();
    if (ccContext_ != NULL) {
        ccContext_->prepareToShutdown();
    }
}

void OperatorImpl::registerShutdownHandler(void (*shutdownFunc)(void*), void* param)
{
    Distillery::AutoMutex am(shutdownFuncMutex_);
    shutdownFunc_.push_back(make_pair(shutdownFunc, param));
}

uint32_t OperatorImpl::createThreads(uint32_t num)
{
    increaseCurrentNumberOfThreads(num);
    APPTRC(L_DEBUG, "Creating " << num << " operator threads...", SPL_OPER_DBG);
    for (uint32_t i = 0; i < num; ++i) {
        OperatorThreadPtr thread(new OperatorThread(*this, thread_.size()));
        thread_.push_back(thread);
        thread->create();
    }
    APPTRC(L_INFO, "Created " << num << " operator threads", SPL_OPER_DBG);
    return thread_.size() - num;
}

OperatorThread& OperatorImpl::getThread(uint32_t idx)
{
    if (static_cast<size_t>(idx) >= thread_.size()) {
        THROW(SPLRuntimeInvalidIndex, "Operator thread index '" << idx << "' out of "
                                                                << "valid range [0.."
                                                                << (thread_.size() - 1) << "]");
    }
    return *thread_[idx];
}

void OperatorImpl::performSynchronousCheckpoint(NetworkByteBuffer const& opstate)
{
    pe_.performSynchronousCheckpoint(*this, opstate);
}

void OperatorImpl::badPortIndex(uint32_t port, uint32_t size)
{
    SPLTRACEMSGANDTHROW(SPLRuntimeInvalidIndex, L_ERROR,
                        SPL_RUNTIME_OUTPUT_PORT_INDEX_OUT_OF_RANGE(port, size), SPL_OPER_DBG);
}

void OperatorImpl::badPortCtor(uint32_t port)
{
    SPLTRACEMSGANDTHROW(SPLRuntimeInvalidIndex, L_ERROR, SPL_RUNTIME_OUTPUT_PORT_CTOR(port),
                        SPL_OPER_DBG);
}

void OperatorImpl::badConstTuple(uint32_t port)
{
    SPLTRACEMSGANDTHROW(SPLRuntimeTypeMismatch, L_ERROR,
                        SPL_RUNTIME_CONSTANT_TUPLE_SENT_TO_MUTATING_PORT(port), SPL_OPER_DBG);
}

const unordered_set<string>& OperatorImpl::getParameterNames()
{
    if (!paramValues_) {
        loadParameterInformation();
    }
    assert(paramValues_);
    return paramNames_;
}

const OperatorImpl::ParameterValueListType& OperatorImpl::getParameterValues(string const& param)
{
    if (!paramValues_) {
        loadParameterInformation();
    }
    assert(paramValues_);
    ParameterMapType::const_iterator it = paramValues_->find(param);
    if (it == paramValues_->end()) {
        SPLTRACEMSGANDTHROW(SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_RUNTIME_PARAMETER_NAME_NOT_FOUND(param), SPL_OPER_DBG);
    }
    return it->second;
}

void OperatorImpl::loadParameterInformation()
{
    paramValues_ = &oper_.getParameters();
    assert(paramValues_);

    // Load up the names
    ParameterMapType::const_iterator it;
    for (it = paramValues_->begin(); it != paramValues_->end(); it++) {
        paramNames_.insert(it->first);
    }
}

void OperatorImpl::addView(const ViewProperties& view)
{
    // Assert that the view is added to the operator with which it is associated
    if (this->getIndex() != view.getOperator().getIndex()) {
        THROW(SPLRuntimeAssertionFailed,
              "View id " << view.getViewId() << " is associated with operator index "
                         << view.getOperator().getIndex()
                         << " but is being added to operator with index " << this->getIndex());
    }
    submit_[view.getPort()]->addView(view);
}

void OperatorImpl::deleteView(const ViewProperties& view)
{
    submit_[view.getPort()]->deleteView(view);
}

void OperatorImpl::visualizeTuple(uint32_t port, const NativeByteBuffer& rawTuple)
{
    submit_[port]->visualize(rawTuple);
}

PayloadContainer* OperatorImpl::getPunctPayloadContainer(Punctuation& punct) const
{
    PayloadContainer* payloadC = punct.getPayloadContainer();
    if (payloadC == NULL) {
        payloadC = new PayloadContainer();
        punct.setPayloadContainer(payloadC);
    }

    return payloadC;
}

void OperatorImpl::createDrainPunctuationPayload(Punctuation& punct) const
{
    assert(ccContext_ != NULL && !ccContext_->mustReset());

    PayloadContainer* payloadC = getPunctPayloadContainer(punct);

    // Create payload only if punctuation does not already have it
    if (payloadC->find(DrainPunctPayload::name) == NULL) {
        APPTRC(L_TRACE, "Creating DrainMarker payload: " << ccContext_->getSequenceId(),
               SPL_OPER_DBG);
        payloadC->add(DrainPunctPayload::name,
                      *(new DrainPunctPayload(ccContext_->getSequenceId())));
    }
}

void OperatorImpl::createResetPunctuationPayload(Punctuation& punct) const
{
    assert(ccContext_ != NULL);

    PayloadContainer* payloadC = getPunctPayloadContainer(punct);

    // Create payload only if punctuation does not already have it
    if (payloadC->find(ResetPunctPayload::name) == NULL) {
        APPTRC(L_TRACE,
               "Creating ResetMarker payload: " << ccContext_->getSequenceId() << " "
                                                << ccContext_->getResetAttempt(),
               SPL_OPER_DBG);
        payloadC->add(
          ResetPunctPayload::name,
          *(new ResetPunctPayload(ccContext_->getSequenceId(), ccContext_->getResetAttempt())));
    }
}

void OperatorImpl::createResumePunctuationPayload(Punctuation& punct) const
{
    assert(ccContext_ != NULL && !ccContext_->mustReset());

    PayloadContainer* payloadC = getPunctPayloadContainer(punct);

    // Create payload only if punctuation does not already has it
    if (payloadC->find(ResumePunctPayload::name) == NULL) {
        APPTRC(L_TRACE,
               "Creating ResumeMarker payload: " << ccContext_->getSequenceId() << " "
                                                 << ccContext_->getRetiredId(),
               SPL_OPER_DBG);
        payloadC->add(
          ResumePunctPayload::name,
          *(new ResumePunctPayload(ccContext_->getSequenceId(), ccContext_->getRetiredId())));
    }
}

void OperatorImpl::autoRegisterStateHandler()
{
    // Auto registering StateHandler if available
    try {
        APPTRC(L_DEBUG, "Trying to cast Operator to StateHandler", SPL_OPER_DBG);
        StateHandler& sh = dynamic_cast<StateHandler&>(oper_);
        APPTRC(L_DEBUG, "Auto registering Operator as StateHandler", SPL_OPER_DBG);
        context_->registerStateHandler(sh);
    } catch (std::bad_cast& e) {
        APPTRC(L_DEBUG, "Operator does not inherit from StateHandler. Not auto registering.",
               SPL_OPER_DBG);
    }
}

void OperatorImpl::initializeEventTimeContext()
{
    if (eventTimeContext_ != NULL) {
        eventTimeContext_->initialize();
        watermarkFromOutputEventTime_ =
          (EventTimeContext::outputEventTime == eventTimeContext_->getMode());
    }
}

void OperatorImpl::handleSubmitException()
{
    if (ccContext_ != NULL) {
        SPLAPPTRC(L_DEBUG, "Exception on tuple submission. Cannot establish consistent state.",
                  SPL_OPER_DBG);
        ccContext_->setMustReset(true);
    }
    pe_.setMustRethrowException();
}

void OperatorImpl::tryCatchSubmit(Tuple& tuple, uint32_t port)
{
    try {
        submitCommon(tuple, port);
    } catch (Distillery::DistilleryException const& e) {
        SPLAPPTRC(L_ERROR, "Exception received during submission: " << e, SPL_OPER_DBG);
        handleSubmitException();
        throw e;
    } catch (std::exception const& e) {
        SPLAPPTRC(L_ERROR, "Exception received during submission: " << e.what(), SPL_OPER_DBG);
        handleSubmitException();
        throw e;
    } catch (...) {
        SPLAPPTRC(L_ERROR, "Unknown exception received during submission: ", SPL_OPER_DBG);
        handleSubmitException();
        throw;
    }
}

void OperatorImpl::tryCatchSubmit(NativeByteBuffer& buffer, uint32_t port)
{
    try {
        submitCommon(buffer, port);
    } catch (Distillery::DistilleryException const& e) {
        SPLAPPTRC(L_DEBUG, "Exception received during submission: " << e, SPL_OPER_DBG);
        handleSubmitException();
        throw e;
    } catch (std::exception const& e) {
        SPLAPPTRC(L_DEBUG, "Exception received during submission: " << e.what(), SPL_OPER_DBG);
        handleSubmitException();
        throw e;
    } catch (...) {
        SPLAPPTRC(L_DEBUG, "Unknown exception received during submission: ", SPL_OPER_DBG);
        handleSubmitException();
        throw;
    }
}

void OperatorImpl::tryCatchSubmit(Tuple const& tuple, uint32_t port)
{
    try {
        submitCommon(tuple, port);
    } catch (Distillery::DistilleryException const& e) {
        SPLAPPTRC(L_DEBUG, "Exception received during submission: " << e, SPL_OPER_DBG);
        handleSubmitException();
        throw e;
    } catch (std::exception const& e) {
        SPLAPPTRC(L_DEBUG, "Exception received during submission: " << e.what(), SPL_OPER_DBG);
        handleSubmitException();
        throw e;
    } catch (...) {
        SPLAPPTRC(L_DEBUG, "Unknown exception received during submission: ", SPL_OPER_DBG);
        handleSubmitException();
        throw;
    }
}

void OperatorImpl::tryCatchSubmit(Punctuation const& punct, uint32_t port)
{
    try {
        submitCommon(punct, port);
    } catch (Distillery::DistilleryException const& e) {
        SPLAPPTRC(L_DEBUG, "Exception received during submission: " << e, SPL_OPER_DBG);
        handleSubmitException();
        throw e;
    } catch (std::exception const& e) {
        SPLAPPTRC(L_DEBUG, "Exception received during submission: " << e.what(), SPL_OPER_DBG);
        handleSubmitException();
        throw e;
    } catch (...) {
        SPLAPPTRC(L_DEBUG, "Unknown exception received during submission: ", SPL_OPER_DBG);
        handleSubmitException();
        throw;
    }
}

void OperatorImpl::tryCatchForwardWindowPunctuation(Punctuation const& punct)
{
    try {
        forwardWindowPunctuationCommon(punct);
    } catch (Distillery::DistilleryException const& e) {
        APPTRC(L_DEBUG, "Exception received during submission: " << e, SPL_OPER_DBG);
        handleSubmitException();
        throw e;
    } catch (std::exception const& e) {
        APPTRC(L_DEBUG, "Exception received during submission: " << e.what(), SPL_OPER_DBG);
        handleSubmitException();
        throw e;
    } catch (...) {
        APPTRC(L_DEBUG, "Unknown exception received during submission: ", SPL_OPER_DBG);
        handleSubmitException();
        throw;
    }
}

void OperatorImpl::submit(Punctuation const& punct)
{
    if (mustRunTryCatchPath_) {
        tryCatchSubmit(punct);
    } else {
        doSubmit(punct);
    }
}

void OperatorImpl::tryCatchSubmit(Punctuation const& punct)
{
    try {
        doSubmit(punct);
    } catch (Distillery::DistilleryException const& e) {
        APPTRC(L_DEBUG, "Exception received during submission: " << e, SPL_OPER_DBG);
        handleSubmitException();
        throw e;
    } catch (std::exception const& e) {
        APPTRC(L_DEBUG, "Exception received during submission: " << e.what(), SPL_OPER_DBG);
        handleSubmitException();
        throw e;
    } catch (...) {
        APPTRC(L_DEBUG, "Unknown exception received during submission: ", SPL_OPER_DBG);
        handleSubmitException();
        throw;
    }
}

void OperatorImpl::setTagData(const std::string& tagName,
                              const std::map<std::string, std::string>& tagValues)
{
    pe_.setTagData(tagName, tagValues);
}

void OperatorImpl::notifyWindowsOnWatermark(Watermark const& wm)
{
    Punctuation punct(Punctuation::WatermarkMarker);

    Distillery::AutoMutex am(windowMutex_);
    for (WindowRegistry::iterator registryIter = windows_.begin(); registryIter != windows_.end();
         registryIter++) {

        std::vector<BaseWindow*>& windowVector = registryIter->second;
        for (std::vector<BaseWindow*>::iterator windowIter = windowVector.begin();
             windowIter != windowVector.end(); ++windowIter) {
            WatermarkConsumer* consumer = dynamic_cast<WatermarkConsumer*>(*windowIter);
            if (consumer) {
                // Setting the punct watermark is expensive, do it lazily
                if (punct.getPayloadContainer() == NULL) {
                    EventTimeContext::impl::setWatermark(punct, wm);
                }
                consumer->insert(punct);
            }
        }
    }
}

namespace SPL {
/// \brief Class representing one parameter value at runtime
class ParameterValueImpl : public ParameterValue
{
  public:
    /// Construct a ParameterValue representing a non-'Expression' or 'Attribute'
    /// @param value Value representing the runtime parameter value
    ParameterValueImpl(ConstValueHandle const& value)
      : _isSimpleValue(true)
      , _value(value)
    {}

    /// Construct a ParameterValue representing an 'Expression' or 'Attribute'
    ParameterValueImpl()
      : _isSimpleValue(false)
    {}

    /// Is this parameter value a concrete value representable by a ConstValueHandle?
    /// This corresponds to a parameter with an expressionMode kind that is not 'Expression' or
    /// 'Attribute'
    /// @return 'true' if the parameter expressionMode is not 'Expression' or 'Attribute', and is
    ///  representable as a ConstValueHandle at runtime.
    virtual bool isSimpleValue() { return _isSimpleValue; }

    /// Is this parameter value only representable as an expression?
    /// This corresponds to a parameter with an expressionMode kind that is 'Expression or
    /// 'Attribute''
    /// @return 'true' if the parameter expressionMode is 'Expression' or 'Attribute'
    virtual bool isExpressionValue() { return !_isSimpleValue; }

    /// Return the concrete value for the parameter
    /// @pre isSimpleValue() == true
    /// @return the ConstValueHandle representing the parameter value
    virtual ConstValueHandle& getValue();

  private:
    bool _isSimpleValue;
    ConstValueHandle _value;
    virtual ~ParameterValueImpl() {}
};

ConstValueHandle& ParameterValueImpl::getValue()
{
    if (!_isSimpleValue) {
        SPLTRACEMSGANDTHROW(SPLRuntimeInvalidOperation, L_ERROR, SPL_RUNTIME_PARAMETER_NOT_VALUE,
                            SPL_OPER_DBG);
    }
    return _value;
}

ParameterValue& ParameterValue::create()
{
    return *new ParameterValueImpl;
}

ParameterValue& ParameterValue::create(ConstValueHandle const& value)
{
    return *new ParameterValueImpl(value);
}
}
