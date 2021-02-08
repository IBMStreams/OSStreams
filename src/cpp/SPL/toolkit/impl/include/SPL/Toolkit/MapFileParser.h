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

#ifndef SPL_RUNTIME_PARSE_MAP_FILEH
#define SPL_RUNTIME_PARSE_MAP_FILEH

#include <UTILS/HashMapHelpers.h>
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/ProcessingElement/ProcessingElement.h>
#include <SPL/Runtime/Type/List.h>
#include <SPL/Toolkit/RuntimeException.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>

#include <boost/filesystem/path.hpp>

#include <string>
#include <fstream>


namespace SPL
{
    void parseIndicies(const std::string & opClass, const std::string & line,
                       std::stringstream & str, SPL::list<int64_t> & indicies);

    template <class T>
    inline void parseKey(std::stringstream & line,
                         const std::string & str, T & key)
    {
        line >> key;
    }

    template <>
    inline void parseKey(std::stringstream & line, const std::string & str,
                         SPL::rstring & key)
    {
        if(str[0] == '\"')
            line >> key;
        else {
            std::getline(line, key, ',');
            line.putback(',');
        }
    }

    /// Map File Parsing - read in a map file from value to indicies
    /// @param opClass Operator class name (i.e. 'Split')
    /// @param pe Processing Element for operator
    /// @param mapFile  filename for the map file
    /// @param map mapping from values to indicies
    /// @param defaultValue filename for the map file
    /// @param hasDefault set to 'true' if a default value is found
    template <class T>
    void parseMapFile(const std::string & opClass,
                      const SPL::ProcessingElement & pe,
                      const std::string & mapFile,
                      std::tr1::unordered_map<T, std::vector<int64_t> > & map,
                      std::vector<int64_t> & defaultValue,
                      bool & hasDefault)
    {
        T key;
        typedef std::tr1::unordered_map<T, std::vector<int64_t> > MapType;

        SPLAPPTRC(L_DEBUG, opClass << ": process map file '" << mapFile << '\'', SPL_OPER_DBG);
        hasDefault = false;
        namespace bf = boost::filesystem;

        std::ifstream f(mapFile.c_str());
        if(!f) {
            SPLTRACEMSGANDTHROW(SPLRuntimeOperator, L_ERROR, SPL_APPLICATION_RUNTIME_FAILED_OPEN_INPUT_FILE(mapFile, RuntimeUtility::getErrorNoStr()), SPL_OPER_DBG);
        }

        f.imbue(std::locale::classic());
        while(!f.eof()) {
            std::string s;
            // Read a line
            f >> std::skipws;
            std::getline(f, s);
            if(s.empty() || s[0] == '#')
                continue;
            std::stringstream line(s);
            line.imbue(std::locale::classic());
            SPL::list<int64_t> indicies;
            if(s.size() >= 8 && s.substr(0,7) == "default" && (isspace(s[7]) || s[7] == ',')) {
                std::string def;
                std::getline(line, def, ',');
                line.putback(',');
                parseIndicies(opClass, s, line, indicies);
                hasDefault = true;
                defaultValue = indicies;
                SPLAPPTRC(L_DEBUG, "Set default index list to " << indicies, SPL_OPER_DBG);
                continue;
            }
            parseKey(line, s, key);
            if(line.fail()) {
                SPLTRACEMSGANDTHROW(SPLRuntimeOperator, L_ERROR, SPL_APPLICATION_RUNTIME_FAILED_PARSE_KEY(opClass, s), SPL_OPER_DBG);
            }

            parseIndicies(opClass, s, line, indicies);
            typename MapType::iterator it = map.find(key);
            if(it != map.end()) {
                SPLTRACEMSGANDTHROW(SPLRuntimeOperator, L_ERROR, SPL_APPLICATION_RUNTIME_DUPLICATE_PARSE_KEY(opClass, s), SPL_OPER_DBG);
            } else {
                SPLAPPTRC(L_DEBUG, "Added mapping of '" << key << "' to " << indicies, SPL_OPER_DBG);
                map.insert(typename MapType::value_type(key, indicies));
            }
        }
    }
};

#endif /*SPL_RUNTIME_PARSE_MAP_FILEH*/
