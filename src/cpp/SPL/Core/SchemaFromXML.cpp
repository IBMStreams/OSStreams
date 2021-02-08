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

#include <SPL/Core/SchemaFromXML.h> // Include this header first

#include <SPL/Core/CompilerError.h>
#include <SPL/Core/CompilerException.h>
#include <SPL/Core/CompilerMessage.h>
#include <SPL/Utility/ProcessLauncher.h>
#include <SPL/Utility/SysOut.h>
#include <SPL/Utility/Utility.h>
#include <UTILS/HashMapHelpers.h>
#include <UTILS/RegEx.h>
#include <UTILS/SupportFunctions.h>

#include <libxml/parser.h>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/pool/object_pool.hpp>

#include <cctype>

using namespace std;
using namespace Distillery;
namespace bf = boost::filesystem;

#include <SPL/Runtime/Utility/Singleton.t>

// Replace all occurances of string b in string a with string c
static string replaceAllOccurences(string a, const string& b, const string& c)
{
    size_t pos = a.find(b);
    while (pos != string::npos) {
        string tmp = a.substr(0, pos);
        tmp += c;
        tmp += a.substr(pos + b.length());
        a = tmp;
        pos = a.find(b);
    }
    return a;
}

namespace SPL {
template class Singleton<SchemaFromXML>;
template class Singleton<SPLSchemaFromXML::SAXHandler>;

namespace SPLSchemaFromXML {
static boost::object_pool<ElementType> elementTypePool;
static boost::object_pool<Element> elementPool;
static boost::object_pool<Document> documentPool;

void SAXHandler::endDocument()
{
    if (1 != _elementStack.size()) {
        throw "Invalid XML";
    }
}

void SAXHandler::startElement(const string& name, const vector<string>& attrs)
{
    Node& newElement = _elementStack.back()->startElement(name, attrs);
    _elementStack.push_back(&newElement);
}

void SAXHandler::characters(const char* chars, int len)
{
    if (0 == _elementStack.size()) {
        throw "Invalid XML";
    }
    _elementStack.back()->characters(chars, len);
}

void SAXHandler::endElement(const string& name)
{
    if (0 == _elementStack.size()) {
        throw "Invalid XML";
    }
    _elementStack.back()->endElement(name);
    _elementStack.pop_back();
}

bool ElementType::isEmptyElement() const
{
    return !(hasContent() || hasNestedTypes() || hasAttributes());
}

string ElementType::formattedElementName() const
{
    string formattedName = replaceAllOccurences(_elementName, ":", "_0x3a_");
    formattedName = replaceAllOccurences(formattedName, ".", "_");
    formattedName = replaceAllOccurences(formattedName, "-", "_");
    return formattedName;
}

string ElementType::formattedTypeName(bool flattenElements) const
{
    if (flattenElements && canBeFlattened()) {
        return "rstring";
    }

    // The name has the form /a/d/c:d.  Remove the leading '/' and replace all ':' and '/' with '_'
    string name = _typeName.substr(1);
    name = replaceAllOccurences(name, "/", "_");
    name = replaceAllOccurences(name, ":", "_");
    name = replaceAllOccurences(name, ".", "_");
    name = replaceAllOccurences(name, "-", "_");
    name += "_type";
    return name;
}

bool ElementType::canBeFlattened() const
{
    // Elements of this type can be flattened if it has no attributes or nested elements
    return !(hasNestedTypes() || hasAttributes());
}

bool ElementType::triggeredBy(const string& trigger) const
{
    if (_typeName.substr(0, trigger.length()) == trigger) {
        return true;
    }
    return false;
}

// Count the number of times the given element type is included in each instance of this element type
void ElementType::elementCount(unsigned int& minCount,
                               unsigned int& maxCount,
                               const ElementType& type) const
{
    minCount = maxCount = 0;
    bool first = true;
    for (size_t i = 0, sz = _instances.size(); i < sz; ++i) {
        const Element* element = _instances[i];
        unsigned int count = element->elementCount(type);
        if (first) {
            first = false;
            minCount = maxCount = count;
        } else {
            if (count > maxCount) {
                maxCount = count;
            }
            if (count < minCount) {
                minCount = count;
            }
        }
    }
}

bool ElementType::attributeIsOptional(const string& name) const
{
    // Check if the given attribute exists in each element instance
    for (size_t i = 0, sz = _instances.size(); i < sz; ++i) {
        const Element* element = _instances[i];
        if (!element->hasAttribute(name)) {
            return true;
        }
    }
    return false;
}

bool ElementType::attributesCanBeFlattened(bool diagnoseIssues) const
{
    // A given element's attributes can be flatten if there are no collisions between any of the
    // attribute names, and the names of the nested elements
    bool canFlatten = true;
    for (size_t i = 0, sz = _nestedTypes.size(); i < sz; ++i) {
        const ElementType* type = _nestedTypes[i];
        string name = type->formattedElementName();
        Attributes::const_iterator it = _attributes.find(name);
        if (it != _attributes.end()) {
            canFlatten = false;
            if (diagnoseIssues) {
                SysOut::warnln(SPL_CORE_ATTR_ELEM_COLLISION(formattedElementName(), name));
            }
        }
    }
    return canFlatten;
}

void ElementType::addInstance(const Element& instance)
{
    _instances.push_back(&instance);
    const Attributes& attributes = instance.attributes();
    for (Attributes::const_iterator it1 = attributes.begin(); it1 != attributes.end(); ++it1) {
        _attributes.insert(*it1);
    }
    if (instance.hasContent()) {
        _hasContent = true;
    }
}

bool ElementType::hasNestedType(const ElementType& type) const
{
    //ToDo: Revisit the linear search
    const string& typName = type.typeName();
    for (size_t i = 0, sz = _nestedTypes.size(); i < sz; ++i) {
        if (typName == _nestedTypes[i]->typeName()) {
            return true;
        }
    }
    return false;
}

ElementType& SAXHandler::findOrCreateElementType(const ElementType& parentType,
                                                 const string& elementName)
{
    string typeName = parentType.typeName() + "/" + elementName;
    TypeMap::iterator it = _elementTypes.find(typeName);
    if (it != _elementTypes.end()) {
        return *it->second;
    }
    ElementType& newType = *elementTypePool.construct(typeName, elementName);
    _typeList.push_back(&newType);
    _elementTypes.insert(make_pair(typeName, &newType));
    return newType;
}

string formattedAttributeName(const string& attrName)
{
    string name = replaceAllOccurences(attrName, ":", "_0x3a_");
    name = replaceAllOccurences(name, ".", "_");
    name = replaceAllOccurences(name, "-", "_");
    return name;
}

string ElementType::genTypedef(bool flattenAttributes, bool flattenElements) const
{
    string tupleType;
    bool diagnose = true;
    if (hasAttributes()) {
        if (flattenAttributes && attributesCanBeFlattened(diagnose)) {
            for (Attributes::const_iterator it = _attributes.begin(); it != _attributes.end();
                 ++it) {
                const string& attrName = *it;
                string formattedAttrName = formattedAttributeName(attrName);
                formattedAttrName =
                  SchemaFromXML::instance().checkForReservedWords(formattedAttrName);
                bool optional = attributeIsOptional(attrName);
                if (!tupleType.empty()) {
                    tupleType += ", ";
                }
                if (optional) {
                    tupleType += "list<rstring>[1] ";
                } else {
                    tupleType += "rstring ";
                }
                tupleType += formattedAttrName;
            }
        } else {
            tupleType = "map<rstring, rstring> ";
            tupleType += SchemaFromXML::instance().attrsAttributeName();
        }
    }
    if (hasContent() || isEmptyElement()) {
        if (!tupleType.empty()) {
            tupleType += ", ";
        }
        tupleType += (isEmptyElement() ? "list<rstring>[1] " : "rstring ");
        tupleType += SchemaFromXML::instance().textAttributeName();
    }
    for (size_t i = 0, sz = _nestedTypes.size(); i < sz; ++i) {
        // For each nested element type, we need to determine if we need a single element or a list
        // and if we need a list, is it bounded or not
        const ElementType& nestedType = *_nestedTypes[i];
        unsigned int minCount, maxCount;
        elementCount(minCount, maxCount, nestedType);
        string formattedTypName = nestedType.formattedTypeName(flattenElements);
        string type;
        if (minCount == 0 || maxCount > 1) {
            type = "list<" + formattedTypName + ">";
            if (maxCount == 1) {
                type += "[1]";
            }
        } else {
            type = formattedTypName;
        }
        if (!tupleType.empty()) {
            tupleType += ", ";
        }
        tupleType +=
          type + " " +
          SchemaFromXML::instance().checkForReservedWords(nestedType.formattedElementName());
    }
    string formattedType = formattedTypeName(false); // Never use flattened type name for a typedef
    return formattedType + " = tuple<" + tupleType + ">;";
}

string ElementType::genOutputClause(bool flattenAttributes, bool flattenElements) const
{
    string def;
    if (hasAttributes()) {
        if (flattenAttributes && attributesCanBeFlattened(false)) {
            for (Attributes::const_iterator it = _attributes.begin(); it != _attributes.end();
                 ++it) {
                const string& attrName = *it;
                bool optional = attributeIsOptional(attrName);
                string formattedAttributeName = attrName;
                formattedAttributeName =
                  replaceAllOccurences(formattedAttributeName, ":", "_0x3a_");
                formattedAttributeName =
                  SchemaFromXML::instance().checkForReservedWords(formattedAttributeName);
                if (!def.empty()) {
                    def += ", ";
                }
                def += formattedAttributeName + " = ";
                if (optional) {
                    def += "(list<rstring>[1])XPathList(\"@" + attrName + "\")";
                } else {
                    def += "XPath(\"@" + attrName + "\")";
                }
            }
        } else {
            def = SchemaFromXML::instance().attrsAttributeName() + " = XPathMap(\"@*\")";
        }
    }
    if (hasContent() || isEmptyElement()) {
        if (!def.empty()) {
            def += ", ";
        }
        def += SchemaFromXML::instance().textAttributeName() + " =";
        if (isEmptyElement()) {
            def += " (list<rstring>[1])";
        }
        def += " XPath";
        if (isEmptyElement()) {
            def += "List";
        }
        def += "(\"text()\")";
    }
    for (size_t i = 0, sz = _nestedTypes.size(); i < sz; ++i) {
        const ElementType& nestedType = *_nestedTypes[i];
        if (!def.empty()) {
            def += ", ";
        }
        def += SchemaFromXML::instance().checkForReservedWords(nestedType.formattedElementName()) +
               " = ";
        unsigned int minCount, maxCount;
        elementCount(minCount, maxCount, nestedType);
        bool needsList = minCount == 0 || maxCount > 1;
        bool needsCast = minCount == 0 && maxCount == 1;
        if (flattenElements && nestedType.canBeFlattened()) {
            if (needsCast) {
                def += "(list<rstring>[1])";
            }
            def += "XPath";
            if (needsList) {
                def += "List";
            }
            def += "(\"" + nestedType.elementName() + "/text()\")";
        } else {
            if (needsCast) {
                def += "(list<" + nestedType.formattedTypeName(false) + ">[1])";
            }
            def += "XPath";
            if (needsList) {
                def += "List";
            }
            string nestedDef = nestedType.genOutputClause(flattenAttributes, flattenElements);
            def += "(\"" + nestedType.elementName() + "\", {" + nestedDef + "})";
        }
    }
    return def;
}

Element::Element(const string& name, const vector<string>& attribs, const ElementType& parentType)
  : _type(SAXHandler::instance().findOrCreateElementType(parentType, name))
{
    for (size_t i = 0, size = attribs.size(); i < size; ++i) {
        _attributes.insert(attribs[i]);
    }
}

unsigned int Element::elementCount(const ElementType& type) const
{
    unsigned int count = 0;
    for (size_t i = 0, sz = _elements.size(); i < sz; ++i) {
        if (type == _elements[i]->elementType()) {
            ++count;
        }
    }
    return count;
}

Node& Element::startElement(const string& name, const vector<string>& attrs)
{
    Element& newElement = *elementPool.construct(name, attrs, _type);
    _elements.push_back(&newElement);
    // Add the ElementType to the parent type's list of nested types
    _type.addNestedType(newElement.elementType());
    return newElement;
}

void Element::endElement(const std::string& name)
{
    _type.addInstance(*this);
}

void Element::characters(const char* chars, int len)
{
    _content += string(chars, len);
}

bool Element::hasContent() const
{
    if (0 == _content.size())
        return false;
    // Content is considered any non-whitespace
    for (size_t i = 0, sz = _content.size(); i < sz; ++i) {
        char c = _content[i];
        if (!isspace(c)) {
            return true;
        }
    }
    return false;
}

bool Element::hasAttribute(const string& name) const
{
    Attributes::const_iterator it = _attributes.find(name);
    return it != _attributes.end();
}

Node& Document::startElement(const string& name, const vector<string>& attrs)
{
    assert(NULL == _root);
    _root = elementPool.construct(name, attrs, _type);
    return *_root;
}

void Document::endElement(const string& name) {}

void Document::genTypedefs(ofstream& ostr,
                           bool flattenAttributes,
                           bool flattenElements,
                           const StringList& triggers,
                           bool isSkeleton) const
{
    const TypeList& typeList = SAXHandler::instance().typeList();
    std::tr1::unordered_set<string> typesFound;
    StringList typedefs;

    // For each trigger, scan the set of types and see if they are within the trigger hierarchy
    for (size_t i = 0, sz = triggers.size(); i < sz; ++i) {
        const string& trigger = triggers[i];
        bool typedefFound = false;
        for (size_t j = 0, sz1 = typeList.size(); j < sz1; ++j) {
            const ElementType& type = *typeList[j];
            if (type.triggeredBy(trigger)) {
                typedefFound = true;
                std::tr1::unordered_set<string>::const_iterator it =
                  typesFound.find(type.typeName());
                if (it == typesFound.end()) {
                    typesFound.insert(type.typeName());
                    string tdef = type.genTypedef(flattenAttributes, flattenElements);
                    typedefs.push_back(tdef);
                }
            }
        }
        if (!typedefFound) {
            SysOut::die(SPL_CORE_NO_ELEMENTS_FOUND(trigger));
        }
    }
    ostr << "type" << endl;
    for (int i = 0, sz = typedefs.size(); i < sz; ++i) {
        ostr << "    ";
        if (isSkeleton)
            ostr << "static ";
        ostr << typedefs[i] << endl;
    }
}

void Document::emitTypeNameForTrigger(ofstream& ostr,
                                      bool flattenElements,
                                      const string& trigger) const
{
    const TypeList& typeList = SAXHandler::instance().typeList();
    bool typeFound = false;
    for (size_t j = 0, sz1 = typeList.size(); j < sz1; ++j) {
        const ElementType& type = *typeList[j];
        if (type.triggeredBy(trigger)) {
            typeFound = true;
            string typeName = type.formattedTypeName(false); // Never use flattened type name here
            ostr << typeName;
            break;
        }
    }
    if (!typeFound) {
        SysOut::die(SPL_CORE_NO_ELEMENTS_FOUND(trigger));
    }
}

void Document::genOutputClauses(ofstream& ostr,
                                bool flattenAttributes,
                                bool flattenElements,
                                const StringList& triggers,
                                bool isSkeleton) const
{
    // Generate an output clause for each trigger expression, or the root
    // Find the element type for the element we are triggering on, or the root element, if the trigger was not specified
    // If the element type has attributes, generate the XPath expression for that
    // if the element has content, generate the XPAth expression for that
    // Finally, if it has nested elements, generate the output for that
    bool first = true;
    const TypeMap& elementTypes = SAXHandler::instance().elementTypes();
    for (size_t i = 0, sz = triggers.size(); i < sz; ++i) {
        const string& trigger = triggers[i];
        if (first) {
            first = false;
            if (isSkeleton)
                ostr << "       ";
            ostr << "output ";
            if (isSkeleton) {
                ostr << "output" << i << " : ";
            } else {
                ostr << "? : ";
            }
        } else {
            ostr << endl;
            if (isSkeleton) {
                ostr << "              output" << i << " : ";
            } else {
                ostr << "       ? : ";
            }
        }
        TypeMap::const_iterator it = elementTypes.find(trigger);
        if (it == elementTypes.end()) {
            SysOut::die(SPL_CORE_NO_ELEMENTS_FOUND(trigger));
        } else {
            string def = it->second->genOutputClause(flattenAttributes, flattenElements);
            ostr << def << ";";
            if (isSkeleton) {
                ostr << " // *trigger: " << trigger;
            }
        }
    }
}

void SAXHandler::startDocument()
{
    _document = documentPool.construct();
    _elementStack.push_back(_document);
}

} // namespace SPLSchemaFromXML

using namespace SPLSchemaFromXML;

SchemaFromXML::SchemaFromXML()
  : Singleton<SchemaFromXML>(*this)
  , _outputClause(false)
  , _runFromSrcTree(false)
  , _ignoreNamespaces(false)
  , _prependUsed(false)
  , _attributesName("_attrs")
  , _textName("_text")
{}

int SchemaFromXML::init()
{
    int r = DistilleryApplication::init();

    // Compute the compiler bin directory
    bf::path exe(obtainExecutableFullPath(_argv_0));
    exe = system_complete(exe).normalize();

    // Fix the case where libtool creates wrappers
    string edir = exe.branch_path().string();
    string lib = "/.libs";
    size_t pos = edir.rfind(lib);
    if (pos == edir.size() - lib.size()) {
        edir.erase(pos);
    }
    _exeDir = edir;

    return r;
}

void SchemaFromXML::getDistilleryApplicationVisibleOptions(option_vector_t& options)
{
    // We don't want any of the default visible options
}

void SchemaFromXML::getArguments(option_vector_t& options)
{
    setKeepRemainingArguments();
    option_t args[] = {
        { 'h', "help", 0, "", SPL_CORE_OPTION_HELP, STR_OPT(SchemaFromXML::setHelp), 0, 0 },
        { 0, "attributesName", ARG, "", SPL_CORE_SFX_OPTION_ATTRNAME,
          STR_OPT(SchemaFromXML::setAttributesName), 0, 0 },
        { 'c', "outputClause", 0, "", SPL_CORE_SFX_OPTION_OUTCLAUSE,
          STR_OPT(SchemaFromXML::setOutputClause), 0, 0 },
        { 0, "composite", ARG, "", SPL_CORE_SFX_OPTION_COMPOSITE,
          STR_OPT(SchemaFromXML::setComposite), 0, 0 },
        { 'f', "flatten", ARG, "", SPL_CORE_SFX_OPTION_FLATTEN, STR_OPT(SchemaFromXML::setFlatten),
          0, 0 },
        { 0, "ignoreNamespaces", 0, "", SPL_CORE_SFX_OPTION_IGNORE_NAMEPSPACES,
          STR_OPT(SchemaFromXML::setIgnoreNamespaces), 0, 0 },
        { 0, "mainComposite", ARG, "", SPL_CORE_SFX_OPTION_MAIN_COMPOSITE,
          STR_OPT(SchemaFromXML::setMainComposite), 0, 0 },
        { 0, "namespace", ARG, "", SPL_CORE_SFX_OPTION_NAMESPACE,
          STR_OPT(SchemaFromXML::setNamespace), 0, 0 },
        { 'o', "outfile", ARG, "", SPL_CORE_SFX_OPTION_OUTFILE, STR_OPT(SchemaFromXML::setOutfile),
          0, 0 },
        { 0, "textName", ARG, "", SPL_CORE_SFX_OPTION_TEXTNAME, STR_OPT(SchemaFromXML::setTextName),
          0, 0 },
        { 't', "trigger", ARG, "", SPL_CORE_SFX_OPTION_TRIGGER, STR_OPT(SchemaFromXML::setTrigger),
          0, 0 },
        { 'x', "xsd", ARG, "", SPL_CORE_SFX_OPTION_XSDFILE, STR_OPT(SchemaFromXML::setXSDFile), 0,
          0 },
        { 'b', "build-using-source-tree", 0, "", SPL_CORE_OPTION_BUILD_USING_SOURCE_TREE,
          STR_OPT(SchemaFromXML::setSourceTreeBuild), 1, 0 }
    };
    APPEND_OPTIONS(options, args);
}

ostream& SchemaFromXML::printDescription(std::ostream& o) const
{
    o << SPL_CORE_SCHEMA_FROM_XML_USAGE;
    return o;
}

void SchemaFromXML::setSourceTreeBuild(const option_t* option, const char* value)
{
    _runFromSrcTree = true;
}

void SchemaFromXML::setComposite(const option_t* option, const char* value)
{
    _composite = value;
}

void SchemaFromXML::setMainComposite(const option_t* option, const char* value)
{
    _mainComposite = value;
}

void SchemaFromXML::setNamespace(const option_t* option, const char* value)
{
    _namespace = value;
}

void SchemaFromXML::setIgnoreNamespaces(const option_t* option, const char* value)
{
    _ignoreNamespaces = true;
}

void SchemaFromXML::setHelp(const option_t* option, const char* value)
{
    printUsage(cout, false);
    cout << endl;
    Distillery::DistAppTerminate(0);
}

void SchemaFromXML::setOutfile(const option_t* option, const char* value)
{
    _outfile = value;
}

void SchemaFromXML::setTrigger(const option_t* option, const char* value)
{
    if (0 < strlen(value)) {
        // Remove any child:: axis from the trigger expressions
        string triggers = replaceAllOccurences(value, "child::", "");
        Distillery::csvtokenize(triggers, _triggers, *value == '\"');
    }
}

void SchemaFromXML::setFlatten(const option_t* option, const char* value)
{
    _flatten = value;
}

void SchemaFromXML::setAttributesName(const option_t* option, const char* value)
{
    _attributesName = value;
}

void SchemaFromXML::setTextName(const option_t* option, const char* value)
{
    _textName = value;
}

void SchemaFromXML::setOutputClause(const option_t* option, const char* value)
{
    _outputClause = true;
}

void SchemaFromXML::setXSDFile(const option_t* option, const char* value)
{
    _xsdFile = value;
}

string SchemaFromXML::checkForReservedWords(const string& in)
{
    if (Utility::reservedName(in) || in == _attributesName || in == _textName) {
        _prependUsed = true;
        return "__" + in;
    }
    return in;
}

bool validateSPLAttributeName(const string& name)
{
    char c = name[0];
    if (!(isalpha(c) || '_' == c)) {
        return false;
    }
    for (size_t i = 1, sz = name.size(); i < sz; ++i) {
        c = name[i];
        if (!(isalnum(c) || '_' == c)) {
            return false;
        }
    }
    return true;
}

bool validateTrigger(const string& trigger)
{
    if ('/' != trigger[0]) {
        return false;
    }
    if (1 == trigger.size()) {
        return false;
    }
    if ('/' == trigger[trigger.size() - 1]) {
        return false;
    }
    Distillery::RegEx expr("(//|@|\\*|\\.\\.|::)");
    if (expr.match(trigger)) {
        return false;
    }
    return true;
}

bool validateIdentifier(const string& name)
{
    // An identifier must be alpha (digit | alpha)*
    string::size_type size = name.size();
    if (0 == size)
        return false;

    char c = name[0];
    if (!isalpha(c))
        return false;
    for (string::size_type i = 1; i < size; ++i) {
        if (!isalnum(name[i]))
            return false;
    }
    return true;
}

bool validateNamespace(const string& namespaceName)
{
    // catch the ::composite case
    if (namespaceName.empty())
        return false;

    string::size_type start = 0;
    string::size_type pos = namespaceName.find(".");
    if (0 == pos)
        return false; // can't be .id

    while (pos != string::npos) {
        string id = namespaceName.substr(start, pos - start);
        if (!validateIdentifier(id))
            return false;
        start = pos + 1;
        pos = namespaceName.find(".");
    }
    return true;
}

bool SchemaFromXML::validateOptions(const vector<string>& remains)
{
    bool optionsAreValid = true;
    if (remains.size() < 1) {
        SysOut::errorln(SPL_CORE_SFX_XML_FILE_MISSING);
        optionsAreValid = false;
    }
    if (remains.size() >= 1) {
        const bf::path file = _xmlFile = remains[0];
        // Any additional command-line arguments are silently ignored.
        if (!bf::exists(file)) {
            SysOut::errorln(SPL_CORE_XML_FILE_DOESNT_EXIST(_xmlFile));
            optionsAreValid = false;
        }
    }
    if (0 == _triggers.size()) {
        SysOut::errorln(SPL_CORE_SFX_TRIGGER_MISSING);
        optionsAreValid = false;
    }
    for (size_t i = 0, sz = _triggers.size(); i < sz; ++i) {
        const string& trigger = _triggers[i];
        if (!validateTrigger(trigger)) {
            SysOut::errorln(SPL_CORE_INVALID_TRIGGER(trigger));
            optionsAreValid = false;
        }
    }
    if (0 != _flatten.size() && _flatten != "none" && _flatten != "elements" &&
        _flatten != "attributes") {
        SysOut::errorln(SPL_CORE_ENUM_OPTION_VALUE_INVALID(
          "flatten", "\'elements\', \'attributes\'", "\'none\'"));
        optionsAreValid = false;
    }
    if (0 != _attributesName.size() && !validateSPLAttributeName(_attributesName)) {
        SysOut::errorln(SPL_CORE_INVALID_ATTRIBUTE_NAME(_attributesName, "attributesName"));
        optionsAreValid = false;
    }
    if (Utility::reservedName(_attributesName)) {
        SysOut::errorln(SPL_CORE_RESERVED_ATTR_TEXT_NAME(_attributesName, "attributesName"));
        optionsAreValid = false;
    }
    if (0 != _textName.size() && !validateSPLAttributeName(_textName)) {
        SysOut::errorln(SPL_CORE_INVALID_ATTRIBUTE_NAME(_textName, "textName"));
        optionsAreValid = false;
    }
    if (Utility::reservedName(_textName)) {
        SysOut::errorln(SPL_CORE_RESERVED_ATTR_TEXT_NAME(_textName, "textName"));
        optionsAreValid = false;
    }
    if (0 == _outfile.size()) {
        SysOut::errorln(SPL_CORE_OUTFILE_NOT_SPECIFIED);
        optionsAreValid = false;
    }
    if (!_composite.empty()) {
        if (!validateIdentifier(_composite)) {
            SysOut::errorln(SPL_CORE_SFX_OPTION_INVALID("composite", _composite));
            optionsAreValid = false;
        }
    }
    if (!_mainComposite.empty()) {
        if (_composite.empty()) {
            // option is ignored unless composite is specified
            SysOut::warnln(SPL_CORE_SFX_OPTION_IGNORED("mainComposite"));
        } else {
            if (!validateIdentifier(_mainComposite)) {
                SysOut::errorln(SPL_CORE_SFX_OPTION_INVALID("composite", _mainComposite));
                optionsAreValid = false;
            }
        }
    }
    if (!_namespace.empty()) {
        if (_composite.empty()) {
            // option is ignored unless composite is specified
            SysOut::warnln(SPL_CORE_SFX_OPTION_IGNORED("namespace"));
        } else {
            if (!validateIdentifier(_namespace)) {
                SysOut::errorln(SPL_CORE_SFX_OPTION_INVALID("namespace", _namespace));
                optionsAreValid = false;
            }
        }
    }

    return optionsAreValid;
}

static void doStartDocument(void* ctx)
{
    SAXHandler* myHandler = static_cast<SAXHandler*>(ctx);
    myHandler->startDocument();
}

static void doEndDocument(void* ctx)
{
    SAXHandler* myHandler = static_cast<SAXHandler*>(ctx);
    myHandler->endDocument();
}

static void doStartElement(void* ctx,
                           const xmlChar* localName,
                           const xmlChar* prefix,
                           const xmlChar* uri,
                           int nb_namespaces,
                           const xmlChar** namespaces,
                           int nb_attributes,
                           int nb_defaulted,
                           const xmlChar** attrs)
{
    SAXHandler* myHandler = static_cast<SAXHandler*>(ctx);
    bool ignoreNamespaces = SchemaFromXML::instance().ignoreNamespaces();

    string name;
    if (NULL != prefix && !ignoreNamespaces) {
        name += string((const char*)prefix) + ":";
    }
    name += (const char*)localName;
    vector<string> attributes;
    for (int i = 0; i < nb_attributes; ++i) {
        const char* attrName = (const char*)attrs[i * 5];
        const char* attrPrefix = (const char*)attrs[i * 5 + 1];

        // Add all the attributes (except the xmlns pseudo-attribute)
        if (NULL != attrPrefix && "xmlns" == string(attrPrefix))
            continue;
        if ("xmlns" == string(attrName))
            continue;

        string attribute;
        if (NULL != attrPrefix && !ignoreNamespaces) {
            attribute = string(attrPrefix) + ":";
        }
        attribute += attrName;
        attributes.push_back(attribute);
    }
    myHandler->startElement(name, attributes);
}

static void doEndElement(void* ctx,
                         const xmlChar* localName,
                         const xmlChar* prefix,
                         const xmlChar* uri)
{
    SAXHandler* myHandler = static_cast<SAXHandler*>(ctx);
    bool ignoreNamespaces = SchemaFromXML::instance().ignoreNamespaces();
    string name;
    if (NULL != prefix && !ignoreNamespaces) {
        name += string((const char*)prefix) + ":";
    }
    name += (const char*)localName;
    myHandler->endElement(name);
}

static void doCharacters(void* ctx, const xmlChar* chars, int len)
{
    SAXHandler* myHandler = static_cast<SAXHandler*>(ctx);
    myHandler->characters((const char*)chars, len);
}

static void doStructuredError(void* ctx, xmlErrorPtr error)
{
    cout << error->message << endl;
}

static xmlSAXHandler saxHandler = {
    0,                 /* internalSubset */
    0,                 /* isStandalone */
    0,                 /* hasInternalSubset */
    0,                 /* hasExternalSubset */
    0,                 /* resolveEntity */
    0,                 /* getEntity */
    0,                 /* entityDecl */
    0,                 /* notationDecl */
    0,                 /* attributeDecl */
    0,                 /* elementDecl */
    0,                 /* unparsedEntityDecl */
    0,                 /* setDocumentLocator */
    doStartDocument,   /* startDocument */
    doEndDocument,     /* endDocument */
    0,                 /* startElement */
    0,                 /* endElement */
    0,                 /* reference */
    doCharacters,      /* characters */
    0,                 /* ignorableWhitespace */
    0,                 /* processingInstruction */
    0,                 /* comment */
    0,                 /* warning */
    0,                 /* error */
    0,                 /* fatalError */
    0,                 /* getParameterEntity */
    0,                 /* cdataBlock */
    0,                 /* externalSubset */
    XML_SAX2_MAGIC,    /* initialized */
    0,                 /* _private */
    doStartElement,    /* startElementNs */
    doEndElement,      /* endElementNs */
    doStructuredError, /* serror */
};

int SchemaFromXML::runTrang() const
{
    // Use the java imbedded in the install to run
    bf::path java;
    const char* env_si = getenv("STREAMS_INSTALL");
    if (NULL == env_si) {
        SysOut::die(SPL_CORE_STREAMS_INSTALL_UNDEFINED);
    }
    java = bf::path(env_si) / "java";
    java /= "bin";
    java /= "java";

    // Compute the location of the trang jar file
    bf::path trang(_exeDir);
    trang /= "..";
    trang /= bf::path("ext") / "lib";

    trang /= "trang.jar";
    vector<string> args;
    args.push_back("-jar");
    args.push_back(trang.string());
    args.push_back("-I");
    args.push_back("xml");
    args.push_back("-O");
    args.push_back("xsd");
    args.push_back(_xmlFile);
    args.push_back(_xsdFile);
    int rc = EXIT_SUCCESS;
    try {
        rc = ProcessLauncher::runProcess(java.string(), args);
    } catch (SPLCompilerException& e) {
        SysOut::die(SPL_CORE_CANT_RUN_TRANG(e.getExplanation()));
    }

    return rc;
}

void SchemaFromXML::genCompositeSkeleton(const Document& document, std::ofstream& fh) const
{
    fh << "/*" << endl;
    fh << " * Parse an XML document" << endl;
    fh << " */" << endl;
    if (!_namespace.empty()) {
        fh << "namespace " << _namespace << ";" << endl << endl;
    }
    fh << "use spl.XML::*;" << endl << endl;
    int triggerCount = _triggers.size();
    fh << "composite " << _composite << "(input input0; output ";
    for (int i = 0; i < triggerCount; ++i) {
        if (i != 0) {
            fh << ", ";
        }
        fh << "output" << i;
    }
    fh << ") {" << endl;
    document.genTypedefs(fh, "attributes" == _flatten, "elements" == _flatten, _triggers, true);
    fh << "  graph" << endl;
    fh << "    ";
    if (_triggers.size() > 1) {
        fh << "(";
    }
    for (int i = 0; i < triggerCount; ++i) {
        if (i != 0)
            fh << "; ";
        fh << "stream<";
        document.emitTypeNameForTrigger(fh, "elements" == _flatten, _triggers[i]);
        fh << "> output" << i;
    }
    if (triggerCount > 1) {
        fh << ")";
    }
    fh << " = XMLParse(input0) {" << endl;
    fh << "       param trigger : ";
    for (int i = 0; i < triggerCount; ++i) {
        if (i != 0)
            fh << ", ";
        fh << "\"" << _triggers[i] << "\"";
    }
    fh << ";" << endl;
    fh << "             parsing : permissive;  // log and ignore errors" << endl;
    if (_ignoreNamespaces)
        fh << "             ignoreNamespaces : true;" << endl;
    document.genOutputClauses(fh, "attributes" == _flatten, "elements" == _flatten, _triggers,
                              true);
    fh << endl << "    }" << endl;

    fh << "}" << endl;

    if (_mainComposite.empty())
        return;

    // Now generate the main composite
    fh << endl;

    fh << "composite " << _mainComposite << "() {" << endl;
    fh << "  graph" << endl;
    fh << "    stream<rstring s> Input = FileSource() {" << endl;
    fh << "      param file : \"" << bf::path(_xmlFile).filename().string() << "\";" << endl;
    fh << "            format : line;" << endl;
    fh << "    }" << endl;
    fh << "    ";
    if (triggerCount > 1)
        fh << "(";
    for (int i = 0; i < triggerCount; ++i) {
        if (i != 0) {
            fh << "; ";
        }
        fh << "stream<" << _composite << ".";
        document.emitTypeNameForTrigger(fh, "elements" == _flatten, _triggers[i]);
        fh << "> X" << i;
    }
    if (triggerCount > 1)
        fh << ")";
    fh << " = " << _composite << "(Input) {" << endl;

    fh << "    }" << endl;

    for (int i = 0; i < triggerCount; ++i) {
        fh << "    () as O" << i << " = FileSink(X" << i << ") {" << endl;
        fh << "      param file : \"out" << i << ".dat\";" << endl;
        fh << "    }" << endl;
    }

    fh << "}" << endl;
}

void SchemaFromXML::genSparseOutput(const Document& document, std::ofstream& fh) const
{
    fh << "// " << SPL_CORE_XML_SCHEMA_TYPES(_xmlFile).key() << ": "
       << SPL_CORE_XML_SCHEMA_TYPES(_xmlFile) << endl;
    document.genTypedefs(fh, "attributes" == _flatten, "elements" == _flatten, _triggers, false);
    if (_outputClause) {
        fh << endl;
        fh << "// " << SPL_CORE_XML_SCHEMA_OUTPUT(_xmlFile).key() << ": "
           << SPL_CORE_XML_SCHEMA_OUTPUT(_xmlFile) << endl;
        document.genOutputClauses(fh, "attributes" == _flatten, "elements" == _flatten, _triggers,
                                  false);
        fh << endl;
    }
}

int SchemaFromXML::run(const vector<string>& remains)
{
    if (!validateOptions(remains)) {
        return EXIT_FAILURE;
    }

    SAXHandler myHandler;

    try {
        int rc = xmlSAXUserParseFile(&saxHandler, (void*)&myHandler, _xmlFile.c_str());
        if (-1 == rc) {
            return EXIT_FAILURE;
        }
    } catch (...) {
        SysOut::die(SPL_CORE_INVALID_XML(_xmlFile));
    }

    const bf::path outfile = _outfile;
    const bf::path parent = outfile.parent_path();
    if (!parent.empty()) {
        Utility::createDirectory(parent, true);
    }
    std::auto_ptr<ofstream> fh(new ofstream(_outfile.c_str()));
    if (!*fh) {
        if (bf::exists(outfile)) {
            Utility::removeFile(outfile, false);
            fh.reset(new ofstream(_outfile.c_str()));
        }
        // Test it again...
        if (!*fh) {
            SysOut::die(SPL_CORE_CANNOT_CREATE_FILE(_outfile, RuntimeUtility::getErrorNoStr()));
        }
    }

    const Document& document = myHandler.document();
    if (_composite.empty()) {
        genSparseOutput(document, *fh);
    } else {
        genCompositeSkeleton(document, *fh);
    }

    fh->close();

    // If any tuple attribute names had a "__" prepended, emit a diagnostic
    if (_prependUsed) {
        SysOut::writeln(SPL_CORE_PREPEND_DOUBLE_UNDERSCORE);
    }

    // Run trang if the user has specified an xsd file
    if (0 != _xsdFile.size()) {
        return runTrang();
    }
    return EXIT_SUCCESS;
}

} // namespace SPL
