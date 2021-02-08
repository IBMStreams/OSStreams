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

#ifndef SPL_RUNTIME_TYPE_XML_INTERNAL_H
#define SPL_RUNTIME_TYPE_XML_INTERNAL_H

#include <SPL/Runtime/Type/Xml.h>
#include <SPL/Runtime/Utility/Mutex.h>
#include <xercesc/dom/DOM.hpp>

namespace SPL {
class XMLInternal
{
  public:
    /// Constructor
    /// @param x XML object to cache
    ///
    XMLInternal(const xml& x, xercesc::DOMDocument& doc)
      : _document(doc)
    {}

    xercesc::DOMDocument& getDocument() const { return _document; }

    /// Destructor
    ///
    ~XMLInternal() { _document.release(); }

  private:
    xercesc::DOMDocument& _document;
    Mutex _mutex;
};
}
#endif // SPL_RUNTIME_TYPE_XML_INTERNAL_H
