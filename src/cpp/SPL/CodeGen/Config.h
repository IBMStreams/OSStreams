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

#ifndef Core_CONFIG_H
#define Core_CONFIG_H

/// Operator Configuratin Information

#include <Core/PlacementConstraint.h>
#include <UTILS/HashMapHelpers.h>
#include <assert.h>
#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>

namespace SPL {
class RootTyp;
class Literal;
class SourceLocation;
class PrimitiveOperatorInstance;

class Config
{
  public:
    class OneHostPool;

    //! Configuration Kind
    enum Kind
    {
        ApplicationScope,  //! Application scope for main composite (goes in ADL)
        Checkpoint,        //! period of checkpoints, and 'periodic' status of them
        DefPool,           //! default pool
        HostPools,         //! poll definitions
        InputTransport,    //! Transport mechanism for input streams
        LogLevel,          //! How much to log -- deprecated - Use Tracing
        OutputTransport,   //! Transport mechanism for output streams
        Placement,         //! Where do they all go?
        Relocatable,       //! Can I move?
        Restartable,       //! Can I restart?
        StreamViewability, //! Is the output stream viewable?
        Tracing,           //! How much to trace
        Wrapper,           //! Wrapper application

        numKinds
    };

    static const char* kindNames[numKinds];

    //! Tracing - Also used for LogLevel backwards compatibility
    enum TracingKind
    {
        Off,   //! No output
        Error, //! Errors
        Info,  //! Informational Messages
        Warn,  //! Warnings
        Debug, //! Internal Debug Messages
        Trace  //! What is happening...
    };

    //! Transport Types TCP
    enum TransportKind
    {
        TCP, //! TCP
        numTransportKinds
    };

    static const char* transportKindNames[numTransportKinds];

    /// Find a configuration amongst the given list, considering
    /// restartability and relocatability implications
    /// @param kind the config kind to search for
    /// @param configs configs to search from
    /// @return the found config, or NULL if not found
    static Config* findConfig(Config::Kind kind, std::vector<Config*> const& configs);

    /// Find a placement configuration amongst the given list, only looking for
    /// the correct subKind
    /// @param subKind the placement subKind to search for
    /// @param configs configs to search from
    /// @return the found config, or NULL if not found
    static Config* findConfig(PlacementConstraint::PlacementSubKind subKind,
                              std::vector<Config*> const& configs);

    /// Checkpoint
    /// @param loc  Location of Config directive (for error messages)
    /// @param period Checkpoint Period
    /// @param periodic 'true' if the SPL runtime should do periodic checkpoints
    /// @return Constructed config directive for checkpoint
    static Config& fromCheckpoint(const SourceLocation& loc, double period, bool periodic)
    {
        return *new Config(loc, period, periodic);
    }

    //! Default Pool characteristics
    class DefaultPool
    {
      public:
        /// Construction
        /// @param size Pool size
        DefaultPool(uint32_t size)
          : _size(size)
        {}

        /// @return default pool size
        uint32_t poolSize() const { return _size; }

      private:
        uint32_t _size;
    };

    //! Output stream viewability
    class StreamsViewability
    {
      public:
        /// Construction
        StreamsViewability()
          : _hasDefaultViewability(false)
          , _defaultViewability(false)
        {}

        /// Copy ctor
        /// @param rhs right hand side
        StreamsViewability(const StreamsViewability& rhs)
          : _hasDefaultViewability(rhs._hasDefaultViewability)
          , _defaultViewability(rhs._defaultViewability)
          , _ports(rhs._ports)
        {}

        /// Are these two equal?
        bool operator==(const StreamsViewability& rhs) const;

        /// Print
        /// @param  s stream to print to
        /// @return s
        std::ostream& print(std::ostream& s) const;

        /// Set the default viewability
        /// @param value The default viewability
        void setDefaultViewability(bool value)
        {
            _hasDefaultViewability = true;
            _defaultViewability = value;
        }

        /// Is there a default viewability for this config?
        /// @return 'true' if setDefaultViewability has been called
        bool hasDefaultViewability() const { return _hasDefaultViewability; }

        /// Return the default viewability for this config
        /// @return 'true' if setDefaultViewability(true) has been called else 'false'
        bool getDefaultViewability() const { return _defaultViewability; }

        /// Is there a default viewability for this config?
        /// @param port The output port number
        /// @param isViewable set to the correct viewability if mentioned by this config
        /// @return 'true' if the port was already set
        bool setViewability(uint32_t port, bool isViewable)
        {
            std::pair<PortMap::iterator, bool> res =
              _ports.insert(std::make_pair(port, isViewable));
            return !res.second;
        }

        // Is a given port viewable?
        /// @param port The output port number
        /// @param isViewable set to the correct viewability if mentioned by this config
        /// @return 'true' if the port is explicitly set by this config
        bool portIsViewable(uint32_t port, bool& isViewable) const
        {
            PortMap::const_iterator it = _ports.find(port);
            if (it != _ports.end()) {
                isViewable = it->second;
                return true;
            }
            if (_hasDefaultViewability) {
                isViewable = _defaultViewability;
                return true;
            }
            return false;
        }

      private:
        bool _hasDefaultViewability, _defaultViewability;
        typedef std::tr1::unordered_map<uint32_t, bool> PortMap;
        PortMap _ports;
    };

    /// DefaultPool
    /// @param loc  Location of Config directive (for error messages)
    /// @param size pool size
    /// @return Constructed config directive for default pool
    static Config& fromDefaultPool(const SourceLocation& loc, uint32_t size)
    {
        return *new Config(loc, size);
    }

    /// PlacementConstraint
    /// @param loc  Location of Config directive (for error messages)
    /// @param placement placement constraint
    /// @return Constructed config directive for placement constraint
    static Config& fromPlacementConstraint(const SourceLocation& loc,
                                           PlacementConstraint& placement)
    {
        return *new Config(loc, placement);
    }

    /// HostPools
    /// @param loc  Location of Config directive (for error messages)
    /// @param hosts Host information
    /// @return Constructed config directive for hostPool
    static Config& fromHostPools(const SourceLocation& loc, std::vector<OneHostPool>& hosts)
    {
        return *new Config(loc, hosts);
    }

    /// LogLevel - deprecated by runtime - Users should use tracing
    /// @param loc  Location of Config directive (for error messages)
    /// @param loglevel Level of logging
    /// @return Constructed config directive for logLevel
    static Config& fromLogLevel(const SourceLocation& loc, TracingKind loglevel)
    {
        return *new Config(loc, LogLevel, loglevel);
    }

    /// Tracing
    /// @param loc  Location of Config directive (for error messages)
    /// @param tracelevel Level of tracing
    /// @return Constructed config directive for tracing
    static Config& fromTracing(const SourceLocation& loc, TracingKind tracelevel)
    {
        return *new Config(loc, Tracing, tracelevel);
    }

    /// Relocatable
    /// @param loc  Location of Config directive (for error messages)
    /// @param value 'true'/'false'
    /// @return Constructed config directive for relocatable
    static Config& fromRelocatable(const SourceLocation& loc, bool value)
    {
        return *new Config(loc, Relocatable, value);
    }

    /// Restartable
    /// @param loc  Location of Config directive (for error messages)
    /// @param value 'true'/'false'
    /// @return Constructed config directive for restartable
    static Config& fromRestartable(const SourceLocation& loc, bool value)
    {
        return *new Config(loc, Restartable, value);
    }

    /// Constructor - InputTransport
    /// @param loc  Location of Config directive (for error messages)
    /// @param transport input transport type
    /// @return Constructed config directive for inputTransport
    static Config& fromInputTransport(const SourceLocation& loc, TransportKind transport)
    {
        return *new Config(loc, InputTransport, transport);
    }

    /// Constructor - OuputTransport
    /// @param loc  Location of Config directive (for error messages)
    /// @param transport output transport type
    /// @return Constructed config directive for outputTransport
    static Config& fromOutputTransport(const SourceLocation& loc, TransportKind transport)
    {
        return *new Config(loc, OutputTransport, transport);
    }

    /// Constructor - Wrapper
    /// @param loc  Location of Config directive (for error messages)
    /// @param wrapper Wrapper
    /// @return Constructed config directive for wrapper
    static Config& fromWrapper(const SourceLocation& loc, const std::string& wrapper)
    {
        return *new Config(loc, Wrapper, wrapper);
    }

    /// Constructor - ApplicationScope
    /// @param loc  Location of Config directive (for error messages)
    /// @param scope application scope value
    /// @return Constructed config directive for applicationScope
    static Config& fromApplicationScope(const SourceLocation& loc, const std::string& scope)
    {
        return *new Config(loc, ApplicationScope, scope);
    }

    /// Constructor - StreamViewability
    /// @param loc  Location of Config directive (for error messages)
    /// @return Constructed config directive for streamViewability
    static Config& fromStreamViewability(const SourceLocation& loc)
    {
        return *new Config(loc, *new StreamsViewability);
    }

  private:
    static Config _implicitNotRelocatable, _implicitRestartable;

    friend class PrimitiveOperatorInstance;
    /// Constructor - Checkpoint
    /// @param loc Source Location
    /// @param period Checkpoint Period
    /// @param periodic 'true' if SPL runtime will checkpoint periodically
    Config(const SourceLocation& loc, double period, bool periodic)
      : _kind(Checkpoint)
      , _location(loc)
    {
        _checkpointInfo._checkpointPeriod = period;
        _checkpointInfo._checkpointPeriodic = periodic;
    }

    /// Constructor - DefaultPool
    /// @param loc Source Location
    /// @param size pool size
    Config(const SourceLocation& loc, uint32_t size)
      : _kind(DefPool)
      , _location(loc)
    {
        _defaultPool = new DefaultPool(size);
    }

    /// Constructor - PlacementConstraint
    /// @param loc Source Location
    /// @param placement placement constraint
    Config(const SourceLocation& loc, PlacementConstraint& placement)
      : _kind(Placement)
      , _location(loc)
      , _placement(&placement)
    {}

    /// Constructor - HostPoola
    /// @param loc Source Location
    /// @param hosts Host information
    Config(const SourceLocation& loc, std::vector<OneHostPool>& hosts)
      : _kind(HostPools)
      , _location(loc)
      , _hostPools(&hosts)
    {}

    /// Constructor - LogLevel
    /// @param loc Source Location
    /// @param loglevel Level of logging
    Config(const SourceLocation& loc, Kind kind, TracingKind loglevel)
      : _kind(kind)
      , _location(loc)
      , _tracingLevel(loglevel)
    {}

    /// Constructor - Relocatable/Restartable
    /// @param loc Source Location
    /// @param kind Relocatable/Restartable
    /// @param value 'true'/'false'
    Config(const SourceLocation& loc, Kind kind, bool value)
      : _kind(kind)
      , _location(loc)
      , _relocatable(value)
    {}

    /// Constructor - InputTransport/OutputTransport
    /// @param loc Source Location
    /// @param transport Transport Type
    Config(const SourceLocation& loc, Kind kind, TransportKind transport)
      : _kind(kind)
      , _location(loc)
      , _transport(transport)
    {}

    /// Constructor - Wrapper/ApplicationScope
    /// @param loc Source Location
    /// @param kind Wrapper/ApplicationScope
    /// @param str string parameter
    Config(const SourceLocation& loc, Kind kind, const std::string& str);

    /// Constructor - StreamsViewability
    /// @param loc Source Location
    /// @param sv streams viewability
    Config(const SourceLocation& loc, StreamsViewability& sv)
      : _kind(StreamViewability)
      , _location(loc)
      , _streamsViewability(&sv)
    {}

  public:
    /// Copy Constructor
    /// @param rhs Value to copy
    Config(const Config& rhs);

    /// Destructor
    ~Config();

    /// Print Config
    /// @param  s stream to print to
    /// @return s
    std::ostream& print(std::ostream& s) const;

    /// Compare for Equality
    /// @param rhs  Config to compare to
    /// @return 'true' if *this is equal to rhs
    bool operator==(const Config& rhs) const;

    /// Compare for Inequality
    /// @param rhs  Config to compare to
    /// @return 'false' if *this is equal to rhs
    bool operator!=(const Config& rhs) const { return !(*this == rhs); }

    class OneHostPool
    {
      public:
        /// Explicit Time List
        /// @param name Pool name
        /// @param hosts Literal set of strings
        OneHostPool(const std::string& name, const Literal& hosts)
          : _implicit(false)
          , _name(name)
          , _hosts(&hosts)
        {}

        /// Implicit List
        /// @param name Pool name
        /// @param size Size of implicit pool
        OneHostPool(const std::string& name, uint32_t size)
          : _implicit(true)
          , _name(name)
          , _size(size)
        {}

        /// Implicit Tagged/Sized/Shared List
        /// @param name Pool name
        /// @param size Size of implicit pool
        /// @param hasSize is 'size' valid
        /// @param tags list of tag names
        /// @param exclusive 'true' if the hosts in the pool are exclusive to this pool
        OneHostPool(const std::string& name,
                    uint32_t size,
                    bool hasSizeIn,
                    const std::vector<std::string> tags,
                    bool exclusive)
          : _implicit(true)
          , _name(name)
          , _size(size)
          , _hasSize(hasSizeIn)
          , _tags(tags)
          , _exclusive(exclusive)
        {}

        /// Copy Constructor
        /// @param rhs OneHostPool to copy
        OneHostPool(const OneHostPool& rhs);

        /// Destructor
        ~OneHostPool();

        /// Is the host pool implicit?
        /// return 'true' if this is an implicit pool
        bool isImplicit() const { return _implicit; }

        /// Is the host pool explicit?
        /// return 'true' if this is an explicit pool
        bool isExplicit() const { return !_implicit; }

        /// Return the pool name
        /// return pool name
        const std::string& getName() const { return _name; }

        /// Return the size of a implicit pool
        /// return pool size
        uint32_t getSize() const { return _size; }

        /// Was the size explicitly set?
        /// return 'true' if size is valid
        bool hasSize() const { return _hasSize; }

        /// Is the pool exclusive?
        /// return 'true' if pools hosts can't be shared
        bool getExclusive() const { return _exclusive; }

        /// Return the list of hosts in the pool
        const Literal& getHosts() const { return *_hosts; }

        /// Return the list of tags for the pool
        const std::vector<std::string>& getTags() const { return _tags; }

        /// Print
        std::ostream& print(std::ostream& ostr) const;

        /// Comparison of OneHostPools
        /// @param rhs object to compare against
        /// return 'true' if *this is equivalent to rhs
        bool operator==(const OneHostPool& rhs) const;

        /// Comparison of OneHostPools
        /// @param rhs object to compare against
        /// return 'false' if *this is equivalent to rhs
        bool operator!=(const OneHostPool& rhs) const { return !(*this == rhs); }

      private:
        bool _implicit;
        std::string _name;
        union
        {
            uint32_t _size;
            const Literal* _hosts;
        };
        bool _hasSize;
        std::vector<std::string> _tags;
        bool _exclusive;
    };

    /// Return Config Kind
    /// @return kind
    Kind getKind() const { return _kind; }

    /// Return location
    /// @return source location
    const SourceLocation& getLocation() const { return _location; }

    /// Return Checkpoint Period
    /// @return the time between periodic checkpoints
    double getPeriod() const { return _checkpointInfo._checkpointPeriod; }

    /// Will checkpointing be done periodically by the runtime?
    /// @return 'true' if the SPL runtime will automatically checkpoint the operator
    bool hasPeriodicCheckpoint() const { return _checkpointInfo._checkpointPeriodic; }

    /// Return Default Pool
    /// @return default pool information
    const DefaultPool& getDefaultPool() const { return *_defaultPool; }

    /// Return Placement Constraint
    /// @return placement
    const PlacementConstraint& getPlacementConstraint() const { return *_placement; }

    /// Return Host Pools
    /// @return host pools
    const std::vector<OneHostPool>& getHostPools() const { return *_hostPools; }

    /// Return Streams Viewability
    /// @return streams viewability
    const StreamsViewability& getStreamsViewability() const { return *_streamsViewability; }

    /// Return Streams Viewability
    /// @return streams viewability
    StreamsViewability& getStreamsViewability() { return *_streamsViewability; }

    /// Return Tracing Level
    /// @return tracing level
    TracingKind getTracingLevel() const { return _tracingLevel; }

    /// Return Relocatability
    /// @return 'true' if operator is relocatable
    bool isRelocatable() const { return _relocatable; }

    /// Return Restartability
    /// @return 'true' if operator is restartable
    bool isRestartable() const { return _restartable; }

    /// Return Input Transport
    /// @return input transport kind
    TransportKind getInputTransport() const { return _transport; }

    /// Return Output Transport
    /// @return output transport kind
    TransportKind getOutputTransport() const { return _transport; }

    /// Return Wrapper information
    /// @return return wrapper
    const std::string& getWrapper() const { return *_str; }

    /// Return application scope information
    /// @return return application scope
    const std::string& getApplicationScope() const { return *_str; }

  private:
    Kind _kind;
    const SourceLocation& _location;
    struct CheckpointInfo
    {
        double _checkpointPeriod;
        bool _checkpointPeriodic;
    };
    union
    {
        CheckpointInfo _checkpointInfo;
        DefaultPool* _defaultPool;
        PlacementConstraint* _placement;
        std::vector<OneHostPool>* _hostPools;
        StreamsViewability* _streamsViewability;
        TracingKind _tracingLevel;
        bool _relocatable;
        bool _restartable;
        std::string* _str;
        TransportKind _transport;
    };
};

inline std::ostream& operator<<(std::ostream& s, const Config& c)
{
    return c.print(s);
}

inline std::ostream& operator<<(std::ostream& s, const Config::StreamsViewability& sv)
{
    return sv.print(s);
}
};

#endif /* Core_CONFIG_H */
