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

#if (defined(DST_DBG_INST) || defined(DUMP_LINE_NUMBER))
#include <bfd.h>
#endif
#include <cassert>
#include <errno.h>
#include <fstream>

#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/Formatter.h>
#include <UTILS/HashStream.h>
#include <UTILS/RegEx.h>
#include <UTILS/SymbolReader.h>

UTILS_NAMESPACE_USE;
using namespace std;
DEBUG_NAMESPACE_USE

SymbolReader::SymbolReader()
{
    loadSymbol("/proc/self/exe");
}

SymbolReader::SymbolReader(const string& object_file)
{
    loadSymbol(object_file);
}

SymbolReader::~SymbolReader()
{
    for (unsigned int i = 0; i < _symbols.size(); i++) {
        delete _symbols[i];
    }
}

void SymbolReader::loadSymbol(const string& object_file)
{
    RegEx re("__dbgpreproc" // dont touch this line !! otherwise the
                            // instrumentation will modify it
             "___([0-9a-f]+)_([0-9]+)_([0-9])");
    if (!re.match("_TEPKcE61__dbgpreproc___1")) {
        SPCDBG(L_INFO, "Symbols are not instrumented", CORE_UTILS);
        return;
    }

#if (defined(DST_DBG_INST) || defined(DUMP_LINE_NUMBER))
    bfd_set_default_target("i686-pc-linux-gnu");

    bfd* abfd = bfd_openr(object_file.c_str(), NULL);
    if (abfd == NULL) {
        THROW(FileReadError,
              "Unable to read file " << QT(object_file) << ": " << ErrnoFormat(errno));
    }

    if (!bfd_check_format(abfd, bfd_object)) {
    }

    if (!(bfd_get_file_flags(abfd) & HAS_SYMS)) {
        bfd_close(abfd);

        THROW(NoSymbol, "No symbol in file " << QT(object_file));
    }

    long storage_needed = bfd_get_symtab_upper_bound(abfd);

    if (storage_needed < 0) {
        bfd_close(abfd);

        THROW(NoSymbol, "bfd_get_symtab_upper_bound failed");
    }

    if (storage_needed == 0) {
        THROW(NoSymbol, "bfd_get_symtab_upper_bound returned 0");
    }

    asymbol** symbol_table = (asymbol**)malloc(storage_needed);

    long number_of_symbols = bfd_canonicalize_symtab(abfd, symbol_table);

    if (number_of_symbols < 0) {
        free(symbol_table);
        bfd_close(abfd);

        THROW(NoSymbol, "bfd_canonicalize_symtab failed");
    }

    // ofstream f1("bob.1") ;
    // ofstream f2("bob.2") ;

    for (long i = 0; i < number_of_symbols; i++) {
        // f2 << (char)bfd_decode_symclass(symbol_table[i]) << ' ' <<
        // bfd_asymbol_name(symbol_table[i]) << endl ;

        if (symbol_table[i]->flags & BSF_LOCAL || symbol_table[i]->flags & BSF_WEAK) {
            vector<string> res;

            if (re.match(bfd_asymbol_name(symbol_table[i]), res)) {
                symbol_t* sym = new symbol_t;

                sym->file_hash = res[1];
                sym->line = atoi(res[2].c_str());
                sym->level = atoi(res[3].c_str());
                sym->var = (unsigned long*)bfd_asymbol_value(symbol_table[i]);

                // f1 << *sym << endl ;

                _symbols.push_back(sym);
            }
        }
    }

    free(symbol_table);
    bfd_close(abfd);
#endif
}

SymbolReader::Callback::~Callback() {}

class SetSelfCallback : public SymbolReader::Callback
{
  protected:
    bool _state;

  public:
    SetSelfCallback(unsigned long state)
      : _state(state)
    {}

    virtual ~SetSelfCallback() {}

    virtual void set(volatile unsigned long* addr) { *addr = (_state ? 1 : 0); }
};

void SymbolReader::iterateByLevel(int level, SymbolReader::Callback* cb)
{
    vector<symbol_t*>::iterator it(_symbols.begin());
    while (it != _symbols.end()) {
        if ((*it)->level <= level) {
            cb->set((*it)->var);
        }
        it++;
    }
}

void SymbolReader::iterateByFile(const string& filename, SymbolReader::Callback* cb)
{
    SHA1HashStream h;
    h << filename;
    string hash(h.toString());

    vector<symbol_t*>::iterator it(_symbols.begin());
    while (it != _symbols.end()) {
        if ((*it)->file_hash == hash) {
            cb->set((*it)->var);
        }
        it++;
    }
}

void SymbolReader::iterateByFileAndLine(const string& filename,
                                        int line,
                                        SymbolReader::Callback* cb)
{
    SHA1HashStream h;
    h << filename;
    string hash(h.toString());

    vector<symbol_t*>::iterator it(_symbols.begin());
    while (it != _symbols.end()) {
        if ((*it)->line == line) {
            if ((*it)->file_hash == hash) {
                cb->set((*it)->var);
            }
        }
        it++;
    }
}

void SymbolReader::iterateByFileAndLevel(const string& filename,
                                         int level,
                                         SymbolReader::Callback* cb)
{
    SHA1HashStream h;
    h << filename;
    string hash(h.toString());

    vector<symbol_t*>::iterator it(_symbols.begin());
    while (it != _symbols.end()) {
        if ((*it)->level <= level) {
            if ((*it)->file_hash == hash) {
                cb->set((*it)->var);
            }
        }
        it++;
    }
}

void SymbolReader::setByLevel(int level, bool state)
{
    SetSelfCallback cb(state);
    iterateByLevel(level, &cb);
}

void SymbolReader::setByFile(const string& filename, bool state)
{
    SetSelfCallback cb(state);
    iterateByFile(filename, &cb);
}

void SymbolReader::setByFileAndLine(const string& filename, int line, bool state)
{
    SetSelfCallback cb(state);
    iterateByFileAndLine(filename, line, &cb);
}

void SymbolReader::setByFileAndLevel(const string& filename, int level, bool state)
{
    SetSelfCallback cb(state);
    iterateByFileAndLevel(filename, level, &cb);
}

void SymbolReader::processToggleFile(const string& argv_0, const string& filename)
{
    /*

    syntax:

    [bob]

    cpp/UTILS/src/SymbolReader.cpp:123
    cpp/UTILS/src/SymbolReader.cpp:L3
    cpp/UTILS/src/SymbolReader.cpp:*

    */

    char argv_0_buffer[4096];
    strncpy(argv_0_buffer, argv_0.c_str(), 4096);
    char* argv_0_ptr = basename(argv_0_buffer);

    ifstream f(filename.c_str());

    if (!f) {
        SPCDBG(L_WARN, "Cannot open " << QT(filename), CORE_UTILS);
    }

    RegEx re_file("^[ ]*[[]([^]]+)[]][ ]*$");
    RegEx re_one_line("^[ ]*([^:]+):([0-9]+)[ ]*$");
    RegEx re_one_level("^[ ]*([^:]+):L([0-9]+)[ ]*$");
    RegEx re_one_file("^[ ]*([^:]+):\\*[ ]*$");

    bool found_file = false;

    while (f) {
        char line[4096];
        f.getline(line, 4096);

        if (!f) {
            break;
        } else {
            vector<string> vec;
            if (re_file.match(line, vec)) {
                // cout << "XXXXXXXXXXX FILE " << vec[1] << " (" << argv_0 << ")" << endl ;

                if (vec[1] == argv_0_ptr) {
                    found_file = true;
                } else {
                    found_file = false;
                }
            } else {
                if (found_file) {
                    if (re_one_line.match(line, vec)) {
                        // cout << "XXXXXXXXXXX FILE/LINE " << vec[1] << " " << vec[2] << endl ;

                        setByFileAndLine(vec[1], atoi(vec[2].c_str()));
                    } else {
                        if (re_one_file.match(line, vec)) {
                            // cout << "XXXXXXXXXXX FILE " << vec[1] << endl ;

                            setByFile(vec[1]);
                        } else {
                            if (re_one_level.match(line, vec)) {
                                // cout << "XXXXXXXXXXX FILE/LEVEL " << vec[1] << " " << vec[2] <<
                                // endl ;

                                setByFileAndLevel(vec[1], atoi(vec[2].c_str()));
                            } else {
                                // cout << "XXXXXXXXXXX {LINE DOES NOT MATCH} " << line << endl ;
                            }
                        }
                    }
                } else {
                    // cout << "XXXXXXXXXXX {NOT IN FILE} " << line << endl ;
                }
            }
        }
    }
}

ostream& Distillery::operator<<(ostream& strm, const SymbolReader::symbol_t& sym)
{
    strm << "{" << sym.file_hash << ":" << sym.line << ":" << sym.level << ":" << (void*)sym.var
         << "}";

    return strm;
}

IMPL_EXCEPTION(Distillery, SymbolReader, Distillery);
IMPL_EXCEPTION(Distillery, FileReadError, SymbolReader);
IMPL_EXCEPTION(Distillery, NoSymbol, SymbolReader);
