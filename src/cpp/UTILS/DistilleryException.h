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

//----------------------------------------------------------------------------
//
// Title-
//       DistilleryException.h
//
// Purpose-
//       Exception-handling convenience routines
//
//----------------------------------------------------------------------------
#ifndef _DISTILLERYEXCEPTIONS_H_
#define _DISTILLERYEXCEPTIONS_H_

#include "DistilleryExceptionCode.h"
#include "UTILSTypes.h"
#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <cassert>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#define SPC_REGISTER_INSTANTIATORS 1

// forward declare class from Exception.h
namespace xmlns {
namespace prod {
namespace streams {
namespace runtime {
namespace utils {
class additionalExceptionDataType;
}
}
}
}
}

UTILS_NAMESPACE_BEGIN

// forward declaration
class SerializationBuffer;

//---------------------------------------------------------------------------
// Distillery Exception base class
//----------------------------------------------------------------------------
class DistilleryExceptionInstantiator;

typedef std::map<const std::string, DistilleryExceptionInstantiator*> exception_map_t;

/**
   This is the root class for all distillery exception

*/
class DistilleryException : public std::exception
{
  public:
    static const std::string NO_EXCEPTION;

    /// Constructor
    /// @param _location location of the error, typically the function, file and line
    /// @param _msg a message associate to the exception when possible
    /// @param _backtrace the execution stack at the time where the
    /// exception was thrown (if not provided, it is computed
    /// automatically)
    /// @param _exceptionCode substitution text strings related to the exception
    /// code
    /// @param _substitutionText substitution text strings related to the
    /// @param _additionalData  xml string containing additional data related to the cause of the
    /// exception exception code
    DistilleryException(
      const std::string& _location,
      const std::string& _msg,
      const std::string& _backtrace = "",
      const DistilleryExceptionCode& _exceptionCode =
        DistilleryExceptionCode(DistilleryExceptionCode::NO_MESSAGE_ID),
      const std::vector<std::string>& _substitutionText = std::vector<std::string>(),
      const std::string& _additionalData = "");

    /// Constructor
    /// @param _location location of the error, typically the function, file and line
    /// @param _msg a message associate to the exception when possible
    /// @param _cause the exception that caused this new exception to be thrown
    /// @param _exceptionCode substitution text strings related to the exception code
    /// @param _substitutionText substitution text strings related to the
    /// @param _additionalData  xml string containing additional data related to the cause of the
    /// exception exception code
    DistilleryException(
      const std::string& _location,
      const std::string& _msg,
      const DistilleryException& _cause,
      const DistilleryExceptionCode& _exceptionCode =
        DistilleryExceptionCode(DistilleryExceptionCode::NO_MESSAGE_ID),
      const std::vector<std::string>& _substitutionText = std::vector<std::string>(),
      const std::string& _additionalData = "");

    /// Copy constructor
    /// @param e reference to another exception
    DistilleryException(const DistilleryException& e);

    /// Construct an exception from a serialized version
    /// @param data reference to the serilized buffer containing the exception
    DistilleryException(SerializationBuffer& data);

    /// Construct an exception from an XML string
    /// @param xmlString reference to the XML string containing the exception
    DistilleryException(const std::string& xmlString);

    /// Serialize an existing exception to a buffer
    /// @param s reference to the buffer when the exception will be written
    virtual void serialize(SerializationBuffer& s) const;

    /// Return the type of the exception (ie the name of the object)
    virtual const char* getType(void) const;

    /// Return the explanation associated with this exception, as
    /// supplied in the constuctor
    virtual const std::string& getExplanation(void) const;

    /// Return the location associated with this exception, as supplied
    /// in the constuctor
    virtual const std::string& getLocation(void) const;

    /// Return a dump of the stack at the time the exception was thrown,
    /// when the GCC-specific backtrace function is available
    virtual const std::string& getBacktrace(void) const;

    /// Return the exception code associated with this exception.
    /// @return int64_t A reference to the exception code
    virtual const DistilleryExceptionCode& getExceptionCode(void) const;
    virtual void setMessageID(const char* msgId);

    /// Return the substitution text strings associated with the exception code.
    /// @return vector<string> A reference to the vector of substitution text
    /// strings
    virtual const std::vector<std::string>& getSubstitutionText(void) const;
    virtual void setSubstitutionText(const std::vector<std::string>& replacements);

    /// Return true if there is additional data associated with the exception code.
    /// @return true if there is additional data associated with the exception code.
    virtual bool hasAdditionalData(void) const;

    /// Return the additional data string associated with the exception code.
    /// @return string A reference to the additional data string (length==0 when
    /// hasAdditionalData()==false)
    virtual const std::string& getAdditionalDataXML(void) const;

    /// Return the additional data string associated with the exception code.
    /// @return additionalExceptionDataType the object form of the additional data
    /// @throws std::runtime_error if hasAdditionalData()==false
    virtual ::xmlns::prod::streams::runtime::utils::additionalExceptionDataType getAdditionalData(
      void) const;

    /// Returns the entire message information about this exception.
    /// This includes the primary message, along with the additional data
    /// @return additionalExceptionDataType the object form of the complete message
    virtual ::xmlns::prod::streams::runtime::utils::additionalExceptionDataType getMessageInfo(
      void) const;

    /// Set the additional exception data into the exception
    /// @param str XML string for the additional data
    virtual void setAdditionalData(const std::string xml);

    /// Set the additional exception data into the exception
    /// @param additionalExceptionDataType reference to an additionalExceptionDataType
    virtual void setAdditionalData(
      const ::xmlns::prod::streams::runtime::utils::additionalExceptionDataType& obj);

    /// Write a description of the exception to an output stream (name,
    /// message and location only)
    /// @param o a reference to the C++ stream to write to
    virtual void printShort(std::ostream& o) const;

    /// Write a description of the exception to an output stream (name,
    /// message, location and transation information)
    /// @param o a reference to the C++ stream to write to
    virtual void printMedium(std::ostream& o) const;

    /// Write a description of the exception to an output stream (name,
    /// message, location, transation information and backtrace)
    /// @param o a reference to the C++ stream to write to
    virtual void printLong(std::ostream& o) const;

    /// Export the XML version of this object
    /// @param str output stream holding the XML version of the object
    virtual void toXML(std::stringstream& str) const;

    /// Export the XML version of this object
    /// @return an XML'ized string representation of the exception
    virtual const std::string toXMLString(void) const;

    // Set variable arguments in class - DistilleryException Code and
    //  substitution text
    // @param variable argument list
    virtual void setVariableArgs(int parm1, ...);

    // Set variable arguments in class - DistilleryException Code and
    //  substitution text
    // @param variable argument list
    virtual void setVariableArgs(va_list vl);

    /// Destructor
    virtual ~DistilleryException(void) throw();

    /// Return the type of the exception
    virtual const char* what() const throw();

    /// static function for THROW(Distrillery,...)
    static void throwWithContext(const char* msg,
                                 const char* funcName,
                                 const char* fileName,
                                 int lineNum,
                                 ...) __attribute__((__noreturn__));
    static void throwWithContextAndMsgs(const char* msg,
                                        const char* funcName,
                                        const char* fileName,
                                        int lineNum,
                                        const char* msgKey,
                                        const std::vector<std::string>& replacements)
      __attribute__((__noreturn__));

    /// add a DistilleryException object to an additionalExceptionDataType object,
    /// So can be used to wrap with new exception.
    static void addToAdditionalExceptionData(
      ::xmlns::prod::streams::runtime::utils::additionalExceptionDataType& obj,
      const DistilleryException& e);

  protected:
    std::string location;
    std::string msg;
    std::string backtrace;

    DistilleryExceptionCode exceptionCode;
    std::vector<std::string> substitutionText;

    std::string additionalData;

    friend std::ostream& operator<<(std::ostream& o, const DistilleryException& e);
    friend class DistilleryExceptionInstantiator;
};

class DistilleryExceptionInstantiator
{

#if SPC_REGISTER_INSTANTIATORS

  private:
    static exception_map_t& getInstantiators();

#endif

  public:
    DistilleryExceptionInstantiator(const char* type);
    virtual const char* getType(void) const;

    static void registerExceptionInstantiator(DistilleryExceptionInstantiator* new_instantiator,
                                              const char* type);
    static void instantiateAndThrow(SerializationBuffer& data);
    static void instantiateAndThrow(const std::string& xmlString);
    static void print(std::ostream& o);
    static void instantiateAndThrow(
      const std::string& type,
      const std::string& location,
      const std::string& msg,
      const std::string& backtrace,
      const DistilleryExceptionCode& exceptionCode =
        DistilleryExceptionCode(DistilleryExceptionCode::NO_MESSAGE_ID),
      const std::vector<std::string>& substitutionText = std::vector<std::string>(),
      const std::string& additionalData = "");

    virtual void throwNew(SerializationBuffer& data);
    virtual void throwNew(
      const std::string& location,
      const std::string& msg,
      const std::string& backtrace,
      const DistilleryExceptionCode& exceptionCode =
        DistilleryExceptionCode(DistilleryExceptionCode::NO_MESSAGE_ID),
      const std::vector<std::string>& substitutionText = std::vector<std::string>(),
      const std::string& additionalData = "");
    virtual ~DistilleryExceptionInstantiator(void);

    friend std::ostream& operator<<(std::ostream& o, const DistilleryExceptionInstantiator& e);
};

/// Convert an additionalExceptionDataType object to its corresponding XML string
/// @param additionalData the object
/// @return string XML string
std::string toString(
  const ::xmlns::prod::streams::runtime::utils::additionalExceptionDataType& additionalData);

/// Create an additionalExceptionDataType from an XML representation string
/// @param additionalData the object
/// @param s the XML string representation
void fromString(::xmlns::prod::streams::runtime::utils::additionalExceptionDataType& additionalData,
                const std::string& s);

UTILS_NAMESPACE_END

// convenience macro for throwing exceptions -- ensures that method name,
// filename, and line number are added to the error messages (for debugging
// purposes).

#ifndef THROW
#define EX_LOCATION(var)                                                                           \
    std::stringstream var;                                                                         \
    var << "'" << __PRETTY_FUNCTION__ << "'"                                                       \
        << " [" << __FILE__ << ":" << __LINE__ << "]";

// Macro for throwing exception. Variable length parameter list is for
// passing a message id to be set in the DistilleryExceptionCode
// object and the variable parameters after that are char *'s for each
// substitution text variable.
// NOTE: __my_ex_strm cannot be pushed into the function
//       since some invocation cases look like:
//       THROW("blabla" << someStringVar,...);

// keep old throw here to investigate perf regression on rh 5.5
#if 0

#define THROW(type, msg, ...)                                                                      \
    do {                                                                                           \
        SPCDBG(L_INFO, "Throwing Exception: " << #type << " (Msg: " << msg << ")", "ex");          \
        std::stringstream __my_ex_strm;                                                            \
        __my_ex_strm << msg;                                                                       \
        EX_LOCATION(__my_ex_location);                                                             \
        type##Exception __my_ex(__my_ex_location.str().c_str(), __my_ex_strm.str().c_str());       \
        __my_ex.setVariableArgs(0, ##__VA_ARGS__, NULL);                                           \
        throw __my_ex;                                                                             \
    } while (0)

#else

// new THROW macro
#define THROW(type, msg, ...)                                                                      \
    do {                                                                                           \
        std::stringstream __my_ex_strm;                                                            \
        __my_ex_strm << msg;                                                                       \
        type##Exception::throwWithContext(__my_ex_strm.str().c_str(), __PRETTY_FUNCTION__,         \
                                          __FILE__, __LINE__, ##__VA_ARGS__, NULL);                \
    } while (0)

#endif

#define THROW_MSG(type, msg, msgKey, substitutionTexts)                                            \
    do {                                                                                           \
        std::stringstream __my_ex_strm;                                                            \
        __my_ex_strm << msg;                                                                       \
        type##Exception::throwWithContextAndMsgs(__my_ex_strm.str().c_str(), __PRETTY_FUNCTION__,  \
                                                 __FILE__, __LINE__, (msgKey),                     \
                                                 (substitutionTexts));                             \
    } while (0)

// Macro for throwing exception. Variable length parameter list is for
// passing a message id to be set in the DistilleryExceptionCode
// object and the variable parameters after that are char *'s for each
// substitution text variable.
// NOTE: In this case we  assume that the msg is a char *
//       with no concatenation
#define THROW_CHAR(type, msg, ...)                                                                 \
    type##Exception::throwWithContext((const char*)(msg), __PRETTY_FUNCTION__, __FILE__, __LINE__, \
                                      ##__VA_ARGS__, NULL)

// Macro for throwing exception. Variable length parameter list is for
// passing a message id to be set in the DistilleryExceptionCode
// object and the variable parameters after that are char *'s for each
// substitution text variable.
// NOTE: In this case we  assume that the msg is a simple string
//       with no concatenation
#define THROW_STRING(type, msg, ...)                                                               \
    type##Exception::throwWithContext(((const std::string)(msg)).c_str(), __PRETTY_FUNCTION__,     \
                                      __FILE__, __LINE__, ##__VA_ARGS__, NULL)

// Macro for throwing a nested exception. Variable length parameter
// list is for passing a message id to be set in the
// DistilleryExceptionCode object and the variable parameters after
// that are char *'s for each substitution text variable.
#define THROW_NESTED(type, msg, ex, ...)                                                           \
    do {                                                                                           \
        SPCDBG(L_INFO,                                                                             \
               "Throwing Exception: " << #type << " (Msg: " << msg                                 \
                                      << " Nested Ex: " << ex.getExplanation() << ")",             \
               DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);                                              \
        std::stringstream __my_ex_strm;                                                            \
        __my_ex_strm << msg;                                                                       \
        EX_LOCATION(__my_ex_location);                                                             \
        type##Exception __my_ex(__my_ex_location.str().c_str(), __my_ex_strm.str(), ex);           \
        __my_ex.setVariableArgs(0, ##__VA_ARGS__, NULL);                                           \
        throw __my_ex;                                                                             \
    } while (0)

// Macro for throwing exception with additional message information.
// Variable length parameter list is for
// passing a message id to be set in the DistilleryExceptionCode
// object and the variable parameters after that are char *'s for each
// substitution text variable.
#define THROW_WITH_ADDITIONAL_DATA(type, msg, adddata, ...)                                        \
    do {                                                                                           \
        SPCDBG(L_INFO, "Throwing Exception: " << #type << " (Msg: " << msg << ")",                 \
               DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);                                              \
        std::stringstream __my_ex_strm;                                                            \
        __my_ex_strm << msg;                                                                       \
        EX_LOCATION(__my_ex_location);                                                             \
        type##Exception __my_ex(__my_ex_location.str().c_str(), __my_ex_strm.str().c_str());       \
        __my_ex.setVariableArgs(0, ##__VA_ARGS__, NULL);                                           \
        __my_ex.setAdditionalData(adddata);                                                        \
        throw __my_ex;                                                                             \
    } while (0)

// If assert(expr) fails, generates the following message and terminates the
// process: <source_file>:<line_number>:<function name>: Assertion `'expr' (msg)' failed.
//
#ifdef NDEBUG
#define ASSERT_MSG(expr, msg) (__ASSERT_VOID_CAST(0))
#else
#define ASSERT_MSG(expr, msg)                                                                      \
    do {                                                                                           \
        std::stringstream __my_ex_strm;                                                            \
        __my_ex_strm << '\'' << __STRING(expr) << "' (" << msg << ")";                             \
        ((expr)                                                                                    \
           ? __ASSERT_VOID_CAST(0)                                                                 \
           : __assert_fail(__my_ex_strm.str().c_str(), __FILE__, __LINE__, __ASSERT_FUNCTION));    \
    } while (0)
#endif

#endif

// macro for declaring an exception and establishing a parentage relationship

#ifndef DECL_EXCEPTION

#define DECL_EXCEPTION(NameSpace, Type, SuperType)                                                 \
    class Type##Exception : public SuperType##Exception                                            \
    {                                                                                              \
      public:                                                                                      \
        Type##Exception(                                                                           \
          const std::string& myLocation,                                                           \
          const std::string& myMsg,                                                                \
          const UTILS_NAMESPACE::DistilleryExceptionCode& exceptionCode =                          \
            UTILS_NAMESPACE::DistilleryExceptionCode(                                              \
              UTILS_NAMESPACE::DistilleryExceptionCode::NO_MESSAGE_ID),                            \
          const std::vector<std::string>& mySubstitutionText = std::vector<std::string>(),         \
          const std::string& myAdditionalData = "");                                               \
        Type##Exception(                                                                           \
          const std::string& myLocation,                                                           \
          const std::string& myMsg,                                                                \
          const std::string& myBacktrace,                                                          \
          const UTILS_NAMESPACE::DistilleryExceptionCode& exceptionCode =                          \
            UTILS_NAMESPACE::DistilleryExceptionCode(                                              \
              UTILS_NAMESPACE::DistilleryExceptionCode::NO_MESSAGE_ID),                            \
          const std::vector<std::string>& mySubstitutionText = std::vector<std::string>(),         \
          const std::string& myAdditionalData = "");                                               \
        Type##Exception(                                                                           \
          const std::string& myLocation,                                                           \
          const std::string& myMsg,                                                                \
          const UTILS_NAMESPACE::DistilleryException& myCause,                                     \
          const UTILS_NAMESPACE::DistilleryExceptionCode& exceptionCode =                          \
            UTILS_NAMESPACE::DistilleryExceptionCode(                                              \
              UTILS_NAMESPACE::DistilleryExceptionCode::NO_MESSAGE_ID),                            \
          const std::vector<std::string>& mySubstitutionText = std::vector<std::string>(),         \
          const std::string& myAdditionalData = "");                                               \
        virtual ~Type##Exception(void) throw();                                                    \
        Type##Exception(UTILS_NAMESPACE::SerializationBuffer& data);                               \
        virtual const char* getType(void) const;                                                   \
        static void throwWithContext(const char* msg,                                              \
                                     const char* funcName,                                         \
                                     const char* fileName,                                         \
                                     int lineNum,                                                  \
                                     ...) __attribute__((__noreturn__));                           \
    };                                                                                             \
    class Type##ExceptionInstantiator : public SuperType##ExceptionInstantiator                    \
    {                                                                                              \
      public:                                                                                      \
        Type##ExceptionInstantiator(const char* type);                                             \
        virtual ~Type##ExceptionInstantiator(void);                                                \
                                                                                                   \
      protected:                                                                                   \
        virtual const char* getType(void) const;                                                   \
        virtual void throwNew(UTILS_NAMESPACE::SerializationBuffer& data);                         \
        virtual void throwNew(                                                                     \
          const std::string& location,                                                             \
          const std::string& msg,                                                                  \
          const std::string& backtrace,                                                            \
          const UTILS_NAMESPACE::DistilleryExceptionCode& exceptionCode =                          \
            UTILS_NAMESPACE::DistilleryExceptionCode(                                              \
              UTILS_NAMESPACE::DistilleryExceptionCode::NO_MESSAGE_ID),                            \
          const std::vector<std::string>& mySubstitutionText = std::vector<std::string>(),         \
          const std::string& myAdditionalData = "");                                               \
    };

#define IMPL_EXCEPTION(NameSpace, Type, SuperType)                                                 \
    Type##Exception::Type##Exception(                                                              \
      const std::string& myLocation, const std::string& myMsg,                                     \
      const UTILS_NAMESPACE::DistilleryExceptionCode& myExceptionCode,                             \
      const std::vector<std::string>& mySubstitutionText, const std::string& myAdditionalData)     \
      : SuperType##Exception(myLocation, myMsg, "", myExceptionCode, mySubstitutionText,           \
                             myAdditionalData)                                                     \
    {}                                                                                             \
    Type##Exception::Type##Exception(                                                              \
      const std::string& myLocation, const std::string& myMsg, const std::string& myBacktrace,     \
      const UTILS_NAMESPACE::DistilleryExceptionCode& myExceptionCode,                             \
      const std::vector<std::string>& mySubstitutionText, const std::string& myAdditionalData)     \
      : SuperType##Exception(myLocation, myMsg, myBacktrace, myExceptionCode, mySubstitutionText,  \
                             myAdditionalData)                                                     \
    {}                                                                                             \
    Type##Exception::Type##Exception(                                                              \
      const std::string& myLocation, const std::string& myMsg,                                     \
      const UTILS_NAMESPACE::DistilleryException& myCause,                                         \
      const UTILS_NAMESPACE::DistilleryExceptionCode& myExceptionCode,                             \
      const std::vector<std::string>& mySubstitutionText, const std::string& myAdditionalData)     \
      : SuperType##Exception(myLocation, myMsg, myCause, myExceptionCode, mySubstitutionText,      \
                             myAdditionalData)                                                     \
    {}                                                                                             \
    Type##Exception::~Type##Exception(void) throw() {}                                             \
    Type##Exception::Type##Exception(UTILS_NAMESPACE::SerializationBuffer& data)                   \
      : SuperType##Exception(data)                                                                 \
    {}                                                                                             \
    const char* Type##Exception::getType(void) const { return #NameSpace "::" #Type "Exception"; } \
    void Type##Exception::throwWithContext(const char* msg, const char* funcName,                  \
                                           const char* fileName, int lineNum, ...)                 \
    {                                                                                              \
        SPCDBG_TST(L_INFO, {                                                                       \
            std::stringstream __my_ex_strmx;                                                       \
            __my_ex_strmx << "Throwing Exception: " << #Type << " (Msg: " << msg << ")";           \
            Distillery::debug::write_msg(L_INFO, DEBUG_NAMESPACE_QUALIFIER CORE_UTILS, "",         \
                                         funcName, fileName, lineNum, __my_ex_strmx.str());        \
        });                                                                                        \
        std::stringstream __my_ex_strm;                                                            \
        __my_ex_strm << msg;                                                                       \
        std::stringstream __my_ex_location;                                                        \
        __my_ex_location << "'" << funcName << "'"                                                 \
                         << " [" << fileName << ":" << lineNum << "]";                             \
        Type##Exception __my_ex(__my_ex_location.str().c_str(), __my_ex_strm.str().c_str());       \
        va_list __my_args;                                                                         \
        va_start(__my_args, lineNum);                                                              \
        __my_ex.setVariableArgs(__my_args);                                                        \
        va_end(__my_args);                                                                         \
        throw __my_ex;                                                                             \
    }                                                                                              \
    Type##ExceptionInstantiator::Type##ExceptionInstantiator(const char* type)                     \
      : SuperType##ExceptionInstantiator(type)                                                     \
    {}                                                                                             \
    Type##ExceptionInstantiator::~Type##ExceptionInstantiator(void) {}                             \
    const char* Type##ExceptionInstantiator::getType(void) const                                   \
    {                                                                                              \
        return #NameSpace "::" #Type "Exception";                                                  \
    }                                                                                              \
    void Type##ExceptionInstantiator::throwNew(UTILS_NAMESPACE::SerializationBuffer& data)         \
    {                                                                                              \
        Type##Exception newex(data);                                                               \
        SPCDBG(L_INFO,                                                                             \
               "throwNew is throwing a new " #Type "Exception(" << newex.getExplanation() << ")",  \
               DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);                                              \
        throw newex;                                                                               \
    }                                                                                              \
    void Type##ExceptionInstantiator::throwNew(                                                    \
      const std::string& location, const std::string& msg, const std::string& backtrace,           \
      const UTILS_NAMESPACE::DistilleryExceptionCode& exceptionCode,                               \
      const std::vector<std::string>& substitutionText, const std::string& myAdditionalData)       \
    {                                                                                              \
        Type##Exception newex(location, msg, backtrace, exceptionCode, substitutionText,           \
                              myAdditionalData);                                                   \
        SPCDBG(L_INFO,                                                                             \
               "throwNew is throwing a new " #Type "Exception(" << newex.getExplanation() << ")",  \
               DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);                                              \
        throw newex;                                                                               \
    }                                                                                              \
    static Type##ExceptionInstantiator __my_private_exception_instantiator_##Type(                 \
      #NameSpace "::" #Type "Exception");

#endif

UTILS_NAMESPACE_BEGIN

// a few common exceptions

DECL_EXCEPTION(UTILS_NAMESPACE, OOM, Distillery);
DECL_EXCEPTION(UTILS_NAMESPACE, NewAllocatorFailure, OOM);
DECL_EXCEPTION(UTILS_NAMESPACE, MallocFailure, OOM);
DECL_EXCEPTION(UTILS_NAMESPACE, OutOfBound, Distillery);
DECL_EXCEPTION(UTILS_NAMESPACE, UpperBoundError, OutOfBound);
DECL_EXCEPTION(UTILS_NAMESPACE, InvalidOption, Distillery);
DECL_EXCEPTION(UTILS_NAMESPACE, InvalidArgument, Distillery);
DECL_EXCEPTION(UTILS_NAMESPACE, LogicError, Distillery);
DECL_EXCEPTION(UTILS_NAMESPACE, Zookeeper, Distillery);
DECL_EXCEPTION(UTILS_NAMESPACE, ZNodeNotEmpty, Zookeeper);

DECL_EXCEPTION(UTILS_NAMESPACE, Utils, Distillery);
DECL_EXCEPTION(UTILS_NAMESPACE, DistilleryDB, Utils);

DECL_EXCEPTION(UTILS_NAMESPACE, DateTimeFormatError, Utils);

// SSL connection set up related exceptions
DECL_EXCEPTION(UTILS_NAMESPACE, SSL, Distillery);
DECL_EXCEPTION(UTILS_NAMESPACE, SSLSetup, Distillery);
DECL_EXCEPTION(UTILS_NAMESPACE, SSLHandshake, Distillery);

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// c-file-offsets: ((arglist-intro . ++)) ***
// End: ***
