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

#include <UTILS/SPCTypes.h>
#include <UTILS/XML_utils.h>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/util/IOException.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

using namespace std;
using namespace xercesc;
SPC_NAMESPACE_USE

#define XML_UNICODE_FORM(str) XStr(str).unicodeForm()

//----------------------------------------------------------------------
// get the value of a text node
//----------------------------------------------------------------------
string XML_utils::getTextValue(DOMNode* pCurrent)
{
    char* strValue = NULL;
    strValue = XMLString::transcode(pCurrent->getTextContent());
    string s;
    if (strValue) {
        s.assign(strValue);
        XMLString::release(&strValue);
    }
    return s;
}

//----------------------------------------------------------------------
// get the XML value of a node
//----------------------------------------------------------------------
string XML_utils::getRawTextValue(DOMNode* pCurrent)
{
    string toret;
    try {
        // outputting the updated JDL
        static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
        DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(gLS);

        // construct the DOMLSSerializer
        DOMLSSerializer* mySerializer =
          (static_cast<DOMImplementationLS*>(impl))->createLSSerializer();
        assert(mySerializer);

        // getting the updated jdl
        char* str = XMLString::transcode(mySerializer->writeToString(pCurrent));
        toret = str;

        // release the memory
        mySerializer->release();
        XMLString::release(&str);
    } catch (const XERCES_CPP_NAMESPACE_QUALIFIER OutOfMemoryException& ex) {
        stringstream msg;
        msg << __FUNCTION__ << ": An error occurred during parsing/rewriting "
            << ": " << ex.getType() << " (" << ex.getMessage() << ")";
        THROW(ParseError, msg.str());
    } catch (const XERCES_CPP_NAMESPACE_QUALIFIER IOException& ex) {
        stringstream msg;
        msg << __FUNCTION__ << ": An error occurred during parsing/rewriting "
            << ": " << ex.getType() << " (" << ex.getMessage() << ")";
        THROW(ParseError, msg.str());
    } catch (const XMLException& ex) {
        stringstream msg;
        msg << __FUNCTION__ << ": An error occurred during parsing/rewriting "
            << ": " << ex.getType() << " (" << ex.getMessage() << ")";
        THROW(ParseError, msg.str());
    } catch (const DOMException& ex) {
        stringstream msg;
        msg << __FUNCTION__ << ": An error occurred during parsing/rewriting "
            << ": code=" << ex.code;
        unsigned maxLen = 1023;
        XMLCh errText[maxLen + 1];
        if (DOMImplementation::loadDOMExceptionMsg(ex.code, errText, maxLen)) {
            msg << " DOMImplementation::loadDOMExceptionMsg failed";
        } else {
            msg << " " << errText;
        }
        THROW(ParseError, msg.str());
    } catch (...) {
        stringstream msg;
        msg << __FUNCTION__ << ": An error of unknown nature happened during parsing/rewriting ";
        THROW(ParseError, msg.str());
    }
    return toret;
}

//----------------------------------------------------------------------
// get the value of an attribute name
//----------------------------------------------------------------------
string XML_utils::getAttribValue(DOMNode* pCurrent, const char* attribName)
{
    string str;
    // Get the attributes, if any, of the current element
    DOMNamedNodeMap* nodeMap = pCurrent->getAttributes();

    // Check if the current node has any attributes
    if ((nodeMap != NULL) && (nodeMap->getLength() > 0)) {
        XMLCh* xmlString = NULL;
        xmlString = XMLString::transcode(attribName);
        if (xmlString) {
            DOMNode* attribNode = nodeMap->getNamedItem(xmlString);
            // Check if attribute node exists
            if (attribNode) {
                XMLCh* attribValue = NULL;
                attribValue = (XMLCh*)attribNode->getNodeValue();
                if (attribValue) {
                    // get the instance name
                    char* strValue = NULL;
                    strValue = XMLString::transcode(attribValue);
                    if (strValue) {
                        str.assign(strValue);
                        XMLString::release(&strValue);
                    }
                }
            }
            XMLString::release(&xmlString);
        }
    }
    return str;
}

void XML_utils::setAttribValue(DOMDocument* doc,
                               DOMNode* pCurrent,
                               const string& attribName,
                               const string& attribValue)
{
    string str;
    // Get the attributes, if any, of the current element
    DOMNamedNodeMap* nodeMap = pCurrent->getAttributes();

    // Check if the current node has any attributes
    if ((nodeMap != NULL) && (nodeMap->getLength() > 0)) {
        XMLCh* xmlString = NULL;
        xmlString = XMLString::transcode(attribName.c_str());
        if (xmlString) {
            DOMNode* attribNode = nodeMap->getNamedItem(xmlString);
            // Check if attribute node exists
            if (attribNode != NULL) {
                XMLCh* value = NULL;
                value = XMLString::transcode(attribValue.c_str());
                if (value) {
                    attribNode->setNodeValue(value);
                    XMLString::release(&value);
                }
            } else {
                DOMElement* el = static_cast<DOMElement*>(pCurrent);
                XMLCh* name = NULL;
                name = XMLString::transcode(attribName.c_str());
                XMLCh* value = NULL;
                value = XMLString::transcode(attribValue.c_str());
                el->setAttribute(name, value);
                if (name) {
                    XMLString::release(&name);
                }
                if (value) {
                    XMLString::release(&value);
                }
            }
            XMLString::release(&xmlString);
        }
    } else {
        DOMElement* el = static_cast<DOMElement*>(pCurrent);
        XMLCh* name = NULL;
        name = XMLString::transcode(attribName.c_str());
        XMLCh* value = NULL;
        value = XMLString::transcode(attribValue.c_str());
        el->setAttribute(name, value);
        if (name) {
            XMLString::release(&name);
        }
        if (value) {
            XMLString::release(&value);
        }
    }
}

DOMNode* XML_utils::getMatchingNode(DOMDocument* pDoc,
                                    const char* tagName,
                                    const char* attribName,
                                    const char* attribValue)
{
    DOMNodeList* nodeList = pDoc->getElementsByTagName(XML_UNICODE_FORM(tagName));
    // go through the list and find the element with the matching value.
    int numElements = nodeList->getLength();
    for (int i = 0; i < numElements; i++) {
        if (getAttribValue(nodeList->item(i), attribName) == attribValue) {
            return nodeList->item(i);
        }
    }
    return NULL;
}

int XML_utils::addChildTextTag(DOMDocument* pDoc,
                               const char* tagName,
                               const char* attribName,
                               const char* attribValue,
                               const char* childTag,
                               const char* childValue)
{
    DOMNode* dNode = getMatchingNode(pDoc, tagName, attribName, attribValue);
    if (!dNode) {
        return 1; // error
    }
    DOMElement* pElem = pDoc->createElement(XML_UNICODE_FORM(childTag));
    dNode->appendChild(pElem);
    DOMText* pFragment = pDoc->createTextNode(XML_UNICODE_FORM(childValue));
    pElem->appendChild(pFragment);
    return 0;
}

void XML_utils::trimWhiteSpace(string& str)
{
    string::size_type fpos = str.find_first_not_of(" \r\t\n");
    string::size_type lpos = str.find_last_not_of(" \r\t\n");
    if (lpos == string::npos || fpos == string::npos) {
        str = string();
    } else {
        str = str.substr(fpos, lpos - fpos + 1);
    }
}

string XML_utils::XMLCh2string(const XMLCh* chars)
{
    const char* const outEncoding = "iso-8859-1";
    XMLSize_t length = XMLString::stringLen(chars);
    auto_ptr<XMLByte> result(new XMLByte[length + 1]);
    XMLTransService::Codes failReason;
    XMLCh* fOutEncoding = XMLString::transcode(outEncoding);
    auto_ptr<XMLTranscoder> xml_iso_8859_1_transcoder(
      XMLPlatformUtils::fgTransService->makeNewTranscoderFor(fOutEncoding, failReason, 1024));
    if (xml_iso_8859_1_transcoder.get() == NULL) {
        cerr << "ERROR: XMLCh2char: failed to switch encoding " << endl;
        return string("");
    }

    XMLSize_t charsConsumed = 0;
    unsigned bytes_encoded = xml_iso_8859_1_transcoder->transcodeTo(
      static_cast<const XMLCh*>(chars), length, static_cast<XMLByte*>(result.get()), length,
      charsConsumed, XMLTranscoder::UnRep_Throw);
    XMLByte* ptr = result.get();
    ptr[bytes_encoded] = '\0';
    delete[] fOutEncoding;

    return string(reinterpret_cast<char*>(result.get()));
}

string XML_utils::escape(const char* str)
{
    string res;

    if (str) {
        const char* ptr = str;

        while (*ptr) {
            switch (*ptr) {
                case '<':
                    res += "&lt;";
                    break;
                case '>':
                    res += "&gt;";
                    break;
                case '&':
                    res += "&amp;";
                    break;
                default:
                    res += *ptr;
            }

            ptr++;
        }
    }

    return res;
}
