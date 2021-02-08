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

#include <SPL/Core/MixedModeLineMapper.h>

#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/Utility/SysOut.h>

#include <SPL/Runtime/Utility/Singleton.t>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
namespace bf = boost::filesystem;

#include <cstdlib>
#include <errno.h>
#include <fstream>
#include <string>

using namespace std;

template class SPL::Singleton<
  SPL::MixedModeLineMapper>; // Explicitly instantiate the singleton code here

template class std::tr1::unordered_map<int, std::vector<int> >;
template class std::tr1::unordered_map<std::string, int>;

using namespace SPL;

MixedModeLineMapper::MixedModeLineMapper()
  : Singleton<MixedModeLineMapper>(*this)
{}

MixedModeLineMapper::~MixedModeLineMapper()
{
    FileToLineMap::const_iterator it;
    for (it = fileToLineMap_.begin(); it != fileToLineMap_.end(); ++it)
        delete it->second;
}

SourceLocation MixedModeLineMapper::map(SourceLocation const& loc) const
{
    int fileId = loc.fileNameIdx();
    assert(fileId >= 0);
    FileToLineMap::const_iterator it = fileToLineMap_.find(fileId);
    if (it == fileToLineMap_.end()) {
        string idxFile;
        if (hasIndexFile(loc, idxFile)) {
            LineMap* map1 = new LineMap();
            setLineMapping(idxFile, *map1);
            fileToLineMap_.insert(make_pair(fileId, map1));
        } else {
            fileToLineMap_.insert(make_pair(fileId, (LineMap*)0));
        }
        it = fileToLineMap_.find(fileId);
    }
    const LineMap* lineMap = it->second;
    if (lineMap) {
        LineMap::const_iterator mit = lineMap->find(loc.line());
        if (mit != lineMap->end()) {
            int mappedLine = mit->second[0];
            bf::path mappedFile = loc.fileName();
            mappedFile = bf::change_extension(mappedFile, ".splmm");
            return SourceLocation(mappedFile.string(), mappedLine, 0);
        }
    }
    return loc;
}

bool MixedModeLineMapper::hasIndexFile(SourceLocation const& loc, std::string& idxFile) const
{
    bf::path mmFile = loc.fileName() + "mm";
    // check if corresponding mm file exsits
    if (!bf::exists(mmFile))
        return false;
    std::string tkDirString;
    PathSearch& ps = PathSearch::instance();
    // locate the toolkit path for the given spl file
    if (!ps.mapSourceLocationToToolkitDirectory(loc, tkDirString)) {
        // Handle the spl-make-toolkit case
        CompilerConfiguration& c = CompilerConfiguration::instance();
        if (c.isSet(CompilerConfiguration::MakeToolkitDir))
            tkDirString = c.getStringValue(CompilerConfiguration::MakeToolkitDir);
        else
            return false;
    }
    bf::path idxPath = tkDirString + "/etc/config";
    // Compute the name of the index file
    if (!tkDirString.empty()) {
        // We have to remove the common prefix
        bf::path tk = tkDirString;
        bf::path::iterator tkit = tk.begin();
        bf::path::iterator mmit = mmFile.begin();
        while (tkit != tk.end() && mmit != mmFile.end() && *tkit == *mmit) {
            ++tkit;
            ++mmit;
        }

        // Now copy the remants
        bf::path result;
        while (mmit != mmFile.end()) {
            result /= *mmit;
            ++mmit;
        }
        mmFile = result;
    }
    idxPath /= mmFile.string() + ".idx";
    if (bf::exists(idxPath)) {
        // return path of index file if it exists
        idxFile = idxPath.string();
        return true;
    }
    return false; // index file doesn't exist
}

static void addLineMapping(int splLine, int mmLine, MixedModeLineMapper::LineMap& lineMap)
{
    // LineMap  &lineMap = map_.at(fileIndex);
    MixedModeLineMapper::LineMap::iterator it = lineMap.find(splLine);
    if (it != lineMap.end()) {
        it->second.push_back(mmLine);
        return;
    }
    vector<int> v;
    v.push_back(mmLine);
    lineMap.insert(make_pair(splLine, v));
}

void MixedModeLineMapper::setLineMapping(std::string const& idxFile, LineMap& lineMap) const
{
    lineMap.clear();
    ifstream istr(idxFile.c_str(), fstream::in);
    if (!istr)
        SysOut::die(SPL_CORE_CANNOT_OPEN_FILE(idxFile, RuntimeUtility::getErrorNoStr()));
    while (istr) {
        string line;
        getline(istr, line);
        if (line.empty() || line[0] == '#')
            continue;
        string::size_type start = 0, end;
        end = line.find(':', start);
        int mmLine = atoi(line.substr(start, end - start).c_str());
        start = end + 2;
        while (end != string::npos) {
            end = line.find(' ', start);
            string splLine = line.substr(start, end - start);
            // create line mapping using line and file index
            addLineMapping(atoi(splLine.c_str()), mmLine, lineMap);
            start = end + 1;
        }
    }
}
