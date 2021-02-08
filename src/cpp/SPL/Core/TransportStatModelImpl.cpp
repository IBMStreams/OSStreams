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

#include <SPL/Core/TransportStatModelImpl.h>

using namespace std;
using namespace std::tr1;
using namespace SPL;
using namespace SPL::TransportStat;
using boost::shared_ptr;

TransportStatModel::TransportStatModel(const transportStatModelType& xml)
  : productVersion_(xml.productVersion())
  , cpuKind_(xml.cpuKind())
  , bogoMips_(xml.bogoMips())
{
    typedef transportStatModelType::transportProfile_sequence::const_iterator myiter;
    myiter itbeg = xml.transportProfile().begin();
    myiter itend = xml.transportProfile().end();
    for (myiter it = itbeg; it != itend; ++it) {
        TransportProfilePtr tpptr(new TransportProfile(*it));
        transportProfiles_.push_back(tpptr);
    }
}

TransportProfile::TransportProfile(const transportProfileType& xml)
{
    transportKind_.reset(new TransportKindType(xml.transportKind()));
    typedef transportProfileType::tuplePerf_sequence::const_iterator myiter;
    myiter itbeg = xml.tuplePerf().begin();
    myiter itend = xml.tuplePerf().end();
    for (myiter it = itbeg; it != itend; ++it) {
        TuplePerfPtr tpptr(new TuplePerf(*it));
        tuplePerfs_.push_back(tpptr);
    }
}

TuplePerf::TuplePerf(const tuplePerfType& xml)
  : size_(xml.size())
  , rate_(xml.rate())
  , senderUtil_(xml.senderUtil())
  , receiverUtil_(xml.receiverUtil())
{}
