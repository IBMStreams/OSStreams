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

#ifndef XML_UTILS_INCLUDED
#define XML_UTILS_INCLUDED

#include <cstdio>
#include <string>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

#include "xercesc/dom/DOM.hpp"
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNode.hpp>

#include <SAM/SAMNamespaceDefs.h>

class XStr
{
  public:
    XStr(const char* const toTranscode)
    {
        fUnicodeForm = xercesc::XMLString::transcode(toTranscode);
    }
    ~XStr() { xercesc::XMLString::release(&fUnicodeForm); }
    const XMLCh* unicodeForm() const { return fUnicodeForm; }

  private:
    XMLCh* fUnicodeForm;
};

//#define Xx(str) XStr(str).unicodeForm()

SPC_NAMESPACE_BEGIN

class XML_utils
{
  public:
    //--------------------------------------------------------------------//
    //  Constructor
    //--------------------------------------------------------------------//
    XML_utils() {}

    //--------------------------------------------------------------------//
    //  Destructor
    //--------------------------------------------------------------------//
    ~XML_utils() {}

    //--------------------------------------------------------------------//
    //  Utility functions
    //--------------------------------------------------------------------//
    static std::string getTextValue(xercesc::DOMNode* pCurrentNode);
    static std::string getRawTextValue(xercesc::DOMNode* pCurrentNode);
    static std::string getAttribValue(xercesc::DOMNode* pCurrentNode, const char* attribName);
    static void setAttribValue(xercesc::DOMDocument* doc,
                               xercesc::DOMNode* pCurrent,
                               const std::string& attribName,
                               const std::string& attribValue);
    static void freeMemory(void* ptr);
    static xercesc::DOMNodeList* findNodesByTagName(const char* tagName);
    static xercesc::DOMNode* findNodesByTagValue(const char* tagName, const char* value);
    static xercesc::DOMNode* getMatchingNode(xercesc::DOMDocument* pNode,
                                             const char* tagName,
                                             const char* attribName,
                                             const char* attribValue);
    static int addChildTextTag(xercesc::DOMDocument* pDoc,
                               const char* tagName,
                               const char* attribName,
                               const char* attribValue,
                               const char* childTag,
                               const char* childValue);
    static void trimWhiteSpace(std::string& str);
    static std::string XMLCh2string(const XMLCh* chars);
    static std::string escape(const char* str);
};

SPC_NAMESPACE_END

#endif
