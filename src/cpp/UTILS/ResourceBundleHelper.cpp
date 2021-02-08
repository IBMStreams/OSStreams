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

#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/ResourceBundleHelper.h>
#include <UTILS/SupportFunctions.h>
#include <UTILS/UTILSTypes.h>

using namespace std;
U_NAMESPACE_USE
UTILS_NAMESPACE_USE;
DEBUG_NAMESPACE_USE

// Define embedded sentinel class to watch over the Formattable object and
// ensure it gets deallocated.
class formattableArgumentSentinel
{
  public:
    formattableArgumentSentinel()
      : _fArgPtr(NULL){};

    Formattable* getStorage(int32_t numberOfArgs)
    {
        _fArgPtr = new Formattable[numberOfArgs];
        return _fArgPtr;
    };
    ~formattableArgumentSentinel()
    {
        if (_fArgPtr != NULL) {
            Formattable* tempPtr = _fArgPtr;
            _fArgPtr = NULL;
            delete[] tempPtr;
        }
    };

  protected:
    Formattable* _fArgPtr;

  private:
};

// Constructor
ResourceBundleHelper::ResourceBundleHelper(const string& locale, const string& ResourceBundleName)
  : _bundlePtr(NULL)
{
    UErrorCode status = U_ZERO_ERROR;

    // set the active locale to the server locale
    setlocale(LC_ALL, "");

    // Register the location of the Streams ICU Data Directory
    // (location of the resource bundles)
    u_setDataDirectory(getStreamsICUDataDirectory().c_str());
    if (U_SUCCESS(status)) {
        SPCDBG(L_DEBUG, "Streams ICU data directory set to " << u_getDataDirectory(), CORE_UTILS);
    } else {
        SPCDBG(L_ERROR,
               "Streams ICU data directory " << getStreamsICUDataDirectory()
                                             << " cannot be registered.",
               CORE_UTILS);
        THROW(CannotAccessMsgDataError,
              "Streams ICU data directory " << getStreamsICUDataDirectory()
                                            << " cannot be registered.",
              UTILSCannotAccessMsgDataError, getStreamsICUDataDirectory().c_str());
    }

    setLocale(locale);

    if (!(ResourceBundleName.empty())) {
        setResourceBundleName(ResourceBundleName);
    }
};

void ResourceBundleHelper::setLocale(const string& locale)
{
    _locale = setlocale(LC_ALL, locale.c_str());
    if (_locale.empty()) {
        THROW(LocaleError, "Locale not set", UTILSLocaleError);
    }
};

void ResourceBundleHelper::setResourceBundleName(const string& name)
{

    UErrorCode status = U_ZERO_ERROR;

    ClearBundle();

    // This ctor used to specify the locale too
    //  _bundlePtr = new ResourceBundle(name.c_str(), _locale.c_str(), status);
    // but since the latest ICU (REL 6.x) was used, this fails if locale is inconsistent
    // see RTC 19625
    _bundlePtr = new ResourceBundle(name.c_str(), status);

    if (U_SUCCESS(status)) {
        _cachedResourceBundleName.assign(_bundlePtr->getName());

        SPCDBG(L_DEBUG, "Bundle " << _bundlePtr->getName() << " opened.", CORE_UTILS);
    } else {
        SPCDBG(L_ERROR, "Bundle " << name << " failed in setResourceBundleName. STATUS= " << status,
               CORE_UTILS);
        ClearBundle();
        THROW(MsgBundleError, "Message bundling error.", UTILSMsgBundleError);
    }
};

// Get the Streams ICU Data directory. By default, the <install>/messages
// directory is used unless it is overriden by the STREAMS_ICU_DATA_DIR
// environment variable.
string ResourceBundleHelper::getStreamsICUDataDirectory()
{
    const char* streamsDataDirEv = getenv(__STREAMS_ICU_DATA_DIR_EV.c_str());
    if (streamsDataDirEv != NULL) {
        return streamsDataDirEv;
    }
    const char* install =
      getenv(UTILS_NAMESPACE::SystemStatics::get_STREAMS_INSTALL_envVar().c_str());
    if (install != NULL) {
        string dataDir(install);
        dataDir.append(__DEFAULT_ICU_DATA_DIR.c_str());
        return dataDir.c_str();
    }
    return "";
}

// Free and clear the stored bundle ptr
void ResourceBundleHelper::ClearBundle()
{
    ResourceBundle* TempBundlePtr;

    _cachedResourceBundleName.assign("");

    if (_bundlePtr != NULL) {
        TempBundlePtr = _bundlePtr;
        _bundlePtr = NULL;
        delete TempBundlePtr;
    }
}

// reset the class
void ResourceBundleHelper::reset()
{

    ClearBundle();
    setLocale("");
}

// Destructor
ResourceBundleHelper::~ResourceBundleHelper()
{
    try {
        ClearBundle();
    } catch (DistilleryException& x) {
    }
}

// Print message to an ostream with a vector of SubstitutionObjects
void UTILS_NAMESPACE::PrintMessage(ostream& strm,
                                   ResourceBundleHelper& rscBndHelper,
                                   const string& msgID,
                                   const vector<SubstitutionObject>& substitutionText,
                                   const string& bundleName,
                                   const bool printMessagePrefix)
{

    strm << PrintMessageToString(rscBndHelper, msgID, substitutionText, bundleName,
                                 printMessagePrefix);
};

// Print message to an ostream with a vector of strings
void UTILS_NAMESPACE::PrintMessage(ostream& strm,
                                   ResourceBundleHelper& rscBndHelper,
                                   const string& msgID,
                                   const vector<string>& substitutionText,
                                   const string& bundleName,
                                   const bool printMessagePrefix)
{

    strm << PrintMessageToString(rscBndHelper, msgID, substitutionText, bundleName,
                                 printMessagePrefix);
};

// Print the vector to a UnicodeString with a vector of strings
UnicodeString UTILS_NAMESPACE::PrintMessageToString(ResourceBundleHelper& rscBndHelper,
                                                    const string& msgID,
                                                    const vector<string>& substitutionText,
                                                    const string& bundleName,
                                                    const bool printMessagePrefix)
{

    Formattable* argumentsPtr;
    try {
        formattableArgumentSentinel fArgFSentinel;
        GetBundle(bundleName, msgID, rscBndHelper);

        SPCDBG(L_TRACE, "Message arguments:   " << substitutionText.size(), CORE_UTILS);
        // Process the substitution text as the arguments to the message formatter
        int32_t i = 0;
        int32_t numArguments = substitutionText.size();

        argumentsPtr = fArgFSentinel.getStorage(numArguments);

        while (i < numArguments) {
            SPCDBG(L_TRACE, "Message argument: " << substitutionText[i], CORE_UTILS);
            // A string argument
            argumentsPtr[i] = substitutionText[i].c_str();
            i++;
        }
        return FormatMsgAndPrint(rscBndHelper, msgID, numArguments, argumentsPtr,
                                 printMessagePrefix);

    } catch (const exception& e) {
        SPCDBG(L_ERROR, "Exception:" << e.what(), CORE_UTILS);
        throw;
    } catch (...) {
        // unknown exception handler
        THROW(Utils, "Unknown Exeption", UTILSUnexpectedException);
    }
};

// Print message to a UnicodeString with a vector of SubstitutionObjects
UnicodeString UTILS_NAMESPACE::PrintMessageToString(
  ResourceBundleHelper& rscBndHelper,
  const string& msgID,
  const vector<SubstitutionObject>& substitutionText,
  const string& bundleName,
  const bool printMessagePrefix)
{

    Formattable* argumentsPtr;
    try {
        formattableArgumentSentinel fArgFSentinel;
        GetBundle(bundleName, msgID, rscBndHelper);

        SPCDBG(L_TRACE, "Message arguments:   " << substitutionText.size(), CORE_UTILS);
        // Process the substitution text as the arguments to the message formatter
        int32_t i = 0;
        int32_t numArguments = substitutionText.size();

        argumentsPtr = fArgFSentinel.getStorage(numArguments);

        while (i < numArguments) {
            SPCDBG(L_TRACE, "Message argument: " << substitutionText[i], CORE_UTILS);
            SubstitutionObject::SubstitutionType subType = substitutionText[i].GetType();

            switch (subType) {
                case SubstitutionObject::SUB_STRING:
                    argumentsPtr[i] = substitutionText[i].GetString().c_str();
                    break;
                case SubstitutionObject::SUB_INT:
                    argumentsPtr[i] = substitutionText[i].GetInt();
                    break;
                case SubstitutionObject::SUB_DATE:
                    argumentsPtr[i] =
                      Formattable(substitutionText[i].GetDate(), Formattable::kIsDate);

                    break;
                default:;
            }
            i++;
        }

        return FormatMsgAndPrint(rscBndHelper, msgID, numArguments, argumentsPtr,
                                 printMessagePrefix);

    } catch (const exception& e) {
        SPCDBG(L_ERROR, "Exception:" << e.what(), CORE_UTILS);
        throw;
    } catch (...) {
        // unknown exception handler
        THROW(Utils, "Unknown Exeption", UTILSUnexpectedException);
    }
};

//  Get the default bundle to use. This will be determined by the message prefix
//  used in the message ID. If a default bundle cannot be located, the admin
//  resource bundle will be returned.
string UTILS_NAMESPACE::getDefaultBundle(const string& messageID)
{
    string prefix = messageID.substr(0, 5);
    for (int i = 0; i < __DEFAULT_BUNDLE_COUNT; i++) {
        if (prefix == defaultBundles[i][0]) {
            return defaultBundles[i][1];
        }
    }

    return __DEFAULT_BUNDLE;
}

// Get the appropriate message bundle allocated
void UTILS_NAMESPACE::GetBundle(const string& bundleName,
                                const string& messageID,
                                ResourceBundleHelper& rscBndHelper)
{
    // If a bundle name isn't specified, try and infer it by the message prefix
    string bName(bundleName);
    if (bName.empty()) {
        bName = getDefaultBundle(messageID);
        SPCDBG(L_DEBUG, "Message bundle name: " << bName, CORE_UTILS);
    }

    // If the bundle name was not specified and cannot be determined, send an
    // error
    if (bName.empty()) {
        THROW(MsgBundleError, "Message bundle can not be found", UTILSMsgBundleError);
    }

    SPCDBG(L_DEBUG, "Debug mode is enabled", CORE_UTILS);
    SPCDBG(L_DEBUG, "Message ID:          " << messageID, CORE_UTILS);

    if (bName == rscBndHelper.getResourceBundleName()) {
        SPCDBG(L_DEBUG, "Using cached bundle PTR", CORE_UTILS);

    } else {
        rscBndHelper.setResourceBundleName(bName);
    }
}

// Print the message with the substitution arguments provided
UnicodeString UTILS_NAMESPACE::FormatMsgAndPrint(ResourceBundleHelper& rscBndHelper,
                                                 const string& messageID,
                                                 const int32_t numArguments,
                                                 Formattable* argPtr,
                                                 bool printMsgPrefix)
{

    // define an imbedded sentinel calss to watch over the MessageFormat
    // object and  ensure it get's deallocated.
    class formatSentinel
    {
      public:
        formatSentinel()
          : _formatPtr(NULL){};
        MessageFormat* getStorage(UnicodeString translated, string locale)
        {
            UErrorCode status = U_ZERO_ERROR;
            // See comment above regarding ICU and RTC 19625
            // _formatPtr = new MessageFormat(translated, locale.c_str(),status);
            _formatPtr = new MessageFormat(translated, status);
            if (U_SUCCESS(status)) {
                return _formatPtr;
            } else {
                return NULL;
            }
        };
        ~formatSentinel()
        {
            if (_formatPtr != NULL) {
                MessageFormat* tempPtr = _formatPtr;
                _formatPtr = NULL;
                delete tempPtr;
            }
        };

      protected:
        MessageFormat* _formatPtr;

      private:
    };

    UErrorCode status = U_ZERO_ERROR;

    UnicodeString translated = rscBndHelper.getBundlePtr()->getStringEx(messageID.c_str(), status);

    if (U_SUCCESS(status)) {
        UnicodeString result;
        formatSentinel deleteme;

        MessageFormat* formatter = deleteme.getStorage(translated, rscBndHelper.getLocale());

        if (formatter != NULL) {
            FieldPosition fpos = 0;

            result = formatter->format(argPtr, numArguments, result, fpos, status);
            if (printMsgPrefix == true) {
                string prepend = messageID;
                prepend.append(" ");
                result.insert(0, prepend.c_str());
            }
            return result;

        } else {
            THROW(MsgFormatError, "Message format error.", UTILSMsgFormatError, messageID.c_str());
        }
    } else {
        THROW(MsgNotFound, " Message not found.", UTILSMsgNotFound, messageID.c_str());
    }
};

IMPL_EXCEPTION(UTILS_NAMESPACE, LocaleError, Utils);
IMPL_EXCEPTION(UTILS_NAMESPACE, MsgBundleError, Utils);
IMPL_EXCEPTION(UTILS_NAMESPACE, MsgNotFound, Utils);
IMPL_EXCEPTION(UTILS_NAMESPACE, MsgFormatError, Utils);
IMPL_EXCEPTION(UTILS_NAMESPACE, CannotAccessMsgDataError, Utils);
