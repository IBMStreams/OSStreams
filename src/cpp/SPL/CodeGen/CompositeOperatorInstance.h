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

#ifndef SPL_COMPOSITE_OPERATOR_INSTANCE_H
#define SPL_COMPOSITE_OPERATOR_INSTANCE_H

#include <SPL/CodeGen/Config.h>
#include <SPL/CodeGen/OperatorIR.h>
#include <SPL/Utility/ModelDumper.h>
#include <string>
#include <vector>

namespace SPL {
class PrimitiveOperatorInstance;
class CompositeDefinition;
class SourceLocation;
class SPLAnnotation;
class SPLParallelAnnotation;

/// This class represents a composite operator instance
class CompositeOperatorInstance
{
  public:
    /// Constructor
    /// @param compositeIndex index into used composites
    /// @param parent parent composite operator instance
    CompositeOperatorInstance(uint32_t compositeIndex, const CompositeOperatorInstance* parent);

    /// Destructor
    ~CompositeOperatorInstance();

    /// Get the composite definition for this composite operator instance
    /// @return composite definition for this index
    const CompositeDefinition& composite() const;

    /// Create a nested composite operator instance to the current one (allows nice cleanup)
    /// @param compositeIndex index into used composites for the nested composite operator instance
    /// @return new nested composite operator instance
    CompositeOperatorInstance& createNestedCompOperInstance(uint32_t compositeIndex);

    /// Add a primitive operator instance
    /// @param op a primitive operator instance nested in this composite operator instance
    void addPrimitiveInstance(const PrimitiveOperatorInstance& op);

    /// Add a config
    /// @param config to add
    void addConfig(Config& config) { _configs.push_back(&config); }

    const std::vector<Config*>& getConfigs() const { return _configs; }

    /// Get the index into used composites
    /// @return index into used composites
    uint32_t getCompositeIndex() const { return _index; }

    /// Set the index into used composites
    /// @param index index into used composites
    void setCompositeIndex(uint32_t index) { _index = index; }

    /// Return the parent composite operator instance
    /// @return parent
    const CompositeOperatorInstance* parent() const { return _parent; }

    /// Return all the primitive instances contained
    /// @return vector of operator invocations
    const std::vector<const PrimitiveOperatorInstance*>& getPrimitiveInstances() const
    {
        return _primitives;
    }

    /// Return all the compsite instances contained
    /// @return vector of composite instances
    const std::vector<const CompositeOperatorInstance*>& getCompositeInstances() const
    {
        return _nestedCompOperInstances;
    }

    /// Set the start location for the composite
    /// @param start Start Location
    void setStartLocation(const SourceLocation& start) { _startLocation = &start; }

    /// Set the end location for the composite
    /// @param end End Location
    void setEndLocation(const SourceLocation& end) { _endLocation = &end; }

    /// Set the instance name for the composite (outer.inner)
    /// @param name Instance Name
    void setInstanceName(const std::string& name) { _instanceName = name; }

    /// Return the start location for the composite
    /// @return Start Location
    const SourceLocation& getStartLocation() const { return *_startLocation; }

    /// Return the end location for the composite
    /// @return End Location
    const SourceLocation& getEndLocation() const { return *_endLocation; }

    /// Return the instance name for the composite (outer.inner)
    /// @return instance name
    const std::string& getInstanceName() const { return _instanceName; }

    /// Look for a particular config
    /// @param kind config kind
    /// @return pointer to Config, or NULL
    const Config* find(Config::Kind kind) const;

    /// Look for a particular placement config
    /// @param subKind placement config subKind
    /// @return pointer to Config, or NULL
    const Config* find(PlacementConstraint::PlacementSubKind subKind) const;

    /// Push an empty port vector of 'actual' input streams
    void pushInputPortActualStreamList() { _inputStreams.push_back(std::vector<std::string>()); }

    /// Add to the current port vector of 'actual' input streams
    /// @param stream name for the 'actual' input port stream, this is the
    /// fully qualified local name of the stream in the invoking scope
    void addInputPortActualStream(const std::string& stream)
    {
        _inputStreams.rbegin()->push_back(stream);
        MODEL("AddInputPortActualStream(" << (void*)this << ", " << stream << ")\n");
    }

    /// Add the 'formal' stream name for the next input port
    /// @param stream name for the 'formal' input port stream, this is the
    /// fully qualified local name of the stream in the invoked scope
    void addInputPortFormalStream(const std::string& stream)
    {
        _inputDefinitionStreams.push_back(stream);
        MODEL("AddInputPortFormalStream(" << (void*)this << ", " << stream << ")\n");
    }

    /// Add the 'actual' stream name for the next output port
    /// @param stream name for the 'actual' output port stream, this is the
    /// fully qualified local name of the stream in the invoking scope
    void addOutputPortActualStream(const std::string& stream) { _outputStreams.push_back(stream); }

    /// Add the 'formal' stream name for the next output port
    /// @param stream name for the 'formal' output port stream, this is the
    /// fully qualified local name of the stream in the invoked scope
    void addOutputPortFormalStream(const std::string& stream)
    {
        _outputDefinitionStreams.push_back(stream);
    }

    /// Add the alias for the output port.  There will always be one added,
    /// although it may be an empty string, which indicates no port alias
    /// @param alias port name for the output port
    void addInputPortAlias(const std::string& alias) { _inputAliases.push_back(alias); }

    /// Add the alias for the output port.  There will always be one added,
    /// although it may be an empty string, which indicates no port alias
    /// @param alias port name for the output port
    void addOutputPortAlias(const std::string& alias) { _outputAliases.push_back(alias); }

    /// Return the input port 'actual' streams
    /// @pre index < number of input ports
    /// @param index input port number
    /// @return vector of streams feeding input port (fully qualified local
    /// names in the calling scope)
    const std::vector<std::string>& getInputPortActualStreams(uint32_t index) const
    {
        assert(index < _inputStreams.size());
        return _inputStreams[index];
    }

    /// Return the output port 'actual' stream
    /// @pre index < number of output ports
    /// @param index output port number
    /// @return stream generated by output port (fully qualified local name
    /// in the invoking scope)
    const std::string& getOutputPortActualStream(uint32_t index) const
    {
        assert(index < _outputStreams.size());
        return _outputStreams[index];
    }

    /// Return the input port 'formal' stream
    /// @pre index < number of input ports
    /// @param index input port number
    /// @return stream feeding input port index (fully qualified local
    /// name in the invoked scope)
    const std::string& getInputPortFormalStream(uint32_t index) const
    {
        assert(index < _inputDefinitionStreams.size());
        return _inputDefinitionStreams[index];
    }

    /// Return the output port 'formal' stream
    /// @pre index < number of output ports
    /// @param index output port number
    /// @return stream feeding output port index (fully qualified local
    /// name in the invoked scope)
    const std::string& getOutputPortFormalStream(uint32_t index) const
    {
        assert(index < _outputDefinitionStreams.size());
        return _outputDefinitionStreams[index];
    }

    /// Return the input port alias
    /// @pre index < number of input ports
    /// @param index input port number
    /// @return alias for the given input port, or an empty string if there isn't one
    const std::string& getInputPortAlias(uint32_t index) const
    {
        assert(index < _inputDefinitionStreams.size());
        return _inputAliases[index];
    }

    /// Return the output port alias
    /// @pre index < number of output ports
    /// @param index output port number
    /// @return alias for the given output port, or an empty string if there isn't one
    const std::string& getOutputPortAlias(uint32_t index) const
    {
        assert(index < _outputDefinitionStreams.size());
        return _outputAliases[index];
    }

    /// Set spldoc comment for the invocation
    /// @param comment @splDoc comment for this invocation
    /// @param congestionPolicy congestion policy on queue full
    void setSplDocComment(const std::string& comment) { _splDocComment = comment; }

    /// Return the splDocComment for this invocation
    /// @return saved comment
    const std::string& getSplDocComment() const { return _splDocComment; }

    /// Print the composite operator instance
    /// @param s output stream
    /// @return s
    std::ostream& print(std::ostream& s) const;

    /// Add an SPL annotation to this composite
    /// @param annotation The annotation
    void addAnnotation(const SPLAnnotation& annotation) const;

    /// Access the set of annatations associated with this composite
    /// @return Returns the annotations
    const std::vector<const SPLAnnotation*>& annotations() const { return _annotations; }

    /// Access the set of view annatations associated with this composite
    /// @return Returns the annotations
    const std::vector<const SPLAnnotation*>& viewAnnotations() const { return _viewAnnotations; }

    /// Determine if this primitive operator represents a parallel region
    /// @return Returns true if this primitive is a parallel region, otherwise false;
    bool isParallelRegion() const { return NULL != _parallelAnnotation; }

    /// Access the parallel annotation associated with this composite
    /// @return Returns the SPLParallelAnnotation
    const SPLParallelAnnotation& parallelAnnotation() const
    {
        assert(NULL != _parallelAnnotation);
        return *_parallelAnnotation;
    }

    /// Get the name of the parallel region defined by this composite
    /// This must be called on a composite that is a parallel region
    /// @return Returns the name of the parallel region
    std::string parallelRegionName() const;

    /// Is this operator a parallel region, or is it within a parallel region
    bool isInParallelRegion() const;

    /// Get the level of nesting in UDP regions. If the operator is
    /// not in a UDP region it returns 0.
    uint32_t parallelNestingLevel() const;
    uint32_t parallelNestingLevel(const CompositeOperatorInstance& introducingComp) const;

    /// Get the parallel annotation for this operator, or its containing region
    const SPLParallelAnnotation& getParallelAnnotation() const;

  private:
    const Config* findInstance(Config::Kind kind) const;
    const Config* findDefinition(Config::Kind kind) const;
    const Config* findInstance(PlacementConstraint::PlacementSubKind subKind) const;
    const Config* findDefinition(PlacementConstraint::PlacementSubKind subKind) const;

    const SourceLocation *_startLocation, *_endLocation;
    std::string _instanceName;
    uint32_t _index;
    std::vector<Config*> _configs;
    const CompositeOperatorInstance* _parent;
    mutable const SPLParallelAnnotation* _parallelAnnotation;
    std::vector<const CompositeOperatorInstance*> _nestedCompOperInstances;
    std::vector<const PrimitiveOperatorInstance*> _primitives;
    std::vector<std::vector<std::string> > _inputStreams;
    std::vector<std::string> _inputAliases;
    std::vector<std::string> _inputDefinitionStreams;
    std::vector<std::string> _outputDefinitionStreams;
    std::vector<std::string> _outputStreams;
    std::vector<std::string> _outputAliases;
    mutable std::vector<const SPLAnnotation*> _annotations;
    mutable std::vector<const SPLAnnotation*> _viewAnnotations;
    std::string _splDocComment;
};

inline std::ostream& operator<<(std::ostream& s, const CompositeOperatorInstance& c)
{
    c.print(s);
    return s;
}
};

#endif /* SPL_COMPOSITE_OPERATOR_INSTANCE_H */
