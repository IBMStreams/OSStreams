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

#ifndef SPL_RUNTIME_PROCESSING_ELEMENT_ELASTIC_PORT_ADAPTER_H
#define SPL_RUNTIME_PROCESSING_ELEMENT_ELASTIC_PORT_ADAPTER_H

#include <SPL/Runtime/ProcessingElement/ScheduledQueue.h>
#include <istream>
#include <limits>
#include <set>
#include <stdint.h>
#include <vector>

namespace SPL {
class ScheduledQueue;
class ElasticPortAdapter
{
    friend class Adjustment;

  public:
    enum AdjustDecision
    {
        NEG_STOP,
        CONTINUE_UP,
        CONTINUE_DOWN,
        RESET,
        CONTINUE
    };
    ElasticPortAdapter(ScheduledQueue& sq);
    void adjust(double& throughput);
    void initialize(AdjustDirection dir);
    void clear();
    void confirmToAdd(vector<uint32_t>& operators, pair<int, int>& range);

  private:
    struct MetaAdjustment
    {
        double throughput;
        int level;
        int aboveLevel;
        int belowLevel;
        int upperBound;
        int lowerBound;
        MetaAdjustment* next;
        MetaAdjustment* prev;
        MetaAdjustment(int level_,
                       int aboveLevel_,
                       int belowLevel_,
                       int upperBound_,
                       int lowerBound_)
          : level(level_)
          , aboveLevel(aboveLevel_)
          , belowLevel(belowLevel_)
          , upperBound(upperBound_)
          , lowerBound(lowerBound_)
        {
            next = prev = NULL;
        }
    };

    class Adjustment
    {
      public:
        Adjustment(double& preThroughput, std::vector<uint32_t>& operators, int& groupNum);
        ~Adjustment();
        AdjustDecision nextStep(bool refine = true);
        void extend(std::vector<uint32_t>& ops, int c);
        void incStat(double& throughput);
        std::vector<uint32_t>& getOperators() { return operators_; }
        std::pair<int, int>& getAdjustRange() { return adjustRange_; }
        std::pair<int, int>& getRange() { return range_; }
        void reset()
        {
            currLevel_ = lastPositivePos_;
            range_.second = currLevel_;
        }
        int getEndPos() { return currLevel_; }
        int getLastPositivePos() { return lastPositivePos_; }
        int getSize() { return size_; }
        double& getMaxThroughput() { return maxThroughput_; }
        int& getCurGroupNum() { return curGroup_; }

      private:
        void initialize();
        bool trendAbove();
        bool trendBelow();
        bool trustAbove();
        bool trustBelow();
        void insertAdjustment(int above, int below);
        void searchMax(ElasticPortAdapter::AdjustDecision& decision);
        std::map<int, MetaAdjustment*> adjustInfo_;
        double maxThroughput_;
        std::vector<uint32_t> operators_;
        int curGroup_;
        int currLevel_;
        MetaAdjustment* currAdjustment_;
        int size_;
        int lastPositivePos_;
        double curThroughput_;
        std::pair<int, int> adjustRange_;
        std::pair<int, int> range_;
    };

  private:
    void addNewAdjustment(double& t);
    void refine(double& t);
    bool extendAdjustment();
    void resetSchedulerQueuePlacement();
    void groupOperators();
    int getHeaviestGroupNum();
    int getLightestGroupNum();
    int getNextGroupNum(int&);
    int getPrevGroupNum(int&);
    void confirmToRemove(vector<uint32_t>& operators, pair<int, int>& range);

  private:
    ScheduledQueue& _sq;
    Adjustment* _curAdjustment;
    std::vector<std::vector<uint32_t> > _perfGroups;
    std::set<uint32_t> _tmpExcludedSet;
    std::vector<uint32_t> _affectedPorts;
    AdjustDirection _trend;
    bool _recordHist;
};
}

#endif // SPL_RUNTIME_PROCESSING_ELEMENT_ELASTIC_PORT_ADAPTER_H
