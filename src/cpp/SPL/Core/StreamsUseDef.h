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

#ifndef STREAMS_USE_DEF_H
#define STREAMS_USE_DEF_H

#include <SPL/Runtime/Utility/Singleton.h>
#include <UTILS/HashMapHelpers.h>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace SPL {
struct StreamUseDefLocation;
}

namespace std {
namespace tr1 {
template<>
struct hash<SPL::StreamUseDefLocation>
{
    size_t operator()(SPL::StreamUseDefLocation const&) const;
};
}
}

namespace SPL {

//! Location of a stream use/def
class StreamUseDefLocation
{
  public:
    enum PortType
    {
        input,
        output
    };
    StreamUseDefLocation(std::string const& oper, uint32_t port, PortType portType)
      : oper_(oper)
      , port_(port)
      , portType_(portType)
    {}
    std::string const& getOperator() const { return oper_; }
    uint32_t getPort() const { return port_; }
    PortType getPortType() const { return portType_; }
    std::ostream& print(std::ostream& str) const;
    bool operator==(StreamUseDefLocation const& o) const
    {
        return oper_ == o.oper_ && port_ == o.port_ && portType_ == o.portType_;
    }

  private:
    std::string oper_;
    uint32_t port_;
    PortType portType_;
};

//! Stream Operator Use and Definition Class
class StreamsUseDef : public Singleton<StreamsUseDef>
{
  public:
    typedef std::tr1::unordered_set<StreamUseDefLocation> Locations;

    /// Constructor
    StreamsUseDef();

    /// Set the definition of a stream
    /// @param stream stream name
    /// @param oper operator name
    /// @param port port index
    /// @param portType type of the port
    void setDef(std::string const& stream,
                std::string const& oper,
                uint32_t port,
                StreamUseDefLocation::PortType portType);

    /// Add a use of a stream
    /// @param stream stream name
    /// @param oper operator name
    /// @param port port number
    /// @param portType type of the port
    void addUse(std::string const& stream,
                std::string const& oper,
                uint32_t port,
                StreamUseDefLocation::PortType portType);

    /// Return the uses for a stream
    /// @param stream stream name
    /// @return vector of use locations
    const Locations& getUses(const std::string& stream) const;

    /// Return the definitions for a stream
    /// @param stream stream name
    /// @return definition location, if exists
    const StreamUseDefLocation* getDef(const std::string& stream) const;

    /// Print current data
    /// @param str Output Stream
    /// @return Output Stream
    std::ostream& print(std::ostream& str) const;

  private:
    struct UseDefs
    {
        Locations _uses;
        Locations _defs; // size() <= 1 at all times
    };
    // Map map
    std::tr1::unordered_map<std::string, UseDefs> _streams;
};

inline std::ostream& operator<<(std::ostream& str, const StreamsUseDef& s)
{
    return s.print(str);
}
};

#endif /* STREAMS_USE_DEF_H */
