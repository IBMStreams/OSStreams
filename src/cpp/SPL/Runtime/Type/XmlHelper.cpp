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

#include <SPL/Runtime/Common/RuntimeDebugAspect.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/Type/XmlHelper.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>

#include <SPL/Runtime/Utility/Singleton.t>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/validators/common/Grammar.hpp>

#include <boost/filesystem/path.hpp>
namespace bf = boost::filesystem;

#include <sys/time.h>

using namespace xercesc;
using namespace std;

namespace SPL {

template class Singleton<XMLHelper>;
XMLHelper v; // create the instance here

class PrintableString
{
  public:
    PrintableString(const XMLCh* const s) { _s = XMLString::transcode(s); }

    ~PrintableString() { XMLString::release(&_s); }

    const char* value() const { return _s; }

  private:
    char* _s;
};

inline ostream& operator<<(ostream& ostr, const PrintableString& s)
{
    ostr << s.value();
    return ostr;
}

XMLHelperImpl::XMLHelperImpl(const string& s, vector<string>& errors)
  : _schema(s)
  , _errors(&errors)
{
    XMLPlatformUtils::Initialize();
    _saxParser.reset(new SAXParser);
    _saxParser->setValidationScheme(SAXParser::Val_Never);
    _saxParser->setErrorHandler(this);
    _saxParser->setDoNamespaces(true);
    _saxParser->setLoadExternalDTD(false);
    _saxParser->setLoadSchema(false);
    if (!_schema.empty()) {
        if (_saxParser->loadGrammar(_schema.c_str(), Grammar::SchemaGrammarType, true) == NULL) {
            _errors = NULL;
            SPLTRACEMSGANDTHROW(SPLRuntimeInvalidXML, L_ERROR,
                                SPL_RUNTIME_XML_VALIDATION_UNABLE_LOAD_SCHEMA(_schema),
                                SPL_FUNC_DBG);
        }
        _saxParser->useCachedGrammarInParse(true);
        _saxParser->setDoSchema(true);
        _saxParser->setValidationScheme(SAXParser::Val_Always);
        _saxParser->setValidationSchemaFullChecking(true);
    }

    _domParser.reset(new XercesDOMParser);
    _domParser->setValidationScheme(XercesDOMParser::Val_Never);
    _domParser->setErrorHandler(this);
    _domParser->setDoNamespaces(true);
    _domParser->setLoadExternalDTD(false);
    _domParser->setLoadSchema(false);
    if (!_schema.empty()) {
        if (_domParser->loadGrammar(_schema.c_str(), Grammar::SchemaGrammarType, true) == NULL) {
            _errors = NULL;
            SPLTRACEMSGANDTHROW(SPLRuntimeInvalidXML, L_ERROR,
                                SPL_RUNTIME_XML_VALIDATION_UNABLE_LOAD_SCHEMA(_schema),
                                SPL_FUNC_DBG);
        }
        _domParser->useCachedGrammarInParse(true);
        _domParser->setDoSchema(true);
        _domParser->setValidationScheme(XercesDOMParser::Val_Always);
        _domParser->setValidationSchemaFullChecking(true);
    }
    _errors = NULL;
}

void XMLHelperImpl::error(const SAXParseException& e)
{
    assert(NULL != _errors);
    _errors->push_back(SPL_RUNTIME_XML_VALIDATION_ERROR(PrintableString(e.getSystemId()).value(),
                                                        e.getLineNumber(), e.getColumnNumber(),
                                                        PrintableString(e.getMessage()).value()));
}

void XMLHelperImpl::fatalError(const SAXParseException& e)
{
    error(e);
}

xercesc::DOMDocument* XMLHelperImpl::domDocument(const string& value, vector<string>& errors)
{
    // We won't get here if the value is empty
    assert(!value.empty());

    // Serialize access through the helper as xerces is not re-entrant
    AutoMutex mutex(_mutex);
    _errors = &errors;
    try {
        MemBufInputSource input((const XMLByte*)value.c_str(), value.size(), "<input xml>");
        _domParser->parse(input);
    } catch (const XMLException& e) {
        _errors = NULL;
        SPLTRACEMSGANDTHROW(
          SPLRuntimeInvalidXML, L_ERROR,
          SPL_RUNTIME_XML_VALIDATION_EXCEPTION(PrintableString(e.getMessage()).value()),
          SPL_FUNC_DBG);
    }
    _errors = NULL;
    return errors.empty() ? _domParser->adoptDocument() : NULL;
}

void XMLHelperImpl::validate(const string& value, vector<string>& errors)
{
    // We allow an xml object to be initialized with an empty value
    // It's considered valid unless we try and read it.
    if (value.empty()) {
        return;
    }

    // Serialize access through the helper as xerces is not re-entrant
    AutoMutex mutex(_mutex);
    _errors = &errors;
    try {
        MemBufInputSource input((const XMLByte*)value.c_str(), value.size(), "<input xml>");
        _saxParser->parse(input);
    } catch (const XMLException& e) {
        _errors = NULL;
        SPLTRACEMSGANDTHROW(
          SPLRuntimeInvalidXML, L_ERROR,
          SPL_RUNTIME_XML_VALIDATION_EXCEPTION(PrintableString(e.getMessage()).value()),
          SPL_FUNC_DBG);
    }
    _errors = NULL;
}

XMLHelper::XMLHelper()
  : Singleton<XMLHelper>(*this)
{
    XMLPlatformUtils::Initialize();
    _xqilla = new XQilla;
    _xconfig = new XercesConfiguration;
}

XMLHelper::~XMLHelper()
{
    for (UseOrderedXMLHelpers::iterator it = _helperList.begin(); it != _helperList.end(); ++it) {
        delete it->first;
    }
    delete _xconfig;
    delete _xqilla;
    /*
     * Original comment:
     * Skip the Terminate call as it may interfere with other active XML uses in the process.
     *
     * NOTE(xrg) This should be investigated further as it leaves a whole bunch of memory allocated.
     * To that end, I am reinstantiating it.
     */
    XMLPlatformUtils::Terminate();
}

XMLHelperImpl* XMLHelper::getHelper(const string& schema, vector<string>& errors)
{
    const TimeType oneDay = 60 * 60 * 24;
    struct timeval tval;
    gettimeofday(&tval, NULL);

    XMLHelperImpl* helper = NULL;
    // Ensure the cleanup/search/update is sequential
    {
        AutoMutex mutex(_mutex);

        // Use this opportunity to clean up unused helpers
        // The head of the list is the least recently used
        UseOrderedXMLHelpers::iterator it1 = _helperList.begin();
        while (it1 != _helperList.end()) {
            if (tval.tv_sec - it1->second < oneDay) { // No helper is older than one day
                break;
            }

            // We have at least one helper older than one day
            XMLHelperImpl* help = it1->first;

            // Don't remove the helper if it happens to be the one we are looking for
            if (schema == help->schema()) {
                ++it1;
                continue;
            }

            // Find the entry in the map
            HelperMap::iterator it = _helperMap.find(help->schema());
            _helperMap.erase(it);
            _helperList.erase(it1);
            delete help;
            it1 = _helperList.begin();
        }

        // See if we already have an allocator for the given schema
        HelperMap::iterator it = _helperMap.find(schema);

        if (it != _helperMap.end()) {
            helper = it->second->first;
            if (*(it->second) == _helperList.back()) {
                // If this happens to be the last element in the list, just update the time
                it->second->second = tval.tv_sec;
            } else {
                // Otherwise move it to the end
                _helperList.erase(it->second);
                it->second = _helperList.insert(_helperList.end(), make_pair(helper, tval.tv_sec));
            }
        } else {
            string fqSchema = schema;
            if (!fqSchema.empty() && fqSchema.substr(0, 5) != "http:") {
                bf::path p(fqSchema);
                if (!p.is_complete()) {
                    // Compute the fully qualified schema based on the output directory
                    const PEImpl& pe = PEImpl::instance();
                    bf::path base(pe.getOutputDirectory());
                    base /= p;
                    fqSchema = base.string();
                }
            }
            helper = new XMLHelperImpl(fqSchema, errors);
            UseOrderedXMLHelpers::iterator it2 =
              _helperList.insert(_helperList.end(), make_pair(helper, tval.tv_sec));
            _helperMap.insert(make_pair(schema, it2));
        }
    }
    assert(NULL != helper);
    return helper;
}

void XMLHelper::validate(const string& value, const string& schema, vector<string>& errors)
{
    errors.clear();
    XMLHelperImpl* helper = getHelper(schema, errors);
    helper->validate(value, errors);
}

xercesc::DOMDocument* XMLHelper::domDocument(const string& value,
                                             const string& schema,
                                             vector<string>& errors)
{
    errors.clear();
    XMLHelperImpl* helper = getHelper(schema, errors);
    return helper->domDocument(value, errors);
}
} // namespace SPL
