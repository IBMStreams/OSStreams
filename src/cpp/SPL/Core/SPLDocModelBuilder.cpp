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

#include <SPL/Core/CompilerError.h>
#include <SPL/Core/SPLDocModelBuilder.h>
#include <SPL/Core/sourceCodeModel.h>
#include <SPL/Utility/SourceLocation.h>
#include <SPL/Utility/SysOut.h>
#include <UTILS/auto_array.h>

#include <tr1/unordered_set>
#include <unicode/uchar.h>
#include <unicode/unistr.h>

#include <iostream>

using namespace std;
using namespace icu;
using namespace std::tr1;
using namespace SPL;
using namespace UTILS_NAMESPACE;
using namespace xmlns::prod::streams::spl::sourceCode;

static void buildSplDoc(splDocType& splDoc,
                        UnicodeString const& comment,
                        SourceLocation const& loc);

static string extractSplDocContent(UnicodeString const& comment,
                                   int32_t beg,
                                   int32_t end,
                                   bool trimWhiteSpace);

splDocType* SPLDocModelBuilder::parse(string const& comment, SourceLocation const& loc)
{
    if (comment.size() < 5) { // not an SplDoc
        return NULL;
    }
    splDocType* splDoc(new splDocType());
    UnicodeString uComment(comment.c_str(), comment.length(), "UTF-8");
    buildSplDoc(*splDoc, uComment, loc);
    return splDoc;
}

bool isRecognizedTagWithTarget(UnicodeString const& comment,
                               int32_t pos,
                               int32_t commentEndIndex,
                               const unordered_set<string>& recognizedTagsWithTarget)
{
    // Extract the content of the prospective tag
    int32_t end = pos;
    while (end < commentEndIndex && !u_isUWhiteSpace(comment[end])) {
        ++end;
    }
    string s = extractSplDocContent(comment, pos, end, false);

    if (recognizedTagsWithTarget.count(s) > 0)
        return true;
    return false;
}

string extractLine(UnicodeString const& comment,
                   uint32_t lineStart,
                   uint32_t lineEnd,
                   uint32_t offsetOfFirstCharToSave,
                   uint32_t indent)
{
    int32_t extractStart = lineStart + offsetOfFirstCharToSave;
    if (indent > 0 && offsetOfFirstCharToSave > (indent - 1)) {
        extractStart -= (offsetOfFirstCharToSave - (indent - 1));
    }
    string line = extractSplDocContent(comment, extractStart, lineEnd, false);
    return line;
}

uint32_t parseDescription(splDocType& splDoc,
                          UnicodeString const& comment,
                          uint32_t commentBeginIndex,
                          uint32_t commentEndIndex,
                          SourceLocation const& loc,
                          uint32_t& line,
                          uint32_t& column,
                          uint32_t& indent)
{
    uint32_t pos = commentBeginIndex;
    uint32_t offsetOfFirstCharToSave = 0;
    uint32_t startColumn = loc.column() + 3;
    column = startColumn;
    line = loc.line();
    string description;
    uint32_t startPos = pos;
    bool accumulatingComment = false;
    bool decorationFound = false;

    // indent is 1-based, so it matches column, which is also 1-based
    // This is set to match the first non-decoration character in the comment.
    indent = 0;

    // Parse out the description, if any.
    // The description goes until the end of the comment block,
    // or the first tag is seen.  Tags must be the first non white-space
    // characters on the line other than decoration.
    enum State
    {
        start,
        lookingForEndOfFirstLine,
        lookingForFirstCharOnLine,
        lookingForEndOfLine,
        end
    };

    State state = start;
    while (state != end && pos <= commentEndIndex) {
        if (u_isUWhiteSpace(comment[pos])) {
            // White-space char
            if (comment[pos] == '\n') {
                // We hit the end of a line.
                switch (state) {
                    case start:
                        state = lookingForFirstCharOnLine;
                        break;

                    case lookingForEndOfFirstLine: {
                        string s =
                          extractSplDocContent(comment, offsetOfFirstCharToSave, pos + 1, false);
                        description += s;
                        state = lookingForFirstCharOnLine;
                    } break;

                    case lookingForEndOfLine:
                        if (accumulatingComment) {
                            description += extractLine(comment, startPos, pos + 1,
                                                       offsetOfFirstCharToSave, indent);
                        }
                        state = lookingForFirstCharOnLine;
                        break;

                    case lookingForFirstCharOnLine:
                        if (accumulatingComment) {
                            description += "\n";
                        }
                        break;
                    case end:
                        SysOut::errorln(SPL_CORE_INTERNAL_ERROR("1"), loc);
                        break;
                }
                ++line;
                column = 0;
                startPos = pos + 1;
                offsetOfFirstCharToSave = 0;
                decorationFound = false;
            }
        } else {
            // Non white-space char
            if (comment[pos] == '*') {
                switch (state) {
                    case start:
                        state = lookingForEndOfFirstLine;
                        indent = column;
                        offsetOfFirstCharToSave = pos;
                        accumulatingComment = true;
                        break;

                    case lookingForEndOfLine:
                    case lookingForEndOfFirstLine:
                        break;

                    case lookingForFirstCharOnLine:
                        // Could be decoration
                        if (!decorationFound && u_isUWhiteSpace(comment[pos + 1])) {
                            decorationFound = true;
                            // It is decoration, if left of the indentation.
                            // Otherwise it will be included with the comment.
                            if (comment[pos + 1] == '\n') {
                                offsetOfFirstCharToSave = column;
                            } else {
                                ++pos;
                                ++column;
                            }
                        } else {
                            offsetOfFirstCharToSave = column - 1;
                            state = lookingForEndOfLine;
                            accumulatingComment = true;
                            if (indent == 0)
                                indent = column;
                        }
                        break;

                    case end:
                        SysOut::errorln(SPL_CORE_INTERNAL_ERROR("2"), loc);
                        break;
                }
            } else if (comment[pos] == '@') {
                switch (state) {
                    case start:
                    case lookingForFirstCharOnLine:
                        state = end;
                        break;

                    case lookingForEndOfFirstLine:
                    case lookingForEndOfLine:
                        break;

                    case end:
                        SysOut::errorln(SPL_CORE_INTERNAL_ERROR("3"), loc);
                        break;
                }
            } else {
                switch (state) {
                    case start:
                        state = lookingForEndOfFirstLine;
                        indent = column;
                        offsetOfFirstCharToSave = pos;
                        accumulatingComment = true;
                        break;

                    case lookingForFirstCharOnLine:
                        offsetOfFirstCharToSave = column - 1;
                        state = lookingForEndOfLine;
                        accumulatingComment = true;
                        if (indent == 0)
                            indent = column;
                        break;

                    case lookingForEndOfLine:
                    case lookingForEndOfFirstLine:
                        // Nothing to do
                        break;

                    case end:
                        SysOut::errorln(SPL_CORE_INTERNAL_ERROR("4"), loc);
                        break;
                }
            }
        }
        if (state != end) {
            ++pos;
            ++column;
        }
    }
    if (state != end) {
        switch (state) {
            case lookingForEndOfLine:
                description += extractLine(comment, startPos, pos, offsetOfFirstCharToSave, indent);
                break;
            case lookingForEndOfFirstLine: {
                string s = extractSplDocContent(comment, offsetOfFirstCharToSave, pos, false);
                description += s;
            } break;

            case lookingForFirstCharOnLine:
                // Do nothing
                break;

            case start:
            case end:
                SysOut::errorln(SPL_CORE_INTERNAL_ERROR("5"), loc);
                break;
        }
    }

    if (0 < description.size()) {
        auto_ptr<splDocDescriptionType> bd(new splDocDescriptionType());
        bd->description(description);
        splDoc.description().set(bd);
    }

    return pos;
}

void createAnnotation(splDocType& splDoc,
                      const string& name,
                      const string& target,
                      const string& description)
{
    auto_ptr<splDocAnnotationType> an(new splDocAnnotationType());
    an->name(name);
    if (target.size() > 0) {
        an->target(target);
    }
    if (description.size() > 0) {
        an->description(description);
    }
    splDoc.annotation().push_back(an);
}

void parseAnnotations(splDocType& splDoc,
                      UnicodeString const& comment,
                      uint32_t commentBeginIndex,
                      uint32_t commentEndIndex,
                      const unordered_set<string>& recognizedTagsWithTarget,
                      SourceLocation const& loc,
                      uint32_t line,
                      uint32_t column,
                      uint32_t indent)
{
    // Parse out the annotations, if any
    enum State
    {
        start,
        lookingForName,
        lookingForEndOfName,
        lookingForTarget,
        lookingForEndOfTarget,
        lookingForDescription,
        lookingForEndOfDescription
    };

    uint32_t pos = commentBeginIndex;
    uint32_t startPos = pos - (column - 1);
    string name, target, description;
    uint32_t offsetOfFirstCharToSave = 0;
    bool starSeen = false;
    bool descriptionOnNewLine = false;
    bool nonWSCharSeen = false;

    State state = start;

    while (pos <= commentEndIndex) {
        if (u_isUWhiteSpace(comment[pos])) {
            // This is a white-space char
            if (comment[pos] == '\n') {
                switch (state) {
                    // We hit the end of the line while reading the description
                    case lookingForEndOfDescription: {
                        if (descriptionOnNewLine)
                            description += "\n";
                        description += extractLine(comment, startPos, pos, offsetOfFirstCharToSave,
                                                   descriptionOnNewLine ? indent : 0);
                        descriptionOnNewLine = true;
                        break;
                    }
                    case lookingForDescription:
                    case lookingForTarget:
                        // Nothing to do in this case
                        break;
                    case lookingForEndOfName:
                        name = extractSplDocContent(comment, startPos + offsetOfFirstCharToSave,
                                                    pos, false);
                        if (recognizedTagsWithTarget.count(name) > 0) {
                            state = lookingForTarget;
                        } else {
                            state = lookingForDescription;
                        }
                        break;
                    case lookingForEndOfTarget:
                        target = extractSplDocContent(comment, startPos + offsetOfFirstCharToSave,
                                                      pos, false);
                        state = lookingForDescription;
                        break;

                    case lookingForName: {
                        SourceLocation l(loc.fileName(), line, column - 1);
                        SysOut::errorln(SPL_CORE_SPLDOC_SYNTAX_ERROR, l);
                    }
                        return;

                    case start:
                        break;
                }
                offsetOfFirstCharToSave = 0;
                startPos = pos + 1;
                ++line;
                column = 0;
                starSeen = false;
                nonWSCharSeen = false;
            } else {
                switch (state) {
                    case lookingForEndOfName:
                        name = extractSplDocContent(comment, startPos + offsetOfFirstCharToSave,
                                                    pos, false);
                        // if the name is recognized, we look for a target, otherwise we look for a description
                        if (recognizedTagsWithTarget.count(name) > 0) {
                            state = lookingForTarget;
                        } else {
                            state = lookingForDescription;
                        }
                        break;
                    case lookingForEndOfTarget:
                        target = extractSplDocContent(comment, startPos + offsetOfFirstCharToSave,
                                                      pos, false);
                        state = lookingForDescription;
                        break;

                    case lookingForEndOfDescription:
                    case lookingForTarget:
                    case lookingForDescription:
                        // nothing to do for these cases
                        break;

                    case start:
                    case lookingForName: {
                        SourceLocation l(loc.fileName(), line, column);
                        SysOut::errorln(SPL_CORE_SPLDOC_SYNTAX_ERROR, l);
                        return;
                    }
                }
            }
        } else {
            // This is not white-space
            // If we hit a '*',
            // and if it is the first non-white-space char on the line,
            // and if it is followed by either a space or a newline,
            // and if it is left of the starting indentation,
            // then it is decoration,
            // otherwise it is a valid char.
            // If it is at or beyond the indentation it will be included
            // as part of the comment.
            if (!starSeen && !nonWSCharSeen && comment[pos] == '*') {
                // Once we've seen a star we don't look any more
                starSeen = true;
                if (pos + 1 < commentEndIndex && u_isUWhiteSpace(comment[pos + 1])) {
                    // decoration
                    if (comment[pos + 1] != '\n') {
                        ++pos;
                        ++column;
                    }
                    offsetOfFirstCharToSave = column;
                } else {
                    offsetOfFirstCharToSave = column - 1;
                    nonWSCharSeen = true;
                }
            } else if (comment[pos] == '@') {
                switch (state) {
                    case start:
                        state = lookingForName;
                        break;
                    case lookingForEndOfDescription:
                        // '@' can be found in a description.  It only forms the start of a tag
                        // if it's the first non decoration/white-space char on a line
                        if (!nonWSCharSeen) {
                            createAnnotation(splDoc, name, target, description);
                            name = "";
                            target = "";
                            description = "";
                            state = lookingForName;
                            descriptionOnNewLine = false;
                        }
                        break;
                    case lookingForDescription:
                        if (!nonWSCharSeen) {
                            // We were looking for a description and hit the next annotation.  Guess there isn't a description
                            createAnnotation(splDoc, name, target, description);
                            name = "";
                            target = "";
                            description = "";
                            state = lookingForName;
                            descriptionOnNewLine = false;
                        } else {
                            // We allow an '@' to appear in a description
                            offsetOfFirstCharToSave = column - 1;
                            state = lookingForEndOfDescription;
                        }
                        break;

                    case lookingForTarget:
                    case lookingForEndOfTarget:
                    case lookingForEndOfName:
                    case lookingForName: {
                        SourceLocation l(loc.fileName(), line, column);
                        SysOut::errorln(SPL_CORE_SPLDOC_SYNTAX_ERROR, l);
                    }
                        return;
                }
                nonWSCharSeen = true;
            } else {
                switch (state) {
                    case lookingForEndOfName:
                    case lookingForEndOfTarget:
                        // We don't do anything in this state
                        break;
                    case lookingForEndOfDescription:
                        if (!nonWSCharSeen) {
                            offsetOfFirstCharToSave = column - 1;
                        }
                        break;
                    case lookingForName:
                        offsetOfFirstCharToSave = column - 1;
                        state = lookingForEndOfName;
                        break;
                    case lookingForTarget:
                        offsetOfFirstCharToSave = column - 1;
                        state = lookingForEndOfTarget;
                        break;
                    case lookingForDescription:
                        offsetOfFirstCharToSave = column - 1;
                        state = lookingForEndOfDescription;
                        break;

                    case start:
                        SysOut::errorln(SPL_CORE_INTERNAL_ERROR("6"), loc);
                        return;
                }
                nonWSCharSeen = true;
            }
        }
        ++pos;
        ++column;
    }

    // Handle the final state
    switch (state) {
        case lookingForEndOfDescription: {
            // Make sure there is something valid to capture
            if (offsetOfFirstCharToSave != 0) {
                if (descriptionOnNewLine)
                    description += "\n";
                description += extractLine(comment, startPos, pos, offsetOfFirstCharToSave,
                                           descriptionOnNewLine ? indent : 0);
            }
            createAnnotation(splDoc, name, target, description);
            break;
        }

        case lookingForEndOfTarget:
            target = extractSplDocContent(comment, startPos + offsetOfFirstCharToSave, pos, false);
            createAnnotation(splDoc, name, target, description);
            break;

        case lookingForEndOfName:
            name = extractSplDocContent(comment, startPos + offsetOfFirstCharToSave, pos, false);
            createAnnotation(splDoc, name, target, description);
            break;

        case lookingForDescription:
            createAnnotation(splDoc, name, target, description);
            break;

        case lookingForTarget:
        case lookingForName: {
            SourceLocation l(loc.fileName(), line, column);
            SysOut::errorln(SPL_CORE_SPLDOC_SYNTAX_ERROR, l);
        }
            return;

        case start:
            SysOut::errorln(SPL_CORE_INTERNAL_ERROR("7"), loc);
            return;
    }
}

void buildSplDoc(splDocType& splDoc, UnicodeString const& comment, SourceLocation const& loc)
{
    unordered_set<string> recognizedTagsWithTarget;
    recognizedTagsWithTarget.insert("input");
    recognizedTagsWithTarget.insert("output");
    recognizedTagsWithTarget.insert("param");

    // This is the length in 16-bit units (could be surrogates that take two units)
    uint32_t commentSize = comment.length();
    uint32_t commentBeginIndex = 0, commentEndIndex = commentSize - 1;

    // We have a comment of the form /** .* */
    // Skip the starting /** and the ending */
    commentBeginIndex += 3;
    commentEndIndex -= 2;
    commentSize -= 5;
    uint32_t line, column, indent;
    uint32_t pos = parseDescription(splDoc, comment, commentBeginIndex, commentEndIndex, loc, line,
                                    column, indent);

    if (pos < commentEndIndex)
        parseAnnotations(splDoc, comment, pos, commentEndIndex, recognizedTagsWithTarget, loc, line,
                         column, indent);
}

string extractSplDocContent(UnicodeString const& comment,
                            int32_t beg,
                            int32_t end,
                            bool trimWhiteSpace)
{
    UnicodeString sub(comment, beg, end - beg);
    if (trimWhiteSpace)
        sub.trim();
    unsigned size = 4 * sub.length() + 1;
    auto_array<char> arr(new char[size]);
    sub.extract(0, sub.length(), arr.get(), size, "UTF-8");
    return string(arr.get());
}
