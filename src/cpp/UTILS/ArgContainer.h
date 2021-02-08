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

// IBM
//
// Class Description:
//
// Implements a convenience to convert a command line to argv/argc, suitable
// for getopt

#ifndef ARGCONTAINER_H
#define ARGCONTAINER_H

#include <UTILS/SupportFunctions.h>
#include <cstring>
#include <string>
#include <unistd.h>
#include <vector>

UTILS_NAMESPACE_BEGIN

/// Convert a regular STL string to argv/argc as needed by getopt
class ArgContainer
{
  public:
    /// Default constructor
    ArgContainer(void)
    {
        // we must do the stuff below! this is the official way to reset getopt!
        // Horrible!
        optind = 0;
    };

    /// Copy constructor
    /// @param arg input ArgContainer
    ArgContainer(const ArgContainer& arg)
    {
        deallocate();
        argc = arg.argc;
        argv = new char*[argc + 1];
        argv[argc] = NULL;
        for (int i = 0; i < arg.argc; ++i) {
            argv[i] = new char[strlen(arg.argv[i]) + 1];
            strcpy(argv[i], arg.argv[i]);
        }
        // we must do the stuff below! this is the official way to reset getopt!
        // Horrible!
        optind = 0;
    };

    /// Copy constructor
    /// @param cmdline input command line
    /// @param addBogusArgv0 switch to add a bogus argv[0] value to the
    /// command line
    ArgContainer(const std::string& cmdline, const bool addBogusArgv0 = true)
    {
        std::vector<std::string> tokens;
        std::string mycmdline(cmdline);
        if (addBogusArgv0) {
            mycmdline = "bogus " + cmdline;
        }
        mycmdline = shellStyleSanitize(mycmdline);
        /**
        std::cout << "shell sanitized --> [" << mycmdline << "]" << std::endl;
        **/
        {
            unsigned begin = 0;
            bool quoteToken = false;
            bool spaceToken = true;
            unsigned i;
            for (i = 0; i < mycmdline.size(); ++i) {
                if (mycmdline[i] == '"') {
                    if (quoteToken) {
                        if (i + 1 == mycmdline.size() ||
                            (i + 1 < mycmdline.size() && mycmdline[i + 1] == ' ')) {
                            tokens.push_back(mycmdline.substr(begin, i - begin));
                            quoteToken = false;
                        }
                    } else {
                        quoteToken = true;
                        spaceToken = false;
                        begin = i + 1;
                    }
                } else if (mycmdline[i] == ' ') {
                    if (!quoteToken) {
                        if (spaceToken) {
                            tokens.push_back(mycmdline.substr(begin, i - begin));
                        } else {
                            spaceToken = true;
                        }
                        begin = i + 1;
                    }
                }
            }
            if (spaceToken) {
                tokens.push_back(mycmdline.substr(begin, i - begin));
            }
        }
        /**
        for(unsigned i=0; i<tokens.size(); ++i) {
          std::cout << "token #" << i << " is [" << tokens[i] << "]" << std::endl;
        }
        **/
        argv = new char*[tokens.size() + 1];
        argv[tokens.size()] = NULL;
        for (unsigned i = 0; i < tokens.size(); ++i) {
            argv[i] = new char[tokens[i].length() + 1];
            strcpy(argv[i], tokens[i].c_str());
        }
        argc = tokens.size();
        // we must do the stuff below! this is the official way to reset getopt!
        // Horrible!
        optind = 0;
    }

    /// Destructor
    ~ArgContainer(void) { deallocate(); };

    /// argv as expected :-)
    char** argv;
    /// argc as expected :-)
    int argc;

  private:
    /// Remove backslash/quote pattern and multiple chained space characters
    /// @return backslash/quote pattern become quote alone
    std::string shellStyleSanitize(const std::string& str) const
    {
        std::string ret;
        bool lastCharWasSpace = false;
        bool quoted = false;
        for (unsigned i = 0; i < str.size(); ++i) {
            if (i < str.size() - 1 && str[i] == '\\' && str[i + 1] == '"') {
                ret += '"';
                i++;
                lastCharWasSpace = false;
            } else {
                if (str[i] == '"') {
                    if (quoted) {
                        quoted = false;
                    } else {
                        quoted = true;
                    }
                }
                if (str[i] == ' ') {
                    if (!lastCharWasSpace || quoted) {
                        ret += str[i];
                    }
                    lastCharWasSpace = true;
                } else {
                    ret += str[i];
                    lastCharWasSpace = false;
                }
            }
        }
        return ret;
    }

    /// Deallocate memory used by argv
    void deallocate(void)
    {
        for (int i = 0; i < argc; ++i) {
            delete[] argv[i];
        }
        delete[] argv;
    }
};

UTILS_NAMESPACE_END

#endif
