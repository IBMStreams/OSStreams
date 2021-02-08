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

#pragma once

#include <UTILS/DistilleryApplication.h>

namespace adl2dot {

class Application : public UTILS_NAMESPACE::DistilleryApplication
{
  public:
    int run(const UTILS_NAMESPACE::arg_vector_t& args);

  private:
    void getArguments(option_vector_t& options);
    std::ostream& printDescription(std::ostream& o) const;

    void processFileOption(const option_t* option, const char* value);
    void processSchemaOption(const option_t* option, const char* value);

    std::string m_fileName;
    std::string m_schema;
};
}
