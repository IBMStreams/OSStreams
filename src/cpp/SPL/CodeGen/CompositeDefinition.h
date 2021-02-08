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

#ifndef SPL_COMPOSITE_DEFINITION_H
#define SPL_COMPOSITE_DEFINITION_H

#include <SPL/CodeGen/Config.h>
#include <UTILS/HashMapHelpers.h>
#include <cassert>
#include <string>
#include <vector>

namespace SPL {
class SourceLocation;
class CompositeOperatorInstance;
class SPLAnnotation;

//! class to describe the information for 1 composite operator
class CompositeDefinition
{
  public:
    /// Constructor
    /// @param kind     name   (name.space::oper.name)
    /// @param index    index in table
    CompositeDefinition(const std::string& kind, uint32_t index)
      : _kind(kind)
      , _start(NULL)
      , _end(NULL)
      , _main(false)
      , _potentialMain(false)
      , _index(index)
      , _toolkitIndex(0)
      , _numInputPorts(0)
      , _numOutputPorts(0)
      , _implicitInstance(NULL)
    {
        std::string::size_type i = _kind.find("::");
        if (i != std::string::npos) {
            _name = _kind.substr(i + 2);
            _namespace = _kind.substr(0, i);
        } else {
            _name = _kind;
        }
    }

    /// Constructor
    /// @param name   name
    /// @param nspace name space
    /// @param index  index in table
    CompositeDefinition(const std::string& name, const std::string& nspace, uint32_t index)
      : _name(name)
      , _namespace(nspace)
      , _kind(nspace + "::" + name)
      , _start(NULL)
      , _end(NULL)
      , _main(false)
      , _potentialMain(false)
      , _index(index)
      , _numInputPorts(0)
      , _numOutputPorts(0)
      , _implicitInstance(NULL)
    {}

    // Destructor
    ~CompositeDefinition();

    /// Get name (oper.name for name.space::oper.name)
    /// @return the name
    std::string getName() const { return _name; }

    /// Get namespace (name.space for name.space::oper.name)
    /// @return the namespace
    std::string getNamespace() const { return _namespace; }

    /// Add a Config to the Composite
    /// @param config Config to be added
    void addConfig(Config& config);

    /// Return the Config information
    /// @return list of Configs
    const std::vector<Config*>& getConfigs() const { return _configs; }

    /// Look for a particular config
    /// @param kind     Config kind
    /// @return pointer to Config, or NULL
    const Config* find(Config::Kind kind) const;

    /// Look for a particular placement config subKind
    /// @param subKindind     Placement config subKind
    /// @return pointer to Config, or NULL
    const Config* find(PlacementConstraint::PlacementSubKind subKind) const;

    /// Return the index of the Composite in the table
    /// @return where we are in the table
    uint32_t getIndex() const { return _index; }

    /// Set the toolkit index where the Composite is defined.  0 is the 'main SPL file' (and the
    /// default)
    /// @param toolkitIndex The index of the toolkit where the composite is defined
    void setToolkitIndex(uint32_t toolkitIndex) { _toolkitIndex = toolkitIndex; }

    /// Return the toolkit index where the Composite is defined.  0 is the 'main SPL file'
    /// @return the toolkit index number
    uint32_t getToolkitIndex() const { return _toolkitIndex; }

    /// Set the start line number
    /// @param start source location of end
    void setStartLocation(const SourceLocation& start) { _start = &start; }

    /// Set the end line number
    /// @param end      source location of end
    void setEndLocation(const SourceLocation& end) { _end = &end; }

    /// Return the start location
    /// @return start location information
    const SourceLocation& getStartLocation() const { return *_start; }

    /// Return the end location
    /// @return end location information
    const SourceLocation& getEndLocation() const { return *_end; }

    /// Set a composite as a potential main
    /// @param value true if this is a potential main composite
    void setPotentialMain(bool value) { _potentialMain = value; }

    /// Is this a potential main composite?
    /// @return true if we might be the main composite
    bool isPotentialMain() const { return _potentialMain; }

    /// Set a composite as the main
    /// @param value true if this is a main composite
    void setMain(bool value) { _main = value; }

    /// Is this a the main composite?
    /// @return 'true' if we ate the main composite
    bool isMain() const { return _main; }

    /// Set the number of input ports for a composite
    /// @param value number of input ports
    void setNumInputs(uint32_t value) { _numInputPorts = value; }

    /// How many input ports does this composite have?
    /// @return number of input ports for the composite
    uint32_t getNumInputs() const { return _numInputPorts; }

    /// Set the number of output ports for a composite
    /// @param value number of output ports
    void setNumOutputs(uint32_t value) { _numOutputPorts = value; }

    /// How many output ports does this composite have?
    /// @return number of output ports for the composite
    uint32_t getNumOutputs() const { return _numOutputPorts; }

    /// Return the 'kind' of the Composite
    /// @return the name
    const std::string& getKind() const { return _kind; }

    /// Set the implicit composite operator instance. This should be called for a
    /// composite that is a main, thus have an implicit instance
    /// @param implicitInstance CompositeOperatorInstance
    void setImplicitInstance(const CompositeOperatorInstance& implicitInstance)
    {
        _implicitInstance = &implicitInstance;
    }

    /// Get the implicit composite operator instance.
    /// @return implicitInstance Implicit operator instance, or NULL if this composite is
    /// not a potential main
    const CompositeOperatorInstance* getImplicitInstance() const { return _implicitInstance; }

    /// Set spldoc comment for the definition
    /// @param comment @splDoc comment for this definition
    /// @param congestionPolicy congestion policy on queue full
    void setSplDocComment(const std::string& comment) { _splDocComment = comment; }

    /// Return the splDocComment for this definition
    /// @return saved comment
    const std::string& getSplDocComment() const { return _splDocComment; }

    /// Print the Composite information
    /// @param s output stream
    /// @return s
    std::ostream& print(std::ostream& s) const;

    /// Add an SPL annotation to this composite
    /// Ownership of the annotation object is taken
    /// @param annotation The annotation
    void addAnnotation(const SPLAnnotation& annotation) { _annotations.push_back(&annotation); }

    /// Access the set of annatations associated with this composite
    /// @return Returns the annotations
    const std::vector<const SPLAnnotation*> annotations() const { return _annotations; }

  private:
    std::string _name;      // oper.name
    std::string _namespace; // name.space
    std::string _kind;      // name.space::oper.name
    const SourceLocation* _start;
    const SourceLocation* _end;
    bool _main;          // true if this is the 'main' composite
    bool _potentialMain; // No I/O ports - might be main
    uint32_t _index;
    uint32_t _toolkitIndex;
    uint32_t _numInputPorts, _numOutputPorts;
    std::vector<Config*> _configs;
    std::vector<const SPLAnnotation*> _annotations;
    const CompositeOperatorInstance* _implicitInstance;
    std::string _splDocComment;
};
};

#endif /* SPL_COMPOSITE_DEFINITION_H */
