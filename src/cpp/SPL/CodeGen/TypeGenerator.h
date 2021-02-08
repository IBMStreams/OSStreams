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

#ifndef SPL_TUPLE_GENERATOR_H
#define SPL_TUPLE_GENERATOR_H

#include <SPL/Runtime/Utility/Singleton.h>

#include <SPL/CodeGen/CodeGenHelper.h>
#include <UTILS/HashMapHelpers.h>

namespace SPL {
class RootTyp;
class CompilerConfiguration;
class EnumTyp;
class TupleTyp;
class XMLTyp;

namespace Enumeration_ {
class EnumerationModel;
};

namespace Tuple_ {
class TupleModel;
};

namespace XML_ {
class XMLModel;
};

class TypeGenerator : public SPL::Singleton<TypeGenerator>
{
    typedef Enumeration_::EnumerationModel EnumerationModel;
    typedef Tuple_::TupleModel TupleModel;
    typedef XML_::XMLModel XMLModel;

  public:
    TypeGenerator()
      : Singleton<TypeGenerator>(*this)
      , config_(CompilerConfiguration::instance())
      , codeGenHelper_(CodeGenHelper::instance())
    {}

    ~TypeGenerator();

    /// Generate code for all tuples and enums
    void generateCPPCode();

    /// Build models for all used types
    void buildModels();

    /// Given an EnumTyp, find the associated EnumerationModel.
    /// @param type The type for which we want the model
    /// @return Returns the model
    EnumerationModel const& getEnumModel(EnumTyp const& type) const;

    /// Given the name of an enumeration, get its model
    /// @param type The name of the enumeration type
    /// @return Returns the model
    EnumerationModel const& getEnumModel(std::string const& type) const;

    /// Given an XMLTyp, find the associated XMLModel.
    /// @param type The type for which we want the model
    /// @return Returns the model
    XMLModel const& getXMLModel(XMLTyp const& type) const;

    /// Given the name of an xml type, get its model
    /// @param type The name of the xml type
    /// @return Returns the model
    XMLModel const& getXMLModel(std::string const& type) const;

    /// Given a TupleTyp, find the associated TupleModel.
    /// @param type The type for which we want the model
    /// @return Returns the model
    TupleModel const& getTupleModel(TupleTyp const& type) const;

    /// Given the name of a tuple, get its model
    /// @param type The name of the tuple type
    /// @return Returns the model
    TupleModel const& getTupleModel(std::string const& type) const;

    /// Get the c++ type name for the give RootTyp
    /// @param type The type for which we want the c++ type name
    /// @return Returns the c++ type name
    std::string const& getCppType(RootTyp const& type) const;

    /// Given a TupleModel, find the associate TupleTyp.
    /// @param model The model for which we want the type
    /// @return Returns the type
    TupleTyp const& getTupleType(TupleModel const& model) const;

    /// Given an EnumModel, find the associate TupleTyp.
    /// @param model The model for which we want the type
    /// @return Returns the type
    EnumTyp const& getEnumType(EnumerationModel const& model) const;

    /// Given an XMLModel, find the associate XMLTyp.
    /// @param model The model for which we want the type
    /// @return Returns the type
    XMLTyp const& getXMLType(XMLModel const& model) const;

    /// Can this tuple be a facade?
    /// @param tuple Tyuple type to be examined
    /// @return 'true' if it is legal to be a facade tuple
    bool isFacade(TupleTyp const& tuple) const;

    /// Can this tuple suport ==, !=?
    /// @param tuple Tyuple type to be examined
    /// @return 'true' if it is legal compare tuples of this type
    bool isEqualitySupported(TupleTyp const& tuple) const;

  private:
    void setPaths();
    void mangleTypes();
    void buildModel(TupleTyp const& tuple, TupleModel& model);
    void buildModel(EnumTyp const& enum_, EnumerationModel& model);
    void buildModel(XMLTyp const& xml, XMLModel& model);
    template<class T>
    std::string listOrSetType(RootTyp const& type,
                              const char* collectionName,
                              std::tr1::unordered_set<std::string>& headers) const;
    template<class T>
    std::string blistOrSetType(RootTyp const& type,
                               const char* collectionName,
                               std::tr1::unordered_set<std::string>& headers) const;
    template<class T>
    std::string mapType(RootTyp const& type,
                        const char* collectionName,
                        std::tr1::unordered_set<std::string>& headers) const;

    template<class T>
    std::string bmapType(RootTyp const& type,
                         const char* collectionName,
                         std::tr1::unordered_set<std::string>& headers) const;

    void launchGeneratorIfNeeded(TupleModel const& model);
    void launchGeneratorIfNeeded(EnumerationModel const& model);

    void genCodeGen(std::string const& type,
                    std::string const& templtFile,
                    std::string const& generator) const;

    // Returns the C++ name for a tuple's type.
    std::string typeName(RootTyp const& type, std::tr1::unordered_set<std::string>& headers) const;

    bool isFacadeScalarType(RootTyp const& tuple) const;
    std::tr1::unordered_map<TupleTyp const*, TupleModel*> tuples_;
    std::tr1::unordered_map<TupleModel const*, TupleTyp const*> reverseTupleMap_;
    std::tr1::unordered_map<EnumTyp const*, EnumerationModel*> enums_;
    std::tr1::unordered_map<EnumerationModel const*, EnumTyp const*> reverseEnumMap_;
    std::tr1::unordered_map<XMLTyp const*, XMLModel*> xmls_;
    std::tr1::unordered_map<XMLModel const*, XMLTyp const*> reverseXMLMap_;
    mutable std::tr1::unordered_map<RootTyp const*, std::string> cppnames_;

    std::string
      targetDirectory_; //!< Directory that will hold the generated code for tuples and enums
    CompilerConfiguration const& config_;
    CodeGenHelper const& codeGenHelper_;
};
};

#endif /* SPL_TUPLE_GENERATOR_H */
