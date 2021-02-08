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

#ifndef SPL_CORE_OPERATOR_INVOCATION_H
#define SPL_CORE_OPERATOR_INVOCATION_H

#include <SPL/CodeGen/Config.h>
#include <SPL/CodeGen/Param.h>
#include <SPL/Utility/ModelDumper.h>
#include <UTILS/HashMapHelpers.h>
#include <assert.h>
#include <string>
#include <vector>

namespace SPL {
class RootTyp;
class TupleTyp;
class SourceLocation;
class Expression;
class FunctionHeadSym;
class BinaryExpression;
class Statement;
class Literal;
class CompositeOperatorInstance;
class SPLAnnotation;
class SPLParallelAnnotation;
class OperatorGraphNode;

/// Hold Operators at compile time
/// Operator IR classes
/// This will ALWAYS be a tree (not a DAG), and we therefore use
/// the following rules to handle memory
/// 1 - All allocation of Operator classes shall be done with 'new'
/// 2 - A statement (when deleted) will delete all sub statements

class PrimitiveOperatorInstance
{
  public:
    class Logic;
    class Window;
    class Output;
    class Port;

    /// Construction
    /// @param location         Source Location
    /// @param endLocation      End Source Location
    /// @param name             Outside in name (X.Y.B)
    /// @param kind             Operator Kind (spl.relational::Functor)
    /// @param compOperInstance Which composite operator instance is this contained in?
    /// @param toolkitIndex     Which toolkit is this from?
    PrimitiveOperatorInstance(const SourceLocation& location,
                              const SourceLocation& endLocation,
                              const std::string& name,
                              const std::string& kind,
                              const CompositeOperatorInstance* compOperInstance,
                              uint32_t toolkitIndex);

    /// Destructor
    ~PrimitiveOperatorInstance();

    /// Add another name for the operator
    /// @param name a name for the operator
    void addName(const std::string& name);

    /// Return Operator source location
    /// @return the coordinates for the Operator
    const SourceLocation& getLocation() const { return _location; }

    /// Return Operator end source location
    /// @return the last coordinates for the Operator
    const SourceLocation& getEndLocation() const { return _endLocation; }

    /// Return Operator names
    /// @return the names of the Operator (the output streams)
    const std::vector<std::string>& getNames() const { return _names; }

    /// Return Operator kind
    /// @return the kind of the Operator
    const std::string& getKind() const { return _kind; }

    /// Return composite operator instance.
    /// @return the composite operator instance of this operator invocation
    const CompositeOperatorInstance* getCompOperInstance() const { return _compOperInstance; }

    /// Return the index of the toolkit that defined this operator
    /// @return the toolkit index
    uint32_t getToolkitIndex() const { return _toolkitIndex; }

    /// Add an input port for an operator
    /// @param p              Input Port
    /// @post                 PrimitiveOperatorInstance owns the memory
    void addInputPort(Port& p) { _iports.push_back(&p); }

    /// Get an input port for an operator
    /// @param portNum        Input port number
    /// @return port          Input Port
    Port& getInputPort(uint32_t portNum) const { return *_iports[portNum]; }

    /// Return the number of input ports
    /// @return count of input ports
    uint32_t numInputPorts() const { return _iports.size(); }

    /// Add an output port for an operator
    /// @param p              Output Port
    /// @post                 PrimitiveOperatorInstance owns the memory
    void addOutputPort(Port& p)
    {
        _oports.push_back(&p);
        _outputs.push_back(Output());
    }

    /// Get an output port for an operator
    /// @param portNum        Output port number
    /// @return port          Output Port
    const Port& getOutputPort(uint32_t portNum) const { return *_oports[portNum]; }

    /// Return the number of output ports
    /// @return count of output ports
    uint32_t numOutputPorts() const { return _oports.size(); }

    /// Get the logic for an Operator
    /// @return logic for the operator
    const Logic& getLogic() const { return _logic; }

    /// Get the logic for an Operator
    /// @return logic for the operator
    Logic& getLogic() { return _logic; }

    /// Add a window
    /// @param w            Window type
    /// @post               PrimitiveOperatorInstance owns the memory
    void addWindow(Window& w);

    /// Access the Window Information
    /// @param port       Input port
    /// @return Window information for that port or NULL if no windowing on that port
    const Window* getWindow(uint32_t port) const;

    /// Add a parameter for an Operator
    /// @param param         Identifier
    void addParam(Param& param)
    {
        _params.push_back(&param);
        MODEL("AddParam(" << (void*)this << ", " << (void*)&param << ")\n");
    }

    /// Return all the parameters for an operator
    /// @return parameters
    const std::vector<Param*>& getParams() const { return _params; }

    /// Access the Output Information for an output port
    /// @param port Port Number
    Output& getOutput(uint32_t port) { return _outputs[port]; }

    /// Access the Output Information for an output port
    /// @param port Port Number
    const Output& getOutput(uint32_t port) const { return _outputs[port]; }

    /// Add a configuration option to the operator
    /// @param config  configuration option
    void addConfig(Config& config) { _configs.push_back(&config); }

    /// Access the Configs for the operator
    /// @return list of configs
    const std::vector<Config*>& getConfigs() const { return _configs; }

    /// Find the first matching config, if present
    /// @param kind   Config kind we are looking for
    /// @return matching Config or NULL
    const Config* findConfig(Config::Kind kind) const;

    /// Find the first matching placement config with the right subKind, if present
    /// @param subKind   PlacementConstraint config subKind we are looking for
    /// @return matching Config or NULL
    const Config* findConfig(PlacementConstraint::PlacementSubKind subKind) const;

    /// Is this output port viewable?
    /// @param port output port number
    /// @return 'true' if the output port is viewable
    bool outputPortViewable(uint32_t port) const;

    /// Is this operator restartable
    /// @returns true if the operator is restartable, otherwise false
    bool isRestartable() const;

    /// Is this operator relocatable
    /// @returns true if the operator is relocatable, otherwise false
    bool isRelocatable() const;

    /// Set spldoc comment for the invocation
    /// @param comment @splDoc comment for this invocation
    /// @param congestionPolicy congestion policy on queue full
    void setSplDocComment(const std::string& comment) { _splDocComment = comment; }

    /// Return the splDocComment for this invocation
    /// @return saved comment
    const std::string& getSplDocComment() const { return _splDocComment; }

    /// Print an Operator
    /// @param  s stream to print to
    /// @return s
    std::ostream& print(std::ostream& s) const;

    /// Add an SPL annotation to this primitive
    /// @param annotation The annotation
    void addAnnotation(const SPLAnnotation& annotation) const;

    /// Access the set of annatations associated with this composite
    /// @return Returns the annotations
    const std::vector<const SPLAnnotation*>& annotations() const { return _annotations; }

    /// Collect Type and Function Information
    /// @param types   if non-NULL, named types will be added
    /// @param fcns    if non-NULL, referenced functions will be added
    /// @param inFcns  if true, walk into references functions
    void collectTypeAndFunctions(std::tr1::unordered_set<const RootTyp*>* types,
                                 std::tr1::unordered_set<const FunctionHeadSym*>* fcns,
                                 bool inFcns) const;

    /// Determine if this primitive operator represents a parallel region
    /// @return Returns true if this primitive is a parallel region, otherwise false;
    bool isParallelRegion() const { return NULL != _parallelAnnotation; }

    const SPLParallelAnnotation& parallelAnnotation() const
    {
        assert(NULL != _parallelAnnotation);
        return *_parallelAnnotation;
    }

    std::string parallelRegionName() const;

    /// Is this operator a parallel region, or is it within a parallel region
    bool isInParallelRegion() const;

    /// Get the level of nesting in UDP regions. If the operator is
    /// not in a UDP region it returns 0.
    uint32_t parallelNestingLevel() const;

    /// Get the level of nesting in UDP regions. If the operator is
    /// not in a UDP region it returns 0.
    /// @param introducingComp The composite instance where a colocation config using byChannel or
    /// byReplica was introduced
    uint32_t parallelNestingLevel(const CompositeOperatorInstance& introducingComp) const;

    /// Get the parallel annotation for this operator, or its containing region
    const SPLParallelAnnotation& getParallelAnnotation() const;

    /// Get the OperatorGraphNode that represents this operator
    const OperatorGraphNode& getGraphNode() const
    {
        assert(NULL != _graphNode);
        return *_graphNode;
    }

    /// Set the OperatorGraphNode that represents this operator
    void setGraphNode(OperatorGraphNode& graphNode)
    {
        assert(NULL == _graphNode);
        _graphNode = &graphNode;
    }

    //! Class to represent one port
    class Port
    {
      public:
        /// Copy of ActiveQueue version
        enum CongestionPolicy
        {
            Wait,
            DropFirst,
            DropLast
        };

        /// Constructor
        /// @param tuple     tuple type
        /// @param loc       source location
        Port(const TupleTyp* tuple, const SourceLocation& loc)
          : _loc(loc)
          , _tuple(tuple)
          , _isQueue(false)
          , _submitCalled(false)
          , _mutationAllowed(true)
          , _queueSize(0)
        {
            MODEL("Port(" << (void*)this << ")\n");
        }

        /// Add another stream to the port
        /// @param name fully qualified stream name
        void addStream(const std::string& name)
        {
            _streams.push_back(name);
            MODEL("AddStream(" << (void*)this << ", " << name << ")\n");
        }

        /// Add another actual stream to the port
        /// @param name local stream name in the invoking scope
        void addActualStream(const std::string& name)
        {
            _actualStreams.push_back(name);
            MODEL("AddActualStream(" << (void*)this << ", " << name << ")\n");
        }

        /// Return the source location of this port
        /// @return location
        const SourceLocation& getLocation() const { return _loc; }

        /// Return the number of streams in this port
        /// @return number of streams (always 1 for output ports)
        uint32_t getNumStreams() const { return _streams.size(); }

        /// Return the name of the stream in the port
        /// @param streamNum Stream Number
        /// @return stream name (the fully qualified stream name)
        const std::string& getStreamName(uint32_t streamNum = 0) const
        {
            return _streams[streamNum];
        }

        /// Return the number of 'actual' streams in this port
        /// @return number of streams (always 1 for output ports)
        uint32_t getNumActualStreams() const { return _actualStreams.size(); }

        /// Return the name of the actual stream in the port
        /// @param streamNum Stream Number
        /// @return stream name (local stream name in the invoking scope)
        const std::string& getActualStreamName(uint32_t streamNum = 0) const
        {
            return _actualStreams[streamNum];
        }

        /// Return the type of the tuples on the port
        /// @return tuple type
        const TupleTyp* getTuple() const { return _tuple; }

        /// Set the alias name (short name) for a port
        /// @param alias another alias for the port
        void setAlias(const std::string& alias) { _alias = alias; }

        /// Check if an alias exists for the port
        /// @return 'true' if there is an alias for the port
        bool hasAlias() const { return !_alias.empty(); }

        /// Get the alias name (short name) for a port
        /// @return alias for the port
        const std::string& getAlias() const { return _alias; }

        /// Set the port name. This is the local alias if specified, or the
        /// local name of the first stream attached to the port. It is an
        /// unqualified name in both cases.
        /// @param name port name
        void setName(const std::string& name) { _name = name; }

        /// Get the port name. This is the local alias if specified, or the
        /// local name of the first stream attached to the port. It is an
        /// unqualified name in both cases.
        /// @return port name
        const std::string& getName() const { return _name; }

        /// Set 'queue' flag - Only for input ports
        /// @param val 'true' if the input port is a threaded queue
        /// @param congestionPolicy congestion policy on queue full
        void setIsQueue(bool val, CongestionPolicy congestionPolicy)
        {
            _isQueue = val;
            _congestionPolicy = congestionPolicy;
        }

        /// Get 'queue' flag - Only for input ports
        /// @return 'true' if the input port is a threaded queue
        bool getIsQueue() const { return _isQueue; }

        /// Set 'queue' size - Only for input ports; also sets 'queue' flag
        /// @param val number of elements in threaded input queue
        /// @param congestionPolicy congestion policy on queue full
        void setQueueProperties(uint32_t val, CongestionPolicy congestionPolicy)
        {
            _queueSize = val;
            _isQueue = _queueSize != 0;
            _congestionPolicy = congestionPolicy;
        }

        /// Get 'queue' size - Only for input ports
        /// @return number of elements in threaded input queue
        uint32_t getQueueSize() const
        {
            assert(_isQueue);
            return _queueSize;
        }

        /// Get 'drop mode' congestion policy - Only for input ports
        /// @return congestion policy
        CongestionPolicy getCongestionPolicy() const
        {
            assert(_isQueue);
            return _congestionPolicy;
        }

        /// Print Port
        /// @param  s stream to print to
        /// @return s
        std::ostream& print(std::ostream& s) const;

        /// Collect Type and Function Information
        /// @param types   if non-NULL, named types will be added
        /// @param fcns    if non-NULL, referenced functions will be added
        /// @param inFcns  if true, walk into references functions
        void collectTypeAndFunctions(std::tr1::unordered_set<const RootTyp*>* types,
                                     std::tr1::unordered_set<const FunctionHeadSym*>* fcns,
                                     bool inFcns) const;

        /// Set the 'submit called' flag for this port
        /// @param value value
        void setSubmitCalled(bool value) { _submitCalled = value; }

        /// Return true if a submit of a tuple is done on logic attached to this port
        /// @return value of submit tuple done
        bool getSubmitCalled() const { return _submitCalled; }

        /// Set the 'mutation allowed' flag for this port
        /// @param value value
        void setMutationAllowed(bool value) { _mutationAllowed = value; }

        /// Return true if tuple mutation is allowed on this port
        /// @return value of mutation allowed flag
        bool getMutationAllowed() const { return _mutationAllowed; }

      private:
        std::string _name;
        std::vector<std::string> _streams;
        std::vector<std::string> _actualStreams;
        const SourceLocation& _loc;
        const TupleTyp* _tuple;
        std::string _alias;
        bool _isQueue;
        bool _submitCalled;
        bool _mutationAllowed;
        uint32_t _queueSize;
        CongestionPolicy _congestionPolicy;
    };

    //! Logic for an operator
    //! optional state (common)
    //! optional logic for each input port for tuples
    //! optional logic for each input port for punct
#if 0
        // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
        //! optional logic for each input port for watermarks
#endif
    class Logic
    {
      public:
        /// Constructor
        Logic()
        {
            _state = NULL;
            _onProcess = NULL;
        }

        /// Destructor: delete statements
        ~Logic();

        /// Is there any logic associated with this operator?
        /// @return 'state' statement
        bool hasLogic() const
        {
#if 0
                // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
                return _state || _onProcess || _portTupleLogic.size() > 0 ||
                    _portPunctLogic.size() > 0 || _portWatermarkLogic.size() > 0;
#else
            return _state || _onProcess || _portTupleLogic.size() > 0 || _portPunctLogic.size() > 0;
#endif
        }

        /// Set the state statement
        /// @param stmt statement for operator state
        void setState(Statement& stmt) { _state = &stmt; }

        /// Return the state statement (if present)
        /// @return 'state' statement
        Statement* getState() const { return _state; }

        /// Return the state statement (if present)
        /// @return 'state' statement
        Statement*& getState() { return _state; }

        /// Set the onProcess statement
        /// @param stmt statement for operator onProcess
        void setProcess(Statement& stmt) { _onProcess = &stmt; }

        /// Return the onProcess statement (if present)
        /// @return 'onProcess' statement
        Statement* getProcess() const { return _onProcess; }

        /// Return the onProcess statement (if present)
        /// @return 'onProcess' statement
        Statement*& getProcess() { return _onProcess; }

        /// Add a port/logic statement for incoming tuple
        /// @param portNum port index
        /// @param stmt statement for tuple input at that port
        /// @param port port if Custom operator or NULL
        void addTuplePort(uint32_t portNum, Statement& stmt, Port* port);

        /// Return the statement for a port for a tuple input
        /// @param port port number
        /// @return statement to be executed at tuple input on that port
        const Statement* getTuplePort(uint32_t port) const;

        /// Return the tuple logic for all the ports
        /// @return the whole list
        const std::vector<Statement*>& getTuplePorts() const { return _portTupleLogic; }
        std::vector<Statement*>& getTuplePorts() { return _portTupleLogic; }

        /// Add a port/logic statement for incoming punct
        /// @param portNum port index
        /// @param stmt statement for punct input at that port
        /// @param port port if Custom operator or NULL
        void addPunctPort(uint32_t portNum, Statement& stmt, Port* p);

        /// Return the statement for a port for a punct input
        /// @param port port number
        /// @return statement to be executed at punct input on that port
        const Statement* getPunctPort(uint32_t port) const;

        /// Return the punct logic for all the ports
        /// @return the whole list
        const std::vector<Statement*>& getPunctPorts() const { return _portPunctLogic; }
        std::vector<Statement*>& getPunctPorts() { return _portPunctLogic; }

#if 0
            // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
            /// Add a port/logic statement for incoming watermark
            /// @param portNum port index
            /// @param stmt statement for watermark input at that port
            /// @param port port if Custom operator or NULL
            void addWatermarkPort(uint32_t portNum, Statement & stmt, Port* p);

            /// Return the statement for a port for a watermark input
            /// @param port port number
            /// @return statement to be executed at watermark input on that port
            const Statement * getWatermarkPort(uint32_t port) const;

            /// Return the watermark logic for all the ports
            /// @return the whole list
            const std::vector<Statement *> & getWatermarkPorts() const
            {
                return _portWatermarkLogic;
            }
            std::vector<Statement *> & getWatermarkPorts()
            {
                return _portWatermarkLogic;
            }
#endif
        /// Print Stream
        /// @param  s stream to print to
        /// @return s
        std::ostream& print(std::ostream& s) const;

        /// Collect Type and Function Information
        /// @param types   if non-NULL, named types will be added
        /// @param fcns    if non-NULL, referenced functions will be added
        /// @param inFcns  if true, walk into references functions
        void collectTypeAndFunctions(std::tr1::unordered_set<const RootTyp*>* types,
                                     std::tr1::unordered_set<const FunctionHeadSym*>* fcns,
                                     bool inFcns) const;

      private:
        Statement* _state;
        Statement* _onProcess;
        std::vector<Statement*> _portTupleLogic;
        std::vector<Statement*> _portPunctLogic;
#if 0
            // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
            std::vector<Statement *> _portWatermarkLogic;
#endif
    };

    /// A general list of expressions
    typedef std::vector<Expression*> ExpnList;
    typedef std::pair<std::string, ExpnList> IdWithExpressions;

    //! configuration parameters for the window on one input port
    //! This is a vector of ID [expns]+
    //! Examples are:  sliding              (no expn list)
    //!                delta (attrib, 5)    (expn list is attrib, 5)
    class Window
    {
      public:
        //! Types of Window
        enum WindowKind
        {
            Tumbling, //! Whole window moves at once
            Sliding,  //! window slides
            Buckets,  //! event-time driven window
        };

        //! Logic for windows
        enum LogicKind
        {
            Delta, //! attribute/value
            Count, //! number of tuples
            Time,  //! time since last change
            Punct, //! punctuation
        };

        //! Partition Eviction Policy
        enum PartitionEvictionPolicy
        {
            PartitionAge,   //! age of partition
            PartitionCount, //! number of partitions
            TupleCount      //! number of tuples
        };

        /// Constructor: Tumbling window
        /// @param loc Source Location
        /// @param name Port Name
        /// @param port_number The number of the port with which this window is associated
        /// @param ex_policy
        /// @param ex_id Attribute for Delta
        /// @param ex_value diff for Delta/ value to count/time
        Window(const SourceLocation& loc,
               const std::string& name,
               uint32_t port_number,
               LogicKind ex_policy,
               const Expression* ex_id = NULL,
               const Expression* ex_value = NULL)
          : _kind(Tumbling)
          , _loc(loc)
          , _name(name)
          , _port_number(port_number)
          , _ex_policy(ex_policy)
          , _ex_id(ex_id)
          , _ex_value(ex_value)
          , _tr_id(NULL)
          , _tr_value(NULL)
          , _partitioned(false)
          , _partitionEvictionValue(NULL)
          , _intervalDuration(NULL)
          , _creationPeriod(NULL)
          , _discardAge(NULL)
          , _intervalOffset(NULL)
        {}

        /// Constructor: Sliding window
        /// @param loc Source Location
        /// @param name Port Name
        /// @param port_number The number of the port with which this window is associated
        /// @param ex_policy
        /// @param tr_policy
        /// @param ex_id Attribute for Delta
        /// @param ex_value diff for Delta/ value to count/time
        /// @param tr_id Attribute for Delta
        /// @param tr_value diff for Delta/ value to count/time
        Window(const SourceLocation& loc,
               const std::string& name,
               uint32_t port_number,
               LogicKind ex_policy,
               LogicKind tr_policy,
               const Expression* ex_id = NULL,
               const Expression* ex_value = NULL,
               const Expression* tr_id = NULL,
               const Expression* tr_value = NULL)
          : _kind(Sliding)
          , _loc(loc)
          , _name(name)
          , _port_number(port_number)
          , _ex_policy(ex_policy)
          , _ex_id(ex_id)
          , _ex_value(ex_value)
          , _tr_policy(tr_policy)
          , _tr_id(tr_id)
          , _tr_value(tr_value)
          , _partitioned(false)
          , _partitionEvictionValue(NULL)
          , _intervalDuration(NULL)
          , _creationPeriod(NULL)
          , _discardAge(NULL)
          , _intervalOffset(NULL)
        {}

        /// Constructor: Event window
        Window(const SourceLocation& loc,
               const std::string& name,
               uint32_t port_number,
               const Expression* intervalDuration,
               const Expression* creationPeriod,
               const Expression* discardAge,
               const Expression* intervalOffset)
          : _kind(Buckets)
          , _loc(loc)
          , _name(name)
          , _port_number(port_number)
          , _ex_policy()
          , _ex_id()
          , _ex_value()
          , _tr_policy()
          , _tr_id()
          , _tr_value()
          , _partitioned(false)
          , _partitionEvictionValue()
          , _intervalDuration(intervalDuration)
          , _creationPeriod(creationPeriod)
          , _discardAge(discardAge)
          , _intervalOffset(intervalOffset)
        {}

        /// Copy Constructor
        Window(const Window& rhs);

        /// Destructor
        ~Window();

        /// Return the source location of this window directive
        /// @return location
        const SourceLocation& getLocation() const { return _loc; }

        /// What kind of window is this?
        /// @return Window Kind
        WindowKind getKind() const { return _kind; }

        /// What port does this window control?
        /// @return Port Name
        const std::string& getName() const { return _name; }

        /// What port does this window control?
        /// @return Port Number
        const uint32_t getPortNumber() const { return _port_number; }

        /// How does a window expire?
        /// @return eviction policy
        LogicKind getEvictionPolicy() const { return _ex_policy; }

        /// Eviction attribute (for Delta)
        /// @return attribute for eviction
        const Expression* getEvictionAttribute() const
        {
            assert(_ex_id);
            return _ex_id;
        }

        /// Eviction size (for non-Punct)
        /// @return size for eviction
        const Expression* getEvictionSize() const { return _ex_value; }

        /// How does a window trigger?
        /// @return trigger policy
        LogicKind getTriggerPolicy() const { return _tr_policy; }

        /// Trigger attribute (for Delta)
        /// @return attribute for triggering
        const Expression* getTriggerAttribute() const { return _tr_id; }

        /// Trigger size (for non-Punct)
        /// @return size for triggering
        const Expression* getTriggerSize() const { return _tr_value; }

        /// Is this window partitioned?
        /// @return 'true' is partitioned is set
        bool getIsPartitioned() const { return _partitioned; }

        /// Set this window partitioned setting
        /// @param v 'true' if window is partitioned
        void setIsPartitioned(bool v) { _partitioned = v; }

        /// Set this window partition eviction policy
        /// @param kind policy kind
        /// @param value policy expression value
        void setPartitionEvictionPolicy(PartitionEvictionPolicy kind, const Expression& value)
        {
            _partitionEvictionPolicy = kind;
            _partitionEvictionValue = &value;
        }

        /// Does this window have a partition eviction policy?
        /// @return 'true' if there is a partitionEviction Policy
        bool hasPartitionEvictionPolicy() const { return _partitionEvictionValue != NULL; }

        /// Get this window partition eviction policy
        /// @return partition eviction policy
        PartitionEvictionPolicy getPartitionEvictionPolicy() const
        {
            return _partitionEvictionPolicy;
        }

        /// Get this window partition eviction value
        /// @return partition eviction value
        const Expression& getPartitionEvictionPolicyValue() const
        {
            assert(_partitionEvictionValue);
            return *_partitionEvictionValue;
        }

        /// Print a Window
        /// @param  s stream to print to
        /// @return s
        std::ostream& print(std::ostream& s) const;

        /// Collect Type and Function Information
        /// @param types   if non-NULL, named types will be added
        /// @param fcns    if non-NULL, referenced functions will be added
        /// @param inFcns  if true, walk into references functions
        void collectTypeAndFunctions(std::tr1::unordered_set<const RootTyp*>* types,
                                     std::tr1::unordered_set<const FunctionHeadSym*>* fcns,
                                     bool inFcns) const;

        /// Get the intervalDuration of the window.  This is valid only if the
        /// window is an event-time window.
        const Expression& getIntervalDuration() const
        {
            assert(_intervalDuration);
            return *_intervalDuration;
        }

        /// Determine whether creationPeriod is defined for this window.
        bool hasCreationPeriod() const { return _creationPeriod; }
        /// Get the creationPeriod of the window.  This is valid only if the
        /// window is an event-time window.
        const Expression& getCreationPeriod() const
        {
            assert(_creationPeriod);
            return *_creationPeriod;
        }
        /// Determine whether discardAge is defined for this window.
        bool hasDiscardAge() const { return _discardAge; }
        /// Get the discardAge of the window.  This is valid only if the
        /// window is an event-time window.
        const Expression& getDiscardAge() const
        {
            assert(_discardAge);
            return *_discardAge;
        }
        /// Determine whether intervalOffset is defined for this window.
        bool hasIntervalOffset() const { return _intervalOffset; }
        /// Get the intervalOffset of the window.  This is valid only if the
        /// window is an event-time window.
        const Expression& getIntervalOffset() const
        {
            assert(_intervalOffset);
            return *_intervalOffset;
        }

      private:
        WindowKind _kind;
        const SourceLocation& _loc;
        std::string _name;
        uint32_t _port_number;
        LogicKind _ex_policy;
        const Expression* _ex_id;    // eviction attribute, or event-time attribute
        const Expression* _ex_value; // eviction size
        LogicKind _tr_policy;
        const Expression* _tr_id;    // trigger attribute
        const Expression* _tr_value; // trigger size
        bool _partitioned;
        PartitionEvictionPolicy _partitionEvictionPolicy;
        const Expression* _partitionEvictionValue;

        const Expression* _intervalDuration; // interval duration of event-time window
        const Expression* _creationPeriod;   // creation period of event-time window
        const Expression* _discardAge;       // discard age of event-time window
        const Expression* _intervalOffset;   // interval offset of event-time window
    };

    /// Defines the outputs for one output port
    /// Supports  attrib = expn
    /// Implemented as a BinaryExpression:   attr '=' (eq) expn
    class Output
    {
      public:
        // Constructor
        Output() {}

        // Destructor: delete expressions
        ~Output();

        /// Add an 'id = expn' Expression to the list
        /// @param expn Expression to be added
        void add(const BinaryExpression& expn) { _attrs.push_back(&expn); }

        /// Get the vector of expressions
        const std::vector<const BinaryExpression*>& get() const { return _attrs; }

        /// Print a Window
        /// @param  s stream to print to
        /// @param  needLabel  Do we need a label printed (updated)
        /// @param  name Port name
        /// @return s
        std::ostream& print(std::ostream& s, bool& needLabel, const std::string& name) const;

        /// Collect Type and Function Information
        /// @param types   if non-NULL, named types will be added
        /// @param fcns    if non-NULL, referenced functions will be added
        /// @param inFcns  if true, walk into references functions
        void collectTypeAndFunctions(std::tr1::unordered_set<const RootTyp*>* types,
                                     std::tr1::unordered_set<const FunctionHeadSym*>* fcns,
                                     bool inFcns) const;

      private:
        std::vector<const BinaryExpression*> _attrs;
    };

  private:
    const SourceLocation& _location;
    const SourceLocation& _endLocation;
    const CompositeOperatorInstance* _compOperInstance;
    uint32_t _toolkitIndex;
    std::string _kind;
    mutable const SPLParallelAnnotation* _parallelAnnotation;
    OperatorGraphNode* _graphNode;
    std::vector<std::string> _names;
    std::vector<Port*> _iports;
    std::vector<Port*> _oports;
    Logic _logic;
    std::tr1::unordered_map<uint32_t, Window*> _windows;
    std::vector<Param*> _params;
    std::vector<Output> _outputs;
    std::vector<Config*> _configs;
    std::string _splDocComment;
    mutable std::vector<const SPLAnnotation*> _annotations;
};
std::ostream& operator<<(std::ostream& s, const PrimitiveOperatorInstance& o);

//! Write a WindowKind value to a stream
inline std::ostream& operator<<(std::ostream& s, PrimitiveOperatorInstance::Window::WindowKind kind)
{
    switch (kind) {
        case PrimitiveOperatorInstance::Window::Tumbling:
            return s << "tumbling";
        case PrimitiveOperatorInstance::Window::Sliding:
            return s << "sliding";
        case PrimitiveOperatorInstance::Window::Buckets:
            return s << "timeInterval";
        default:
            assert(!"Invalid WindowKind value");
            return s << "";
    }
}
} // end namespace SPL

#endif /* SPL_CORE_OPERATOR_INVOCATION_H */
