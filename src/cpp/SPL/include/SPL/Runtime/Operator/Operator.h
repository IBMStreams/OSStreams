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

#ifndef SPL_RUNTIME_OPERATOR_OPERATOR_H
#define SPL_RUNTIME_OPERATOR_OPERATOR_H

/*!
 * \namespace SPL \brief C++ classes for writing operators
 */

// Descriptions of our namespaces

// Main page
/*!
 * \mainpage SPL Operator Runtime C++ API Documentation
 *
 * \section intro_sec Introduction
 *
 * This is the %SPL Runtime C++ API documentation. These APIs are used to
 * develop %SPL primitive operators in C++ and native functions in C++.
 *
 * \section oper_sec SPL Operator APIs
 *
 * %SPL operator APIs are covered under the following namespaces:
 *     - SPL: Covers type classes (such as \link SPL::list list\endlink) and
 *       operator classes (such as \link SPL::Operator Operator\endlink)
 *     - SPL::Meta: Covers meta-type classes (such as \link SPL::Meta::ListType
 *       ListType\endlink)
 *
 * These classes are targeted at operator development. Type related classes are
 * used to manipulate tuples, whereas operator related classes are used to
 * interact with the %SPL runtime. Meta type classes are useful for runtime
 * introspection of tuples and %SPL types in general.
 *
 * \subsection types_subsec SPL Type System in C++
 *
 * All %SPL types have their corresponding types in C++. The C++ types extend
 * the C++ standard library types when the appropriate types are present in the
 * standard library. An example is SPL::list, which extends std::vector. C++
 * types that represent %SPL composite types use multiple inheritance and extend
 * from a base class that provides support for reflective manipulation. For
 * instance, SPL::list not only extends from the std::vector class, but also
 * from the SPL::List class. The latter provides the ability to manipulate the
 * contents of the list without compile-time knowledge of the contained type.
 *
 * \section funct_sec SPL Native Function APIs
 *
 * %SPL native function APIs are covered under the SPL::Functions namespace,
 * and contain the following sub-namespaces:
 *     - SPL::Functions::Collections: Covers collection functions (such as \link
 *       SPL::Functions::Collections::insertM insertM\endlink)
 *     - SPL::Functions::EventTime: Covers native functions that deal with
 *       event-time operations (such as
 *       \link SPL::Functions::EventTime::getEventTime getEventTime\endlink)
 *     - SPL::Functions::File: Covers file functions (such as \link
 *       SPL::Functions::File::fopen fopen\endlink)
 *     - SPL::Functions::Math: Covers math functions (such as \link
 *       SPL::Functions::Math::exp exp\endlink)
 *     - SPL::Functions::String: Covers string functions (such as \link
 *       SPL::Functions::String::trim trim\endlink)
 *     - SPL::Functions::Time: Covers time functions (such as \link
 *       SPL::Functions::Time::getTimestamp getTimestamp\endlink)
 *     - SPL::Functions::Utility: Covers utility functions (such as \link
 *       SPL::Functions::Utility::getEnvironmentVariable getEnvironmentVariable\endlink)
 *
 * These functions provide the C++ counterparts of the native functions present
 * in the %SPL standard toolkit. They are most commonly used for implementation
 * of new native functions that make use of the ones present in the %SPL
 * standard toolkit. They can also be used to implement C++ operators.
 */

/*!
 * \file Operator.h \brief Definition of the SPL::Operator class.
 */

#include <SPL/Runtime/ProcessingElement/ProcessingElement.h>
#ifndef DOXYGEN_SKIP_FOR_USERS
namespace SPL
{
    namespace JNI
    {
        class DLL_PUBLIC OpSetupInvoker;
        class DLL_PUBLIC JNIBridgeInvoker;
    }
}

#include <SPL/Runtime/Utility/Visibility.h>
#include <SPL/Runtime/Operator/OperatorForwardDeclarations.h>
#include <inttypes.h>
#include <string>
#include <vector>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

namespace SPL
{
    /*! \brief Class that serves as the base class of all %SPL primitive
     *  operators.
     *
     *  Primitive C++ operators extend this class to implement their processing
     *  logic.
     *
     *  The life-time of the operator starts with a call to its constructor. It
     *  is common for an operator to perform initialization tasks in the
     *  constructor.
     *
     *  Next, the \link SPL::Operator::allPortsReady allPortsReady\endlink
     *  function is called by the runtime to notify the operator that all PE
     *  ports are ready and it is safe to submit tuples. This function is often
     *  used to by the operator to create threads (either using the \link
     *  SPL::Operator::createThreads createThreads\endlink function or by other
     *  means) that will perform asynchronous work or in the case of source
     *  operators, do the main work of the operator.
     *
     *  Once all PE ports are ready, the tuple and punctuation process functions
     *  (\link SPL::Operator::process(Tuple&,uint32_t) process(Tuple)\endlink
     *  and \link SPL::Operator::process(Punctuation const&,uint32_t)
     *  process(Punctuation)\endlink) of the operator are called by the runtime
     *  to drive the execution. These functions may run concurrently and thus
     *  proper synchronization should be performed (see \link SPL::AutoPortMutex
     *  AutoPortMutex\endlink) by the operator developer. It is safe to submit
     *  tuples from within the context of the tuple and punctuation processing
     *  functions, and there is no reason to wait for the \link
     *  SPL::Operator::allPortsReady allPortsReady\endlink notification to do
     *  so.
     *
     *  The \link SPL::Operator::prepareToShutdown prepareToShutdown\endlink
     *  function is called by the runtime to notify the operator about the
     *  pending termination of the PE. Commonly, this function is used by the
     *  operator to make a note of the termination, so that any asynchronous
     *  processing performed by the operator is notified properly of the pending
     *  shutdown. The tuple and punctuation processing functions can execute
     *  concurrently with the \link SPL::Operator::prepareToShutdown
     *  prepareToShutdown\endlink function. See also: \link
     *  SPL::ProcessingElement::getShutdownRequested
     *  ProcessingElement::getShutdownRequested\endlink.
     *
     *  Finally, the destructor is called to finalize the operator. The
     *  destructor is commonly used by the operator to perform resource cleanup
     *  tasks. It is guranteed that no tuple or punctuation processing functions
     *  are active when the destructor is running.
     *
     *  The common flow of the operator life-cycle is given as follows:
     *
     *   \code
     *   Control path       Data path (async.)
     *   ============================================
     *    Constructor
     *         |                   \
     *    allPortsReady      process(Tuple,..)
     *         |                   and
     *    prepareToShutdown  process(Punctuation,...)
     *         |                   /
     *    Destructor
     *  \endcode
     *
     *  If an operator is configured to checkpoint in its SPL invocation, then
     *  the \link SPL::Operator::restoreCheckpoint restoreCheckpoint\endlink
     *  function is called after the constructor in case the operator is
     *  restarted and there is a checkpoint to restore from. Also, when
     *  checkpointing is configured, \link SPL::Operator::getCheckpoint
     *  getCheckpoint\endlink function is called by the runtime to
     *  asynchronously checkpoint the operator.
     */
    class DLL_PUBLIC Operator
    {
    public:
        /// Constructor
        ///
        Operator();

        /// Destructor
        ///
        virtual ~Operator();

        /// Get the index of the operator within the PE
        /// @return index of the operator within the PE
        uint32_t getIndex() const;

        /// Get the processing element hosting this operator
        /// @return processing element hosting this operator
        ProcessingElement & getPE();

        /// Get the context for this operator
        /// @return a reference to the context object for this operator
        OperatorContext & getContext() const;

        /// Get the number of inputs for this operator
        /// @return the number of inputs for this operator
        uint32_t getNumberOfInputPorts() const;

        /// Get the number of outputs for this operator
        /// @return the number of outputs for this operator
        uint32_t getNumberOfOutputPorts() const;

        /// Get the operator input port
        /// @param port input port index
        /// @return operator input port at the given index
        /// @throw SPLRuntimeInvalidIndexException when the index is out of bounds
        OperatorInputPort & getInputPortAt(uint32_t port);

        /// Get the operator output port
        /// @param port output port index
        /// @return operator output port at the given index
        /// @throw SPLRuntimeInvalidIndexException when the index is out of bounds
        OperatorOutputPort & getOutputPortAt(uint32_t port);

        /// This function is called by the runtime when the operator receives a
        /// tuple and the port on which the tuple is received is marked as
        /// mutating in the operator model
        /// @param tuple tuple value
        /// @param port index for the input port, from which the tuple is received
        virtual void process(Tuple & tuple, uint32_t port) {}

        /// This function is called by the runtime when the operator receives a
        /// tuple and the port on which the tuple is received is not marked as
        /// mutating in the operator model
        /// @param tuple tuple value
        /// @param port index for the input port, from which the tuple is received
        virtual void process(Tuple const & tuple, uint32_t port) {}

        /// This function is called by the runtime when the operator receives a
        /// punctuation
        /// @param punct punctuation value
        /// @param port index of the input port, from which the punctuation is
        /// received
        virtual void process(Punctuation const & punct, uint32_t port) {}

        /// This function is called by the runtime once and only once for each
        /// thread created, and runs the thread's main logic
        /// @param idx thread index (starts from 0)
        virtual void process(uint32_t idx) {}

        /// Submit a tuple to a specified port
        /// @param tuple tuple value
        /// @param port port on which the tuple will be submitted
        /// @throws SPLRuntimeTypeMismatchException if in non-optimized mode and
        /// the tuple has incorrect type
        /// @throws SPLRuntimeInvalidIndexException if the port is out of the
        /// valid range
        void submit(Tuple & tuple, uint32_t port);

        /// Submit a tuple to a specified port
        /// @param tuple tuple value
        /// @param port port on which the tuple will be submitted
        /// @throws SPLRuntimeTypeMismatchException if the port is mutable
        /// @throws SPLRuntimeTypeMismatchException if in non-optimized mode and
        /// the tuple has incorrect type
        /// @throws SPLRuntimeInvalidIndexException if the port is out of the
        /// valid range
        void submit(Tuple const & tuple, uint32_t port);

        /// Submit a punctuation to a specified port
        /// @param punct punctuation value
        /// @param port port on which the punctuation will be submitted
        /// @throws SPLRuntimeInvalidIndexException if the port is out of the
        /// valid range
        void submit(Punctuation const & punct, uint32_t port);

        /// Forward a Window punctuation to all output ports
        /// @param punct punctuation to forward
        void forwardWindowPunctuation(Punctuation const & punct);

        /// This function is called by the runtime as a notification to the
        /// operator that its ports are ready to receive and submit tuples.
        virtual void allPortsReady() {}

        /// Create threads (most commonly used from within \c allPortsReady
        /// function)
        /// @param numThreads number of threads to create
        /// @return the index of the first thread created
        uint32_t createThreads(uint32_t numThreads);

        /// Access a specific thread
        /// @param index thread index
        OperatorThread & getThread(uint32_t index);

        /// This function is called by the runtime when the operator should start
        /// preparing for shutdown
        virtual void prepareToShutdown() {}

        /// This function is called by the runtime when the operator state is to
        /// be checkpointed. The implementer should use the \a opstate parameter
        /// to populate the data that is to be checkpointed.
        /// @param opstate serialization buffer representing the operator state
        /// (out-parameter)
        /// @deprecated
        virtual void getCheckpoint(NetworkByteBuffer & opstate) {}

        /// This function is called by the runtime when the operator state is
        /// to be restored
        /// @param opstate serialization buffer containing the operator state
        /// @deprecated
        virtual void restoreCheckpoint(NetworkByteBuffer & opstate) {}

        /// Force a synchronous checkpoint
        /// @param opstate serialization buffer containing the operator state
        /// @note performSynchronousCheckpoint will not checkpoint an operator
        /// that has not been configured to checkpoint in its %SPL invocation
        /// @deprecated
        void performSynchronousCheckpoint(NetworkByteBuffer const & opstate);

    protected:
#ifndef DOXYGEN_SKIP_FOR_USERS

        /// This function is called internally
        /// @param tuple tuple value
        /// @param port index for the input port, from which the tuple is received
        virtual void processRaw(Tuple & tuple, uint32_t port) {}

        /// This function is called internally
        /// @param tuple tuple value
        /// @param port index for the input port, from which the tuple is received
        virtual void processRaw(Tuple const & tuple, uint32_t port) {}

        /// This function is called when the operator received a punctuation
        /// @param punct punctuation value
        /// @param port index for the input port, from which the punctuation is received
        virtual void processRaw(Punctuation const & punct, uint32_t port) {}

        typedef Operator * (*Instantiator) ();
        static Operator * instanceOf(std::string const & name);
        static std::tr1::unordered_map<std::string, Instantiator> instantiators_;
#endif /* DOXYGEN_SKIP_FOR_USERS */

    public:
        /// Get the set of all parameter names
        /// @return all the parameter names for an operator instance
        const std::tr1::unordered_set<std::string>& getParameterNames() const;

        /// A vector of pointers to parameter values.
        typedef std::vector<ParameterValue*> ParameterValueListType;

        /// The map from parameter names to lists of parameter values.
        typedef std::tr1::unordered_map<std::string, ParameterValueListType> ParameterMapType;

        /// Get the list of parameter values for a given parameter name
        /// @param param parameter name
        /// @return the parameter values
        /// @throws SPLRuntimeInvalidArgumentException if param is not found
        const ParameterValueListType& getParameterValues(std::string const & param) const;

        /// Get the parameter map of parameter names to parameter values
        /// @return the map from parameter names to lists of parameter values.
        virtual ParameterMapType& getParameters() = 0;

        /// This function is automatically generated by the %SPL compiler, in order
        /// to allow checkpointing of %SPL logic state variables.  When called,
        /// this routine will save the current values of the logic state variables
        /// into \a opstate. This function should be called from getCheckpoint to
        /// save any state variables.
        /// @param opstate serialization buffer representing the operator state
        /// (out-parameter)
        virtual void checkpointStateVariables(NetworkByteBuffer & opstate) const = 0;

        /// This function is automatically generated by the %SPL compiler, in order
        /// to allow restoration of checkpointed %SPL logic state variables.  When called,
        /// this routine will restore the the logic state variables from \a opstate.
        /// This function should be called from restoreCheckpoint to restore any state variables.
        /// @param opstate serialization buffer representing the operator state
        virtual void restoreStateVariables(NetworkByteBuffer & opstate) = 0;

        /// Returns the runtime path of the root directory of the toolkit containing the operator
        /// code making this call.
        /// @return Returns the root toolkit directory
        std::string getToolkitDirectory() const;

#ifndef DOXYGEN_SKIP_FOR_USERS

        /// Check if operator's input ports are called by multiple threads
        /// @return 'true' if there if there are multiple threads calling the input ports
        bool isMultiThreadedOnInputs() const { return isMultiThreadedOnInputs_; }

        /// Returns the runtime path of the root directory of the toolkit with the
        /// specified name
        /// @param toolkitName
        /// @return Returns the root toolkit directory
        std::string getToolkitDirectory(const std::string & toolkitName) const;

        /// Get the index of the toolkit in which this operator lives
        /// @return Returns the toolkit index
        uint32_t getToolkitIndex() const;

#endif /*DOXYGEN_SKIP_FOR_USERS*/

    private:
        OperatorImpl * impl_;
        OperatorImpl & getOperatorImpl() { return *impl_; }
        friend class JNI::OpSetupInvoker;
        friend class JNI::JNIBridgeInvoker;
        friend class PEImpl;
        template <typename T>
        friend class OpInputPortDelegate;
#ifndef DOXYGEN_SKIP_FOR_USERS
        // Valid for 3.0 and up only
        bool isMultiThreadedOnInputs_;
        bool dummy1_[sizeof(void *) - sizeof (bool)];
        void *dummy2_;

    protected:
        /// This function is called internally
        /// @param buffer buffer value
        /// @param port index for the input port, from which the tuple is received
        virtual void processRaw(NativeByteBuffer & buffer, uint32_t port) {}

    public:
        /// This function is called when we do not want tuple serialization/de-serialization
        virtual bool sendRawBufferData() { return false; }

        /// Submit a tuple as byte buffer to a specified port
         /// @param buffer tuple value
         /// @param port port on which the tuple will be submitted
         /// @throws SPLRuntimeInvalidIndexException if the port is out of the
         /// valid range
        void submit(NativeByteBuffer & buffer, uint32_t port);

        /// Submit a punctuation to a specified port. This method is called only
        /// by auto-generated and it is used to differentiate between explicit
        /// user-code submission and SPL runtime auto-forwarding submission.
        /// @param punct punctuation value
        /// @param port port on which the punctuation will be submitted
        /// @throws SPLRuntimeInvalidIndexException if the port is out of the
        /// valid range
        void autoSubmit(Punctuation const & punct, uint32_t port);

        /// This function is automatically generated by the %SPL compiler
        /// to reset an operator to its initial state
        virtual void resetToInitialStateRaw() {};

        /// This function is automatically generated by the %SPL compiler
        /// to checkpoint the operator state
        /// @param ckpt reference to created checkpoint
        virtual void checkpointRaw(Checkpoint & ckpt) {};

        /// This function is automatically generated by the %SPL compiler
        /// to reset the operator state from a checkpoint
        /// @param ckpt reference to the restored checkpoint
        virtual void resetRaw(Checkpoint & ckpt) {};

        /// This function indicates if the operator has logic state variables
        /// @return true if it has state variables, false otherwise
        virtual bool hasStateVariables() const = 0;

        class TagNames {
        public:
            static const char* OperatorIGC;
        };

        /// This function sets tag data for the operator.  If this value is
        /// \link SPL::Operator::TagNames::OperatorIGC TagNames.OperatorIGC \endlink,
        /// the tags will be registered
        /// in the IG catalog.  Other tag names may be defined in future
        /// releases.
        void setTagData(const std::string& tagName, const std::map<std::string, std::string>& tagValues);

        /// This function requests the operator to block its consistent region
        /// permit. Currently, this call is implemented by Java operators only.
        /// @since Streams&reg; Version 4.2.0
        virtual void blockConsistentRegionPermit() {};

        /// Gets the current watermark for the operator.
        /// @since IBM Streams&reg; Version 4.3.0
        virtual timestamp watermark() const;

        /// This function requests the operator to unblock its consistent region
        /// permit. Currently, this call is implemented by Java operators only.
        virtual void releaseConsistentRegionPermit() {};

#endif /*DOXYGEN_SKIP_FOR_USERS*/
    };
};

#endif /* SPL_RUNTIME_OPERATOR_OPERATOR_H */
