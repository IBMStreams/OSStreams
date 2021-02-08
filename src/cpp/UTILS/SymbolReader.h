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

#ifndef _SYMBOL_READER_H_
#define _SYMBOL_READER_H_

#include <ostream>
#include <string>
#include <vector>

#include <UTILS/DistilleryException.h>
#include <UTILS/UTILSTypes.h>

UTILS_NAMESPACE_BEGIN

class SymbolReader
{
  protected:
    typedef struct _symbol_t
    {
        std::string file_hash;
        int line;
        int level;
        volatile unsigned long* var;
    } symbol_t;

    void loadSymbol(const std::string& object_file);
    std::vector<symbol_t*> _symbols;

  public:
    class Callback
    {
      public:
        virtual ~Callback();
        virtual void set(volatile unsigned long* addr) = 0;
    };

    SymbolReader();
    SymbolReader(const std::string& object_file);
    ~SymbolReader();

    void iterateByLevel(int level, Callback* cb);
    void iterateByFile(const std::string& filename, Callback* cb);
    void iterateByFileAndLine(const std::string& filename, int line, Callback* cb);
    void iterateByFileAndLevel(const std::string& filename, int level, Callback* cb);

    void setByLevel(int level, bool state = true);
    void setByFile(const std::string& filename, bool state = true);
    void setByFileAndLine(const std::string& filename, int line, bool state = true);
    void setByFileAndLevel(const std::string& filename, int level, bool state = true);

    void processToggleFile(const std::string& argv_0, const std::string& filename);

    friend std::ostream& operator<<(std::ostream& strm, const SymbolReader::symbol_t& sym);
};

std::ostream& operator<<(std::ostream& strm, const SymbolReader::symbol_t& sym);

DECL_EXCEPTION(Distillery, SymbolReader, Distillery);
DECL_EXCEPTION(Distillery, FileReadError, SymbolReader);
DECL_EXCEPTION(Distillery, NoSymbol, SymbolReader);

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
