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

#include <SPL/Core/ViewAnnotationChecker.h>

#include <SPL/CodeGen/CompositeDefinition.h>
#include <SPL/CodeGen/CompositeOperatorInstance.h>
#include <SPL/CodeGen/OperatorIR.h>
#include <SPL/CodeGen/SPLAnnotation.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/OperatorGraph.h>
#include <SPL/Core/OperatorGraphNode.h>
#include <SPL/FrontEnd/FrontEndDriver.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/FrontEnd/TypeFactory.h>
#include <SPL/Utility/SysOut.h>

#include <string>
#include <vector>

using namespace std;
namespace SPL {

void ViewAnnotationChecker::check(const OperatorGraph& graph)
{
    CompositeDefinition const* mainComposite = FrontEndDriver::instance().getMainComposite();
    if (NULL == mainComposite)
        return;
    CompositeOperatorInstance const* compOperInstanceIR = mainComposite->getImplicitInstance();
    assert(NULL != compOperInstanceIR);

    // Check for port names on annotations associated with primitives.  We should do this
    // prior to checking the ones on composites becuase we can synthesize them correctly.
    checkViewAnnotationsOnPrimitives(graph);

    checkForViewAnnotationsOnComposites(*compOperInstanceIR);
}

string unqualifiedStreamName(const string& streamName)
{
    size_t pos = streamName.find_last_of('.');
    if (pos != string::npos) {
        return streamName.substr(pos + 1);
    }
    return streamName;
}

void ViewAnnotationChecker::checkViewAnnotationsOnPrimitives(const OperatorGraph& graph)
{
    // Walk all the graph nodes looking for annotations.  Verify that the port name corresponds to the real port name and is not simply a
    // stream name
    uint32_t numNodes = graph.numNodes();
    for (uint32_t i = 0; i < numNodes; ++i) {
        const OperatorGraphNode& node = graph.getNode(i);
        PrimitiveOperatorInstance* ir = node.getOperatorIR();
        assert(NULL != ir);
        const vector<const SPLAnnotation*>& annotations = ir->annotations();
        for (vector<const SPLAnnotation*>::const_iterator it = annotations.begin(),
                                                          itEnd = annotations.end();
             it != itEnd; ++it) {
            const SPLAnnotation& annot = **it;
            if (annot.tag() == "view") {
                assert(annot.is<SPLViewAnnotation>());
                const SPLViewAnnotation& viewAnnot = annot.as<SPLViewAnnotation>();
                const vector<SPLSimpleAnnotation::KeyValuePair*>& kvps = viewAnnot.values();
                for (vector<SPLSimpleAnnotation::KeyValuePair*>::const_iterator it1 = kvps.begin(),
                                                                                it1End = kvps.end();
                     it1 != it1End; ++it1) {
                    const SPLSimpleAnnotation::KeyValuePair& kvp = **it1;
                    if (kvp.key() == "port") {
                        string portName = kvp.value().substr(1, kvp.value().size() - 2);
                        uint32_t numOutputPorts = ir->numOutputPorts();
                        for (uint32_t j = 0; j != numOutputPorts; ++j) {
                            const PrimitiveOperatorInstance::Port& port = ir->getOutputPort(j);
                            const string& streamName =
                              unqualifiedStreamName(port.getActualStreamName());
                            if (port.hasAlias() && portName == port.getAlias()) {
                                // We found the port referred to by the annotation and it has the correct name
                                break;
                            }
                            if (portName == streamName && port.hasAlias()) {
                                // We found the port referred to by the annotation but it has the stream name rather than the
                                // port name.  Fix that.
                                kvp.setValue(port.getAlias());
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
}

void projectViewAnnotation(const CompositeOperatorInstance& composite,
                           const string& streamName,
                           const SPLViewAnnotation& viewAnnot)
{
    // Check primitive operators before recursing
    const vector<const PrimitiveOperatorInstance*>& primitives = composite.getPrimitiveInstances();
    for (vector<const PrimitiveOperatorInstance*>::const_iterator it = primitives.begin(),
                                                                  itEnd = primitives.end();
         it != itEnd; ++it) {
        const PrimitiveOperatorInstance& primitive = **it;
        for (uint32_t i = 0, iEnd = primitive.numOutputPorts(); i != iEnd; ++i) {
            const PrimitiveOperatorInstance::Port& port = primitive.getOutputPort(i);
            // Check the output stream name for the one we are looking for
            if (port.getActualStreamName() == streamName) {
                const vector<SPLSimpleAnnotation::KeyValuePair*>& keyValues = viewAnnot.values();
                vector<SPLSimpleAnnotation::KeyValuePair*> newKVPs;
                for (vector<SPLSimpleAnnotation::KeyValuePair*>::const_iterator
                       it1 = keyValues.begin(),
                       it1End = keyValues.end();
                     it1 != it1End; ++it1) {
                    const SPLSimpleAnnotation::KeyValuePair& kvp = **it1;
                    SPLSimpleAnnotation::KeyValuePair* newKVP = NULL;
                    if (kvp.key() == "port") {
                        string pName = "\"" + port.getName() + "\"";
                        EnumTyp::Values values;
                        values.push_back(port.getName());
                        EnumTyp const& eType = TypeFactory::instance().enumT(values);
                        newKVP = new SPLSimpleAnnotation::KeyValuePair(
                          kvp.key(), pName, kvp.compositeName(), kvp.submissionTimeName(),
                          kvp.defaultValue(), eType, kvp.sourceLocation());
                    } else {
                        newKVP = new SPLSimpleAnnotation::KeyValuePair(kvp);
                    }
                    assert(NULL != newKVP);
                    newKVPs.push_back(newKVP);
                }
                SPLViewAnnotation& newAnnot = AnnotationFactory::createViewAnnotation(
                  newKVPs, viewAnnot.sourceLocation(), composite.getInstanceName());
                primitive.addAnnotation(newAnnot);
                return;
            }
        }
    }

    // Check the nested composites
    const vector<const CompositeOperatorInstance*>& composites = composite.getCompositeInstances();
    for (vector<const CompositeOperatorInstance*>::const_iterator it = composites.begin(),
                                                                  itEnd = composites.end();
         it != itEnd; ++it) {
        const CompositeOperatorInstance& comp = **it;
        const CompositeDefinition& compositeDef = comp.composite();
        for (uint32_t i = 0; i < compositeDef.getNumOutputs(); ++i) {
            const string& formalStreamName = comp.getOutputPortFormalStream(i);
            const string& actualStreamName = comp.getOutputPortActualStream(i);
            if (actualStreamName == streamName) {
                projectViewAnnotation(comp, formalStreamName, viewAnnot);
                return;
            }
        }
    }
}

void ViewAnnotationChecker::checkForViewAnnotationsOnComposites(
  const CompositeOperatorInstance& composite)
{
    // Check nested composites
    const vector<const CompositeOperatorInstance*>& composites = composite.getCompositeInstances();
    {
        for (vector<const CompositeOperatorInstance*>::const_iterator it = composites.begin(),
                                                                      itEnd = composites.end();
             it != itEnd; ++it) {
            const CompositeOperatorInstance& comp = **it;
            checkForViewAnnotationsOnComposites(comp);
        }
    }

    // Check this composite
    const vector<const SPLAnnotation*>& viewAnnotations = composite.viewAnnotations();
    for (vector<const SPLAnnotation*>::const_iterator it = viewAnnotations.begin(),
                                                      itEnd = viewAnnotations.end();
         it != itEnd; ++it) {
        const SPLAnnotation& annot = **it;
        const SPLViewAnnotation& viewAnnot = annot.as<SPLViewAnnotation>();
        // We have a view annotation. We need to project that onto the appropriate primitive operator
        const vector<SPLSimpleAnnotation::KeyValuePair*>& keyValues = viewAnnot.values();
        string portName;
        for (vector<SPLSimpleAnnotation::KeyValuePair*>::const_iterator it1 = keyValues.begin(),
                                                                        it1End = keyValues.end();
             it1 != it1End; ++it1) {
            const SPLSimpleAnnotation::KeyValuePair& kvp = **it1;
            if (kvp.key() == "port") {
                portName = kvp.value().substr(1, kvp.value().size() - 2);
                break;
            }
        }
        const CompositeDefinition& compositeDef = composite.composite();
        for (uint32_t i = 0; i < compositeDef.getNumOutputs(); ++i) {
            const string& formalStreamName = composite.getOutputPortFormalStream(i);
            const string& actualStreamName = composite.getOutputPortActualStream(i);
            const string& alias = composite.getOutputPortAlias(i);
            string parentName;
            const CompositeOperatorInstance* parent = composite.parent();
            if (NULL != parent && parent->parent() != NULL) {
                parentName = parent->getInstanceName();
            }
            string streamName = (parentName.empty())
                                  ? actualStreamName
                                  : actualStreamName.substr(parentName.length() + 1);
            if (streamName == portName || alias == portName) {
                // Project the view annotation onto the output port of the operator that feeds this output port
                projectViewAnnotation(composite, formalStreamName, viewAnnot);
                break;
            }
        }
    }
}

} // namespace SPL
