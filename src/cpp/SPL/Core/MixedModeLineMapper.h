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

#ifndef MIXED_MODE_LINE_MAPPER_H
#define MIXED_MODE_LINE_MAPPER_H

#include <SPL/Runtime/Utility/Singleton.h>
#include <SPL/Utility/SourceLocation.h>
#include <UTILS/HashMapHelpers.h>

namespace SPL {
class MixedModeLineMapper : public Singleton<MixedModeLineMapper>
{
  public:
    /// Constructor
    ///
    MixedModeLineMapper();

    /// Destructor
    ///
    ~MixedModeLineMapper();

    /// Maps the given source location to that in the splmm file if there is
    /// a mapping, otherwise it returns the location it was given
    /// @param loc source location in the spl file
    /// @return source location in the splmm file if one exists, 'loc' otherwise
    SourceLocation map(SourceLocation const& loc) const;

    // .spl line -> one or more .splmm lines
    typedef std::tr1::unordered_map<int, std::vector<int> > LineMap;
    // file id -> line map pointer or null if none exists
    typedef std::tr1::unordered_map<int, LineMap*> FileToLineMap;

  private:
    mutable FileToLineMap fileToLineMap_;

    bool hasIndexFile(SourceLocation const& loc, std::string& idxFile) const;
    void setLineMapping(std::string const& idxFile, LineMap& lineMap) const;
};
}
#endif /* MIXED_MODE_LINE_MAPPER_H */
