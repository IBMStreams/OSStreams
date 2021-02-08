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

#ifndef STANDALONE_MODEL_IMPL_H
#define STANDALONE_MODEL_IMPL_H

#include <SPL/Core/ModelHelper.h>
#include <SPL/Core/standaloneModel.h> // xsd-generated

namespace SPL {

namespace Standalone {

using namespace xmlns::prod::streams::spl::standalone;

class StandaloneModel
{
  public:
    StandaloneModel() {}
    StandaloneModel(const StandaloneModel& ot);
    const StandaloneModel& operator=(const StandaloneModel& ot);

    std::auto_ptr<standaloneModelType> toXsdInstance() const;

    void getCodeGenSignature(std::ostream& ostr) const;

  private:
};

}
}

#endif /* STANDALONE_MODEL_IMPL_H */
