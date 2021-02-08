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

#ifndef SPL_ST_GET_MSG_H
#define SPL_ST_GET_MSG_H

#include <SPL/Runtime/Utility/Singleton.h>
#include <SPL/Runtime/Utility/Visibility.h>

#include <UTILS/DistilleryApplication.h>

#include <string>
#include <vector>

namespace SPL {

class DLL_PUBLIC STGetMsg
  : public Distillery::DistilleryApplication
  , public Singleton<STGetMsg>
{
  public:
    STGetMsg();

  protected:
    virtual int run(const std::vector<std::string>& remains);
    virtual void getArguments(option_vector_t& options);
    virtual void getDistilleryApplicationVisibleOptions(option_vector_t& options);
    virtual int init();
    virtual std::ostream& printDescription(std::ostream& o) const;

  private:
    void setHelp(const option_t* option, const char* value);
    void setSourceTree(const option_t* option, const char* value) { _runFromSrcTree = true; }
    void setMessage(const option_t* option, const char* value) { _message = value; }
    void setBundle(const option_t* option, const char* value) { _bundle = value; }
    void setLocale(const option_t* option, const char* value) { _locale = value; }
    void setDefaultLocale(const option_t* option, const char* value) { _defaultLocale = value; }
    void setDefaultText(const option_t* option, const char* value) { _defaultText = value; }
    void setNoNewLine(const option_t* option, const char* value) { _newLine = false; }
    void setNoPrefix(const option_t* option, const char* value) { _prefix = false; }
    void setPath(const option_t* option, const char* value) { _path = value; }
    void setToolkit(const option_t* option, const char* value) { _toolkit = value; }
    void setDebug(const option_t* option, const char* value) { _debug = true; }
    void setUser(const option_t* option, const char* value) { _userMessage = true; }
    bool validateOptions() const;
    std::string findBundle() const;
    std::string prefix() const { return _message.substr(0, 5); }

    typedef std::tr1::unordered_map<std::string, std::string> BundleMap;

    bool _runFromSrcTree;
    bool _newLine;
    bool _prefix;
    bool _debug;
    bool _userMessage;
    std::string _exeDir;
    std::string _message;
    std::string _bundle;
    std::string _locale;
    std::string _path;
    std::string _toolkit;
    std::string _defaultLocale;
    std::string _defaultText;
    BundleMap _knownBundleMap;
};

} // namespace SPL

#endif // define SPL_ST_GET_MSG_H
