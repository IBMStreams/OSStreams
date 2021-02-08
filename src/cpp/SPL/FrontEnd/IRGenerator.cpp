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

#include <SPL/FrontEnd/IRGenerator.h>

#include <SPL/CodeGen/CompositeDefinition.h>
#include <SPL/CodeGen/CompositeOperatorInstance.h>
#include <SPL/CodeGen/Config.h>
#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/Literal.h>
#include <SPL/CodeGen/OperatorIR.h>
#include <SPL/CodeGen/Statement.h>
#include <SPL/Core/CompilationState.h>
#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/CompilerException.h>
#include <SPL/Core/ExpressionEvaluator.h>
#include <SPL/Core/ExpressionValue.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/Core/PlacementConstraint.h>
#include <SPL/Core/StreamsUseDef.h>
#include <SPL/Core/Toolkit.h>
#include <SPL/FrontEnd/FrontEndDriver.h>
#include <SPL/FrontEnd/FrontEndErrors.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/SourceLocationFactory.h>
#include <SPL/FrontEnd/SupportedAnnotations.h>
#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>
#include <SPL/FrontEnd/SymbolTableEntry.h>
#include <SPL/FrontEnd/TreeHelper.h>
#include <SPL/FrontEnd/TypeFactory.h>
#include <SPL/FrontEnd/UsedCompositeDefinitions.h>
#include <SPL/FrontEnd/UsedOperatorInvocations.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/SysOut.h>
#include <SPL/Utility/Utility.h>

#include <UTILS/DistilleryException.h>
#include <UTILS/SupportFunctions.h>

#include <antlr3.h>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/path.hpp>
#include <cassert>
#include <vector>

namespace bf = boost::filesystem;

using namespace std;

namespace SPL {
struct IRGenerator::State
{
    vector<Config*> _configs;
    vector<PrimitiveOperatorInstance::Port*> _inputPorts, _outputPorts;
    vector<pair<string, uint32_t> > _inputUses;
    vector<string> _outputDefs;
    vector<PrimitiveOperatorInstance::Window*> _windows;
#if 0
        // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
        vector<pair<uint32_t, Statement *> > _portTupleLogic, _portPunctLogic, _portWatermarkLogic;
#else
    vector<pair<uint32_t, Statement*> > _portTupleLogic, _portPunctLogic;
#endif
    vector<Param*> _params;
    vector<pair<string, const BinaryExpression*> > _outputs;
    Statement* _stateLogic;
    Statement* _processLogic;
    bool _visitingCompositeDef;

    State()
      : _stateLogic(NULL)
      , _processLogic(NULL)
      , _visitingCompositeDef(false)
    {}
};

// ======================== namespace IRGeneratorHelper ========================
namespace IRGeneratorHelper {
// forward declarations
// static Expression& attributeExpr(ParserContext const &, AstNode &);
// static Expression* identifier(ParserContext const &, AstNode &);

// private helper functions ---------------------------------------
static LiteralExpression& setLiteralIR(AstPool& pool,
                                       AstNode& ast,
                                       const RootTyp& type,
                                       const RootTyp& varType,
                                       SourceLocation const& loc,
                                       ExpressionValue const* expv)
{
    Literal* lit = new Literal(type, varType, expv);
    LiteralExpression* ir = new LiteralExpression(varType, loc, *lit);
    setAstExpression(pool, ast, *ir);
    return *ir;
}

static LiteralExpression& setLiteralIR(AstPool& pool,
                                       AstNode& ast,
                                       const RootTyp& type,
                                       SourceLocation const& loc,
                                       ExpressionValue const* expv)
{
    return setLiteralIR(pool, ast, type, type, loc, expv);
}

static LiteralExpression& setLiteralIR(AstPool& pool,
                                       AstNode& ast,
                                       const RootTyp& type,
                                       SourceLocation const& loc,
                                       const SPL::list<SPL::rstring>& v)
{
    const RootTyp& eType = type.as<ListTyp>().getElementType();
    Literal* lit = new Literal(type, NULL);
    for (uint32_t i = 0, n = v.size(); i < n; i++) {
        lit->addListElement(*new Literal(eType, new ExpressionValue(v[i])));
    }
    LiteralExpression* ir = new LiteralExpression(type, loc, *lit);
    setAstExpression(pool, ast, *ir);
    return *ir;
}

static inline void setStatementIR(AstPool& pool,
                                  SourceLocation const& loc,
                                  AstNode& ast,
                                  Statement* ir)
{
    setAstStatement(pool, ast, *ir);
}

static void dumpOne(const char* str, AstNode* ast)
{
    cerr << str << ": ast = " << ast;
    if (ast) {
        if (astSymbolPtr(*ast)) {
            cerr << ", sym: " << &astSymbol(*ast) << endl;
        } else {
            cerr << ", NO SYM" << endl;
        }
    } else {
        cerr << endl;
    }
}

static void dumpRecursive(AstNode* ast)
{
    // Print me
    dumpOne("", ast);
    // Print all the children, recursively
    for (int i = 0, n = astSize(*ast); i < n; i++) {
        dumpRecursive(&astChild(*ast, i));
    }
}

static inline void dumpAST(const char* str, AstNode* ast)
{
    if (indebug(Debug::IRGen)) {
        dumpOne(str, ast);
    }
}

// composites -----------------------------------------------------

/*
 * The IR Generator Helper uses static variables to store information during the
 * tree parse, that is then used when the Composites (and Functions) are created in the IR
 * This works because we know the order of parsing of the tree.
 *
 * If we ever decide to compile this in parallel, this will have to be changed to either
 * store the information in the tree directory, or keep a per thread version of these
 * variables.
 */

static void expnError(const Expression& e, char* type)
{
    SysOut::errorln(SPL_CORE_CONFIG_EXPECTED_EXPN_WITH_TYPE(type, e.toString()),
                    e.getStartLocation());
}

static void listError(const Expression& e, char* values)
{
    SysOut::errorln(SPL_CORE_CONFIG_EXPECTED_ONE_OF_LIST(values, e.toString()),
                    e.getStartLocation());
}

static bool getBoolValue(const Expression& e)
{
    const Literal* l = e.getLiteral();
    if (l && l->getType().getMetaType() == MetaType::BOOLEAN) {
        return l->primitive().get_boolean();
    }
    expnError(e, "boolean");
    return true;
}

static double getFloatValue(const Expression& e)
{
    const Literal* l = e.getLiteral();
    if (!l || l->getKind() != Literal::Primitive) {
        expnError(e, "float64");
        return 0;
    }
    const ExpressionValue& lit = l->primitive();
    double v;
    if (lit.getFloatValue(v)) {
        return v;
    }
    expnError(e, "float64");
    return 0;
}

static Expression& getSimplifiedExpn(AstNode& a)
{
    Expression& oldE = astExpression(a);
    ExpressionEvaluator ee(CompilerConfiguration::instance());
    Expression* e = Expression::simplify(&oldE, ee);
    assert(e);
    resetAstExpression(AstPool::instance(), a, *e);
    return *e;
}

static void placementError(const Expression& e, AstNode* a, ParserContext const* pContext)
{
    if (a) {
        SysOut::errorln(SPL_CORE_CONFIG_INVALID_PLACEMENT_EXPN(e.toString()),
                        SourceLocationFactory::instance().create(*pContext, *a));
    } else {
        SysOut::errorln(SPL_CORE_CONFIG_INVALID_PLACEMENT_EXPN(e.toString()), e.getStartLocation());
    }
}

static void getHost(PlacementConstraint& pc, const CallExpression& call)
{
    // format:   string OR poolName[intExpn|'compileTime'|'runTime']
    const vector<Expression*>& args = call.getArgs();
    if (args.size() != 1) {
        placementError(call, NULL, NULL);
        return;
    }
    const Expression* e = args[0];
    if (e->getKind() == Expression::Subscript) {
        const SubscriptExpression& sub = e->as<SubscriptExpression>();
        if (sub.getSlice()) {
            placementError(call, NULL, NULL);
            return;
        }
        const Expression& lhs = sub.getLhs();
        if (lhs.getKind() != Expression::Symbol) {
            placementError(call, NULL, NULL);
            return;
        }
        const SymbolExpression& lhsSymbol = lhs.as<SymbolExpression>();

        // RHS is int literal
        const Expression* rhs = sub.lower();
        assert(rhs);
        // Better be int literal
        const Literal* l = rhs->getLiteral();
        if (!l) {
            placementError(call, NULL, NULL);
            return;
        }
        int64_t size;
        const ExpressionValue& ev = l->primitive();
        if (ev.getIntegerValue(size)) {
            pc.setHostPlacement(PlacementConstraint::PoolExpn, lhsSymbol.getId(), size);
        } else {
            placementError(call, NULL, NULL);
        }
    } else if (e->getKind() == Expression::Symbol) {
        pc.setHostPlacement(PlacementConstraint::Pool, e->as<SymbolExpression>().getId(), 0);
    } else {
        // Want a string literal
        const Literal* l = e->getLiteral();
        if (l && l->getType().getMetaType() == MetaType::RSTRING) {
            pc.setHostPlacement(l->primitive().get_rstring());
        } else {
            SysOut::errorln(SPL_CORE_CONFIG_PLACEMENT_EXPECTED_STRING(e->toString()),
                            e->getStartLocation());
        }
    }
}

static void getLocation(PlacementConstraint& pc,
                        PlacementConstraint::PlacementConstraintKind kind,
                        const CallExpression& call)
{
    const vector<Expression*>& args = call.getArgs();
    assert(args.size() == 1);
    const Expression* e = args[0];
    const Literal* l = e->getLiteral();
    if (l && l->getType().getMetaType() == MetaType::RSTRING) {
        pc.addPlacementConstraint(kind, l->primitive().get_rstring());
        return;
    }
    SysOut::errorln(SPL_CORE_CONFIG_PLACEMENT_EXPECTED_STRING(e->toString()),
                    e->getStartLocation());
}

static void checkpointError(const Expression& e)
{
    SysOut::errorln(SPL_FRONT_END_CONFIG_INVALID_CHECKPOINT_EXPN(e.toString()),
                    e.getStartLocation());
}

static void invalidCheckpoint(const Expression& e)
{
    SysOut::errorln(SPL_FRONT_END_CONFIG_INVALID_CHECKPOINT_COMBINATION, e.getStartLocation());
}

static void invalidCheckpointPeriod(const Expression& e)
{
    SysOut::errorln(SPL_FRONT_END_CONFIG_INVALID_CHECKPOINT_PERIOD(e.toString()),
                    e.getStartLocation());
}

static Config* getCheckpoint(ParserContext const& pContext, AstNode& ast)
{
    dumpAST(__FUNCTION__, &ast);

    bool operatorDriven = false;
    double period = 0;
    bool seenOperatorDriven = false, seenPeriod = false;

    for (int i = 0, n = AST::config::exprCount(ast); i < n; i++) {
        // Looking for SymbolExpression or CallExpression
        AstNode& a = AST::config::expr(ast, i);
        const Expression& exp = getSimplifiedExpn(a);
        if (exp.getKind() == Expression::Call) {
            const CallExpression& call = exp.as<CallExpression>();
            const string& token = call.getFcn().name();
            if (token == "periodic") {
                seenPeriod = true;
                if (seenOperatorDriven) {
                    invalidCheckpoint(call);
                }
                period = getFloatValue(*call.getArgs()[0]);
                if (period <= 0) {
                    invalidCheckpointPeriod(*call.getArgs()[0]);
                }
            } else {
                checkpointError(exp);
                continue;
            }
        } else {
            // Better be just a symbol Expression
            string token = astText(a);
            if (token == "operatorDriven") {
                seenOperatorDriven = true;
                if (seenPeriod) {
                    invalidCheckpoint(exp);
                }
                operatorDriven = true;
            } else {
                checkpointError(exp);
                continue;
            }
        }
    }
    SourceLocation const& loc = SourceLocationFactory::instance().create(pContext, ast);
    return &Config::fromCheckpoint(loc, period, !operatorDriven);
}

static void getPlacement(PlacementConstraint& pc, ParserContext const& pContext, AstNode& ast)
{
    dumpAST(__FUNCTION__, &ast);
    bool seenHost = false;
    for (int i = 0, n = AST::config::exprCount(ast); i < n; i++) {
        // Looking for SymbolExpression or CallExpression
        AstNode& a = AST::config::expr(ast, i);
        CompilationState& state = CompilationState::instance();
        state.push();
        state.setStringizeGetChannel();
        state.setStringizeGetLocalChannel();
        state.setStringizeGetAllChannels();
        state.setStringizeGetMaxChannels();
        state.setStringizeGetLocalMaxChannels();
        state.setStringizeGetAllMaxChannels();
        state.setAllowByChannel();
        const Expression& exp = getSimplifiedExpn(a);
        state.pop();
        if (exp.getKind() == Expression::Call) {
            const CallExpression& call = exp.as<CallExpression>();
            const string& token = call.getFcn().name();
            if (token == "host") {
                if (seenHost) {
                    SysOut::errorln(SPL_CORE_CONFIG_PLACEMENT_MULTIPLE_HOST,
                                    exp.getStartLocation());
                } else {
                    getHost(pc, call);
                    seenHost = true;
                }
            } else if (token == "hostColocation") {
                getLocation(pc, PlacementConstraint::HostColocation, call);
            } else if (token == "hostExlocation") {
                getLocation(pc, PlacementConstraint::HostExlocation, call);
            } else if (token == "partitionColocation") {
                getLocation(pc, PlacementConstraint::PartitionColocation, call);
            } else if (token == "partitionExlocation") {
                getLocation(pc, PlacementConstraint::PartitionExlocation, call);
            } else {
                placementError(exp, &a, &pContext);
                continue;
            }
        } else {
            // Better be just a symbol Expression
            string token = astText(a);
            if (token == "hostIsolation") {
                pc.setHostIsolation(true);
            } else if (token == "partitionIsolation") {
                pc.setPartitionIsolation(true);
            } else {
                placementError(exp, &a, &pContext);
                continue;
            }
        }
    }
}

static void hostPoolError(const Expression& e)
{
    SysOut::errorln(SPL_CORE_CONFIG_INVALID_HOST_POOL_EXPN(e.toString()), e.getStartLocation());
}

static bool getExclusive(const Expression& e, bool& exclusive)
{
    const EnumExpression& en = e.as<EnumExpression>();
    const string& id = en.getId();
    if (!(id == "Shared" || id == "Exclusive")) {
        return false;
    }

    exclusive = id == "Exclusive";
    return true;
}

static vector<Config::OneHostPool>& getHostPool(AstNode& ast)
{
    dumpAST(__FUNCTION__, &ast);
    vector<Config::OneHostPool>* pools = new vector<Config::OneHostPool>;
    for (int i = 0, n = AST::config::exprCount(ast); i < n; i++) {
        // Looking for name = LISTLITERAL or name = createPool (literal) or
        // createPool ([<tuple>,] enum);
        const Expression& e = getSimplifiedExpn(AST::config::expr(ast, i));
        if (e.getKind() != Expression::Binary) {
            hostPoolError(e);
            continue;
        }
        const BinaryExpression& be = e.as<BinaryExpression>();
        if (be.getOp() != Expression::eq) {
            hostPoolError(e);
            continue;
        }

        const Expression& lhs = be.getLhs();
        const Expression& rhs = be.getRhs();

        // LHS is just a name
        string poolName;
        if (lhs.getKind() == Expression::Symbol) {
            poolName = lhs.as<SymbolExpression>().getId();

            // RHS must be list of strings or createPool (args)
            const Literal* l = rhs.getLiteral();
            if (l) {
                if (l->getKind() != Literal::List) {
                    hostPoolError(e);
                    continue;
                }
                const RootTyp* type = &l->getType();
                const ListTyp* lt = &type->as<ListTyp>();
                type = &lt->getElementType();
                if (type->getMetaType() != MetaType::RSTRING) {
                    hostPoolError(e);
                    continue;
                }
                // Okay, it is a list of strings!
                pools->push_back(Config::OneHostPool(poolName, *l));
            } else {
                if (!rhs.is<CallExpression>()) {
                    hostPoolError(e);
                    continue;
                }
                const CallExpression& callExpn = rhs.as<CallExpression>();

                // Check the function: createPool is what we want
                const FunctionHeadSym& fcn = callExpn.getFcn();
                if (fcn.name() != "createPool") {
                    hostPoolError(e);
                    continue;
                }

                // createPool (Sys.{Shared/Exclusive})
                // Check the args
                const vector<Expression*>& args = callExpn.getArgs();
                if (args.size() < 1 || args.size() > 2) {
                    hostPoolError(e);
                    continue;
                }
                // Can be one of several choices
                const Expression* arg0 = args[0];
                if (arg0->getKind() == Expression::Enum) {
                    // createPool (Sys.{Shared/Exclusive})
                    bool exclusive;
                    if (args.size() != 1 || !getExclusive(*arg0, exclusive)) {
                        hostPoolError(e);
                        continue;
                    }
                    if (exclusive) {
                        SysOut::errorln(SPL_CORE_CONFIG_UNSIZED_EXCLUSIVE_POOL,
                                        e.getStartLocation());
                        continue;
                    }
                    pools->push_back(
                      Config::OneHostPool(poolName, 0, false, vector<string>(), exclusive));
                    continue;
                }
                l = args[0]->getLiteral();
                if (!l) {
                    hostPoolError(e);
                    continue;
                }
                if (l->getKind() == Literal::Tuple) {
                    // New form of createPool
                    if (args.size() != 2) {
                        hostPoolError(e);
                        continue;
                    }
                    // createPool ({size/tags}, Sys.{Shared/Exclusive})
                    const Expression* arg1 = args[1];
                    bool exclusive;
                    if (arg1->getKind() != Expression::Enum || !getExclusive(*arg1, exclusive)) {
                        hostPoolError(e);
                        continue;
                    }
                    bool hasSize = false;
                    int64_t size = 0;
                    vector<string> tags;
                    bool error = false;

                    // Parse the literal
                    for (uint32_t j = 0, p = l->numElements(); j < p; j++) {
                        const Literal::TupleLiteral& tl = l->tupleElement(j);
                        if (tl._id == "size") {
                            if (tl._value->getKind() != Literal::Primitive) {
                                hostPoolError(e);
                                error = true;
                                break;
                            }
                            const ExpressionValue& ev = tl._value->primitive();
                            if (!ev.getIntegerValue(size)) {
                                hostPoolError(e);
                                error = true;
                                break;
                            }
                            hasSize = true;
                        } else if (tl._id == "tags") {
                            if (tl._value->getKind() != Literal::List) {
                                hostPoolError(e);
                                continue;
                            }
                            // Check element type
                            if (tl._value->getType().as<ListTyp>().getElementType().getMetaType() !=
                                MetaType::RSTRING) {
                                hostPoolError(e);
                                continue;
                            }
                            // Okay, it is a list of strings!
                            for (uint32_t k = 0, q = tl._value->numElements(); k < q; k++) {
                                const Literal& l1 = tl._value->listSetElement(k);
                                tags.push_back(l1.primitive().get_rstring());
                            }
                        } else {
                            hostPoolError(e);
                            error = true;
                            break;
                        }
                    }
                    if (error) {
                        continue;
                    }
                    if (exclusive && !hasSize) {
                        SysOut::errorln(SPL_CORE_CONFIG_UNSIZED_EXCLUSIVE_POOL,
                                        e.getStartLocation());
                        continue;
                    }
                    pools->push_back(Config::OneHostPool(poolName, size, hasSize, tags, exclusive));
                } else {
                    hostPoolError(e);
                    continue;
                }
            }
        } else {
            hostPoolError(e);
            continue;
        }
    }
    return *pools;
}

static Config::TracingKind getLogLevel(const Expression& e)
{
    if (e.getKind() == Expression::Symbol) {
        const SymbolExpression& se = e.as<SymbolExpression>();
        const string& id = se.getId();
        if (id == "error") {
            return Config::Error;
        }
        if (id == "info") {
            return Config::Info;
        }
        if (id == "debug") {
            return Config::Debug;
        }
        if (id == "trace") {
            return Config::Trace;
        }
    }
    listError(e, "error/info/debug/trace");
    return Config::Error;
}

static Config::TracingKind getTraceLevel(const Expression& e)
{
    if (e.getKind() == Expression::Symbol) {
        const SymbolExpression& se = e.as<SymbolExpression>();
        const string& id = se.getId();
        if (id == "error") {
            return Config::Error;
        }
        if (id == "info") {
            return Config::Info;
        }
        if (id == "debug") {
            return Config::Debug;
        }
        if (id == "trace") {
            return Config::Trace;
        }
        if (id == "off") {
            return Config::Off;
        }
        if (id == "warn") {
            return Config::Warn;
        }
    }
    listError(e, "off/error/warn/info/debug/trace");
    return Config::Error;
}

static Config::TransportKind getTransport(const Expression& e)
{
    if (e.getKind() == Expression::Symbol) {
        const SymbolExpression& se = e.as<SymbolExpression>();
        const string& id = se.getId();
        if (id == "TCP") {
            return Config::TCP;
        }
    }
    listError(e, "TCP");
    return Config::TCP;
}

static string getString(const Expression& e)
{
    const Literal* l = e.getLiteral();
    if (l && l->getType().getMetaType() == MetaType::RSTRING) {
        return l->primitive().get_rstring();
    }
    expnError(e, "rstring");
    return "<error>";
}

static string getWrapper(const Expression& e)
{
    if (e.getKind() == Expression::Symbol) {
        const SymbolExpression& se = e.as<SymbolExpression>();
        const string& id = se.getId();
        if (id == "none" || id == "gdb" || id == "nrgdb" || id == "sdb" || id == "valgrind" ||
            id == "console") {
            return id;
        }
    }
    listError(e, "none/nrgdb/gdb/sdb/valgrind/console");
    return "<error>";
}

/// Desired Port Kind
enum PortType
{
    InputPort,  //!< only input port wanted
    OutputPort, //!< only output port wanted
    AnyPort     //!< either port is acceptable
};
/// Desired Port Name
enum PortNameType
{
    StreamName,    //!< Must be the stream name
    PortAliasName, //!< Must be the alias for the port
    AnyPortName    //!< Can be either a stream or port name
};

static uint32_t findPort(const string& name,
                         vector<PrimitiveOperatorInstance::Port*> const& ports,
                         PortNameType portNameType,
                         uint32_t& index)
{
    uint32_t count = 0;
    uint32_t i, n = ports.size();
    for (i = 0; i < n; i++) {
        PrimitiveOperatorInstance::Port const& p = *ports[i];
        if (portNameType == StreamName || portNameType == AnyPortName) {
            uint32_t j, ns = p.getNumStreams();
            for (j = 0; j < ns; j++) {
                if (name == p.getStreamName(j)) {
                    index = i;
                    count++;
                    break;
                }
            }
        }
        if (portNameType == PortAliasName || portNameType == AnyPortName) {
            if (name == p.getAlias()) {
                index = i;
                count++;
            }
        }
    }
    return count;
}

/// Convert a name to the stream index.  Complain if the name is ambiguous or not found
/// @param name User written name that we want to match
/// @param loc Source location of string (for error messag)
/// @param portType Desired port kind
/// @param portNameType Desired point type
/// @return index into input or output streams
static uint32_t nameToPortIndex(const string& name,
                                const SourceLocation& loc,
                                PortType portType,
                                PortNameType portNameType,
                                IRGenerator::State& state)
{
    uint32_t index = 0, count = 0;
    if (portType == InputPort || portType == AnyPort) {
        count += findPort(name, state._inputPorts, portNameType, index);
    }
    if (portType == OutputPort || portType == AnyPort) {
        count += findPort(name, state._outputPorts, portNameType, index);
    }

    if (count == 0) {
        if (portType == InputPort) {
            SysOut::errorln(SPL_FRONT_END_CANNOT_LOCATE_INPUT_PORT_FOR_IDENTIFIER(name), loc);
        } else if (portType == OutputPort) {
            SysOut::errorln(SPL_FRONT_END_CANNOT_LOCATE_OUTPUT_PORT_FOR_IDENTIFIER(name), loc);
        } else {
            SysOut::errorln(SPL_FRONT_END_CANNOT_LOCATE_PORT_FOR_IDENTIFIER(name), loc);
        }
    }
    if (count > 1) {
        SysOut::errorln(SPL_FRONT_END_LOCATED_MULTIPLE_PORTS_FOR_IDENTIFIER(name), loc);
    }
    return index;
}

static string actualStreamName(SymbolTableEntry& sym)
{
    if (sym.is<CompositeInputPortSym>()) {
        vector<string> bundle;
        sym.as<CompositeInputPortSym>().pushBundle(bundle);
        return bundle.at(0);
    }
    if (sym.is<StreamSym>()) {
        return sym.fullName();
    }
    assert(sym.is<PortAliasSym>());
    return sym.name();
}

static void doOneStreamViewability(Config::StreamsViewability& sv,
                                   const Expression& e,
                                   IRGenerator::State* state)
{
    const Literal* l = e.getLiteral();
    if (l) {
        // We have a literal - it better be a boolean
        if (l->getType().getMetaType() == MetaType::BOOLEAN) {
            // Do we have multiple boolean values?
            if (sv.hasDefaultViewability()) {
                SysOut::errorln(SPL_FRONT_END_STREAM_VISIBILITY_MULTIPLE_VALUES,
                                e.getStartLocation());
            } else {
                sv.setDefaultViewability(l->primitive().get_boolean());
            }
        } else {
            SysOut::errorln(SPL_FRONT_END_STREAM_VISIBILITY_BAD_EXPN(e.toString()),
                            e.getStartLocation());
        }
        return;
    }

    // If it isn't a boolean literal, it better be a call to viewable (stream, bool)
    // We can't have viewable if we are not in a state
    if (e.getKind() != Expression::Call || state == NULL) {
        SysOut::errorln(SPL_FRONT_END_STREAM_VISIBILITY_BAD_EXPN(e.toString()),
                        e.getStartLocation());
        return;
    }
    const CallExpression& call = e.as<CallExpression>();
    const FunctionHeadSym& fcn = call.getFcn();
    if (fcn.name() != "viewable") {
        SysOut::errorln(SPL_FRONT_END_STREAM_VISIBILITY_BAD_EXPN(e.toString()),
                        e.getStartLocation());
        return;
    }
    const vector<Expression*>& args = call.getArgs();
    if (args[0]->getKind() != Expression::StreamSymbol) {
        SysOut::errorln(SPL_FRONT_END_STREAM_VISIBILITY_BAD_EXPN(e.toString()),
                        e.getStartLocation());
        return;
    }
    const StreamSymbolExpression& sSym = args[0]->as<StreamSymbolExpression>();

    uint32_t port =
      nameToPortIndex(sSym.getId(), sSym.getStartLocation(), OutputPort, AnyPortName, *state);
    bool viewable = getBoolValue(*args[1]);
    if (sv.setViewability(port, viewable)) {
        SysOut::errorln(SPL_FRONT_END_STREAM_VISIBILITY_ALREADY_SET(sSym.getId()),
                        e.getStartLocation());
    }
}

static Config* getStreamViewability(AstNode& ast, IRGenerator::State* state)
{
    SourceLocation const& loc = astSymbol(ast).sourceLocation();
    Config& c = Config::fromStreamViewability(loc);
    Config::StreamsViewability& sv = c.getStreamsViewability();
    AstNode& a = AST::config::expr(ast, 0);
    const Expression& e = getSimplifiedExpn(a);
    doOneStreamViewability(sv, e, state);
    uint32_t numNodes = AST::config::exprCount(ast);
    for (uint32_t i = 1; i < numNodes; i++) {
        AstNode& a1 = AST::config::expr(ast, i);
        doOneStreamViewability(sv, getSimplifiedExpn(a1), state);
    }
    return &c;
}

static void getQueue(const Expression& e, IRGenerator::State& state)
{
    if (e.getKind() != Expression::Call) {
        SysOut::errorln(SPL_FRONT_END_THREADED_PORT_BAD_EXPN(e.toString()), e.getStartLocation());
        return;
    }
    const CallExpression& call = e.as<CallExpression>();
    const FunctionHeadSym& fcn = call.getFcn();
    if (fcn.name() != "queue") {
        SysOut::errorln(SPL_FRONT_END_THREADED_PORT_BAD_EXPN(e.toString()), e.getStartLocation());
        return;
    }
    const vector<Expression*>& args = call.getArgs();
    if (args[0]->getKind() != Expression::StreamSymbol) {
        SysOut::errorln(SPL_FRONT_END_THREADED_PORT_BAD_EXPN(e.toString()), e.getStartLocation());
        return;
    }
    const StreamSymbolExpression& sSym = args[0]->as<StreamSymbolExpression>();

    uint32_t port =
      nameToPortIndex(sSym.getId(), sSym.getStartLocation(), InputPort, AnyPortName, state);

    PrimitiveOperatorInstance::Port::CongestionPolicy congestionPolicy;
    const EnumExpression& enumValue = args[1]->as<EnumExpression>();
    const string& value = enumValue.getId();
    if (value == "Wait") {
        congestionPolicy = PrimitiveOperatorInstance::Port::Wait;
    } else if (value == "DropFirst") {
        congestionPolicy = PrimitiveOperatorInstance::Port::DropFirst;
    } else {
        assert(value == "DropLast");
        congestionPolicy = PrimitiveOperatorInstance::Port::DropLast;
    }

    // Already specified?
    if (state._inputPorts[port]->getIsQueue()) {
        SysOut::warnln(SPL_FRONT_END_THREADED_PORT_MULTIPLE_TIMES(args[0]->toString()),
                       args[0]->getStartLocation());
    }

    if (args.size() == 3) {
        const Literal* l = args[2]->getLiteral();
        if (!l || l->getKind() != Literal::Primitive) {
            SysOut::errorln(SPL_FRONT_END_THREADED_PORT_BAD_SIZE(args[2]->toString()),
                            args[2]->getStartLocation());
            return;
        }
        const ExpressionValue& lit = l->primitive();
        int64_t size;
        if (!lit.getIntegerValue(size) || size < 1) {
            SysOut::errorln(SPL_FRONT_END_THREADED_PORT_BAD_SIZE(args[2]->toString()),
                            args[2]->getStartLocation());
            return;
        }
        state._inputPorts[port]->setQueueProperties(size, congestionPolicy);
    } else {
        // queue with unspecified size
        assert(args.size() == 2);
        state._inputPorts[port]->setIsQueue(true, congestionPolicy);
    }
}

static Config& fromDefaultPool(const SourceLocation& loc, AstNode* ast)
{
    dumpAST(__FUNCTION__, ast);
    int64_t size = 0;

    // Only argument - literal count - Frontend has already checkd
    AstNode& a = AST::config::expr(*ast, 0);
    const Expression& e = getSimplifiedExpn(a);
    const Literal* l = e.getLiteral();
    if (l && l->getKind() == Literal::Primitive) {
        const ExpressionValue& lit = l->primitive();
        if (!lit.getIntegerValue(size) || size <= 0) {
            SysOut::errorln(SPL_FRONT_END_CONFIG_DEFAULTPOOL_SIZE, loc);
        }
    } else {
        SysOut::errorln(SPL_FRONT_END_CONFIG_DEFAULTPOOL_SIZE, loc);
    }

    // And return the results
    return Config::fromDefaultPool(loc, size);
}

static Config* createConfig(ParserContext const& pContext, AstNode& ast, IRGenerator::State* state)
{
    dumpAST(__FUNCTION__, &ast);
    // Grab the string of the config ID
    string id = astText(AST::config::id(ast));
    // And the associated expression
    AstNode& a = AST::config::expr(ast, 0);
    const Expression* e = NULL;
    if (id == "placement") {
        CompilationState& cs = CompilationState::instance();
        cs.push();
        cs.setAllowByChannel();
        e = &getSimplifiedExpn(a);
        cs.pop();
    } else {
        e = &getSimplifiedExpn(a);
    }
    // Now map from string/expn to Config
    SourceLocation const& loc = astSymbol(ast).sourceLocation();
    Config* c = NULL;
    if (id == "restartable") {
        c = &Config::fromRestartable(loc, getBoolValue(*e));
    } else if (id == "relocatable") {
        c = &Config::fromRelocatable(loc, getBoolValue(*e));
    } else if (id == "checkpoint") {
        c = getCheckpoint(pContext, ast);
    } else if (id == "defaultPoolSize") {
        c = &fromDefaultPool(loc, &ast);
    } else if (id == "placement") {
        PlacementConstraint* pc = new PlacementConstraint;
        getPlacement(*pc, pContext, ast);
        c = &Config::fromPlacementConstraint(loc, *pc);
    } else if (id == "hostPool") {
        c = &Config::fromHostPools(loc, getHostPool(ast));
    } else if (id == "logLevel") {
        c = &Config::fromLogLevel(loc, getLogLevel(*e));
    } else if (id == "tracing") {
        c = &Config::fromTracing(loc, getTraceLevel(*e));
    } else if (id == "inputTransport") {
        c = &Config::fromInputTransport(loc, getTransport(*e));
    } else if (id == "outputTransport") {
        c = &Config::fromOutputTransport(loc, getTransport(*e));
    } else if (id == "wrapper") {
        c = &Config::fromWrapper(loc, getWrapper(*e));
    } else if (id == "applicationScope") {
        c = &Config::fromApplicationScope(loc, getString(*e));
    } else if (id == "threadedPort" && NULL != state) {
        // Process all the nodes
        getQueue(*e, *state);
        uint32_t numNodes = AST::config::exprCount(ast);
        for (uint32_t i = 1; i < numNodes; i++) {
            AstNode& a1 = AST::config::expr(ast, i);
            getQueue(getSimplifiedExpn(a1), *state);
        }
        return NULL;
    } else if (id == "streamViewability") {
        return getStreamViewability(ast, state);
    } else {
        THROW(SPLCompilerAssertionFailed, "Unknown config " << ast);
    }
    return c;
}

// this is the only externally visible function from this namespace,
// exported by the header file
Config* createConfig(ParserContext const& pContext, AstNode& ast)
{
    return createConfig(pContext, ast, NULL);
}

static void errorCheckWindow(ParserContext& pContext,
                             AstNode& ast,
                             uint32_t port,
                             IRGenerator::State& state)
{
    bool isTumbling = false;
    bool isEvent = false;
    int numberOfPolicies = 0;
    bool seenPartitioned = false;
    int numberOfPartitionEvictions = 0;
    int numberOfIntervalDuration = 0;
    int numberOfCreationPeriod = 0;
    int numberOfDiscardAge = 0;
    int numberOfIntervalOffset = 0;
    for (int i = 0, n = AST::opInvokeWindow::exprCount(ast); i < n; i++) {
        AstNode& childAst = AST::opInvokeWindow::expr(ast, i);
        bool childIsCall = AST::callExpr::is(childAst);
        bool childIsIntrinsicCall = false;
        if (childIsCall) {
            if (AST::callExpr::is(childAst) &&
                astText(AST::callExpr::target(childAst)) == "partitioned") {
                childIsIntrinsicCall = true;
            } else {
                Expression& callee = astExpression(childAst);
                childIsIntrinsicCall =
                  callee.is<CallExpression>() && callee.as<CallExpression>().getFcn().isIntrinsic();
            }
        }
        bool childIsId = AST::idT::is(childAst);
        string token = childIsCall ? astText(AST::callExpr::target(childAst)) : astText(childAst);
        if (token == "sliding") {
            if (!childIsId) {
                SysOut::errorln(SPL_FRONT_END_EXPECTED_IDENTIFIER_IN_WINDOW(token),
                                astSymbol(childAst).sourceLocation());
            }
        } else if (token == "partitioned") {
            if (!childIsId) {
                SysOut::errorln(SPL_FRONT_END_EXPECTED_IDENTIFIER_IN_WINDOW(token),
                                astSymbol(childAst).sourceLocation());
            }
            seenPartitioned = true;
        } else if (token == "tumbling") {
            isTumbling = true;
            if (!childIsId) {
                SysOut::errorln(SPL_FRONT_END_EXPECTED_IDENTIFIER_IN_WINDOW(token),
                                astSymbol(childAst).sourceLocation());
            }
        } else if (token == "timeInterval") {
            isEvent = true;
            if (!childIsId) {
                SysOut::errorln(SPL_FRONT_END_EXPECTED_IDENTIFIER_IN_WINDOW(token),
                                astSymbol(childAst).sourceLocation());
            }
        } else if (token == "punct" || token == "time" || token == "count") {
            numberOfPolicies++;
            if (!childIsIntrinsicCall) {
                SysOut::errorln(SPL_FRONT_END_EXPECTED_INTRINSIC_IN_WINDOW(token),
                                astSymbol(childAst).sourceLocation());
            }
        } else if (token == "partitionAge" || token == "partitionCount" || token == "tupleCount") {
            if (!childIsIntrinsicCall) {
                SysOut::errorln(SPL_FRONT_END_EXPECTED_INTRINSIC_IN_WINDOW(token),
                                astSymbol(childAst).sourceLocation());
            }
            numberOfPartitionEvictions++;
        } else if (token == "delta") {
            numberOfPolicies++;
            if (childIsIntrinsicCall) {
                SymbolTableEntry& labelSym = astSymbol(AST::opInvokeWindow::id(ast));
                SymbolTableEntry& attrSym = astSymbol(AST::callExpr::actual(childAst, 0));
                if (!(attrSym.is<AttributeAccessSym>() || attrSym.is<IndirectSym>()) ||
                    !labelSym.heldScope()->has(attrSym.name())) {
                    SysOut::errorln(
                      SPL_FRONT_END_EXPECTED_ATTRIBUTE_IN_DELTA_WINDOW(labelSym.name()),
                      astSymbol(childAst).sourceLocation());
                }
            } else {
                SysOut::errorln(SPL_FRONT_END_EXPECTED_INTRINSIC_IN_WINDOW(token),
                                astSymbol(childAst).sourceLocation());
            }
        } else if (token == "intervalDuration") {
            ++numberOfIntervalDuration;
            if (!childIsIntrinsicCall) {
                SysOut::errorln(SPL_FRONT_END_EXPECTED_INTRINSIC_IN_WINDOW(token),
                                astSymbol(childAst).sourceLocation());
            }
        } else if (token == "intervalOffset") {
            ++numberOfIntervalOffset;
            if (!childIsIntrinsicCall) {
                SysOut::errorln(SPL_FRONT_END_EXPECTED_INTRINSIC_IN_WINDOW(token),
                                astSymbol(childAst).sourceLocation());
            }
        } else if (token == "discardAge") {
            ++numberOfDiscardAge;
            if (!childIsIntrinsicCall) {
                SysOut::errorln(SPL_FRONT_END_EXPECTED_INTRINSIC_IN_WINDOW(token),
                                astSymbol(childAst).sourceLocation());
            }
        } else if (token == "creationPeriod") {
            ++numberOfCreationPeriod;
            if (!childIsIntrinsicCall) {
                SysOut::errorln(SPL_FRONT_END_EXPECTED_INTRINSIC_IN_WINDOW(token),
                                astSymbol(childAst).sourceLocation());
            }
        } else {
            SysOut::errorln(SPL_FRONT_END_INVALID_EXPRESSION_IN_WINDOW,
                            astSymbol(childAst).sourceLocation());
        }
    }
    if (isEvent) {
        if (numberOfIntervalDuration != 1) {
            SysOut::errorln(SPL_FRONT_END_MISSING_REQUIRED_ELEMENT_IN_WINDOW("intervalDuration"),
                            astSymbol(ast).sourceLocation());
        }
        if (numberOfIntervalOffset > 1) {
            SysOut::errorln(SPL_FRONT_END_DUPLICATE_OPTIONAL_ELEMENT_IN_WINDOW("intervalOffset"),
                            astSymbol(ast).sourceLocation());
        }
        if (numberOfCreationPeriod > 1) {
            SysOut::errorln(SPL_FRONT_END_DUPLICATE_OPTIONAL_ELEMENT_IN_WINDOW("creationPeriod"),
                            astSymbol(ast).sourceLocation());
        }
        if (numberOfDiscardAge > 1) {
            SysOut::errorln(SPL_FRONT_END_DUPLICATE_OPTIONAL_ELEMENT_IN_WINDOW("discardAge"),
                            astSymbol(ast).sourceLocation());
        }
    } else {
        if (numberOfIntervalDuration != 0) {
            SysOut::errorln(SPL_FRONT_END_EVENT_WINDOW_REQUIRED("intervalDuration"),
                            astSymbol(ast).sourceLocation());
        }
        if (numberOfIntervalOffset != 0) {
            SysOut::errorln(SPL_FRONT_END_EVENT_WINDOW_REQUIRED("intervalOffset"),
                            astSymbol(ast).sourceLocation());
        }
        if (numberOfCreationPeriod != 0) {
            SysOut::errorln(SPL_FRONT_END_EVENT_WINDOW_REQUIRED("creationPeriod"),
                            astSymbol(ast).sourceLocation());
        }
        if (numberOfDiscardAge != 0) {
            SysOut::errorln(SPL_FRONT_END_EVENT_WINDOW_REQUIRED("discardAge"),
                            astSymbol(ast).sourceLocation());
        }
    }
    if (isTumbling) {
        if (1 != numberOfPolicies) {
            SysOut::errorln(SPL_FRONT_END_EXPECTED_ONE_CLAUSE_WINDOW(numberOfPolicies),
                            astSymbol(ast).sourceLocation());
        }
    } else if (isEvent) {
        // Nothing needed here; it is handled above.
    } else { // sliding.  This needs to be the default for backward compat
        if (1 != numberOfPolicies && 2 != numberOfPolicies) {
            SysOut::errorln(SPL_FRONT_END_EXPECTED_ONE_OR_TWO_CLAUSE_WINDOW(numberOfPolicies),
                            astSymbol(ast).sourceLocation());
        }
    }
    for (int i = 0, n = state._windows.size(); i < n; i++) {
        if (port == state._windows[i]->getPortNumber()) {
            SysOut::errorln(
              SPL_FRONT_END_REDUNDANT_WINDOW_FOR_PORT(astText(AST::opInvokeWindow::id(ast))),
              astSymbol(ast).sourceLocation());
        }
    }
    if (numberOfPartitionEvictions > 1) {
        SysOut::errorln(
          SPL_FRONT_END_TOO_MANY_PARTITION_EVICTION_CLAUSE_WINDOW(numberOfPartitionEvictions),
          astSymbol(ast).sourceLocation());
    }
    if (numberOfPartitionEvictions && !seenPartitioned) {
        SysOut::errorln(SPL_FRONT_END_PARTITION_EVICTION_NOT_PARTITIONED,
                        astSymbol(ast).sourceLocation());
    }
}

// statements -----------------------------------------------------
//
static void pushBlocks(vector<Statement*>& body, BlockStatement& b)
{
    if (b.getUnnest()) {
        vector<Statement*>& stmts = b.getStmts();
        for (uint32_t i = 0, n = stmts.size(); i < n; i++) {
            Statement* s = stmts[i];
            if (s->is<BlockStatement>()) {
                pushBlocks(body, s->as<BlockStatement>());
            } else {
                body.push_back(s);
            }
        }
    } else {
        body.push_back(&b);
    }
}

static BlockStatement& blockStmt(AstPool& pool, ParserContext const& pContext, AstNode& ast)
{
    dumpAST(__FUNCTION__, &ast);
    SourceLocation const& loc = SourceLocationFactory::instance().create(pContext, ast);
    vector<Statement*> body;
    bool hasScope = AST::blockStmt::is(ast);
    for (int i = 0,
             n = hasScope ? AST::blockStmt::stmtCount(ast) : AST::blockStmtNoScope::stmtCount(ast);
         i < n; i++) {
        AstNode& stmtAst =
          hasScope ? AST::blockStmt::stmt(ast, i) : AST::blockStmtNoScope::stmt(ast, i);
        if (!AST::typeDef::is(stmtAst)) {
            Statement* s = getAstStatement(stmtAst);
            if (s->is<BlockStatement>()) {
                pushBlocks(body, s->as<BlockStatement>());
            } else {
                body.push_back(s);
            }
        }
    }
    BlockStatement* ir = new BlockStatement(loc, &body);
    setStatementIR(pool, loc, ast, ir);
    return *ir;
}

// expressions ----------------------------------------------------
static void checkPort(AstNode& ast, PortType pType, IRGenerator::State& state)
{
    bool isStream = false;
    Expression& ir = astExpression(ast);
    if (ir.is<StreamSymbolExpression>()) {
        isStream = true;
        string argName = actualStreamName(astSymbol(ast));
        uint32_t index = 0;
        if (1 != findPort(argName, pType == InputPort ? state._inputPorts : state._outputPorts,
                          AnyPortName, index)) {
            SysOut::errorln(pType == InputPort
                              ? SPL_FRONT_END_CANNOT_LOCATE_INPUT_PORT_FOR_IDENTIFIER(argName)
                              : SPL_FRONT_END_CANNOT_LOCATE_OUTPUT_PORT_FOR_IDENTIFIER(argName),
                            ir.getLocation());
        }
    }
    if (!isStream) {
        SysOut::errorln(SPL_FRONT_END_OUTPUT_PORT_NAME_EXPECTED, ir.getLocation());
    }
}

static void errorCheckIntrinsics(AstNode& ast, FunctionHeadSym& callee, IRGenerator::State& state)
{
    const string& name = callee.name();
    if (name == "submit") {
        if (2 == AST::callExpr::actualCount(ast)) {
            AstNode& port = AST::callExpr::actual(ast, 1);
            Expression& ir = astExpression(port);
            if (ir.getType().getMetaType() == MetaType::UINT32) {
                const Literal* l = ir.getLiteral();
                if (l) {
                    assert(l->getKind() == Literal::Primitive);
                    uint64_t v;
                    if (l->primitive().getIntegerValue(v) && v >= state._outputPorts.size()) {
                        SysOut::errorln(SPL_FRONT_END_PORT_OUT_OF_RANGE(v), ir.getLocation());
                    }
                }
            } else {
                checkPort(port, OutputPort, state);
            }
        }
    } else if (name == "getEventTime") {
        // TODO this is valid only when there is an "eventTimeContext"
        // synthetic annotation associated with this operator.
        // (We might not be able to determine that here.)
    }
}

static void errorCheckBuiltins(AstNode& ast, FunctionHeadSym& callee, IRGenerator::State& state)
{
    const string& name = callee.name();
    if (name == "inputPort") {
        checkPort(AST::callExpr::actual(ast, 0), InputPort, state);
    } else if (name == "outputPort") {
        checkPort(AST::callExpr::actual(ast, 0), OutputPort, state);
    }
}

static const Toolkit& findApplicationToolkit()
{
    const Toolkit* toolkit = NULL;
    const vector<const Toolkit*> toolkits = PathSearch::instance().toolkits();
    for (vector<const Toolkit*>::const_iterator it = toolkits.begin(), itEnd = toolkits.end();
         it != itEnd; ++it) {
        if ((*it)->isCurrentToolkit()) {
            toolkit = *it;
            break;
        }
    }
    assert(NULL != toolkit);
    return *toolkit;
}

static const Toolkit& findContainingToolkit(const SourceLocation& loc)
{
    bf::path p = loc.fileName();
    p = bf::canonical(p); // p must exist
    p = p.parent_path();
    const Toolkit* toolkit = NULL;
    const vector<const Toolkit*> toolkits = PathSearch::instance().toolkits();
    for (vector<const Toolkit*>::const_iterator it = toolkits.begin(), itEnd = toolkits.end();
         it != itEnd; ++it) {
        bf::path tkPath = (*it)->path();
        tkPath = bf::canonical(tkPath);
        if (p.string().length() >= tkPath.string().length()) {
            if (p.string().substr(0, tkPath.string().length()) == tkPath.string()) {
                // We found the toolkit corresponding to the source containing the call
                toolkit = *it;
                break;
            }
        }
    }
    assert(NULL != toolkit);
    return *toolkit;
}

static Expression& buildCallToGetToolkitDir(const Toolkit& toolkit,
                                            AstPool& pool,
                                            AstNode& ast,
                                            const RootTyp& type,
                                            const SourceLocation& loc)
{
    string compositeName;
    vector<Expression*>* elist = new vector<Expression*>;
    SPL::rstring val(toolkit.name());
    Expression& lit = setLiteralIR(pool, ast, type, loc, new ExpressionValue(val));
    elist->push_back(&lit);
    const FunctionHeadSym& funcSym = SymbolTable::instance().getToolkitDirectorySym();
    CallExpression* ir = new CallExpression(type, loc, funcSym, *elist, compositeName);
    resetAstExpression(pool, ast, *ir);
    return *ir;
}

static Expression* simplifyBuiltin(AstPool& pool, AstNode& ast, const string& fName)
{
    CallExpressionSym& callSym = astSymbol(ast).as<CallExpressionSym>();
    SourceLocation const& loc = callSym.sourceLocation();
    RootTyp const& type = callSym.type();

    if (fName == "getCompileTimeValue") {
        const vector<string>& compilerArgs = CompilerApp::getSPLCompiler().getPreprocessorArgs();
        AstNode& arg = AST::callExpr::actual(ast, 0);
        const Expression& e = getSimplifiedExpn(arg);
        const Literal* l = e.getLiteral();
        if (!l || l->getKind() != Literal::Primitive ||
            l->getType().getMetaType() != MetaType::RSTRING) {
            SysOut::errorln(
              SPL_CORE_GET_ARGUMENT_NEEDS_LITERAL("getCompileTimeValue", e.toString()),
              e.getLocation());
        }
        string search_for = l->primitive().get_rstring() + '=';
        SPL::rstring val = "0"; // result string
        if (AST::callExpr::actualCount(ast) == 1) {
            // no default value
            bool found = false;
            for (uint32_t i = 0, iu = compilerArgs.size(); i < iu; i++) {
                if (compilerArgs[i].find(search_for) == 0) {
                    found = true;
                    val = compilerArgs[i].substr(search_for.size());
                    break;
                }
            }
            if (!found) {
                SysOut::errorln(SPL_CORE_GET_ARGUMENT_MISSING("getCompileTimeValue", e.toString()),
                                e.getLocation());
            }
            return &setLiteralIR(pool, ast, type, loc, new ExpressionValue(val));
        } else {
            assert(AST::callExpr::actualCount(ast) == 2);
            const Expression& e2 = getSimplifiedExpn(AST::callExpr::actual(ast, 1));
            const Literal* l2 = e2.getLiteral();
            if (!l2 || l2->getKind() != Literal::Primitive ||
                l2->getType().getMetaType() != MetaType::RSTRING) {
                SysOut::errorln(
                  SPL_CORE_GET_ARGUMENT_NEEDS_LITERAL("getCompileTimeValue", e2.toString()),
                  e2.getLocation());
            }
            // Set the default value
            val = l2->primitive().get_rstring();
            for (uint32_t i = 0, iu = compilerArgs.size(); i < iu; i++) {
                if (compilerArgs[i].find(search_for) == 0) {
                    val = compilerArgs[i].substr(search_for.size());
                    break;
                }
            }
            return &setLiteralIR(pool, ast, type, loc, new ExpressionValue(val));
        }
    } else if (fName == "getCompileTimeListValue") {
        const vector<string>& compilerArgs = CompilerApp::getSPLCompiler().getPreprocessorArgs();
        assert(AST::callExpr::actualCount(ast) == 1);
        AstNode& arg = AST::callExpr::actual(ast, 0);
        const Expression& e = getSimplifiedExpn(arg);
        const Literal* l = e.getLiteral();
        if (!l || l->getKind() != Literal::Primitive ||
            l->getType().getMetaType() != MetaType::RSTRING) {
            SysOut::errorln(
              SPL_CORE_GET_ARGUMENT_NEEDS_LITERAL("getCompileTimeListValue", e.toString()),
              e.getLocation());
        }
        string search_for = l->primitive().get_rstring() + '=';
        SPL::rstring val; // result string
        bool found = false;
        for (uint32_t i = 0, iu = compilerArgs.size(); i < iu; i++) {
            if (compilerArgs[i].find(search_for) == 0) {
                found = true;
                val = compilerArgs[i].substr(search_for.size());
                break;
            }
        }
        if (!found) {
            SysOut::errorln(SPL_CORE_GET_ARGUMENT_MISSING("getCompileTimeListValue", e.toString()),
                            e.getLocation());
        }
        vector<string> tokens;
        Distillery::csvtokenize(val, tokens);
        SPL::list<SPL::rstring> values(tokens.begin(), tokens.end());
        return &setLiteralIR(pool, ast, type, loc, values);
    } else if (fName == "getThisFileName") {
        const SourceLocation& l = loc.mappedSourceLocation();
        bf::path p = l.fileName();
        SPL::rstring val = p.leaf().string();
        return &setLiteralIR(pool, ast, type, loc, new ExpressionValue(val));
    } else if (fName == "getThisFileDir" || fName == "getThisFilePath") {
        const SourceLocation& l = loc.mappedSourceLocation();
        bf::path p = Utility::normalize(bf::canonical(l.fileName()));
        if (fName == "getThisFileDir") {
            p = p.parent_path(); // we only want the directory
        }
        // Find the containing toolkit
        const Toolkit& tk = findContainingToolkit(l);
        // Find the relative reminder under the toolkit directory
        bf::path tkPath = Utility::normalize(bf::canonical(tk.path()));
        int nup;
        Utility::makeRelativePath(p, tkPath, nup);
        // Build an add expression to add it to the result of getToolkitDirectory
        SPL::rstring val = p.string();
        // Build a getTookitDirectory call
        Expression& ir = buildCallToGetToolkitDir(tk, pool, ast, type, loc);
        if (val.empty() || val == ".") {
            return &ir;
        }
        // Add the remainder to the toolkit dir
        Literal* lit = new Literal(type, new ExpressionValue(SPL::rstring("/") + val));
        Expression* litExpr = new LiteralExpression(type, loc, *lit);
        Expression* add = new BinaryExpression(type, loc, Expression::plus, ir, *litExpr);
        resetAstExpression(pool, ast, *add);
        return add;
    } else if (fName == "getThisLine") {
        const SourceLocation& l = loc.mappedSourceLocation();
        SPL::int32 val = l.line();
        ExpressionValue result(val);
        return &setLiteralIR(pool, ast, type, loc, new ExpressionValue(val));
    } else if (fName == "getApplicationDir") {
        // Find the name of the application toolkit
        const Toolkit& tk = findApplicationToolkit();
        // We need to build a call to getToolkitDirectory
        Expression& ir = buildCallToGetToolkitDir(tk, pool, ast, type, loc);
        return &ir;
    } else if (fName == "getThisToolkitDir") {
        const SourceLocation& l = loc.mappedSourceLocation();
        const Toolkit& tk = findContainingToolkit(l);
        // We need to build a call to getToolkitDirectory
        Expression& ir = buildCallToGetToolkitDir(tk, pool, ast, type, loc);
        return &ir;
    } else if (fName == "inputPort" || fName == "outputPort") {
        assert(AST::callExpr::actualCount(ast) == 1);
        AstNode& arg = AST::callExpr::actual(ast, 0);
        const Expression& e = getSimplifiedExpn(arg);
        if (e.is<StreamSymbolExpression>()) {
            ExpressionValue portValue(e.as<StreamSymbolExpression>().getPort());
            return &setLiteralIR(pool, ast, type, loc, new ExpressionValue(portValue));
        }
    }
    return NULL;
}

static bool getIntValue(const Literal& l, int32_t& res)
{
    if (l.getKind() != Literal::Primitive) {
        return false; // not a basic type;
    }
    const ExpressionValue& lit = l.primitive();
    int64_t v;
    if (!lit.getIntegerValue(v)) {
        return false;
    }
    res = v;
    return true;
}
}

Literal& createLiteral(SymbolTableEntry& sym, const ExpressionValue* ev)
{
    Literal* lit = NULL;
    try {
        lit = new Literal(sym.type(), ev);
    } catch (Distillery::DistilleryException const& e) {
        SysOut::errorln(SPL_FRONT_END_INVALID_STRING_LITERAL(e.getExplanation()),
                        sym.sourceLocation());
    }
    return *lit;
}

Literal& createLiteral(AstNode& ast)
{
    SymbolTableEntry& sym = astSymbol(ast);
    string val = astText(ast);
    Literal* lit = NULL;
    switch (sym.type().getMetaType()) {
        case MetaType::RSTRING: {
            val = val.substr(0, 1 + val.find_last_not_of("r"));
            rstring v = interpretRStringLiteral(val);
            lit = &createLiteral(sym, new ExpressionValue(v));
            break;
        }
        case MetaType::USTRING: {
            val = val.substr(0, 1 + val.find_last_not_of("u"));
            ustring u = interpretUStringLiteral(val);
            rstring v = ustring::toUTF8(u);
            lit = &createLiteral(sym, new ExpressionValue(v));
            break;
        }
        case MetaType::FLOAT32:
        case MetaType::FLOAT64: {
            istringstream istr(val, istringstream::in);
            double v;
            istr >> v;
            lit = &createLiteral(sym, new ExpressionValue(v));
            break;
        }
        case MetaType::INT8:
        case MetaType::INT16:
        case MetaType::INT32:
        case MetaType::INT64:
        case MetaType::UINT8:
        case MetaType::UINT16:
        case MetaType::UINT32:
        case MetaType::UINT64: {
            istringstream istr(val, istringstream::in);
            int64_t v;
            istr >> v;
            lit = &createLiteral(sym, new ExpressionValue(v));
            break;
        }
        case MetaType::ENUM: {
            lit = &createLiteral(sym, new ExpressionValue((rstring)val));
            break;
        }

        case MetaType::LIST: {
            lit = &createLiteral(sym, NULL);
            for (int32_t i = 0, count = AST::listLiteral::exprCount(ast); i < count; ++i) {
                AstNode& expr = AST::listLiteral::expr(ast, i);
                Literal& l = createLiteral(expr);
                lit->addListElement(l);
            }
            break;
        }

        case MetaType::TUPLE: {
            lit = &createLiteral(sym, NULL);
            for (int32_t i = 0, count = AST::tupleLiteral::attributeAssignCount(ast); i < count;
                 ++i) {
                AstNode& attrAssign = AST::tupleLiteral::attributeAssign(ast, i);
                AstNode& attrId = AST::attributeAssign::id(attrAssign);
                AstNode& expr = AST::attributeAssign::expr(attrAssign);
                string attrName = astText(attrId);
                Literal& l = createLiteral(expr);
                lit->addTupleElement(attrName, l);
            }
            break;
        }

        default: {
            cerr << "createLiteral failed for type " << sym.type() << endl;
            assert(false);
        }
    }

    assert(NULL != lit);
    return *lit;
}

void processCallExpression(const CallExpression& call,
                           string& compositeNameForAnnotation,
                           string& submissionTimeName,
                           string& defaultValue)
{
    assert(call.getFcn().name() == "getSubmissionTimeValue");
    compositeNameForAnnotation = call.getCompositeName();
    vector<Expression*> args = call.getArgs();
    // First arg is the submission name
    assert(args.size() > 0);
    const Expression& substNameExpr = *args[0];
    assert(substNameExpr.is<LiteralExpression>());
    submissionTimeName = substNameExpr.toString();
    if (args.size() > 1) {
        // Second are is the default value
        const Expression& defaultValueExpr = *args[1];
        assert(defaultValueExpr.is<LiteralExpression>());
        defaultValue = defaultValueExpr.toString();
    }
}

// We should have a list like [i,j] which needs to be converted to
// ["i", "j"]
string fixViewAttributes(const string& s)
{
    string fixed;
    for (size_t i = 0; i < s.size(); ++i) {
        char c = s[i];
        switch (c) {
            case ',':
                fixed.append("\", \"");
                ++i;
                break;
            case '[':
                fixed.append("[\"");
                break;
            case ']':
                fixed.append("\"]");
                break;
            default:
                fixed.push_back(c);
                break;
        }
    }
    return fixed;
}

string fixViewFilter(const string& s)
{
    string fixed("{");
    size_t pos1 = s.find("attr = ");
    fixed.append(s.substr(pos1, 7));
    pos1 += 7;
    fixed.append("\"");
    size_t pos2 = s.find(",", pos1);
    if (pos2 == string::npos) {
        pos2 = s.find("}");
    }
    fixed.append(s.substr(pos1, pos2 - pos1));
    fixed.append("\", ");

    pos1 = s.find("regEx = ");
    fixed.append(s.substr(pos1, 8));
    pos1 += 8;
    pos2 = s.find(",", pos1);
    if (pos2 == string::npos) {
        pos2 = s.find("}");
    }
    fixed.append(s.substr(pos1, pos2 - pos1));

    fixed.append("}");
    return fixed;
}

static SourceLocation const& newLoc(ParserContext& pCtx, AstNode& ast)
{
    return SourceLocationFactory::instance().create(pCtx, ast);
}

SPLSimpleAnnotation::KeyValuePair& IRGenerator::createKeyValuePair(ParserContext& pCtx,
                                                                   const string& tag,
                                                                   const string& keyText,
                                                                   AstNode& value,
                                                                   const string& compositeName)
{
    string compositeNameForAnnotation = compositeName;
    run(pCtx, value);
    const Expression& valueExpr = IRGeneratorHelper::getSimplifiedExpn(value);
    const RootTyp& valueType = valueExpr.getType();
    string valueStr;
    string submissionTimeName;
    string defaultValue;
    // See if we have a substitution variable for this value
    // The value expr should be either one that we can make a literal out of, or a function call
    // (with a possible cast)
    if (valueExpr.is<CastExpression>()) {
        const Expression& childExpr = valueExpr.as<CastExpression>().getExpn();
        assert(childExpr.is<CallExpression>());
        processCallExpression(childExpr.as<CallExpression>(), compositeNameForAnnotation,
                              submissionTimeName, defaultValue);
    } else if (valueExpr.is<CallExpression>()) {
        processCallExpression(valueExpr.as<CallExpression>(), compositeNameForAnnotation,
                              submissionTimeName, defaultValue);
    } else if (valueExpr.is<CustomLiteralExpression>()) {
        valueStr = "\"" + valueExpr.as<CustomLiteralExpression>().getId() + "\"";
    } else {
        assert(valueExpr.getKind() == Expression::Lit);
        const Literal* lit = valueExpr.getLiteral();
        assert(NULL != lit);
        ostringstream s;
        s << *lit;
        valueStr = s.str();
        // The following is a hack to fix something that is too late to change for spree
        if (tag == "view") {
            if (keyText == "attributes") {
                valueStr = fixViewAttributes(valueStr);
            } else if (keyText == "filter") {
                valueStr = fixViewFilter(valueStr);
            }
        }
    }
    SPLSimpleAnnotation::KeyValuePair* kvp = new SPLSimpleAnnotation::KeyValuePair(
      keyText, valueStr, compositeNameForAnnotation, submissionTimeName, defaultValue, valueType,
      newLoc(pCtx, value));
    return *kvp;
}

static string extractAttributeName(AstNode& attribExpr)
{
    string attributeName;
    if (AST::idT::is(attribExpr)) {
        attributeName = astText(attribExpr);
    } else if (AST::attributeExpr::is(attribExpr)) {
        AstNode& base = AST::attributeExpr::base(attribExpr);
        AstNode& id = AST::attributeExpr::id(attribExpr);
        attributeName = extractAttributeName(base);
        if (!attributeName.empty()) {
            attributeName += ".";
            attributeName += astText(id);
        }
    }
    return attributeName;
}

const SPLAnnotation* IRGenerator::createSPLAnnotation(ParserContext& pCtx,
                                                      AstNode& annot,
                                                      const string& compositeName)
{
    static std::tr1::unordered_map<string, const SourceLocation*> viewAnnotationNames;
    string compositeNameForAnnotation = compositeName;
    SymbolTableEntry& sym = astSymbol(annot);

    if (sym.is<SimpleAnnotationSym>()) {
        const SimpleAnnotationSym& simpleSym = sym.as<SimpleAnnotationSym>();
        const string& tag = simpleSym.name();
        const vector<pair<AstNode*, AstNode*> >& keyValuePairs = simpleSym.keyValuePairs();
        vector<SPLSimpleAnnotation::KeyValuePair*> keyValues;
        for (vector<pair<AstNode*, AstNode*> >::const_iterator it = keyValuePairs.begin();
             it != keyValuePairs.end(); ++it) {
            AstNode& key = *it->first;
            string keyText = astText(key);
            AstNode& value = *it->second;
            keyValues.push_back(&createKeyValuePair(pCtx, tag, keyText, value, compositeName));
        }
        return &AnnotationFactory::instance().createSimpleAnnotation(tag, keyValues,
                                                                     simpleSym.sourceLocation());
    } else if (sym.is<EventTimeAnnotationSym>()) {
        EventTimeAnnotationSym const& etSym = sym.as<EventTimeAnnotationSym>();
        vector<SPLSimpleAnnotation::KeyValuePair*> keyValues;
        string tag = "eventTime";
        AstNode& attributeAst = etSym.eventTimeAttribute();
        std::string attributeValue = extractAttributeName(attributeAst);
        run(pCtx, attributeAst);
        Expression& valueExpr = IRGeneratorHelper::getSimplifiedExpn(attributeAst);
        RootTyp const& valueType = valueExpr.getType();
        SPLSimpleAnnotation::KeyValuePair* kvp =
          new SPLSimpleAnnotation::KeyValuePair("eventTimeAttribute", attributeValue, compositeName,
                                                "", "", valueType, newLoc(pCtx, attributeAst));
        keyValues.push_back(kvp);

        if (etSym.resolution()) {
            keyValues.push_back(
              &createKeyValuePair(pCtx, tag, "resolution", *etSym.resolution(), compositeName));
        }
        if (etSym.lag()) {
            keyValues.push_back(&createKeyValuePair(pCtx, tag, "lag", *etSym.lag(), compositeName));
        }
        if (etSym.minimumGap()) {
            keyValues.push_back(
              &createKeyValuePair(pCtx, tag, "minimumGap", *etSym.minimumGap(), compositeName));
        }
        return &AnnotationFactory::instance().createEventTimeAnnotation(keyValues,
                                                                        etSym.sourceLocation());

    } else if (sym.is<ViewAnnotationSym>()) {
        const ViewAnnotationSym& viewSym = sym.as<ViewAnnotationSym>();
        vector<SPLSimpleAnnotation::KeyValuePair*> keyValues;

        string tag = "view";
        keyValues.push_back(
          &createKeyValuePair(pCtx, tag, "name", viewSym.viewName(), compositeName));
        keyValues.push_back(&createKeyValuePair(pCtx, tag, "port", viewSym.port(), compositeName));
        keyValues.push_back(
          &createKeyValuePair(pCtx, tag, "sampleSize", viewSym.sampleSize(), compositeName));
        if (NULL != viewSym.bufferSize()) {
            keyValues.push_back(
              &createKeyValuePair(pCtx, tag, "bufferSize", *viewSym.bufferSize(), compositeName));
        }
        if (NULL != viewSym.bufferTime()) {
            keyValues.push_back(
              &createKeyValuePair(pCtx, tag, "bufferTime", *viewSym.bufferTime(), compositeName));
        }
        if (NULL != viewSym.description()) {
            keyValues.push_back(
              &createKeyValuePair(pCtx, tag, "description", *viewSym.description(), compositeName));
        }
        if (NULL != viewSym.attributes()) {
            keyValues.push_back(
              &createKeyValuePair(pCtx, tag, "attributes", *viewSym.attributes(), compositeName));
        }
        if (NULL != viewSym.filter()) {
            keyValues.push_back(
              &createKeyValuePair(pCtx, tag, "filter", *viewSym.filter(), compositeName));
        }
        if (NULL != viewSym.activateOption()) {
            keyValues.push_back(&createKeyValuePair(pCtx, tag, "activateOption",
                                                    *viewSym.activateOption(), compositeName));
        }

        CompositeOperatorInstance& compositeOpInstance =
          SymbolTable::instance().currentCompositeInstance().compositeInstanceIR();
        const string& compositeInstanceName = compositeOpInstance.getInstanceName();
        return &AnnotationFactory::instance().createViewAnnotation(
          keyValues, viewSym.sourceLocation(), compositeInstanceName);
    } else if (sym.is<ParallelAnnotationSym>()) {
        const ParallelAnnotationSym& pSym = sym.as<ParallelAnnotationSym>();
        AstNode& astWidth = pSym.width();
        run(pCtx, astWidth);
        const Expression& widthExpr = IRGeneratorHelper::getSimplifiedExpn(astWidth);
        // We should have either a literal expression or an expression that ultimatly
        // is a getSubmissionTimeValue
        int64_t width = 0;
        string submissionTimeName;
        string defaultWidth;
        bool widthExprInError = false;
        if (widthExpr.is<LiteralExpression>()) {
            const LiteralExpression& lit = widthExpr.as<LiteralExpression>();
            ExpressionValue ev;
            lit.getExpressionValue(ev);
            ev.getIntegerValue(width);
            if (width <= 0) {
                widthExprInError = true;
            }
        } else {
            if (AST::castExpr::is(astWidth)) {
                // Walk the expression to get to the call.
                AstNode& expr = AST::castExpr::expr(astWidth);
                if (AST::callExpr::is(expr)) {
                    const Expression& cExpr = astExpression(expr);
                    const CallExpression& callExpr = cExpr.as<CallExpression>();
                    if (callExpr.getFcn().name() == "getSubmissionTimeValue") {
                        processCallExpression(callExpr, compositeNameForAnnotation,
                                              submissionTimeName, defaultWidth);
                        submissionTimeName =
                          submissionTimeName.substr(1, submissionTimeName.size() - 2);
                        if (!defaultWidth.empty()) {
                            string def = defaultWidth.substr(1, defaultWidth.size() - 2);
                            istringstream v(def);
                            bool done = v >> width;
                            if (!done) {
                                vector<Expression*> args = callExpr.getArgs();
                                const Expression& defaultValueExpr = *args[1];
                                SysOut::errorln(SPL_FRONT_END_SUB_TIME_DEFAULT_NOT_INT,
                                                defaultValueExpr.getLocation());
                            }
                        }
                    } else {
                        widthExprInError = true;
                    }
                } else {
                    widthExprInError = true;
                }
            } else {
                widthExprInError = true;
            }
        }
        if (widthExprInError) {
            SysOut::errorln(SPL_FRONT_END_INVALID_PARALLEL_WIDTH,
                            SourceLocationFactory::instance().create(pCtx, astWidth));
        }

        vector<string> hostTagList;
        AstNode* hostTags = pSym.hostTags();
        if (NULL != hostTags) {
            run(pCtx, *hostTags);
            const Expression& hostTagsExpr = astExpression(*hostTags);
            if (hostTagsExpr.is<LiteralExpression>()) {
                const Literal* hostTagsLit = hostTagsExpr.as<LiteralExpression>().getLiteral();
                assert(NULL != hostTagsLit);
                assert(hostTagsLit->getKind() == Literal::List);
                for (uint32 i = 0; i < hostTagsLit->numElements(); ++i) {
                    const Literal& lit = hostTagsLit->listSetElement(i);
                    string val = lit.primitive().get_rstring();
                    hostTagList.push_back(val);
                }
            } else {
                SysOut::errorln(SPL_FRONT_END_INVALID_PARALLEL_HOST_TAGS,
                                SourceLocationFactory::instance().create(pCtx, *hostTags));
            }
        }

        vector<SPLParallelAnnotation::Partition> partitions;
        const ParallelAnnotationSym::PartitionList& partitionAsts = pSym.partitions();
        for (ParallelAnnotationSym::PartitionList::const_iterator it = partitionAsts.begin();
             it != partitionAsts.end(); ++it) {
            const ParallelAnnotationSym::Partition& partition = *it;
            AstNode& port = partition.port();
            AstNode& attributes = partition.attributes();
            string portName = astText(port);
            for (vector<SPLParallelAnnotation::Partition>::iterator partitionIt =
                   partitions.begin();
                 partitionIt != partitions.end(); ++partitionIt) {
                if (partitionIt->port() == portName) {
                    SysOut::errorln(SPL_FRONT_END_PARALLEL_DUPLICATE_PARTITION_PORT(portName),
                                    SourceLocationFactory::instance().create(pCtx, port));
                }
            }
            std::tr1::unordered_set<std::string> attributesSeen;
            vector<string> attributeNames;
            assert(AST::listLiteral::is(attributes));
            for (int i = 0, n = AST::listLiteral::exprCount(attributes); i < n; i++) {
                AstNode& attribute = AST::listLiteral::expr(attributes, i);
                string attrName = extractAttributeName(attribute);
                if (attrName.empty()) {
                    SysOut::errorln(SPL_FRONT_END_PARALLEL_INVALID_ATTRIBUTE_TYPE,
                                    SourceLocationFactory::instance().create(pCtx, attribute));
                }
                std::tr1::unordered_set<std::string>::const_iterator seen =
                  attributesSeen.find(attrName);
                if (seen != attributesSeen.end()) {
                    SysOut::errorln(SPL_FRONT_END_PARALLEL_DUPLICATE_ATTRIBUTE(attrName),
                                    SourceLocationFactory::instance().create(pCtx, attribute));
                } else {
                    attributesSeen.insert(attrName);
                    attributeNames.push_back(attrName);
                }
            }
            partitions.push_back(SPLParallelAnnotation::Partition(portName, attributeNames));
        }

        ParallelAnnotationSym::BroadcastList broadcastPorts;
        AstNode* broadcasts = pSym.broadcasts();
        if (broadcasts && AST::listLiteral::is(*broadcasts)) {
            for (int i = 0, n = AST::listLiteral::exprCount(*broadcasts); i < n; i++) {
                // Populate broadcastPorts from broadcasts.
                // Check for duplicates, and ports also used with
                // partitions.
                AstNode& broadcast = AST::listLiteral::expr(*broadcasts, i);
                string broadcastPort = astText(broadcast);

                if (std::find(broadcastPorts.begin(), broadcastPorts.end(), broadcastPort) !=
                    broadcastPorts.end()) {
                    SysOut::errorln(SPL_FRONT_END_PARALLEL_DUPLICATE_BROADCAST_PORT(broadcastPort),
                                    SourceLocationFactory::instance().create(pCtx, *broadcasts));
                }

                for (vector<SPLParallelAnnotation::Partition>::iterator partitionIt =
                       partitions.begin();
                     partitionIt != partitions.end(); ++partitionIt) {
                    if (partitionIt->port() == broadcastPort) {
                        SysOut::errorln(
                          SPL_FRONT_END_PARALLEL_PARTITION_AND_BROADCAST_PORT(broadcastPort),
                          SourceLocationFactory::instance().create(pCtx, *broadcasts));
                    }
                }
                broadcastPorts.push_back(broadcastPort);
            }
        }
        const SPLParallelAnnotation& splAnnot =
          AnnotationFactory::instance().createParallelAnnotation(
            (int32_t)width, compositeNameForAnnotation, submissionTimeName, defaultWidth,
            partitions, hostTagList, broadcastPorts, pSym.sourceLocation());
        return &splAnnot;
    } else if (sym.is<ThreadingAnnotationSym>()) {
        const ThreadingAnnotationSym& pSym = sym.as<ThreadingAnnotationSym>();
        vector<SPLSimpleAnnotation::KeyValuePair*> keyValues;
        string tag = "threading";
        keyValues.push_back(
          &createKeyValuePair(pCtx, tag, "model", pSym.model(), compositeNameForAnnotation));
        if (NULL != pSym.threads()) {
            keyValues.push_back(&createKeyValuePair(pCtx, tag, "threads", *pSym.threads(),
                                                    compositeNameForAnnotation));
        }
        if (NULL != pSym.elastic()) {
            keyValues.push_back(&createKeyValuePair(pCtx, tag, "elastic", *pSym.elastic(),
                                                    compositeNameForAnnotation));
        }
        const SPLThreadingAnnotation& splAnnot =
          AnnotationFactory::instance().createThreadingAnnotation(keyValues, pSym.sourceLocation());
        return &splAnnot;
    }

    // We should never get here
    assert(0);
    return (SPLAnnotation*)NULL;
}

// ======================== class IRGenerator ========================
void IRGenerator::run(ParserContext& pCtx, AstNode& ast)
{
    IRGenerator visitor(pCtx);
    MODEL("PhaseStart(IRGenerator, " << (void*)&ast << ")\n");
    visitor.visit(ast);
    MODEL("PhaseEnd(IRGenerator, " << (void*)&ast << ")\n");
}

IRGenerator::IRGenerator(ParserContext& pCtx)
  : AstVisitorPushPop(pCtx, Debug::TraceIRGeneration)
  , _state(*new State())
  , _pool(AstPool::instance())
{}

void IRGenerator::visit(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast
                               << ") stage is " << astStage(ast).toString(),
           Debug::TraceIRGeneration);
    if (astStage(ast) < FrontEndStage::DONE) {
        setAstStage(_pool, ast, FrontEndStage::DONE);
        AstVisitorPushPop::visit(ast);
    }
}

// ----- The compilation unit is the start symbol of the SPL grammar.
void IRGenerator::visitNamespace(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    if (AST::splNamespace::has_name(ast)) {
        AstNode& nameAst = AST::splNamespace::name(ast);
        AstNode* a = &AST::namespaceName::body(nameAst);
        while (AST::attributeExpr::is(*a)) {
            const string& n = astText(AST::attributeExpr::id(*a));
            if (Utility::reservedName(n)) {
                SysOut::errorln(SPL_FRONT_END_NS_RESERVED(n),
                                SourceLocationFactory::instance().create(_pCtx, nameAst));
            }
            a = &AST::attributeExpr::base(*a);
        }
        const string& n = astText(*a);
        if (Utility::reservedNamespace(n)) {
            SysOut::errorln(SPL_FRONT_END_NS_RESERVED(n),
                            SourceLocationFactory::instance().create(_pCtx, nameAst));
        }
    }
}

void IRGenerator::visitNamespaceName(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    /* skip (name gets converted to IR by parent rules) */
}

// ----- Composite operators are defined at the top-level in a namespace.
void IRGenerator::visitCompositeDef(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    _state._visitingCompositeDef = true;
    AstVisitorPushPop::visitCompositeDef(ast);
    CompositeDefinition& compOpDef = astSymbol(ast).as<CompositeDefSym>().compositeDefIR();
    compOpDef.setEndLocation(
      SourceLocationFactory::instance().create(_pCtx, AST::compositeDef::end(ast)));
    // Add in all the saved information
    for (uint32_t i = 0; i < _state._configs.size(); i++) {
        compOpDef.addConfig(*_state._configs[i]);
    }
    _state._configs.clear();
    // Any non-default params?
    bool hasNonDefaultParams = false;
    AstNode& body = AST::compositeDef::compositeBody(ast);
    if (AST::compositeBody::has_formal(body)) {
        AstNode& formalsAst = AST::compositeBody::formal(body);
        for (int i = 0, n = AST::formalClause::compositeFormalCount(formalsAst); i < n; i++) {
            AstNode& formalAst = AST::formalClause::compositeFormal(formalsAst, i);
            AstNode& formalInitAst = AST::compositeFormal::compositeFormalInit(formalAst);
            if (!AST::compositeFormalInit::has_opActual(formalInitAst)) {
                hasNonDefaultParams = true;
                break;
            }
        }
    }
    // Do we have no inputs or outputs?
    if (compOpDef.getNumInputs() == 0 && compOpDef.getNumOutputs() == 0 && !hasNonDefaultParams) {
        compOpDef.setPotentialMain(true);
        // Allow just the composite name for convenience, but prefer the whole name
        CompilerConfiguration& config = CompilerConfiguration::instance();
        string mainName = config.getStringValue(CompilerConfiguration::MainComposite);
        const string& name = compOpDef.getName();
        if (name == mainName || (compOpDef.getNamespace() + "::" + name) == mainName) {
            // This is the main composite, whose implicit instance serves as root for all composite
            // operator expansion.
            compOpDef.setMain(true);
        }
    }
    _state._visitingCompositeDef = false;
}

void IRGenerator::visitCompositeHead(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    CompositeDefSym const& compositeDefSym = SymbolTable::instance().currentCompositeDef();
    CompositeDefinition& compOpDef =
      UsedCompositeDefinitions::instance().add(compositeDefSym.fullName());

    compOpDef.setStartLocation(compositeDefSym.sourceLocation());
    uint32_t toolkitIndex = FrontEndDriver::instance().getToolkitIndex();
    compOpDef.setToolkitIndex(toolkitIndex);
    const Toolkit& tk = *PathSearch::instance().toolkits()[toolkitIndex];
    CompilationState::instance().setToolkitUsed(tk);

    compositeDefSym.setCompositeDefIR(compOpDef);
    AstNode& docAst = AST::compositeHead::splDoc(ast);
    if (AST::splDoc::has_comment(docAst)) {
        compOpDef.setSplDocComment(astText(AST::splDoc::comment(docAst)));
    }
    if (!AST::noneT::is(AST::compositeHead::compositeIn(ast))) {
        compOpDef.setNumInputs(
          AST::compositeIn::compositePortCount(AST::compositeHead::compositeIn(ast)));
    }
    if (!AST::noneT::is(AST::compositeHead::compositeOut(ast))) {
        compOpDef.setNumOutputs(
          AST::compositeOut::compositePortCount(AST::compositeHead::compositeOut(ast)));
    }

    // Do we have any annotations?
    AstNode& annotations = AST::compositeHead::annotations(ast);
    for (int i = 0, sz = astSize(annotations); i < sz; ++i) {
        AstNode& annot = AST::annotations::annotation(annotations, i);
        const SPLAnnotation* splAnnot =
          createSPLAnnotation(_pCtx, annot, compositeDefSym.fullName());
        if (NULL != splAnnot) {
            compOpDef.addAnnotation(*splAnnot);
        }
    }
}

void IRGenerator::visitConfigItem(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    _symTab.pushOld(ast);
    for (int i = 0, n = AST::config::exprCount(ast); i < n; i++) {
        visit(AST::config::expr(ast, i));
    }
    _symTab.pop(ast);
    Config* c = IRGeneratorHelper::createConfig(_pCtx, ast, &_state);
    if (NULL != c) {
        _state._configs.push_back(c);
    }
}

// ----- Streams are defined in a composite operator's graph clause.
void IRGenerator::visitOpInvoke(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
#if 0
        // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
        assert(NULL == _state._stateLogic && NULL == _state._processLogic &&
               0 == _state._portTupleLogic.size() && 0 == _state._portPunctLogic.size() && 0 == _state._portWatermarkLogic.size());
#else
    assert(NULL == _state._stateLogic && NULL == _state._processLogic &&
           0 == _state._portTupleLogic.size() && 0 == _state._portPunctLogic.size());
#endif
    AstVisitorPushPop::visitOpInvoke(ast);
    SymbolTableEntry& opSym = astSymbol(AST::opInvokeHead::opName(AST::opInvoke::head(ast)));
    if (opSym.is<CompositeDefSym>()) {
        OpInvokeSym& opInvokeSym = astSymbol(ast).as<OpInvokeSym>();
        AstNode& headAst = AST::opInvoke::head(ast);
        AstNode& annotations = AST::opInvokeHead::annotations(headAst);
        string compositeName = SymbolTable::instance().currentCompositeDef().fullName();
        for (int i = 0, sz = astSize(annotations); i < sz; ++i) {
            AstNode& annot = AST::annotations::annotation(annotations, i);
            const SPLAnnotation* splAnnot = createSPLAnnotation(_pCtx, annot, compositeName);
            if (NULL != splAnnot) {
                opInvokeSym.addAnnotation(*splAnnot);
            }
        }

        // clean-ups to file-local variables
        for (uint32_t i = 0, n = _state._configs.size(); i < n; i++) {
            delete _state._configs[i];
        }
        _state._configs.clear();
        for (uint32_t i = 0, n = _state._outputPorts.size(); i < n; i++) {
            delete _state._outputPorts[i];
        }
        _state._outputPorts.clear();
        for (uint32_t i = 0, n = _state._inputPorts.size(); i < n; i++) {
            delete _state._inputPorts[i];
        }
        _state._inputPorts.clear();
        _state._inputUses.clear();
        _state._outputDefs.clear();
        assert(0 == _state._params.size());
        assert(NULL == _state._stateLogic);
        assert(NULL == _state._processLogic);
        assert(0 == _state._portTupleLogic.size());
        assert(0 == _state._portPunctLogic.size());
#if 0
            // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
            assert(0 == _state._portWatermarkLogic.size());
#endif
        assert(0 == _state._windows.size());
        assert(0 == _state._outputs.size());
        // skip opInvokes to composites, their expansion happens in FrontEndDriver
    } else {
        assert(opSym.is<PrimitiveOperatorSym>());
        int32_t toolkitIndex = opSym.as<PrimitiveOperatorSym>().toolkitIndex();
        // Figure out the name of the operator.
        CompositeOperatorInstance& compositeOpInstance =
          SymbolTable::instance().currentCompositeInstance().compositeInstanceIR();
        SymbolTableEntry& opInvokeSym = astSymbol(ast);
        string name = compositeOpInstance.composite().isMain()
                        ? opInvokeSym.name()
                        : compositeOpInstance.getInstanceName() + "." + opInvokeSym.name();
        if (Utility::reservedName(name)) {
            SysOut::errorln(SPL_FRONT_END_OPERATOR_CPP_NAME_CLASH(name),
                            astSymbol(ast).sourceLocation());
        }
        // Compute the source location and instantiate the PrimitiveOperatorInstance IR
        // The source region needs to be the beginning of the outputs, not any annotations that may
        // come before
        AstNode& headAst = AST::opInvoke::head(ast);
#if 0
            AstNode & outputs = AST::opInvokeHead::outputs(headAst);
            AstNode * output = astChildWithLeftmostLocation(outputs);
            SourceLocation const * startLoc = NULL;
            if (NULL != output) {
                startLoc = &SourceLocationFactory::instance().create(_pCtx, *output);
            } else {
                startLoc = &astSymbol(ast).sourceLocation();
            }
#endif
        SourceLocation const* startLoc = &astSymbol(ast).sourceLocation();
        assert(NULL != startLoc);
        SourceLocation const& endLoc =
          SourceLocationFactory::instance().create(_pCtx, AST::opInvoke::end(ast));
        PrimitiveOperatorInstance& op = *new PrimitiveOperatorInstance(
          *startLoc, endLoc, name, opSym.fullName(), &compositeOpInstance, toolkitIndex);

        AstNode& docAst = AST::opInvokeHead::splDoc(headAst);
        if (AST::splDoc::has_comment(docAst)) {
            op.setSplDocComment(astText(AST::splDoc::comment(docAst)));
        }

        // Add any annotations
        AstNode& annotations = AST::opInvokeHead::annotations(headAst);
        string compositeName = SymbolTable::instance().currentCompositeDef().fullName();
        for (int i = 0, sz = astSize(annotations); i < sz; ++i) {
            AstNode& annot = AST::annotations::annotation(annotations, i);
            const SPLAnnotation* splAnnot = createSPLAnnotation(_pCtx, annot, compositeName);
            if (NULL != splAnnot) {
                op.addAnnotation(*splAnnot);
            }
        }

        // Add in all the saved information
        for (uint32_t i = 0; i < _state._configs.size(); i++) {
            op.addConfig(*_state._configs[i]);
        }
        _state._configs.clear();

        // Ports
        StreamsUseDef& sud = StreamsUseDef::instance();
        bool usedAlias = AST::as::is(AST::opInvokeHead::alias(AST::opInvoke::head(ast)));
        for (uint32_t i = 0; i < _state._outputPorts.size(); i++) {
            PrimitiveOperatorInstance::Port& p = *_state._outputPorts[i];
            op.addOutputPort(p);
            if (usedAlias || i > 0) { // if have not used an alias then the name
                                      // for the first port's stream is already used
                op.addName(p.getStreamName());
            }
            MODEL("AddOutputPort(" << (void*)&op << ", " << (void*)&p << ")\n");
        }
        _state._outputPorts.clear();
        for (uint32_t i = 0; i < _state._inputPorts.size(); i++) {
            op.addInputPort(*_state._inputPorts[i]);
            MODEL("AddInputPort(" << (void*)&op << ", " << (void*)_state._inputPorts[i] << ")\n");
        }
        for (uint32_t i = 0; i < _state._inputUses.size(); i++) {
            sud.addUse(_state._inputUses[i].first, name, _state._inputUses[i].second,
                       StreamUseDefLocation::input);
        }
        _state._inputUses.clear();
        for (uint32_t i = 0; i < _state._outputDefs.size(); i++) {
            sud.setDef(_state._outputDefs[i], name, i, StreamUseDefLocation::output);
        }
        _state._outputDefs.clear();

        // Logic
        PrimitiveOperatorInstance::Logic& logic = op.getLogic();
        if (_state._stateLogic) {
            logic.setState(*_state._stateLogic);
        }
        if (_state._processLogic) {
            logic.setProcess(*_state._processLogic);
            if (_state._inputPorts.size()) {
                SysOut::errorln(SPL_FRONT_END_ONPROCESS_NOT_CUSTOM,
                                _state._processLogic->location());
            }
        }

        bool isCustom = op.getKind() == "spl.utility::Custom";
        for (uint32_t i = 0; i < _state._portTupleLogic.size(); i++) {
            const pair<uint32_t, Statement*>& p = _state._portTupleLogic[i];
            logic.addTuplePort(p.first, *p.second, isCustom ? &op.getInputPort(p.first) : NULL);
        }
        for (uint32_t i = 0; i < _state._portPunctLogic.size(); i++) {
            const pair<uint32_t, Statement*>& p = _state._portPunctLogic[i];
            logic.addPunctPort(p.first, *p.second, isCustom ? &op.getInputPort(p.first) : NULL);
        }
#if 0
            // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
            for(uint32_t i = 0; i < _state._portWatermarkLogic.size(); i++) {
                const pair<uint32_t, Statement *> & p = _state._portWatermarkLogic[i];
                logic.addWatermarkPort(p.first, *p.second, isCustom ? &op.getInputPort(p.first) : NULL);
            }
#endif
        _state._stateLogic = NULL;
        _state._processLogic = NULL;
        _state._portTupleLogic.clear();
        _state._portPunctLogic.clear();
#if 0
            // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
            _state._portWatermarkLogic.clear();
#endif

        // Clear after state uses this
        _state._inputPorts.clear();

        // Windows
        for (uint32_t i = 0; i < _state._windows.size(); i++) {
            op.addWindow(*_state._windows[i]);
        }
        _state._windows.clear();
        // Params
        for (uint32_t i = 0; i < _state._params.size(); i++) {
            op.addParam(*_state._params[i]);
        }
        _state._params.clear();
        // Outputs
        string lastName;
        uint32_t lastPort = 0;
        for (uint32_t i = 0; i < _state._outputs.size(); i++) {
            const pair<string, const BinaryExpression*>& p = _state._outputs[i];
            // Find the correct port
            uint32_t port = 0;
            // Match the stream name;
            if (lastName == p.first) {
                port = lastPort;
            } else {
                for (uint32_t j = 0, n = op.numOutputPorts(); j < n; j++) {
                    PrimitiveOperatorInstance::Port const& portJ = op.getOutputPort(j);
                    if (p.first == portJ.getStreamName() || p.first == portJ.getAlias()) {
                        port = j;
                        lastPort = j;
                        lastName = p.first;
                        break;
                    }
                }
            }
            assert(lastName == p.first);
            op.getOutput(port).add(*p.second);
        }
        _state._outputs.clear();
        // Add it to the compositeOpInstance
        compositeOpInstance.addPrimitiveInstance(op);
        // Add it to the list of used operators
        UsedOperatorInvocations::instance().add(op);
    }
}

void IRGenerator::visitOpOutput(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    SourceLocation const& loc = SourceLocationFactory::instance().create(_pCtx, ast);
    StreamSym& sym = astSymbol(AST::opOutput::id(ast)).as<StreamSym>();
    SPLDBG("output stream defn: " << sym, Debug::ShowStreams);
    _state._outputDefs.push_back(sym.localName());
    TupleTyp const& type = sym.type().as<TupleTyp>();
    PrimitiveOperatorInstance::Port& portIR = *new PrimitiveOperatorInstance::Port(&type, loc);
    portIR.addStream(sym.fullName());
    portIR.addActualStream(sym.localName());
    _state._outputPorts.push_back(&portIR);
    if (AST::opOutput::has_alias(ast)) {
        portIR.setAlias(astText(AST::as::id(AST::opOutput::alias(ast))));
        portIR.setName(portIR.getAlias());
    } else {
        portIR.setName(sym.name());
    }
}

void IRGenerator::visitPortInputs(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    SourceLocation const& loc = SourceLocationFactory::instance().create(_pCtx, ast);
    assert(1 <= AST::portInputs::idCount(ast));
    SymbolTableEntry& idSym0 = astSymbol(AST::portInputs::id(ast, 0));
    if (!idSym0.type().is<TupleTyp>()) {
        return;
    }
    TupleTyp const& type = idSym0.type().as<TupleTyp>();
    PrimitiveOperatorInstance::Port& portIR = *new PrimitiveOperatorInstance::Port(&type, loc);
    vector<string> bundle;
    for (uint32_t i = 0, n = AST::portInputs::idCount(ast); i < n; i++) {
        SymbolTableEntry& streamSym = astSymbol(AST::portInputs::id(ast, i));
        if (streamSym.is<StreamSym>()) {
            const StreamSym& s = streamSym.as<StreamSym>();
            const string aName = s.localName();
            bundle.push_back(s.fullName());
            portIR.addActualStream(aName);
            _state._inputUses.push_back(make_pair(aName, _state._inputPorts.size()));
        } else {
            const CompositeInputPortSym& c = streamSym.as<CompositeInputPortSym>();
            c.pushBundle(bundle);
            const string aName = c.instance().fullName() + '.' + c.name();
            portIR.addActualStream(aName);
            _state._inputUses.push_back(make_pair(aName, _state._inputPorts.size()));
        }
    }
    for (size_t i = 0, n = bundle.size(); i < n; i++) {
        portIR.addStream(bundle.at(i));
    }
    _state._inputPorts.push_back(&portIR);
    if (AST::portInputs::has_alias(ast)) {
        portIR.setAlias(astText(AST::as::id(AST::portInputs::alias(ast))));
        portIR.setName(portIR.getAlias());
    } else {
        if (idSym0.is<StreamSym>()) {
            portIR.setName(idSym0.as<StreamSym>().name());
        } else {
            portIR.setName(idSym0.as<CompositeInputPortSym>().name());
        }
    }
}

void IRGenerator::visitLogicState(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitLogicState(ast);
    Statement* s = getAstStatement(AST::opInvokeLogicState::stmt(ast));
    assert(NULL != s);
    _state._stateLogic = s;
}

void IRGenerator::visitOnProcessLogic(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitOnProcessLogic(ast);
    Statement* s = getAstStatement(AST::opInvokeLogicOnProcess::stmt(ast));
    _state._processLogic = s;
}

void IRGenerator::visitOnTupleLogic(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitOnTupleLogic(ast);
    Statement* s = getAstStatement(AST::opInvokeLogicOnTuple::stmt(ast));
    AstNode& idAst = AST::opInvokeLogicOnTuple::id(ast);
    int portIndex = -1;
#ifndef NDEBUG
    int countFound =
#endif // NDEBUG
      findStreamInOpInvoke(astText(idAst), _symTab.currentOpInvoke().ast(), NULL, NULL, &portIndex,
                           NULL);
    assert(0 < countFound && -1 != portIndex);
    _state._portTupleLogic.push_back(make_pair(portIndex, s));
}

void IRGenerator::visitOnPunctLogic(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitOnPunctLogic(ast);
    Statement* s = getAstStatement(AST::opInvokeLogicOnPunct::stmt(ast));
    AstNode& idAst = AST::opInvokeLogicOnPunct::id(ast);
    int portIndex = -1;
#ifndef NDEBUG
    int countFound =
#endif // NDEBUG
      findStreamInOpInvoke(astText(idAst), _symTab.currentOpInvoke().ast(), NULL, NULL, &portIndex,
                           NULL);
    assert(0 < countFound && -1 != portIndex);
    _state._portPunctLogic.push_back(make_pair(portIndex, s));
}

#if 0
    // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
    void IRGenerator::visitOnWatermarkLogic(AstNode & ast)
    {
        SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
        AstVisitorPushPop::visitOnWatermarkLogic(ast);
        Statement * s = getAstStatement(AST::opInvokeLogicOnWatermark::stmt(ast));
        AstNode & idAst = AST::opInvokeLogicOnWatermark::id(ast);
        int portIndex = -1;
#ifndef NDEBUG
        int countFound =
#endif // NDEBUG
            findStreamInOpInvoke(astText(idAst), _symTab.currentOpInvoke().ast(), NULL, NULL, &portIndex, NULL);
        assert(0 < countFound && -1 != portIndex);
        _state._portWatermarkLogic.push_back(make_pair(portIndex, s));
    }
#endif

void IRGenerator::visitOpInvokeWindow(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitOpInvokeWindow(ast);
    // Input port Name
    AstNode& idAst = AST::opInvokeWindow::id(ast);
    string name = astText(idAst);
    SourceLocation const& loc = astSymbol(ast).sourceLocation();
    uint32_t port =
      nameToPortIndex(IRGeneratorHelper::actualStreamName(astSymbol(idAst)), loc,
                      IRGeneratorHelper::InputPort, IRGeneratorHelper::AnyPortName, _state);
    IRGeneratorHelper::errorCheckWindow(_pCtx, ast, port, _state);
    bool isPartitioned = false;
    bool isTumbling = false;
    bool isSliding = false;
    bool isEvent = false;

    bool hasEventPhrase = false;    // Has a phrase that requires event time window
    bool hasNonEventPhrase = false; // Has a phrase that requires non-event time window
    PrimitiveOperatorInstance::Window::LogicKind policy1 = PrimitiveOperatorInstance::Window::Count;
    PrimitiveOperatorInstance::Window::LogicKind policy2 = PrimitiveOperatorInstance::Window::Count;
    const Expression *v1 = NULL, *v2 = NULL;
    const Expression *id1 = NULL, *id2 = NULL;
    PrimitiveOperatorInstance::Window::PartitionEvictionPolicy pep =
      PrimitiveOperatorInstance::Window::PartitionAge;
    const Expression* pepValue = NULL;

    // note id1 is attribute for event-time windows
    const Expression* intervalDuration = NULL;
    const Expression* creationPeriod = NULL;
    const Expression* discardAge = NULL;
    const Expression* intervalOffset = NULL;

    // A state to figure out if we are ordered
    enum
    {
        Start,
        Kind,
        Buckets,
        Options,
        Eviction,
        Trigger,
        Partitioned,
        Done
    } state = Start;
    bool misOrdered = false;

    // Helpers
#define NeedState(NEED, NEWSTATE)                                                                  \
    do {                                                                                           \
        if (state != NEED)                                                                         \
            misOrdered = true;                                                                     \
        state = NEWSTATE;                                                                          \
    } while (false)

#define NeedState2(NEED1, NEED2, NEWSTATE)                                                         \
    do {                                                                                           \
        if (state != NEED1 && state != NEED2)                                                      \
            misOrdered = true;                                                                     \
        state = NEWSTATE;                                                                          \
    } while (false)

#define CheckEvictionTrigger()                                                                     \
    do {                                                                                           \
        if (state == Kind)                                                                         \
            state = Eviction;                                                                      \
        else if (state == Eviction && isSliding)                                                   \
            state = Trigger;                                                                       \
        else                                                                                       \
            misOrdered = true;                                                                     \
    } while (false)

    for (uint32_t i = 0, n = AST::opInvokeWindow::exprCount(ast); i < n; i++) {
        AstNode* a = &AST::opInvokeWindow::expr(ast, i);
        string token;
        if (AST::callExpr::is(*a)) {
            token = astText(AST::callExpr::target(*a));
        } else {
            token = astText(*a);
        }
        if (token == "partitioned") {
            isPartitioned = true;
            if (isEvent) {
                NeedState(Options, Partitioned);
            } else {
                NeedState2(Eviction, Trigger, Partitioned);
            }
        } else if (token == "tumbling") {
            isTumbling = true;
            hasNonEventPhrase = true;
            NeedState(Start, Kind);
        } else if (token == "sliding") {
            isSliding = true;
            hasNonEventPhrase = true;
            NeedState(Start, Kind);
        } else if (token == "timeInterval") {
            isEvent = true;
            hasEventPhrase = true;
            NeedState(Start, Buckets);
        } else if (token == "punct") {
            if (!v1) {
                policy1 = PrimitiveOperatorInstance::Window::Punct;
            } else {
                policy2 = PrimitiveOperatorInstance::Window::Punct;
            }
            CheckEvictionTrigger();
        } else if (token == "time") {
            hasNonEventPhrase = true;
            if (!v1) {
                policy1 = PrimitiveOperatorInstance::Window::Time;
                v1 = &astExpression(AST::callExpr::actual(*a, 0));
            } else {
                policy2 = PrimitiveOperatorInstance::Window::Time;
                v2 = &astExpression(AST::callExpr::actual(*a, 0));
            }
            CheckEvictionTrigger();
        } else if (token == "count") {
            hasNonEventPhrase = true;
            if (!v1) {
                policy1 = PrimitiveOperatorInstance::Window::Count;
                v1 = &astExpression(AST::callExpr::actual(*a, 0));
            } else {
                policy2 = PrimitiveOperatorInstance::Window::Count;
                v2 = &astExpression(AST::callExpr::actual(*a, 0));
            }
            CheckEvictionTrigger();
        } else if (token == "delta") {
            hasNonEventPhrase = true;
            if (!v1) {
                policy1 = PrimitiveOperatorInstance::Window::Delta;
                id1 = &astExpression(AST::callExpr::actual(*a, 0));
                v1 = &astExpression(AST::callExpr::actual(*a, 1));
            } else {
                policy2 = PrimitiveOperatorInstance::Window::Delta;
                id2 = &astExpression(AST::callExpr::actual(*a, 0));
                v2 = &astExpression(AST::callExpr::actual(*a, 1));
            }
            CheckEvictionTrigger();
        } else if (token == "partitionAge") {
            pep = PrimitiveOperatorInstance::Window::PartitionAge;
            pepValue = &astExpression(AST::callExpr::actual(*a, 0));
            NeedState(Partitioned, Done);
        } else if (token == "partitionCount") {
            pep = PrimitiveOperatorInstance::Window::PartitionCount;
            pepValue = &astExpression(AST::callExpr::actual(*a, 0));
            NeedState(Partitioned, Done);
        } else if (token == "tupleCount") {
            pep = PrimitiveOperatorInstance::Window::TupleCount;
            pepValue = &astExpression(AST::callExpr::actual(*a, 0));
            NeedState(Partitioned, Done);
        } else if (token == "intervalDuration") {
            hasEventPhrase = true;
            intervalDuration = &astExpression(AST::callExpr::actual(*a, 0));
            // This can be accepted if the state is Buckets or Options,
            // and the kind is event.  Transition to options.
            // A check for duplicate intervalDuration expressions has previously
            // been performed.
            NeedState2(Buckets, Options, Options);

        } else if (token == "creationPeriod") {
            hasEventPhrase = true;
            creationPeriod = &astExpression(AST::callExpr::actual(*a, 0));
            NeedState2(Buckets, Options, Options);

        } else if (token == "discardAge") {
            hasEventPhrase = true;
            discardAge = &astExpression(AST::callExpr::actual(*a, 0));
            NeedState2(Buckets, Options, Options);

        } else if (token == "intervalOffset") {
            hasEventPhrase = true;
            intervalOffset = &astExpression(AST::callExpr::actual(*a, 0));
            NeedState2(Buckets, Options, Options);
        }
    }
    // Parsed it all
    if (!isTumbling && !isSliding && !isEvent) {
        // SPL 1 mistranslation?
        if (v2) {
            isSliding = true;
        } else {
            isTumbling = true;
        }
    }

    if ((hasEventPhrase && hasNonEventPhrase) || (isEvent + isTumbling + isSliding > 1)) {
        SysOut::errorln(SPL_FRONT_END_MIX_TIMEINTERVAL_WITH_NON_TIMEINTERVAL, loc);

        // For now, suppress the misordered message when this happens
        misOrdered = false;
    }

    PrimitiveOperatorInstance::Window* w = NULL;
    if (isTumbling) {
        w = new PrimitiveOperatorInstance::Window(loc, name, port, policy1, id1, v1);
    } else if (isEvent) {
        w = new PrimitiveOperatorInstance::Window(loc, name, port, intervalDuration, creationPeriod,
                                                  discardAge, intervalOffset);
    } else { // sliding
        if (!v2) {
            // defaults to count(1)
            policy2 = PrimitiveOperatorInstance::Window::Count;
            SPL::int32 one = 1;
            const RootTyp& type = TypeFactory::instance().primitiveT(MetaType::INT32);
            Literal* lit = new Literal(type, new ExpressionValue(one));
            v2 = new LiteralExpression(type, SourceLocation::invalidSourceLocation, *lit);
        }
        w = new PrimitiveOperatorInstance::Window(loc, name, port, policy1, policy2, id1, v1, id2,
                                                  v2);
    }
    if (isPartitioned) {
        w->setIsPartitioned(true);
        if (pepValue) {
            w->setPartitionEvictionPolicy(pep, *pepValue);
        }
    }
    _state._windows.push_back(w);

    if (!misOrdered) {
        return;
    }

    // Okay, the user got something wrong.  Complain
    CompilerConfiguration& config = CompilerConfiguration::instance();
    if (!config.isSet(CompilerConfiguration::WindowClauseOrderingBehavior)) {
        // default behaviour is now a warning
        SysOut::warnln(SPL_FRONT_END_WINDOW_CLAUSE_WARNING, loc);
    } else {
        if (config.getStringValue(CompilerConfiguration::WindowClauseOrderingBehavior) ==
            "strict") {
            SysOut::errorln(SPL_FRONT_END_WINDOW_CLAUSE_ERROR, loc);
        }
        // else do nothing
    }
}

void IRGenerator::visitOpInvokeActual(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitOpInvokeActual(ast);
    OpInvokeSym const& opInvokeSym = SymbolTable::instance().currentOpInvoke();
    if (opInvokeSym.op()->is<CompositeDefSym>()) {
        return;
    }
    AstNode& opActualAst = AST::opInvokeActual::opActual(ast);
    vector<const Expression*>* expnList = new vector<const Expression*>;
    if (AST::opActualExprs::is(opActualAst)) {
        for (int i = 0, n = AST::opActualExprs::exprCount(opActualAst); i < n; i++) {
            const Expression& expr = astExpression(AST::opActualExprs::expr(opActualAst, i));
            expnList->push_back(&expr);
        }
    } else {
        const Expression& expr = astExpression(AST::opActualName::name(opActualAst));
        expnList->push_back(&expr);
    }
    SymbolTableEntry& sym = astSymbol(ast);
    _state._params.push_back(new Param(sym.name(), sym.sourceLocation(), *expnList));
}

void IRGenerator::visitOpInvokeOutput(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    _symTab.pushOld(ast);
    OpInvokeOutputSym& sym = astSymbol(ast).as<OpInvokeOutputSym>();
    for (int i = 0, n = AST::opInvokeOutput::exprCount(ast); i < n; i++) {
        AstNode& exprAst = AST::opInvokeOutput::expr(ast, i);
        visit(AST::infixExpr::lhs(exprAst));
        sym.switchRight();
        visit(AST::infixExpr::rhs(exprAst));
        SymbolTableEntry& lhs = astSymbol(AST::infixExpr::lhs(exprAst));
        SourceLocation const& loc =
          SourceLocationFactory::instance().create(_pCtx, AST::infixExpr::lhs(exprAst));
        Expression* left = lhs.genExpression(loc);
        assert(left);
        SourceLocation const& lLoc = lhs.sourceLocation();
        RootTyp const& type = lhs.type();
        Expression& right = astExpression(AST::infixExpr::rhs(exprAst));
        BinaryExpression& ir = *new BinaryExpression(type, lLoc, Expression::eq, *left, right);
        setAstExpression(_pool, exprAst, ir);
        SPLDBG(__PRETTY_FUNCTION__ << ": AssociateExpression: ast: " << (void*)&exprAst
                                   << " expr: " << (void*)&ir,
               Debug::ShowSymbols);
        MODEL("AssociateExpression(" << (void*)&exprAst << ", " << (void*)&ir << ")\n");
        sym.switchLeft();
    }
    _symTab.pop(ast);
    SymbolTableEntry& idSym = astSymbol(AST::opInvokeOutput::id(ast));
    string name = IRGeneratorHelper::actualStreamName(idSym);
    // Now add the expressions;
    for (int i = 0, n = AST::opInvokeOutput::exprCount(ast); i < n; i++) {
        const Expression& e = astExpression(AST::opInvokeOutput::expr(ast, i));
        assert(e.getKind() == Expression::Binary);
        const BinaryExpression& be = e.as<BinaryExpression>();
        _state._outputs.push_back(make_pair(name, &be));
    }
}

// ----- Functions are defined at the top-level in a namespace.
void IRGenerator::visitFunctionDef(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    _symTab.pushOld(ast);
    visit(AST::functionDef::body(ast));
    _symTab.pop(ast);
    FunctionHeadSym& head = astSymbol(ast).as<FunctionHeadSym>();
    if (Utility::reservedName(head.name())) {
        SysOut::errorln(SPL_FRONT_END_FUNCTION_CPP_NAME_CLASH(head.name()),
                        astSymbol(ast).sourceLocation());
    }
    Statement* s = getAstStatement(AST::functionDef::body(ast));
    assert(s && s->is<BlockStatement>());
    ExpressionEvaluator ee(CompilerConfiguration::instance());
    s->simplifyExpression(ee);
    head.setFunctionBodyIR(dynamic_cast<BlockStatement*>(s));
}

// ----- Imperative statements can appear in function bodies or the logic clause of operator
// invocations.
void IRGenerator::visitLocalDecl(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    bool isMutable = 0 < AST::modifiers::modifierCount(AST::localDecl::modifiers(ast));
    RootTyp const& type = astSymbol(AST::localDecl::type(ast)).type();
    SourceLocation const& loc = SourceLocationFactory::instance().create(_pCtx, ast);
    BlockStatement* block = new BlockStatement(loc, NULL);
    for (int i = 0, n = AST::localDecl::localDeclItemCount(ast); i < n; i++) {
        visit(AST::localDeclItem::expr_q(AST::localDecl::localDeclItem(ast, i)));
        AstNode& astI = AST::localDecl::localDeclItem(ast, i);
        SymbolTableEntry& entry = astSymbol(AST::localDeclItem::id(astI));
        VariableSym& var = entry.as<VariableSym>();
        Expression* expr = NULL;
        if (!AST::noneT::is(AST::localDeclItem::expr_q(astI))) {
            expr = &astExpression(AST::localDeclItem::expr_q(astI));
            if (expr->getType() != type) {
                cerr << "Mismatch in expression types at " << loc.toString() << ": "
                     << expr->getType().getName() << " and " << type.getName() << endl;
                assert(false);
                expr = new CastExpression(type, expr->getLocation(), *expr);
            }
        }
        block->append(*new DeclStatement(var.sourceLocation(), isMutable, type, var.name(), expr,
                                         var.parentScope()->holder()->is<OpInvokeSym>()));
    }
    block->setUnnest();
    setAstStatement(_pool, ast, *block);
}

void IRGenerator::visitBlockStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitBlockStmt(ast);
    IRGeneratorHelper::blockStmt(_pool, _pCtx, ast);
}

void IRGenerator::visitBlockStmtNoScope(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitBlockStmtNoScope(ast);
    IRGeneratorHelper::blockStmt(_pool, _pCtx, ast);
}

void IRGenerator::visitExprStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitExprStmt(ast);
    Expression& e = astExpression(AST::exprStmt::expr(ast));
    SourceLocation const& loc = e.getLocation();
    IRGeneratorHelper::setStatementIR(_pool, loc, ast, new ExpressionStatement(loc, e));
}

void IRGenerator::visitIfStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitIfStmt(ast);
    SourceLocation const& loc = SourceLocationFactory::instance().create(_pCtx, ast);
    Expression& cond = astExpression(AST::ifStmt::expr(ast));
    Statement* lhs = getAstStatement(AST::ifStmt::stmt(ast));
    Statement* rhs = NULL;
    if (AST::ifStmt::has_elseClause(ast)) {
        rhs = getAstStatement(AST::elseClause::stmt(AST::ifStmt::elseClause(ast)));
    }
    IRGeneratorHelper::setStatementIR(_pool, loc, ast, new IfStatement(loc, cond, *lhs, rhs));
}

void IRGenerator::visitForStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitForStmt(ast);
    SourceLocation const& loc = astSymbol(ast).sourceLocation();
    AstNode* init = &AST::forStmt::forInit(ast);
    RootTyp const& type = astSymbol(AST::forInit::type(*init)).type();
    string id = astText(AST::forInit::id(*init));
    Expression& expr = astExpression(AST::forInit::expr(*init));
    Statement* body = getAstStatement(AST::forStmt::stmt(ast));
    IRGeneratorHelper::setStatementIR(_pool, loc, ast,
                                      new ForStatement(loc, type, id, expr, *body));
}

void IRGenerator::visitWhileStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitWhileStmt(ast);
    SourceLocation const& loc = SourceLocationFactory::instance().create(_pCtx, ast);
    Expression& expr = astExpression(AST::whileStmt::expr(ast));
    Statement* stmt = getAstStatement(AST::whileStmt::stmt(ast));
    IRGeneratorHelper::setStatementIR(_pool, loc, ast, new WhileStatement(loc, expr, *stmt));
}

void IRGenerator::visitBreakStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitBreakStmt(ast);
    SourceLocation const& loc = SourceLocationFactory::instance().create(_pCtx, ast);
    BreakStatement* ir = new BreakStatement(loc);
    IRGeneratorHelper::setStatementIR(_pool, loc, ast, ir);
}

void IRGenerator::visitContinueStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitContinueStmt(ast);
    SourceLocation const& loc = SourceLocationFactory::instance().create(_pCtx, ast);
    ContinueStatement* ir = new ContinueStatement(loc);
    IRGeneratorHelper::setStatementIR(_pool, loc, ast, ir);
}

void IRGenerator::visitReturnStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitReturnStmt(ast);
    SourceLocation const& loc = SourceLocationFactory::instance().create(_pCtx, ast);
    Expression* expr = NULL;
    if (AST::returnStmt::has_expr(ast)) {
        expr = &astExpression(AST::returnStmt::expr(ast));
    }
    IRGeneratorHelper::setStatementIR(_pool, loc, ast, new ReturnStatement(loc, expr));
}

// ----- Expressions can appear in many places in the grammar.
void IRGenerator::visitAttributeExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    visit(AST::attributeExpr::base(ast));
    setAstStage(_pool, AST::attributeExpr::id(ast), astStage(ast));
    SourceLocation const& loc = SourceLocationFactory::instance().create(_pCtx, ast);
    AstNode* lhs = &AST::attributeExpr::base(ast);
    AstNode* rhs = &AST::attributeExpr::id(ast);
    SymbolTableEntry& rSym = astSymbol(*rhs);
    SourceLocation const& rLoc = SourceLocationFactory::instance().create(_pCtx, *rhs);
    RootTyp const& type = rSym.type();
    // Special case - left is operator
    if (astSymbol(*lhs).is<OperatorSym>() || astSymbol(*lhs).is<DefTypeSym>()) {
        if (rSym.is<DefTypeSym>() || rSym.is<EnumTypeSym>()) {
            // this is an opActualName of a type. No need to resolve here.
            return;
        } else {
            // This is composite.SYM.   Just take the RHS
            Expression* e = rSym.genExpression(loc);
            assert(e);
            setAstExpression(_pool, ast, *e);
            SPLDBG(__PRETTY_FUNCTION__ << ": AssociateExpression: ast: " << (void*)&ast
                                       << " expr: " << (void*)e,
                   Debug::ShowSymbols);
            MODEL("AssociateExpression(" << (void*)&ast << ", " << (void*)e << ")\n");
            return;
        }
    }
    // Normal attribute
    Expression& left = astExpression(*lhs);
    Expression& e = *new SymbolExpression(type, rLoc, astText(*rhs), Expression::Attr);
    if (NULL == astExpressionPtr(*rhs)) {
        setAstExpression(_pool, *rhs, e);
    } else { // already set for composite parameter of expression type "attribute"
        resetAstExpression(_pool, *rhs, e);
    }
    Expression& attrExpr = *new AttributeExpression(type, loc, left.clone(), e);
    setAstExpression(_pool, ast, attrExpr);
    SPLDBG(__PRETTY_FUNCTION__ << ": AssociateExpression: ast: " << (void*)&ast
                               << " expr: " << (void*)&attrExpr,
           Debug::ShowSymbols);
    MODEL("AssociateExpression(" << (void*)&ast << ", " << (void*)&attrExpr << ")\n");
}

void IRGenerator::visitCastExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitCastExpr(ast);
    const SymbolTableEntry& sym = astSymbol(ast);
    RootTyp const& type = sym.type();
    SourceLocation const& loc = sym.sourceLocation();
    // This might be a (CASTEXPR (PRIMITIVETYPE complex32)[1.0w 1.0w]
    // We have to convert this to a real complex literal
    switch (type.getMetaType()) {
        case MetaType::COMPLEX32:
        case MetaType::COMPLEX64: {
            const Literal* ll = NULL;
            const Literal* rl = NULL;
            const Expression& e = IRGeneratorHelper::getSimplifiedExpn(AST::castExpr::expr(ast));
            const Literal* l = e.getLiteral();
            if (l && l->getKind() == Literal::List && l->numElements() == 2) {
                ll = &l->listSetElement(0);
                if (ll->getKind() != Literal::Primitive) {
                    ll = NULL;
                }
                rl = &l->listSetElement(1);
                if (rl->getKind() != Literal::Primitive) {
                    rl = NULL;
                }
            }
            if (ll && rl) {
                // This is a real constant to be converted
                switch (type.getMetaType()) {
                    case MetaType::COMPLEX32: {
                        float l2, r;
                        ll->primitive().getFloatValue(l2);
                        rl->primitive().getFloatValue(r);
                        complex32 v(l2, r);
                        IRGeneratorHelper::setLiteralIR(_pool, ast, type, loc,
                                                        new ExpressionValue(v));
                        return;
                    }
                    case MetaType::COMPLEX64: {
                        double l2, r;
                        ll->primitive().getFloatValue(l2);
                        rl->primitive().getFloatValue(r);
                        complex64 v(l2, r);
                        IRGeneratorHelper::setLiteralIR(_pool, ast, type, loc,
                                                        new ExpressionValue(v));
                        return;
                    }
                    default:
                        break;
                }
            }
            break;
        }
        default:
            break;
    }
    Expression& child = astExpression(AST::castExpr::expr(ast));
    setAstExpression(_pool, ast, *new CastExpression(type, loc, child));
}

void IRGenerator::visitCallExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    for (int i = 0, n = AST::callExpr::actualCount(ast); i < n; i++) {
        visit(AST::callExpr::actual(ast, i));
    }
    CallExpressionSym& callSym = astSymbol(ast).as<CallExpressionSym>();
    SourceLocation const& loc = callSym.sourceLocation();
    SymbolTableEntry* sent = callSym.resolvedTgt();
    if (indebug(Debug::IRGen) && sent) {
        cerr << "sent = " << *sent << endl;
    }
    FunctionHeadSym* functionHead;
    RootTyp const* type;
    string compositeName;
    if (NULL == sent) {
        assert(isInSubscriptionOrFilterExpression(SymbolTable::instance().currentScope()));
        functionHead = NULL;
        type = &TypeFactory::instance().unknownT();
    } else {
        functionHead = &sent->as<FunctionHeadSym>();
        type = &callSym.type();
        if (functionHead->isIntrinsic()) {
            IRGeneratorHelper::errorCheckIntrinsics(ast, *functionHead, _state);
            const string& fName = functionHead->name();
            // Check for getThisCompositeInstanceName() call
            if (fName == "getThisCompositeInstanceName") {
                assert(0 == AST::callExpr::actualCount(ast));
                // If we are here while visiting a composite def, we need to ignore it
                if (_state._visitingCompositeDef) {
                    // We want to re-IRGen this expression when we get to it
                    setAstStage(_pool, ast, FrontEndStage::OUTPUT_SIDE_EFFECTS_CHECKER);
                    return;
                }

                // We need to replace this with the string name of the current composite operator
                // instance
                rstring iName = SymbolTable::instance()
                                  .currentCompositeInstance()
                                  .compositeInstanceIR()
                                  .getInstanceName();
                IRGeneratorHelper::setLiteralIR(_pool, ast, *type, loc, new ExpressionValue(iName));
                return;
            } else if (fName == "getSubmissionTimeValue" || fName == "getSubmissionTimeListValue") {
                compositeName = SymbolTable::instance().currentCompositeDef().fullName();
            }
        } else if (functionHead->builtinNumber() >= 0) {
            IRGeneratorHelper::errorCheckBuiltins(ast, *functionHead, _state);
            Expression* ret = IRGeneratorHelper::simplifyBuiltin(_pool, ast, functionHead->name());
            if (ret) {
                return;
            }
        }
    }
    vector<Expression*>* elist = new vector<Expression*>;
    for (int i = 0, n = AST::callExpr::actualCount(ast); i < n; i++) {
        elist->push_back(&astExpression(AST::callExpr::actual(ast, i)));
    }
    CallExpression* ir;
    if (NULL == functionHead) {
        ir = new CallExpression(*type, loc, astText(AST::callExpr::target(ast)), *elist,
                                compositeName);
    } else {
        ir = new CallExpression(*type, loc, *functionHead, *elist, compositeName);
    }
    if (callSym.getSubmitNeedingCopy()) {
        ir->setSubmitNeedingCopy();
    }
    setAstExpression(_pool, ast, *ir);
    SPLDBG(__PRETTY_FUNCTION__ << ": AssociateExpression: ast: " << (void*)&ast
                               << " expr: " << (void*)ir,
           Debug::ShowSymbols);
    MODEL("AssociateExpression(" << (void*)&ast << ", " << (void*)ir << ")\n");
}

void IRGenerator::visitConditionalExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitConditionalExpr(ast);
    Expression& cond = astExpression(AST::conditionalExpr::condExpr(ast));
    Expression& lhs = astExpression(AST::conditionalExpr::thenExpr(ast));
    Expression& rhs = astExpression(AST::conditionalExpr::elseExpr(ast));
    ConditionalExpression* ir =
      new ConditionalExpression(lhs.getType(), cond.getLocation(), cond, lhs, rhs);
    setAstExpression(_pool, ast, *ir);
    SPLDBG(__PRETTY_FUNCTION__ << ": AssociateExpression: ast: " << (void*)&ast
                               << " expr: " << (void*)ir,
           Debug::ShowSymbols);
    MODEL("AssociateExpression(" << (void*)&ast << ", " << (void*)ir << ")\n");
}

void IRGenerator::visitInfixExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitInfixExpr(ast);
    SymbolTableEntry& sym = astSymbol(ast);
    Expression::Op op = astNodeOp(ast);
    Expression& left = astExpression(AST::infixExpr::lhs(ast));
    Expression& right = astExpression(AST::infixExpr::rhs(ast));
    BinaryExpression* ir = new BinaryExpression(sym.type(), sym.sourceLocation(), op, left, right);
    setAstExpression(_pool, ast, *ir);
    SPLDBG(__PRETTY_FUNCTION__ << ": AssociateExpression: ast: " << (void*)&ast
                               << " expr: " << (void*)ir,
           Debug::ShowSymbols);
    MODEL("AssociateExpression(" << (void*)&ast << ", " << (void*)ir << ")\n");
}

void IRGenerator::visitParenthesizedExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitParenthesizedExpr(ast);
    setAstExpression(_pool, ast, astExpression(AST::parenthesizedExpr::expr(ast)));
}

void IRGenerator::visitPostfixExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitPostfixExpr(ast);
    Expression::Op op = astNodeOp(AST::postfixExpr::postfixOp(ast));
    Expression& child = astExpression(AST::postfixExpr::expr(ast));
    PostfixExpression* ir =
      new PostfixExpression(child.getType(), astSymbol(ast).sourceLocation(), op, child);
    setAstExpression(_pool, ast, *ir);
    SPLDBG(__PRETTY_FUNCTION__ << ": AssociateExpression: ast: " << (void*)&ast
                               << " expr: " << (void*)ir,
           Debug::ShowSymbols);
    MODEL("AssociateExpression(" << (void*)&ast << ", " << (void*)ir << ")\n");
}

void IRGenerator::visitPrefixExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitPrefixExpr(ast);
    Expression::Op op = astNodeOp(AST::prefixExpr::prefixOp(ast));
    Expression& child = astExpression(AST::prefixExpr::expr(ast));
    PrefixExpression* ir =
      new PrefixExpression(child.getType(), astSymbol(ast).sourceLocation(), op, child);
    setAstExpression(_pool, ast, *ir);
    SPLDBG(__PRETTY_FUNCTION__ << ": AssociateExpression: ast: " << (void*)&ast
                               << " expr: " << (void*)ir,
           Debug::ShowSymbols);
    MODEL("AssociateExpression(" << (void*)&ast << ", " << (void*)ir << ")\n");
}

void IRGenerator::visitQualifierExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting (" << (void*)&ast << ") " << astText(ast),
           Debug::TraceIRGeneration);
    AstVisitorPushPop::visitQualifierExpr(ast);
    SymbolTableEntry& sym = astSymbol(ast);
    if (sym.is<CompositeDefSym>() || sym.is<PrimitiveOperatorSym>() || sym.is<DefTypeSym>()) {
        // Already set
    } else if (sym.is<CompositeFormalSym>()) {
        assert(AST::functionT::is(
          AST::expressionMode::name(AST::compositeFormal::expressionMode(sym.ast()))));
    } else if (sym.is<EnumValueSym>()) {
        Expression* expr = sym.genExpression(sym.sourceLocation());
        assert(NULL != expr);
        setAstExpression(_pool, ast, *expr);
        SPLDBG(__PRETTY_FUNCTION__ << ": AssociateExpression: ast: " << (void*)&ast
                                   << " expr: " << (void*)expr,
               Debug::ShowSymbols);
        MODEL("AssociateExpression(" << (void*)&ast << ", " << (void*)expr << ")\n");
    } else {
        cerr << "IRGenerator::qualifierExpr(" << sym << ")" << endl;
        assert(!"Unexpected qualifier expression");
    }
}

void IRGenerator::visitSubscriptExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitSubscriptExpr(ast);
    Expression& base = astExpression(AST::subscriptExpr::expr(ast));
    // Is this a stream history subscript?
    RootTyp const* type = &base.getType();
    AstNode& subscript = AST::subscriptExpr::subscript(ast);
    if (base.getKind() == Expression::StreamSymbol) {
        // This will become a StreamHistorySymbolExpression
        assert(!AST::subscriptSlice::is(subscript));
        const Expression& v = IRGeneratorHelper::getSimplifiedExpn(subscript);
        const Literal* lit = v.getLiteral();
        int32_t depth;
        if (lit && IRGeneratorHelper::getIntValue(*lit, depth)) {
            const StreamSymbolExpression& str = base.as<StreamSymbolExpression>();
            StreamHistorySymbolExpression* s = new StreamHistorySymbolExpression(
              *type, str.getStartLocation(), str.getId(), str.getPort(), depth);
            setAstExpression(_pool, ast, *s);
            SPLDBG(__PRETTY_FUNCTION__ << ": AssociateExpression: ast: " << (void*)&ast
                                       << " expr: " << (void*)s,
                   Debug::ShowSymbols);
            MODEL("AssociateExpression(" << (void*)&ast << ", " << (void*)s << ")\n");
            return;
        } else {
            Expression& e = astExpression(subscript);
            SysOut::errorln(SPL_FRONT_END_SUBSCRIPT_HISTORY_VALUE(e.toString()),
                            e.getStartLocation());
        }
    }
    SubscriptExpression* s;
    if (AST::subscriptSlice::is(subscript)) {
        AstNode* lhs = &AST::subscriptSlice::lower(subscript);
        AstNode* rhs = &AST::subscriptSlice::upper(subscript);
        Expression* l = AST::noneT::is(*lhs) ? NULL : &astExpression(*lhs);
        Expression* r = AST::noneT::is(*rhs) ? NULL : &astExpression(*rhs);
        s = new SubscriptExpression(*type, astSymbol(ast).sourceLocation(), base, true, l, r);
    } else {
        switch (type->getMetaType()) {
            case MetaType::LIST: {
                ListTyp const* lt = &type->as<ListTyp>();
                type = &lt->getElementType();
                break;
            }
            case MetaType::BLIST: {
                BListTyp const* lt = &type->as<BListTyp>();
                type = &lt->getElementType();
                break;
            }
            case MetaType::MAP: {
                MapTyp const* lt = &type->as<MapTyp>();
                type = &lt->getValueType();
                break;
            }
            case MetaType::BMAP: {
                BMapTyp const* lt = &type->as<BMapTyp>();
                type = &lt->getValueType();
                break;
            }
            case MetaType::BLOB: {
                // blob type needs to be changed to uint8
                type = &TypeFactory::instance().primitiveT(MetaType::UINT8);
                break;
            }
            default:
                break;
        }
        Expression& sub = astExpression(AST::subscriptExpr::subscript(ast));
        s =
          new SubscriptExpression(*type, astSymbol(ast).sourceLocation(), base, false, &sub, NULL);
    }
    setAstExpression(_pool, ast, *s);
    SPLDBG(__PRETTY_FUNCTION__ << ": AssociateExpression: ast: " << (void*)&ast
                               << " expr: " << (void*)s,
           Debug::ShowSymbols);
    MODEL("AssociateExpression(" << (void*)&ast << ", " << (void*)s << ")\n");
}

static inline bool alreadyKnownId(const SymbolTableEntry* e)
{
    return e->is<ActualConfigSym>() || e->is<CompilationUnitSym>() || e->is<FormalConfigSym>() ||
           e->is<FunctionHeadSym>() || e->is<NamespaceSym>() || e->is<OnPunctLogicSym>() ||
#if 0
               // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
               e->is<OnWatermarkLogicSym>() ||
#endif
           e->is<OnTupleLogicSym>() || e->is<OpInvokeActualSym>() || e->is<OpInvokeOutputSym>() ||
           e->is<OpInvokeWindowSym>();
}

void IRGenerator::visitId(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitId(ast);
    SymbolTableEntry* e = astSymbolPtr(ast);

    // Is this in error, or something we don't have to process (see above)?
    if (NULL == e || alreadyKnownId(e)) {
        return;
    }

    SourceLocation const& loc = SourceLocationFactory::instance().create(_pCtx, ast);
    Expression* s = e->genExpression(loc);
    if (e->is<CompositeDefSym>()) {
        // This is a composite symbol.   Just set the expression and return;
        setAstExpression(_pool, ast, *s);
        SPLDBG(__PRETTY_FUNCTION__ << ": AssociateExpression: ast: " << (void*)&ast
                                   << " expr: " << (void*)s,
               Debug::ShowSymbols);
        MODEL("AssociateExpression(" << (void*)&ast << ", " << (void*)s << ")\n");
        return;
    }
    RootTyp const* type = &e->type();
    if (!type) {
        // config expns, etc.   i.e. config logLevel: trace;
        type = &TypeFactory::instance().primitiveT(MetaType::UINT32);
    }
    if (!s) {
        if (e->is<PortAliasSym>()) {
            s = e->as<PortAliasSym>().genExpression(loc);
        }
        if (!s) {
            s = new SymbolExpression(*type, loc, astText(ast), Expression::Other);
        }
    }
    setAstExpression(_pool, ast, *s);
    SPLDBG(__PRETTY_FUNCTION__ << ": AssociateExpression: ast: " << (void*)&ast
                               << " expr: " << (void*)s,
           Debug::ShowSymbols);
    MODEL("AssociateExpression(" << (void*)&ast << ", " << (void*)s << ")\n");
}

void IRGenerator::visitUnwrapExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitUnwrapExpr(ast);
    Expression& lhs = astExpression(AST::unwrapExpr::lhs(ast));
    RootTyp const& resultType = lhs.getType().isOptional()
                                  ? lhs.getType().as<OptionalTyp>().getUnderlyingType()
                                  : lhs.getType();
    UnwrapExpression* ir = new UnwrapExpression(resultType, lhs.getLocation(), lhs);
    setAstExpression(_pool, ast, *ir);
    SPLDBG(__PRETTY_FUNCTION__ << ": AssociateExpression: ast: " << (void*)&ast
                               << " expr: " << (void*)ir,
           Debug::ShowSymbols);
    MODEL("AssociateExpression(" << (void*)&ast << ", " << (void*)ir << ")\n");
}

void IRGenerator::visitUnwrapOrElseExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitUnwrapOrElseExpr(ast);
    Expression& lhs = astExpression(AST::unwrapOrElseExpr::lhs(ast));
    Expression& rhs = astExpression(AST::unwrapOrElseExpr::rhs(ast));
    RootTyp const& resultType = lhs.getType().isOptional()
                                  ? lhs.getType().as<OptionalTyp>().getUnderlyingType()
                                  : lhs.getType();
    UnwrapOrElseExpression* ir =
      new UnwrapOrElseExpression(resultType, lhs.getLocation(), lhs, rhs);
    setAstExpression(_pool, ast, *ir);
    SPLDBG(__PRETTY_FUNCTION__ << ": AssociateExpression: ast: " << (void*)&ast
                               << " expr: " << (void*)ir,
           Debug::ShowSymbols);
    MODEL("AssociateExpression(" << (void*)&ast << ", " << (void*)ir << ")\n");
}

void IRGenerator::visitIsPresentExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitUnwrapExpr(ast);
    Expression& lhs = astExpression(AST::isPresentExpr::lhs(ast));
    RootTyp const& resultType = TypeFactory::instance().primitiveT(MetaType::BOOLEAN);
    IsPresentExpression* ir = new IsPresentExpression(resultType, lhs.getLocation(), lhs);
    setAstExpression(_pool, ast, *ir);
    SPLDBG(__PRETTY_FUNCTION__ << ": AssociateExpression: ast: " << (void*)&ast
                               << " expr: " << (void*)ir,
           Debug::ShowSymbols);
    MODEL("AssociateExpression(" << (void*)&ast << ", " << (void*)ir << ")\n");
}

// ----- Literals are the highest-precedence expressions denoting values.
void IRGenerator::visitListLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitListLiteral(ast);
    SymbolTableEntry& sym = astSymbol(ast);
    Literal* lit = new Literal(sym.type(), NULL);
    for (int i = 0, n = AST::listLiteral::exprCount(ast); i < n; i++) {
        Expression& e = IRGeneratorHelper::getSimplifiedExpn(AST::listLiteral::expr(ast, i));
        Literal* l = e.getLiteral();
        if (!l) {
            l = new Literal(e);
        }
        lit->addListElement(*l);
    }
    LiteralExpression* ir = new LiteralExpression(sym.type(), sym.sourceLocation(), *lit);
    setAstExpression(_pool, ast, *ir);
}

void IRGenerator::visitCurlyLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitCurlyLiteral(ast);
    cerr << "IRGenerator::emptyCurlyLiteral(" << ast << ")" << endl;
    assert(!"should have been erased by EmptyCurlyTypeFinder");
}

void IRGenerator::visitTupleLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitTupleLiteral(ast);
    TupleLiteralSym& sym = astSymbol(ast).as<TupleLiteralSym>();
    // Create the literal for the whole tuple
    Literal* lit = new Literal(sym.type(), NULL);
    for (int i = 0, n = AST::tupleLiteral::attributeAssignCount(ast); i < n; i++) {
        // (= field value)
        AstNode& fieldAst = AST::tupleLiteral::attributeAssign(ast, i);
        string fieldName = astText(AST::attributeAssign::id(fieldAst));
        Expression& fieldValue =
          IRGeneratorHelper::getSimplifiedExpn(AST::attributeAssign::expr(fieldAst));
        Literal* l = fieldValue.getLiteral();
        if (!l) {
            l = new Literal(fieldValue);
        }
        lit->addTupleElement(fieldName, *l);
    }
    LiteralExpression* ir = new LiteralExpression(sym.type(), sym.sourceLocation(), *lit);
    setAstExpression(_pool, ast, *ir);
}

void IRGenerator::visitMapLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitMapLiteral(ast);
    SymbolTableEntry& sym = astSymbol(ast);
    Literal* lit = new Literal(sym.type(), NULL);
    for (int i = 0, n = AST::mapLiteral::mappingCount(ast); i < n; i++) {
        AstNode& mapping = AST::mapLiteral::mapping(ast, i);
        Expression& k = IRGeneratorHelper::getSimplifiedExpn(AST::mapping::fromExpr(mapping));
        Expression& v = IRGeneratorHelper::getSimplifiedExpn(AST::mapping::toExpr(mapping));
        Literal* kl = k.getLiteral();
        if (!kl) {
            kl = new Literal(k);
        }
        Literal* vl = v.getLiteral();
        if (!vl) {
            vl = new Literal(v);
        }
        lit->addMapElement(*kl, *vl);
    }
    LiteralExpression* ir = new LiteralExpression(sym.type(), sym.sourceLocation(), *lit);
    setAstExpression(_pool, ast, *ir);
}

void IRGenerator::visitSetLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitSetLiteral(ast);
    SymbolTableEntry& sym = astSymbol(ast);
    Literal* lit = new Literal(sym.type(), NULL);
    for (int i = 0, n = AST::setLiteral::exprCount(ast); i < n; i++) {
        Expression& e = IRGeneratorHelper::getSimplifiedExpn(AST::setLiteral::expr(ast, i));
        Literal* l = e.getLiteral();
        if (!l) {
            l = new Literal(e);
        }
        lit->addSetElement(*l);
    }
    LiteralExpression* ir = new LiteralExpression(sym.type(), sym.sourceLocation(), *lit);
    setAstExpression(_pool, ast, *ir);
}

void IRGenerator::visitBooleanLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitBooleanLiteral(ast);
    SymbolTableEntry& sym = astSymbol(ast);
    const RootTyp& symType = sym.type();
    string token = astText(ast);
    const RootTyp& type = TypeFactory::instance().primitiveT(MetaType::BOOLEAN);
    boolean v = token == "true";
    IRGeneratorHelper::setLiteralIR(_pool, ast, type, symType, astSymbol(ast).sourceLocation(),
                                    new ExpressionValue(v));
}

void IRGenerator::visitXMLLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitXMLLiteral(ast);
    string token = astText(ast);
    token = token.substr(0, token.length() - 1);
    SymbolTableEntry& sym = astSymbol(ast);
    rstring v;
    try { // continue with empty string on error
        v = interpretRStringLiteral(token);
    } catch (Distillery::DistilleryException const& e) {
        SysOut::errorln(SPL_FRONT_END_INVALID_STRING_LITERAL(e.getExplanation()),
                        sym.sourceLocation());
    }
    IRGeneratorHelper::setLiteralIR(_pool, ast, sym.type(), sym.sourceLocation(),
                                    new ExpressionValue(v));
}

void IRGenerator::visitStringLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitStringLiteral(ast);
    string token = astText(ast);
    {
        char lastChar = token.at(token.length() - 1);
        if ('r' == lastChar || 'u' == lastChar) {
            token = token.substr(0, token.length() - 1);
        }
    }
    SymbolTableEntry& sym = astSymbol(ast);
    switch (sym.type().getMetaType()) {
        case MetaType::RSTRING: {
            rstring v;
            try { // continue with empty string on error
                v = interpretRStringLiteral(token);
            } catch (Distillery::DistilleryException const& e) {
                SysOut::errorln(SPL_FRONT_END_INVALID_STRING_LITERAL(e.getExplanation()),
                                sym.sourceLocation());
            }
            IRGeneratorHelper::setLiteralIR(_pool, ast, sym.type(), sym.sourceLocation(),
                                            new ExpressionValue(v));
            break;
        }
        case MetaType::USTRING: {
            ustring v;
            try { // continue with empty string on error
                v = interpretUStringLiteral(token);
            } catch (Distillery::DistilleryException const& e) {
                SysOut::errorln(SPL_FRONT_END_INVALID_STRING_LITERAL(e.getExplanation()),
                                sym.sourceLocation());
            }
            IRGeneratorHelper::setLiteralIR(_pool, ast, sym.type(), sym.sourceLocation(),
                                            new ExpressionValue(v));
            break;
        }
        default:;
    }
}

template<class T>
LiteralExpression* readSignedHex(istringstream& istr,
                                 const SymbolTableEntry& sym,
                                 AstNode& ast,
                                 AstPool& pool)
{
    T v = 0;
    uint64_t value;
    if (istr.peek() == '-') {
        // we can't read a negative value into an unsigned variable
        (void)istr.get(); // toss it
        istr >> hex >> value;
        value = -value;
    } else {
        istr >> hex >> value;
    }

    // Check to see if we overflowed
    int64_t iValue = value;
    const uint32_t shiftSize = ((sizeof(int64_t) - sizeof(T)) * 8);
    v = iValue;
    iValue = (iValue << shiftSize) >> shiftSize; // ensure sign bit extended
    if (v != iValue) {
        istr.setstate(ios::failbit);
    }
    return &IRGeneratorHelper::setLiteralIR(pool, ast, sym.type(), sym.sourceLocation(),
                                            new ExpressionValue(v));
}

template<class T>
LiteralExpression* readUnsignedHex(istringstream& istr,
                                   const SymbolTableEntry& sym,
                                   AstNode& ast,
                                   AstPool& pool)
{
    T v = 0;
    istr >> hex >> v;
    return &IRGeneratorHelper::setLiteralIR(pool, ast, sym.type(), sym.sourceLocation(),
                                            new ExpressionValue(v));
}

void IRGenerator::visitNullLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    SymbolTableEntry& sym = astSymbol(ast);
    IRGeneratorHelper::setLiteralIR(_pool, ast, TypeFactory::instance().nullT(), sym.type(),
                                    sym.sourceLocation(), NULL);
}

void IRGenerator::visitNumericLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    AstVisitorPushPop::visitNumericLiteral(ast);
    Expression* e = astExpressionPtr(ast);
    if (e) {
        assert(e->is<LiteralExpression>());
        return;
    }
    SymbolTableEntry& sym = astSymbol(ast);
    const RootTyp& type = sym.type();
    MetaType mt = (type.isOptional()) ? type.as<OptionalTyp>().getUnderlyingType().getMetaType()
                                      : type.getMetaType();

    string token = astText(ast);
    LiteralExpression* result = NULL;
    if (AST::hexLiteralT::is(ast)) {
        token = token.substr(0, 1 + token.find_last_not_of("su"));
        istringstream istr(token);
        switch (mt) {
            case MetaType::INT8: {
                result = readSignedHex<int8>(istr, sym, ast, _pool);
                break;
            }
            case MetaType::INT16: {
                result = readSignedHex<int16>(istr, sym, ast, _pool);
                break;
            }
            case MetaType::INT32: {
                result = readSignedHex<int32>(istr, sym, ast, _pool);
                break;
            }
            case MetaType::INT64: {
                result = readSignedHex<int64>(istr, sym, ast, _pool);
                break;
            }
            case MetaType::UINT8: {
                result = readUnsignedHex<uint8>(istr, sym, ast, _pool);
                break;
            }
            case MetaType::UINT16: {
                result = readUnsignedHex<uint16>(istr, sym, ast, _pool);
                break;
            }
            case MetaType::UINT32: {
                result = readUnsignedHex<uint32>(istr, sym, ast, _pool);
                break;
            }
            case MetaType::UINT64: {
                result = readUnsignedHex<uint64>(istr, sym, ast, _pool);
                break;
            }
            default: {
                THROW(SPLCompilerAssertionFailed, "Unknown set literal " << ast);
                break;
            }
        }

        if (!result) {
            THROW(SPLCompilerAssertionFailed, "read literal failed " << ast);
        }
        if (!istr) {
            ostringstream os;
            os << sym.type();
            SysOut::errorln(SPL_FRONT_END_LITERAL_OUT_OF_BOUNDS(token, os.str()),
                            sym.sourceLocation());
        }
    } else {
        token = token.substr(0, 1 + token.find_last_not_of("sufdbhwlq"));
        istringstream istr(token);
#define ReadLit(T)                                                                                 \
    T v = 0;                                                                                       \
    istr >> v;                                                                                     \
    result = &IRGeneratorHelper::setLiteralIR(_pool, ast, sym.type(), sym.sourceLocation(),        \
                                              new ExpressionValue(v))

        switch (mt) {
            case MetaType::INT8: {
                ReadLit(int8);
                break;
            }
            case MetaType::INT16: {
                ReadLit(int16);
                break;
            }
            case MetaType::INT32: {
                ReadLit(int32);
                break;
            }
            case MetaType::INT64: {
                ReadLit(int64);
                break;
            }
            case MetaType::UINT8: {
                ReadLit(uint8);
                break;
            }
            case MetaType::UINT16: {
                ReadLit(uint16);
                break;
            }
            case MetaType::UINT32: {
                ReadLit(uint32);
                break;
            }
            case MetaType::UINT64: {
                ReadLit(uint64);
                break;
            }
            case MetaType::FLOAT32: {
                ReadLit(float32);
                break;
            }
            case MetaType::FLOAT64: {
                ReadLit(float64);
                break;
            }
            case MetaType::DECIMAL32: {
                ReadLit(decimal32);
                break;
            }
            case MetaType::DECIMAL64: {
                ReadLit(decimal64);
                break;
            }
            case MetaType::DECIMAL128: {
                ReadLit(decimal128);
                break;
            }
#undef ReadLit
            default: {
                THROW(SPLCompilerAssertionFailed, "Unknown set literal " << ast);
                break;
            }
        }
        assert(NULL != result);
        if (!istr) {
            ostringstream os;
            os << sym.type();
            SysOut::errorln(SPL_FRONT_END_LITERAL_OUT_OF_BOUNDS(token, os.str()),
                            sym.sourceLocation());
        }
    }
}

// ----- Types are defined in a composite operator's type clause, or in a block, or at the
// top-level.
void IRGenerator::visitTypeName(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TraceIRGeneration);
    /* do nothing */
}
}
