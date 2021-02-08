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

#ifndef SPL_CORE_IMPORTSPEC_H
#define SPL_CORE_IMPORTSPEC_H

#include <SPL/FrontEnd/MetaType.h>
#include <assert.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace SPL {
class Param;
class PrimitiveOperatorInstance;
class SourceLocation;
class Expression;
class ExpressionEvaluator;

/// Define the Import Specification for an Import port
/// Example:
///   param applicationScope : "myAppScope";   (may be empty if omitted)
///         application : som.namespace::MainOp;
///         subscription : expression
///         filter : another expression
///  special case: expression is 'streamID == "string"'
///  This defines a 'name based import'
class ImportSpec
{
  public:
    ~ImportSpec();

    /// Get the operator IR used to create this import spec
    /// @return operator IR used to create this import spec
    PrimitiveOperatorInstance const& getOperatorIR() const { return _operatorIR; }

    /// Is this import a stream named one?
    /// @return 'true' if the expression is 'streamID == "string"'
    bool isNameBased() const { return _isNameBased; }

    /// Return the application name for the import spec
    /// @return the application name
    const std::string& getAppName() const { return _appName; }

    /// Check if application name is specified
    /// @return true if there is an application name
    bool hasAppName() const { return !_appName.empty(); }

    /// Return the application scope for the import spec
    /// @return the application scope
    const std::string& getAppScope() const { return _appScope; }

    /// Check if application scope is specified
    /// @return true if there is an application scope
    bool hasAppScope() const { return !_appScope.empty(); }

    /// Return the stream name for the import spec
    /// @return the stream name
    const std::string& getStreamName() const
    {
        assert(_isNameBased);
        return _streamName;
    }

    /// Return the SPL expression used to generate the subscription for ADL
    /// @return expression
    const Expression& getSubscription() const
    {
        assert(!_isNameBased);
        return *_expn;
    }

    /// Is the subscription expression empty?
    /// @return 'true' if the expression is not present, but this is not by name.
    bool isEmpty() const
    {
        assert(!_isNameBased);
        return _expn == NULL;
    }

    /// Construct an import spec from the parameters to an Import operator
    /// @param importOp import operator invocation
    /// @return constructed ImportSpec
    static const ImportSpec* createFromImportOper(const PrimitiveOperatorInstance& importOp);

    /// Print an Import Spec
    std::ostream& print(std::ostream&) const;

    /// Return the filter expression
    /// @return pointer to filter expression or NULL if no filter expression
    const Expression* getFilter() const { return _filterExpn; }

  private:
    // empty subscription expression
    ImportSpec(const std::string& appScope,
               const std::string& appName,
               const PrimitiveOperatorInstance& operatorIR);

    ImportSpec(const std::string& appScope,
               const std::string& appName,
               const Expression& expn,
               const PrimitiveOperatorInstance& operatorIR);

    ImportSpec(const std::string& appScope,
               const std::string& appName,
               const std::string& streamId,
               const PrimitiveOperatorInstance& operatorIR);

    ImportSpec(const std::string& appScope,
               const std::string& appName,
               const Expression& expn,
               const std::string& subscriptionString,
               const PrimitiveOperatorInstance& operatorIR);

    enum Context
    {
        Literal,
        General,
        Symbol,
        SymbolMod,
        SymbolIn
    };
    void checkSubscriptionExpression(const Expression&, Context) const;
    MetaType checkFilterExpression(const Expression&,
                                   Context,
                                   const PrimitiveOperatorInstance& operatorIR) const;
    bool findStreamName(const Expression&, std::string& streamName) const;
    void invalidExpn(const Expression& e) const;
    void invalidFilterExpn(const Expression& e) const;
    std::auto_ptr<const Expression> simplify(const Expression&) const;
    Expression* convertFromString(const std::string& expn, const Expression& e);
    Expression* convertFilterFromString(const std::string& expn,
                                        const Expression& e,
                                        const PrimitiveOperatorInstance& operatorIR);

    static const ImportSpec& addFilter(ImportSpec& is,
                                       const std::string* filterString,
                                       const Expression* filter,
                                       const PrimitiveOperatorInstance& operatorIR);
    void addFilterExpression(const Expression& filterExpn,
                             const PrimitiveOperatorInstance& operatorIR);
    void addFilterExpression(const std::string& str,
                             const Expression& filterExpn,
                             const PrimitiveOperatorInstance& operatorIR);

    MetaType getMetaType(const Expression& expn, const PrimitiveOperatorInstance& operatorIR) const;
    MetaType getElementMetaType(const Expression& expn,
                                const PrimitiveOperatorInstance& operatorIR) const;
    const Expression* _expn;
    const Expression* _filterExpn;
    std::string _streamName;
    std::string _appScope;
    std::string _appName;
    bool _isNameBased;

    PrimitiveOperatorInstance const& _operatorIR;
    std::auto_ptr<ExpressionEvaluator> _ee;
};
};

#endif // SPL_CORE_IMPORTSPEC_H
