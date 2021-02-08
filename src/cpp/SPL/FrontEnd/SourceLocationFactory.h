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

#ifndef SPL_INPUT_ERRORS_H
#define SPL_INPUT_ERRORS_H

#include <SPL/Runtime/Utility/Singleton.h>
#include <SPL/Utility/SourceLocation.h>
#include <UTILS/HashMapHelpers.h>

class ANTLR3_BASE_TREE_struct;

namespace SPL {
typedef ANTLR3_BASE_TREE_struct AstNode;

class ParserContext;

/// Source location where the source code comes from a string as opposed
/// to a file. This class also has static members that maintain mappings
/// from integer index to source string and vice versa to keep the
/// source strings unique.
class StringSourceLocation : public SourceLocation
{
    friend class SourceLocationFactory;

  protected:
    int _sourceStringIdx;
    StringSourceLocation(int, int, int, int);

  public:
    /// Special value to denote that the string is unavailable.
    static std::string const NOSTRING;

    /// Index of NOSTRING in the internal table.
    static int const NOSTRING_IDX;

    /// Print this location to the output stream.
    virtual std::ostream& print(std::ostream&) const;

    /// Print the 'extra' information to the output stream
    virtual std::ostream& printExtra(std::ostream&) const;

    /// The source string of this location.
    std::string const& sourceString() const;

    /// The index of the source string.
    int sourceStringIdx() const { return _sourceStringIdx; }
};

struct SourceLocationHash
{
    size_t operator()(SourceLocation const* x) const;
};
struct SourceLocationEquals
{
    bool operator()(SourceLocation const* x, SourceLocation const* y) const;
};

/// Factory for source locations. May return an instance of SourceLocation or an instance of
/// the subclass StringSourceLocation. The locations are owned by SourceLocationFactory, and
/// are guaranteed to be unique, meaning that if the same location is returned twice, it
/// actually refers to the same object. In addition, the internal file names and source strings
/// are also unique.
class SourceLocationFactory : public Singleton<SourceLocationFactory>
{
    SourceLocation::String2Int _sourceString2idx;
    SourceLocation::Int2String _idx2sourceString;
    typedef std::tr1::unordered_set<SourceLocation const*, SourceLocationHash, SourceLocationEquals>
      Set;
    Set _instances;

  public:
    /// Constructor; must be called only once, since this is a singleton.
    SourceLocationFactory();

    /// Create source location for file name, source string,
    /// line and column.
    SourceLocation const& create(std::string const& fileName,
                                 std::string const& sourceString,
                                 int line,
                                 int col);

    /// Create source location for file name index, source string index,
    /// line and column.
    SourceLocation const& create(int fileNameIdx, int sourceStringIdx, int line, int col);

    /// Create source location for the file of the parser context, and
    /// the line and column of the AST node.
    /// @param printExtra If true and if we are parsing source from a string, then any
    /// extra information associated wit the source location will be printed.
    SourceLocation const& create(ParserContext const&, AstNode&, bool printExtra = true);

    /// Create source location for the same file or string as the other
    /// source location, and the line and column of the AST node.
    SourceLocation const& create(SourceLocation const&, AstNode&);

    /// Given a source string, return its unique index in this factory.
    int sourceString2idx(std::string const&);

    /// Given a unique index in this factory, return the source string.
    std::string const& idx2sourceString(int);
};
};

#endif /*SPL_INPUT_ERRORS_H*/
