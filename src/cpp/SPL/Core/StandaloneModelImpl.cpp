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

#include <SPL/Core/StandaloneModelImpl.h>

#include <SPL/Core/BaseMangler.h>
#include <SPL/Utility/Debug.h>

using namespace std;
using namespace SPL;
using namespace SPL::Standalone;

StandaloneModel::StandaloneModel(const StandaloneModel& ot)
{
    this->operator=(ot);
}

const StandaloneModel& StandaloneModel::operator=(const StandaloneModel& ot)
{
    return *this;
}

auto_ptr<standaloneModelType> StandaloneModel::toXsdInstance() const
{
    auto_ptr<standaloneModelType> model(new standaloneModelType());
    return model;
}

void StandaloneModel::getCodeGenSignature(ostream& ostr) const
{
    ostringstream oss;
    oss << SPL_SIG_SEP;
    string sig = BaseMangler::compress(oss.str(), 0, false, false);
    ostr << sig;
    SPLDBG("Standalone Signature: " << sig, Debug::ShowSig);
}
