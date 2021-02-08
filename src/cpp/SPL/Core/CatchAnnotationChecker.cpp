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

#include <SPL/Core/CatchAnnotationChecker.h>

#include <SPL/CodeGen/CompositeDefinition.h>
#include <SPL/CodeGen/CompositeOperatorInstance.h>
#include <SPL/CodeGen/OperatorIR.h>
#include <SPL/CodeGen/SPLAnnotation.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/FrontEnd/FrontEndDriver.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/SysOut.h>

using namespace std;

namespace SPL {

const SPLSimpleAnnotation* findCatchAnnotation(const vector<const SPLAnnotation*>& annotations)
{
    const SPLSimpleAnnotation* catchAnnotation = NULL;
    for (vector<const SPLAnnotation*>::const_iterator it = annotations.begin(),
                                                      itEnd = annotations.end();
         it != itEnd; ++it) {
        const SPLAnnotation& annot = **it;
        if (annot.tag() == "catch") {
            catchAnnotation = &annot.as<SPLSimpleAnnotation>();
            break;
        }
    }
    return catchAnnotation;
}

bool isSourceOperator(const PrimitiveOperatorInstance& primitive)
{
    return primitive.numInputPorts() == 0;
}

bool CatchAnnotationChecker::visitPrimitive(const PrimitiveOperatorInstance& primitive)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting primitive " << primitive.getNames()[0],
           Debug::TraceCatchAnnotationChecker);

    const SPLSimpleAnnotation* catchAnnotation = findCatchAnnotation(primitive.annotations());
    if (catchAnnotation != NULL) {
        // Check if this is a source operator
        if (isSourceOperator(primitive)) {
            // Check if the catch specifies "none"
            const vector<SPLSimpleAnnotation::KeyValuePair*>& keyValues = catchAnnotation->values();
            for (vector<SPLSimpleAnnotation::KeyValuePair*>::const_iterator it = keyValues.begin(),
                                                                            itEnd = keyValues.end();
                 it != itEnd; ++it) {
                const SPLSimpleAnnotation::KeyValuePair& kv = **it;
                if (kv.key() == "exception") {
                    if (kv.value() != "\"none\"") {
                        SysOut::errorln(
                          SPL_CORE_ANNOTATION_NOT_ALLOWED_ON_OP("catch", primitive.getNames()[0]),
                          catchAnnotation->sourceLocation());
                    }
                    break;
                }
            }
        }
        return true;
    }
    return false;
}

void CatchAnnotationChecker::propagateAnnotation(const PrimitiveOperatorInstance& primitive,
                                                 const SPLSimpleAnnotation& annot)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": propagating annotation to primitive "
                               << primitive.getNames()[0],
           Debug::TraceCatchAnnotationChecker);

    if (visitPrimitive(primitive)) {
        // The primitive already has a catch annotation
        return;
    }
    if (isSourceOperator(primitive)) {
        SysOut::errorln(SPL_CORE_ANNOTATION_NOT_ALLOWED_ON_OP("catch", primitive.getNames()[0]),
                        annot.sourceLocation());
        return;
    }
    // Otherwise we can add this annotation to this operator. The annotation lives in a pool so we can share the pointer.
    primitive.addAnnotation(annot);
}

void CatchAnnotationChecker::propagateAnnotation(const CompositeOperatorInstance& composite,
                                                 const SPLSimpleAnnotation& annot)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": propagating annotation to composite "
                               << composite.getInstanceName(),
           Debug::TraceCatchAnnotationChecker);

    // A catch annotation is being propagated to this composite.
    // See if this composite operator already has a catch annotation
    const SPLSimpleAnnotation* catchAnnotation = findCatchAnnotation(composite.annotations());
    if (catchAnnotation == NULL) {
        // if not, propagate the one we were asked to propagate
        catchAnnotation = &annot;
    }

    // Propagate the catch to all primitives
    const vector<const PrimitiveOperatorInstance*>& primitives = composite.getPrimitiveInstances();
    for (vector<const PrimitiveOperatorInstance*>::const_iterator it = primitives.begin(),
                                                                  itEnd = primitives.end();
         it != itEnd; ++it) {
        const PrimitiveOperatorInstance& primitive = **it;
        propagateAnnotation(primitive, *catchAnnotation);
    }

    // Propagate to all nested composites
    const vector<const CompositeOperatorInstance*> composites = composite.getCompositeInstances();
    for (vector<const CompositeOperatorInstance*>::const_iterator it1 = composites.begin(),
                                                                  it1End = composites.end();
         it1 != it1End; ++it1) {
        const CompositeOperatorInstance& comp = **it1;
        propagateAnnotation(comp, *catchAnnotation);
    }
}

void CatchAnnotationChecker::visitComposite(const CompositeOperatorInstance& composite)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting composite " << composite.getInstanceName(),
           Debug::TraceCatchAnnotationChecker);

    // See if this composite operator has a catch annotation
    const SPLSimpleAnnotation* catchAnnotation = findCatchAnnotation(composite.annotations());

    // Visit all the contained composites
    const vector<const CompositeOperatorInstance*> composites = composite.getCompositeInstances();
    for (vector<const CompositeOperatorInstance*>::const_iterator it1 = composites.begin();
         it1 != composites.end(); ++it1) {
        const CompositeOperatorInstance& comp = **it1;
        if (catchAnnotation != NULL) {
            propagateAnnotation(comp, *catchAnnotation);
        } else {
            visitComposite(comp);
        }
    }

    // Visit all the primitives to see if they are sources with a catch annotation
    const vector<const PrimitiveOperatorInstance*>& primitives = composite.getPrimitiveInstances();
    for (vector<const PrimitiveOperatorInstance*>::const_iterator it2 = primitives.begin();
         it2 != primitives.end(); ++it2) {
        const PrimitiveOperatorInstance& primitive = **it2;
        if (catchAnnotation != NULL) {
            propagateAnnotation(primitive, *catchAnnotation);
        } else {
            visitPrimitive(primitive);
        }
    }
}

void CatchAnnotationChecker::check(const OperatorGraph& graph)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceCatchAnnotationChecker);

    CompositeDefinition const* mainComposite = FrontEndDriver::instance().getMainComposite();
    if (NULL == mainComposite)
        return;

    CompositeOperatorInstance const* compOperInstanceIR = mainComposite->getImplicitInstance();
    assert(NULL != compOperInstanceIR);

    visitComposite(*compOperInstanceIR);
}

} // namespace SPL
