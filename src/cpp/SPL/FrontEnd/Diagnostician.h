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

#ifndef SPL_DIAGNOSTICIAN_H
#define SPL_DIAGNOSTICIAN_H

#include <SPL/FrontEnd/AstVisitor.h>
#include <SPL/FrontEnd/SupportedAnnotations.h>
#include <UTILS/HashMapHelpers.h>

#include <cassert>
#include <stack>
#include <string>

namespace SPL {
struct SetEntry;
} // namespace SPL

namespace std {
namespace tr1 {
template<>
struct hash<struct SPL::SetEntry>
{
    size_t operator()(const struct SPL::SetEntry& s) const;
};
}
}

namespace SPL {
class ParserContext;
class Diagnostician;
class SourceLocation;

struct SetEntry
{
    SetEntry(const std::string& formalName, const SourceLocation& loc)
      : _formalName(formalName)
      , _loc(&loc)
    {}
    std::string _formalName;
    const SourceLocation* _loc;
    bool operator==(const SetEntry& other) const { return _formalName == other._formalName; }
};

namespace DiagnosticContext {

class DiagnosticianContext
{
  public:
    enum Kind
    {
        Start,
        Annotation,
        AnnotationValue,
        Annotations,
        CompositeHead,
        numContextKind
    };

    Kind getKind() const { return _kind; }

    /// Declares the primary template for a method to query if the object is one of a specific kind.
    /// @return Returns true if the object is of the specific kind, otherwise false.
    template<class T>
    bool is() const;

    /// Declares the primary template for a const downcaster
    /// Before the downcaster is used you must verify that the object is of the desired type using
    /// the as method
    template<class T>
    const T& as() const;

    /// Declares the primary template for a non-const downcaster
    /// Before the downcaster is used you must verify that the object is of the desired type using
    /// the as method
    template<class T>
    T& as();

    const DiagnosticianContext& previousContext() const
    {
        assert(NULL != _previousContext);
        return *_previousContext;
    }

    virtual void print() const = 0;

  protected:
    DiagnosticianContext(Diagnostician& diag, Kind k);
    virtual ~DiagnosticianContext();

  private:
    Diagnostician& _diag;
    Kind _kind;
    DiagnosticianContext* _previousContext;
};

#define DC_IS_AS_SPECIALIZATIONS(isEnum, asType)                                                   \
    template<>                                                                                     \
    inline bool DiagnosticianContext::is<asType>() const                                           \
    {                                                                                              \
        return getKind() == DiagnosticianContext::isEnum;                                          \
    }                                                                                              \
    template<>                                                                                     \
    inline asType& DiagnosticianContext::as<asType>()                                              \
    {                                                                                              \
        assert(dynamic_cast<asType*>(this) != 0);                                                  \
        return *static_cast<asType*>(this);                                                        \
    }                                                                                              \
    template<>                                                                                     \
    inline const asType& DiagnosticianContext::as<asType>() const                                  \
    {                                                                                              \
        assert(dynamic_cast<const asType*>(this) != 0);                                            \
        return *static_cast<const asType*>(this);                                                  \
    }

class StartContext : public DiagnosticianContext
{
  public:
    StartContext(Diagnostician& diag);
    virtual void print() const { std::cerr << "Start" << std::endl; }

  private:
};
DC_IS_AS_SPECIALIZATIONS(Start, StartContext)

class AnnotationsContext : public DiagnosticianContext
{
  public:
    AnnotationsContext(Diagnostician& diag)
      : DiagnosticianContext(diag, DiagnosticianContext::Annotations)
      , _error(false)
    {}
    virtual void print() const { std::cerr << "Annotations" << std::endl; }
    void annotationSeen(const SupportedAnnotation& annot, const SourceLocation& loc);
    bool readAndResetErrorState()
    {
        bool error = _error;
        _error = false;
        return error;
    }
    void setError() { _error = true; }

  private:
    typedef std::tr1::unordered_map<std::string, unsigned> AnnotationsSeen;
    AnnotationsSeen _annotationsSeen;
    bool _error;
};
DC_IS_AS_SPECIALIZATIONS(Annotations, AnnotationsContext)

class AnnotationContext : public DiagnosticianContext
{
  public:
    AnnotationContext(Diagnostician& diag,
                      const SupportedAnnotation& annot,
                      const SourceLocation& loc,
                      AnnotationsContext& ac);
    AnnotationsContext& annotationsContext() const { return _annotationsContext; }
    const SupportedAnnotation& annotation() const { return _annot; }
    virtual void print() const { std::cerr << "Annotation" << std::endl; }
    const SourceLocation& location() const { return _loc; }

    typedef std::tr1::unordered_set<std::string> KeysSeen;
    KeysSeen& keysSeen() { return _keysSeen; }

  private:
    const SupportedAnnotation& _annot;
    const SourceLocation& _loc;
    AnnotationsContext& _annotationsContext;
    KeysSeen _keysSeen;
};
DC_IS_AS_SPECIALIZATIONS(Annotation, AnnotationContext)

class AnnotationValueContext : public DiagnosticianContext
{
  public:
    AnnotationValueContext(Diagnostician& diag, const SupportedAnnotation::Key& key_)
      : DiagnosticianContext(diag, DiagnosticianContext::AnnotationValue)
      , _key(key_)
    {}
    virtual void print() const { std::cerr << "AnnotationValue" << std::endl; }
    const SupportedAnnotation::Key& key() const { return _key; }

  private:
    const SupportedAnnotation::Key& _key;
};
DC_IS_AS_SPECIALIZATIONS(AnnotationValue, AnnotationValueContext)

class CompositeHeadContext : public DiagnosticianContext
{
  public:
    CompositeHeadContext(Diagnostician& diag)
      : DiagnosticianContext(diag, DiagnosticianContext::CompositeHead)
    {}
    virtual void print() const { std::cerr << "CompositeHead" << std::endl; }
};
DC_IS_AS_SPECIALIZATIONS(CompositeHead, CompositeHeadContext)

} // namespace DiagnosticContext

/// This class is intended as a compile phase who's purpose is to detect and
/// diagnose problems in the AST.  This class will be extended over time as diagnostic
/// checks are moved in.
class Diagnostician : public AstVisitor
{
    enum State
    {
        Start,
        FunctionPrototype,
        TypeFormalDeclarationId,
        BoundsFormalDeclaration,
        FunctionFormalType,
        FunctionFormalTypeDims,
        numValues
    };

  public:
    static void run(ParserContext& pCtx, AstNode& ast);

    DiagnosticContext::DiagnosticianContext* setContext(
      DiagnosticContext::DiagnosticianContext* context)
    {
        DiagnosticContext::DiagnosticianContext* prev = _currentContext;
        _currentContext = context;
        return prev;
    }

  protected:
    const DiagnosticContext::DiagnosticianContext& currentContext() const
    {
        assert(NULL != _currentContext);
        return *_currentContext;
    }
    DiagnosticContext::DiagnosticianContext& currentContext()
    {
        assert(NULL != _currentContext);
        return *_currentContext;
    }

    virtual void visit(AstNode& ast);
    virtual void visitFunctionPrototype(AstNode& ast);
    virtual void visitTypeName(AstNode& ast);
    virtual void visitId(AstNode& ast);
    virtual void visitTypeDims(AstNode& ast);
    virtual void visitPortInputs(AstNode&);
    virtual void visitProtoFormal(AstNode& ast);
    virtual void visitTypeFormal(AstNode&);
    virtual void visitAnnotations(AstNode& ast);
    virtual void visitAnnotation(AstNode& ast);
    virtual void visitAttributeAssign(AstNode& ast);
    virtual void visitCallExpr(AstNode& ast);
    virtual void visitCompositeHead(AstNode& ast);

  private:
    Diagnostician(ParserContext& pCtx);
    void captureTypeFormalDeclaration(AstNode& ast);
    void captureBoundsFormalDeclaration(AstNode& ast);
    void captureFunctionFormalTypeDims(AstNode& ast);
    void setFatalError() { _fatalError = true; }
    bool getFatalError() const { return _fatalError; }
    bool inCompositeHeadContext() const;

    State state() const { return _state.top(); }

    typedef std::tr1::unordered_set<SetEntry> MySet;
    typedef MySet::const_iterator Iter;

    ParserContext& _pCtx;
    std::stack<State> _state;
    MySet _typeFormalIds;
    MySet _boundsFormalIds;
    std::tr1::unordered_set<std::string> _typeIdsUsed;
    std::tr1::unordered_set<std::string> _boundsIdsUsed;
    DiagnosticContext::DiagnosticianContext* _currentContext;
    DiagnosticContext::StartContext _startContext;

    AstNode* _functionPrototype;
    bool _fatalError;
};

} // namespace SPL

#endif
