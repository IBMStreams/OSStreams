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

#include <SPL/Core/CommonModelImpl.h>
#include <SPL/Core/FunctionModelImpl.h>

using namespace std;
using namespace std::tr1;
using namespace SPL;
using namespace SPL::Function;
using namespace SPL::Common;
using boost::shared_ptr;

IMPL_EXCEPTION(SPL, SPLCompilerInvalidFunctionModel, SPLCompiler);

FunctionModel::FunctionModel(const FunctionModel& ot)
{
    this->operator=(ot);
}

FunctionModel::FunctionModel(const functionModelType& xml, const std::string& baseDir)
{
    typedef functionModelType::functionSet_const_iterator myiter;
    myiter itbeg = xml.functionSet().begin();
    myiter itend = xml.functionSet().end();
    for (myiter it = itbeg; it != itend; ++it)
        functionSets_.push_back(FunctionSetPtr(new FunctionSet(*it, baseDir)));
}

const FunctionModel& FunctionModel::operator=(const FunctionModel& ot)
{
    functionSets_.clear();
    const vector<FunctionSetPtr>& functionSets = ot.getFunctionSets();
    for (size_t i = 0, iu = functionSets.size(); i < iu; ++i)
        functionSets_.push_back(FunctionSetPtr(new FunctionSet(*functionSets[i])));
    return *this;
}

auto_ptr<functionModelType> FunctionModel::toXsdInstance() const
{
    auto_ptr<functionModelType> model(new functionModelType());
    for (size_t i = 0, iu = functionSets_.size(); i < iu; ++i) {
        auto_ptr<functionSetType> fset = functionSets_[i]->toXsdInstance();
        model->functionSet().push_back(fset);
    }
    return model;
}

FunctionSet::FunctionSet(const FunctionSet& ot)
{
    this->operator=(ot);
}

const FunctionSet& FunctionSet::operator=(const FunctionSet& ot)
{
    headerFileName_ = ot.headerFileName_;
    cppNamespaceName_ = ot.cppNamespaceName_;
    functions_ = ot.functions_;
    dependencies_.clear();
    const vector<LibraryPtr>& dependencies = ot.getDependencies();
    for (size_t i = 0, iu = dependencies.size(); i < iu; ++i)
        dependencies_.push_back(LibraryPtr(new Library(*dependencies[i])));
    return *this;
}

FunctionSet::FunctionSet(const functionSetType& xml, const std::string& baseDir)
{
    if (xml.headerFileName().present())
        headerFileName_ = xml.headerFileName().get();
    if (xml.cppNamespaceName().present())
        cppNamespaceName_ = xml.cppNamespaceName().get();
    const functionsType& funcs = xml.functions();
    typedef functionsType::function_const_iterator myiter;
    myiter itbeg = funcs.function().begin();
    myiter itend = funcs.function().end();
    for (myiter it = itbeg; it != itend; ++it) {
        std::string desc = it->description().present() ? it->description().get() : "";
        const prototypeType& proto = it->prototype();
        uint32_t line = proto.line().present() ? proto.line().get() : 0;
        uint32_t column = proto.column().present() ? proto.column().get() : 0;
        std::string cppname = proto.cppName().present() ? proto.cppName().get() : "";
        functions_.push_back(CppNameLocation(proto, line, column, cppname, desc));
    }
    if (xml.dependencies().present()) {
        const dependenciesType& libs = xml.dependencies().get();
        typedef dependenciesType::library_const_iterator myiter;
        myiter itbeg1 = libs.library().begin();
        myiter itend1 = libs.library().end();
        for (myiter it = itbeg1; it != itend1; ++it)
            dependencies_.push_back(LibraryPtr(new Library(*it, baseDir)));
    }
}

auto_ptr<functionSetType> FunctionSet::toXsdInstance() const
{
    auto_ptr<functionSetType> functionSet(new functionSetType());
    if (hasHeaderFileName())
        functionSet->headerFileName().set(headerFileName_);
    if (hasCppNamespaceName())
        functionSet->cppNamespaceName().set(cppNamespaceName_);
    auto_ptr<functionsType> functions(new functionsType());
    for (size_t i = 0, iu = functions_.size(); i < iu; ++i) {
        const CppNameLocation& nl = functions_[i];
        auto_ptr<prototypeType> proto(new prototypeType(nl.name()));
        if (nl.line())
            proto->line(nl.line());
        if (nl.col())
            proto->column(nl.col());
        if (!nl.hasCppName())
            proto->cppName(nl.getCppName());
        functionType f(*proto);
        if (nl.hasDescription())
            f.description(nl.getDescription());
        functions->function().push_back(f);
    }
    functionSet->functions(functions);
    if (!dependencies_.empty()) {
        auto_ptr<dependenciesType> dependencies(new dependenciesType());
        for (size_t i = 0, iu = dependencies_.size(); i < iu; ++i) {
            auto_ptr<libraryType> lib = dependencies_[i]->toXsdInstance();
            dependencies->library().push_back(lib);
        }
        functionSet->dependencies().set(dependencies);
    }
    return functionSet;
}
