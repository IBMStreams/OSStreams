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

#ifndef SPL_NAME_DEF_FINDER_H
#define SPL_NAME_DEF_FINDER_H

#include <SPL/FrontEnd/AstVisitor.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>

#include <cassert>

namespace SPL {
class NameDefFinder;

namespace NameDefFinderContexts {

class NameDefFinderContext
{
  public:
    enum Kind
    {
        Start,
        Annotations,
        Dummy,              // Set in order to block actions on recursive calls
        Optional,           // Indicates that the child is in the context of an optional variable
        ParallelAnnotation, // set when processing a parallel annotation
        ParallelAnnotationPartitionBy, // set when processing the parallel annotations partitionBy
                                       // key
        SimpleAnnotation,     // Set when processing one of the simple, key/value, annotations, like
                              // spl_note
        ThreadingAnnotation,  // Set when processing the threading annotation
        ViewAnnotation,       // Set when processing a view annotation
        ViewAnnotationFilter, // Set when processing the filter attribute of a view annotation view
                              // annotation,
        EventTimeAnnotation,  // Set when processing an event-time annotation.
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

    const NameDefFinderContext& previousContext() const
    {
        assert(NULL != _previousContext);
        return *_previousContext;
    }

    virtual void print() const = 0;

  protected:
    NameDefFinderContext(NameDefFinder& finder, Kind k);
    virtual ~NameDefFinderContext();

  private:
    NameDefFinder& _finder;
    Kind _kind;
    NameDefFinderContext* _previousContext;
};
#define NDF_IS_AS_SPECIALIZATIONS(isEnum, asType)                                                  \
    template<>                                                                                     \
    inline bool NameDefFinderContext::is<asType>() const                                           \
    {                                                                                              \
        return getKind() == NameDefFinderContext::isEnum;                                          \
    }                                                                                              \
    template<>                                                                                     \
    inline asType& NameDefFinderContext::as<asType>()                                              \
    {                                                                                              \
        assert(dynamic_cast<asType*>(this) != 0);                                                  \
        return *static_cast<asType*>(this);                                                        \
    }                                                                                              \
    template<>                                                                                     \
    inline const asType& NameDefFinderContext::as<asType>() const                                  \
    {                                                                                              \
        assert(dynamic_cast<const asType*>(this) != 0);                                            \
        return *static_cast<const asType*>(this);                                                  \
    }

class StartContext : public NameDefFinderContext
{
  public:
    StartContext(NameDefFinder& finder);
    virtual void print() const { std::cerr << "Start" << std::endl; }

  private:
};
NDF_IS_AS_SPECIALIZATIONS(Start, StartContext)

class AnnotationsContext : public NameDefFinderContext
{
  public:
    AnnotationsContext(NameDefFinder& finder, AstNode& annotations, int child);
    virtual void print() const { std::cerr << "Annotations" << std::endl; }
    AstNode& node() const { return _node; }
    int childIndex() const { return _childIndex; }

  private:
    AstNode& _node;
    int _childIndex;
};
NDF_IS_AS_SPECIALIZATIONS(Annotations, AnnotationsContext)

class SimpleAnnotationContext : public NameDefFinderContext
{
  public:
    SimpleAnnotationContext(NameDefFinder& finder, SimpleAnnotationSym& sym_);
    virtual void print() const { std::cerr << "SimpleAnnotation" << std::endl; }
    SimpleAnnotationSym& sym() { return _sym; }

  private:
    SimpleAnnotationSym& _sym;
};
NDF_IS_AS_SPECIALIZATIONS(SimpleAnnotation, SimpleAnnotationContext)

class EventTimeAnnotationContext : public NameDefFinderContext
{
  public:
    EventTimeAnnotationContext(NameDefFinder& finder);

    virtual void print() const { std::cerr << "EventTimeAnnotation" << std::endl; }

    void setEventTimeAttribute(AstNode& ast) { attribute_ = &ast; }
    AstNode& eventTimeAttribute()
    {
        assert(attribute_);
        return *attribute_;
    }
    void setResolution(AstNode& ast) { resolution_ = &ast; }
    AstNode* resolution() { return resolution_; }
    void setMinimumGap(AstNode& ast) { minimumGap_ = &ast; }
    AstNode* minimumGap() { return minimumGap_; }
    void setLag(AstNode& ast) { lag_ = &ast; }
    AstNode* lag() { return lag_; }

  private:
    AstNode* attribute_;
    AstNode* resolution_;
    AstNode* minimumGap_;
    AstNode* lag_;
};
NDF_IS_AS_SPECIALIZATIONS(EventTimeAnnotation, EventTimeAnnotationContext)

class ParallelAnnotationContext : public NameDefFinderContext
{
  public:
    ParallelAnnotationContext(NameDefFinder& finder);
    virtual void print() const { std::cerr << "ParallelAnnotation" << std::endl; }
    void setWidth(AstNode& ast) { _width = &ast; }
    AstNode& width() const
    {
        assert(NULL != _width);
        return *_width;
    }
    void setHostTags(AstNode& ast) { _hostTags = &ast; }
    AstNode* hostTags() const { return _hostTags; }
    void addPartition(const ParallelAnnotationSym::Partition& partition)
    {
        _partitions.push_back(partition);
    }
    const ParallelAnnotationSym::PartitionList& partitions() const { return _partitions; }
    AstNode* broadcasts() const { return _broadcasts; }
    void setPartitionBySeen() { _partitionBySeen = true; }
    bool partitionBySeen() const { return _partitionBySeen; }
    void setBroadcasts(AstNode& ast) { _broadcasts = &ast; }
    void setError() { _error = true; }
    bool error() const { return _error; }

  private:
    AstNode* _width;
    AstNode* _hostTags;
    bool _error;
    ParallelAnnotationSym::PartitionList _partitions;
    AstNode* _broadcasts;
    bool _partitionBySeen;
};
NDF_IS_AS_SPECIALIZATIONS(ParallelAnnotation, ParallelAnnotationContext)

class ParallelAnnotationPartitionByContext : public NameDefFinderContext
{
  public:
    ParallelAnnotationPartitionByContext(NameDefFinder& finder,
                                         ParallelAnnotationContext& parallelContextIn);
    virtual void print() const { std::cerr << "ParallelAnnotationPartitionBy" << std::endl; }
    void setPort(AstNode& portIn) { _port = &portIn; };
    void setAttributes(AstNode& attributesIn) { _attributes = &attributesIn; };
    bool hasPort() const { return NULL != _port; }
    AstNode& port() const { return *_port; }
    bool hasAttributes() const { return NULL != _attributes; }
    AstNode* attributes() const { return _attributes; }
    ParallelAnnotationContext& parallelContext() const { return _parallelContext; }

  private:
    AstNode* _port;
    AstNode* _attributes;
    ParallelAnnotationContext& _parallelContext;
};
NDF_IS_AS_SPECIALIZATIONS(ParallelAnnotationPartitionBy, ParallelAnnotationPartitionByContext)

class ViewAnnotationContext : public NameDefFinderContext
{
  public:
    ViewAnnotationContext(NameDefFinder& finder);
    virtual void print() const { std::cerr << "ViewAnnotation" << std::endl; }
    void setName(AstNode& name_) { _name = &name_; }
    AstNode& name() const
    {
        assert(NULL != _name);
        return *_name;
    }
    void setPort(AstNode& port_) { _port = &port_; }
    AstNode& port() const
    {
        assert(NULL != _port);
        return *_port;
    }
    void setSampleSize(AstNode& sampleSize_) { _sampleSize = &sampleSize_; }
    AstNode& sampleSize() const
    {
        assert(NULL != _sampleSize);
        return *_sampleSize;
    }
    void setBufferSize(AstNode& bufferSize_) { _bufferSize = &bufferSize_; }
    AstNode* bufferSize() const { return _bufferSize; }
    void setBufferTime(AstNode& bufferTime_) { _bufferTime = &bufferTime_; }
    AstNode* bufferTime() const { return _bufferTime; }
    void setDescription(AstNode& description_) { _description = &description_; }
    AstNode* description() const { return _description; }
    void setAttributes(AstNode& attributes_) { _attributes = &attributes_; }
    AstNode* attributes() const { return _attributes; }
    void setFilter(AstNode& filter_) { _filter = &filter_; }
    AstNode* filter() const { return _filter; }
    void setActivateOption(AstNode& activateOption_) { _activateOption = &activateOption_; }
    AstNode* activateOption() const { return _activateOption; }

  private:
    AstNode* _name;
    AstNode* _port;
    AstNode* _sampleSize;
    AstNode* _bufferSize;
    AstNode* _bufferTime;
    AstNode* _description;
    AstNode* _attributes;
    AstNode* _filter;
    AstNode* _activateOption;
};
NDF_IS_AS_SPECIALIZATIONS(ViewAnnotation, ViewAnnotationContext)

class DummyContext : public NameDefFinderContext
{
  public:
    DummyContext(NameDefFinder& finder)
      : NameDefFinderContext(finder, NameDefFinderContext::Dummy)
    {}
    virtual void print() const { std::cerr << "DummyContext" << std::endl; }
};
NDF_IS_AS_SPECIALIZATIONS(Dummy, DummyContext)

class ViewAnnotationFilterContext : public NameDefFinderContext
{
  public:
    ViewAnnotationFilterContext(NameDefFinder& finder)
      : NameDefFinderContext(finder, NameDefFinderContext::ViewAnnotationFilter)
    {}
    virtual void print() const { std::cerr << "ViewAnnotationFilterContext" << std::endl; }
};
NDF_IS_AS_SPECIALIZATIONS(ViewAnnotationFilter, ViewAnnotationFilterContext)

class ThreadingAnnotationContext : public NameDefFinderContext
{
  public:
    ThreadingAnnotationContext(NameDefFinder& finder);
    virtual void print() const { std::cerr << "ThreadingAnnotationContext" << std::endl; }

    void setModel(AstNode& modelId_, AstNode& model_)
    {
        _modelId = &modelId_;
        _model = &model_;
    }
    AstNode& modelId() const
    {
        assert(NULL != _modelId);
        return *_modelId;
    }
    AstNode& model() const
    {
        assert(NULL != _model);
        return *_model;
    }

    void setThreads(AstNode& threadsId_, AstNode& threads_)
    {
        _threadsId = &threadsId_;
        _threads = &threads_;
    }
    AstNode* threadsId() const { return _threadsId; }
    AstNode* threads() const { return _threads; }

    void setElastic(AstNode& elasticId_, AstNode& elastic_)
    {
        _elasticId = &elasticId_;
        _elastic = &elastic_;
    }
    AstNode* elasticId() const { return _elasticId; }
    AstNode* elastic() const { return _elastic; }

  private:
    AstNode* _modelId;
    AstNode* _model;
    AstNode* _threadsId;
    AstNode* _threads;
    AstNode* _elasticId;
    AstNode* _elastic;
};
NDF_IS_AS_SPECIALIZATIONS(ThreadingAnnotation, ThreadingAnnotationContext)

class OptionalContext : public NameDefFinderContext
{
  public:
    OptionalContext(NameDefFinder& finder);
    virtual void print() const { std::cerr << "OptionalContext" << std::endl; }
};
NDF_IS_AS_SPECIALIZATIONS(Optional, OptionalContext)

} // namespace NameDefFinderContexts

class AstPool;
class ParserContext;
class SourceLocation;
class SymbolTable;

/// Visitor that decorates the AST with new SymbolTableEntry instances
/// that also go into the symbol table scopes for later lookup.
class NameDefFinder : public AstVisitor
{
  public:
    /// Create a temporary instance of this class and visit the AST with it.
    static void run(ParserContext&, AstNode&);

    NameDefFinderContexts::NameDefFinderContext* setContext(
      NameDefFinderContexts::NameDefFinderContext* context)
    {
        NameDefFinderContexts::NameDefFinderContext* prev = _currentContext;
        _currentContext = context;
        return prev;
    }

  protected:
    NameDefFinderContexts::NameDefFinderContext& currentContext()
    {
        assert(NULL != _currentContext);
        return *_currentContext;
    }

    /// If this stage isn't yet done for 'ast', mark it as done, and then
    /// call the superclass generic visit() method to actually do this stage.
    virtual void visit(AstNode& ast);

    // ----- The compilation unit is the start symbol of the SPL grammar.
    virtual void visitCompilationUnit(AstNode&);
    virtual void visitUseDirective(AstNode&);

    // ----- Composite operators are defined at the top-level in a namespace.
    virtual void visitCompositeDef(AstNode&);
    virtual void visitCompositeFormal(AstNode&);
    virtual void visitConfigItem(AstNode&);

    // ----- Streams are defined in a composite operator's graph clause.
    virtual void visitOpInvoke(AstNode&);
    virtual void visitOpOutput(AstNode&);
    virtual void visitPortInputs(AstNode&);
    virtual void visitOnProcessLogic(AstNode&);
    virtual void visitOnTupleLogic(AstNode&);
    virtual void visitOnPunctLogic(AstNode&);
#if 0
                        // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
                        virtual void visitOnWatermarkLogic(AstNode &);
#endif
    virtual void visitOpInvokeWindow(AstNode&);
    virtual void visitOpInvokeActual(AstNode&);
    virtual void visitOpInvokeOutput(AstNode&);

    // ----- Functions are defined at the top-level in a namespace.
    virtual void visitFunctionDef(AstNode&);
    virtual void visitFunctionFormal(AstNode&);

    // ----- Function Prototype declarations come from XML files, not regular SPL files.
    virtual void visitFunctionPrototype(AstNode&);
    virtual void visitTypeFormal(AstNode&);
    virtual void visitBoundsFormals(AstNode&);
    virtual void visitProtoFormal(AstNode&);

    // ----- Imperative statements can appear in function bodies or the logic clause of operator
    // invocations.
    virtual void visitLocalDecl(AstNode&);
    virtual void visitBlockStmt(AstNode&);
    virtual void visitForStmt(AstNode&);

    // ----- Expressions can appear in many places in the grammar.
    virtual void visitAttributeExpr(AstNode&);

    // ----- Literals are the highest-precedence expressions denoting values.
    virtual void visitTupleLiteral(AstNode&);
    virtual void visitAttributeAssign(AstNode&);
    virtual void visitListLiteral(AstNode&);

    // ----- Types are defined in a composite operator's type clause, or in a block, or at the
    // top-level.
    virtual void visitDefType(AstNode&);
    virtual void visitEnumType(AstNode&);
    virtual void visitXMLType(AstNode&);
    virtual void visitListType(AstNode&);
    virtual void visitMapType(AstNode&);
    virtual void visitPrimitiveType(AstNode&);
    virtual void visitRStringType(AstNode&);
    virtual void visitSetType(AstNode&);
    virtual void visitTupleType(AstNode&);
    virtual void visitUStringType(AstNode&);
    virtual void visitVoidType(AstNode&);
    virtual void visitTupleAttrib(AstNode&);
    virtual void visitTupleExtend(AstNode&);
    virtual void visitOptionalType(AstNode&);
    virtual void visitAttributeDecl(AstNode&);

    // Annotations
    virtual void visitAnnotations(AstNode&);
    virtual void visitAnnotation(AstNode&);

  private:
    NameDefFinder(ParserContext&);
    SourceLocation const& newLoc(AstNode&);
    void setFatalError() { _fatalError = true; }
    bool getFatalError() const { return _fatalError; }

    ParserContext& _pCtx;
    bool _fatalError;
    AstPool& _pool;
    SymbolTable& _symTab;
    NameDefFinderContexts::NameDefFinderContext* _currentContext;
    NameDefFinderContexts::StartContext _startContext;
};
}

#endif /* SPL_NAME_DEF_FINDER_H */
