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

#include "SPL/Toolkit/XMLParse.h"
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <alloca.h>
#include <endian.h>
#include <iconv.h>
#include <iostream>
#include <libxml/parserInternals.h>
#include <string.h>

using namespace std;

namespace SPL {
class XMLParseImpl
{
  public:
    XMLParseImpl(XMLParse& p, bool isUTF16, bool ignoreNamespaces, bool xmlParseHuge = false);
    ~XMLParseImpl();
    void parse(const char* data, size_t dataLen);
    void warning(const char* msg);
    void error(const char* msg);
    void error(xmlErrorPtr e);
    void shutdown();
    void resetParser();
    void finishedParse()
    {
        if (!_sentFinishedParse) {
            _p.finishedParse();
            _sentFinishedParse = true;
        }
    }
    void doStartElement(const char* name, const char** attrs)
    {
        _depth++;
        _p.doStartElement(name, attrs);
    }
    void doStartElementNs(const char* localname,
                          const char* prefix,
                          const char* URI,
                          int nb_namespaces,
                          const char** namespaces,
                          int nb_attributes,
                          int nb_defaulted,
                          const char** attributes)
    {

        _depth++;
        if (attributes == NULL) {
            _p.doStartElement(localname, NULL);
        } else {
            const char* localAttrs[nb_attributes * 2 + 1];
            string values[nb_attributes];
            for (int i = 0; i < nb_attributes; i++) {
                localAttrs[2 * i] = attributes[i * 5];
                values[i] = string((const char*)attributes[i * 5 + 3],
                                   (size_t)(attributes[i * 5 + 4] - attributes[i * 5 + 3]));
                localAttrs[2 * i + 1] = values[i].c_str();
            }
            localAttrs[nb_attributes * 2] = NULL;
            _p.doStartElement(localname, localAttrs);
        }
    }
    void doEndElement(const char* name)
    {
        _p.doEndElement(name);
        _depth--;
        if (_depth == 0) {
            finishedDocument();
        }
    }
    void doEndElementNs(const char* localname, const char* prefix, const char* URI)
    {
        _p.doEndElement(localname);
        _depth--;
        if (_depth == 0) {
            finishedDocument();
        }
    }
    void doCharacters(const char* characters, int len) { _p.doCharacters(characters, len); }
    void initState() { _p.initState(); }
    void resetState() { _p.resetState(); }
    uint64_t unusedDataLeft() const { return _leftover; }
    void createParser(const char* chunk, int size)
    {
        _xmlContext = xmlCreatePushParserCtxt(_handler, this, chunk, size, NULL);
    }

  private:
    void computeRemainderCount(const string& encoding);
    void finishedDocument();
    void parseUntilNextDocument(const char*& data, size_t& dataLen);
    void bufferCharacters(const char*& data, size_t& dataLen);
    uint16_t getNextChar(const unsigned char* data);
    bool detectEncoding(const unsigned char* data);

    static int32_t _libXml2InitCount;
    xmlSAXHandler _saxHandler;
    xmlParserCtxtPtr _xmlContext;
    void initializeParser();
    void terminateParser();
    void freeXmlParser();
    enum UTFType
    {
        UTF8,
        UTF16LE,
        UTF16BE,
        UCS32LE,
        UCS32BE
    } _utfType;
    size_t _bump;
    bool _sentFinishedParse;
    bool _eatEndOfDocument;
    bool _encodingDetected;
    bool _shutdown;
    bool _isUTF16;
    bool _sawBOM;
    uint64_t _leftover;
    XMLParse& _p;
    char _buffer[4]; // used to save initial doc if needed
    unsigned int _buflen;
    unsigned int _depth;
    enum EndOfDocState
    {
        Ready,                   // Ready to accept anything
        SeenLessThan,            // Parsed '<'
        InProcessingInstruction, // Seen <?
        InComment1,              // Seen <!
        InComment2,              // Seen <!-
        InComment3,              // Seen <!--
        InComment4,              // Seen <!-- ...  -
        InComment5               // Seen <!-- ...  --
    } _eodState;                 // Where are we at the end of the doc parsing?
    string _iconvEncoding;       // last saved encoding
    iconv_t _iconv;
    xmlSAXHandler* _handler;
    bool xmlParseHuge_;
};

int32_t XMLParseImpl::_libXml2InitCount = 0;

XMLParse::XMLParse(bool isUTF16, bool ignoreNamespaces) :
  _shutdown(false),
  _impl(*new XMLParseImpl(*this, isUTF16, ignoreNamespaces, /*xmlParseHuge*/ false))
{}

XMLParse::XMLParse(bool isUTF16, bool ignoreNamespaces, bool xmlParseHuge) :
  _shutdown(false),
  _impl(*new XMLParseImpl(*this, isUTF16, ignoreNamespaces, xmlParseHuge))
{}

XMLParse::~XMLParse()
{
    delete &_impl;
}

void XMLParse::parse(const char* data, size_t dataLen)
{
    _impl.parse(data, dataLen);
}

void XMLParse::shutdown()
{
    _shutdown = true;
    _impl.shutdown();
}

void XMLParse::resetParser()
{
    resetState();
    _impl.resetParser();
}

uint64_t XMLParse::unusedDataLeft() const
{
    return _impl.unusedDataLeft();
}

void XMLParseImpl::initializeParser()
{
    if (++_libXml2InitCount == 1) {
        xmlInitParser();
    }
}

void XMLParseImpl::terminateParser()
{
    if (--_libXml2InitCount == 0) {
        xmlCleanupParser();
    }
}

void XMLParseImpl::warning(const char* msg)
{
    _p.warning(msg);
}

void XMLParseImpl::computeRemainderCount(const string& encoding)
{
    // Can we reuse the last encoding?
    if (_iconvEncoding != encoding) {
        if (!_iconvEncoding.empty()) {
            iconv_close(_iconv);
        }
        _iconv = iconv_open(encoding.c_str(), "UTF-8");
        _iconvEncoding = encoding;
    } else {
        iconv(_iconv, NULL, 0, NULL, 0);
    }
    size_t inbytesleft = _leftover;
    char* inbuf = (char*)_xmlContext->input->cur;
    // Why 9?
    const size_t outSize = _leftover * 9;
    size_t outbytesleft = outSize;
    char* outbuf = (char*)alloca(outbytesleft);
    iconv(_iconv, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
    _leftover = outSize - outbytesleft + inbytesleft;
}

void XMLParseImpl::finishedDocument()
{
    _leftover = _xmlContext->input->end - _xmlContext->input->cur;
    // Need to worry about encoding
    const char* encoding = (const char*)_xmlContext->input->encoding;
    if (encoding) {
        computeRemainderCount(encoding);
    } else if (_utfType != UTF8) {
        // Take our best guess
        if (_utfType == UTF16LE || _utfType == UTF16BE) {
            computeRemainderCount("UTF16LE");
        } else {
            computeRemainderCount("UCS4");
        }
    }
    xmlStopParser(_xmlContext);
    finishedParse();
    _eatEndOfDocument = true;
    _eodState = Ready;
}

uint16_t XMLParseImpl::getNextChar(const unsigned char* data)
{
    switch (_utfType) {
        case UTF16LE:
            return data[1] << 8 | data[0];
        case UTF16BE:
            return data[0] << 8 | data[1];
        case UCS32LE:
            return data[3] << 24 | data[2] << 16 | data[1] << 8 | data[0];
        case UCS32BE:
            return data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];
        default:
            break;
    }
    return *data;
}

void XMLParseImpl::bufferCharacters(const char*& data, size_t& dataLen)
{
    while (_buflen < sizeof(_buffer) && dataLen > 0) {
        _buffer[_buflen++] = *data++;
        dataLen--;
    }
}

void XMLParseImpl::parseUntilNextDocument(const char*& data, size_t& dataLen)
{
    // We want to get to the start of the next XML document.  Legal entities after
    // the end of the document are:  comments, processing directives, blanks
    while (dataLen) {
        if (dataLen < _bump) {
            // We don't have a full character here
            _p.error("Unknown content at end of UTF-16 or UTF-32 document - ignored");
            dataLen = 0;
            _eatEndOfDocument = false;
            _encodingDetected = false;
            return;
        }

        uint16_t curChar = getNextChar((const unsigned char*)data);
        // Have we hit a BOM?
        if (curChar == 0xFEFF || (_utfType == UTF8 && curChar == 0xEF)) {
            // Assume a BOM.   We must have finished the previous document
            _eatEndOfDocument = false;
            _encodingDetected = false;
            return;
        }
        switch (_eodState) {
            case Ready: // Ready to accept anything
                // Look for a blank or a comment or <?
                switch (curChar) {
                    case 0x20:
                    case 0x09:
                    case 0x0A:
                    case 0x0D:
                        // Blanks - just ignore
                        break;
                    case '<':
                        _eodState = SeenLessThan;
                        break;
                    default:
                        // Unexpected data!
                        break;
                }
                break;
            case SeenLessThan: // Parsed '<'
                if (curChar == '!') {
                    _eodState = InComment1;
                } else {
                    // assume start of next document
                    if (_utfType == UTF8) {
                        _buffer[0] = '<';
                        _buflen = 1;
                    } else if (_isUTF16) {
                        // We know that this is UTF16, and we haven't seen a BOM (or we wouldn't be here)
                        // Put one in
#if __BYTE_ORDER == __LITTLE_ENDIAN
                        _buffer[0] = 0xFF;
                        _buffer[1] = 0xFE;
                        _buffer[2] = '<';
                        _buffer[3] = '\0';

#else
                        _buffer[0] = 0xFE;
                        _buffer[1] = 0xFF;
                        _buffer[2] = '\0';
                        _buffer[3] = '<';
#endif
                        _buflen = 4;
                    } else {
                        _p.error("Unknown content at end of UTF-16 or UTF-32 document - ignored");
                        dataLen = 0;
                    }
                    _eatEndOfDocument = false;
                    _encodingDetected = false;
                    return;
                }
                break;
            case InComment1: // Seen <!
                if (curChar == '-') {
                    _eodState = InComment2;
                } else {
                    _eodState = Ready; // Unexpected data!
                }
                break;
            case InComment2: // Seen <!-
                if (curChar == '-') {
                    _eodState = InComment3;
                } else {
                    _eodState = Ready; // Unexpected data!
                }
                break;
            case InComment3: // Seen <!--
                // Already in the comment
                if (curChar == '-') {
                    _eodState = InComment4;
                }
                break;
            case InComment4: // Seen <!-- ...  -
                if (curChar == '-') {
                    _eodState = InComment5;
                } else {
                    _eodState = InComment3;
                }
                break;
            case InComment5: // Seen <!-- ...  --
                if (curChar == '>') {
                    _eodState = Ready;
                } else {
                    _eodState = InComment3;
                }
                break;
            case InProcessingInstruction: // Seen <?
                // can't get here now
                break;
        }
        // Consume the char we just examined
        data += _bump;
        dataLen -= _bump;
    }
}

void XMLParseImpl::error(const char* msg)
{
    if (_shutdown) {
        return;
    }
    _p.error(msg);
}

void XMLParseImpl::error(xmlErrorPtr e)
{
    if (e->code == XML_ERR_DOCUMENT_END) {
        // we are at the end.  remember the leftover amount
        finishedParse();
        _eodState = Ready;
    } else {
        if (_shutdown) {
            return;
        }

        if (e->level == XML_ERR_FATAL) {
            _p.error(e->message);
        } else {
            _p.warning(e->message);
        }
    }
}

/**
     * Callbacks for the libxml2 SAX parser.
     * DO NOT THROW EXCEPTIONS IN CALLBACKS!!
     * Because the pure C libxml2 library which calls those callbacks will just choke on them.
     * Instead log the error
     **/

static xmlEntityPtr doGetEntity(void* ctx, const xmlChar* name)
{
    return xmlGetPredefinedEntity(name);
}

static void doStartDocument(void* ctx)
{
    XMLParseImpl* pi = static_cast<XMLParseImpl*>(ctx);
    pi->initState();
}

static void doEndDocument(void* ctx)
{
    XMLParseImpl* pi = static_cast<XMLParseImpl*>(ctx);
    pi->finishedParse();
    pi->resetState();
}

static void doStartElement(void* ctx, const xmlChar* name, const xmlChar** attrs)
{
    XMLParseImpl* pi = static_cast<XMLParseImpl*>(ctx);
#if DEBUG
    cout << "Start Element: " << name;
    const xmlChar** p = attrs;
    if (p) {
        cout << " attrs: ";
        while (*p) {
            cout << *p << "=\"" << *(p + 1) << "\" ";
            p += 2;
        }
    }
    cout << endl;
#endif
    pi->doStartElement((const char*)name, (const char**)attrs);
}

static void doStartElementNs(void* ctx,
                             const xmlChar* localname,
                             const xmlChar* prefix,
                             const xmlChar* URI,
                             int nb_namespaces,
                             const xmlChar** namespaces,
                             int nb_attributes,
                             int nb_defaulted,
                             const xmlChar** attributes)
{
    XMLParseImpl* pi = static_cast<XMLParseImpl*>(ctx);
#if DEBUG
    cout << "Start Element: NS ";
    if (prefix)
        cout << prefix << ':';
    cout << localname;
    const xmlChar** p = attributes;
    if (p) {
        cout << " attrs: ";
        while (*p) {
            cout << p[0] << "=\"" << string((const char*)p[3], (size_t)(p[4] - p[3])) << "\" ";
            p += 5;
        }
    }
    cout << endl;
#endif
    pi->doStartElementNs((const char*)localname, (const char*)prefix, (const char*)URI,
                         nb_namespaces, (const char**)namespaces, nb_attributes, nb_defaulted,
                         (const char**)attributes);
}

static void doEndElement(void* ctx, const xmlChar* name)
{
    XMLParseImpl* pi = static_cast<XMLParseImpl*>(ctx);
#if DEBUG
    cout << "End Element: " << name << endl;
#endif
    pi->doEndElement((const char*)name);
}

static void doEndElementNs(void* ctx,
                           const xmlChar* localname,
                           const xmlChar* prefix,
                           const xmlChar* URI)
{
    XMLParseImpl* pi = static_cast<XMLParseImpl*>(ctx);
#if DEBUG
    cout << "End Element NS: ";
    if (prefix)
        cout << prefix << ':';
    cout << localname << endl;
#endif
    pi->doEndElementNs((const char*)localname, (const char*)prefix, (const char*)URI);
}

static void doCharacters(void* ctx, const xmlChar* characters, int len)
{
    XMLParseImpl* pi = static_cast<XMLParseImpl*>(ctx);
#if DEBUG
    cout << "Characters: \"" << string((const char*)characters, len) << '"' << endl;
#endif
    pi->doCharacters((const char*)characters, len);
}

static void doComment(void* ctx, const xmlChar* comment)
{
#if DEBUG
    // XMLParseImpl * pi = static_cast<XMLParseImpl*>(ctx);
    cout << "Comment: '" << comment << '\'' << endl;
#endif
}

static void doWarning(void* ctx, const char* msg, ...)
{
    XMLParseImpl* pi = static_cast<XMLParseImpl*>(ctx);
    va_list args;
    va_start(args, msg);
    char buffer[4096];
    vsnprintf(buffer, sizeof(buffer), reinterpret_cast<const char*>(msg), args);
    va_end(args);
    pi->warning(buffer);
}

static void doStructuredError(void* ctx, xmlErrorPtr error)
{
#if DEBUG
    cout << "Structured Error: " << error->message << endl;
#endif
    XMLParseImpl* pi = static_cast<XMLParseImpl*>(ctx);
    pi->error(error);
}

static void doProcessingInstruction(void* ctx, const xmlChar* target, const xmlChar* data)
{
#if DEBUG
    // XMLParseImpl * pi = static_cast<XMLParseImpl*>(ctx);
    cout << "Processing Instruction: '" << target << "' '" << data << '\'' << endl;
#endif
}

static xmlSAXHandler sax2Handler_default = {
    0,                       /* internalSubset */
    0,                       /* isStandalone */
    0,                       /* hasInternalSubset */
    0,                       /* hasExternalSubset */
    0,                       /* resolveEntity */
    doGetEntity,             /* getEntity */
    0,                       /* entityDecl */
    0,                       /* notationDecl */
    0,                       /* attributeDecl */
    0,                       /* elementDecl */
    0,                       /* unparsedEntityDecl */
    0,                       /* setDocumentLocator */
    doStartDocument,         /* startDocument */
    doEndDocument,           /* endDocument */
    doStartElement,          /* startElement */
    doEndElement,            /* endElement */
    0,                       /* reference */
    doCharacters,            /* characters */
    0,                       /* ignorableWhitespace */
    doProcessingInstruction, /* processingInstruction */
    doComment,               /* comment */
    doWarning,               /* warning */
    0,                       /* error */
    0,                       /* fatalError */
    0,                       /* getParameterEntity */
    0,                       /* cdataBlock */
    0,                       /* externalSubset */
    XML_SAX2_MAGIC,          /* initialized */
    0,                       /* _private */
    0,                       /* startElementNs */
    0,                       /* endElementNs */
    doStructuredError,       /* serror */
};

static xmlSAXHandler sax2HandlerNs_default = {
    0,                       /* internalSubset */
    0,                       /* isStandalone */
    0,                       /* hasInternalSubset */
    0,                       /* hasExternalSubset */
    0,                       /* resolveEntity */
    doGetEntity,             /* getEntity */
    0,                       /* entityDecl */
    0,                       /* notationDecl */
    0,                       /* attributeDecl */
    0,                       /* elementDecl */
    0,                       /* unparsedEntityDecl */
    0,                       /* setDocumentLocator */
    doStartDocument,         /* startDocument */
    doEndDocument,           /* endDocument */
    0,                       /* startElement */
    0,                       /* endElement */
    0,                       /* reference */
    doCharacters,            /* characters */
    0,                       /* ignorableWhitespace */
    doProcessingInstruction, /* processingInstruction */
    doComment,               /* comment */
    doWarning,               /* warning */
    0,                       /* error */
    0,                       /* fatalError */
    0,                       /* getParameterEntity */
    0,                       /* cdataBlock */
    0,                       /* externalSubset */
    XML_SAX2_MAGIC,          /* initialized */
    0,                       /* _private */
    doStartElementNs,        /* startElementNs */
    doEndElementNs,          /* endElementNs */
    doStructuredError,       /* serror */
};

XMLParseImpl::XMLParseImpl(XMLParse& p, bool isUTF16, bool ignoreNamespaces, bool xmlParseHuge) :
  _xmlContext(NULL),
  _utfType(UTF8),
  _bump(1),
  _sentFinishedParse(false),
  _eatEndOfDocument(false),
  _encodingDetected(false),
  _shutdown(false),
  _isUTF16(isUTF16),
  _sawBOM(false),
  _leftover(0),
  _p(p),
  _buflen(0),
  _depth(0),
  _handler(ignoreNamespaces ? &sax2HandlerNs_default : &sax2Handler_default),
  xmlParseHuge_(xmlParseHuge)
{
    initializeParser();
}

XMLParseImpl::~XMLParseImpl()
{
    freeXmlParser();
    terminateParser();
    if (!_iconvEncoding.empty()) {
        iconv_close(_iconv);
    }
}

void XMLParseImpl::freeXmlParser()
{
    if (_xmlContext) {
        _xmlContext->disableSAX = 1;
        // Possibly this is not needed??
        xmlParseChunk(_xmlContext, "", 0, 1);
        xmlFreeParserCtxt(_xmlContext);
        _xmlContext = NULL;
    }
    _sentFinishedParse = false;
}

bool XMLParseImpl::detectEncoding(const unsigned char* data)
{
    // Already figured it out
    if (_encodingDetected) {
        return false;
    }
    _sawBOM = false;

    bool skipBOM = false;
    // libxml2 doesn't seem to look for 32 bit BOMs
    if (data[0] == 0x3C && data[1] == 0x00 && data[2] == 0x00 && data[3] == 0x00) {
        _bump = 4;
        _utfType = UCS32LE;
    } else if (data[0] == 0x00 && data[1] == 0x00 && data[2] == 0x00 && data[3] == 0x3C) {
        _bump = 4;
        _utfType = UCS32BE;
        // Now test the Byte Order Marker (BOM)
    } else if (data[0] == 0x00 && data[1] == 0x00 && data[2] == 0xFE && data[3] == 0xFF) {
        _bump = 4;
        _utfType = UCS32BE;
        _sawBOM = true;
        skipBOM = true;
    } else if (data[0] == 0xFF && data[1] == 0xFE && data[2] == 0x00 && data[3] == 0x00) {
        _bump = 4;
        _utfType = UCS32LE;
        _sawBOM = true;
        skipBOM = true;
    } else if (data[0] == 0xFE && data[1] == 0xFF) {
        _sawBOM = true;
        _bump = 2;
        _utfType = UTF16BE;
    } else if (data[0] == 0x3C && data[1] == 0x00 && data[2] == 0x3F && data[3] == 0x00) { // <?
        _bump = 2;
        _utfType = UTF16LE;
    } else if (data[0] == 0xFF && data[1] == 0xFE) {
        _sawBOM = true;
        _bump = 2;
        _utfType = UTF16LE;
    } else if (data[0] == 0x00 && data[1] == 0x3C && data[2] == 0x00 && data[3] == 0x3F) { // <?
        _bump = 2;
        _utfType = UTF16BE;
    } else if (_isUTF16) {
        _bump = 2;
#if __BYTE_ORDER == __LITTLE_ENDIAN
        _utfType = UTF16LE;
#else
        _utfType = UTF16BE;
#endif
    } else {
        _bump = 1;
        _utfType = UTF8;
    }
    // We know what we are :-)
    _encodingDetected = true;
    return skipBOM;
}

void XMLParseImpl::parse(const char* data, size_t dataLen)
{
    while (true) {
        if (_eatEndOfDocument) {
            freeXmlParser();
            // we are at the end of the document.  We have to parse enough XML to
            // get to the start of the current document
            parseUntilNextDocument(data, dataLen);
            if (_eatEndOfDocument) {
                return; // haven't found the next document yet
            }
        }
        _leftover = 0;
        if (!_xmlContext) {
            _sentFinishedParse = false;
            _depth = 0;
            // We have to have at least 4 characters to call xmlCreatePushParserCtxt
            // to allow encoding detection.   Buffer them here if needed
            if (_buflen || dataLen < sizeof(_buffer)) {
                // Fill up _buffer
                bufferCharacters(data, dataLen);

                // Is there enough?
                if (_buflen < sizeof _buffer) {
                    return; // We don't have enough data to start this yet
                }

                // Initialize the context with the buffer
                _buflen = 0;
                if (detectEncoding((const unsigned char*)_buffer)) {
                    bufferCharacters(data, dataLen);

                    // Is there enough?
                    if (_buflen < sizeof _buffer) {
                        return; // We don't have enough data to start this yet
                    }
                } else if (_isUTF16 && !_sawBOM) {
                    // There is no BOM, libxml2 will need one
                    if (_utfType == UTF16LE) {
                        _buffer[0] = 0xFF;
                        _buffer[1] = 0xFE;
                    } else {
                        _buffer[0] = 0xFE;
                        _buffer[1] = 0xFF;
                    }
                    _buflen = 2;

                    bufferCharacters(data, dataLen);

                    // Is there enough?
                    if (_buflen < sizeof _buffer) {
                        return; // We don't have enough data to start this yet
                    }
                }
                createParser(_buffer, sizeof(_buffer));
            } else {
                // We have enough data to do this
                if (detectEncoding((const unsigned char*)data)) {
                    // We had a BOM that libxml2 doesn't like
                    _buflen = 0;
                    data += sizeof _buffer;
                    dataLen -= sizeof _buffer;
                    if (dataLen < sizeof _buffer) {
                        bufferCharacters(data, dataLen);
                        return; // We don't have enough data to start this yet
                    }
                    createParser(data, sizeof(_buffer));
                    data += sizeof(_buffer);
                    dataLen -= sizeof(_buffer);
                } else if (_isUTF16 && !_sawBOM) {
                    // There is no BOM, libxml2 will need one
                    if (_utfType == UTF16LE) {
                        _buffer[0] = 0xFF;
                        _buffer[1] = 0xFE;
                    } else {
                        _buffer[0] = 0xFE;
                        _buffer[1] = 0xFF;
                    }
                    _buflen = 2;
                    bufferCharacters(data, dataLen);

                    // Is there enough?
                    if (_buflen < sizeof _buffer) {
                        return; // We don't have enough data to start this yet
                    }
                    createParser(_buffer, sizeof(_buffer));
                } else {
                    createParser(data, sizeof(_buffer));
                    data += sizeof(_buffer);
                    dataLen -= sizeof(_buffer);
                }
            }

#ifdef DEVELOPER_WORKS
            int xmlOptions = XML_PARSE_RECOVER;
#else
            int xmlOptions =
              _isUTF16 ? (XML_PARSE_IGNORE_ENC | XML_PARSE_RECOVER) : XML_PARSE_RECOVER;
#endif
            if (xmlParseHuge_) {
                xmlOptions |= XML_PARSE_HUGE;
            }
            xmlCtxtUseOptions(_xmlContext, xmlOptions);
        }

        // May have data left over from above
        if (dataLen) {
            xmlParseChunk(_xmlContext, data, dataLen, 0);
        }

        // Do we have a leftover amount?
        if (_leftover) {
            freeXmlParser();
            data += (dataLen - _leftover);
            dataLen = _leftover;
        } else {
            break;
        }
    }
    // error code?
}

void XMLParseImpl::shutdown()
{
    _shutdown = true;
    freeXmlParser();
}

void XMLParseImpl::resetParser()
{

    freeXmlParser();
    _eatEndOfDocument = false;
    _encodingDetected = false;
    _eodState = Ready;

    _buflen = 0;
}

}
