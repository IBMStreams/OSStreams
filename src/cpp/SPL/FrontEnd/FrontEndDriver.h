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

#ifndef SPL_FRONT_END_DRIVER_H
#define SPL_FRONT_END_DRIVER_H

#include <SPL/FrontEnd/FrontEndStage.h>
#include <SPL/FrontEnd/TreeHelper.h>
#include <SPL/Runtime/Utility/Singleton.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <UTILS/HashMapHelpers.h>
#include <cassert>

namespace SPL {
class RootTyp;
class CompositeDefSym;
class CompositeDefinition;
class CompositeOperatorInstance;
class DriverCompilationUnit;
class MakeToolkit;
class ParserContext;
class SourceLocationFactory;
class SymbolTable;
class TypeFactory;
class UsedCompositeDefinitions;
class UsedOperatorInvocations;
class UsedSPLFunctions;

class DLL_PUBLIC FrontEndDriver : public Singleton<FrontEndDriver>
{
  public:
    typedef std::tr1::unordered_map<std::string, RootTyp const*> TypeMap;

    /// Constructor
    FrontEndDriver();

    /// Destructor
    ~FrontEndDriver();

    /// Get the types that are used by the application
    /// @return types that are used by the application
    TypeMap& getUsedTypes() { return *_usedTypes; }

    /// Get the types that are used by the application
    /// @return types that are used by the application (const version)
    const TypeMap& getUsedTypes() const { return *_usedTypes; }

    /// Return SPL Functions used in this compile
    /// @return SPL Functions defined in this compile
    UsedSPLFunctions& getUsedSPLFunctions() { return *_usedFunctions; }

    /// Return SPL Functions used in this compile
    /// @return SPL Functions defined in this compile (const version)
    const UsedSPLFunctions& getUsedSPLFunctions() const { return *_usedFunctions; }

    /// Return SPL Operators used in this compile
    /// @return SPL Operators defined in this compile
    UsedOperatorInvocations& getUsedOperatorInvocations() { return *_usedOperInvocations; }

    /// Return SPL Operators used in this compile
    /// @return SPL Operators defined in this compile (const version)
    const UsedOperatorInvocations& getUsedOperatorInvocations() const
    {
        return *_usedOperInvocations;
    }

    /// Return SPL Composites used in this compile
    /// @return SPL Composites defined in this compile
    UsedCompositeDefinitions& getUsedCompositeDefinitions() { return *_usedCompositeDefs; }

    /// Return SPL Composites used in this compile
    /// @return SPL Composites defined in this compile (const version)
    const UsedCompositeDefinitions& getUsedCompositeDefinitions() const
    {
        return *_usedCompositeDefs;
    }

    /// Return main (root) composite operator instance.
    /// @return main (root) composite operator instance.
    CompositeOperatorInstance& mainCompositeOpInstance()
    {
        assert(NULL != _mainCompositeOpInstance.get());
        return *_mainCompositeOpInstance;
    }

    /// Find the primary (main) composite.   It has no input or output streams
    /// Look for 'Main', then look for the first composite with no I/O
    /// @return primary Composite
    const CompositeDefinition* getMainComposite() const;

    /// Compile the SPL program
    void run();

    /// Find all the used types
    void collectUsedTypes();

    /// Should this front-end stage analyze this AST?
    /// @return whether this front-end stage should analyze this AST.
    bool shouldAnalyze(FrontEndStage::Value, AstNode&) const;

    /// Set the current toolkit index, and return the previous value
    /// @param newIndex new toolkit index to be set
    /// @return old toolkit index
    uint32_t setToolkitIndex(uint32_t newIndex)
    {
        uint32_t oldIndex = _currentToolkitIndex;
        _currentToolkitIndex = newIndex;
        return oldIndex;
    }

    /// Get the current toolkit index
    uint32_t getToolkitIndex() const { return _currentToolkitIndex; }

    /// If the compilation unit already exists, return its index, otherwise
    /// create it and return the new index. Parse it, run CurlyNormalizer and
    /// NameDefFinder, and add it to the list of compilation units to finish.
    int newCompilationUnit(std::string const& fileName, uint32_t toolkitIndex);

    /// Run all front-end stages up to 'tillWhere' on all compilation units
    /// for which they haven't run yet.
    void finishCompilationUnits(SPL::FrontEndStage::Value tillWhere);

    /// Extract the compilation unit signature of the file named by
    /// 'ExplicitInputFile', and save the results in the 'makeToolkit'.
    void extractSignatures(MakeToolkit& toolkitMaker);

  private:
    int newCompilationUnit(std::string const& fileName, uint32_t toolkitIndex, bool isMain);

    std::vector<DriverCompilationUnit*> _compilationUnits;
    /// auto_ptr's are used to avoid header includes
    std::auto_ptr<AstPool> _astPool;
    std::auto_ptr<TypeFactory> _typeFactory;
    std::auto_ptr<SourceLocationFactory> _sourceLocationFactory;
    std::auto_ptr<SymbolTable> _symbolTable;
    std::auto_ptr<TypeMap> _usedTypes;
    std::auto_ptr<UsedSPLFunctions> _usedFunctions;
    std::auto_ptr<CompositeOperatorInstance> _mainCompositeOpInstance;
    std::auto_ptr<UsedCompositeDefinitions> _usedCompositeDefs;
    std::auto_ptr<UsedOperatorInvocations> _usedOperInvocations;

    void parseAndAnalyze(std::string const& fileName);
    uint32_t _currentToolkitIndex;
};
};

#endif /* SPL_FRONT_END_DRIVER_H*/
