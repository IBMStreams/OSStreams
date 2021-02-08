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

#include <SPL/Utility/Utility.h>

#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/CompilerException.h>
#include <SPL/FrontEnd/AstVisitor.h>
#include <SPL/FrontEnd/NameDefFinder.h>
#include <SPL/FrontEnd/SymbolTableEntry.h>
#include <SPL/FrontEnd/TreeHelper.h>
#include <SPL/Runtime/Utility/RuntimeUtility.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/SysOut.h>
#include <UTILS/RegEx.h>
#include <UTILS/SupportFunctions.h>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>

#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;
using namespace SPL;
using namespace Distillery;
namespace bf = boost::filesystem;

string Utility::shorten(string const& txt, size_t const n)
{
    if (txt.size() <= n) {
        return txt;
    } else if (n < 4) {
        return txt.substr(0, n);
    } else {
        size_t r = n / 4;
        size_t m = n - r;
        string str = txt.substr(0, m - 3);
        str += "...";
        str += txt.substr(txt.size() - r);
        return str;
    }
}

void Utility::writeIfNecessary(const bf::path& file, const string& value)
{
    if (bf::exists(file)) {
        // Okay, it is there.   has it changed?
        ifstream f(file.string().c_str());
        string oldVal((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
        if (value == oldVal) {
            SPLDBG("Contents of " << file.string() << " match new value",
                   Debug::TraceFunctionGenerator);
            return; // nothing to do
        }
    }

    // We have to write out the new contents
    ofstream f(file.string().c_str());
    if (!f) {
        SysOut::die(SPL_CORE_CANNOT_CREATE_FILE(file.string(), RuntimeUtility::getErrorNoStr()));
    }
    f << value;
    SPLDBG("Wrote new contents for " << file.string(), Debug::TraceFunctionGenerator);
}

bool Utility::isReservedJavaOperatorParam(string const& name)
{
    if (name == "className" || name == "classLibrary" || name == "generated" ||
        name.find("javaop") == 0) {
        return true;
    }
    return false;
}

static string getEnvironmentVariable(string const& name, string const& defaultVal)
{
    char* nsstr = getenv(name.c_str());
    if (nsstr) {
        return nsstr;
    }
    return defaultVal;
}

string Utility::expandEnvironmentVariables(const string& text)
{
    static RegEx regex("@([^@]+)@");
    vector<string> evars;
    string result(text);
    while (true) {
        evars.clear();
        regex.match(result, evars);
        if (evars.empty()) {
            break;
        }
        const string& evar = evars[1];
        string eval = getEnvironmentVariable(evar, "");
        if (eval.empty()) {
            THROW(SPLCompiler, SPL_UTILITY_ENVIRONMENT_VARIABLE_IS_NOT_SET(evar));
        }
        result = RegEx::search_replace(regex, eval, 0, result);
    }
    return result;
}

namespace bf = boost::filesystem;

void Utility::randomPartitioning(const uint32_t n, uint32_t& pcnt, uint32_t assgn[])
{
    pcnt = 0;
    srand(time(NULL));
    double* bells = new double[n];
    bells[0] = 1.0; // bells[i-1] = (bell number for i) / i!
    for (uint32_t i = 1; i < n; ++i) {
        double sum = 1.0 / i;
        for (uint32_t j = 1; j < i; ++j) {
            sum = (sum + bells[j - 1]) / (i - j);
        }
        bells[i] = (sum + bells[i - 1]) / (i + 1.0);
    }
    // assign partitions
    for (uint32_t m = n; m > 0; ++pcnt) {
        double z = rand() / (RAND_MAX + 1.0);
        z *= m * bells[m - 1];
        for (uint32_t k = 1; z >= 0.0; ++k) {
            assgn[m - 1] = pcnt;
            if (--m == 0) {
                break;
            }
            z = k * (z - bells[m - 1]);
        }
    }
    // do a random permutation
    for (uint32_t i = 0; i < n; ++i) {
        double z = rand() / (RAND_MAX + 1.0);
        uint32_t j = i + static_cast<uint32_t>(z * (n - i));
        uint32_t t = assgn[i];
        assgn[i] = assgn[j];
        assgn[j] = t;
    }
    delete[] bells;
}

string Utility::replaceDoubleColonsWithDot(string const& str)
{
    size_t pos = 0;
    ostringstream ostr;
    do {
        size_t cpos = str.find("::", pos);
        if (cpos == string::npos) {
            ostr << str.substr(pos);
            break;
        }
        ostr << str.substr(pos, cpos - pos) << ".";
        pos = cpos + 2;
    } while (pos < str.size());
    return ostr.str();
}

bf::path Utility::normalize(const bf::path& path)
{
    bf::path newPath = path;
    newPath.normalize();

    // Boost normalization does not remove leading ./
    string p = newPath.string();
    size_t len = p.length();
    // Boost normalization does not remove trailing /.
    if (len > 2 && p[len - 1] == '.' && p[len - 2] == '/') {
        newPath = bf::path(p.substr(0, len - 2));
    }
    return newPath;
}

// Change 'opath' into a relative path, rooted at 'root'
// Store the number of levels remaining in the resulting relative path in 'nup'
bool Utility::makeRelativePath(bf::path& opath, bf::path const& root, int& nup)
{
    bf::path npath = normalize(opath);
    bf::path rpath = normalize(root);
    bf::path::iterator rit = rpath.begin();
    bf::path::iterator nit = npath.begin();
    for (; rit != rpath.end(); ++rit, ++nit) {
        if (nit == npath.end()) {
            return false;
        }
        if (*rit != *nit) {
            return false;
        }
    }
    if (nit == npath.end()) {
        opath = ".";
        nup = 0;
        return true;
    }
    opath = *(nit++);
    nup = 1;
    for (; nit != npath.end(); ++nit) {
        opath /= *nit;
        nup++;
    }
    return true;
}

int Utility::findNumLevelsUpFromOutputDirToAppDir()
{
    CompilerConfiguration const& config = CompilerConfiguration::instance();
    bf::path outDir = config.getStringValue(CompilerConfiguration::OutputDir);
    bf::path appDir = config.getStringValue(CompilerConfiguration::ApplicationDir);
    int numLevelsUpFromOutDirToAppDir = -1;
    if (!Utility::makeRelativePath(outDir, appDir, numLevelsUpFromOutDirToAppDir)) {
        numLevelsUpFromOutDirToAppDir = -1;
    }
    return numLevelsUpFromOutDirToAppDir;
}

void Utility::makeAppDirRelativePathOutputDirRelative(bf::path& path)
{
    static int numLevelsUpFromOutDirToAppDir = findNumLevelsUpFromOutputDirToAppDir();
    if (numLevelsUpFromOutDirToAppDir <= 0) {
        return;
    }
    for (int i = 0; i < numLevelsUpFromOutDirToAppDir; ++i) {
        path = ".." / path;
    }
}

void Utility::removeFile(const bf::path& file, bool dieOnError /*=true*/)
{
    if (!bf::exists(file) && !bf::symbolic_link_exists(file)) {
        return;
    }
    // note: symbolic_link_exists is depreciated in later versions of boost
    // filesystem. It checks if the given path is a sym link (does not matter if
    // the pointed file/directory exists of not). The regular exists function
    // always follows sym links (not so obvious).
    try {
        bf::remove(file);
    } catch (bf::filesystem_error const& e) {
        if (dieOnError) {
            SysOut::die(SPL_CORE_CANNOT_REMOVE_FILE(file.string(), e.what()));
        }
    } catch (...) {
        if (dieOnError) {
            SysOut::die(
              SPL_CORE_CANNOT_REMOVE_FILE(file.string(), RuntimeUtility::getErrorNoStr()));
        }
    }
}

void Utility::removeDirectory(const bf::path& dir, bool evenIfNonEmpty, bool dieOnError /*=true*/)
{
    if (!bf::exists(dir) || !bf::is_directory(dir)) {
        return;
    }
    try {
        if (bf::is_empty(dir)) {
            bf::remove(dir);
        } else if (evenIfNonEmpty) {
            bf::remove_all(dir);
            if (bf::exists(dir)) {
                for (bf::directory_iterator it = bf::directory_iterator(dir);
                     it != bf::directory_iterator(); ++it) {
                    removeDirectory(*it, true);
                }
            }
        }
    } catch (bf::filesystem_error const& e) {
        if (dieOnError) {
            SysOut::die(SPL_CORE_CANNOT_REMOVE_DIRECTORY(dir.string(), e.what()));
        }
    } catch (...) {
        if (dieOnError) {
            SysOut::die(
              SPL_CORE_CANNOT_REMOVE_DIRECTORY(dir.string(), RuntimeUtility::getErrorNoStr()));
        }
    }
}

void Utility::createDirectory(const bf::path& dir, bool dieOnError /*=true*/)
{
    // Already there?
    if (bf::exists(dir) && bf::is_directory(dir)) {
        return;
    }

    // Is the parent directory present?
    if (!dir.empty()) {
        bf::path pdir = dir.branch_path();
        if (!pdir.empty() && !(bf::exists(pdir) && bf::is_directory(pdir))) {
            Utility::createDirectory(pdir);
        }
    }

    // Parent must be empty, or a proper directory
    try {
        bf::create_directory(dir);
        // Set my permission to that of my parent
        bf::path pdir = dir.branch_path();
        if (pdir.empty()) {
            pdir = ".";
        }
        struct stat pdirStat;
        stat(pdir.string().c_str(), &pdirStat);
        chmod(dir.string().c_str(), pdirStat.st_mode);
    } catch (const bf::filesystem_error& e) {
        if (dieOnError) {
            SysOut::die(SPL_CORE_CANNOT_CREATE_DIRECTORY(dir.string(), e.what()));
        }
    } catch (...) {
        if (dieOnError) {
            SysOut::die(
              SPL_CORE_CANNOT_CREATE_DIRECTORY(dir.string(), RuntimeUtility::getErrorNoStr()));
        }
    }
}

class CppKeywordClasher
{
  public:
    /// Constructor
    CppKeywordClasher();

    /// ensure an id doesn't clash with C++ keyword
    /// @param id variable to remap
    /// @return 'id' if okay, or remapped one if not
    const string& remapId(const string& id);

    /// Is id a reserved namespace?   This includes C++ keywords and certain directory
    /// names not checked by spl-make-toolkit
    /// @param id name to check
    /// @return 'true' if id is a problem.
    bool reservedDirectory(const string& id);

    /// Is id a reserved C++ keyword?
    /// @param id name to check
    /// @return 'true' if id is a problem.
    bool reservedName(const string& id);

  private:
    tr1::unordered_map<string, string> _map;
    tr1::unordered_set<string> _directories;
};

CppKeywordClasher::CppKeywordClasher()
{
    // Populate map
    const char* keywords[] = { "and",
                               "and_eq",
                               "asm",
                               "auto",
                               "bitand",
                               "bitor",
                               "bool",
                               "case",
                               "catch",
                               "char",
                               "class",
                               "compl",
                               "const",
                               "const_cast",
                               "default",
                               "delete",
                               "do",
                               "double",
                               "dynamic_cast",
                               "enum",
                               "explicit",
                               "export",
                               "extern",
                               "float",
                               "friend",
                               "goto",
                               "inline",
                               "int",
                               "long",
                               "namespace",
                               "new",
                               "not",
                               "not_eq",
                               "operator",
                               "or",
                               "or_eq",
                               "private",
                               "protected",
                               "public",
                               "register",
                               "reinterpret_cast",
                               "short",
                               "signed",
                               "sizeof",
                               "static",
                               "static_cast",
                               "struct",
                               "switch",
                               "template",
                               "this",
                               "throw",
                               "try",
                               "typedef",
                               "typeid",
                               "typename",
                               "union",
                               "unsigned",
                               "using",
                               "virtual",
                               "void",
                               "volatile",
                               "wchar_t",
                               "while",
                               "xor",
                               "xor_eq",
                               NULL };
    for (const char** p = keywords; *p; p++) {
        string s(*p);
        _map.insert(make_pair(s, "id$" + s));
    }

    // Reserved names - mostly from spl-make-toolkit
    const char* directories[] = { "lib", "include", "samples", "output", "data",    "etc",
                                  "doc", "opt",     "bin",     "impl",   "license", NULL };

    // These are okay for variables, but bad for directories - map to themselves
    for (const char** p = directories; *p; p++) {
        string s(*p);
        _directories.insert(s);
    }
}

bool CppKeywordClasher::reservedName(const string& id)
{
    tr1::unordered_map<string, string>::const_iterator it;
    it = _map.find(id);
    // If it is not in the map, it is okay.
    // If it is in the map, but the mapped id is the same, then it is okay
    return it != _map.end();
}

bool CppKeywordClasher::reservedDirectory(const string& id)
{
    return _directories.find(id) != _directories.end();
}

const string& CppKeywordClasher::remapId(const string& id)
{
    tr1::unordered_map<string, string>::const_iterator it;
    it = _map.find(id);
    if (it == _map.end()) {
        return id;
    }
    return it->second;
}

static CppKeywordClasher remapper;

bool Utility::reservedNamespace(const string& id)
{
    return id == "streams" || remapper.reservedDirectory(id) || remapper.reservedName(id);
}

bool Utility::reservedDirectory(const string& id)
{
    return remapper.reservedDirectory(id);
}

const string& Utility::remapId(const string& id)
{
    return remapper.remapId(id);
}

bool Utility::reservedName(const string& id)
{
    return remapper.reservedName(id);
}

class TypeChecker : public AstVisitor
{
  public:
    TypeChecker(bool& error)
      : AstVisitor(Debug::None)
      , _error(error)
    {}

    void visit(AstNode& ast) { AstVisitor::visit(ast); }

    void visitTypeDims(AstNode& ast)
    {
        AstNode& e = AST::typeDims::expr(ast);
        if (!AST::intLiteralT::is(e)) {
            _error = true;
        }
    }

  private:
    bool& _error;
};

bool Utility::validSPLType(const string& type)
{
    bool error;
    SPLAst& result = SPL::SPLAst::SPLTypeFromString("", type, error);
    if (error) {
        return false;
    }
    AstNode& ast = result.ast();
    TypeChecker tCheck(error);
    tCheck.visit(ast);
    return !error;
}

typedef tr1::unordered_map<string, const RootTyp*> TypeMap;
static TypeMap typeMap;

const RootTyp* Utility::splType(const string& type)
{
    TypeMap::const_iterator it = typeMap.find(type);
    if (it != typeMap.end()) {
        return it->second;
    }

    bool error;
    SPLAst& result = SPL::SPLAst::SPLTypeFromString("", type, error);
    if (error) {
        typeMap.insert(make_pair(type, (const RootTyp*)NULL));
        return NULL;
    }

    AstNode& ast = result.ast();
    if (AST::idT::is(ast)) {
        typeMap.insert(make_pair(type, (const RootTyp*)NULL));
        return NULL; // must be custom literal
    }

    ParserContext pCtx("dummy");
    NameDefFinder::run(pCtx, ast);
    const RootTyp& t = astSymbol(ast).type();
    typeMap.insert(make_pair(type, &t));
    return &t;
}

void Utility::splDoc::parse(const string& comment)
{
    // Handle the empty case
    if (comment.empty()) {
        return;
    }

    assert(comment.substr(0, 3) == "/**");

    // remove comment chars
    string c = Distillery::trim(comment.substr(3, comment.size() - 5));
    if (c.empty()) {
        return;
    }

    istringstream s(c);

    // Grab the short description
    getline(s, _shortDescription);

    bool readingDescription = true;
    while (s) {
        string l;
        getline(s, l);
        l = Distillery::trim(l);
        if (l.empty()) {
            continue;
        }
        if (l[0] == '@') {
            readingDescription = false;
            istringstream oneLine(l);
            string token;
            string name;
            oneLine >> token >> name >> skipws;
            getline(oneLine, l);
            l = Distillery::trim(l);
            if (token == "@input") {
                _map.insert(make_pair("input " + name, l));
            } else if (token == "@output") {
                _map.insert(make_pair("output " + name, l));
            } else if (token == "@param") {
                _map.insert(make_pair("param " + name, l));
            }
        } else if (readingDescription) {
            if (!_longDescription.empty()) {
                _longDescription += '\n';
            }
            _longDescription += l;
        }
    }
}

const string* Utility::splDoc::find(const string& key) const
{
    tr1::unordered_map<string, string>::const_iterator it = _map.find(key);
    if (it == _map.end()) {
        return NULL;
    }
    return &it->second;
}

bf::path Utility::cleanPath(const bf::path& path)
{
    bf::path res;
    for (bf::path::iterator it = path.begin(); it != path.end(); ++it) {
        const string& c = (*it).string();
        if (c == ".") {
            continue; // We can ignore ./
        }
        if (c == "..") {
            if (bf::is_symlink(res) || res.empty() || res.filename() == "..") {
                res /= c; // We have to save it
            } else {
                res = res.parent_path(); // go up one level
            }
        } else {
            res /= c;
        }
    }
    // Don't remove everything (in case we were passed '.')
    if (res.empty()) {
        return ".";
    }
    return res;
}

static unsigned char toHex(unsigned int c)
{
    c &= 0xf;
    if (c < 10) {
        return '0' + c;
    }
    return 'A' + (c - 10);
}

static unsigned char toInt(unsigned char c)
{
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return c - '0';
}

string Utility::uriEncode(const string& filename)
{
    string result;
    string::const_iterator it;
    for (it = filename.begin(); it != filename.end(); it++) {
        char c = *it;
        if (isalnum(c)) {
            result += c;
            continue;
        }
        switch (c) {
            case '.':
            case '-':
            case '~':
            case '_': // standard unreservered characters
            case '/': // directory separator
                result += c;
                continue;
            default:;
        }

        // All the rest need to be percent encoded;
        result += '%';
        result += toHex(c >> 4);
        result += toHex(c);
    }
    return result;
}

string Utility::uriDecode(const string& input)
{
    string result;
    string::const_iterator it;
    for (it = input.begin(); it != input.end(); it++) {
        if (*it == '%') {
            char c1 = *++it;
            char c2 = *++it;
            result += static_cast<char>((toInt(c1) << 4) | toInt(c2));
            continue;
        }
        result += *it;
    }
    return result;
}

string Utility::convertAntPatternToRegEx(const string& pattern)
{
    string regEx;
    int32_t length = pattern.size();
    for (int32_t i = 0; i < length; ++i) {
        char c = pattern[i];
        if (c == '.') {
            // A dot needs to be escaped as it is not a wild card
            regEx.push_back('\\');
            regEx.push_back(c);
        } else if (c == '?') {
            // A '?' represents an arbitrary char so it converts to a '.'
            regEx.push_back('.');
        } else if (c == '/') {
            // If we have '/**' consume the slash and add a '.*' to the pattern
            if (i < length - 2 && pattern[i + 1] == '*' && pattern[i + 2] == '*') {
                regEx.append(".*");
                i += 2;
                // If we have a '/' following the '**', consume it too
                if (i < length - 1 && pattern[i + 1] == '/') {
                    ++i;
                }
            } else {
                regEx.push_back(c);
            }
        } else if (c == '*') {
            // A single star maps to '[^/]*'
            // A double star maps to '.*' as does '**/'
            if (i < length - 1 && pattern[i + 1] == '*') {
                regEx.append(".*");
                ++i;
                if (i < length - 1 && pattern[i + 1] == '/') {
                    ++i;
                }
            } else {
                regEx.append("[^/]*");
            }
        } else {
            regEx.push_back(c);
        }
    }

    // All patterns have to match exactly
    regEx.push_back('$');
    return regEx;
}
