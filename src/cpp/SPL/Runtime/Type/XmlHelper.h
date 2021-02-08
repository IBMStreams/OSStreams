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

#ifndef SPL_RUNTIME_TYPE_XMLVALIDATOR_H
#define SPL_RUNTIME_TYPE_XMLVALIDATOR_H

#include <SPL/Runtime/Utility/Mutex.h>
#include <SPL/Runtime/Utility/Singleton.h>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>

#if defined(__clang__) ||                                                                          \
  ((__GNUC__) && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ > 5))))
#pragma GCC diagnostic push
#endif

#pragma GCC diagnostic ignored "-Wextra"
#include <xqilla/xqilla-simple.hpp>

#if defined(__clang__) ||                                                                          \
  ((__GNUC__) && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ > 5))))
#pragma GCC diagnostic pop
#endif

#include <string>
#include <tr1/unordered_map>
#include <vector>
#ifndef DOXYGEN_SKIP_FOR_USERS

namespace SPL {

/// Represents an instance of an xml helper for xml of a given schema
class XMLHelperImpl : public xercesc::HandlerBase
{
  public:
    XMLHelperImpl(const std::string& s, std::vector<std::string>& errors);
    void validate(const std::string& value, std::vector<std::string>& errors);
    xercesc::DOMDocument* domDocument(const std::string& value, std::vector<std::string>& errors);
    const std::string& schema() const { return _schema; }

  private:
    void error(const xercesc::SAXParseException& e);
    void fatalError(const xercesc::SAXParseException& e);
    std::string _schema;
    Mutex _mutex;
    std::auto_ptr<xercesc::SAXParser> _saxParser;
    std::auto_ptr<xercesc::XercesDOMParser> _domParser;
    std::vector<std::string>* _errors;
};

/// Main entity used to validate xml, optionally against a schema
/// Without a schema the xml is validated only for well-formedness
class XMLHelper : public Singleton<XMLHelper>
{
  public:
    XMLHelper();
    ~XMLHelper();

    /// Validate xml
    /// @param schema The schema against which to validate.  An empty string validates only form
    /// @param value The xml to validate
    /// @param errors A list of errors.  The xml is valid if empty upon return.
    void validate(const std::string& schema,
                  const std::string& value,
                  std::vector<std::string>& errors);

    /// Return the DOM tree for the XML
    /// @param schema The schema for the XML value
    /// @param value The xml to validate
    /// @param errors A list of errors.  The xml is valid if empty upon return.
    /// @return the DOM Document tree for the XML, or NULL if there were errors
    xercesc::DOMDocument* domDocument(const std::string& schema,
                                      const std::string& value,
                                      std::vector<std::string>& errors);

    /// Access a singleton instance of xquila
    /// @return Returns the singleton instance
    XQilla& xqilla() { return *_xqilla; }

    /// Access a singleton instance of a XercesConfiguration
    /// @return Returns the singleton instance
    XercesConfiguration& xconfig() { return *_xconfig; }

  private:
    typedef uint64_t TimeType;
    typedef std::pair<XMLHelperImpl*, TimeType> TimestampedXMLHelper;
    typedef std::list<TimestampedXMLHelper> UseOrderedXMLHelpers;
    typedef std::tr1::unordered_map<std::string, UseOrderedXMLHelpers::iterator> HelperMap;
    XMLHelperImpl* getHelper(const std::string& schema, std::vector<std::string>& errors);

    UseOrderedXMLHelpers _helperList;
    HelperMap _helperMap;
    Mutex _mutex;
    XQilla* _xqilla;
    XercesConfiguration* _xconfig;
};

} // namespace SPL
#endif // DOXYGEN_SKIP_FOR_USERS
#endif // SPL_RUNTIME_TYPE_XMLVALIDATOR_H
