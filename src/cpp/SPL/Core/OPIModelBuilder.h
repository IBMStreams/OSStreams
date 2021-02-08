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

#ifndef SPL_OPI_MODEL_BUILDER_H
#define SPL_OPI_MODEL_BUILDER_H

#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/Statement.h>
#include <SPL/Core/OperatorInstanceModelImpl.h>
#include <UTILS/HashMapHelpers.h>
#include <memory>
#include <string>
#include <vector>

class HistoryVisitorContext;

namespace SPL {
class RootTyp;
class FunctionHEad;
class Literal;
class LiteralReplacer;
class OperatorGraphNode;
class PrimitiveOperatorInstance;

namespace Operator {
class OperatorModel;
}

class OPIModelBuilder
{
    typedef Operator::Instance::Context Context;
    typedef Operator::OperatorModel OperatorModel;
    typedef Operator::Instance::OperatorInstanceModel OperatorInstanceModel;
    typedef std::tr1::unordered_map<std::string, int> TypeMap;

  public:
    static void build(PrimitiveOperatorInstance const&, OperatorGraphNode& node);
    static void verify(PrimitiveOperatorInstance const&, OperatorGraphNode& node);

  private:
    static void populateParameters(PrimitiveOperatorInstance const& ir,
                                   OperatorInstanceModel& oim,
                                   OperatorModel const& om,
                                   LiteralReplacer& litRep,
                                   HistoryVisitorContext& historyContext,
                                   TypeMap& typeMap,
                                   bool verify);

    static void populateContext(PrimitiveOperatorInstance const& ir,
                                OperatorInstanceModel& oim,
                                OperatorModel const& om,
                                LiteralReplacer& litRep,
                                TypeMap& typeMap,
                                std::string const& operatorModelDirectory,
                                bool verify);

    static void populateInputPorts(PrimitiveOperatorInstance const& ir,
                                   OperatorInstanceModel& oim,
                                   OperatorModel const& om,
                                   LiteralReplacer& litRep,
                                   HistoryVisitorContext& historyContext,
                                   bool verify);

    static void populateOutputPorts(PrimitiveOperatorInstance const& ir,
                                    OperatorInstanceModel& oim,
                                    OperatorModel const& om,
                                    LiteralReplacer& litRep,
                                    HistoryVisitorContext& historyContext,
                                    TypeMap& typeMap,
                                    bool verify);

    static bool simplifyAndReplaceLiterals(std::auto_ptr<Expression>& expr,
                                           LiteralReplacer& litRep,
                                           bool onlySTP);
    static void simplifyAndReplaceLiterals(std::auto_ptr<Statement>& stmt, LiteralReplacer& litRep);

    static void analyzeExpression(Expression const& expr,
                                  bool& hasFunctions,
                                  bool& hasSideEffects,
                                  bool& readsState);

    static Operator::Instance::ExpressionPtr buildStringExpression(std::string const& value);
    static Operator::Instance::ExpressionPtr buildGetToolkitDirectoryRelativeExpression(
      const std::string& toolkitName,
      std::string const& relativePath,
      LiteralReplacer& litRep,
      TypeMap* typeMap,
      const PrimitiveOperatorInstance& ir);

    static Operator::Instance::ExpressionPtr buildExpression(Expression const& expr,
                                                             bool rewriteIsAllowed,
                                                             LiteralReplacer& litRep,
                                                             TypeMap* typeMap,
                                                             bool genCppCodeInSPLExpnTree,
                                                             PrimitiveOperatorInstance const& ir,
                                                             bool verify);

    static std::string const& getAttrNameFromExpression(Expression const& expr);
    static OperatorModel const& getOperatorModel(PrimitiveOperatorInstance const& ir,
                                                 OperatorInstanceModel& oim,
                                                 std::string& operatorModelDirectory);
    static void doBuild(OperatorInstanceModel& oim,
                        PrimitiveOperatorInstance const&,
                        OperatorGraphNode& node,
                        bool verify);
};
};

#endif /* SPL_OPI_MODEL_BUILDER_H */
