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

#include <SPL/Runtime/Function/BuiltinSPLFunctions.h>
#include <unicode/unistr.h>

#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/Type/UStringImpl.h>
#include <SPL/Runtime/Utility/RuntimeUtility.h>
#include <UTILS/DistilleryApplication.h>
#include <UTILS/HostInfo.h>
#include <UTILS/RegEx.h>
#include <UTILS/SupportFunctions.h>

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <iostream>

#include <unicode/numfmt.h>
#include <unicode/parseerr.h>
#include <unicode/regex.h>

#include <boost/algorithm/string/regex.hpp>
#include <boost/regex.hpp>
#include <boost/regex/icu.hpp>
#include <boost/shared_ptr.hpp>

#define WARNANDTHROW(x, y)                                                                         \
    {                                                                                              \
        SPLAPPTRC(L_ERROR, y, SPL_FUNC_DBG);                                                       \
        THROW_STRING(x, y);                                                                        \
    }

using namespace std;
using namespace SPL;

namespace SPL {
namespace Functions {
namespace String {
template<class Input, class Pattern>
class RegularExpressionCache
{
  public:
    typedef boost::shared_ptr<Pattern> PatternPtr;
    RegularExpressionCache()
      : rbufPos_(0)
      , rbufCsz_(0)
      , rbufCap_(128)
      , regexBuf_(rbufCap_)
    {}
    PatternPtr getRegEx(const Input& patt)
    {
        Distillery::AutoMutex am(regexMutex_);
        typename tr1::unordered_map<Input, uint32_t>::iterator it;
        if ((it = regexMap_.find(patt)) != regexMap_.end()) {
            return regexBuf_[it->second].first;
        }
        return compile(patt);
    }

  private:
    uint32_t rbufPos_;
    uint32_t rbufCsz_;
    uint32_t rbufCap_;
    tr1::unordered_map<Input, uint32_t> regexMap_;
    vector<pair<PatternPtr, Input> > regexBuf_;
    Distillery::Mutex regexMutex_;
    PatternPtr compile(const Input& patt);

    void enter(PatternPtr regex, const Input& patt)
    {
        if (rbufCsz_ == rbufCap_) {
            const Input& opatt = regexBuf_[rbufPos_].second;
            typename tr1::unordered_map<Input, uint32_t>::iterator it = regexMap_.find(opatt);
            if (it != regexMap_.end()) {
                regexMap_.erase(it);
            }
        } else {
            rbufCsz_++;
        }
        regexBuf_[rbufPos_] = make_pair(regex, patt);
        regexMap_[patt] = rbufPos_++;
        if (rbufPos_ == rbufCap_) {
            rbufPos_ = 0;
        }
    }
};

typedef RegularExpressionCache<ustring, icu::RegexPattern> RegEx16Cache;
static RegEx16Cache regex16Cache_;

template<>
RegularExpressionCache<ustring, icu::RegexPattern>::PatternPtr
RegularExpressionCache<ustring, icu::RegexPattern>::compile(const ustring& patt)
{
    UParseError pe;
    UErrorCode status = U_ZERO_ERROR;
    PatternPtr regex(icu::RegexPattern::compile(patt.impl(), pe, status));
    if (U_FAILURE(status)) {
        WARNANDTHROW(SPLRuntimeInvalidArgument,
                     SPL_APPLICATION_RUNTIME_REGEXP_FAILED_COMPILE(ustring::toUTF8(patt)));
    }
    enter(regex, patt);
    return regex;
}

typedef RegularExpressionCache<rstring, Distillery::RegEx> RegExCache;
static RegExCache regexCache_;

template<>
RegularExpressionCache<rstring, Distillery::RegEx>::PatternPtr
RegularExpressionCache<rstring, Distillery::RegEx>::compile(const rstring& patt)
{
    PatternPtr regex;
    try {
        regex.reset(new Distillery::RegEx(patt));
    } catch (Distillery::DistilleryException& e) {
        WARNANDTHROW(SPLRuntimeInvalidArgument,
                     SPL_APPLICATION_RUNTIME_REGEXP_FAILED_COMPILE(patt));
    }
    enter(regex, patt);
    return regex;
}

typedef RegularExpressionCache<rstring, boost::regex> RegExPerlCache;
static RegExPerlCache regexPerlCache_;

template<>
RegularExpressionCache<rstring, boost::regex>::PatternPtr
RegularExpressionCache<rstring, boost::regex>::compile(const rstring& patt)
{
    PatternPtr regex;
    std::string spatt(patt.c_str(), patt.size());
    try {
        regex.reset(new boost::regex(spatt, boost::regex_constants::perl));
    } catch (boost::bad_expression& e) {
        WARNANDTHROW(SPLRuntimeInvalidArgument,
                     SPL_APPLICATION_RUNTIME_REGEXP_FAILED_COMPILE(patt));
    }
    enter(regex, patt);
    return regex;
}

typedef RegularExpressionCache<ustring, boost::u32regex> RegEx16PerlCache;
static RegEx16PerlCache regex16PerlCache_;

template<>
RegularExpressionCache<ustring, boost::u32regex>::PatternPtr
RegularExpressionCache<ustring, boost::u32regex>::compile(const ustring& patt)
{
    PatternPtr regex;
    try {
        regex.reset(
          new boost::u32regex(boost::make_u32regex(patt.impl(), boost::regex_constants::perl)));
    } catch (boost::bad_expression& e) {
        WARNANDTHROW(SPLRuntimeInvalidArgument,
                     SPL_APPLICATION_RUNTIME_REGEXP_FAILED_COMPILE(ustring::toUTF8(patt)));
    }
    enter(regex, patt);
    return regex;
}

list<ustring> regexMatch(const ustring& text, const ustring& patt)
{
    list<ustring> strs;
    RegEx16Cache::PatternPtr regex = regex16Cache_.getRegEx(patt);
    UErrorCode status = U_ZERO_ERROR;
    auto_ptr<icu::RegexMatcher> matcher(regex->matcher(text.impl(), status));
    if (U_FAILURE(status)) {
        APPTRC(L_DEBUG, "Error creating RegexMatcher: " << u_errorName(status), SPL_FUNC_DBG);
        return strs;
    }
    if (matcher->find(0, status)) {
        // We have a match - first one is the whole string
        strs.push_back(new ustringImpl(matcher->group(status)));
        assert(U_SUCCESS(status));
        // Now the groups (skip the first one (the whole string))
        for (int32_t i = 0, iu = matcher->groupCount(); i < iu; i++) {
            strs.push_back(new ustringImpl(matcher->group(1 + i, status)));
            assert(U_SUCCESS(status));
        }
    }
    return strs;
}

ustring regexReplace(const ustring& text,
                     const ustring& searchPatt,
                     const ustring& substPatt,
                     const boolean global)
{
    ustring result;
    RegEx16Cache::PatternPtr regex = regex16Cache_.getRegEx(searchPatt);
    UErrorCode status = U_ZERO_ERROR;
    auto_ptr<icu::RegexMatcher> matcher(regex->matcher(text.impl(), status));
    if (U_FAILURE(status)) {
        APPTRC(L_DEBUG, "Error creating RegexMatcher: " << u_errorName(status), SPL_FUNC_DBG);
        return result;
    }
    if (global) {
        result = new ustringImpl(matcher->replaceAll(substPatt.impl(), status));
    } else {
        result = new ustringImpl(matcher->replaceFirst(substPatt.impl(), status));
    }
    if (U_FAILURE(status)) {
        APPTRC(L_DEBUG, "Error executing replace: " << u_errorName(status), SPL_FUNC_DBG);
    }
    return result;
}

list<rstring> regexMatch(const rstring& text, const rstring& patt)
{
    list<rstring> res;
    vector<string> strs;
    RegExCache::PatternPtr regex = regexCache_.getRegEx(patt);
    regex->match(text, strs);
    res.insert(res.end(), strs.begin(), strs.end());
    return res;
}

rstring regexReplace(const rstring& text,
                     const rstring& searchPatt,
                     const rstring& substPatt,
                     const boolean global)
{
    rstring result;
    RegExCache::PatternPtr regex = regexCache_.getRegEx(searchPatt);
    try {
        if (global) {
            result = Distillery::RegEx::search_replace(*regex, substPatt,
                                                       Distillery::RegEx::SR_GLOBAL_REPLACE, text);
        } else {
            result = Distillery::RegEx::search_replace(*regex, substPatt, 0, text);
        }
    } catch (Distillery::DistilleryException& e) {
        WARNANDTHROW(SPLRuntimeInvalidArgument, SPL_RUNTIME_REGEXP_SUB_FAILED_COMPILE(substPatt));
    }
    return result;
}

list<rstring> regexMatchPerl(const rstring& text, const rstring& patt)
{
    list<rstring> res;
    boost::cmatch m;
    RegExPerlCache::PatternPtr regex = regexPerlCache_.getRegEx(patt);
    if (boost::regex_search(text.c_str(), m, *regex, boost::match_perl)) {
        // We found something
        res.reserve(m.size());
        for (uint32_t i = 0, ui = m.size(); i < ui; i++) {
            res.push_back(rstring(m[i].first, m.length(i)));
        }
    }
    return res;
}

list<ustring> regexMatchPerl(const ustring& text, const ustring& patt)
{
    list<ustring> res;
    boost::u16match m;
    RegEx16PerlCache::PatternPtr regex = regex16PerlCache_.getRegEx(patt);
    if (boost::u32regex_search(text.impl(), m, *regex, boost::match_perl)) {
        // We found something
        res.reserve(m.size());
        for (uint32_t i = 0, ui = m.size(); i < ui; i++) {
            res.push_back(ustring(m[i].first, m.length(i)));
        }
    }
    return res;
}

rstring regexReplacePerl(const rstring& text,
                         const rstring& searchPatt,
                         const rstring& substPatt,
                         const boolean global)
{
    RegExPerlCache::PatternPtr regex = regexPerlCache_.getRegEx(searchPatt);

    std::string ssubstPatt(substPatt.c_str(), substPatt.size());
    std::string stext(text.c_str(), text.size());

    try {
        return boost::regex_replace(stext, *regex, ssubstPatt,
                                    boost::format_all |
                                      (global ? boost::match_default : boost::format_first_only));
    } catch (boost::bad_expression& e) {
        WARNANDTHROW(SPLRuntimeInvalidArgument,
                     SPL_APPLICATION_RUNTIME_REGEXP_FAILED_COMPILE(substPatt));
    }
    return "";
}

ustring regexReplacePerl(const ustring& text,
                         const ustring& searchPatt,
                         const ustring& substPatt,
                         const boolean global)
{
    RegEx16PerlCache::PatternPtr regex = regex16PerlCache_.getRegEx(searchPatt);

    try {
        return new ustringImpl(boost::u32regex_replace(
          text.impl(), *regex, substPatt.impl(),
          boost::format_all | (global ? boost::match_default : boost::format_first_only)));
    } catch (boost::bad_expression& e) {
        WARNANDTHROW(SPLRuntimeInvalidArgument,
                     SPL_APPLICATION_RUNTIME_REGEXP_FAILED_COMPILE(ustring::toUTF8(substPatt)));
    }
    return new ustringImpl("");
}

}
namespace Utility {
SPL::int32 getPEOutputPortConnections(uint32 port, List& connections)
{
    vector<ProcessingElement::OutputConnection> conns;
    bool success = ::SPL::PEImpl::instance().getOutputPortConnections(port, conns);
    if (!success) {
        return -1;
    }

    if (connections.getElementMetaType() != Meta::Type::TUPLE) {
        return -2; // something is seriously wrong
    }

    // Transfer the information to the list
    connections.removeAllElements();
    ValueHandle elem = connections.createElement();
    Tuple& e = (Tuple&)elem;
    int64& peId = (int64&)e.getAttribute(0).getValue();
    int64& inputPortIndex = (int64&)e.getAttribute(1).getValue();
    uint8& congestionFactor = (uint8&)e.getAttribute(2).getValue();
    uint64& nTuplesFilteredOut = (uint64&)e.getAttribute(3).getValue();
    for (uint32_t i = 0, ni = conns.size(); i < ni; i++) {
        ProcessingElement::OutputConnection& ci = conns[i];

        // populate the element
        peId = ci.peId();
        inputPortIndex = ci.inputPortIndex();
        congestionFactor = ci.congestionFactor();
        nTuplesFilteredOut = ci.nTuplesFilteredOut();

        // Add it to the list
        connections.pushBack(e);
    }
    elem.deleteValue();

    // It worked
    return 0;
}

SPL::int32 getOutputPortCongestionFactor(uint32 port)
{
    return ::SPL::PEImpl::instance().getOutputPortCongestionFactor(port);
}

SPL::rstring getToolkitDirectory(const SPL::rstring& toolkitName)
{
    return ::SPL::PEImpl::instance().getToolkitDirectory(toolkitName);
}
}
}
}
