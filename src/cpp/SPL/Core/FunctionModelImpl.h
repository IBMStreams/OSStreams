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

#ifndef FUNCTION_MODEL_IMPL_H
#define FUNCTION_MODEL_IMPL_H

#include <SPL/Core/CommonModelImpl.h>
#include <SPL/Core/functionModel.h> // xsd-generated
#include <SPL/Runtime/Utility/Visibility.h>
#include <SPL/Utility/NameLocation.h>

PUSH_DLL_PUBLIC
namespace SPL {

DECL_EXCEPTION(SPL, SPLCompilerInvalidFunctionModel, SPLCompiler);

namespace Function {

using namespace xmlns::prod::streams::spl::function;
#include <SPL/Core/FunctionModelForwardDeclarations.h>

class FunctionModel
{
  public:
    FunctionModel() {}
    FunctionModel(const FunctionModel& ot);
    FunctionModel(const functionModelType& xml, const std::string& baseDir) DLL_PUBLIC;
    const std::vector<FunctionSetPtr>& getFunctionSets() const { return functionSets_; }
    std::vector<FunctionSetPtr>& getFunctionSets() { return functionSets_; }
    const FunctionModel& operator=(const FunctionModel& ot);
    std::auto_ptr<functionModelType> toXsdInstance() const DLL_PUBLIC;

  private:
    std::vector<FunctionSetPtr> functionSets_;
};

class CppNameLocation : public NameLocation
{
  public:
    CppNameLocation(const std::string& nameIn,
                    uint32_t lineIn,
                    uint32_t colIn,
                    const std::string& cppName,
                    const std::string& description)
      : NameLocation(nameIn, lineIn, colIn)
      , _cppName(cppName)
      , _description(description)
    {}

    bool hasCppName() const { return !_cppName.empty(); }
    const std::string& getCppName() const { return _cppName; }
    bool hasDescription() const { return !_description.empty(); }
    const std::string& getDescription() const { return _description; }

  private:
    std::string _cppName;
    std::string _description;
};

class FunctionSet
{
  public:
    FunctionSet() {}
    FunctionSet(const FunctionSet& ot);
    FunctionSet(const functionSetType& xml, const std::string& baseDir);
    bool hasCppNamespaceName() const { return !cppNamespaceName_.empty(); }
    const std::string& getCppNamespaceName() const { return cppNamespaceName_; }
    std::string& getCppNamespaceName() { return cppNamespaceName_; }
    bool hasHeaderFileName() const { return !headerFileName_.empty(); }
    const std::string& getHeaderFileName() const { return headerFileName_; }
    const std::vector<CppNameLocation>& getFunctions() const { return functions_; }
    std::vector<CppNameLocation>& getFunctions() { return functions_; }
    const std::vector<LibraryPtr>& getDependencies() const { return dependencies_; }
    std::vector<LibraryPtr>& getDependencies() { return dependencies_; }
    bool hasDependencies() const { return !dependencies_.empty(); }
    const FunctionSet& operator=(const FunctionSet& ot);
    std::auto_ptr<functionSetType> toXsdInstance() const;

  private:
    std::string cppNamespaceName_;
    std::string headerFileName_;
    std::vector<CppNameLocation> functions_;
    std::vector<LibraryPtr> dependencies_;
};
}
}
POP_DLL_PUBLIC

#endif /* FUNCTION_MODEL_IMPL_ */
