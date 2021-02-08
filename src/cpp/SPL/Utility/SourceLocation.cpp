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

#include <SPL/Utility/SourceLocation.h>

#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/MixedModeLineMapper.h>

#include <cassert>
#include <iostream>
#include <sstream>

using namespace std;

namespace SPL {

SourceLocation::String2Int SourceLocation::_fileName2idx;
SourceLocation::Int2String SourceLocation::_idx2fileName;

SourceLocation SourceLocation::invalidSourceLocation("<invalid>", -1, 0);

SourceLocation::SourceLocation(string const& infileName)
  : _fileNameIdx(fileName2idx(infileName))
  , _line(0)
  , _column(0)
{}

SourceLocation::SourceLocation(string const& infileName, int in_line, int col)
  : _fileNameIdx(fileName2idx(infileName))
  , _line(in_line)
  , _column(col)
{}

SourceLocation::SourceLocation(int infileNameIdx, int in_line, int col)
  : _fileNameIdx(infileNameIdx)
  , _line(in_line)
  , _column(col)
{
    assert(0 <= infileNameIdx && infileNameIdx < (int)_idx2fileName.size());
}

string const& SourceLocation::fileName() const
{
    assert(0 <= _fileNameIdx && _fileNameIdx < (int)_idx2fileName.size());
    return _idx2fileName[_fileNameIdx];
}

int SourceLocation::fileName2idx(string const& fileName)
{
    String2Int::const_iterator iter = _fileName2idx.find(fileName);
    if (iter == _fileName2idx.end()) {
        uint32_t idx = _idx2fileName.size();
        _fileName2idx.insert(make_pair(fileName, idx));
        _idx2fileName.push_back(fileName);
        return idx;
    }
    return iter->second;
}

string const& SourceLocation::idx2fileName(int fileNameIdx)
{
    return _idx2fileName.at(fileNameIdx);
}

SourceLocation SourceLocation::mappedSourceLocation() const
{
    if (_line > 0) {
        MixedModeLineMapper const& mapper = MixedModeLineMapper::instance();
        return mapper.map(*this);
    }
    return *this;
}

ostream& SourceLocation::print(ostream& os) const
{
    os << fileName();
    if (0 < _line) {
        os << ':' << _line;
        if (0 < _column) {
            os << ':' << _column;
        }
    }
    return os;
}

string SourceLocation::toString() const
{
    ostringstream os;
    os << this;
    return os.str();
}

ostream& operator<<(ostream& os, SourceLocation const& loc)
{
    return loc.print(os);
}

ostream& operator<<(ostream& os, SourceLocation const* loc)
{
    if (NULL == loc) {
        return os << "NULL_LOC";
    }
    return loc->print(os);
}
} // namespace SPL
