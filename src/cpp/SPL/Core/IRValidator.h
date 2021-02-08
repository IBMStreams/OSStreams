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

#ifndef SPL_IR_VALIDATOR_H
#define SPL_IR_VALIDATOR_H

#include <SPL/CodeGen/OperatorIR.h>
#include <SPL/Core/OperatorModelImpl.h>

#include <string>

namespace SPL {

class RootTyp;
class Expression;
class OperatorGraphNode;
class Param;
class TupleTyp;

class IRValidator
{
  public:
    static void validate(PrimitiveOperatorInstance& ir, OperatorGraphNode& node);

  private:
    static void validateParameters(PrimitiveOperatorInstance const& ir,
                                   Operator::OperatorModel const& om);
    static void validateInputPorts(PrimitiveOperatorInstance const& ir,
                                   Operator::OperatorModel const& om);
    static void validateOutputPorts(PrimitiveOperatorInstance& ir,
                                    Operator::OperatorModel const& om);
    static void synthesizeAssignment(PrimitiveOperatorInstance const& ir,
                                     uint32_t portNum,
                                     PrimitiveOperatorInstance::Output& irOutput,
                                     std::string const& attrName,
                                     RootTyp const& attrType,
                                     TupleTyp const& tupleType,
                                     Operator::OutputPort const& omPort);
    static void validateParameterExpressionIsAttribute(Expression const& irExpr,
                                                       Operator::Parameter const& omParameter,
                                                       PrimitiveOperatorInstance const& ir);
    static void validateParameterExpressionIsAttributeFree(Expression const& irExpr,
                                                           Operator::Parameter const& omParameter,
                                                           PrimitiveOperatorInstance const& ir);
    static void validateParameterExpressionIsConstant(Expression const& irExpr,
                                                      Operator::Parameter const& omParameter,
                                                      PrimitiveOperatorInstance const& ir);
    static void validateParameterExpressionIsCustomLiteral(Expression const& irExpr,
                                                           Operator::Parameter const& omParameter,
                                                           PrimitiveOperatorInstance const& ir);
    static void validateParameterPortScope(Expression const& irExpr,
                                           Operator::Parameter const& omParameter,
                                           PrimitiveOperatorInstance const& ir);
    static void validateAssignmentIsAttribute(Expression const& irExpr,
                                              PrimitiveOperatorInstance& ir);
    static void validateAssignmentIsAttributeFree(Expression const& irExpr,
                                                  PrimitiveOperatorInstance& ir);
    static void validateAssignmentIsConstant(Expression const& irExpr,
                                             PrimitiveOperatorInstance& ir);
    static void validateInputPortWindow(Operator::InputPorts const& omPorts,
                                        uint32_t portNumber,
                                        PrimitiveOperatorInstance const& ir);
    static void validateAssignmentPortScope(Expression const& irExpr,
                                            Operator::OutputPort const& omPort,
                                            uint32_t portNum,
                                            PrimitiveOperatorInstance const& ir);
    static std::string const& getAttrNameFromExpression(Expression const& expr);
};
};

#endif /* SPL_IR_VALIDATOR_H */
