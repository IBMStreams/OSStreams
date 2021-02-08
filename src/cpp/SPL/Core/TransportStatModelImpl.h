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

#ifndef TRANSPORT_STAT_MODEL_IMPL_H
#define TRANSPORT_STAT_MODEL_IMPL_H

#include <SPL/Core/ModelHelper.h>
#include <SPL/Core/commonModel.h>
#include <SPL/Core/transportStatModel.h>
#include <SPL/Runtime/Utility/Visibility.h>

namespace trmodel = xmlns::prod::streams::spl::transportStat;

namespace SPL {
namespace TransportStat {
using namespace xmlns::prod::streams::spl::transportStat;
using namespace xmlns::prod::streams::spl::common;
using namespace xml_schema;

class TransportProfile;
typedef boost::shared_ptr<TransportProfile> TransportProfilePtr;

class TransportKindType;
typedef boost::shared_ptr<TransportKindType> TransportKindTypePtr;

class TuplePerf;
typedef boost::shared_ptr<TuplePerf> TuplePerfPtr;

class DLL_PUBLIC TransportStatModel
{
  public:
    TransportStatModel(const transportStatModelType& xml);

    const std::string& getProductVersion() const { return productVersion_; }
    const std::string& getCpuKind() const { return cpuKind_; }
    const unsigned_long getBogoMips() const { return bogoMips_; }
    const std::vector<TransportProfilePtr>& getTransportProfiles() const
    {
        return transportProfiles_;
    }

  private:
    std::string productVersion_;
    std::string cpuKind_;
    unsigned_long bogoMips_;
    std::vector<TransportProfilePtr> transportProfiles_;
};

class TransportProfile
{
  public:
    TransportProfile(const transportProfileType& xml);

    const TransportKindType& getTransportKind() const { return *transportKind_; }
    const std::vector<TuplePerfPtr>& getTuplePerfs() const { return tuplePerfs_; }

  private:
    TransportKindTypePtr transportKind_;
    std::vector<TuplePerfPtr> tuplePerfs_;
};

SPL_MODEL_ENUM(TransportKindType, trmodel, transportKindType, tcp, llm_rum_tcp, llm_rum_ib);

class TuplePerf
{
  public:
    TuplePerf(const tuplePerfType& xml);
    TuplePerf(unsigned_long size,
              unsigned_long rate,
              unsigned_int senderUtil,
              unsigned_int receiverUtil)
      : size_(size)
      , rate_(rate)
      , senderUtil_(senderUtil)
      , receiverUtil_(receiverUtil)
    {}
    unsigned_long getSize() const { return size_; }
    unsigned_long getRate() const { return rate_; }
    unsigned_int getSenderUtil() const { return senderUtil_; }
    unsigned_int getReceiverUtil() const { return receiverUtil_; }

  private:
    unsigned_long size_;
    unsigned_long rate_;
    unsigned_int senderUtil_;
    unsigned_int receiverUtil_;
};

typedef boost::shared_ptr<TransportStatModel> TransportStatModelPtr;
};
};

#endif /* TRANSPORT_STAT_MODEL_IMPL_H */
