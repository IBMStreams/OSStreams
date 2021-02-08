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

#include <SPL/Core/STGetMsg.h> // Include this header first

#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Utility/FormattableMessage.h>
#include <SPL/Runtime/Utility/Message.h>
#include <SPL/Runtime/Utility/MessageFormatter.h>
#include <SPL/Runtime/Utility/Singleton.t>
#include <SPL/Utility/SysOut.h>
#include <UTILS/SupportFunctions.h>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/pool/object_pool.hpp>

#include <locale>
#include <sstream>

using namespace std;
using namespace Distillery;
namespace bf = boost::filesystem;

namespace SPL {
template class Singleton<STGetMsg>;

static const char* knownBundles[] = {
    "CDISA", "StreamsAdminMessages",       "CDISI", "StreamsInstallMessages",
    "CDISP", "StreamsSPLCompilerMessages", "CDISR", "StreamsRuntimeMessages",
    "CDISS", "StreamsStudioMessages",      "CDISW", "StreamsWebMessages"
};

STGetMsg::STGetMsg()
  : Singleton<STGetMsg>(*this)
  , _runFromSrcTree(false)
  , _newLine(true)
  , _prefix(true)
  , _debug(false)
  , _userMessage(false)
  , _toolkit("spl")
{}

int STGetMsg::init()
{
    int r = DistilleryApplication::init();

    // Compute the compiler bin directory
    bf::path exe(obtainExecutableFullPath(_argv_0));
    exe = system_complete(exe).normalize();

    // Fix the case where libtool creates wrappers
    string edir = exe.branch_path().string();
    string lib = "/.libs";
    size_t pos = edir.rfind(lib);
    if (pos == edir.size() - lib.size()) {
        edir.erase(pos);
    }
    _exeDir = edir;

    // Populate the known bundle map prior to checking options as they will use it
    for (unsigned i = 0; i < sizeof(knownBundles) / sizeof(const char*); i += 2) {
        _knownBundleMap.insert(make_pair(knownBundles[i], knownBundles[i + 1]));
    }

    return r;
}

void STGetMsg::getDistilleryApplicationVisibleOptions(option_vector_t& options)
{
    // We don't want any of the default visible options
}

void STGetMsg::getArguments(option_vector_t& options)
{
    setKeepRemainingArguments();
    option_t args[] = {
        { 'b', "bundle", ARG, "", SPL_RUNTIME_ST_GETMSG_OPTION_BUNDLE, STR_OPT(STGetMsg::setBundle),
          0, 0 },
        { 'D', "debug", 0, "", SPL_RUNTIME_ST_GETMSG_OPTION_DEBUG, STR_OPT(STGetMsg::setDebug), 0,
          0 },
        { 'd', "default-locale", ARG, "", SPL_RUNTIME_ST_GETMSG_OPTION_DEFAULT_LOCALE,
          STR_OPT(STGetMsg::setDefaultLocale), 0, 0 },
        { 'h', "help", 0, "", SPL_RUNTIME_OPTION_HELP, STR_OPT(STGetMsg::setHelp), 0, 0 },
        { 'i', "path", ARG, "", SPL_RUNTIME_ST_GETMSG_OPTION_PATH, STR_OPT(STGetMsg::setPath), 0,
          0 },
        { 'l', "locale", ARG, "", SPL_RUNTIME_ST_GETMSG_OPTION_LOCALE, STR_OPT(STGetMsg::setLocale),
          0, 0 },
        { 'm', "message", ARG, "", SPL_RUNTIME_ST_GETMSG_OPTION_MESSAGE,
          STR_OPT(STGetMsg::setMessage), 0, 0 },
        { 'n', "no-new-line", 0, "", SPL_RUNTIME_ST_GETMSG_OPTION_NEWLINE,
          STR_OPT(STGetMsg::setNoNewLine), 0, 0 },
        { 'p', "no-prefix", 0, "", SPL_RUNTIME_ST_GETMSG_OPTION_PREFIX,
          STR_OPT(STGetMsg::setNoPrefix), 0, 0 },
        { 's', "source-tree", 0, "", SPL_RUNTIME_SOURCE_TREE, STR_OPT(STGetMsg::setSourceTree), 1,
          0 },
        { 't', "toolkit-name", ARG, "", SPL_RUNTIME_ST_GETMSG_OPTION_TOOLKIT,
          STR_OPT(STGetMsg::setToolkit), 1, 0 },
        { 'u', "user", 0, "", SPL_RUNTIME_ST_GETMSG_OPTION_USER, STR_OPT(STGetMsg::setUser), 0, 0 },
        { 'x', "default-text", ARG, "", SPL_RUNTIME_ST_GETMSG_OPTION_DEFAULT_TEXT,
          STR_OPT(STGetMsg::setDefaultText), 0, 0 }
    };
    APPEND_OPTIONS(options, args);
}

ostream& STGetMsg::printDescription(std::ostream& o) const
{
    o << SPL_RUNTIME_ST_GETMSG_USAGE;
    return o;
}

void STGetMsg::setHelp(const option_t* option, const char* value)
{
    printUsage(cout, false);
    cout << endl;
    Distillery::DistAppTerminate(0);
}

bool STGetMsg::validateOptions() const
{
    bool optionsAreValid = true;
    // Message must be specified
    if (_message.empty()) {
        SysOut::errorln(SPL_RUNTIME_ST_GETMSG_MESSAGE_MANDATORY);
        optionsAreValid = false;
    }

    // If the message does not have a recognizable prefix then the bundle must be specified
    if (!_userMessage) {
        string bundle = findBundle();
        // The prefix better match the bundle name if specified
        if (!_bundle.empty()) {
            if (_bundle != bundle) {
                SysOut::errorln(SPL_RUNTIME_ST_GETMSG_PREFIX_MISMATCH(prefix(), _bundle));
                optionsAreValid = false;
            }
        }
    } else {
        // Both path, toolkit and bundle name must be specified if this is a user message
        if (_path.empty()) {
            SysOut::errorln(SPL_RUNTIME_ST_GETMSG_PATH_MISSING);
            optionsAreValid = false;
        }
        if (_toolkit.empty()) {
            SysOut::errorln(SPL_RUNTIME_ST_GETMSG_TOOLKIT_MISSING);
            optionsAreValid = false;
        }
        if (_bundle.empty()) {
            SysOut::errorln(SPL_RUNTIME_ST_GETMSG_BUNDLE_MISSING);
            optionsAreValid = false;
        }
    }

    // If the bundle is not one of the known bundles then the path must be specified
    return optionsAreValid;
}

string STGetMsg::findBundle() const
{
    string bundle("StreamsAdminMessages");
    BundleMap::const_iterator it = _knownBundleMap.find(prefix());
    if (it != _knownBundleMap.end()) {
        bundle = it->second;
        // The following is a special case for the SPL runtime because it shares a
        // message range with the Streams runtime, but not the same message bundle.
        // The message bundles should be merged at some point.
        if (bundle == "StreamsRuntimeMessages") {
            string suffix(_message.substr(5, 4));
            istringstream istr(suffix, istringstream::in);
            int32_t v;
            istr >> v;
            if (v >= 5000 && v < 6000) {
                bundle = "StreamsSPLRuntimeMessages";
            }
        }
    }
    return bundle;
}

int STGetMsg::run(const vector<string>& remains)
{
    if (!validateOptions()) {
        return EXIT_FAILURE;
    }

    if (_locale.empty()) {
        _locale = setlocale(LC_ALL, NULL);
    }
    string bundle = (_bundle.empty()) ? findBundle() : _bundle;
    assert(!bundle.empty());

    // If this is a user toolkit register it
    if (_userMessage) {
        RuntimeMessageFormatter::instance().registerToolkit(_toolkit, _path);
    }

    // Build a message and call the runtime to load and format it.
    FormattableMessage msg(_toolkit, bundle, _defaultLocale, _message, _defaultText);
    // Add any substitution parameters provided
    if (!remains.empty()) {
        for (vector<string>::size_type i = 0, sz = remains.size(); i < sz; ++i) {
            // The substitution args have the form type:value, where type is either s, i, f or d.
            const string& arg = remains[i];
            if (arg[1] != ':' || arg.size() < 2) {
                SysOut::errorln(SPL_RUNTIME_ST_GETMSG_INVALID_SUBST(arg));
                return EXIT_FAILURE;
            }
            switch (arg[0]) {
                case 's':
                    msg % arg.substr(2);
                    break;
                case 'i': {
                    string s(arg.substr(2));
                    istringstream istr(s);
                    int64_t v;
                    istr >> v;
                    msg % v;
                } break;
                case 'f': {
                    string s(arg.substr(2));
                    istringstream istr(s);
                    double v;
                    istr >> v;
                    msg % v;
                } break;
                case 'd': {
                    string s(arg.substr(2));
                    istringstream istr(s);
                    double v;
                    istr >> v;
                    msg % Message::SubText::Date_t(v);
                } break;
                default:
                    SysOut::errorln(SPL_RUNTIME_ST_GETMSG_INVALID_SUBST(arg));
                    return EXIT_FAILURE;
            }
        }
    }

    string formattedMessage = RuntimeMessageFormatter::instance().format(msg, false, _locale);
    if (_prefix) {
        cout << _message << " ";
    }
    cout << formattedMessage;
    if (_newLine) {
        cout << endl;
    }

    return EXIT_SUCCESS;
}

} // namespace SPL
