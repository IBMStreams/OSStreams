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

#ifndef SPL_XMLDIAG_H
#define SPL_XMLDIAG_H

#include <SPL/Core/CompilerError.h>
#include <SPL/Utility/SourceLocation.h>
#include <SPL/Utility/SysOut.h>
#include <string>
#include <xsd/cxx/xml/error-handler.hxx>

namespace SPL {
namespace XMLDiag {

/// Class that handles printing of XML parsing diagnostics
class XMLDiagPrinter : public ::xsd::cxx::xml::error_handler<char>
{
  public:
    XMLDiagPrinter(const std::string& fileName, int line = 0, int column = 0)
      : _fileName(fileName)
      , _line(line)
      , _column(column)
      , _errorsAsWarnings(false)
    {}

    void setTreatErrorsAsWarnings(bool errorsAsWarnings) { _errorsAsWarnings = errorsAsWarnings; }

    bool handle(const std::string& id,
                unsigned long line,
                unsigned long column,
                severity sev,
                const std::string& message)
    {
        SourceLocation loc(_fileName, line, column);
        if (sev == severity::warning || _errorsAsWarnings) {
            SysOut::warnln(SPL_CORE_XML_WARNING(message), loc);
        } else {
            SysOut::errorln(SPL_CORE_XML_ERROR(message), loc);
        }
        // try to continue
        return true;
    }

  private:
    std::string _fileName;
    int _line;
    int _column;
    bool _errorsAsWarnings;
};
} // namespace XMLDiag
} // namespace SPL

#endif /*SPL_XMLDIAG_H*/
