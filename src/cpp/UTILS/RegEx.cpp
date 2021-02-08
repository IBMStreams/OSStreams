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

#include <UTILS/RegEx.h>
#include <string.h>

UTILS_NAMESPACE_USE;
using namespace std;

RegEx::RegEx(const string& pattern, int flags)
  : _pattern(pattern)
  , _flags(flags)
{
    memset(&_re, 0, sizeof(_re));

    AutoMutex lck(_mutex);
    compile();
}

RegEx::RegEx(const char* pattern, int flags)
  : _pattern(pattern)
  , _flags(flags)
{
    memset(&_re, 0, sizeof(_re));

    AutoMutex lck(_mutex);
    compile();
}

RegEx::RegEx(const RegEx& other)
  : _pattern(other._pattern)
  , _flags(other._flags)
{
    memset(&_re, 0, sizeof(_re));

    AutoMutex lck(_mutex);
    compile();
}

RegEx& RegEx::operator=(const RegEx& other)
{
    AutoMutex lck(_mutex);

    regfree(&_re);

    _pattern = other._pattern;
    _flags = other._flags;

    compile();

    return *this;
}

RegEx::~RegEx(void)
{
    AutoMutex lck(_mutex);
    if (&_re) {
        regfree(&_re);
    }
}

void RegEx::compile(void)
{
    int res = regcomp(&_re, _pattern.c_str(), _flags);

    if (res) {
        char buffer[4096];
        regerror(res, &_re, buffer, 4096);
        THROW(BadRegEx, buffer);
    }
}

int RegEx::match(const char* str, int nmatch, regmatch_t* pmatch, int flags) const
{
    return regexec(&_re, str, nmatch, pmatch, flags);
}

int RegEx::match(const string& str, vector<string>& result, int flags, int match_size) const
{
    regmatch_t matches[match_size];

    memset(matches, 0xff, match_size * sizeof(regmatch_t));

    int res = match(str.c_str(), match_size, matches, flags);

    if (res) {
        return 0;
    } else {
        int count = 0;

        for (int i = 0; i < match_size; i++) {
            if (matches[i].rm_so != -1 && matches[i].rm_eo != -1) {
                string elem = str.substr(matches[i].rm_so, matches[i].rm_eo - matches[i].rm_so);
                result.push_back(elem);
                count++;
            }
        }

        return count;
    }
}

int RegEx::match(const string& str, int flags) const
{
    int res = match(str.c_str(), 0, 0, flags);

    if (res) {
        return 0;
    } else {
        return 1;
    }
}

string RegEx::glob2regex(const string& glob_pattern,
                         char match_many,
                         char match_one1,
                         char match_one2,
                         bool filename_glob)
{
    stringstream strm;

    strm << "^";

    int in_alt = 0;

    for (unsigned long i = 0; i < glob_pattern.length(); i++) {
        if (glob_pattern[i] == match_many) {
            // case '*':

            if (filename_glob) {
                strm << "[^/]*";
            } else {
                strm << ".*";
            }
        } else {
            if (glob_pattern[i] == match_one1 || glob_pattern[i] == match_one2) {
                // case '?':
                // case '+':

                if (filename_glob) {
                    strm << "[^/]";
                } else {
                    strm << ".";
                }
            } else {
                switch (glob_pattern[i]) {
                    case '.':
                    case '|':
                    case '\\':
                    case '[':
                    case ']':
                    case '(':
                    case ')':
                        strm << "\\" << glob_pattern[i];
                        break;

                    case '{':
                        strm << "(";
                        in_alt++;
                        break;

                    case '}':
                        if (in_alt <= 0) {
                            THROW(BadGlobRegEx, "Mismatch in '}'");
                        }

                        strm << ")";
                        in_alt--;
                        break;

                    case ',':
                        if (in_alt > 0) {
                            strm << "|";
                        } else {
                            strm << glob_pattern[i];
                        }
                        break;

                    default:
                        strm << glob_pattern[i];
                        break;
                }
            }
        }
    }

    if (in_alt > 0) {
        THROW(BadGlobRegEx, "Mismatch in '}'");
    }

    strm << "$";

    return strm.str();
}

string RegEx::search_replace(const string& pattern, const string& str)
{
    char delimiter[] = { pattern[1], 0 };

    string::size_type pos = pattern.find_first_of(delimiter, 2);

    if (pos == string::npos) {
        THROW(BadSearchReplace, pattern);
    } else {
        string::size_type pos2 = pattern.find_first_of(delimiter, pos + 1);

        if (pos2 == string::npos) {
            THROW(BadSearchReplace, pattern);
        } else {
            return search_replace(pattern.substr(2, pos - 2),
                                  pattern.substr(pos + 1, pos2 - pos - 1), pattern.substr(pos2 + 1),
                                  str);
        }
    }
}

string RegEx::search_replace(const string& search,
                             const string& replace,
                             const string& options,
                             const string& str)
{
    unsigned int opt = 0;
    unsigned int re_opt = REG_EXTENDED;

    //   cout << "----------------------------" << endl
    //        << "search =" << search << endl
    //        << "replace=" << replace << endl
    //        << "options=" << options << endl
    //        << "----------------------------" << endl ;

    for (string::size_type i = 0; i < options.length(); i++) {
        switch (options[i]) {
            case 'g':
                opt |= SR_GLOBAL_REPLACE;
                break;
            case 'i':
            case 'I':
                re_opt |= REG_ICASE;
                break;
            case 'm':
            case 'M':
                re_opt |= REG_NEWLINE;
                break;
            default:
                THROW(BadSearchReplace, "Invalid option");
        }
    }

    RegEx re(search, re_opt);

    return search_replace(re, replace, opt, str);
}

bool RegEx::search_replace_one(RegEx& search,
                               const string& replace,
                               unsigned int options,
                               const string& str,
                               string& res,
                               string& remainder)
{
    vector<string> sub_matches;

    bool found = false;
    if (search.match(str, sub_matches)) {
        // cout << "+> " << sub_matches[0]  << " (" <<  sub_matches[0].size( ) << "/" <<
        // sub_matches.size( ) << ")" << endl ;

        if (sub_matches[0].size() > 0) {
            string::size_type pos = str.find(sub_matches[0]);
            found = true;
            res += str.substr(0, pos);

            // cout << "=> " << pos << endl ;

            bool escape = false;
            for (string::size_type i = 0; i < replace.length(); i++) {
                if (replace[i] == '\\' && !(options & SR_FIXED)) {
                    if (escape) {
                        res += '\\';
                    } else {
                        escape = true;
                    }
                } else {
                    if (escape) {
                        if (replace[i] >= '1' && replace[i] <= '9') {
                            unsigned int p = replace[i] - '0';

                            if (p < sub_matches.size()) {
                                res += sub_matches[p];
                            } else {
                                THROW(BadSearchReplace, "Out of bound");
                            }
                        } else {
                            res += '\\';
                            res += replace[i];
                        }
                        escape = false;
                    } else {
                        res += replace[i];
                    }
                }
            }
            remainder = str.substr(pos + sub_matches[0].length());
        }
    }
    return found;
}

string RegEx::search_replace_recur(RegEx& search,
                                   const string& replace,
                                   unsigned int options,
                                   const string& str)
{
    vector<string> sub_matches;

    if (search.match(str, sub_matches)) {
        // cout << "+> " << sub_matches[0]  << " (" <<  sub_matches[0].size( ) << "/" <<
        // sub_matches.size( ) << ")" << endl ;

        if (sub_matches[0].size() > 0) {
            string::size_type pos = str.find(sub_matches[0]);

            string res = str.substr(0, pos);

            // cout << "=> " << pos << endl ;

            bool escape = false;
            for (string::size_type i = 0; i < replace.length(); i++) {
                if (replace[i] == '\\' && !(options & SR_FIXED)) {
                    if (escape) {
                        res += '\\';
                    } else {
                        escape = true;
                    }
                } else {
                    if (escape) {
                        if (replace[i] >= '1' && replace[i] <= '9') {
                            unsigned int p = replace[i] - '0';

                            if (p < sub_matches.size()) {
                                res += sub_matches[p];
                            } else {
                                THROW(BadSearchReplace, "Out of bound");
                            }
                        } else {
                            res += '\\';
                            res += replace[i];
                        }
                        escape = false;
                    } else {
                        res += replace[i];
                    }
                }
            }

            if (options & SR_GLOBAL_REPLACE) {
                res += search_replace_recur(search, replace, options,
                                            str.substr(pos + sub_matches[0].length()));
            } else {
                res += str.substr(pos + sub_matches[0].length());
            }

            return res;
        }
    }

    return str;
}

// leave old code in just in case we want to additional testing
#define REGEX_REPLACE_RECUR 0

string RegEx::search_replace(RegEx& search,
                             const string& replace,
                             unsigned int options,
                             const string& str)
{
    string res, remainder, nremainder;

    // invoke recursive version
    // if (str.length() < 1000)
#if REGEX_REPLACE_RECUR == 1
    return search_replace_recur(search, replace, options, str);
#endif

    bool found = false;
    found = search_replace_one(search, replace, options, str, res, remainder);

    if (!found) {
        return str;
    }

    if (options & SR_GLOBAL_REPLACE) {
        for (;;) {
            found = search_replace_one(search, replace, options, remainder, res, nremainder);
            if (found) {
                remainder = nremainder;
            } else {
                res += remainder;
                break;
            }
        }

    } else {
        res += remainder;
    }

    return res;
}

ostream& UTILS_NAMESPACE_QUALIFIER operator<<(ostream& strm, const RegEx& re)
{
    strm << re._pattern;
    return strm;
}

IMPL_EXCEPTION(Distillery, RegEx, Distillery);
IMPL_EXCEPTION(Distillery, BadRegEx, RegEx);
IMPL_EXCEPTION(Distillery, BadGlobRegEx, RegEx);
IMPL_EXCEPTION(Distillery, BadSearchReplace, RegEx);
