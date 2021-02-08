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

#ifndef POOL_POPULATOR_H
#define POOL_POPULATOR_H

#include <SPL/Runtime/Utility/Singleton.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <UTILS/HashMapHelpers.h>

#include <string>
#include <vector>

namespace SPL {
class PoolCollection;
class CompilerConfiguration;

class DLL_PUBLIC PoolPopulator : public Singleton<PoolPopulator>
{
  public:
    PoolPopulator();
    /// Populate the pools with node names
    /// @param pools pool collection to populate
    void populate(PoolCollection& pools);

    /// Convert host name to canonical host name, unless compiler
    /// configuration has it turned off
    /// @param hostname host name to be converted
    /// @return canonical host name
    /// @throws SPLCompilerNoValidCanonicalHostNames if an ip address
    /// cannot be found, or the ip address does not resolve back to a host
    /// other than the localhost or localhost.*
    static std::string convertToCanonicalName(std::string const& hostname);

  private:
    CompilerConfiguration& config_;
};
};

#endif /* POOL_POPULATOR_H  */
