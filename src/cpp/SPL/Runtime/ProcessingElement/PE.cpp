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

#include <SPL/Runtime/ProcessingElement/PE.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/Utility/Singleton.t>

using namespace SPL;
using namespace std;

template class Singleton<SPL::PE>;    // explicitly instantiate the singleton here
template class Singleton<SPL::SplPE>; // explicitly instantiate the singleton here

#if 0
PE::PE()
    : Singleton<PE>(*this),
      impl_(new PEImpl(*this))

{}

PE::PE(bool standalone)
    : Singleton<PE>(*this),
      impl_(new PEImpl(*this, standalone, NULL))
{}
#endif

PE::PE(bool standalone, std::vector<std::string>* javaVMArgs)
  : Singleton<PE>(*this)
  , javaVMArgs_(javaVMArgs)
{}

PE::~PE() {}

BasePEImpl& PE::getImpl()
{
    assert(false);
    return *(BasePEImpl*)NULL;
}

#if 0
extern "C" Streams::BasePE* makeStandalonePE()
{
    return static_cast<Streams::BasePE*>(new PE(true));
}
#endif

#if 0
PE::PE()
    : Singleton<PE>(*this),
      impl_(new PEImpl(*this))

{}
#endif

SplPE::SplPE(bool standalone)
  : Singleton<SplPE>(*this)
  , impl_(new PEImpl(*this, standalone))
{}

#if 0
PE::PE(bool standalone, std::vector<std::string> * javaVMArgs)
    : Singleton<PE>(*this),
      javaVMArgs_(javaVMArgs)
{}
#endif

SplPE::~SplPE() {}

BasePEImpl& SplPE::getImpl()
{
    return *impl_;
}
