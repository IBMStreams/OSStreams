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

#ifndef SPL_RUNTIME_PROCESSING_ELEMENT_ELASTIC_THREAD_ADAPTER_H
#define SPL_RUNTIME_PROCESSING_ELEMENT_ELASTIC_THREAD_ADAPTER_H

#include <istream>
#include <limits>
#include <stdint.h>
#include <vector>

namespace SPL {
class ElasticThreadAdapter
{
  public:
    ElasticThreadAdapter();
    ~ElasticThreadAdapter();
    void initialize(uint32_t minThreads, uint32_t maxThreads, uint32_t activeThreads);
    uint32_t calculateThreads(double throughput, bool& loadChanged);
    void untrustAbove();
    void untrustLevel(uint32_t level, double t);

  private:
    struct ThreadInfo
    {
        double lastThroughput;
        double firstThroughput;
        int64_t lastTime;
        bool trusted;

        ThreadInfo()
          : lastThroughput(std::numeric_limits<double>::infinity())
          , firstThroughput(std::numeric_limits<double>::quiet_NaN())
          , lastTime(-1)
          , trusted(false)
        {}
    };

    class CPUStat
    {
      public:
        CPUStat()
          : user(0)
          , nice(0)
          , system(0)
          , idle(0)
          , iowait(0)
          , irq(0)
          , softirq(0)
          , steal(0)
          , guest(0)
        {}

        CPUStat operator-(const CPUStat& other)
        {
            CPUStat res;
            res.user = user - other.user;
            res.nice = nice - other.nice;
            res.system = system - other.system;
            res.idle = idle - other.idle;
            res.iowait = iowait - other.iowait;
            res.irq = irq - other.irq;
            res.softirq = softirq - other.softirq;
            res.steal = steal - other.steal;
            res.guest = guest - other.guest;
            return res;
        }

        friend std::istream& operator>>(std::istream& is, CPUStat& stat)
        {
            is >> stat.user >> stat.nice >> stat.system >> stat.idle >> stat.iowait >> stat.irq >>
              stat.softirq >> stat.steal >> stat.guest;
            return is;
        }

        friend std::ostream& operator<<(std::ostream& os, CPUStat& stat)
        {
            os << stat.user << " " << stat.nice << " " << stat.system << " " << stat.idle << " "
               << stat.iowait << " " << stat.irq << " " << stat.softirq << " " << stat.steal << " "
               << stat.guest;
            return os;
        }

        bool isDefault()
        {
            return user == 0 && nice == 0 && system == 0 && idle == 0 && iowait == 0 && irq == 0 &&
                   softirq == 0 && steal == 0 && guest == 0;
        }

        double usage()
        {
            uint64_t total = user + nice + system + idle + iowait + irq + softirq + steal + guest;
            return (user + nice + system) / static_cast<double>(total);
        }

      private:
        uint64_t user;
        uint64_t nice;
        uint64_t system;
        uint64_t idle;
        uint64_t iowait;
        uint64_t irq;
        uint64_t softirq;
        uint64_t steal;
        uint64_t guest;
        // As of Linux 2.6.33, there is also a 10th field, guest_nice, but we have to support
        // versions of Linux prior to that. The easiest solution is to ignore it, but we are
        // potentially undercounting.
    };

    enum LoadChange
    {
        LessLoad,
        MoreLoad,
        PotentialNoise,
        UnknownLoadChange
    };

    uint32_t _belowLevel;
    uint32_t _currLevel;
    uint32_t _aboveLevel;
    uint32_t _minLevel;
    uint32_t _maxLevel;
    int64_t _time; // logical adaptation time
    int32_t _noiseDetector;
    std::vector<ThreadInfo> _threadInfos;
    double _changeSensitivity;
    CPUStat _lastStat;

    LoadChange checkForChangeInLoadBasedOnThroughput(double throughput);
    LoadChange pingNoiseDetector(LoadChange direction);
    bool improvementTrendBelow(double throughput);
    bool improvementTrendAbove(double throughput);
    bool trustBelow();
    bool trustAbove();
    void untrustOtherData(double throughput);
    void decreaseLevel();
    void increaseLevel();
    bool isCPUUsageAcceptable();

    static CPUStat getCPUStat();
};
}

#endif // SPL_RUNTIME_PROCESSING_ELEMENT_ELASTIC_THREAD_ADAPTER_H
