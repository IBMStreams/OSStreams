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

#include <SPL/Core/IRValidator.h>

#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/Statement.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/CompilerMessage.h>
#include <SPL/Core/ExpressionEvaluator.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/Core/Visitor.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/FrontEnd/TypeFactory.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/SysOut.h>
#include <SPL/Utility/Utility.h>
#include <UTILS/HashMapHelpers.h>

#include <iostream>
#include <sstream>

using namespace SPL::Operator;

using namespace std;
using namespace std::tr1;
using namespace SPL;

class TupleMutatorVisitorContext
{
  public:
    enum State
    {
        Start,
        Storing,
        Loading,
        MutableArgument
    };
    enum ErrorContext
    {
        Logic,
        Parameter,
        Output
    };

    TupleMutatorVisitorContext(InputPorts const& inomPorts,
                               string const& inoperatorName,
                               ErrorContext inerrorContext,
                               const string& inparameterName)
      : omPorts_(inomPorts)
      , operatorName_(inoperatorName)
      , state_(Start)
      , errorContext_(inerrorContext)
      , parameterName_(inparameterName)
    {}

    TupleMutatorVisitorContext(TupleMutatorVisitorContext& previousContext, State instate)
      : omPorts_(previousContext.omPorts_)
      , operatorName_(previousContext.operatorName_)
      , state_(instate)
      , errorContext_(previousContext.errorContext_)
      , parameterName_(previousContext.parameterName_)
    {}

    InputPorts const& omPorts() const { return omPorts_; }
    string const& operatorName() const { return operatorName_; }
    State state() const { return state_; }
    ErrorContext errorContext() const { return errorContext_; }
    const string& parameterName() const { return parameterName_; }

    Debug::DebugFlag debugFlag() const { return Debug::TraceTupleMutationVisitor; }

  private:
    InputPorts const& omPorts_;
    string const& operatorName_;
    State state_;
    ErrorContext errorContext_;
    const string& parameterName_;
};

ostream& operator<<(ostream& s, TupleMutatorVisitorContext::State state)
{
    switch (state) {
        case TupleMutatorVisitorContext::Start:
            return s << "Start";
        case TupleMutatorVisitorContext::Storing:
            return s << "Storing";
        case TupleMutatorVisitorContext::Loading:
            return s << "Loading";
        case TupleMutatorVisitorContext::MutableArgument:
            return s << "MutableArgument";
        default:
            assert(!"Missing case");
    }
    return s << "unknown";
}

namespace SPL {
template<>
bool DepthFirstVisitor<TupleMutatorVisitorContext>::visitBinaryExpression(
  TupleMutatorVisitorContext& context,
  BinaryExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expr " << expr.getKind() << " with op '"
                               << expr.getOp() << "'",
           context.debugFlag());

    switch (expr.getOp()) {
        case Expression::eq:
        case Expression::starEq:
        case Expression::slashEq:
        case Expression::modEq:
        case Expression::plusEq:
        case Expression::minusEq:
        case Expression::ampEq:
        case Expression::hatEq:
        case Expression::barEq:
        case Expression::lShiftEq:
        case Expression::rShiftEq: {
            TupleMutatorVisitorContext lhsContext(context, TupleMutatorVisitorContext::Storing);
            DepthFirstVisitor<TupleMutatorVisitorContext> visitor;
            if (!done_) {
                done_ = visitor.visit(lhsContext, expr.getLhs());
            }
            TupleMutatorVisitorContext rhsContext(context, TupleMutatorVisitorContext::Loading);
            if (!done_) {
                done_ = visitor.visit(rhsContext, expr.getRhs());
            }
            break;
        }
        default:
            break;
    }
    return done_;
}

template<>
bool DepthFirstVisitor<TupleMutatorVisitorContext>::visit(TupleMutatorVisitorContext& context,
                                                          SubscriptExpression const& expr)
{
    DepthFirstVisitor<TupleMutatorVisitorContext> visitor;
    Expression const* lower = expr.lower();
    if (NULL != lower && !done_) {
        TupleMutatorVisitorContext lowerContext(context, TupleMutatorVisitorContext::Loading);
        done_ = visitor.visit(lowerContext, *lower);
    }
    Expression const* upper = expr.upper();
    if (NULL != upper && !done_) {
        TupleMutatorVisitorContext upperContext(context, TupleMutatorVisitorContext::Loading);
        done_ = visitor.visit(upperContext, *upper);
    }
    if (!done_) {
        Expression const& lhs = expr.getLhs();
        done_ = visitor.visit(context, lhs);
    }
    return done_;
}

template<>
bool DepthFirstVisitor<TupleMutatorVisitorContext>::visit(TupleMutatorVisitorContext& context,
                                                          PostfixExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expr " << expr.getKind() << " with state "
                               << context.state(),
           context.debugFlag());

    switch (expr.getOp()) {
        case Expression::plusPlus:
        case Expression::minusMinus: {
            DepthFirstVisitor<TupleMutatorVisitorContext> visitor;
            TupleMutatorVisitorContext localContext(context, TupleMutatorVisitorContext::Storing);
            if (!done_) {
                done_ = visitor.visit(localContext, expr.getExpn());
                break;
            }
        }

        default:
            assert(!"Unexpected operator type");
    }
    return done_;
}

template<>
bool DepthFirstVisitor<TupleMutatorVisitorContext>::visit(TupleMutatorVisitorContext& context,
                                                          PrefixExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expr " << expr.getKind() << " with state "
                               << context.state() << "and op '" << expr.getOp() << "'",
           context.debugFlag());

    switch (expr.getOp()) {
        case Expression::plusPlus:
        case Expression::minusMinus: {
            DepthFirstVisitor<TupleMutatorVisitorContext> visitor;
            TupleMutatorVisitorContext localContext(context, TupleMutatorVisitorContext::Storing);
            if (!done_) {
                done_ = visitor.visit(localContext, expr.getExpn());
                break;
            }
        }
        case Expression::bang:
        case Expression::tilde:
        case Expression::unaryMinus:
        case Expression::plus:
            break;
        default:
            assert(!"Unexpected operator type");
    }
    return done_;
}

template<>
bool DepthFirstVisitor<TupleMutatorVisitorContext>::visit(TupleMutatorVisitorContext& context,
                                                          StreamSymbolExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expr " << expr.getKind() << " for port "
                               << expr.getPort() << " with state " << context.state(),
           context.debugFlag());

    // We've found a stream symbol on the LHS of some mutating context.  See if the port allows mutation
    if (context.state() == TupleMutatorVisitorContext::Storing ||
        context.state() == TupleMutatorVisitorContext::MutableArgument) {
        InputPorts const& omPorts = context.omPorts();
        Operator::InputPort const& omPort = omPorts.getPortAt(expr.getPort());
        if (!omPort.isTupleMutationAllowed()) {
            switch (context.errorContext()) {
                case TupleMutatorVisitorContext::Logic:
                    SysOut::errorln(
                      SPL_CORE_INPUT_TUPLE_MUTATED_LOGIC(context.operatorName(), expr.getPort()),
                      expr.getLocation());
                    break;
                case TupleMutatorVisitorContext::Parameter:
                    SysOut::errorln(SPL_CORE_INPUT_TUPLE_MUTATED_PARAMETER(context.parameterName(),
                                                                           context.operatorName(),
                                                                           expr.getPort()),
                                    expr.getLocation());
                    break;
                case TupleMutatorVisitorContext::Output:
                    SysOut::errorln(
                      SPL_CORE_INPUT_TUPLE_MUTATED_OUTPUT(context.operatorName(), expr.getPort()),
                      expr.getLocation());
                    break;
                default:
                    assert(!"Missing case");
            }
        }
    }
    return false;
}

template<>
bool DepthFirstVisitor<TupleMutatorVisitorContext>::visit(TupleMutatorVisitorContext& context,
                                                          CallExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expr " << expr.getKind() << " with state "
                               << context.state(),
           context.debugFlag());

    // We need to know if there is an attribute expression that is an argument to a call expression who's parameter is mutable.
    // If it's not mutable we don't care.
    FunctionHeadSym const& head = expr.getFcn();
    FunctionHeadSym::Formals const& formals = head.formals();
    vector<Expression*> const& args = expr.getArgs();
    for (uint32_t i = 0; i < formals.size(); ++i) {
        FunctionFormalSym const* formal = formals[i];
        if (formal->isMutable()) {
            SPLDBG(__PRETTY_FUNCTION__ << ": Visiting argument " << i, context.debugFlag());

            TupleMutatorVisitorContext callContext(context,
                                                   TupleMutatorVisitorContext::MutableArgument);
            DepthFirstVisitor<TupleMutatorVisitorContext> visitor;
            if (!done_) {
                done_ = visitor.visit(callContext, *args[i]);
            }
        }
    }
    return done_;
}
};

void IRValidator::validate(PrimitiveOperatorInstance& ir, OperatorGraphNode& node)
{
    OperatorModel const* om = PathSearch::instance().findOperatorModel(ir.getKind(), NULL);
    assert(
      om); // The assumption here is that if there is a problem finding the operator model it would have been caught earlier during parsing

    if (indebug(Debug::ShowIR)) {
        ostringstream ss;
        ir.print(ss);
        SysOut::debug(ss.str());
    }

    // Validate that the IR is correct based on the model for the operator we are generating
    // Validate that if a logic clause was specified that it is allowed for this operator
    if (om->getContext().allowCustomLogic() == false) {
        const PrimitiveOperatorInstance::Logic& logic = ir.getLogic();
        if (logic.hasLogic()) {
            SysOut::errorln(SPL_CORE_CUSTOM_LOGIC_NOT_ALLOWED(ir.getNames()[0]), ir.getLocation());
        }
    }

    validateParameters(ir, *om);

    validateInputPorts(ir, *om);

    validateOutputPorts(ir, *om);

    if (indebug(Debug::ShowIRFinal)) {
        ostringstream ss;
        ir.print(ss);
        SysOut::debug(ss.str());
    }
}

void IRValidator::validateParameters(PrimitiveOperatorInstance const& ir,
                                     Operator::OperatorModel const& om)
{
    SPLDBG("Performing parameter validations on operator " << ir.getNames()[0],
           Debug::IRValidation);

    vector<Param*> const& irParameters = ir.getParams();
    Operator::Parameters const& omParameters = om.getParameters();
    Operator::Parameters::NamedParameters const& omNamedParameters =
      omParameters.getNamedParameters();

    // For each parameter in the model verify that those marked as not optional exist in the IR
    for (Operator::Parameters::NamedParameters::const_iterator it = omNamedParameters.begin();
         it != omNamedParameters.end(); ++it) {
        Parameter const& parameter = *it->second;
        string const& parameterName = parameter.getName();

        if (om.isJavaOperator() && Utility::isReservedJavaOperatorParam(parameterName))
            continue;

        if (!parameter.isOptional()) {
            SPLDBG("Ensuring parameter " << parameterName << " is used", Debug::IRValidation);

            bool found = false;
            for (vector<Param*>::const_iterator it2 = irParameters.begin();
                 it2 != irParameters.end(); ++it2) {
                if (parameterName == (*it2)->getName()) {
                    SPLDBG("Found parameter " << (*it2)->getName() << " in the IR",
                           Debug::IRValidation);
                    found = true;
                    break;
                }
            }
            if (!found) {
                SysOut::errorln(
                  SPL_CORE_REQUIRED_PARAMETER_NOT_FOUND(parameterName, ir.getNames()[0]),
                  ir.getLocation());
            }
        }
    }

    // For each parameter in the IR, ...
    for (vector<Param*>::const_iterator it = irParameters.begin(); it != irParameters.end(); ++it) {
        Param const& irParameter = **it;
        string const& parameterName = irParameter.getName();

        // We will add the special java primitive op parameters in OPIModelBuilder
        if (om.isJavaOperator() && Utility::isReservedJavaOperatorParam(parameterName))
            SysOut::errorln(SPL_CORE_INVALID_JAVA_OPI_PARAM_NAME(parameterName),
                            irParameter.getLocation());

        vector<Expression const*> const& irExpressionList = irParameter.getExpressionList();

        // Find its equivalent in the model
        Operator::Parameters::NamedParameters::const_iterator it2 =
          omNamedParameters.find(parameterName);
        if (it2 != omNamedParameters.end()) {
            // We found the parameter in the model
            Parameter const& omParameter = *(it2->second);
            SPLDBG("Validating parameter " << omParameter.getName(), Debug::IRValidation);

            // Cardinality and expression type were checked in the FE
            // Verify the type of each expression
            Operator::ExpressionMode const& opeMode = omParameter.getExpressionMode();

            // If the mode is Expression than anything is allowed unless type is specified, in which case each value must match the specified type
            for (vector<Expression const*>::const_iterator it3 = irExpressionList.begin();
                 it3 != irExpressionList.end(); ++it3) {
                Expression const& irExpr = **it3;
                SPLDBG("Validating the expression against the expression mode for parameter "
                         << parameterName,
                       Debug::IRValidation);
                if (opeMode == Operator::ExpressionMode::Expression) {
                    // Type checked in FE
                } else if (opeMode == Operator::ExpressionMode::Attribute) {
                    validateParameterExpressionIsAttribute(irExpr, omParameter, ir);
                } else if (opeMode == Operator::ExpressionMode::Constant) {
                    validateParameterExpressionIsConstant(irExpr, omParameter, ir);
                } else if (opeMode == Operator::ExpressionMode::CustomLiteral) {
                    // This is validated in the FE - but check that it is constant
                    validateParameterExpressionIsCustomLiteral(irExpr, omParameter, ir);
                } else if (opeMode == Operator::ExpressionMode::AttributeFree) {
                    validateParameterExpressionIsAttributeFree(irExpr, omParameter, ir);
                } else {
                    assert(!"We should not get here");
                }

                // Validate the port scope for this parameter if port scope has been specified
                validateParameterPortScope(irExpr, omParameter, ir);

                // Validate that none of the expressions in the parameter mutate a non-mutable input tuple
                InputPorts const& omInputPorts = om.getInputPorts();
                TupleMutatorVisitorContext context(omInputPorts, ir.getNames()[0],
                                                   TupleMutatorVisitorContext::Parameter,
                                                   parameterName);
                DepthFirstVisitor<TupleMutatorVisitorContext> visitor;
                visitor.visit(context, irExpr);
            }
        } else if (!omParameters.allowArbitraryParameters()) {
            // Not sure we can actually get here at this point
            // The FE may not allow us to get here.
            if (omNamedParameters.find(parameterName) == omNamedParameters.end()) {
                SysOut::errorln(SPL_CORE_PARAMETER_NOT_ALLOWED(parameterName, ir.getNames()[0]),
                                ir.getLocation());
            }
        } else {
            // We have a parameter that is not in the model and arbitrary parameters are allowed
            SPLDBG("Arbitrary parameter '" << parameterName << "' found in operator "
                                           << ir.getNames()[0]
                                           << ".  No validation being performed.",
                   Debug::IRValidation);

            // Validate that none of the expressions in the parameter mutate a non-mutable input tuple
            for (vector<Expression const*>::const_iterator it4 = irExpressionList.begin();
                 it4 != irExpressionList.end(); ++it4) {
                Expression const& irExpr = **it4;
                InputPorts const& omInputPorts = om.getInputPorts();
                TupleMutatorVisitorContext context(omInputPorts, ir.getNames()[0],
                                                   TupleMutatorVisitorContext::Parameter,
                                                   parameterName);
                DepthFirstVisitor<TupleMutatorVisitorContext> visitor;
                visitor.visit(context, irExpr);
            }
        }
    }
}

class PortScopeVisitorContext
{
  public:
    enum Context
    {
        Parameter,
        OutputAssignment
    };

    PortScopeVisitorContext(Context incontext,
                            unordered_set<unsigned> const& inportScopes,
                            string const& inparameterName,
                            Expression const& expr,
                            string const& inoperatorName)
      : context_(incontext)
      , portScopes_(inportScopes)
      , parameterName_(inparameterName)
      , expr_(expr)
      , operatorName_(inoperatorName)
    {}

    Context context() const { return context_; }

    unordered_set<unsigned> const& portScopes() const { return portScopes_; }
    string const& parameterName() const { return parameterName_; }
    string const& operatorName() const { return operatorName_; }
    Expression const& expression() const { return expr_; }

    Debug::DebugFlag debugFlag() const { return Debug::TracePortScopeVisitor; }

  private:
    Context context_;
    unordered_set<unsigned> const& portScopes_;
    string const& parameterName_;
    Expression const& expr_;
    string const& operatorName_;
};

namespace SPL {
template<>
bool DepthFirstVisitor<PortScopeVisitorContext>::visit(PortScopeVisitorContext& context,
                                                       StreamSymbolExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());

    int32_t port = expr.getPort();
    unordered_set<unsigned>::const_iterator it = context.portScopes().find(port);
    SPLDBG("Checking for port " << port << " in port scopes in expression " << expr << " and did"
                                << ((it == context.portScopes().end()) ? "n't" : "") << " find it",
           Debug::IRValidation);

    // If we don't find the port in the set of allowable ports for this parameter then complain
    if (it == context.portScopes().end()) {
        if (context.context() == PortScopeVisitorContext::Parameter) {
            SysOut::errorln(SPL_CORE_INVALID_PORT_ACCESS(context.expression().toString(),
                                                         context.parameterName(),
                                                         context.operatorName()),
                            context.expression().getLocation());
        } else {
            assert(context.context() == PortScopeVisitorContext::OutputAssignment);
            SysOut::errorln(SPL_CORE_INVALID_PORT_IN_ASSIGNMENT(context.expression().toString(),
                                                                context.operatorName()),
                            context.expression().getLocation());
        }
    }
    return done_;
}
}

void IRValidator::validateAssignmentPortScope(Expression const& irExpr,
                                              Operator::OutputPort const& omPort,
                                              uint32_t portNum,
                                              PrimitiveOperatorInstance const& ir)
{
    // Check that, if there is a port scope associated with this port, any attributes that appear in the expression are
    // from ports that appear in the port scope
    std::tr1::unordered_set<unsigned> const& portScopes = omPort.getOutputAssignmentPortScope();
    if (portScopes.size() == 0) {
        SPLDBG("Output assignment " << irExpr << " has no port scope specified",
               Debug::TracePortScopeVisitor);
        return;
    }

    // We have some port scope specified
    // Walk the expression and find any attribute expression who's port is NOT in the port set
    PortScopeVisitorContext context(PortScopeVisitorContext::OutputAssignment, portScopes, "",
                                    irExpr, ir.getNames()[0]);
    DepthFirstVisitor<PortScopeVisitorContext> visitor;
    visitor.visit(context, irExpr);
}

void IRValidator::validateParameterPortScope(Expression const& irExpr,
                                             Parameter const& omParameter,
                                             PrimitiveOperatorInstance const& ir)
{
    SPLDBG("Check port scope for parameter " << omParameter.getName(), Debug::IRValidation);

    unordered_set<unsigned> const& portScopes = omParameter.getPortScope();
    if (portScopes.size() == 0) {
        SPLDBG("Parameter " << omParameter.getName() << " has no port scope specified",
               Debug::TracePortScopeVisitor);
        return;
    }

    // We have some port scope specified
    // Walk the expression and find any attribute expression who's port is NOT in the port set
    PortScopeVisitorContext context(PortScopeVisitorContext::Parameter, portScopes,
                                    omParameter.getName(), irExpr, ir.getNames()[0]);
    DepthFirstVisitor<PortScopeVisitorContext> visitor;
    visitor.visit(context, irExpr);
}

void IRValidator::validateParameterExpressionIsConstant(Expression const& irExpr,
                                                        Parameter const& omParameter,
                                                        PrimitiveOperatorInstance const& ir)
{
    SPLDBG("Validating that parameter " << omParameter.getName() << " is a constant",
           Debug::IRValidation);

    // In this case each expression should evaluate to a constant expression
    ExpressionEvaluator ee(CompilerConfiguration::instance());
    auto_ptr<Expression> clone(&irExpr.clone());
    clone.reset(Expression::simplify(clone.release(), ee));
    if (clone->getKind() != Expression::Lit) {
        SysOut::errorln(SPL_CORE_PARAMETER_EXPRESSION_NOT_CONSTANT(
                          irExpr.toString(), omParameter.getName(), ir.getNames()[0]),
                        irExpr.getLocation());
    }
}

void IRValidator::validateParameterExpressionIsCustomLiteral(Expression const& irExpr,
                                                             Parameter const& omParameter,
                                                             PrimitiveOperatorInstance const& ir)
{
    SPLDBG("Validating that parameter " << omParameter.getName() << " is a custom literal",
           Debug::IRValidation);

    // In this case each expression should evaluate to a constant expression that is a custom literal
    ExpressionEvaluator ee(CompilerConfiguration::instance());
    auto_ptr<Expression> clone(&irExpr.clone());
    clone.reset(Expression::simplify(clone.release(), ee));
    Expression::Kind k = clone->getKind();
    if (!(k == Expression::CustomLiteral || k == Expression::Enum)) {
        SysOut::errorln(SPL_CORE_PARAMETER_EXPRESSION_NOT_CONSTANT_CUSTOM_LITERAL(
                          irExpr.toString(), omParameter.getName(), ir.getNames()[0]),
                        irExpr.getLocation());
    }
}

class AttributeVisitorContext
{
  public:
    enum ExpressionContext
    {
        Parameter,
        OutputAssignment
    };
    AttributeVisitorContext(ExpressionContext inexprContext, string const& inname)
      : exprContext_(inexprContext)
      , name_(inname)
    {}

    ExpressionContext exprContext() const { return exprContext_; }
    string const& name() const { return name_; }

    Debug::DebugFlag debugFlag() const { return Debug::TraceAttributeVisitor; }

  private:
    ExpressionContext exprContext_;
    string const& name_;
};

namespace SPL {
template<>
bool DepthFirstVisitor<AttributeVisitorContext>::visit(AttributeVisitorContext& context,
                                                       AttributeExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expr " << expr.getKind(), context.debugFlag());

    assert(expr.getRhs().getKind() == Expression::Symbol);
    SymbolExpression const& symExpr = expr.getRhs().as<SymbolExpression>();
    if (context.exprContext() == AttributeVisitorContext::Parameter) {
        SysOut::errorln(
          SPL_CORE_PARAMETER_EXPRESSION_NOT_ATTRIBUTE_FREE(context.name(), symExpr.getId()),
          expr.getLocation());
    } else {
        assert(context.exprContext() == AttributeVisitorContext::OutputAssignment);
        SysOut::errorln(SPL_CORE_OUTPUT_ASSIGNMENT_EXPRESSION_NOT_ATTRIBUTE_FREE(symExpr.getId()),
                        expr.getLocation());
    }
    return done_;
}
}

void IRValidator::validateParameterExpressionIsAttributeFree(Expression const& irExpr,
                                                             Parameter const& omParameter,
                                                             PrimitiveOperatorInstance const& ir)
{
    SPLDBG("Validating that the expression for parameter " << omParameter.getName()
                                                           << " is attribute free",
           Debug::IRValidation);

    // The expression for the attribute should not refer to any stream attributes
    AttributeVisitorContext context(AttributeVisitorContext::Parameter, omParameter.getName());
    DepthFirstVisitor<AttributeVisitorContext> visitor;
    visitor.visit(context, irExpr);
}

bool checkForStreamSymbol(Expression const& expr)
{
    if (expr.getKind() == Expression::StreamSymbol)
        return true;
    if (expr.getKind() == Expression::Attribute) {
        AttributeExpression const& attrExpr = expr.as<AttributeExpression>();
        return checkForStreamSymbol(attrExpr.getLhs());
    }
    return false;
}

void IRValidator::validateParameterExpressionIsAttribute(Expression const& irExpr,
                                                         Parameter const& omParameter,
                                                         PrimitiveOperatorInstance const& ir)
{
    SPLDBG("Validating that the expression for parameter " << omParameter.getName()
                                                           << " is an attribute",
           Debug::IRValidation);

    // In this case the expression should be an AttributeExpression with a StreamSymbolExpression on the lhs and a SymbolExpression on the rhs
    if (irExpr.getKind() == Expression::Attribute) {
        AttributeExpression const& attrExpr = irExpr.as<AttributeExpression>();
        if (attrExpr.getRhs().getKind() == Expression::Symbol) {
            if (checkForStreamSymbol(attrExpr.getLhs())) {
                return; // Seems to be the correct form
            }
        }
    }
    SysOut::errorln(SPL_CORE_PARAMETER_EXPRESSION_NOT_ATTRIBUTE(
                      irExpr.toString(), omParameter.getName(), ir.getNames()[0]),
                    irExpr.getLocation());
}

void IRValidator::validateInputPortWindow(InputPorts const& omPorts,
                                          uint32_t portNumber,
                                          PrimitiveOperatorInstance const& ir)
{
    ir.getInputPort(portNumber);
    Operator::InputPort const& omPort = omPorts.getPortAt(portNumber);

    // If the windowing mode is optional we have nothing to check.
    Operator::WindowingMode const& omWindowMode = omPort.getWindowingMode();
    if (omWindowMode == Operator::WindowingMode::OptionallyWindowed) {
        return;
    }

    PrimitiveOperatorInstance::Window const* window = ir.getWindow(portNumber);

    // otherwise...
    if (omWindowMode == Operator::WindowingMode::NonWindowed) {
        SPLDBG("validateInputPortWindow: Validating non-windowed port " << portNumber,
               Debug::IRValidation);

        // Ensure no window is defined for the given port
        if (window) { // Found a window...tisk, tisk
            SPLDBG("validateInputPortWindow: Window found for port " << portNumber,
                   Debug::IRValidation);
            SysOut::errorln(
              SPL_CORE_WINDOW_DEFINED_FOR_NONWINDOWED_PORT(portNumber, ir.getNames()[0]),
              ir.getLocation());
        }
    } else {
        assert(omWindowMode == Operator::WindowingMode::Windowed); // catch if another mode is added
        SPLDBG("validateInputPortWindow: Validating windowed port " << portNumber,
               Debug::IRValidation);

        if (!window) {
            SPLDBG("validateInputPortWindow: Window found for port " << portNumber,
                   Debug::IRValidation);
            SysOut::errorln(
              SPL_CORE_WINDOW_NOT_DEFINED_FOR_WINDOWED_PORT(portNumber, ir.getNames()[0]),
              ir.getLocation());
        }
    }
}

void IRValidator::validateInputPorts(PrimitiveOperatorInstance const& ir,
                                     Operator::OperatorModel const& om)
{
    InputPorts const& omPorts = om.getInputPorts();
    uint32_t numPorts = ir.numInputPorts();
    if (numPorts < omPorts.getMinNumberOfPorts() || numPorts > omPorts.getMaxNumberOfPorts()) {
        SysOut::errorln(SPL_CORE_INCORRECT_NUMBER_OF_INPUT_PORTS(ir.getNames()[0]),
                        ir.getLocation());
    }

    // For each IR port...
    for (uint32_t i = 0; i < numPorts; ++i) {
        if (i >= omPorts.getMaxNumberOfPorts())
            continue;
        // Validate the windowing mode
        validateInputPortWindow(omPorts, i, ir);

        // Check for prohibited tuple mutation in the input logic for this port
        Statement const* irStatement = ir.getLogic().getTuplePort(i);
        if (irStatement) {
            // Okay, we've got some logic.  Walk the statement and find all the LHSs of expressions that may modify an attribute
            TupleMutatorVisitorContext context(omPorts, ir.getNames()[0],
                                               TupleMutatorVisitorContext::Logic, "");
            DepthFirstVisitor<TupleMutatorVisitorContext> visitor;
            visitor.visit(context, *irStatement);
        }
    }
}

void IRValidator::validateAssignmentIsAttribute(Expression const& irExpr,
                                                PrimitiveOperatorInstance& ir)
{
    SPLDBG("validateAssignmentIsAttribute: expression " << irExpr << " has type "
                                                        << irExpr.getKind(),
           Debug::IRValidation);

    // In this case the expression should be an AttributeExpression and it's lhs should be a stream symbol and it's rhs should be a symbol expression
    if (irExpr.getKind() == Expression::Attribute) {
        AttributeExpression const& attrExpr = irExpr.as<AttributeExpression>();
        if (attrExpr.getLhs().getKind() == Expression::StreamSymbol &&
            attrExpr.getRhs().getKind() == Expression::Symbol) {
            return; // Seems like we have the right form
        }
    }
    SysOut::errorln(SPL_CORE_OUTPUT_ASSIGNMENT_NOT_ATTRIBUTE(irExpr.toString(), ir.getNames()[0]),
                    irExpr.getLocation());
}

void IRValidator::validateAssignmentIsAttributeFree(Expression const& irExpr,
                                                    PrimitiveOperatorInstance& ir)
{
    SPLDBG("validateAssignmentIsAttributeFree: expression " << irExpr << " has type "
                                                            << irExpr.getKind(),
           Debug::IRValidation);

    // The expression for the assignment should not refer to any stream attributes
    AttributeVisitorContext context(AttributeVisitorContext::OutputAssignment, "");
    DepthFirstVisitor<AttributeVisitorContext> visitor;
    visitor.visit(context, irExpr);
}

void IRValidator::validateAssignmentIsConstant(Expression const& irExpr,
                                               PrimitiveOperatorInstance& ir)
{
    // All we care about here is if the expression is const.
    ExpressionEvaluator ee(CompilerConfiguration::instance());
    auto_ptr<Expression> clone(&irExpr.clone());
    clone.reset(Expression::simplify(clone.release(), ee));
    if (clone->getKind() != Expression::Lit) {
        SysOut::errorln(
          SPL_CORE_OUTPUT_ASSIGNMENT_NOT_CONSTANT(irExpr.toString(), ir.getNames()[0]),
          irExpr.getLocation());
    }
}

void IRValidator::validateOutputPorts(PrimitiveOperatorInstance& ir,
                                      Operator::OperatorModel const& om)
{
    OutputPorts const& omPorts = om.getOutputPorts();
    uint32_t numPorts = ir.numOutputPorts();
    if (numPorts < omPorts.getMinNumberOfPorts() || numPorts > omPorts.getMaxNumberOfPorts()) {
        SysOut::errorln(SPL_CORE_INCORRECT_NUMBER_OF_OUTPUT_PORTS(ir.getNames()[0]),
                        ir.getLocation());
    }

    // We have a correct number of ports.  Perform validations for each port.
    for (uint32_t portNum = 0; portNum < numPorts; ++portNum) {
        PrimitiveOperatorInstance::Port const& irPort = ir.getOutputPort(portNum);
        PrimitiveOperatorInstance::Output& irOutput = ir.getOutput(portNum);
        OutputPort const& omPort = omPorts.getPortAt(portNum);
        assert(irPort.getTuple()); // tuple must be set by now
        TupleTyp const& tupleType = *irPort.getTuple();
        uint32_t numAttributes = tupleType.getNumberOfAttributes();

        vector<BinaryExpression const*> const& irAssignments = irOutput.get();

        // Validate that the assignment expressions match the expression mode.  We don't
        // need to worry about the type since that will have been checked in the FE.
        Operator::ExpressionMode const& opeMode = omPort.getExpressionMode();
        for (vector<BinaryExpression const*>::const_iterator itExpr = irAssignments.begin();
             itExpr != irAssignments.end(); ++itExpr) {
            BinaryExpression const& irExpr = **itExpr;
            // Split off the rhs as that is all we care about
            Expression const& rhs = irExpr.getRhs();
            if (opeMode == Operator::ExpressionMode::Expression) {
                validateAssignmentPortScope(rhs, omPort, portNum, ir);
            } else if (opeMode == Operator::ExpressionMode::Attribute) {
                validateAssignmentIsAttribute(rhs, ir);
                validateAssignmentPortScope(rhs, omPort, portNum, ir);
            } else if (opeMode == Operator::ExpressionMode::AttributeFree) {
                validateAssignmentIsAttributeFree(rhs, ir);
            } else if (opeMode == Operator::ExpressionMode::Constant) {
                validateAssignmentIsConstant(rhs, ir);
            } else if (opeMode == Operator::ExpressionMode::Nonexistent) {
                SysOut::errorln(SPL_CORE_OUTPUT_ASSIGNMENT_DISALLOWED(ir.getNames()[0], portNum),
                                ir.getLocation());
                break;
            }

            // Validate that the rhs does not mutate an input tuple
            InputPorts const& omInputPorts = om.getInputPorts();
            TupleMutatorVisitorContext context(omInputPorts, ir.getNames()[0],
                                               TupleMutatorVisitorContext::Output, "");
            DepthFirstVisitor<TupleMutatorVisitorContext> visitor;
            visitor.visit(context, rhs);
        }

        // If auto-assignment is not specified and if complete assignment is specified,
        // then there should be a complete set of assignments
        if (irAssignments.size() != numAttributes) {
            // We are short some assignments
            if (!omPort.isAutoAssignmentEnabled()) {
                if (omPort.isCompleteAssignmentRequired()) {
                    // Oops, we need to give an error
                    SysOut::errorln(
                      SPL_CORE_INCORRECT_NUMBER_OF_OUTPUT_ASSIGNMENTS(portNum, ir.getNames()[0]),
                      ir.getLocation());
                }
            } else {
                // Auto-assignment is enabled, so synthesize the missing assignment
                // Build a hash set of all the assignments we have
                unordered_set<string> assignments;
                for (uint32_t j = 0; j < irAssignments.size(); ++j) {
                    BinaryExpression const* irAssign = irAssignments[j];
                    Expression const& irLhs = irAssign->getLhs();
                    string const& attrName = getAttrNameFromExpression(irLhs);
                    assignments.insert(attrName);
                }

                // Walk the attributes and find the missing assignments
                for (uint32_t j = 0; j < numAttributes; ++j) {
                    string const& attrName = tupleType.getAttributeName(j);
                    unordered_set<string>::const_iterator it = assignments.find(attrName);
                    if (it == assignments.end()) {
                        SPLDBG("Synthesizing assignment for attribute " << attrName,
                               Debug::ShowSynthesizedAssignments);
                        synthesizeAssignment(ir, portNum, irOutput, attrName,
                                             tupleType.getAttributeType(j), tupleType, omPort);
                    }
                }
            }
        }
    }
}

void IRValidator::synthesizeAssignment(PrimitiveOperatorInstance const& ir,
                                       uint32_t portNum,
                                       PrimitiveOperatorInstance::Output& irOutput,
                                       string const& attrName,
                                       RootTyp const& attrType,
                                       TupleTyp const& tupleType,
                                       Operator::OutputPort const& omPort)
{

    // We need to build an assignment of the form attr = name.attr, where name is the input port stream name
    // The algorithm is to search the input ports in order until we find an attribute that is an exact match in name and type.
    // If a matching attribute is found but it is not in the port scope, then we'll keep looking to see if we find a match in another port.
    bool matchProhibitedByPortScope = false;
    for (uint32_t inputPortNum = 0; inputPortNum < ir.numInputPorts(); ++inputPortNum) {
        PrimitiveOperatorInstance::Port const& irInputPort = ir.getInputPort(inputPortNum);
        assert(irInputPort.getTuple());
        TupleTyp const& irInputTuple = *irInputPort.getTuple();

        // Find the index of the attribute in the input tuple
        TupleTyp::Name2Index const& inputTupleAttributes = irInputTuple.getAttributeNames();
        TupleTyp::Name2Index::const_iterator it = inputTupleAttributes.find(attrName);
        if (it != inputTupleAttributes.end()) {
            RootTyp const& bt = irInputTuple.getAttributeType(attrName);
            SourceLocation const& loc = SourceLocation::invalidSourceLocation;
            bool isUnderlyingType =
              attrType.isOptional() && attrType.as<OptionalTyp>().getUnderlyingType() == bt;
            if (bt == attrType || isUnderlyingType) {
                // We found a match in type and name
                // Make sure that the attribute we found is in an allowable port if port scope was specified.
                std::tr1::unordered_set<unsigned> const& portScopes =
                  omPort.getOutputAssignmentPortScope();
                // If there is a port scope see if this port is in it.
                // If not, keep track of the fact we fond one but could not match due to port scopes.
                if (portScopes.size() != 0) {
                    // A port scope was specified
                    unordered_set<unsigned>::const_iterator it1 = portScopes.find(inputPortNum);
                    if (it1 == portScopes.end()) {
                        // but our port wasn't in it :(
                        matchProhibitedByPortScope = true;
                        continue;
                    }
                }

                auto_ptr<SymbolExpression> memberExpr(
                  new SymbolExpression(bt, loc, attrName, Expression::Attr));
                auto_ptr<StreamSymbolExpression> portExpr(new StreamSymbolExpression(
                  irInputTuple, loc, irInputPort.getStreamName(), inputPortNum));
                auto_ptr<AttributeExpression> rhs(
                  new AttributeExpression(bt, loc, *portExpr.release(), *memberExpr.release()));
                auto_ptr<SymbolExpression> lhs(
                  new SymbolExpression(attrType, loc, attrName, Expression::Attr));
                if (isUnderlyingType) {
                    auto_ptr<CastExpression> castedRhs(
                      new CastExpression(attrType, loc, *rhs.release()));
                    auto_ptr<BinaryExpression> assign(new BinaryExpression(
                      attrType, loc, Expression::eq, *lhs.release(), *castedRhs.release()));
                    SPLDBG("Assignment '" << *assign << "' synthysized for attribute " << attrName,
                           Debug::ShowSynthesizedAssignments);
                    irOutput.add(*assign.release());
                } else {
                    auto_ptr<BinaryExpression> assign(new BinaryExpression(
                      attrType, loc, Expression::eq, *lhs.release(), *rhs.release()));
                    SPLDBG("Assignment '" << *assign << "' synthysized for attribute " << attrName,
                           Debug::ShowSynthesizedAssignments);
                    irOutput.add(*assign.release());
                }
                return;
            }
            // We found a match in name only.  Keep looking
        }
    }
    if (omPort.isCompleteAssignmentRequired()) {
        // We found no match at all or a match was blocked by port scope
        SysOut::errorln(SPL_CORE_CANNOT_AUTO_ASSIGN(attrName, portNum, ir.getNames()[0]),
                        ir.getLocation());
        if (matchProhibitedByPortScope) {
            SysOut::warnln(SPL_CORE_AUTO_ASSIGN_BLOCKED_BY_PORT_SCOPE(attrName), ir.getLocation());
        }
    }
}

string const& IRValidator::getAttrNameFromExpression(Expression const& expr)
{
    if (expr.getKind() == Expression::Attribute) {
        return getAttrNameFromExpression(expr.as<AttributeExpression>().getRhs());
    }

    assert(expr.getKind() == Expression::Symbol);
    return expr.as<SymbolExpression>().getId();
}
