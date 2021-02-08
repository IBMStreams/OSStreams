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

#ifndef SPL_SCHEMA_FROM_XML_H
#define SPL_SCHEMA_FROM_XML_H

#include <SPL/Runtime/Utility/Singleton.h>
#include <SPL/Runtime/Utility/Visibility.h>

#include <UTILS/DistilleryApplication.h>

#include <cassert>
#include <fstream>
#include <string>
#include <vector>

namespace SPL {

namespace SPLSchemaFromXML {
class Document;
}

class DLL_PUBLIC SchemaFromXML
  : public Distillery::DistilleryApplication
  , public Singleton<SchemaFromXML>
{
  public:
    SchemaFromXML();

    const std::string& attrsAttributeName() const { return _attributesName; }
    const std::string& textAttributeName() const { return _textName; }
    bool ignoreNamespaces() const { return _ignoreNamespaces; }
    std::string checkForReservedWords(const std::string& in);

  protected:
    virtual int run(const std::vector<std::string>& remains);
    virtual void getArguments(option_vector_t& options);
    virtual void getDistilleryApplicationVisibleOptions(option_vector_t& options);
    virtual int init();
    virtual std::ostream& printDescription(std::ostream& o) const;

  private:
    void setHelp(const option_t* option, const char* value);
    void setOutfile(const option_t* option, const char* value);
    void setTrigger(const option_t* option, const char* value);
    void setFlatten(const option_t* option, const char* value);
    void setAttributesName(const option_t* option, const char* value);
    void setTextName(const option_t* option, const char* value);
    void setOutputClause(const option_t* option, const char* value);
    void setSourceTreeBuild(const option_t* option, const char* value);
    void setXSDFile(const option_t* option, const char* value);
    void setIgnoreNamespaces(const option_t* option, const char* value);
    void setComposite(const option_t* option, const char* value);
    void setMainComposite(const option_t* option, const char* value);
    void setNamespace(const option_t* option, const char* value);
    bool validateOptions(const std::vector<std::string>& remains);
    int runTrang() const;
    void genSparseOutput(const SPLSchemaFromXML::Document& document, std::ofstream& fh) const;
    void genCompositeSkeleton(const SPLSchemaFromXML::Document& document, std::ofstream& fh) const;

    bool _outputClause;
    bool _runFromSrcTree;
    bool _ignoreNamespaces;
    bool _prependUsed;
    std::string _exeDir;
    std::string _outfile;
    std::vector<std::string> _triggers;
    std::string _flatten;
    std::string _attributesName;
    std::string _textName;
    std::string _xsdFile;
    std::string _xmlFile;
    std::string _composite;
    std::string _mainComposite;
    std::string _namespace;
};

namespace SPLSchemaFromXML {
class Document;
class Element;
class ElementType;
class Node;

typedef std::tr1::unordered_map<std::string, ElementType*> TypeMap;
typedef std::vector<ElementType*> TypeList;
typedef std::vector<const ElementType*> CTypeList;
typedef std::tr1::unordered_set<std::string> Attributes;
typedef std::vector<Element*> Elements;
typedef std::vector<const Element*> CElements;
typedef std::vector<std::string> StringList;

class SAXHandler : public Singleton<SAXHandler>
{
  public:
    SAXHandler()
      : Singleton<SAXHandler>(*this)
      , _document(NULL)
    {}

    void startDocument();
    void endDocument();
    void startElement(const std::string& name, const std::vector<std::string>& attrs);
    void endElement(const std::string& name);
    void characters(const char* chars, int len);
    const Document& document() const
    {
        assert(_document);
        return *_document;
    }

    ElementType& findOrCreateElementType(const ElementType& parentType,
                                         const std::string& elementName);
    const TypeMap& elementTypes() const { return _elementTypes; }
    const TypeList& typeList() const { return _typeList; }

  private:
    typedef std::vector<Node*> NodeStack;

    Document* _document;
    NodeStack _elementStack;
    TypeMap _elementTypes;
    TypeList _typeList;
};

class ElementType
{
  public:
    ElementType(const std::string& typName, const std::string& elName)
      : _typeName(typName)
      , _elementName(elName)
      , _hasContent(false)
    {}

    ElementType()
      : _typeName("")
      , _elementName("")
      , _hasContent(false)
    {}

    const std::string& elementName() const { return _elementName; }
    std::string formattedElementName() const;
    void addNestedType(const ElementType& type)
    {
        if (!hasNestedType(type))
            _nestedTypes.push_back(&type);
    }
    bool hasNestedType(const ElementType& type) const;
    void addInstance(const Element& instance);
    const std::string& typeName() const { return _typeName; }
    bool isEmptyElement() const;
    bool triggeredBy(const std::string& trigger) const;
    bool canBeFlattened() const;
    std::string formattedTypeName(bool flattenElements) const;
    void elementCount(unsigned int& minCount,
                      unsigned int& maxCount,
                      const ElementType& type) const;
    bool attributeIsOptional(const std::string& name) const;
    bool attributesCanBeFlattened(bool diagnoseIssues) const;
    std::string genTypedef(bool flattenAttributes, bool flattenElements) const;
    std::string genOutputClause(bool flattenAttributes, bool flattenElements) const;
    bool hasAttributes() const { return 0 != _attributes.size(); }
    bool hasNestedTypes() const { return 0 != _nestedTypes.size(); }
    bool hasContent() const { return _hasContent; }
    bool operator==(const ElementType& other) const { return _typeName == other._typeName; }

  private:
    std::string _typeName;
    std::string _elementName;
    bool _hasContent;
    CTypeList _nestedTypes;
    CElements _instances;
    Attributes _attributes;
};

class Node
{
  public:
    virtual ~Node() {}
    virtual Node& startElement(const std::string& name, const std::vector<std::string>& attrs) = 0;
    virtual void endElement(const std::string& name) = 0;
    virtual void characters(const char* chars, int len) = 0;

  private:
};

class Element : public Node
{
  public:
    Element(const std::string& name,
            const std::vector<std::string>& attributes,
            const ElementType& parentType);

    virtual Node& startElement(const std::string& name, const std::vector<std::string>& attrs);
    virtual void endElement(const std::string& name);
    virtual void characters(const char* chars, int len);
    const Attributes& attributes() const { return _attributes; }
    const Elements& elements() const { return _elements; }
    const ElementType& elementType() const { return _type; }
    bool hasContent() const;
    bool hasAttribute(const std::string& name) const;
    bool hasAttributes() const { return 0 != _attributes.size(); }
    unsigned int elementCount(const ElementType& type) const;

  private:
    ElementType& _type;
    std::string _content;
    Elements _elements;
    Attributes _attributes;
};

class Document : public Node
{
  public:
    Document()
      : _root(NULL)
    {}

    virtual Node& startElement(const std::string& name, const std::vector<std::string>& attrs);
    virtual void endElement(const std::string& name);
    virtual void characters(const char* chars, int len) { assert(false); }

    void genTypedefs(std::ofstream& ostr,
                     bool flattenAttributes,
                     bool flattenElements,
                     const StringList& triggers,
                     bool isSkeleton) const;
    void genOutputClauses(std::ofstream& ostr,
                          bool flattenAttributes,
                          bool flattenElements,
                          const StringList& triggers,
                          bool isSkeleton) const;
    void emitTypeNameForTrigger(std::ofstream& ostr,
                                bool flattenElements,
                                const std::string& trigger) const;

  private:
    Element* _root;
    ElementType _type;
};

} // namespace SPLSchemaFromXML
} // namespace SPL

#endif // define SPL_SCHEMA_FROM_XML_H
