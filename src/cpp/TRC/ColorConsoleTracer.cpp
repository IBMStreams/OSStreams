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

#include <TRC/ColorConsoleTracer.h>
#include <TRC/DistilleryDebug.h>
#include <UTILS/Directory.h>
#include <UTILS/SupportFunctions.h>

UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE
using namespace std;

ColorConsoleTracer::ANSIColors ColorConsoleTracer::_c;

ColorConsoleTracer::ANSIColors::ANSIColors(void)
{
    attr.insert(make_pair("0", "0"));
    attr.insert(make_pair("1", "1"));
    attr.insert(make_pair("2", "2"));
    attr.insert(make_pair("4", "4"));
    attr.insert(make_pair("5", "5"));
    attr.insert(make_pair("7", "7"));
    attr.insert(make_pair("8", "8"));

    attr.insert(make_pair("alloff", "0"));
    attr.insert(make_pair("bold", "1"));
    attr.insert(make_pair("dim", "2"));
    attr.insert(make_pair("underscore", "4"));
    attr.insert(make_pair("blink", "5"));
    attr.insert(make_pair("reverse", "7"));
    attr.insert(make_pair("hidden", "8"));

    fg.insert(make_pair("30", "30"));
    fg.insert(make_pair("31", "31"));
    fg.insert(make_pair("32", "32"));
    fg.insert(make_pair("33", "33"));
    fg.insert(make_pair("34", "34"));
    fg.insert(make_pair("35", "35"));
    fg.insert(make_pair("36", "36"));
    fg.insert(make_pair("37", "37"));

    fg.insert(make_pair("black", "30"));
    fg.insert(make_pair("red", "31"));
    fg.insert(make_pair("green", "32"));
    fg.insert(make_pair("yellow", "33"));
    fg.insert(make_pair("blue", "34"));
    fg.insert(make_pair("magenta", "35"));
    fg.insert(make_pair("cyan", "36"));
    fg.insert(make_pair("white", "37"));

    bg.insert(make_pair("40", "40"));
    bg.insert(make_pair("41", "41"));
    bg.insert(make_pair("42", "42"));
    bg.insert(make_pair("43", "43"));
    bg.insert(make_pair("44", "44"));
    bg.insert(make_pair("45", "45"));
    bg.insert(make_pair("46", "46"));
    bg.insert(make_pair("47", "47"));

    bg.insert(make_pair("black", "40"));
    bg.insert(make_pair("red", "41"));
    bg.insert(make_pair("green", "42"));
    bg.insert(make_pair("yellow", "43"));
    bg.insert(make_pair("blue", "44"));
    bg.insert(make_pair("magenta", "45"));
    bg.insert(make_pair("cyan", "46"));
    bg.insert(make_pair("white", "47"));
}

string ColorConsoleTracer::ANSIColors::toANSICodes(const string& str) const
{
    if (str.empty())
        return string("");
    string tstr(str);
    transform(tstr.begin(), tstr.end(), tstr.begin(), (int (*)(int))std::tolower);
    vector<string> t;
    tokenize(tstr, t, ";", false);
    string converted;
    map<const string, const string>::const_iterator i = attr.find(t[0]);
    if (t.size() >= 1) {
        if (i != attr.end())
            converted = i->second;
        else
            converted = t[0];
    }
    if (t.size() >= 2) {
        i = fg.find(t[1]);
        if (i != fg.end())
            converted += ";" + i->second;
        else {
            converted += ";";
            converted += t[1];
        }
    }
    if (t.size() >= 3) {
        i = bg.find(t[2]);
        if (i != bg.end())
            converted += ";" + i->second;
        else {
            converted += ";";
            converted += t[2];
        }
    }

    return converted;
}

ColorConsoleTracer::ColorConsoleTracer(const string& color, int level, bool toStdout)
  : ConsoleTracer(level, toStdout)
  , _color(_c.toANSICodes(color))
{
    if (isatty(getFD())) {
        _use_colors = true;
    } else {
        _use_colors = false;
    }
}

ColorConsoleTracer::~ColorConsoleTracer() {}

void ColorConsoleTracer::output(const string& msg)
{
    if (_use_colors) {
        stringstream m;
        m << "\033[" << _color << "m" << msg << "\033[00m";
        ConsoleTracer::output(m.str());
    } else {
        ConsoleTracer::output(msg);
    }
}
