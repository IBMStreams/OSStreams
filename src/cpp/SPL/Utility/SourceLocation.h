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

#ifndef SPL_UTILITY_SOURCELOCATION_H
#define SPL_UTILITY_SOURCELOCATION_H

#include <SPL/Runtime/Utility/Visibility.h>
#include <UTILS/HashMapHelpers.h>
#include <string>
#include <vector>

namespace SPL {

//! Represent locations in the user source
class DLL_PUBLIC SourceLocation
{
  public:
    typedef std::vector<std::string> Int2String;
    typedef std::tr1::unordered_map<std::string, int> String2Int;

  protected:
    static String2Int _fileName2idx;
    static Int2String _idx2fileName;
    int _fileNameIdx;
    int _line;
    int _column;

  public:
    /// A shared invalid source location for all to use
    static SourceLocation invalidSourceLocation;

    SourceLocation()
      : _fileNameIdx(0)
      , _line(-1)
      , _column(0)
    {}

    /// Create a source location given just a file name.  Line and column
    /// will both be 0.
    /// @param fileName The name of the file associated with this source location.
    SourceLocation(std::string const& fileName);

    SourceLocation(std::string const& fileName, int line, int col);
    SourceLocation(int fileNameIdx, int line, int col);
    virtual ~SourceLocation() {}

    bool operator==(SourceLocation const& other) const
    {
        return _fileNameIdx == other._fileNameIdx && _line == other._line &&
               _column == other._column;
    }
    bool operator!=(SourceLocation const& other) const { return !(*this == other); }

    /// Access the table mapping indexes to filenames
    /// @return Returns the table
    static Int2String const& allFileNames() { return _idx2fileName; }

    /// Get the name of the file associaed with this SourceLocation
    /// @return Returns the name of the file
    std::string const& fileName() const;

    /// Map the given filename to it's index
    /// @param fileName THe name of the file to map
    /// @return Returns the index of the given file.
    static int fileName2idx(std::string const& fileName);

    /// Get the index of the file associaed with this SourceLocation
    /// @return Returns the index of the file
    int fileNameIdx() const { return _fileNameIdx; }

    /// Given an index, return the associated filename.
    /// @param fileNameIdx The index
    /// @return Returns the filename
    static std::string const& idx2fileName(int fileNameIdx);

    /// Get the line number associated with this source location
    /// A value of -1 indicates this is not a valid source location
    /// A value of 0 indicates that there is no line or column number
    /// associated with this SourceLocation.
    /// @return Returns the line number for this SourceLocation
    int line() const { return _line; }

    /// Get the column number associated with this source location
    /// @return Returns the column number for this SourceLocation
    int column() const { return _column; }

    /// Emits a formatted representation of the SourceLocation in the given stream.
    /// @param os The stream
    /// @return Returns a reference to the given stream.
    virtual std::ostream& print(std::ostream& os) const;

    /// Print any 'extra' information associated with this source location
    /// @param os The stream
    /// @return Returns a reference to the given stream.
    virtual std::ostream& printExtra(std::ostream& os) const { return os; }

    /// Determines if this SourceLocation represents a valid location that should be reported in an
    /// error message.
    /// @return Returns true if this is a valid source location, otherwise false.
    bool isValidSourceLocation() const { return _line != -1; }

    /// Same as printing to an ostringstream.
    /// @return Returns the string representation
    std::string toString() const;

    /// If this source location represents a line in a proprocessed file, then return the
    /// source location in the original splmm file that corresponds
    SourceLocation mappedSourceLocation() const;
};

PUSH_DLL_PUBLIC
std::ostream& operator<<(std::ostream&, SourceLocation const&);
std::ostream& operator<<(std::ostream&, SourceLocation const*);
POP_DLL_PUBLIC
}

#endif // SPL_UTILITY_SOURCELOCATION_H
