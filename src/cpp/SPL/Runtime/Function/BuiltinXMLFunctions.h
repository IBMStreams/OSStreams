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

#ifndef SPL_RUNTIME_FUNCTION_BUILTIN_XML_FUNCTIONS_H
#define SPL_RUNTIME_FUNCTION_BUILTIN_XML_FUNCTIONS_H

/*!
 * \file XMLFunctions.h \brief Definitions of C++ counterparts of %SPL native functions for
 * XML operations.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#endif /* DOXYGEN_SKIP_FOR_USERS */
#include <SPL/Runtime/Type/SPLType.h>

/*
 * Builtin XML functions
 */
/// @splcppns SPL::Functions::XML
/// @spldir XML
/// @splname SPL-XML-Functions

/*!
 * \namespace SPL::Functions::XML \brief C++ counterparts of %SPL native
 * functions that deal with XML operations
 */

namespace SPL {
namespace Functions {
namespace XML {
/*
 * General Helper Functions
 */

/// Return if this XML value contains the default (uninitialized) value.
/// @param val Input XML value to be tests.
/// @return The value 'true' if the XML value is the default (uninitialized) value.
/// @splnative <xml X> public boolean isDefaultValue (X val)
inline SPL::boolean isDefaultValue(const SPL::xml& val)
{
    return val.getValue().empty();
}

/*
 * Builtin functions to convert strings to XML
 */

/// Convert a string to XML, returning the success status and an error code.
/// @param xmlResult Result XML value, if successful.
/// @param str Input string.
/// @param error Set to 0 on success, or error status if not successful.
/// @return The value 'true' if the conversion was successful, else 'false'.
/// @splnative <xml X, string T> public boolean convertToXML (mutable X xmlResult, T str, mutable int32 error)
extern SPL::boolean convertToXML(SPL::xml& xmlResult, const std::string& str, SPL::int32& error);

/// Convert a string to XML, returning the success status and an error code.
/// @param xmlResult Result XML value, if successful.
/// @param str Input string.
/// @param error Set to 0 on success, or error status if not successful.
/// @return The value 'true' if the conversion was successful, else 'false'.
inline SPL::boolean convertToXML(SPL::xml& xmlResult, const SPL::ustring& str, SPL::int32& error)
{
    return convertToXML(xmlResult, SPL::ustring::toUTF8(str), error);
}

/// Convert a string to XML, returning the success status and an error code.
/// @param xmlResult Result XML value, if successful.
/// @param str Input string.
/// @param error Set to 0 on success, or error status if not successful.
/// @return The value 'true' if the conversion was successful, else 'false'.
inline SPL::boolean convertToXML(SPL::xml& xmlResult, const SPL::BString& str, SPL::int32& error)
{
    return convertToXML(xmlResult, std::string(str.getCString()), error);
}

/// Convert a string to XML, returning the success status.
/// @param xmlResult Result XML value, if successful.
/// @param str Input string.
/// @return The value 'true' if the conversion was successful, else 'false'.
/// @splnative <xml X, string T> public boolean convertToXML (mutable X xmlResult, T str)
extern SPL::boolean convertToXML(SPL::xml& xmlResult, const std::string& str);

/// Convert a string to XML, returning the success status.
/// @param xmlResult Result XML value, if successful.
/// @param str Input string.
/// @return The value 'true' if the conversion was successful, else 'false'.
inline SPL::boolean convertToXML(SPL::xml& xmlResult, const SPL::ustring& str)
{
    return convertToXML(xmlResult, SPL::ustring::toUTF8(str));
}

/// Convert a string to XML, returning the success status.
/// @param xmlResult Result XML value, if successful.
/// @param str Input string.
/// @return The value 'true' if the conversion was successful, else 'false'.
inline SPL::boolean convertToXML(SPL::xml& xmlResult, const SPL::BString& str)
{
    return convertToXML(xmlResult, std::string(str.getCString()));
}

/*
 * XQuery support
 */

/// Execute an XQuery program against an XML value and return a list of results.
///
/// Starting with Streams v4.2.1.1, the xquery functions use the default namespace of the queried XML.
/// If the xquery() function is given an xqueryExpression without a namespace declaration,
/// then it returns an empty result if the queried document has an explicit namespace.
/// @spldesc
/// @spldesc For the following example data:
/// @spldesc
/// @spldesc <Orders xmlns="http://example.com">
/// @spldesc <Transaction TxID='100' TxType='return'>
/// @spldesc <Item count='3'>Shorts</Item>
/// @spldesc </Transaction>
/// @spldesc </Orders>
/// @spldesc
/// @spldesc The xqueryExpression "declare default element namespace http://example.com ; /Orders/Transaction[@TxType = 'return']/Item/text()" would correctly return the string "Shorts".
/// @param value XML value to be queried.
/// @param xqueryExpression XQuery program to be executed.
/// @return List of string results from running the program against the XML value.
/// @splnative <xml X> public list<rstring> xquery (X value, rstring xqueryExpression)
SPL::list<SPL::rstring> xquery(const SPL::xml& value, const SPL::rstring& xqueryExpression);

/// Execute an XQuery program against an XML value and return a list of results.
///
/// Starting with Streams v4.2.1.1, the xquery functions use the default namespace of the queried XML.
/// If the xquery() function is given an xqueryExpression without a namespace declaration,
/// then it returns an empty result if the queried document has an explicit namespace.
/// @param value XML value to be queried.
/// @param xqueryExpression XQuery program to be executed.
/// @return List of string results from running the program against the XML value.
/// @splnative <xml X> public list<ustring> xquery (X value, ustring xqueryExpression)
SPL::list<SPL::ustring> xquery(const SPL::xml& value, const SPL::ustring& xqueryExpression);

/// Execute an XQuery program against an XML value and return a list of results.
///
/// Starting with Streams v4.2.1.1, the xquery functions use the default namespace of the queried XML.
/// If the xquery() function is given an xqueryExpression without a namespace declaration,
/// then it returns an empty result if the queried document has an explicit namespace.
/// @param value XML value to be queried.
/// @param xqueryExpression XQuery program to be executed.
/// @param xqueryVars XQuery variable definitions.
/// @return List of string results from running the program against the XML value.
/// @splnative <xml X> public list<rstring> xquery (X value, rstring xqueryExpression, map<rstring, rstring> xqueryVars)
SPL::list<SPL::rstring> xquery(const SPL::xml& value,
                               const SPL::rstring& xqueryExpression,
                               const SPL::map<SPL::rstring, SPL::rstring>& xqueryVars);

/// Execute an XQuery program against an XML value and return a list of results.
///
/// Starting with Streams v4.2.1.1, the xquery functions use the default namespace of the queried XML.
/// If the xquery() function is given an xqueryExpression without a namespace declaration,
/// then it returns an empty result if the queried document has an explicit namespace.
/// @param value XML value to be queried.
/// @param xqueryExpression XQuery program to be executed.
/// @param xqueryVars XQuery variable definitions.
/// @return List of string results from running the program against the XML value.
/// @splnative <xml X> public list<ustring> xquery (X value, ustring xqueryExpression, map<ustring, ustring> xqueryVars)
SPL::list<SPL::ustring> xquery(const SPL::xml& value,
                               const SPL::ustring& xqueryExpression,
                               const SPL::map<SPL::ustring, SPL::ustring>& xqueryVars);

/// Execute an XQuery program against an XML value and return a list of results.
///
/// Starting with Streams v4.2.1.1, the xquery functions use the default namespace of the queried XML.
/// If the xquery() function is given an xqueryExpression without a namespace declaration,
/// then it returns an empty result if the queried document has an explicit namespace.
/// @param value XML value to be queried.
/// @param xqueryExpression XQuery program to be executed.
/// @param error Set to 0 on success, or error status if not successful.
/// @return List of string results from running the program against the XML value.
/// @splnative <xml X> public list<rstring> xquery (X value, rstring xqueryExpression, mutable int32 error)
SPL::list<SPL::rstring> xquery(const SPL::xml& value,
                               const SPL::rstring& xqueryExpression,
                               SPL::int32& error);

/// Execute an XQuery program against an XML value and return a list of results.
///
/// Starting with Streams v4.2.1.1, the xquery functions use the default namespace of the queried XML.
/// If the xquery() function is given an xqueryExpression without a namespace declaration,
/// then it returns an empty result if the queried document has an explicit namespace.
/// @param value XML value to be queried.
/// @param xqueryExpression XQuery program to be executed.
/// @param error Set to 0 on success, or error status if not successful.
/// @return List of string results from running the program against the XML value.
/// @splnative <xml X> public list<ustring> xquery (X value, ustring xqueryExpression, mutable int32 error)
SPL::list<SPL::ustring> xquery(const SPL::xml& value,
                               const SPL::ustring& xqueryExpression,
                               SPL::int32& error);

/// Execute an XQuery program against an XML value and return a list of results.
///
/// Starting with Streams v4.2.1.1, the xquery functions use the default namespace of the queried XML.
/// If the xquery() function is given an xqueryExpression without a namespace declaration,
/// then it returns an empty result if the queried document has an explicit namespace.
/// @param value XML value to be queried.
/// @param xqueryExpression XQuery program to be executed.
/// @param xqueryVars XQuery variable definitions.
/// @param error Set to 0 on success, or error status if not successful.
/// @return List of string results from running the program against the XML value.
/// @splnative <xml X> public list<rstring> xquery (X value, rstring xqueryExpression, map<rstring, rstring> xqueryVars, mutable int32 error)
SPL::list<SPL::rstring> xquery(const SPL::xml& value,
                               const SPL::rstring& xqueryExpression,
                               const SPL::map<SPL::rstring, SPL::rstring>& xqueryVars,
                               SPL::int32& error);

/// Execute an XQuery program against an XML value and return a list of results.
///
/// Starting with Streams v4.2.1.1, the xquery functions use the default namespace of the queried XML.
/// If the xquery() function is given an xqueryExpression without a namespace declaration,
/// then it returns an empty result if the queried document has an explicit namespace.
/// @param value XML value to be queried.
/// @param xqueryExpression XQuery program to be executed.
/// @param xqueryVars XQuery variable definitions.
/// @param error Set to 0 on success, or error status if not successful.
/// @return List of string results from running the program against the XML value.
/// @splnative <xml X> public list<ustring> xquery (X value, ustring xqueryExpression, map<ustring, ustring> xqueryVars, mutable int32 error)
SPL::list<SPL::ustring> xquery(const SPL::xml& value,
                               const SPL::ustring& xqueryExpression,
                               const SPL::map<SPL::ustring, SPL::ustring>& xqueryVars,
                               SPL::int32& error);

}
}
}

#endif /* SPL_RUNTIME_FUNCTION_BUILTIN_XML_FUNCTIONS_H */
