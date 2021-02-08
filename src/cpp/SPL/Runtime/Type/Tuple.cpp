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

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Type/SPLType.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <SPL/Runtime/serializedTupleModel.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <xsd/cxx/xml/error-handler.hxx>

using namespace std;
using namespace std::tr1;
using namespace SPL;
using namespace xmlns::prod::streams::spl::tuple;

namespace bf = boost::filesystem;

unordered_map<string, Tuple::Instantiator> Tuple::instantiators_;

Tuple* Tuple::instanceOf(std::string const& name)
{
    assert(instantiators_.count(name));
    Instantiator func = instantiators_.find(name)->second;
    return (*func)();
}

void Tuple::constructPayload(const Tuple& rhs)
{
    if (__builtin_expect((rhs.payload_ == NULL), 1)) {
        payload_ = NULL;
    } else {
        payload_ = new PayloadContainer(*rhs.payload_);
    }
}

void Tuple::assignPayload(const Tuple& rhs)
{
    if (this == &rhs) {
        return;
    }

    delete payload_;
    constructPayload(rhs);
}

Tuple& Tuple::clear()
{
    for (uint32_t i = 0, n = getNumberOfAttributes(); i < n; i++) {
        TupleAttribute ta = getAttribute(i);
        ValueHandle v = ta.getValue();
        switch (v.getMetaType()) {
            case Meta::Type::LIST:
            case Meta::Type::BLIST:
                static_cast<List&>(v).removeAllElements();
                break;
            case Meta::Type::MAP:
            case Meta::Type::BMAP:
                static_cast<Map&>(v).removeAllElements();
                break;
            case Meta::Type::SET:
            case Meta::Type::BSET:
                static_cast<Set&>(v).removeAllElements();
                break;
            case Meta::Type::BLOB:
                static_cast<blob&>(v).clear();
                break;
            case Meta::Type::TUPLE:
                static_cast<Tuple&>(v).clear();
                break;
            case Meta::Type::BOOLEAN:
                static_cast<boolean&>(v) = false;
                break;
            case Meta::Type::ENUM:
                static_cast<Enum&>(v).clear();
                break;
            case Meta::Type::INT8:
                static_cast<int8&>(v) = 0;
                break;
            case Meta::Type::INT16:
                static_cast<int16&>(v) = 0;
                break;
            case Meta::Type::INT32:
                static_cast<int32&>(v) = 0;
                break;
            case Meta::Type::INT64:
                static_cast<int64&>(v) = 0;
                break;
            case Meta::Type::UINT8:
                static_cast<uint8&>(v) = 0;
                break;
            case Meta::Type::UINT16:
                static_cast<uint16&>(v) = 0;
                break;
            case Meta::Type::UINT32:
                static_cast<uint32&>(v) = 0;
                break;
            case Meta::Type::UINT64:
                static_cast<uint64&>(v) = 0;
                break;
            case Meta::Type::FLOAT32:
                static_cast<float32&>(v) = 0;
                break;
            case Meta::Type::FLOAT64:
                static_cast<float64&>(v) = 0;
                break;
            case Meta::Type::DECIMAL32:
                static_cast<decimal32&>(v) = 0;
                break;
            case Meta::Type::DECIMAL64:
                static_cast<decimal64&>(v) = 0;
                break;
            case Meta::Type::DECIMAL128:
                static_cast<decimal128&>(v) = 0;
                break;
            case Meta::Type::COMPLEX32:
                static_cast<complex32&>(v) = 0;
                break;
            case Meta::Type::COMPLEX64:
                static_cast<complex64&>(v) = 0;
                break;
            case Meta::Type::TIMESTAMP:
                static_cast<timestamp&>(v) = timestamp(0);
                break;
            case Meta::Type::RSTRING:
                static_cast<rstring&>(v).clear();
                break;
            case Meta::Type::BSTRING:
                static_cast<BString&>(v).clear();
                break;
            case Meta::Type::USTRING:
                static_cast<ustring&>(v).remove();
                break;
            default:
                assert(!"Unknown Meta Type");
        }
    }
    return *this;
}

void Tuple::assignFrom(const Tuple& otuple, bool typesafe /*=true*/)
{
    if (this == &otuple) {
        return;
    }
    if (typesafe) {
        if (typeid(*this) != typeid(otuple)) {
            SPLTRACEMSGANDTHROW(
              SPLRuntimeTypeMismatch, L_ERROR,
              SPL_RUNTIME_INVALID_TUPLE_ASSIGN(typeid(*this).name(), typeid(otuple).name()),
              SPL_FUNC_DBG);
        }
        assign(otuple);
        return;
    }
    const unordered_map<std::string, uint32_t>& oattrs = otuple.getAttributeNames();
    const unordered_map<std::string, uint32_t>& mattrs = this->getAttributeNames();
    for (unordered_map<std::string, uint32_t>::const_iterator it = mattrs.begin();
         it != mattrs.end(); ++it) {
        const std::string& aname = it->first;
        if (oattrs.count(aname) == 0) {
            continue;
        }
        ConstValueHandle ovalue = otuple.getAttributeValue(aname);
        ValueHandle mvalue = this->getAttributeValue(aname);
        if (ovalue.getMetaType() != mvalue.getMetaType() &&
            mvalue.getMetaType() != Meta::Type::OPTIONAL) {
            continue;
        }
        if (SPL::equals(ovalue.getTypeId(), mvalue.getTypeId())) {
            mvalue.assignFrom(ovalue);
        } else if (mvalue.getMetaType() == Meta::Type::TUPLE) {
            Tuple& mituple = mvalue;
            Tuple const& oituple = ovalue;
            mituple.assignFrom(oituple, typesafe);
        } else if (ovalue.getMetaType() != mvalue.getMetaType()) {
            Optional& mituple = mvalue;
            if (SPL::equals(mituple.createValue().getTypeId(), ovalue.getTypeId())) {
                mituple.setValue(ovalue);
            }
        }
    }
    assignPayload(otuple);
}

void Tuple::swapWith(Tuple& otuple, bool typesafe /*=true*/)
{
    if (this == &otuple) {
        return;
    }

    if (typesafe) {
        if (typeid(*this) != typeid(otuple)) {
            SPLTRACEMSGANDTHROW(
              SPLRuntimeTypeMismatch, L_ERROR,
              SPL_RUNTIME_INVALID_TUPLE_SWAP(typeid(*this).name(), typeid(otuple).name()),
              SPL_FUNC_DBG);
        }
    }
    const unordered_map<std::string, uint32_t>& oattrs = otuple.getAttributeNames();
    const unordered_map<std::string, uint32_t>& mattrs = this->getAttributeNames();
    for (unordered_map<std::string, uint32_t>::const_iterator it = mattrs.begin();
         it != mattrs.end(); ++it) {
        const std::string& aname = it->first;
        if (oattrs.count(aname) == 0) {
            continue;
        }
        ValueHandle ovalue = otuple.getAttributeValue(aname);
        ValueHandle mvalue = this->getAttributeValue(aname);
        if (ovalue.getMetaType() != mvalue.getMetaType()) {
            continue;
        }
        if (SPL::equals(ovalue.getTypeId(), mvalue.getTypeId())) {
            mvalue.swapWith(ovalue);
        } else if (mvalue.getMetaType() == Meta::Type::TUPLE) {
            Tuple& mituple = mvalue;
            Tuple& oituple = ovalue;
            mituple.swapWith(oituple);
        }
    }
    std::swap(payload_, otuple.payload_);
}

void Tuple::normalizeBoundedSetsAndMaps() {}

void Tuple::setPayloadContainer(PayloadContainer* payload)
{
    if (payload == payload_) {
        return;
    }
    delete payload_;
    payload_ = payload;
}

void Tuple::invalidIndex(uint32_t index, uint32_t maxAttr)
{
    SPLTRACEMSGANDTHROW(SPLRuntimeInvalidIndex, L_ERROR,
                        SPL_RUNTIME_TUPLE_INVALID_INDEX(index, maxAttr - 1), SPL_FUNC_DBG);
}

TupleMappings::NameToIndex::iterator Tuple::lookupAttributeName(TupleMappings& map,
                                                                const std::string& attr)
{
    TupleMappings::NameToIndex::iterator it = map.nameToIndex_.find(attr);
    if (it == map.nameToIndex_.end()) {
        SPLTRACEMSGANDTHROW(SPLRuntimeInvalidIndex, L_ERROR, SPL_RUNTIME_TUPLE_INVALID_ATTR(attr),
                            SPL_FUNC_DBG);
    }
    return it;
}

static void indent(std::ostream& ostr, uint32_t depth)
{
    ostr << string(2 * depth, ' ');
}

static void toXML(std::ostream& ostr, const Tuple& t, uint32_t depth);

// Ensure this string value is legal XML
static void toXML(std::ostream& ostr, const string& s)
{
    for (string::const_iterator it = s.begin(); it != s.end(); it++) {
        switch (*it) {
            case '&':
                ostr << "&amp;";
                break;
            case '<':
                ostr << "&lt;";
                break;
            default:
                ostr << *it;
                break;
        }
    }
}

static bool isScalar(Meta::Type::Value t)
{
    switch (t) {
        case Meta::Type::LIST:
        case Meta::Type::BLIST:
        case Meta::Type::SET:
        case Meta::Type::BSET:
        case Meta::Type::MAP:
        case Meta::Type::BMAP:
        case Meta::Type::TUPLE:
            return false;
        default:;
    }
    return true;
}

// Generate one scalar value
static void scalarToXML(ostream& ostr, ConstValueHandle& v)
{
    const Meta::Type& t = v.getMetaType();
    switch (t.getValue()) {
        case Meta::Type::RSTRING: {
            const rstring& s = (const rstring&)v;
            toXML(ostr, makeRStringLiteral(s, true, false));
        } break;
        case Meta::Type::BSTRING: {
            const BString& s = (const BString&)v;
            toXML(ostr, makeRStringLiteral(s.getCString(), true, false));
        } break;
        case Meta::Type::USTRING: {
            const ustring& s = (const ustring&)v;
            toXML(ostr, makeUStringLiteral(s, true, false));
        } break;
        case Meta::Type::XML: {
            const xml& x = (const xml&)v;
            toXML(ostr, makeRStringLiteral(x.getValue().c_str(), true, false));
        } break;
        default:
            v.serialize(ostr);
            break;
    }
}

static void valueToXML(ostream& ostr,
                       const char* elementName,
                       ConstValueHandle& v,
                       bool scalar,
                       uint32_t depth);

static void toXML(ostream& ostr, const List& l, uint32_t depth)
{
    const Meta::Type& e = l.getElementMetaType();
    indent(ostr, depth);
    bool scalar = isScalar(e.getValue());
    if (scalar) {
        ostr << "<list elementType=\"";
        toXML(ostr, e.toString());
        ostr << "\">\n";
    } else {
        ostr << "<list>";
    }
    for (uint32_t i = 0, ui = l.getSize(); i < ui; i++) {
        ConstValueHandle ev = l.getElement(i);
        valueToXML(ostr, "value", ev, scalar, depth + 1);
    }
    indent(ostr, depth);
    ostr << "</list>\n";
}

static void toXML(ostream& ostr, const Set& s, uint32_t depth)
{
    const Meta::Type& e = s.getElementMetaType();
    indent(ostr, depth);
    bool scalar = isScalar(e.getValue());
    if (scalar) {
        ostr << "<set elementType=\"";
        toXML(ostr, e.toString());
        ostr << "\">\n";
    } else {
        ostr << "<set>";
    }
    for (ConstSetIterator it = s.getBeginIterator(); it != s.getEndIterator(); it++) {
        ConstValueHandle ev = *it;
        valueToXML(ostr, "value", ev, scalar, depth + 1);
    }
    indent(ostr, depth);
    ostr << "</set>\n";
}

static void toXML(ostream& ostr, const Map& m, uint32_t depth)
{
    const Meta::Type& k = m.getKeyMetaType();
    const Meta::Type& v = m.getValueMetaType();
    indent(ostr, depth);
    bool kScalar = isScalar(k.getValue());
    bool vScalar = isScalar(v.getValue());
    ostr << "<map";
    if (kScalar) {
        ostr << " keyType=\"";
        toXML(ostr, k.toString());
        ostr << '"';
    }
    if (vScalar) {
        ostr << " valueType=\"";
        toXML(ostr, v.toString());
        ostr << '"';
    }
    ostr << ">\n";
    for (ConstMapIterator it = m.getBeginIterator(); it != m.getEndIterator(); it++) {
        pair<ConstValueHandle, ConstValueHandle> val = *it;
        valueToXML(ostr, "key", val.first, kScalar, depth + 1);
        valueToXML(ostr, "value", val.second, vScalar, depth + 1);
    }
    indent(ostr, depth);
    ostr << "</map>\n";
}

static void toXML(ostream& ostr, const string& n, ConstValueHandle& v, uint32_t depth)
{
    indent(ostr, depth);
    ostr << "<attr name=\"" << n << '"';
    const Meta::Type& t = v.getMetaType();
    bool scalar = isScalar(t.getValue());
    if (scalar) {
        // All scalar types
        ostr << " type=\"";
        toXML(ostr, t.toString());
        ostr << "\">";
        scalarToXML(ostr, v);
        ostr << "</attr>\n";
        return;
    }
    ostr << ">\n";
    switch (t.getValue()) {
        case Meta::Type::LIST:
        case Meta::Type::BLIST: {
            const List& l = (const List&)v;
            toXML(ostr, l, depth + 1);
        } break;
        case Meta::Type::SET:
        case Meta::Type::BSET: {
            const Set& s = (const Set&)v;
            toXML(ostr, s, depth + 1);
        } break;
        case Meta::Type::MAP:
        case Meta::Type::BMAP: {
            const Map& m = (const Map&)v;
            toXML(ostr, m, depth + 1);
        } break;
        case Meta::Type::TUPLE: {
            const Tuple& tup = (const Tuple&)v;
            toXML(ostr, tup, depth + 1);
        } break;
        default:;
    }
    indent(ostr, depth);
    ostr << "</attr>\n";
}

static void toXML(ostream& ostr, const Tuple& t, uint32_t depth)
{
    indent(ostr, depth);
    if (!depth) {
        ostr << "<tuple xmlns=\"http://www.ibm.com/xmlns/prod/streams/spl/tuple\">\n";
    } else {
        ostr << "<tuple>\n";
    }
    for (uint32_t i = 0, ui = t.getNumberOfAttributes(); i < ui; i++) {
        const string& n = t.getAttributeName(i);
        ConstValueHandle v = t.getAttributeValue(i);
        toXML(ostr, n, v, depth + 1);
    }
    indent(ostr, depth);
    ostr << "</tuple>\n";
}

static void valueToXML(ostream& ostr,
                       const char* elementName,
                       ConstValueHandle& v,
                       bool scalar,
                       uint32_t depth)
{
    indent(ostr, depth);
    ostr << '<' << elementName << '>';
    if (scalar) {
        scalarToXML(ostr, v);
        ostr << "</" << elementName << ">\n";
        return;
    }
    ostr << '\n';
    const Meta::Type& t = v.getMetaType();
    switch (t.getValue()) {
        case Meta::Type::LIST:
        case Meta::Type::BLIST: {
            const List& l = (const List&)v;
            toXML(ostr, l, depth + 1);
        } break;
        case Meta::Type::SET:
        case Meta::Type::BSET: {
            const Set& s = (const Set&)v;
            toXML(ostr, s, depth + 1);
        } break;
        case Meta::Type::MAP:
        case Meta::Type::BMAP: {
            const Map& m = (const Map&)v;
            toXML(ostr, m, depth + 1);
        } break;
        case Meta::Type::TUPLE: {
            const Tuple& tup = (const Tuple&)v;
            toXML(ostr, tup, depth + 1);
        } break;
        default:
            break;
    }
    indent(ostr, depth);
    ostr << "</" << elementName << ">\n";
}

void Tuple::toXML(ostream& ostr) const
{
    ::toXML(ostr, *this, 0);
}

/// Class that handles printing of XML parsing diagnostics
class RuntimeXMLDiagPrinter : public ::xsd::cxx::xml::error_handler<char>
{
  public:
    RuntimeXMLDiagPrinter() {}

    bool handle(const std::string& id,
                unsigned long line,
                unsigned long column,
                severity sev,
                const std::string& message)
    {
        SPLTRACEMSG(sev == severity::warning ? L_INFO : L_ERROR,
                    SPL_RUNTIME_XML2TUPLE_MESSAGE(message), SPL_OPER_DBG);
        return true; // try to continue
    }
};

#define UTF8(X) XMLString::transcode(X)

using namespace xercesc;

static void dumpNode(const DOMNode& node, const string& indent)
{
    cout << indent << "Node name: " << UTF8(node.getNodeName()) << endl;
    if (node.getNodeValue()) {
        cout << indent << "Node value: " << UTF8(node.getNodeValue()) << endl;
    }
    cout << indent << "Node type: " << node.getNodeType() << endl;
    DOMNode* c = node.getFirstChild();
    if (node.getFirstChild()) {
        cout << indent << "Node has children\n";
        while (c) {
            dumpNode(*c, indent + "    ");
            c = c->getNextSibling();
        }
    }
}

static void scalarFromXML(const string& typeName, const DOMNode& node, ValueHandle& v)
{
    const XMLCh* textString = NULL;
    for (DOMNode* c = node.getFirstChild(); c; c = c->getNextSibling()) {
        int t = c->getNodeType();
        if (t == DOMNode::TEXT_NODE || t == DOMNode::CDATA_SECTION_NODE) {
            textString = c->getNodeValue();
            break; // just take the first one
        }
    }

    if (!textString) {
        return; // may be empty/missing as in empty blob.   Just use the 'clear'ed value.
    }

    char* utf8 = NULL;
    try {
        // Special case ustrings
        if (v.getMetaType() == Meta::Type::USTRING) {
            SPL::ustring& u = (SPL::ustring&)v;
            u = ustring(textString);
            return;
        }

        utf8 = UTF8(textString);
        istringstream str(utf8);
        if (v.getMetaType() == Meta::Type::XML) {
            // Need to handle this specially too, as we only have the value, not the schema
            rstring r;
            str >> r;
            SPL::xml& x = (SPL::xml&)v;
            x = r;
        } else {
            v.deserialize(str);
        }
    } catch (...) {
        if (!utf8) {
            utf8 = UTF8(textString);
        }
        SPLTRACEMSG(L_ERROR, SPL_RUNTIME_XML2TUPLE_SCALAR(utf8, typeName), SPL_OPER_DBG);
        XMLString::release(&utf8);
        throw;
    }
    XMLString::release(&utf8);
}

static void fromXML(const attributeType& attr, ValueHandle& v);
static void valueFromXML(const string& typeName, const valueType& value, ValueHandle& v);

static void fromXML(const tupleType& xml, Tuple& t)
{
    const tupleType::attr_sequence& attrs = xml.attr();
    tupleType::attr_const_iterator it = attrs.begin();
    for (uint32_t i = 0, ui = t.getNumberOfAttributes(); i < ui; i++, ++it) {
        const string& n = t.getAttributeName(i);
        if (it == attrs.end() || n != it->name()) {
            SPLTRACEMSGANDTHROW(SPLRuntimeXMLParsing, L_ERROR,
                                SPL_RUNTIME_XML2TUPLE_MISSING_ATTR_XML(n), SPL_FUNC_DBG);
        }

        ValueHandle v = t.getAttributeValue(i);
        fromXML(*it, v);
    }
}

static void fromXML(const listOrSetType& list, ValueHandle& v, bool isList)
{
    const listOrSetType::value_sequence& values = list.value();
    listOrSetType::value_const_iterator it = values.begin();
    string typeName = list.elementType().present() ? list.elementType().get() : "";
    if (isList) {
        List& l = v;
        for (; it != values.end(); it++) {
            ValueHandle elem = l.createElement();
            valueFromXML(typeName, *it, elem);
            l.pushBack(elem);
            elem.deleteValue();
        }
        return;
    }

    Set& s = v;
    for (; it != values.end(); it++) {
        ValueHandle elem = s.createElement();
        valueFromXML(typeName, *it, elem);
        s.insertElement(elem);
        elem.deleteValue();
    }
}

static void fromXML(const mapType& map, ValueHandle& v)
{
    Map& m = v;
    const mapType::value_sequence& values = map.value();
    mapType::value_const_iterator vit = values.begin();
    const mapType::key_sequence& keys = map.key();
    mapType::key_const_iterator kit = keys.begin();
    string keyTypeName = map.keyType().present() ? map.keyType().get() : "";
    string valueTypeName = map.valueType().present() ? map.valueType().get() : "";
    for (; vit != values.end(); vit++, kit++) {
        ValueHandle velem = m.createValue();
        valueFromXML(valueTypeName, *vit, velem);
        ValueHandle kelem = m.createKey();
        valueFromXML(keyTypeName, *kit, kelem);
        m.insertElement(kelem, velem);
        kelem.deleteValue();
        velem.deleteValue();
    }
}

static void valueFromXML(const string& typeName, const valueType& value, ValueHandle& v)
{
    const Meta::Type& t = v.getMetaType();
    bool scalar = isScalar(t.getValue());
    const DOMNode* node = value._node();
    assert(node);

    if (scalar) {
        // All scalar types
        scalarFromXML(typeName, *node, v);
        return;
    }

    switch (t.getValue()) {
        case Meta::Type::LIST:
        case Meta::Type::BLIST: {
            if (value.list().present()) {
                fromXML(value.list().get(), v, true);
            } else {
                SPLTRACEMSGANDTHROW(SPLRuntimeXMLParsing, L_ERROR,
                                    SPL_RUNTIME_XML2TUPLE_MISSING_VALUE_XML("list"), SPL_FUNC_DBG);
            }
        } break;
        case Meta::Type::SET:
        case Meta::Type::BSET: {
            if (value.set().present()) {
                fromXML(value.set().get(), v, false);
            } else {
                SPLTRACEMSGANDTHROW(SPLRuntimeXMLParsing, L_ERROR,
                                    SPL_RUNTIME_XML2TUPLE_MISSING_VALUE_XML("set"), SPL_FUNC_DBG);
            }
        } break;
        case Meta::Type::MAP:
        case Meta::Type::BMAP: {
            if (value.map().present()) {
                fromXML(value.map().get(), v);
            } else {
                SPLTRACEMSGANDTHROW(SPLRuntimeXMLParsing, L_ERROR,
                                    SPL_RUNTIME_XML2TUPLE_MISSING_VALUE_XML("map"), SPL_FUNC_DBG);
            }
        } break;
        case Meta::Type::TUPLE: {
            if (value.tuple().present()) {
                fromXML(value.tuple().get(), v);
            } else {
                SPLTRACEMSGANDTHROW(SPLRuntimeXMLParsing, L_ERROR,
                                    SPL_RUNTIME_XML2TUPLE_MISSING_VALUE_XML("tuple"), SPL_FUNC_DBG);
            }
        } break;
        default:;
    }
}

static void fromXML(const attributeType& attr, ValueHandle& v)
{
    const Meta::Type& t = v.getMetaType();
    bool scalar = isScalar(t.getValue());
    const DOMNode* node = attr._node();
    assert(node);

    if (scalar) {
        // All scalar types
        string typeName = attr.type().present() ? attr.type().get() : "";
        scalarFromXML(typeName, *node, v);
        return;
    }

    switch (t.getValue()) {
        case Meta::Type::LIST:
        case Meta::Type::BLIST: {
            if (attr.list().present()) {
                fromXML(attr.list().get(), v, true);
            } else {
                SPLTRACEMSGANDTHROW(SPLRuntimeXMLParsing, L_ERROR,
                                    SPL_RUNTIME_XML2TUPLE_INCORRECT_ATTR_XML(attr.name(), "list"),
                                    SPL_FUNC_DBG);
            }
        } break;
        case Meta::Type::SET:
        case Meta::Type::BSET: {
            if (attr.set().present()) {
                fromXML(attr.set().get(), v, false);
            } else {
                SPLTRACEMSGANDTHROW(SPLRuntimeXMLParsing, L_ERROR,
                                    SPL_RUNTIME_XML2TUPLE_INCORRECT_ATTR_XML(attr.name(), "set"),
                                    SPL_FUNC_DBG);
            }
        } break;
        case Meta::Type::MAP:
        case Meta::Type::BMAP: {
            if (attr.map().present()) {
                fromXML(attr.map().get(), v);
            } else {
                SPLTRACEMSGANDTHROW(SPLRuntimeXMLParsing, L_ERROR,
                                    SPL_RUNTIME_XML2TUPLE_INCORRECT_ATTR_XML(attr.name(), "map"),
                                    SPL_FUNC_DBG);
            }
        } break;
        case Meta::Type::TUPLE: {
            if (attr.tuple().present()) {
                fromXML(attr.tuple().get(), v);
            } else {
                SPLTRACEMSGANDTHROW(SPLRuntimeXMLParsing, L_ERROR,
                                    SPL_RUNTIME_XML2TUPLE_INCORRECT_ATTR_XML(attr.name(), "tuple"),
                                    SPL_FUNC_DBG);
            }
        } break;
        default:;
    }
}

void Tuple::fromXML(istream& istr)
{
    auto_ptr<tupleType> tupleModel;
    try {
        RuntimeXMLDiagPrinter errorPrinter;
        ::xml_schema::properties props;
        char* i = getenv("STREAMS_INSTALL");
        ::xml_schema::flags f = ::xsd::cxx::tree::flags::keep_dom;
        if (i) {
            bf::path schema(string(i) + "/etc/xsd/SPL/serializedTupleModel.xsd");
            if (bf::exists(schema)) {
                props.schema_location("http://www.ibm.com/xmlns/prod/streams/spl/tuple",
                                      schema.string());
            } else {
                f = f | ::xsd::cxx::tree::flags::dont_validate;
            }
        } else {
            f = f | ::xsd::cxx::tree::flags::dont_validate;
        }
        tupleModel = ::xmlns::prod::streams::spl::tuple::tuple(istr, errorPrinter, f, props);
    } catch (::xsd::cxx::exception& e) {
        SPLTRACEMSGANDTHROW(SPLRuntimeXMLParsing, L_ERROR, SPL_RUNTIME_XML2TUPLE_MESSAGE(e.what()),
                            SPL_FUNC_DBG);
    } catch (...) {
        SPLTRACEMSGANDTHROW(SPLRuntimeXMLParsing, L_ERROR, SPL_RUNTIME_XML2TUPLE_UNKNOWN_ERROR,
                            SPL_FUNC_DBG);
    }

    // Okay, we have read in the XML.  Now lets set our values from it
    clear();

    ::fromXML(*tupleModel, *this);
}
