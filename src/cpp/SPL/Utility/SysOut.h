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

#ifndef SPL_SYS_OUT_H
#define SPL_SYS_OUT_H

#include <SPL/Runtime/Utility/Visibility.h>
#include <SPL/Utility/SourceLocation.h>

#include <boost/format.hpp>
#include <iostream>
#include <string>

namespace SPL {

class FormattableMessage;

class DLL_PUBLIC SysOut
{
  private:
    static std::ostream& write(const std::string& text, std::ostream& = std::cerr);
    static std::ostream& write(const boost::format& text, std::ostream& = std::cerr);
    static std::ostream& write(const std::string& file,
                               int line,
                               int col,
                               const std::string& text,
                               std::ostream& = std::cerr);
    static std::ostream& write(const std::string& file,
                               int line,
                               int col,
                               const boost::format& text,
                               std::ostream& = std::cerr);
    static std::ostream& write(const std::string& file,
                               int line,
                               const std::string& text,
                               std::ostream& = std::cerr);
    static std::ostream& write(const std::string& file,
                               int line,
                               const boost::format& text,
                               std::ostream& = std::cerr);
    static std::ostream& write(const std::string& file,
                               const std::string& text,
                               std::ostream& = std::cerr);
    static std::ostream& write(const std::string& file,
                               const boost::format& text,
                               std::ostream& = std::cerr);

    static std::ostream& errorOrWarnln(const FormattableMessage& text,
                                       const SourceLocation& loc,
                                       std::ostream& os,
                                       bool isContinued,
                                       bool isError);

    static void writeSeverityAndMessageId(const FormattableMessage& text, std::ostream& os);
    static void writeLocSeverityAndMessage(const SourceLocation& loc,
                                           const FormattableMessage& text,
                                           std::ostream& os);

  public:
    static void setErrorTrap(unsigned maxErrors, bool dieAtEnd);
    static void endErrorTrap();
    static void updateErrorTrap();
    static unsigned getNumErrorsInCurrentTrap();

    static std::ostream& writeln(const std::string& text, std::ostream& = std::cerr);
    static std::ostream& writeln(const boost::format& text, std::ostream& = std::cerr);
    static std::ostream& writeln(const FormattableMessage& text, std::ostream& = std::cerr);

    static std::ostream& writeln(const std::string& file,
                                 int line,
                                 int col,
                                 const std::string& text,
                                 std::ostream& = std::cerr);
    static std::ostream& writeln(const std::string& file,
                                 int line,
                                 int col,
                                 const boost::format& text,
                                 std::ostream& = std::cerr);
    static std::ostream& writeln(const std::string& file,
                                 int line,
                                 const std::string& text,
                                 std::ostream& = std::cerr);
    static std::ostream& writeln(const std::string& file,
                                 int line,
                                 const boost::format& text,
                                 std::ostream& = std::cerr);
    static std::ostream& writeln(const std::string& file,
                                 const std::string& text,
                                 std::ostream& = std::cerr);
    static std::ostream& writeln(const std::string& file,
                                 const boost::format& text,
                                 std::ostream& = std::cerr);

    // write only in verbose mode
    static std::ostream& verboseln(const std::string& text, std::ostream& = std::cerr);
    static std::ostream& verboseln(const boost::format& text, std::ostream& = std::cerr);
    static std::ostream& verboseln(const FormattableMessage& text, std::ostream& = std::cerr);
    static std::ostream& verboseln(const std::string& file,
                                   int line,
                                   int col,
                                   const std::string& text,
                                   std::ostream& = std::cerr);
    static std::ostream& verboseln(const std::string& file,
                                   int line,
                                   int col,
                                   const boost::format& text,
                                   std::ostream& = std::cerr);
    static std::ostream& verboseln(const std::string& file,
                                   int line,
                                   const std::string& text,
                                   std::ostream& = std::cerr);
    static std::ostream& verboseln(const std::string& file,
                                   int line,
                                   const boost::format& text,
                                   std::ostream& = std::cerr);
    static std::ostream& verboseln(const std::string& file,
                                   const std::string& text,
                                   std::ostream& = std::cerr);
    static std::ostream& verboseln(const std::string& file,
                                   const boost::format& text,
                                   std::ostream& = std::cerr);

    // Output messages intended for user consumption
    static std::ostream& errorln(const FormattableMessage& text,
                                 const SourceLocation& = SourceLocation::invalidSourceLocation,
                                 std::ostream& = std::cerr);
    static std::ostream& detailsErrorln(
      const FormattableMessage& text,
      const SourceLocation& = SourceLocation::invalidSourceLocation,
      std::ostream& = std::cerr);
    static std::ostream& warnln(const FormattableMessage& text,
                                const SourceLocation& = SourceLocation::invalidSourceLocation,
                                std::ostream& = std::cerr);
    static std::ostream& detailsWarnln(
      const FormattableMessage& text,
      const SourceLocation& = SourceLocation::invalidSourceLocation,
      std::ostream& = std::cerr);
    static void die(const FormattableMessage& text,
                    const SourceLocation& = SourceLocation::invalidSourceLocation,
                    int err = EXIT_FAILURE,
                    std::ostream& = std::cerr);

    static void debug(const std::string& text);
    static void debug(const boost::format& text);
};

class AutoErrorTrap
{
  public:
    /// @param dieAtEndIfErrors true if the trap results in a die at the
    /// scope-end if there are errors, yet the maxErrors is not reached
    /// @param maxErrors maximum number of errors tolerated, when this
    /// number is reached the error trap will result in a die
    AutoErrorTrap(bool dieAtEndIfErrors = true,
                  unsigned maxErrors = std::numeric_limits<unsigned>::max())
    {
        SysOut::setErrorTrap(maxErrors, dieAtEndIfErrors);
    }
    ~AutoErrorTrap() { SysOut::endErrorTrap(); }
    unsigned getNumErrorsInCurrentTrap()
    {
        {
            return SysOut::getNumErrorsInCurrentTrap();
        }
    }
};
}

#endif /* SPL_SYS_OUT_H */
