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

#include <SPL/Runtime/Utility/MessageFormatter.h>

#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Utility/FormattableMessage.h>
#include <SPL/Runtime/Utility/Message.h>
#include <SPL/Runtime/Utility/Mutex.h>
#include <SPL/Runtime/Utility/Singleton.t>

#include <UTILS/SupportFunctions.h>
#include <UTILS/auto_array.h>

#include <boost/filesystem/operations.hpp>
#include <iostream>
#include <unicode/locid.h>
#include <unicode/msgfmt.h>
#include <unicode/resbund.h>
#include <unicode/ustream.h>

namespace bf = boost::filesystem;

using namespace std;
using namespace Distillery;
using namespace UTILS_NAMESPACE;

namespace SPL {

template class Singleton<RuntimeMessageFormatter>;
template class Singleton<CoreMessageFormatter>;

// We are using an impl class here in order to keep icu entities out of the interface and out of
// MessageFormatter.h
class MessageFormatterImpl
{
  public:
    MessageFormatterImpl(const string& bundleName,
                         const string& cantLoadBundle,
                         const string& cantLoadMsg);
    string format(const Message& msg, const Locale& locale);
    string format(const FormattableMessage& msg, const Locale& locale, bool includeKey);
    void registerToolkit(const string& toolkitName, const string& toolkitPath);

  private:
    class BundleWrapper
    {
        enum State
        {
            Init,
            Loaded,
            CantLoad
        };

      public:
        BundleWrapper(const std::string bundleName,
                      const string& location,
                      const Locale& defaultLocale,
                      const Locale& locale);
        ~BundleWrapper();

        UnicodeString loadMessageString(const string& key, const string& defaultString);

        static UnicodeString cantLoadMsg_;
        static UnicodeString cantLoadBundle_;
        static UnicodeString cantFindBundle_;

      private:
        void emitErrorMessage(const UnicodeString& str, const string& sub);

        string bundleName_;
        string location_;
        Locale defaultLocale_;
        Locale locale_;
        ResourceBundle* bundlePtr_;
        State state_;
    };

    UnicodeString loadMessageString(const string& toolkitName,
                                    const string& bundleName,
                                    const Locale& locale,
                                    const string& key,
                                    const string& defaultString);
    string format(const string& toolkitName,
                  const string& bundleName,
                  const string& key,
                  Message::SubtextList const& subTexts,
                  const Locale& locale,
                  const string& defaultString,
                  const Locale& defaultLocale,
                  bool includeKey);

    string bundleName_;
    Locale defaultLocale_;
    bool useDefaultText_;
    typedef tr1::unordered_map<std::string, BundleWrapper> BundleMap;
    typedef tr1::unordered_map<std::string, std::string> ToolkitMap;
    BundleMap bundleMap_;
    ToolkitMap toolkitMap_;
    Mutex mutex_;
};

// Note that the content of the following two variables must be kept in sync with the messages in
// StreamsSPLRuntimeMessages.xlf
static const char* cantLoadMsg =
  "Could not load message ''{0}''.  Continuing with default messages.";
static const char* cantLoadBundl =
  "WARNING: Unable to load the messages for locale ''{0}''.  Continuing with default messages.";
static const char* cantFindBundl =
  "WARNING: Unable to find resource bundle ''{0}''.  Continuing with default messages.";
UnicodeString MessageFormatterImpl::BundleWrapper::cantLoadMsg_ = cantLoadMsg;
UnicodeString MessageFormatterImpl::BundleWrapper::cantLoadBundle_ = cantLoadBundl;
UnicodeString MessageFormatterImpl::BundleWrapper::cantFindBundle_ = cantFindBundl;

// The instance of a runtime formatter
RuntimeMessageFormatter rtFormatter;

MessageFormatterImpl::MessageFormatterImpl(const std::string& bundleName,
                                           const std::string& cantLoadBundle,
                                           const std::string& cantLoadMessageMsg)
  : bundleName_(bundleName)
  , defaultLocale_(Locale::createCanonical(setlocale(LC_ALL, "")))
  , useDefaultText_(true)
{
    string messagesDir;
    // Use the path from env var STREAMS_ICU_DATA_DIR if specified
    const char* env = getenv("STREAMS_ICU_DATA_DIR");
    if (env != NULL) {
        messagesDir = env;
    } else {
        // otherwise if the STREAMS_INSTALL env var is specified then use that
        env = getenv(SystemStatics::get_STREAMS_INSTALL_envVar().c_str());
        if (env != NULL && *env) {
            bf::path streams_install(env);
            if (streams_install.is_complete()) {
                messagesDir = (streams_install / "messages").string();
            }
        }
    }

    if (!messagesDir.empty()) {
        // Register the default bundle
        toolkitMap_.insert(make_pair("spl", messagesDir));
        useDefaultText_ = false;
    }

    // Load the message strings for "can't load bundle" and "can't load message"
    // Only bother to load them if the locale is not en.  Note that the messagesDir
    // var is not set properly during a build so we don't want to try loading the messages.
    if (!useDefaultText_ && string(defaultLocale_.getBaseName()).substr(0, 2) != "en") {
        BundleWrapper::cantLoadBundle_ =
          loadMessageString("spl", bundleName_, defaultLocale_, cantLoadBundle, cantLoadBundl);
        BundleWrapper::cantLoadMsg_ =
          loadMessageString("spl", bundleName_, defaultLocale_, cantLoadMessageMsg, cantLoadMsg);
        BundleWrapper::cantFindBundle_ = loadMessageString(
          "spl", "StreamsSPLRuntimeMessages", defaultLocale_, "CDISR5211E", cantFindBundl);
    }
}

static string uformat(const UnicodeString& str,
                      const Locale& locale,
                      Formattable subs[],
                      unsigned numSubs)
{
    UErrorCode status = U_ZERO_ERROR;
    MessageFormat formatter(str, locale, status);
    UnicodeString result;
    FieldPosition fpos = 0;
    status = U_ZERO_ERROR;
    result = formatter.format(subs, numSubs, result, fpos, status);
    unsigned size = 4 * result.length() + 1;
    auto_array<char> arr(new char[size]);
    result.extract(0, result.length(), arr.get(), size, "UTF-8");

    return string(arr.get());
}

MessageFormatterImpl::BundleWrapper::BundleWrapper(const std::string bundleName,
                                                   const std::string& location,
                                                   const Locale& defaultLocale,
                                                   const Locale& locale)
  : bundleName_(bundleName)
  , location_(location)
  , defaultLocale_(defaultLocale)
  , locale_(locale)
  , bundlePtr_(NULL)
  , state_(Init)
{}

MessageFormatterImpl::BundleWrapper::~BundleWrapper()
{
    delete bundlePtr_;
}

void MessageFormatterImpl::BundleWrapper::emitErrorMessage(const UnicodeString& str,
                                                           const string& sub)
{
    Formattable f[] = { sub.c_str() };
    string formattedMsg = uformat(str, defaultLocale_, f, 1);
    cerr << formattedMsg << endl;
}

UnicodeString MessageFormatterImpl::BundleWrapper::loadMessageString(const string& key,
                                                                     const string& defaultString)
{
    if (state_ == CantLoad) {
        return defaultString.c_str();
    }
    if (state_ == Init) {
        bf::path bundleLoc(location_);
        bundleLoc /= bundleName_;
        // The path given to ICU needs to be fully qualified (possibly because cwd is not the
        // application dir) We fully qualify it here rather than when the toolkit location is
        // registered because at that point the PE is not yet initialized and we don't get a valid
        // application directory.
        if (!bundleLoc.is_complete()) {
            // We should never hit this now.  The path should always be fully qualified
            emitErrorMessage(cantLoadBundle_, locale_.getName());
        }
        UErrorCode stat = U_ZERO_ERROR;
        bundlePtr_ = new ResourceBundle(bundleLoc.string().c_str(), locale_, stat);
        if (stat > U_ZERO_ERROR) {
            // We failed to create a resource bundle.  Emit an error and return the default string.
            // From now on we'll use the default text.
            emitErrorMessage(cantLoadBundle_, locale_.getName());
            state_ = CantLoad;
            return defaultString.c_str();
        }
        state_ = Loaded;
    }
    UErrorCode status = U_ZERO_ERROR;
    UnicodeString str = bundlePtr_->getStringEx(key.c_str(), status);
    if (status > U_ZERO_ERROR) {
        emitErrorMessage(cantLoadMsg_, key);
        return defaultString.c_str();
    }
    return str;
}

UnicodeString MessageFormatterImpl::loadMessageString(const string& toolkitName,
                                                      const string& bundleName,
                                                      const Locale& locale,
                                                      const string& key,
                                                      const string& defaultString)
{
    // The key in the map is the unique string "toolkitName:bundleName:locale"
    string name = toolkitName + ":" + bundleName + ":" + locale.getBaseName();

    // Get the message bundle and load the message
    BundleMap::iterator it;
    it = bundleMap_.find(name);

    if (it == bundleMap_.end()) {
        // We've not yet loaded a message from this bundle.
        // Find the toolkit in the toolkit map
        ToolkitMap::iterator it1 = toolkitMap_.find(toolkitName);
        if (it1 == toolkitMap_.end()) {
            Formattable f[] = { bundleName.c_str() };
            string err = uformat(BundleWrapper::cantFindBundle_, locale, f, 1);
            cerr << err << endl;
            return defaultString.c_str();
        }
        const string& toolkitPath = it1->second;
        bundleMap_.insert(
          make_pair(name, BundleWrapper(bundleName, toolkitPath, defaultLocale_, locale)));
        it = bundleMap_.find(name);
    }

    BundleWrapper& br = it->second;
    return br.loadMessageString(key, defaultString);
}

string MessageFormatterImpl::format(const string& toolkitName,
                                    const string& bundleName,
                                    const string& key,
                                    Message::SubtextList const& subTexts,
                                    const Locale& locale,
                                    const string& defaultString,
                                    const Locale& defaultLoc,
                                    bool includeKey)
{
    AutoMutex am(mutex_);
    unsigned numSubs = subTexts.size();

    auto_array<Formattable> subs;
    if (numSubs > 0) {
        subs.reset(new Formattable[numSubs]);
    }

    size_t i = 0;
    for (Message::SubtextList::const_iterator it = subTexts.begin(); it != subTexts.end();
         ++it, ++i) {
        Message::SubText const& st = *it;
        switch (st.type()) {
            case Message::SubText::String:
                subs[i] = st.asString().c_str();
                break;
            case Message::SubText::Int64:
                subs[i] = st.asInt64();
                break;
            case Message::SubText::Float32:
                subs[i] = st.asFloat32();
                break;
            case Message::SubText::Double64:
                subs[i] = st.asDouble64();
                break;
            case Message::SubText::Date:
                subs[i] = Formattable(st.asDate(), Formattable::kIsDate);
                break;
            default:
                assert(!"Missing case statement");
        }
    }

    // This is the interface used internally so we'll use the default message if we are running
    // in an english locale
    UnicodeString str;
    if (useDefaultText_ || (!defaultString.empty() && defaultLocale_ == defaultLoc)) {
        str = defaultString.c_str();
    } else {
        str = loadMessageString(toolkitName, bundleName, locale, key, defaultString);
    }
    string prefix;
    if (includeKey) {
        prefix += key + ": ";
    }
    return prefix + uformat(str, locale, subs.get(), numSubs);
}

string MessageFormatterImpl::format(FormattableMessage const& message,
                                    const Locale& loc,
                                    bool includeKey)
{
    Locale defLoc = (!message.defaultLang().empty())
                      ? Locale::createCanonical(message.defaultLang().c_str())
                      : defaultLocale_;

    // Find the entry in the bundle map for the given toolkit
    return format(message.toolkitName(), message.bundleName(), message.key(),
                  message.substitutions(), loc, message.defaultText(), defLoc, includeKey);
}

string MessageFormatterImpl::format(Message const& message, const Locale& loc)
{
    // There is a bit of a kludge here to enable the re-numbering of SPL Runtime messages.  All
    // messages coming from applications compiled with Humber should come through this interface.
    // Since the messages were renumbered in Concord, the message ID embedded in the Humber
    // application no longer matches the same message ID in Concord.  However, Humber messages have
    // the english text embedded in the application, so we need to force the use of that embeded
    // text.  By using defaultLocale_ here, we cause the called routine to use the embeded message
    // text.
    return format("spl", bundleName_, message.key(), message.substitutions(), loc,
                  message.englishText(), defaultLocale_, false);
}

void MessageFormatterImpl::registerToolkit(const std::string& toolkitName,
                                           const std::string& toolkitPath)
{
    AutoMutex am(mutex_);
    // On entry the toolkit path points to the root dir of the toolkit
    // It also may be a relative path.  ICU wants a fully qualified path.
    bf::path p = bf::path(toolkitPath) / "impl" / "nl";

    // Register the toolkit
    toolkitMap_.insert(make_pair(toolkitName, p.string()));
}

MessageFormatter::MessageFormatter(const std::string& bundleName,
                                   const std::string& cantLoadBundle,
                                   const std::string& cantLoadMessageMsg)
  : _impl(new MessageFormatterImpl(bundleName, cantLoadBundle, cantLoadMessageMsg))
{}

MessageFormatter::~MessageFormatter() {}

string MessageFormatter::format(Message const& message)
{
    string locale = setlocale(LC_ALL, NULL);
    Locale loc = Locale::createCanonical(locale.c_str());
    return impl().format(message, loc);
}

string MessageFormatter::format(FormattableMessage const& message, bool includeKey)
{
    // Find the entry in the bundle map for the given toolkit
    // We'll load the message based on the locale in effect at the moment
    // This may vary from the defaultLocal because we are used by the st-getmsg utility which sets
    // the locale prior to calling format
    string locale = setlocale(LC_ALL, NULL);
    Locale loc = Locale::createCanonical(locale.c_str());
    return impl().format(message, loc, includeKey);
}

string MessageFormatter::format(FormattableMessage const& message,
                                bool includeKey,
                                const string& locale)
{
    Locale loc = Locale::createCanonical(locale.c_str());
    return impl().format(message, loc, includeKey);
}

void MessageFormatter::registerToolkit(const std::string& toolkitName,
                                       const std::string& toolkitPath)
{
    impl().registerToolkit(toolkitName, toolkitPath);
}

} // namespace SPL
