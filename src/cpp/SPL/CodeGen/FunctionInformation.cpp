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

#include <SPL/CodeGen/FunctionInformation.h>

#include <SPL/CodeGen/Statement.h>
#include <SPL/CodeGen/TypeGenerator.h>
#include <SPL/Core/EnumerationModelImpl.h>
#include <SPL/Core/TupleModelImpl.h>
#include <SPL/Core/XMLModelImpl.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/SysOut.h>
#include <SPL/Utility/Utility.h>
#include <UTILS/SupportFunctions.h>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>

using namespace SPL;
using namespace std;
using namespace std::tr1;
namespace bf = boost::filesystem;

void SPL::FunctionInformation::addFunction(const FunctionHeadSym& fcnHead, BlockStatement& block)
{
    OneFcnInfo f(fcnHead, block);
    _fcns.push_back(f);
}

void SPL::FunctionInformation::functionIncludes(const unordered_set<const FunctionHeadSym*>& fcns)
{
    unordered_set<const FunctionHeadSym*>::const_iterator it;
    for (it = fcns.begin(); it != fcns.end(); it++) {
        const FunctionHeadSym* fh = *it;
        const vector<Function::LibraryPtr>* deps = fh->dependencies();
        // Look for native functions
        if (fh->isNativeFunction()) {
            // Exclude the base toolkit one
            if (fh->includeIsBuiltin()) {
                continue; // builtin native functions already handled
            }
            const string& include = fh->includeFileName();
            if (!include.empty()) {
                _fcnNativeIncludes.insert(include);
            }

            // Handle this differently, as we have named includes & libs
            if (deps) {
                for (uint32_t i = 0; i < deps->size(); i++) {
                    const Common::Library& l = *(*deps)[i];
                    uint32_t j;
                    assert(l.isManaged());
                    if (l.isManaged()) {
                        const Common::ManagedLibrary& m = l.managed();
                        const vector<string>& libs = m.getLibs();
                        for (j = 0; j < libs.size(); j++) {
                            _fcnLibs.insert(libs[j]);
                        }
                        const vector<string>& libPaths = m.getLibPaths();
                        for (j = 0; j < libPaths.size(); j++) {
                            bf::path libPath = libPaths[j];
                            if (libPath.is_complete()) {
                                Utility::makeAppDirRelativePathOutputDirRelative(libPath);
                            }
                            _fcnLibPaths.insert(libPath.string());
                        }
                        const vector<string>& includePaths = m.getIncludePaths();
                        for (j = 0; j < includePaths.size(); j++) {
                            bf::path includePath = includePaths[j];
                            if (!includePath.is_complete()) {
                                Utility::makeAppDirRelativePathOutputDirRelative(includePath);
                            }
                            _fcnIncludePaths.insert(includePath.string());
                        }
                    }
                }
            }
            continue;
        }

        // SPL functions
        string n = fh->nameSpace();
        string fname;
        if (!SymbolTableEntry::isDefaultNameSpace(n)) {
            vector<string> ns;
            Distillery::tokenize(n, ns, ".", false);
            for (uint32_t i = 0; i < ns.size(); i++) {
                fname += ns[i] + '/';
            }
        }
        fname += fh->name() + ".h";
        _fcnIncludes.insert(fname);
    }
}

void FunctionInformation::typeIncludes(const unordered_set<const RootTyp*>& types)
{
    // Walk the types, and generate the include files from the names
    // The types are either enums or tuples
    TypeGenerator& tg = TypeGenerator::instance();

    unordered_set<const RootTyp*>::const_iterator it;
    for (it = types.begin(); it != types.end(); it++) {
        const RootTyp& p = **it;
        string fname;
        if (p.is<EnumTyp>()) {
            const EnumTyp& em = p.as<EnumTyp>();
            fname = tg.getEnumModel(em).getClass();
        } else if (p.is<XMLTyp>()) {
            const XMLTyp& xt = p.as<XMLTyp>();
            fname = tg.getXMLModel(xt).getClass();
        } else {
            assert(p.is<TupleTyp>());
            const TupleTyp& ty = p.as<TupleTyp>();
            fname = tg.getTupleModel(ty).getClass();
        }
        fname += ".h";
        _typeIncludes.insert(fname);
    }
}
