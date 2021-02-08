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

#include <SPL/CodeGen/CompositeOperatorInstance.h>

#include <SPL/CodeGen/CompositeDefinition.h>
#include <SPL/CodeGen/OperatorIR.h>
#include <SPL/CodeGen/SPLAnnotation.h>
#include <SPL/FrontEnd/UsedCompositeDefinitions.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/ModelDumper.h>
#include <SPL/Utility/SourceLocation.h>

using namespace std;

namespace SPL {

CompositeOperatorInstance::CompositeOperatorInstance(uint32_t compositeIndex,
                                                     const CompositeOperatorInstance* inparent)
  : _index(compositeIndex)
  , _parent(inparent)
  , _parallelAnnotation(NULL)
{
    _startLocation = &SourceLocation::invalidSourceLocation;
    _endLocation = &SourceLocation::invalidSourceLocation;
    MODEL("CompositeOperatorInstance(" << (void*)this << ", " << (void*)inparent << ")\n");
}

void CompositeOperatorInstance::addAnnotation(const SPLAnnotation& annotation) const
{
    if (annotation.is<SPLParallelAnnotation>()) {
        assert(NULL == _parallelAnnotation);
        _parallelAnnotation = &annotation.as<SPLParallelAnnotation>();
    } else {
        if (annotation.tag() == "view") {
            _viewAnnotations.push_back(&annotation);
        } else {
            _annotations.push_back(&annotation);
        }
    }
}

string CompositeOperatorInstance::parallelRegionName() const
{
    if (NULL != _parallelAnnotation) {
        return getInstanceName();
    }
    // Otherwise this composite instance must be nested in another composite instance
    assert(NULL != _parent);
    return _parent->parallelRegionName();
}

const SPLParallelAnnotation& CompositeOperatorInstance::getParallelAnnotation() const
{
    if (NULL != _parallelAnnotation) {
        return *_parallelAnnotation;
    }
    assert(NULL != _parent);
    return _parent->getParallelAnnotation();
}

uint32_t CompositeOperatorInstance::parallelNestingLevel() const
{
    uint32_t level = 0;
    if (isInParallelRegion()) {
        if (NULL != _parent) {
            level = _parent->parallelNestingLevel();
        }
        if (NULL != _parallelAnnotation) {
            ++level;
        }
    }
    return level;
}

uint32_t CompositeOperatorInstance::parallelNestingLevel(
  const CompositeOperatorInstance& introducingComp) const
{
    if (this == &introducingComp) {
        return parallelNestingLevel();
    }
    assert(NULL != _parent);
    return _parent->parallelNestingLevel(introducingComp);
}

bool CompositeOperatorInstance::isInParallelRegion() const
{
    if (NULL != _parallelAnnotation) {
        return true;
    }
    if (NULL != _parent) {
        return _parent->isInParallelRegion();
    }
    return false;
}

const CompositeDefinition& CompositeOperatorInstance::composite() const
{
    return UsedCompositeDefinitions::instance()[_index];
}

void CompositeOperatorInstance::addPrimitiveInstance(const PrimitiveOperatorInstance& op)
{
    _primitives.push_back(&op);
    MODEL("AddPrimitiveInstance(" << (void*)this << ", " << (void*)&op << ")\n");
}

CompositeOperatorInstance::~CompositeOperatorInstance()
{
    vector<Config*>::const_iterator it;
    for (it = _configs.begin(); it != _configs.end(); ++it) {
        delete *it;
    }

    vector<const CompositeOperatorInstance*>::const_iterator it2;
    for (it2 = _nestedCompOperInstances.begin(); it2 != _nestedCompOperInstances.end(); ++it2) {
        delete *it2;
    }
}

CompositeOperatorInstance& CompositeOperatorInstance::createNestedCompOperInstance(
  uint32_t compositeIndex)
{
    CompositeOperatorInstance* n = new CompositeOperatorInstance(compositeIndex, this);
    _nestedCompOperInstances.push_back(n);
    return *n;
}

ostream& CompositeOperatorInstance::print(ostream& s) const
{
    const UsedCompositeDefinitions& cdt = UsedCompositeDefinitions::instance();
    const CompositeDefinition& c = cdt[_index];
    s << "CompositeOperatorInstance <" << this << ">: " << c.getKind() << ", parent<" << _parent
      << ">\n";
    if (!_primitives.empty()) {
        for (uint32_t i = 0; i < _primitives.size(); i++) {
            s << "\tcontains primitive op ";
            const vector<string>& names = _primitives[i]->getNames();
            for (uint32_t j = 0; j < names.size(); j++) {
                if (j > 0) {
                    s << '/';
                }
                s << names[j];
            }
            s << '\n';
        }
    }
    for (uint32_t i = 0; i < _configs.size(); i++) {
        s << '\t' << *_configs[i] << '\n';
    }

    // Dump the stream information
    if (!_inputStreams.empty()) {
        s << "Input Streams:\n";
        for (uint32_t i = 0; i < _inputStreams.size(); i++) {
            s << '\t' << i << ": ";
            for (uint32_t j = 0; j < _inputStreams[i].size(); j++) {
                s << _inputStreams[i][j] << ' ';
            }
            s << '\n';
        }
    }

    if (!_inputDefinitionStreams.empty()) {
        s << "Inside Input Streams:\n";
        for (uint32_t i = 0; i < _inputDefinitionStreams.size(); i++) {
            s << '\t' << i << ": " << _inputDefinitionStreams[i] << '\n';
        }
    }

    if (!_outputStreams.empty()) {
        s << "Output Streams:\n";
        for (uint32_t i = 0; i < _outputStreams.size(); i++) {
            s << '\t' << i << ": " << _outputStreams[i] << '\n';
        }
    }

    if (!_outputDefinitionStreams.empty()) {
        s << "Inside Output Streams:\n";
        for (uint32_t i = 0; i < _outputDefinitionStreams.size(); i++) {
            s << '\t' << i << ": " << _outputDefinitionStreams[i] << '\n';
        }
    }

    return s;
}

const Config* CompositeOperatorInstance::find(Config::Kind kind) const
{
    // Do we contain it through an instance?
    const Config* c = findInstance(kind);
    if (c) {
        return c;
    }

    // No, search the definitions
    return findDefinition(kind);
}

const Config* CompositeOperatorInstance::findInstance(Config::Kind kind) const
{
    // Do we contain this config information?
    Config* c = Config::findConfig(kind, _configs);
    if (c) {
        return c;
    }

    // No, try our parent
    if (_parent) {
        return _parent->findInstance(kind);
    }
    return NULL;
}

const Config* CompositeOperatorInstance::findDefinition(Config::Kind kind) const
{
    // Do we contain this config information?
    const Config* c = UsedCompositeDefinitions::instance()[_index].find(kind);
    if (c) {
        return c;
    }
    // No, try our parent
    if (_parent) {
        return _parent->findDefinition(kind);
    }
    return NULL;
}

const Config* CompositeOperatorInstance::find(PlacementConstraint::PlacementSubKind subKind) const
{
    // Do we contain it through an instance?
    const Config* c = findInstance(subKind);
    if (c) {
        return c;
    }

    // No, search the definitions
    return findDefinition(subKind);
}

const Config* CompositeOperatorInstance::findInstance(
  PlacementConstraint::PlacementSubKind subKind) const
{
    // Do we contain this config information?
    Config* c = Config::findConfig(subKind, _configs);
    if (c) {
        return c;
    }

    // No, try our parent
    if (_parent) {
        return _parent->findInstance(subKind);
    }
    return NULL;
}

const Config* CompositeOperatorInstance::findDefinition(
  PlacementConstraint::PlacementSubKind subKind) const
{
    // Do we contain this config information?
    const Config* c = UsedCompositeDefinitions::instance()[_index].find(subKind);
    if (c) {
        return c;
    }
    // No, try our parent
    if (_parent) {
        return _parent->findDefinition(subKind);
    }
    return NULL;
}
} // namespace SPL
