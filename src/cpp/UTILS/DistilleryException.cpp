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

#include <signal.h>
#include <sstream>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>

#include <SAM/SAMHelperFunctions.h>
#include <SAM/SAMInterfaceTypes.h>
#include <SAM/SAMNamespaceDefs.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/BacktraceDumper.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/Exception.h>
#include <UTILS/SerializationBuffer.h>
#include <UTILS/SupportFunctions.h>

UTILS_NAMESPACE_USE
using namespace std;
DEBUG_NAMESPACE_USE
using namespace UTILS_RUNTIME_NS;

namespace EXCEPTION_XMLNS = ::xmlns::prod::streams::runtime::utils;

const string DistilleryException::NO_EXCEPTION = "noexception";

static string getBackTrace(void)
{
    stringstream str;
    BacktraceDumper::dump(str, false, 2);
    return str.str();
}

DistilleryException::DistilleryException(const string& _location,
                                         const string& _msg,
                                         const string& _backtrace,
                                         const DistilleryExceptionCode& _exceptionCode,
                                         const vector<string>& _substitutionText,
                                         const string& _additionalData)
  : location(_location)
  , msg(_msg)
  , backtrace(_backtrace.empty() ? getBackTrace() : _backtrace)
  , exceptionCode(_exceptionCode)
  , substitutionText(_substitutionText)
  , additionalData(_additionalData)
{}

DistilleryException::DistilleryException(const string& _location,
                                         const string& _msg,
                                         const DistilleryException& _cause,
                                         const DistilleryExceptionCode& _exceptionCode,
                                         const vector<string>& _substitutionText,
                                         const string& _additionalData)
  : location(_location)
  , msg(_msg)
  , backtrace(_cause.getBacktrace())
  , exceptionCode(_exceptionCode)
  , substitutionText(_substitutionText)
  , additionalData(_additionalData)
{
    ostringstream new_msg;
    new_msg << msg << " (Caused by ";
    _cause.printShort(new_msg);
    new_msg << ")";

    /*
      this code is precious to me... keeping it for Humber!

    std::stringstream __my_strm1 ;                                        \
    __my_strm1 << ex << endl ;                                            \
    std::stringstream __my_strm2 ;                                        \
    __my_strm2 << msg << endl ;                                           \
    __my_strm2 << "------------ NESTED EXCEPTION -------------" << endl ; \
    while(!__my_strm1.eof())                                              \
      {                                                                   \
        char buffer[4096] ;                                               \
        __my_strm1.getline(buffer, 4096) ;                                \
        if (!__my_strm1.eof())                                            \
        {                                                                 \
          __my_strm2 << "    " << buffer << endl ;                        \
        }                                                                 \
      }                                                                   \
    __my_strm2 << "-------------------------------------------" << endl ; \
     */

    msg = new_msg.str();
}

DistilleryException::DistilleryException(const DistilleryException& e)
  : location(e.location)
  , msg(e.msg)
  , backtrace(e.backtrace)
  , exceptionCode(e.exceptionCode)
  , substitutionText(e.substitutionText)
  , additionalData(e.additionalData)
{}

DistilleryException::DistilleryException(SerializationBuffer& sb)
  : // the "type" has already been consumed from the buffer to instantiate
  // the object
  location(sb.getSTLString())
  , msg(sb.getSTLString())
  , backtrace(sb.getSTLString())
  , exceptionCode(sb)
{
    uint32_t sz = sb.getUInt32();

    substitutionText.resize(sz);
    for (uint32_t i = 0; i < sz; ++i) {
        substitutionText[i] = sb.getSTLString();
    }
    additionalData = sb.getSTLString();
}

void DistilleryException::serialize(SerializationBuffer& sb) const
{
#ifdef DST_DEBUG_MACRO
    cout << "------------------------------------> " << getType() << endl;
#endif

    // the type will be consumed by the instanciator, not by the
    // DistilleryException constructor
    sb.addSTLString(getType());

    sb.addSTLString(location);
    sb.addSTLString(msg);
    sb.addSTLString(backtrace);

    /// Serialize exception code and substitution texts
    exceptionCode.serialize(sb);
    uint32_t subSize = substitutionText.size();
    sb.addUInt32((uint32_t)subSize);

    vector<string>::size_type sz = subSize;
    for (uint32_t i = 0; i < sz; i++) {
        sb.addSTLString(substitutionText[i]);
    }
    sb.addSTLString(additionalData);
}

const string& DistilleryException::getExplanation(void) const
{
    return msg;
}

const string& DistilleryException::getLocation(void) const
{
    return location;
}

const string& DistilleryException::getBacktrace(void) const
{
    return backtrace;
}

const DistilleryExceptionCode& DistilleryException::getExceptionCode(void) const
{
    return exceptionCode;
}

void DistilleryException::setMessageID(const char* msgId)
{
    exceptionCode.setMessageId(msgId);
}

const vector<string>& DistilleryException::getSubstitutionText(void) const
{
    return substitutionText;
}

void DistilleryException::setSubstitutionText(const vector<string>& replacements)
{
    substitutionText.insert(substitutionText.begin(), replacements.begin(), replacements.end());
}

bool DistilleryException::hasAdditionalData(void) const
{
    return additionalData.length() > 0;
}

const string& DistilleryException::getAdditionalDataXML(void) const
{
    return additionalData;
}

EXCEPTION_XMLNS::additionalExceptionDataType DistilleryException::getAdditionalData(void) const
{
    // the default additionalExceptionDataType ctor doesn't yield a legal tree
    // (primaryInformation().exceptionCode() segfaults).

    if (!hasAdditionalData()) {
        throw std::runtime_error("Internal Error: DistilleryException::getAdditionalData() called "
                                 "when hasAdditionalData()==false");
    }

    EXCEPTION_XMLNS::additionalExceptionDataType additionalDataObj;
    fromString(additionalDataObj, additionalData);
    return additionalDataObj;
}

EXCEPTION_XMLNS::additionalExceptionDataType DistilleryException::getMessageInfo(void) const
{
    EXCEPTION_XMLNS::MessageElementType primaryMsg(getExceptionCode().getMessageId());

    vector<string> substText = getSubstitutionText();

    for (vector<string>::const_iterator iter = substText.begin(); iter != substText.end(); iter++) {
        primaryMsg.substitutionText().push_back(*iter);
    }

    EXCEPTION_XMLNS::additionalExceptionDataType messageInfo(primaryMsg);
    if (hasAdditionalData()) {
        messageInfo.nestedAdditionalExceptionData().push_back(getAdditionalData());
    }

    return (messageInfo);
}

void DistilleryException::setAdditionalData(const string xml)
{
    additionalData = xml;
    SPCDBG(L_TRACE, "Additional Exception Data is " << additionalData, CORE_UTILS);
}

void DistilleryException::setAdditionalData(const EXCEPTION_XMLNS::additionalExceptionDataType& obj)
{
    additionalData = toString(obj);
    SPCDBG(L_TRACE, "Additional Exception Data is " << additionalData, CORE_UTILS);
}

void DistilleryException::toXML(stringstream& o) const
{
    EXCEPTION_XMLNS::exceptionType e;
    e.type(getType());
    e.location(location);
    e.msg(getExplanation());
    e.backtrace(getBacktrace());
    e.exceptionCode(getExceptionCode().getMessageId());
    for (unsigned i = 0, iu = substitutionText.size(); i < iu; ++i) {
        e.substitutionText().push_back(substitutionText[i]);
    }
    e.exceptionAdditionalData(additionalData);

    stringstream sstream;
    xml_schema::namespace_infomap map;
    try {
        map[""].name = "http://www.ibm.com/xmlns/prod/streams/runtime/utils";
        map["xs"].name = "http://www.w3.org/2001/XMLSchema";
        map["xsi"].name = "http://www.w3.org/2001/XMLSchema-instance";
        EXCEPTION_XMLNS::exception(sstream, e, map, "UTF-8", xml_schema::flags::dont_initialize);
    } catch (xml_schema::exception& xe) {
        SPCDBG(L_ERROR, "Error serializing exception: " << xe, CORE_UTILS);
        stringstream ess;
        ess << "Hit XML exception of error: " << xe.what()
            << " while serializing DistilleryException: " << getType();
        THROW(Distillery, ess.str());
    } catch (const std::exception& stde) {
        SPCDBG(L_ERROR, "Error serializing exception: " << stde.what(), CORE_UTILS);
        stringstream ess;
        ess << "Hit generic exception of error: " << stde.what()
            << " while serializing DistilleryException: " << getType();
        THROW(Distillery, ess.str());
    } catch (...) {
        SPCDBG(L_ERROR, "Unknown Error serializing exception", CORE_UTILS);
        stringstream ess;
        ess << "Hit unknown error while serializing DistilleryException: " << getType();
        THROW(Distillery, ess.str());
    }
    o << sstream.str();
}

void DistilleryException::printShort(ostream& o) const
{
    o << this->getType() << " (" << msg << ") at " << location;
}

void DistilleryException::printMedium(ostream& o) const
{
    printShort(o);

    o << endl
      << "Exception Code=" << exceptionCode << " with " << substitutionText.size()
      << " substitution text strings" << endl;

    for (uint i = 0; i < substitutionText.size(); i++) {
        o << substitutionText[i];
        if ((i != 0) && ((i % 5) == 0)) {
            o << endl;
        } else {
            if (i != (substitutionText.size() - 1)) {
                o << ", ";
            }
        }
    }
    o << endl << "additional data -- " << additionalData;
}

void DistilleryException::printLong(ostream& o) const
{
    printMedium(o);

    if (backtrace.size() > 0) {
        o << endl
          << "================= BACKTRACE ===============" << endl
          << backtrace << endl
          << "===========================================";
    }
}

DistilleryException::~DistilleryException(void) throw() {}

const char* DistilleryException::getType(void) const
{
    return "DistilleryException";
}

const string DistilleryException::toXMLString(void) const
{
    stringstream str;
    toXML(str);
    return str.str();
}

const char* DistilleryException::what() const throw()
{
    // return typeid (*this).name( );
    return getType();
}

// the following code is imbeded into the 2 definitions of setVariableArgs function
#define GenSetVariableArgsBody                                                                     \
    char* v = va_arg(vl, char*);                                                                   \
    if (v == NULL) {                                                                               \
        return;                                                                                    \
    }                                                                                              \
    exceptionCode.setMessageId(v);                                                                 \
    v = va_arg(vl, char*);                                                                         \
    string printString;                                                                            \
                                                                                                   \
    while (v) {                                                                                    \
        substitutionText.push_back(v);                                                             \
        printString.append(" ").append(v);                                                         \
        v = va_arg(vl, char*);                                                                     \
    }                                                                                              \
                                                                                                   \
    va_end(vl);                                                                                    \
                                                                                                   \
    SPCDBG(L_INFO, "Exception code set is " << exceptionCode, CORE_UTILS);                         \
    if (substitutionText.size() > 0) {                                                             \
        SPCDBG(L_INFO, "Substitution text is set to" << printString, CORE_UTILS);                  \
    }

void DistilleryException::setVariableArgs(int parm1, ...)
{
    // Note that the first parameter is not really used for anything other
    // then for va-start to know where the variable arguments start.
    // Also note that when this function is called, a NULL parm is added
    // to the end so the while test will actually work.
    va_list vl;
    va_start(vl, parm1);
    GenSetVariableArgsBody;
}

void DistilleryException::setVariableArgs(va_list vl)
{
    // Also note that when this function is called, a NULL parm is added
    // to the end so the while test will actually work.
    GenSetVariableArgsBody;
}

void DistilleryException::throwWithContext(const char* msg,
                                           const char* funcName,
                                           const char* fileName,
                                           int lineNum,
                                           ...)
{
    SPCDBG_TST(L_INFO, {
        std::stringstream __my_ex_strmx;
        __my_ex_strmx << "Throwing Exception: Distillery"
                      << " (Msg: " << msg << ")";
        Distillery::debug::write_msg(L_INFO, CORE_UTILS, "", funcName, fileName, lineNum,
                                     __my_ex_strmx.str());
    });
    std::stringstream __my_ex_strm;
    __my_ex_strm << msg;
    std::stringstream __my_ex_location;
    __my_ex_location << "'" << funcName << "'"
                     << " [" << fileName << ":" << lineNum << "]";
    DistilleryException __my_ex(__my_ex_location.str().c_str(), __my_ex_strm.str().c_str());
    va_list __my_args;
    va_start(__my_args, lineNum);
    __my_ex.setVariableArgs(__my_args);
    va_end(__my_args);
    throw __my_ex;
}

void DistilleryException::throwWithContextAndMsgs(const char* msg,
                                                  const char* funcName,
                                                  const char* fileName,
                                                  int lineNum,
                                                  const char* msgKey,
                                                  const vector<string>& replacements)
{
    SPCDBG_TST(L_INFO, {
        std::stringstream __my_ex_strmx;
        __my_ex_strmx << "Throwing Exception: Distillery"
                      << " (Msg: " << msg << ")";
        Distillery::debug::write_msg(L_INFO, CORE_UTILS, "", funcName, fileName, lineNum,
                                     __my_ex_strmx.str());
    });
    std::stringstream __my_ex_strm;
    __my_ex_strm << msg;
    std::stringstream __my_ex_location;
    __my_ex_location << "'" << funcName << "'"
                     << " [" << fileName << ":" << lineNum << "]";
    DistilleryException __my_ex(__my_ex_location.str().c_str(), __my_ex_strm.str().c_str());
    __my_ex.setMessageID(msgKey);
    __my_ex.setSubstitutionText(replacements);
    throw __my_ex;
}

void DistilleryException::addToAdditionalExceptionData(
  EXCEPTION_XMLNS::additionalExceptionDataType& obj,
  const DistilleryException& e)
{
    string msgId = e.getExceptionCode().getMessageId();
    if (!msgId.empty() && 0 != msgId.compare(DistilleryExceptionCode::NO_MESSAGE_ID)) {
        EXCEPTION_XMLNS::MessageElementType jrnlMsg(msgId);
        for (vector<string>::const_iterator it = e.getSubstitutionText().begin();
             it != e.getSubstitutionText().end(); it++) {
            jrnlMsg.substitutionText().push_back(*it);
        }
        obj.secondaryInformation().push_back(jrnlMsg);
    }
}

ostream& UTILS_NAMESPACE::operator<<(ostream& o, const DistilleryException& e)
{
    e.printLong(o);
    return o;
}

UTILS_NAMESPACE_BEGIN

string toString(const EXCEPTION_XMLNS::additionalExceptionDataType& additionalData)
{
    string s;
    try {
        stringstream stream;
        xml_schema::namespace_infomap map;
        map[""].name = "http://www.ibm.com/xmlns/prod/streams/runtime/utils";
        map["xs"].name = "http://www.w3.org/2001/XMLSchema";
        map["xsi"].name = "http://www.w3.org/2001/XMLSchema-instance";
        EXCEPTION_XMLNS::additionalExceptionData(stream, additionalData, map, "UTF-8",
                                                 xml_schema::flags::dont_initialize);
        s = stream.str();
        return s;
    } catch (xml_schema::exception& xe) {
        SPCDBG(L_ERROR, "Error serializing exception: " << xe, CORE_UTILS);
        stringstream ess;
        ess << "Hit XML exception of error: " << xe.what()
            << " while serializing additionalExceptionData";
        THROW(Distillery, ess.str());
    } catch (const std::exception& stde) {
        SPCDBG(L_ERROR, "Error serializing exception: " << stde.what(), CORE_UTILS);
        stringstream ess;
        ess << "Hit generic exception of error: " << stde.what()
            << " while serializing additionalExceptionData";
        THROW(Distillery, ess.str());
    } catch (...) {
        SPCDBG(L_ERROR, "Unknown Error serializing exception", CORE_UTILS);
        THROW(Distillery, "Hit unknown error while serializing additionalExceptionData");
    }
    return s;
}

void fromString(EXCEPTION_XMLNS::additionalExceptionDataType& additionalData, const string& s)
{

    SPCDBG(L_INFO, "additional data XML is-- " << s, CORE_UTILS);
    auto_ptr<EXCEPTION_XMLNS::additionalExceptionDataType> adt(NULL);
    try {
        stringstream ss(s);
        adt = EXCEPTION_XMLNS::additionalExceptionData(ss, xml_schema::flags::dont_validate |
                                                             xml_schema::flags::dont_initialize);
    } catch (xml_schema::exception& xe) {
        SPCDBG(L_ERROR, "Error deserializing additionalExceptionData: " << xe << ", XML=" << s,
               CORE_UTILS);
        stringstream ess;
        ess << "Hit XML exception of error: " << xe.what()
            << " while deserializing additionalExceptionData";
        THROW(Distillery, ess.str());
    } catch (const std::exception& stde) {
        SPCDBG(L_ERROR,
               "Error deserializing additionalExceptionData: " << stde.what() << ", XML=" << s,
               CORE_UTILS);
        stringstream ess;
        ess << "Hit generic exception of error: " << stde.what()
            << " while deserializing additionalExceptionData";
        THROW(Distillery, ess.str());
    } catch (...) {
        SPCDBG(L_ERROR, "Unknown Error deserializing exception: " << s, CORE_UTILS);
        THROW(Distillery, "Hit unknown error while deserializing additionalExceptionData");
    }
    additionalData = *adt;
}

UTILS_NAMESPACE_END

/*************************************************************************/
/*************************************************************************/
/*************************************************************************/
/*************************************************************************/

#if SPC_REGISTER_INSTANTIATORS

exception_map_t& DistilleryExceptionInstantiator::getInstantiators()
{
    static exception_map_t instantiators;
    return instantiators;
}

#endif

DistilleryExceptionInstantiator::DistilleryExceptionInstantiator(const char* type)
{
#if SPC_REGISTER_INSTANTIATORS
    registerExceptionInstantiator(this, type);
#endif
}

DistilleryExceptionInstantiator::~DistilleryExceptionInstantiator(void) {}

const char* DistilleryExceptionInstantiator::getType() const
{
    return "DistilleryException";
}

void DistilleryExceptionInstantiator::registerExceptionInstantiator(
  DistilleryExceptionInstantiator* new_instantiator,
  const char* type)
{
#if SPC_REGISTER_INSTANTIATORS
    exception_map_t& instantiators = getInstantiators();

    if (instantiators.find(type) == instantiators.end()) {
#ifdef DEBUG
        cout << "Registering 0x" << new_instantiator << " as " << type << endl;
#endif
        instantiators.insert(make_pair(type, new_instantiator));
    }

#endif
}

void DistilleryExceptionInstantiator::print(ostream& o)
{
#if SPC_REGISTER_INSTANTIATORS

    exception_map_t& instantiators = getInstantiators();
    exception_map_t::const_iterator it = instantiators.begin();

    while (it != instantiators.end()) {
        o << it->first << " ";
        it++;
    }

#else

    o << "exception registration is disabled";

#endif
}

void DistilleryExceptionInstantiator::instantiateAndThrow(SerializationBuffer& sb)
{
    string sub_type = sb.getSTLString();

#if SPC_REGISTER_INSTANTIATORS

    exception_map_t& instantiators = getInstantiators();
    map<const string, DistilleryExceptionInstantiator*>::const_iterator ei =
      instantiators.find(sub_type);
    if (ei != instantiators.end()) {
        SPCDBG(L_INFO, "instantiateAndThrow is throwing a new " << sub_type, CORE_UTILS);
        ei->second->throwNew(sb);
    } else {
        DistilleryException ex(sb);
        SPCDBG(L_INFO,
               "Unable to find an instantiator for "
                 << sub_type << "."
                 << " instantiateAndThrow is throwing a new DistilleryException ("
                 << ex.getExplanation() << ")",
               CORE_UTILS);
        throw ex;
    }

#else

    DistilleryException ex(sb);
    SPCDBG(L_INFO,
           "Unable to find an instantiator for "
             << sub_type << " (instantiators are disabled)."
             << " instantiateAndThrow is throwing a new DistilleryException ("
             << ex.getExplanation() << ")",
           CORE_UTILS);
    throw ex;

#endif
}

void DistilleryExceptionInstantiator::instantiateAndThrow(const string& xmlString)
{
    if (xmlString != DistilleryException::NO_EXCEPTION) {
        SPCDBG(L_INFO, "exception is " << xmlString, CORE_UTILS);
        auto_ptr<EXCEPTION_XMLNS::exceptionType> eap(NULL);
        try {
            stringstream ss(xmlString);
            eap = EXCEPTION_XMLNS::exception(ss, xml_schema::flags::dont_validate |
                                                   xml_schema::flags::dont_initialize);

        } catch (xml_schema::exception& xe) {
            SPCDBG(L_ERROR, "Error deserializing exception: " << xe << ", XML=" << xmlString,
                   CORE_UTILS);
            stringstream ess;
            ess << "Hit XML exception of error: " << xe.what()
                << " while deserializing DistilleryException";
            THROW(Distillery, ess.str());
        } catch (const std::exception& stde) {
            SPCDBG(L_ERROR,
                   "Error deserializing exception: " << stde.what() << ", XML=" << xmlString,
                   CORE_UTILS);
            stringstream ess;
            ess << "Hit generic exception of error: " << stde.what()
                << " while deserializing DistilleryException";
            THROW(Distillery, ess.str());
        } catch (...) {
            SPCDBG(L_ERROR, "Unknown Error deserializing exception: " << xmlString, CORE_UTILS);
            THROW(Distillery, "Hit unknown error while deserializing DistilleryException");
        }
        const EXCEPTION_XMLNS::exceptionType& e = *eap;
        vector<string> vec(e.substitutionText().begin(), e.substitutionText().end());

#if SPC_REGISTER_INSTANTIATORS
        exception_map_t& instantiators = getInstantiators();
        map<const string, DistilleryExceptionInstantiator*>::const_iterator ei =
          instantiators.find(e.type());
        if (ei != instantiators.end()) {
            SPCDBG(L_INFO, "instantiateAndThrow is throwing a new " << e.type(), CORE_UTILS);
            //      ei->second->throwNew(locationsb.getCharPtr(), msgsb.getCharPtr(),
            //      backtracesb.getCharPtr(), e.exceptionCode(), vec);
            ei->second->throwNew(e.location(), e.msg(), e.backtrace(), e.exceptionCode(), vec,
                                 e.exceptionAdditionalData());
        } else {
            DistilleryException ex(e.location(), e.msg(), e.backtrace(), e.exceptionCode(), vec,
                                   e.exceptionAdditionalData());
            SPCDBG(L_INFO,
                   "instantiateAndThrow is throwing a new DistilleryException(" << ex << ")",
                   CORE_UTILS);
            throw ex;
        }

#else

        DistilleryException ex(e.location(), e.msg(), e.backtrace(), e.exceptionCode(), vec,
                               e.exceptionAdditionalData());
        SPCDBG(L_INFO, "instantiateAndThrow is throwing a new DistilleryException(" << ex << ")",
               CORE_UTILS);
        throw ex;

#endif
    } // end if not NO_EXCEPTION
}

void DistilleryExceptionInstantiator::instantiateAndThrow(
  const string& type,
  const string& location,
  const string& msg,
  const string& backtrace,
  const DistilleryExceptionCode& exceptionCode,
  const vector<string>& substitutionText,
  const string& additionalData)
{
#if SPC_REGISTER_INSTANTIATORS
    exception_map_t& instantiators = getInstantiators();
    map<const string, DistilleryExceptionInstantiator*>::const_iterator ei =
      instantiators.find(type);
    if (ei != instantiators.end()) {
        SPCDBG(L_INFO, "instantiateAndThrow is throwing a new " << type, CORE_UTILS);
        ei->second->throwNew(location, msg, backtrace, exceptionCode, substitutionText,
                             additionalData);
    } else {
        DistilleryException ex(location, msg, backtrace, exceptionCode, substitutionText,
                               additionalData);
        SPCDBG(L_INFO, "instantiateAndThrow is throwing a new DistilleryException(" << ex << ")",
               CORE_UTILS);
        throw ex;
    }

#else

    DistilleryException ex(location, msg, backtrace, exceptionCode, substitutionText);

    SPCDBG(L_INFO,
           "Unable to find an instantiator for "
             << type << " (instantiators are disabled)."
             << " instantiateAndThrow is throwing a new DistilleryException(" << ex.getExplanation()
             << ")",
           CORE_UTILS);
    throw ex;

#endif
}

void DistilleryExceptionInstantiator::throwNew(SerializationBuffer& data)
{
    DistilleryException newex(data);
    SPCDBG(L_INFO,
           "throwNew is throwing a new DistilleryException(" << newex.getExplanation() << ")",
           CORE_UTILS);
    throw newex;
}

void DistilleryExceptionInstantiator::throwNew(const string& location,
                                               const string& msg,
                                               const string& backtrace,
                                               const DistilleryExceptionCode& exceptionCode,
                                               const vector<string>& substitutionText,
                                               const string& additionalData)
{
    DistilleryException newex(location, msg, backtrace, exceptionCode, substitutionText,
                              additionalData);
    SPCDBG(L_INFO,
           "throwNew is throwing a new DistilleryException(" << newex.getExplanation() << ")",
           CORE_UTILS);
    throw newex;
}

ostream& UTILS_NAMESPACE::operator<<(ostream& o, const DistilleryExceptionInstantiator& i)
{
    i.print(o);
    return o;
}

IMPL_EXCEPTION(Distillery, OOM, Distillery);
IMPL_EXCEPTION(Distillery, NewAllocatorFailure, OOM);
IMPL_EXCEPTION(Distillery, MallocFailure, OOM);
IMPL_EXCEPTION(Distillery, OutOfBound, Distillery);
IMPL_EXCEPTION(Distillery, UpperBoundError, OutOfBound);
IMPL_EXCEPTION(Distillery, InvalidOption, Distillery);
IMPL_EXCEPTION(Distillery, InvalidArgument, Distillery);
IMPL_EXCEPTION(Distillery, LogicError, Distillery);
IMPL_EXCEPTION(Distillery, Zookeeper, Distillery);
IMPL_EXCEPTION(Distillery, ZNodeNotEmpty, Zookeeper);

IMPL_EXCEPTION(Distillery, Utils, Distillery);
IMPL_EXCEPTION(Distillery, DistilleryDB, Utils);

IMPL_EXCEPTION(Distillery, DateTimeFormatError, Utils);

IMPL_EXCEPTION(Distillery, SSL, Distillery);
IMPL_EXCEPTION(Distillery, SSLSetup, Distillery);
IMPL_EXCEPTION(Distillery, SSLHandshake, Distillery);

// Local Variables: ***
// mode:c++ ***
// c-file-offsets: ((arglist-intro . ++)) ***
// End: ***
