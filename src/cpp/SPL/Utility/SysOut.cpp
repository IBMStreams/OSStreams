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

#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Runtime/Utility/FormattableMessage.h>
#include <SPL/Utility/SysOut.h>

#include <deque>

using namespace SPL;
using namespace std;
using boost::format;

struct ErrCnt
{
    ErrCnt(unsigned cur, unsigned max, bool die)
      : current(cur)
      , maximum(max)
      , dieAtEnd(die)
    {}
    unsigned current;
    unsigned maximum;
    bool dieAtEnd;
};

static deque<ErrCnt> error_cnt_;

void SysOut::setErrorTrap(unsigned maxErrors, bool dieAtEnd)
{
    error_cnt_.push_front(ErrCnt(0, maxErrors, dieAtEnd));
}

void SysOut::endErrorTrap()
{
    if (error_cnt_[0].dieAtEnd && error_cnt_[0].current > 0) {
        SysOut::writeln("");
        SysOut::die(SPL_CORE_CANNOT_CONTINUE_DUE_TO_ERRORS);
    } else {
        if (error_cnt_.size() > 1) {
            error_cnt_[1].current += error_cnt_[0].current;
        }
        error_cnt_.pop_front();
    }
}

void SysOut::updateErrorTrap()
{
    if (error_cnt_.empty()) {
        return;
    }
    error_cnt_[0].current++;
    unsigned errors = 0;
    typedef deque<ErrCnt>::const_iterator iterator;
    for (iterator it = error_cnt_.begin(); it != error_cnt_.end(); ++it) {
        errors += it->current;
        if (errors >= it->maximum) {
            SysOut::writeln("");
            SysOut::die(SPL_CORE_CANNOT_CONTINUE_DUE_TO_ERRORS);
        }
    }
}

unsigned SysOut::getNumErrorsInCurrentTrap()
{
    if (error_cnt_.empty()) {
        return 0;
    }
    return error_cnt_[0].current;
}

ostream& SysOut::write(const string& text, ostream& os)
{
    os << text;
    return os;
}
ostream& SysOut::write(const format& text, ostream& os)
{
    return write(text.str(), os);
}

ostream& SysOut::writeln(const string& text, ostream& os)
{
    write(text, os);
    os << endl;
    return os;
}

ostream& SysOut::writeln(const format& text, ostream& os)
{
    write(text, os);
    os << endl;
    return os;
}

ostream& SysOut::writeln(const FormattableMessage& text, ostream& os)
{
    writeSeverityAndMessageId(text, os);
    write(text, os);
    os << endl;
    return os;
}

ostream& SysOut::write(const string& file, int line, int col, const string& text, ostream& os)
{
    os << file << ':' << line << ':' << col << ": " << text;
    return os;
}
ostream& SysOut::write(const string& file,
                       int line,
                       int col,
                       const boost::format& text,
                       ostream& os)
{
    return write(file, line, col, text.str(), os);
}

ostream& SysOut::writeln(const string& file, int line, int col, const string& text, ostream& os)
{
    write(file, line, col, text, os);
    os << endl;
    return os;
}
ostream& SysOut::writeln(const string& file, int line, int col, const format& text, ostream& os)
{
    write(file, line, col, text, os);
    os << endl;
    return os;
}

ostream& SysOut::write(const string& file, int line, const string& text, ostream& os)
{
    os << file << ':' << line << ": " << text;
    return os;
}
ostream& SysOut::write(const string& file, int line, const format& text, ostream& os)
{
    return write(file, line, text.str(), os);
}

ostream& SysOut::writeln(const string& file, int line, const string& text, ostream& os)
{
    write(file, line, text, os);
    os << endl;
    return os;
}
ostream& SysOut::writeln(const string& file, int line, const format& text, ostream& os)
{
    write(file, line, text, os);
    os << endl;
    return os;
}

ostream& SysOut::write(const string& file, const string& text, ostream& os)
{
    os << file << ": " << text;
    return os;
}
ostream& SysOut::write(const string& file, const format& text, ostream& os)
{
    return write(file, text.str(), os);
}

ostream& SysOut::writeln(const string& file, const string& text, ostream& os)
{
    write(file, text, os);
    os << endl;
    return os;
}

ostream& SysOut::writeln(const string& file, const format& text, ostream& os)
{
    return writeln(file, text.str(), os);
}

ostream& SysOut::verboseln(const string& text, ostream& os)
{
    if (CompilerConfiguration::instance().isSet(CompilerConfiguration::Verbose)) {
        return writeln(text, os);
    }
    return os;
}

ostream& SysOut::verboseln(const format& text, ostream& os)
{
    if (CompilerConfiguration::instance().isSet(CompilerConfiguration::Verbose)) {
        return writeln(text, os);
    }
    return os;
}

ostream& SysOut::verboseln(const FormattableMessage& text, ostream& os)
{
    if (CompilerConfiguration::instance().isSet(CompilerConfiguration::Verbose)) {
        return writeln(text, os);
    }
    return os;
}

ostream& SysOut::verboseln(const string& file, int line, int col, const string& text, ostream& os)
{
    if (CompilerConfiguration::instance().isSet(CompilerConfiguration::Verbose)) {
        return writeln(file, line, col, text, os);
    }
    return os;
}
ostream& SysOut::verboseln(const string& file, int line, int col, const format& text, ostream& os)
{
    if (CompilerConfiguration::instance().isSet(CompilerConfiguration::Verbose)) {
        return writeln(file, line, col, text, os);
    }
    return os;
}

ostream& SysOut::verboseln(const string& file, int line, const string& text, ostream& os)
{
    if (CompilerConfiguration::instance().isSet(CompilerConfiguration::Verbose)) {
        return writeln(file, line, text, os);
    }
    return os;
}
ostream& SysOut::verboseln(const string& file, int line, const format& text, ostream& os)
{
    if (CompilerConfiguration::instance().isSet(CompilerConfiguration::Verbose)) {
        return writeln(file, line, text, os);
    }
    return os;
}

ostream& SysOut::verboseln(const string& file, const string& text, ostream& os)
{
    if (CompilerConfiguration::instance().isSet(CompilerConfiguration::Verbose)) {
        return writeln(file, text, os);
    }
    return os;
}

ostream& SysOut::verboseln(const string& file, const format& text, ostream& os)
{
    write(file, text, os);
    os << endl;
    return os;
}

void SysOut::writeLocSeverityAndMessage(const SourceLocation& loc,
                                        const FormattableMessage& text,
                                        ostream& os)
{
    if (loc.isValidSourceLocation()) {
        os << loc << ": ";
        writeSeverityAndMessageId(text, os);
        loc.printExtra(os);
    } else {
        writeSeverityAndMessageId(text, os);
    }
}

ostream& SysOut::errorOrWarnln(const FormattableMessage& text,
                               const SourceLocation& loc,
                               ostream& os,
                               bool isContinued,
                               bool isError)
{
    assert(isContinued == ('D' == text.key()[9]));
    writeLocSeverityAndMessage(loc, text, os);
    os << text << endl;
    SourceLocation mappedLoc = loc.mappedSourceLocation();
    if (loc != mappedLoc) {
        // the location refers to a line in a proprocessed file
        // emit a second messages with the location in the original spl.mm file
        writeLocSeverityAndMessage(mappedLoc, text, os);
        os << text << endl;
    }
    if (isError) {
        updateErrorTrap();
    }
    return os;
}

ostream& SysOut::errorln(const FormattableMessage& text, const SourceLocation& loc, ostream& os)
{
    return errorOrWarnln(text, loc, os, false, true);
}

ostream& SysOut::detailsErrorln(const FormattableMessage& text,
                                const SourceLocation& loc,
                                ostream& os)
{
    return errorOrWarnln(text, loc, os, true, true);
}

ostream& SysOut::warnln(const FormattableMessage& text, const SourceLocation& loc, ostream& os)
{
    return errorOrWarnln(text, loc, os, false, false);
}

ostream& SysOut::detailsWarnln(const FormattableMessage& text,
                               const SourceLocation& loc,
                               ostream& os)
{
    return errorOrWarnln(text, loc, os, true, false);
}

void SysOut::die(const FormattableMessage& text, const SourceLocation& loc, int err, ostream& os)
{
    writeLocSeverityAndMessage(loc, text, os);
    os << text << endl;

    // If the location refers to a line in a proprocessed file, then we need to emit a second
    // messages with the location in the original spl.mm file
    SourceLocation mappedLoc = loc.mappedSourceLocation();
    if (loc != mappedLoc) {
        writeLocSeverityAndMessage(mappedLoc, text, os);
        os << text << endl;
    }
    exit(err);
}

void SysOut::debug(const string& text)
{
    cerr << text << endl;
}

void SysOut::debug(const boost::format& text)
{
    cerr << text.str() << endl;
}

void SysOut::writeSeverityAndMessageId(const FormattableMessage& text, ostream& os)
{
    string const& id = text.key();

#ifndef NDEBUG
    const char* debugString = getenv("STREAMS_SPL_STOP_ON_ID");
    if (debugString && id == string(debugString)) {
        cout << "Put breakpoint here" << endl;
    }
#endif

    // The message id consists of 5 alpha chars plus 4 numeric chars plus a severity char
    // Only emit the message identifier if it is NOT a transitional message, indicated by a
    // severity of 'T'.
    if (id[9] == 'T') {
        return;
    }
    os << id << ' ';

    // Key the severity of the message so that they are in sync.
    switch (id[9]) {
        case 'E':
            os << SPL_CORE_ERROR << ": ";
            break;
        case 'W':
            os << SPL_CORE_WARNING << ": ";
            break;
        case 'D':
            os << SPL_CORE_DETAILS << ": ";
            break;
        default:
            break;
    }
}
