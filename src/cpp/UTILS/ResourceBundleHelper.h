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

#ifndef RESOURCEBUNDLEHELPER_H
#define RESOURCEBUNDLEHELPER_H

#include <iostream>
#include <langinfo.h>
#include <locale>
#include <sstream>
#include <string>
#include <unicode/msgfmt.h>
#include <unicode/resbund.h>
#include <unicode/udata.h>
#include <unicode/ustream.h>
#include <vector>

#include <UTILS/SubstitutionObject.h>
#include <UTILS/SupportFunctions.h>
#include <UTILS/UTILSTypes.h>

UTILS_NAMESPACE_BEGIN

const std::string __DEFAULT_BUNDLE("StreamsAdminMessages");
const std::string __DEFAULT_ICU_DATA_DIR("/messages/"); // Relative to $STREAMS_INSTALL
const std::string __STREAMS_ICU_DATA_DIR_EV("STREAMS_ICU_DATA_DIR");
const int __DEFAULT_BUNDLE_COUNT = 7;
const std::string defaultBundles[][__DEFAULT_BUNDLE_COUNT] = {

    { "CDISA", "StreamsAdminMessages" },  { "CDISC", "StreamsInstallMessages" },
    { "CDISP", "StreamsSpadeMessages" },  { "CDISR", "StreamsRuntimeMessages" },
    { "CDISS", "StreamsStudioMessages" }, { "CDIST", "StreamsToolkitMessages" },
    { "CDISW", "StreamsWebMessages" },
};

/// This class ResourceBundleHelper will alow you to, store resouce bundle
/// related information that can be used to print messages
///
/// This class is very similar to code in st-getmsg.cpp
/// Changes made here, may be needed there.
class ResourceBundleHelper
{
  public:
    // Function declarations

    /// Constructor
    /// @param - std::string with locale name  Defualt is "".
    /// @param - std::string with resource bundle name  Defualt is "".
    ResourceBundleHelper(const std::string& locale = "",
                         const std::string& ResourceBundleName = "");

    /// Destructor
    ~ResourceBundleHelper();

    /// Get the current locale name
    /// @return - std::string with locale name
    const std::string& getLocale() const { return _locale; }

    /// Get the current resource bundle name
    /// @return - std::string with resource bundle name
    const std::string& getResourceBundleName() const { return _cachedResourceBundleName; }

    /// Get the current bundle ptr
    /// @return - ResourceBundle * set to the current resource bundle
    ResourceBundle* getBundlePtr() const { return _bundlePtr; }

    /// Set the current locale name
    /// @param - std::string with locale name
    void setLocale(const std::string& locale);

    /// Set the current resource bundle name
    /// @param - std::string with locale name
    void setResourceBundleName(const std::string& name);

    /// Reset the current ReseourceBundleHelper
    /// Use it to reset the object if an exception occurs
    ///
    void reset();

  private:
    void ClearBundle();
    std::string getStreamsICUDataDirectory();

    // class variables

    std::string _locale;
    ResourceBundle* _bundlePtr;
    std::string _cachedResourceBundleName;
}; // End ResourceBundleHelper class

/// Function to generate message UnicodeString
/// @param - ResourceBundeHelperObject for generating the message
/// @param - message id
/// @param - substitution text - vector of strings
/// @param - resource bundle name - defaults to ""
/// @prarm - bool - set to true to print the message prefix before the message
///          default is true
/// @return - UnicodeString representing the formatted message
UnicodeString PrintMessageToString(
  ResourceBundleHelper& rscBndHelper,
  const std::string& msgID,
  const std::vector<std::string>& substitutionText = std::vector<std::string>(),
  const std::string& bundleName = "",
  const bool printMessagePrefix = true);

/// Function to generate message UnicodeString
/// @param - ResourceBundeHelperObject for generating the message
/// @param - message id
/// @param - substitution text - vector of SubstitutionObjects
/// @param - resource bundle name - defaults to ""
/// @prarm - bool - set to true to print the message prefix before the message
///          default is true
/// @return - UnicodeString representing the formatted message
UnicodeString PrintMessageToString(ResourceBundleHelper& rscBndHelper,
                                   const std::string& msgid,
                                   const std::vector<SubstitutionObject>& substitutionText,
                                   const std::string& bundleName = "",
                                   const bool printMessagePrefix = true);

/// Function to generate message and print to an ostream
/// @param - an ostream object to print to
/// @param - ResourceBundeHelperObject for generating the message
/// @param - message id
/// @param - substitution text - vector of strings
/// @param - resource bundle name - defaults to ""
/// @prarm - bool - set to true to print the message prefix before the message
///          default is true
void PrintMessage(std::ostream& strm,
                  ResourceBundleHelper& rscBndHelper,
                  const std::string& msgID,
                  const std::vector<std::string>& substitutionText = std::vector<std::string>(),
                  const std::string& bundleName = "",
                  const bool printMessagePrefix = true);

/// Function to generate message and print to an osttream
/// @param - an ostream object to print to
/// @param - ResourceBundeHelperObject for generating the message
/// @param - message id
/// @param - substitution text - vector of SubstitutionObjects
/// @param - resource bundle name - defaults to ""
/// @prarm - bool - set to true to print the message prefix before the message
///          default is true
void PrintMessage(std::ostream& strm,
                  ResourceBundleHelper& rscBndHelper,
                  const std::string& msgid,
                  const std::vector<SubstitutionObject>& substitutionText,
                  const std::string& bundleName = "",
                  const bool printMessagePrefix = true);

/// Helper functions for print message functions above

/// Get default bundle
/// @param - string representing the message id
/// @return - string representing the default bundle name
std::string getDefaultBundle(const std::string& msgId);

/// Get the resource bundle
/// @param - string representing the resource bundle name
/// @param - string representing the message ID
/// @param - Reference to a ResourceBundleHelper object
void GetBundle(const std::string& bundleName,
               const std::string& messageID,
               ResourceBundleHelper& rscBndHelper);

/// Format the message and print to a UnicodeString
/// @param - Reference to a ResourceBundleHelperObject
/// @param - string representing the message ID
/// @param - int32_t representing the number of arguments
/// @param - prointer to formatted objects
/// @prarm - bool - set to true to print the message prefix before the message
///          default is true
/// @return - UnicodeString representing the formatted message
UnicodeString FormatMsgAndPrint(ResourceBundleHelper& rscBndHelper,
                                const std::string& messageID,
                                const int32_t numArgs,
                                Formattable* argPtr,
                                bool printMsgPrefix);

// a few common exceptions

DECL_EXCEPTION(UTILS_NAMESPACE, LocaleError, Utils);
DECL_EXCEPTION(UTILS_NAMESPACE, MsgBundleError, Utils);
DECL_EXCEPTION(UTILS_NAMESPACE, MsgNotFound, Utils);
DECL_EXCEPTION(UTILS_NAMESPACE, MsgFormatError, Utils);
DECL_EXCEPTION(UTILS_NAMESPACE, CannotAccessMsgDataError, Utils);

UTILS_NAMESPACE_END

#endif
