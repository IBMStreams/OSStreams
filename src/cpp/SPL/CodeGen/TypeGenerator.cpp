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

#include <SPL/CodeGen/TypeGenerator.h>

#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/CompilerMessage.h>
#include <SPL/Core/EnumerationModelImpl.h>
#include <SPL/Core/TupleModelImpl.h>
#include <SPL/Core/TypeMangler.h>
#include <SPL/Core/XMLModelImpl.h>
#include <SPL/FrontEnd/FrontEndDriver.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/SysOut.h>
#include <SPL/Utility/Utility.h>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <iostream>

using namespace std;
using namespace std::tr1;
using namespace SPL;
using namespace Enumeration_;
using namespace Tuple_;

namespace bf = boost::filesystem;

#include <SPL/Runtime/Utility/Singleton.t>
template class Singleton<TypeGenerator>; // Explicitly instatiate the singleton code here

TypeGenerator::EnumerationModel const& TypeGenerator::getEnumModel(EnumTyp const& type) const
{
    unordered_map<EnumTyp const*, EnumerationModel*>::const_iterator it = enums_.find(&type);
    if (it == enums_.end()) {
        THROW(SPLCompilerAssertionFailed,
              "Enumeration model does not exist for type '" << type.getName() << "'");
    }
    return *(it->second);
}

TypeGenerator::EnumerationModel const& TypeGenerator::getEnumModel(string const& type) const
{
    const FrontEndDriver::TypeMap& tmap = FrontEndDriver::instance().getUsedTypes();
    FrontEndDriver::TypeMap::const_iterator it = tmap.find(type);
    if (it == tmap.end()) {
        THROW(SPLCompilerAssertionFailed,
              "Enumeration model does not exist for type '" << type << "'");
    }
    RootTyp const* bt = it->second;
    EnumTyp const* et = dynamic_cast<EnumTyp const*>(bt);
    if (et == NULL) {
        THROW(SPLCompilerAssertionFailed,
              "Enumeration model does not exist for non-tuple type '" << type << "'");
    }
    return getEnumModel(*et);
}

TypeGenerator::XMLModel const& TypeGenerator::getXMLModel(XMLTyp const& type) const
{
    unordered_map<XMLTyp const*, XMLModel*>::const_iterator it = xmls_.find(&type);
    if (it == xmls_.end()) {
        THROW(SPLCompilerAssertionFailed,
              "XML model does not exist for type '" << type.getName() << "'");
    }
    return *(it->second);
}

TypeGenerator::XMLModel const& TypeGenerator::getXMLModel(string const& type) const
{
    const FrontEndDriver::TypeMap& tmap = FrontEndDriver::instance().getUsedTypes();
    FrontEndDriver::TypeMap::const_iterator it = tmap.find(type);
    if (it == tmap.end()) {
        THROW(SPLCompilerAssertionFailed, "XML model does not exist for type '" << type << "'");
    }
    RootTyp const* bt = it->second;
    if (!bt->is<XMLTyp>()) {
        THROW(SPLCompilerAssertionFailed,
              "XML model does not exist for non-tuple type '" << type << "'");
    }
    XMLTyp const& xt = bt->as<XMLTyp>();
    return getXMLModel(xt);
}

TypeGenerator::~TypeGenerator()
{
    for (unordered_map<TupleTyp const*, TupleModel*>::const_iterator it = tuples_.begin();
         it != tuples_.end(); ++it) {
        delete it->second;
    }
    for (unordered_map<EnumTyp const*, EnumerationModel*>::const_iterator it = enums_.begin();
         it != enums_.end(); ++it) {
        delete it->second;
    }
    for (unordered_map<XMLTyp const*, XMLModel*>::const_iterator it = xmls_.begin();
         it != xmls_.end(); ++it) {
        delete it->second;
    }
}

TypeGenerator::TupleModel const& TypeGenerator::getTupleModel(TupleTyp const& type) const
{
    unordered_map<TupleTyp const*, TupleModel*>::const_iterator it = tuples_.find(&type);
    if (it == tuples_.end()) {
        THROW(SPLCompilerAssertionFailed,
              "Tuple model does not exist for type '" << type.getName() << "'");
    }
    return *(it->second);
}

TypeGenerator::TupleModel const& TypeGenerator::getTupleModel(string const& type) const
{
    FrontEndDriver::TypeMap const& tmap = FrontEndDriver::instance().getUsedTypes();
    FrontEndDriver::TypeMap::const_iterator it = tmap.find(type);
    if (it == tmap.end()) {
        THROW(SPLCompilerAssertionFailed, "Tuple model does not exist for type '" << type << "'");
    }
    RootTyp const* bt = it->second;
    TupleTyp const* tt = dynamic_cast<TupleTyp const*>(bt);
    if (tt == NULL) {
        THROW(SPLCompilerAssertionFailed,
              "Tuple model does not exist for non-tuple type '" << type << "'");
    }
    return getTupleModel(*tt);
}

TupleTyp const& TypeGenerator::getTupleType(TupleModel const& model) const
{
    assert(reverseTupleMap_.count(&model) > 0);
    return *reverseTupleMap_.find(&model)->second;
}

EnumTyp const& TypeGenerator::getEnumType(EnumerationModel const& model) const
{
    assert(reverseEnumMap_.count(&model) > 0);
    return *reverseEnumMap_.find(&model)->second;
}

XMLTyp const& TypeGenerator::getXMLType(XMLModel const& model) const
{
    assert(reverseXMLMap_.count(&model) > 0);
    return *reverseXMLMap_.find(&model)->second;
}

void TypeGenerator::generateCPPCode()
{
    SPLDBG("Generating types", Debug::TraceTypeGenerator);
    setPaths();

    SysOut::writeln(SPL_CORE_CREATING_TYPES, cout);

    // Generate code for all the tuples
    for (unordered_map<TupleTyp const*, TupleModel*>::const_iterator itt = tuples_.begin();
         itt != tuples_.end(); ++itt) {
        string signature = TypeMangler::getTupleSignature(
          *itt->first, itt->second->getFacade() ? TypeMangler::Facade : TypeMangler::Regular);
        codeGenHelper_.genCodeIfNeeded(*(itt->second), itt->first->getName(), signature,
                                       targetDirectory_);
    }

    // Generate code for all the enums
    for (unordered_map<EnumTyp const*, EnumerationModel*>::const_iterator ite = enums_.begin();
         ite != enums_.end(); ++ite) {
        string signature = TypeMangler::getEnumSignature(*(ite->first), TypeMangler::Regular);
        codeGenHelper_.genCodeIfNeeded(*(ite->second), ite->first->getName(), signature,
                                       targetDirectory_);
    }

    // Generate code for all the xml types
    for (unordered_map<XMLTyp const*, XMLModel*>::const_iterator itx = xmls_.begin();
         itx != xmls_.end(); ++itx) {
        string signature = TypeMangler::getXMLSignature(*(itx->first), TypeMangler::Regular);
        codeGenHelper_.genCodeIfNeeded(*(itx->second), itx->first->getName(), signature,
                                       targetDirectory_);
    }
    SPLDBG("Completed generating types", Debug::TraceTypeGenerator);
}

void TypeGenerator::setPaths()
{
    bf::path cdir = config_.getStringValue(CompilerConfiguration::OutputDir);
    targetDirectory_ = (cdir = cdir / "src" / "type").string();
    Utility::createDirectory(cdir);
}

bool TypeGenerator::isFacadeScalarType(RootTyp const& bt) const
{
    switch (bt.getMetaType()) {
        case MetaType::BOOLEAN:
        case MetaType::INT8:
        case MetaType::INT16:
        case MetaType::INT32:
        case MetaType::INT64:
        case MetaType::UINT8:
        case MetaType::UINT16:
        case MetaType::UINT32:
        case MetaType::UINT64:
        case MetaType::FLOAT32:
        case MetaType::FLOAT64:
        case MetaType::DECIMAL32:
        case MetaType::DECIMAL64:
        case MetaType::DECIMAL128:
        case MetaType::COMPLEX32:
        case MetaType::COMPLEX64:
        case MetaType::TIMESTAMP:
            return true;
        default:;
    };
    return false;
}

bool TypeGenerator::isFacade(TupleTyp const& tuple) const
{
    if (!config_.getBooleanValue(CompilerConfiguration::PreferFacades)) {
        return false;
    }

    // Run through the attributes, and see if the types are legal for a facade
    for (uint32_t i = 0, n = tuple.getNumberOfAttributes(); i < n; i++) {
        const RootTyp& bt = tuple.getAttributeType(i);

        // The following line is placed here in order to cause the g++ 4.4.5 cmpiler to optimize the
        // following switch correctly.  See rtc10438.
        MetaType::value mv = bt.getMetaType().getValue();

        if (isFacadeScalarType(bt)) {
            continue; // okay
        }
        switch (mv) {
            case MetaType::BLIST: {
                const BListTyp& blt = bt.as<BListTyp>();
                if (!isFacadeScalarType(blt.getElementType())) {
                    return false;
                }
            } break;
            case MetaType::BSET: {
                const BSetTyp& blt = bt.as<BSetTyp>();
                if (!isFacadeScalarType(blt.getElementType())) {
                    return false;
                }
            } break;
            case MetaType::BMAP: {
                const BMapTyp& blt = bt.as<BMapTyp>();
                if (!isFacadeScalarType(blt.getKeyType()) ||
                    !isFacadeScalarType(blt.getValueType())) {
                    return false;
                }
                // To eliminate padding, we need the alignments to match
                if (blt.getKeyType().getAlignment() != blt.getValueType().getAlignment()) {
                    return false;
                }
                // even if the alignments match, float alignments can change on different
                // machines.
                if (blt.getValueType().getMetaType() == MetaType::FLOAT64) {
                    return false;
                }
            } break;
            case MetaType::BSTRING:
                break;
            default:
                return false;
        }
    }
    return true;
}

bool TypeGenerator::isEqualitySupported(TupleTyp const& tuple) const
{
    // Run through the attributes, and see if we have an XML attribute
    for (uint32_t i = 0, n = tuple.getNumberOfAttributes(); i < n; i++) {
        const RootTyp& bt = tuple.getAttributeType(i);

        MetaType::value mv = bt.getMetaType().getValue();

        if (mv == MetaType::XML) {
            return false;
        } else if (mv == MetaType::TUPLE) {
            if (!isEqualitySupported(bt.as<TupleTyp>())) {
                return false;
            }
        }
    }
    return true;
}

void TypeGenerator::mangleTypes()
{
    const FrontEndDriver::TypeMap& tmap = FrontEndDriver::instance().getUsedTypes();

    FrontEndDriver::TypeMap::const_iterator it;
    for (it = tmap.begin(); it != tmap.end(); ++it) {
        RootTyp const& bt = *it->second;
        if (bt.is<TupleTyp>()) {
            TupleTyp const& tt = bt.as<TupleTyp>();
            TupleModel* tkm = new TupleModel();
            tuples_.insert(make_pair(&tt, tkm));
            reverseTupleMap_.insert(make_pair(tkm, &tt));
            bool facade = isFacade(tt);
            tkm->getFacade() = facade;
            tkm->getEqualitySupported() = isEqualitySupported(tt);
            tkm->getSPLType() = tt.getName();
            tkm->getClass() = TypeMangler::mangle(
              tt, tuples_.size(), facade ? TypeMangler::Facade : TypeMangler::Regular);
        } else if (bt.is<EnumTyp>()) {
            EnumTyp const& et = bt.as<EnumTyp>();
            EnumerationModel* em = new EnumerationModel();
            enums_.insert(make_pair(&et, em));
            reverseEnumMap_.insert(make_pair(em, &et));
            em->getClass() = TypeMangler::mangle(et, enums_.size(), TypeMangler::Regular);
        } else if (bt.is<XMLTyp>()) {
            XMLTyp const& xt = bt.as<XMLTyp>();
            XMLModel* xm = new XMLModel();
            xmls_.insert(make_pair(&xt, xm));
            reverseXMLMap_.insert(make_pair(xm, &xt));
            xm->getClass() = TypeMangler::mangle(xt, xmls_.size(), TypeMangler::Regular);
        }
    }
}

void TypeGenerator::buildModels()
{

    mangleTypes();

    const FrontEndDriver::TypeMap& tmap = FrontEndDriver::instance().getUsedTypes();
    FrontEndDriver::TypeMap::const_iterator it;
    for (it = tmap.begin(); it != tmap.end(); ++it) {
        RootTyp const& bt = *it->second;
        if (bt.is<TupleTyp>()) {
            TupleTyp const& tt = bt.as<TupleTyp>();
            buildModel(tt, *tuples_[&tt]);
        } else if (bt.is<EnumTyp>()) {
            EnumTyp const& et = bt.as<EnumTyp>();
            buildModel(et, *enums_[&et]);
        } else if (bt.is<XMLTyp>()) {
            XMLTyp const& xt = bt.as<XMLTyp>();
            buildModel(xt, *xmls_[&xt]);
        }
    }
}

void TypeGenerator::buildModel(EnumTyp const& enumer, EnumerationModel& model)
{
    assert(model.getEnumerators().size() == 0);

    // Iterate over the enumerators and populate the model
    for (uint32_t i = 0; i < enumer.getNumberOfValues(); ++i) {
        string const& val = enumer.getValidValues()[i];
        EnumeratorPtr enmr(new Enumerator());
        enmr->getName() = val;
        model.getEnumerators().push_back(enmr);
    }
}

void TypeGenerator::buildModel(XMLTyp const& xml, XMLModel& model)
{
    model.getSchema() = xml.runTimeSchema();
}

void TypeGenerator::buildModel(TupleTyp const& tuple, TupleModel& model)
{
    assert(model.getAttributes().size() == 0);

    unordered_set<string> headers;
    bool facade = isFacade(tuple);

    // Iterate over the attributes and populate the model
    for (uint32_t i = 0; i < tuple.getNumberOfAttributes(); ++i) {
        RootTyp const& type = tuple.getAttributeType(i);
        AttributePtr attr(new Attribute());

        // set the attribute's name
        attr->getName() = tuple.getAttributeName(i);

        // set the attribute's type
        attr->getCPPType() = typeName(type, headers);

        // set the attribute's SPL type
        attr->getSPLType() = type.getName();

        // set the fixed size
        if (facade) {
            attr->setFixedSize(type.getFixedSize());
        }

        model.getAttributes().push_back(attr);
    }

    // Add any headers that we accumulated while building the attributes
    unordered_set<string>::const_iterator it = headers.begin();
    for (it = headers.begin(); it != headers.end(); ++it) {
        model.getHeaders().push_back(*it);
    }
}

template<class T>
string TypeGenerator::listOrSetType(RootTyp const& type,
                                    const char* collectionName,
                                    unordered_set<string>& headers) const
{
    T const* t = dynamic_cast<T const*>(&type);
    assert(t);
    RootTyp const& bt = t->getElementType();
    string collectionOfType = typeName(bt, headers);
    return string(collectionName) + "<" + collectionOfType + " >";
}

template<class T>
string TypeGenerator::blistOrSetType(RootTyp const& type,
                                     const char* collectionName,
                                     unordered_set<string>& headers) const
{
    T const* t = dynamic_cast<T const*>(&type);
    assert(t);
    RootTyp const& bt = t->getElementType();
    ostringstream s;
    s << collectionName << '<' << typeName(bt, headers) << ", " << t->getBound() << '>';
    return s.str();
}

template<class T>
string TypeGenerator::mapType(RootTyp const& type,
                              const char* collectionName,
                              unordered_set<string>& headers) const
{
    T const* t = dynamic_cast<T const*>(&type);
    assert(t);
    RootTyp const& kt = t->getKeyType();
    RootTyp const& vt = t->getValueType();
    string keyType = typeName(kt, headers);
    string valType = typeName(vt, headers);
    return string(collectionName) + "<" + keyType + ", " + valType + " >";
}

template<class T>
string TypeGenerator::bmapType(RootTyp const& type,
                               const char* collectionName,
                               unordered_set<string>& headers) const
{
    T const* t = dynamic_cast<T const*>(&type);
    assert(t);
    RootTyp const& kt = t->getKeyType();
    RootTyp const& vt = t->getValueType();
    ostringstream s;
    s << collectionName << '<' << typeName(kt, headers) << ", " << typeName(vt, headers) << ", "
      << t->getBound() << '>';
    return s.str();
}

string TypeGenerator::typeName(RootTyp const& type, unordered_set<string>& headers) const
{
    // If the type is a primitive type then we can use the name directly
    // Otherwise we need to build a name.  What we build depends on the type.
    MetaType::value v = MetaType::value(type.getMetaType());

    switch (v) {
        case MetaType::TUPLE: {
            // If we have a tuple type then we need to find it's mangled name
            TupleTyp const* tt = dynamic_cast<TupleTyp const*>(&type);
            assert(tt);
            TupleModel const& tm = getTupleModel(*tt);
            string typeName1 = tm.getClass();
            headers.insert(typeName1);
            return "SPL::" + typeName1;
        }

        case MetaType::ENUM: {
            // If we have an enum then we need to find its mangled type name
            EnumTyp const* et = dynamic_cast<EnumTyp const*>(&type);
            assert(et);
            EnumerationModel const& em = getEnumModel(*et);
            string typeName1 = em.getClass();
            headers.insert(typeName1);
            return "SPL::" + typeName1;
        }

        case MetaType::XML: {
            XMLTyp const& xt = type.as<XMLTyp>();
            XMLModel const& xm = getXMLModel(xt);
            string typeName1 = xm.getClass();
            headers.insert(typeName1);
            return "SPL::" + typeName1;
        }

        case MetaType::LIST:
            return listOrSetType<ListTyp>(type, "SPL::list", headers);

        case MetaType::BLIST:
            return blistOrSetType<BListTyp>(type, "SPL::blist", headers);

        case MetaType::SET:
            return listOrSetType<SetTyp>(type, "SPL::set", headers);

        case MetaType::BSET:
            return blistOrSetType<BSetTyp>(type, "SPL::bset", headers);

        case MetaType::MAP:
            return mapType<MapTyp>(type, "SPL::map", headers);

        case MetaType::BMAP:
            return bmapType<BMapTyp>(type, "SPL::bmap", headers);

        case MetaType::VOID:
            return "void";

        case MetaType::NUL:
            return "null";
            break;

        case MetaType::BSTRING: {
            BStringTyp const& bst = static_cast<BStringTyp const&>(type);
            ostringstream s;
            s << "SPL::bstring<" << bst.getBoundSize() << '>';
            return s.str();
        } break;

        case MetaType::OPTIONAL: {
            ostringstream s;
            s << "SPL::optional<" << typeName(type.as<OptionalTyp>().getUnderlyingType(), headers)
              << " >";
            return s.str();
        } break;

        case MetaType::BOOLEAN:

        case MetaType::INT8:
        case MetaType::INT16:
        case MetaType::INT32:
        case MetaType::INT64:

        case MetaType::UINT8:
        case MetaType::UINT16:
        case MetaType::UINT32:
        case MetaType::UINT64:

        case MetaType::FLOAT32:
        case MetaType::FLOAT64:

        case MetaType::DECIMAL32:
        case MetaType::DECIMAL64:
        case MetaType::DECIMAL128:

        case MetaType::COMPLEX32:
        case MetaType::COMPLEX64:

        case MetaType::TIMESTAMP:
        case MetaType::RSTRING:
        case MetaType::USTRING:

        case MetaType::BLOB:
            // fall out to the return statement
            break;
        default: {
            std::cerr << "unexpected case TypeGenerator::typeName(" << v << ")" << std::endl;
            assert(!"cannot happen");
        }
    }

    return "SPL::" + type.getName();
}

string const& TypeGenerator::getCppType(const RootTyp& type) const
{
    unordered_map<RootTyp const*, std::string>::const_iterator it = cppnames_.find(&type);
    if (it == cppnames_.end()) {
        unordered_set<std::string> headers;
        std::string name = typeName(type, headers);
        return cppnames_.insert(make_pair(&type, name)).first->second;
    }
    return it->second;
}
