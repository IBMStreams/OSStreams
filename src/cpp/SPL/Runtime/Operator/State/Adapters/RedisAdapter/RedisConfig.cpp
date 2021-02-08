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

/*
 * Implementation of the SPL::RedisConfig class.
 */

#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisConfig.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisServerPoolActiveReplica.h>

using namespace std;
using namespace SPL;

RedisServerPool* RedisConfig::parseConfiguration(const std::string& configStr)
{
    return static_cast<RedisServerPool*>(
      RedisServerPoolActiveReplica::parseConfiguration(configStr));
}

bool RedisConfig::verifyConfiguration(const std::string& configStr, std::stringstream& errMsg)
{
    return RedisServerPoolActiveReplica::verifyConfiguration(configStr, errMsg);
}
