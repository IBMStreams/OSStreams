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

#ifndef SPL_CORE_SPLDOC_MODEL_BUILDER_H
#define SPL_CORE_SPLDOC_MODEL_BUILDER_H

#include <string>

namespace xmlns {
namespace prod {
namespace streams {
namespace spl {
namespace sourceCode {
class splDocType;
}
}
}
}
}

namespace SPL {
class SourceLocation;
class SPLDocModelBuilder
{
  public:
    static xmlns::prod::streams::spl::sourceCode::splDocType* parse(std::string const& comment,
                                                                    SourceLocation const& loc);
};
}

#endif /* SPL_CORE_SPLDOC_MODEL_BUILDER_H */
