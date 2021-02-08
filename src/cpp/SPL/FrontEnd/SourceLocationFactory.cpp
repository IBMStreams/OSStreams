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

#include <SPL/FrontEnd/SourceLocationFactory.h>

#include <SPL/FrontEnd/FrontEndErrors.h>
#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/FrontEnd/TreeHelper.h>
#include <SPL/Runtime/Utility/Singleton.t>

#include <cassert>
#include <iostream>
using namespace std;

namespace SPL {
// .................................................................
std::string const StringSourceLocation::NOSTRING = "";
int const StringSourceLocation::NOSTRING_IDX = -1;

StringSourceLocation::StringSourceLocation(int infileNameIdx,
                                           int insourceStringIdx,
                                           int in_line,
                                           int col)
  : SourceLocation(infileNameIdx, in_line, col)
  , _sourceStringIdx(insourceStringIdx)
{
    assert(NOSTRING_IDX != insourceStringIdx);
}

std::string const& StringSourceLocation::sourceString() const
{
    return SourceLocationFactory::instance().idx2sourceString(_sourceStringIdx);
}

std::ostream& StringSourceLocation::print(std::ostream& os) const
{
    return SourceLocation::print(os);
}

std::ostream& StringSourceLocation::printExtra(std::ostream& os) const
{
    std::string const& s = sourceString();
    int c = column();
    if (1 < c) {
        os << '\'' << s.substr(0, c - 1) << "'   +   '" << s.substr(c - 1) << "': ";
    } else {
        os << '\'' << s << "': ";
    }
    return os;
}

// .................................................................
size_t SourceLocationHash::operator()(SourceLocation const* x) const
{
    assert(NULL != x); // using pointers, because std::tr1::unordered_set doesn't support references
    size_t baseHash = x->fileNameIdx() + 983 * x->line() + 631 * x->column();
    StringSourceLocation const* const sx = dynamic_cast<StringSourceLocation const*>(x);
    return baseHash + (NULL == sx ? 0 : 823 * sx->sourceStringIdx());
}

bool SourceLocationEquals::operator()(SourceLocation const* x, SourceLocation const* y) const
{
    assert(NULL != x &&
           NULL != y); // using pointers, because std::tr1::unordered_set doesn't support references
    bool baseEquals =
      x->fileNameIdx() == y->fileNameIdx() && x->line() == y->line() && x->column() == y->column();
    StringSourceLocation const* const sx = dynamic_cast<StringSourceLocation const*>(x);
    StringSourceLocation const* const sy = dynamic_cast<StringSourceLocation const*>(y);
    bool typeEquals = (NULL == sx) == (NULL == sy);
    bool stringEquals = NULL == sx || NULL == sy || sx->sourceStringIdx() == sy->sourceStringIdx();
    return baseEquals && typeEquals && stringEquals;
}

// .................................................................
// Explicitly instatiate the singleton code here
template class Singleton<SourceLocationFactory>;

SourceLocationFactory::SourceLocationFactory()
  : Singleton<SourceLocationFactory>(*this)
{}

int SourceLocationFactory::sourceString2idx(std::string const& sourceString)
{
    if (StringSourceLocation::NOSTRING == sourceString) {
        return StringSourceLocation::NOSTRING_IDX;
    }
    SourceLocation::String2Int::const_iterator iter = _sourceString2idx.find(sourceString);
    if (iter == _sourceString2idx.end()) {
        _idx2sourceString.push_back(sourceString);
        return _idx2sourceString.size() - 1;
    }
    return iter->second;
}

std::string const& SourceLocationFactory::idx2sourceString(int idx)
{
    return StringSourceLocation::NOSTRING_IDX == idx ? StringSourceLocation::NOSTRING
                                                     : _idx2sourceString.at(idx);
}

SourceLocation const& SourceLocationFactory::create(std::string const& fileName,
                                                    std::string const& sourceString,
                                                    int line,
                                                    int col)
{
    return create(SourceLocation::fileName2idx(fileName), sourceString2idx(sourceString), line,
                  col);
}

SourceLocation const& SourceLocationFactory::create(int fileNameIdx,
                                                    int sourceStringIdx,
                                                    int line,
                                                    int col)
{
    SourceLocation const* result = NULL;
    if (StringSourceLocation::NOSTRING_IDX == sourceStringIdx) {
        SourceLocation loc(fileNameIdx, line, col);
        Set::iterator it = _instances.find(&loc);
        if (_instances.end() == it) {
            result = new SourceLocation(fileNameIdx, line, col);
            _instances.insert(result);
        } else {
            result = *it;
        }
    } else {
        StringSourceLocation loc(fileNameIdx, sourceStringIdx, line, col);
        Set::iterator it = _instances.find(&loc);
        if (_instances.end() == it) {
            result = new StringSourceLocation(fileNameIdx, sourceStringIdx, line, col);
            _instances.insert(result);
        } else {
            result = *it;
        }
    }
    assert(NULL != result);
    return *result;
}

SourceLocation const& SourceLocationFactory::create(ParserContext const& pCtx,
                                                    AstNode& ast,
                                                    bool useString)
{
    AstNode* firstAst = astNodeWithFirstLocation(ast);
    AstNode& locAst = NULL == firstAst ? ast : *firstAst;
    int fileIndex = astSourceFileIdx(ast);
    if (-1 == fileIndex) {
        fileIndex = pCtx.fileNameIdx();
    }
    int line = astLine(locAst);
    pCtx.mapFileAndLine(fileIndex, line);
    return create(fileIndex,
                  useString ? pCtx.sourceStringIdx() : StringSourceLocation::NOSTRING_IDX, line,
                  astColumn(locAst));
}

SourceLocation const& SourceLocationFactory::create(SourceLocation const& base, AstNode& ast)
{
    AstNode* firstAst = astNodeWithFirstLocation(ast);
    AstNode& locAst = NULL == firstAst ? ast : *firstAst;
    return create(base.fileNameIdx(), StringSourceLocation::NOSTRING_IDX, astLine(locAst),
                  astColumn(locAst));
}
};
