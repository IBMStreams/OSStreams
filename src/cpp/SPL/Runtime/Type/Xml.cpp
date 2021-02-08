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

#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Type/String.h>
#include <SPL/Runtime/Type/XmlHelper.h>
#include <SPL/Runtime/Type/XmlInternal.h>
#include <SPL/Runtime/Utility/Singleton.h>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/validators/common/Grammar.hpp>

#include <SPL/Runtime/Utility/Singleton.t>
#include <xqilla/xqilla-dom3.hpp>

using namespace xercesc;
using namespace std;

namespace SPL {

class PrintableString
{
  public:
    PrintableString(const XMLCh* const s) { _s = XMLString::transcode(s); }

    ~PrintableString() { XMLString::release(&_s); }

    const char* value() const { return _s; }

  private:
    char* _s;
};

void xml::cleanup()
{
    delete internal_;
    internal_ = NULL;
}

// Ensure that the XML is well-formed and valid for the given schema (if there is one)
void xml::validate()
{
    validate(value_, schema_);
}

// Ensure that the XML is well-formed and valid for the given schema (if there is one)
void xml::validate(const string& value, const string& schema)
{
    XMLHelper& helper = XMLHelper::instance();
    vector<string> errors;
    helper.validate(value, schema, errors);
    if (!errors.empty()) {
        // Bundle up errors and throw
        ostringstream s;
        for (uint32_t i = 0, ui = errors.size(); i < ui; i++) {
            if (i) {
                s << "; ";
            }
            s << errors[i];
        }
        THROW(SPLRuntimeInvalidXML, s.str());
    }
}

void xml::createInternal() const
{
    if (internal_) {
        return;
    }

    // Create the DOM Document for this xml
    if (value_.empty()) {
        THROW(SPLRuntimeInvalidXML, SPL_RUNTIME_UNINITIALIZED_XML);
    }

    vector<string> errors;
    XMLHelper& helper = XMLHelper::instance();
    DOMDocument* doc = helper.domDocument(value_, schema_, errors);

    // Did the parse work?
    if (!errors.empty()) {
        // Bundle up errors and throw
        ostringstream s;
        for (uint32_t i = 0, ui = errors.size(); i < ui; i++) {
            if (i) {
                s << "; ";
            }
            s << errors[i];
        }
        THROW(SPLRuntimeInvalidXML, s.str());
    }
    assert(doc);

    internal_ = new XMLInternal(*this, *doc);
}

void xml::serialize(ostream& ostr) const
{
    ostr << rstring(value_) << 'x';
}

void xml::deserialize(istream& istr)
{
    rstring value;
    istr >> value;
    operator=(value);
    int c = istr.peek();
    if (c == 'x') {
        char dummy;
        istr >> dummy;
    }
}
}
