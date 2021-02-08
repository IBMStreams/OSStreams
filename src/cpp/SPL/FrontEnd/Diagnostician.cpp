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

#include <SPL/FrontEnd/Diagnostician.h> // Include this header first

#include <SPL/FrontEnd/AstVisitorPushPop.h>
#include <SPL/FrontEnd/FrontEndErrors.h>
#include <SPL/FrontEnd/SourceLocationFactory.h>
#include <SPL/FrontEnd/SupportedAnnotations.h>
#include <SPL/FrontEnd/TreeHelper.h>
#include <SPL/Utility/Debug.h>

using namespace std;

// TODO: The Diagnostician has been partly converted to use context objects located on the stack.
// Conversion will continue post-Concord.
// TODO: Move the Diagnostition phase to a later pass post-Concord.

namespace std {
namespace tr1 {

size_t hash<struct SPL::SetEntry>::operator()(const struct SPL::SetEntry& s) const
{
    std::tr1::hash<std::string> stringHash;
    return stringHash(s._formalName);
}

}
}

namespace SPL {

namespace DiagnosticContext {

DiagnosticianContext::DiagnosticianContext(Diagnostician& diag, Kind k)
  : _diag(diag)
  , _kind(k)
  , _previousContext(diag.setContext(this))
{}

DiagnosticianContext::~DiagnosticianContext()
{
    _diag.setContext(_previousContext);
}

StartContext::StartContext(Diagnostician& diag)
  : DiagnosticianContext(diag, DiagnosticianContext::Start)
{}

void AnnotationsContext::annotationSeen(const SupportedAnnotation& annot, const SourceLocation& loc)
{
    // Find out if we've already seen this annotation.  If not, add it to the set of seen
    // annotations, otherwise increment the count and check cardinality constraints
    unsigned count = 1;

    AnnotationsSeen::iterator it = _annotationsSeen.find(annot.name());
    if (it != _annotationsSeen.end()) {
        ++it->second;
        count = it->second;
    } else {
        _annotationsSeen.insert(make_pair(annot.name(), count));
    }

    // Ensure that we have not seen too many annotations
    if (count > annot.maxOccur()) {
        SysOut::errorln(SPL_FRONT_END_TOO_MANY_ANNOTATIONS(annot.name(), annot.maxOccur()), loc);
    }
}

AnnotationContext::AnnotationContext(Diagnostician& diag,
                                     const SupportedAnnotation& annot,
                                     const SourceLocation& loc,
                                     AnnotationsContext& ac)
  : DiagnosticianContext(diag, DiagnosticianContext::Annotation)
  , _annot(annot)
  , _loc(loc)
  , _annotationsContext(ac)
{}

} // namespace DiagnosticContext

void Diagnostician::run(ParserContext& pCtx, AstNode& ast)
{
    Diagnostician diag(pCtx);
    diag.visit(ast);
    if (diag.getFatalError()) {
        setAstStage(AstPool::instance(), ast, FrontEndStage::ERROR);
    }
}

Diagnostician::Diagnostician(ParserContext& pCtx)
  : AstVisitor(Debug::TraceDiagnostician)
  , _pCtx(pCtx)
  , _currentContext(NULL)
  , _startContext(*this)
  , _functionPrototype(NULL)
  , _fatalError(false)
{
    assert(NULL != _currentContext && _currentContext->is<DiagnosticContext::StartContext>());
    _state.push(Start);
}

void Diagnostician::visit(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": ast type: " << astType(ast) << ": spelling: " << astText(ast)
                               << " : current stage: " << astStage(ast).toString(),
           debugFlag());
    if (astStage(ast) < FrontEndStage::DIAGNOSTICIAN) {
        setAstStage(AstPool::instance(), ast, FrontEndStage::DIAGNOSTICIAN);
        AstVisitor::visit(ast);
    }
}

void Diagnostician::visitCompositeHead(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, debugFlag());
    DiagnosticContext::CompositeHeadContext context(*this);
    allChildrenVisit(ast);
}

void Diagnostician::visitFunctionPrototype(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, debugFlag());

    _state.push(FunctionPrototype);

    // Initialize the members for this visit
    _functionPrototype = &ast;

    AstNode& typeFormals = AST::functionPrototype::typeFormals(ast);
    visit(typeFormals);

    AstNode& boundsFormals = AST::functionPrototype::boundsFormals(ast);
    _state.push(BoundsFormalDeclaration);
    visit(boundsFormals);
    _state.pop();

    AstNode& protoFormals = AST::functionPrototype::protoFormals(ast);
    visit(protoFormals);

    ostringstream ss;
    assert(NULL != _functionPrototype);

    // Check each type formal to ensure it has been used in a parameter
    // otherwise the compiler doesn't know what type to bind to it.
    for (Iter it = _typeFormalIds.begin(); it != _typeFormalIds.end(); ++it) {
        const SetEntry& entry = *it;
        if (_typeIdsUsed.find(entry._formalName) == _typeIdsUsed.end()) {
            if (0 == ss.str().size()) {
                prettyPrint(ss, *_functionPrototype);
            }
            SysOut::errorln(
              SPL_FRONT_END_TYPE_FORMAL_MISSING_IN_PROTO_FORMAL(entry._formalName, ss.str()),
              *entry._loc);
        }
    }
    // Check each bounds formal for use
    for (Iter it = _boundsFormalIds.begin(); it != _boundsFormalIds.end(); ++it) {
        const SetEntry& entry = *it;
        if (_boundsIdsUsed.find(entry._formalName) == _boundsIdsUsed.end()) {
            if (0 == ss.str().size()) {
                prettyPrint(ss, *_functionPrototype);
            }
            SysOut::errorln(
              SPL_FRONT_END_BOUNDS_FORMAL_MISSING_IN_PROTO_FORMAL(entry._formalName, ss.str()),
              *entry._loc);
        }
    }

    // Clean up after use
    _functionPrototype = NULL;
    _typeFormalIds.clear();
    _boundsFormalIds.clear();
    _typeIdsUsed.clear();
    _boundsIdsUsed.clear();

    _state.pop();
}

void Diagnostician::visitTypeFormal(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, debugFlag());
    AstNode& id = AST::typeFormal::id(ast);
    _state.push(TypeFormalDeclarationId);
    visitId(id);
    _state.pop();
}

void Diagnostician::visitProtoFormal(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, debugFlag());

    AstNode& type = AST::protoFormal::type(ast);
    _state.push(FunctionFormalType);
    visit(type);
    _state.pop();
}

void Diagnostician::visitPortInputs(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, debugFlag());
    allChildrenVisit(ast);
    if (AST::portInputs::has_alias(ast)) {
        // Ensure the alias doesn't have the same name as an input stream
        AstNode& aliasAst = AST::as::id(AST::portInputs::alias(ast));
        const SourceLocation& loc =
          SourceLocationFactory::instance().create(_pCtx, aliasAst, false);
        const string aliasName = astText(aliasAst);
        for (uint32_t i = 0, ui = AST::portInputs::idCount(ast); i < ui; i++) {
            AstNode& streamAst = AST::portInputs::id(ast, i);
            if (astText(streamAst) == aliasName) {
                SysOut::errorln(SPL_FRONT_END_INPUT_ALIAS_REDEFINITION(aliasName), loc);
                setFatalError();
            }
        }
    }
}

void Diagnostician::visitId(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, debugFlag());

    switch (state()) {
        case TypeFormalDeclarationId:
            captureTypeFormalDeclaration(ast);
            break;
        case BoundsFormalDeclaration:
            captureBoundsFormalDeclaration(ast);
            break;
        case FunctionFormalTypeDims:
            captureFunctionFormalTypeDims(ast);
            break;
        default:
            break;
    }
}

void Diagnostician::visitTypeName(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, debugFlag());
    string typeName = astText(AST::typeName::contents(ast));
    if (FunctionFormalType == state()) {
        if (_typeIdsUsed.find(typeName) == _typeIdsUsed.end()) {
            _typeIdsUsed.insert(typeName);
        }
    }
}

void Diagnostician::visitTypeDims(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, debugFlag());
    if (FunctionFormalType == state()) {
        _state.push(FunctionFormalTypeDims);
        allChildrenVisit(ast);
        _state.pop();
    }
}

void Diagnostician::captureFunctionFormalTypeDims(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, debugFlag());
    assert(AST::idT::is(ast));
    string boundId = astText(ast);

    if (_boundsIdsUsed.find(boundId) == _boundsIdsUsed.end()) {
        _boundsIdsUsed.insert(boundId);
    }
}

void Diagnostician::captureBoundsFormalDeclaration(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, debugFlag());

    assert(AST::idT::is(ast));

    string boundsName = astText(ast);
    const SourceLocation& loc = SourceLocationFactory::instance().create(_pCtx, ast, false);
    SetEntry e(boundsName, loc);
    if (_boundsFormalIds.find(e) != _boundsFormalIds.end()) {
        ostringstream ss;
        assert(NULL != _functionPrototype);
        prettyPrint(ss, *_functionPrototype);
        SysOut::errorln(SPL_FRONT_END_BOUNDS_FORMAL_REDEFINITION(boundsName, ss.str()), loc);
    } else {
        _boundsFormalIds.insert(e);
    }
}

void Diagnostician::captureTypeFormalDeclaration(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, debugFlag());

    assert(AST::idT::is(ast));

    string formalName = astText(ast);
    // Check if the type formal id already exists
    const SourceLocation& loc = SourceLocationFactory::instance().create(_pCtx, ast, false);
    SetEntry e(formalName, loc);
    if (_typeFormalIds.find(e) == _typeFormalIds.end()) {
        _typeFormalIds.insert(e);
    } else {
        ostringstream ss;
        assert(NULL != _functionPrototype);
        prettyPrint(ss, *_functionPrototype);
        SysOut::errorln(SPL_FRONT_END_TYPE_FORMAL_REDEFINITION(formalName, ss.str()), loc);
    }
}

void Diagnostician::visitAnnotations(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, debugFlag());
    DiagnosticContext::AnnotationsContext context(*this);
    for (int i = 0, n = astSize(ast); i < n; i++) {
        visit(astChild(ast, i));
        bool error = context.readAndResetErrorState();
        if (error) {
            AstNode& errorNode = AST::errorT::create();
            resetAstChild(AstPool::instance(), ast, i, errorNode);
        }
    }
}

bool Diagnostician::inCompositeHeadContext() const
{
    const DiagnosticContext::DiagnosticianContext* context = &currentContext();

    while (!context->is<DiagnosticContext::StartContext>()) {
        if (context->is<DiagnosticContext::CompositeHeadContext>()) {
            return true;
        }
        context = &context->previousContext();
    }
    return false;
}

void Diagnostician::visitAnnotation(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, debugFlag());
    AstNode& id = AST::annotation::id(ast);

    assert(currentContext().is<DiagnosticContext::AnnotationsContext>());
    DiagnosticContext::AnnotationsContext& annotationsContext =
      currentContext().as<DiagnosticContext::AnnotationsContext>();

    string name = astText(id);
    // Is this a recognized annotation?
    const SupportedAnnotation* annot = SupportedAnnotations::instance().find(name);
    const SourceLocation& loc = SourceLocationFactory::instance().create(_pCtx, ast, false);
    if (NULL == annot && name.find("spl__") == 0) {
        annot = SupportedAnnotations::instance().add("spl__", -1, false);
    }

    if (NULL == annot) {
        SysOut::errorln(SPL_FRONT_END_UNSUPPORTED_ANNOTATION(name), loc);
        annotationsContext.setError();
    } else {
        annotationsContext.annotationSeen(*annot, loc);

        // TODO post spree: generalize the mechanism to see if the annotation can appear in the
        // context that it is found in If this is an annotation being applied to a composite
        // definition, emit an error if that is not allowed
        if ((name == "parallel" || name == "view" || name == "consistent" || name == "autonomous" ||
             name == "catch") &&
            inCompositeHeadContext()) {
            SysOut::errorln(SPL_FRONT_END_ANNOT_ON_COMPOSITE_DEF(name), loc);
            setFatalError();
        }

        DiagnosticContext::AnnotationContext context(*this, *annot, loc, annotationsContext);
        if (AST::annotation::has_tupleLiteral(ast)) {
            AstNode& tail = AST::annotation::tupleLiteral(ast);
            visit(tail);
        }

        // Make sure that all the keys required were specified
        const DiagnosticContext::AnnotationContext::KeysSeen& keysSeen = context.keysSeen();
        SupportedAnnotation::Keys::const_iterator it;
        for (it = annot->keys().begin(); it != annot->keys().end(); ++it) {
            const SupportedAnnotation::Key& key = **it;
            if (key.required()) {
                DiagnosticContext::AnnotationContext::KeysSeen::const_iterator it1 =
                  keysSeen.find(key.name());
                if (keysSeen.end() == it1) {
                    SysOut::errorln(SPL_FRONT_END_ANNOTATION_MISSING_KEY(name, key.name()), loc);
                    annotationsContext.setError();
                }
            }
        }
    }
}

void Diagnostician::visitCallExpr(AstNode& ast)
{
    if (currentContext().is<DiagnosticContext::AnnotationValueContext>()) {
        DiagnosticContext::AnnotationValueContext& context =
          currentContext().as<DiagnosticContext::AnnotationValueContext>();
        const DiagnosticContext::AnnotationContext& annotContext =
          context.previousContext().as<DiagnosticContext::AnnotationContext>();
        AstNode& target = AST::callExpr::target(ast);
        string targetStr = astText(target);
        if (targetStr != "getSubmissionTimeValue" && targetStr != "getCompileTimeValue") {
            SysOut::errorln(SPL_FRONT_END_INVALID_FUNCTION_IN_ANNOTATION(targetStr),
                            annotContext.location());
        } else {
            if (targetStr == "getSubmissionTimeValue" &&
                !context.key().submissionTimeValueAllowed()) {
                SysOut::errorln(SPL_FRONT_END_ANNOT_NO_SUB_TIME(context.key().name(),
                                                                annotContext.annotation().name()),
                                annotContext.location());
            }
        }
    }
}

void Diagnostician::visitAttributeAssign(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, debugFlag());
    if (currentContext().is<DiagnosticContext::AnnotationContext>()) {
        DiagnosticContext::AnnotationContext& context =
          currentContext().as<DiagnosticContext::AnnotationContext>();
        const SupportedAnnotation& annot = context.annotation();
        string keyName = astText(AST::attributeAssign::id(ast));
        const SupportedAnnotation::Key* key = annot.findKey(keyName);

        if (NULL == key) {
            if (annot.name().find("spl__") == 0) {
                SupportedAnnotations::instance().addKey(annot.name(), false, false);
            } else {
                SysOut::errorln(SPL_FRONT_END_ANNOTATION_UNSUPPORTED_KEY(annot.name(), keyName),
                                context.location());
                context.annotationsContext().setError();
            }
        }
        // Keys can only be used once.  Have we seen this one already?
        DiagnosticContext::AnnotationContext::KeysSeen& keysSeen = context.keysSeen();
        if (keysSeen.find(keyName) != keysSeen.end()) {
            SysOut::errorln(SPL_FRONT_END_ANNOTATION_DUPLICATE_KEY(keyName, annot.name()),
                            context.location());
            context.annotationsContext().setError();
        } else {
            keysSeen.insert(keyName);
        }
        {
            DiagnosticContext::AnnotationValueContext avc(*this, *key);
            // Visit the value expression
            AstNode& expr = AST::attributeAssign::expr(ast);
            visit(expr);
        }
    }
}

} // namespace SPL
