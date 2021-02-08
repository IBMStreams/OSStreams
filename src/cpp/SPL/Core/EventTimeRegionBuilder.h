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

#ifndef SPL_EVENT_TIME_REGION_BUILDER_H
#define SPL_EVENT_TIME_REGION_BUILDER_H

#include <set>

#include <SPL/CodeGen/OperatorIR.h>
#include <SPL/CodeGen/SPLAnnotation.h>
#include <SPL/Core/OperatorGraph.h>
#include <SPL/Runtime/Utility/Singleton.h>

namespace SPL {
class SourceLocation;

enum Resolution
{
    Default,      //! the type-dependent default resolution
    Milliseconds, //! milliseconds
    Microseconds, //! microseconds
    Nanoseconds   //! nanoseconds
};

inline std::ostream& operator<<(std::ostream& ostr, Resolution r)
{
    switch (r) {
        case Default:
            return ostr << "(default resolution for type)";
        case Milliseconds:
            return ostr << "Milliseconds";
        case Microseconds:
            return ostr << "Microseconds";
        case Nanoseconds:
            return ostr << "Nanoseconds";
        default:
            assert(false);
            return ostr << (int)r;
    }
}

typedef std::set<SPLSimpleAnnotation::KeyValuePair*> KVPSet;

class EventTimeRegionBuilder : public Singleton<EventTimeRegionBuilder>
{
  public:
    EventTimeRegionBuilder(OperatorGraph const& ograph);
    virtual ~EventTimeRegionBuilder();

    class EventTimeRegionInfo
    {
      public:
        std::string attribute;
        std::string type;
        Resolution resolution;
        KVPSet lagList; // elements are not owned by this class
        KVPSet gapList; // elements are not owned by this class

        EventTimeRegionInfo()
          : attribute()
          , type()
          , resolution(Default)
          , lagList()
          , gapList()
        {}
        EventTimeRegionInfo(std::string const& att,
                            std::string const& t,
                            Resolution u,
                            KVPSet const& l,
                            KVPSet const& g)
          : attribute(att)
          , type(t)
          , resolution(u)
          , lagList(l)
          , gapList(g)
        {}
        EventTimeRegionInfo(EventTimeRegionInfo const& other)
          : attribute(other.attribute)
          , type(other.type)
          , resolution(other.resolution)
          , lagList(other.lagList)
          , gapList(other.gapList)
        {}
        EventTimeRegionInfo& operator=(EventTimeRegionInfo other)
        {
            swap(*this, other);
            return *this;
        }
        ~EventTimeRegionInfo() {}
        friend void swap(EventTimeRegionInfo& lhs, EventTimeRegionInfo& rhs)
        {
            using std::swap;
            swap(lhs.attribute, rhs.attribute);
            swap(lhs.type, rhs.type);
            swap(lhs.resolution, rhs.resolution);
            swap(lhs.lagList, rhs.gapList);
            swap(lhs.gapList, rhs.gapList);
        }

        bool operator==(EventTimeRegionInfo const& other) const
        {
            bool isEqual = false;
            if (attribute == other.attribute && type == other.type &&
                resolution == other.resolution) {
                // lag and gap are intentionally excluded from the comparison.
                isEqual = true;
            }
            return isEqual;
        }

        size_t hashCode() const;

        std::string toString() const;

        friend std::ostream& operator<<(std::ostream& ostr, EventTimeRegionInfo const& info);
    };

    class OutputPortIdentifier
    {
      public:
        std::string nodeName;
        uint32_t portIndex;

        // Name here is the name of the operator, as given in the
        // OperatorGraphNode for the operator.
        OutputPortIdentifier(std::string const& name, uint32_t index)
          : nodeName(name)
          , portIndex(index)
        {}

        OutputPortIdentifier(OutputPortIdentifier const& other)
          : nodeName(other.nodeName)
          , portIndex(other.portIndex)
        {}

        ~OutputPortIdentifier() {}

        OutputPortIdentifier& operator=(OutputPortIdentifier other)
        {
            swap(*this, other);
            return *this;
        }

        bool operator==(OutputPortIdentifier const& other) const
        {
            return nodeName == other.nodeName && portIndex == other.portIndex;
        }

        friend void swap(OutputPortIdentifier& lhs, OutputPortIdentifier& rhs)
        {
            using std::swap;
            swap(lhs.nodeName, rhs.nodeName);
            swap(lhs.portIndex, rhs.portIndex);
        }

        size_t hashCode() const;
        std::string toString() const;

        friend std::ostream& operator<<(std::ostream& ostr, OutputPortIdentifier const& port);
    };

    struct OutputPortIdentifierHasher
    {
        size_t operator()(OutputPortIdentifier const& self) const { return self.hashCode(); }
    };

    // Unique identifier for a connection.  The connection is identified
    // by the operator (node name), input port index, and the connected
    // operator index, and the connected operator output port index.
    class InputConnectionIdentifier
    {
      public:
        std::string nodeName;
        uint32_t inputPortIndex;
        uint32_t connectedOperatorIndex;
        uint32_t connectedPortIndex;

        InputConnectionIdentifier(std::string const& name,
                                  uint32_t port,
                                  uint32_t connectedOperator,
                                  uint32_t connectedPort)
          : nodeName(name)
          , inputPortIndex(port)
          , connectedOperatorIndex(connectedOperator)
          , connectedPortIndex(connectedPort)
        {}
        ~InputConnectionIdentifier() {}

        InputConnectionIdentifier& operator=(InputConnectionIdentifier other)
        {
            swap(*this, other);
            return *this;
        }

        bool operator==(InputConnectionIdentifier const& other) const
        {
            return nodeName == other.nodeName && inputPortIndex == other.inputPortIndex &&
                   connectedOperatorIndex == other.connectedOperatorIndex &&
                   connectedPortIndex == other.connectedPortIndex;
        }

        friend void swap(InputConnectionIdentifier& lhs, InputConnectionIdentifier& rhs)
        {
            using std::swap;
            swap(lhs.nodeName, rhs.nodeName);
            swap(lhs.inputPortIndex, rhs.inputPortIndex);
            swap(lhs.connectedOperatorIndex, rhs.connectedOperatorIndex);
            swap(lhs.connectedPortIndex, rhs.connectedPortIndex);
        }

        size_t hashCode() const;
        std::string toString() const;
        friend std::ostream& operator<<(std::ostream& ostr,
                                        InputConnectionIdentifier const& connection);
    };

    struct InputConnectionIdentifierHasher
    {
        size_t operator()(InputConnectionIdentifier const& self) const { return self.hashCode(); }
    };

    // Find all event time subgraphs and associate event-time region info
    // to each port belonging to a subgraph.
    void build();

  private:
    typedef std::tr1::
      unordered_map<OutputPortIdentifier, EventTimeRegionInfo, OutputPortIdentifierHasher>
        OutputPortEventTimeMap;
    OutputPortEventTimeMap outputPortEventTimeMap;

    typedef std::tr1::
      unordered_map<InputConnectionIdentifier, EventTimeRegionInfo, InputConnectionIdentifierHasher>
        InputConnectionEventTimeMap;
    InputConnectionEventTimeMap inputConnectionEventTimeMap;

    // Test whether a port is compatible with an event-time subgraph.  It is
    // compatible if it has an attribute with name and type matching those
    // in the EventTimeRegionInfo.
    bool portIsCompatible(PrimitiveOperatorInstance::Port const& inputPort,
                          EventTimeRegionInfo const& info) const;

    // Register a port with its event-time region info.
    // If this returns false, the port had previously been registered into
    // an event-time subgraph, so the search should not continue upstream.
    bool registerPort(std::string const& nodeName, uint32_t port, EventTimeRegionInfo const& info);
    bool registerConnection(std::string const& nodeName,
                            uint32_t port,
                            uint32_t connectedOperator,
                            uint32_t connectedPort,
                            EventTimeRegionInfo const& info);

    void visitPrimitive(PrimitiveOperatorInstance const* primitive);
    void visitComposite(CompositeOperatorInstance const* composite);

    // Starting from a primitive operator with an @eventTime annotation,
    // build the connected downstream subgraph of ports with compatible
    // attributes.
    void buildEventTimeSubgraph(PrimitiveOperatorInstance const* primitive,
                                EventTimeRegionInfo& info);

    // Add an input connection to the subgraph, and follow downstream.
    void addInputConnectionToSubgraph(PrimitiveOperatorInstance const* primitive,
                                      uint32_t portIndex,
                                      uint32_t connectedOperator,
                                      uint32_t connectedPort,
                                      EventTimeRegionInfo info);
    // Add an output port to the subgraph, and follow downstream.
    void addOutputPortToSubgraph(PrimitiveOperatorInstance const* primitive,
                                 uint32_t portIndex,
                                 EventTimeRegionInfo info);

    // Add synthetic annotations for the event-time context to the primitive
    // operators.
    void synthesizeAnnotations();

    PrimitiveOperatorInstance::Port const* getIRPort(OutputPortIdentifier const&) const;
    SourceLocation const* getSourceLocation(OutputPortIdentifier const&) const;
    std::string const& getOperatorName(OutputPortIdentifier const&) const;
    std::string getPortName(OutputPortIdentifier const&) const;

    PrimitiveOperatorInstance::Port const* getIRPort(InputConnectionIdentifier const&) const;
    SourceLocation const* getSourceLocation(InputConnectionIdentifier const&) const;
    std::string const& getOperatorName(InputConnectionIdentifier const&) const;
    std::string getPortName(InputConnectionIdentifier const&) const;

    OperatorGraph const& graph;

    SPLSimpleAnnotation::KeyValuePair defaultLag;
    SPLSimpleAnnotation::KeyValuePair defaultGap;

}; // class EventTimeRegionBuilder

} // end namespace SPL

#endif // SPL_EVENT_TIME_REGION_BUILDER_H
