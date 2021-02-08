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

#include <SPL/Runtime/Common/RuntimeDebugAspect.h>
#include <SPL/Runtime/ProcessingElement/ElasticPortAdapter.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <TRC/DistilleryDebug.h>

#include <boost/thread/thread.hpp>

#include <cmath>
#include <fstream>

using namespace SPL;
#define PERF_GROUP_NUM 9
#define PERF_GROUP_SIZE 40
#define GRANULARITY 0.1

/*Adjustment entry related functions */
ElasticPortAdapter::Adjustment::Adjustment(double& prevThroughput,
                                           std::vector<uint32_t>& operators,
                                           int& groupNum)
  : maxThroughput_(prevThroughput)
  , operators_(operators)
  , curGroup_(groupNum)
{
    initialize();
}

void ElasticPortAdapter::Adjustment::initialize()
{
    lastPositivePos_ = -1;
    size_ = (int)operators_.size();
    currLevel_ = size_;
    currAdjustment_ = new MetaAdjustment(size_, size_, 0, size_, 0);
    adjustInfo_[size_] = currAdjustment_;
    range_.first = 0;
    range_.second = size_;
}

ElasticPortAdapter::Adjustment::~Adjustment()
{
    for (std::map<int, MetaAdjustment*>::iterator it = adjustInfo_.begin(); it != adjustInfo_.end();
         ++it) {
        delete it->second;
    }
}

bool ElasticPortAdapter::Adjustment::trustAbove()
{
    if (adjustInfo_.find(currAdjustment_->aboveLevel) == adjustInfo_.end()) {
        return false;
    }
    return true;
}

bool ElasticPortAdapter::Adjustment::trustBelow()
{
    if (adjustInfo_.find(currAdjustment_->belowLevel) == adjustInfo_.end()) {
        return false;
    }
    return true;
}

bool ElasticPortAdapter::Adjustment::trendAbove()
{
    std::map<int, MetaAdjustment*>::iterator above = adjustInfo_.find(currAdjustment_->aboveLevel);
    if (above == adjustInfo_.end()) {
        return false;
    }

    MetaAdjustment* aboveAdjust = above->second;
    if (aboveAdjust->throughput > curThroughput_ &&
        (aboveAdjust->throughput - curThroughput_) > 0.05 * curThroughput_) {
        return true;
    }

    return false;
}

bool ElasticPortAdapter::Adjustment::trendBelow()
{
    std::map<int, MetaAdjustment*>::iterator below = adjustInfo_.find(currAdjustment_->belowLevel);
    if (below == adjustInfo_.end()) {
        return false;
    }

    MetaAdjustment* belowAdjust = below->second;
    if (curThroughput_ > belowAdjust->throughput &&
        (curThroughput_ - belowAdjust->throughput) > belowAdjust->throughput * 0.05) {
        return true;
    }

    return false;
}

/*
 *Evaluate the performance trend between the MetaAdjustment
 */
ElasticPortAdapter::AdjustDecision ElasticPortAdapter::Adjustment::nextStep(bool furtherRefine)
{
    ElasticPortAdapter::AdjustDecision decision;
    if (!furtherRefine) {
        searchMax(decision);
    } else {
        if ((trendBelow() && !trustAbove()) || trendAbove()) {
            // increase scheduler queue count
            int nextLevel_ = (currAdjustment_->upperBound + currLevel_) / 2;
            if ((nextLevel_ - currLevel_) <= 2) {
                // too small adjustment, stop
                searchMax(decision);
            } else {
                int newAboveLevel = currAdjustment_->upperBound;
                int newBelowLevel = currLevel_;
                MetaAdjustment* tmp = currAdjustment_;
                while (tmp->next != NULL) {
                    tmp = tmp->next;
                    // search for the adjustment above the current level
                    if (tmp->level > nextLevel_ && tmp->level < newAboveLevel) {
                        newAboveLevel = tmp->level;
                    }
                    if (tmp->level < nextLevel_ && tmp->level > newBelowLevel) {
                        newBelowLevel = tmp->level;
                    }
                }

                if ((newAboveLevel - nextLevel_) <= 2 || (nextLevel_ - newBelowLevel) <= 2) {
                    // the difference between previous adjustment is too small, stop
                    searchMax(decision);
                } else {
                    adjustRange_.first = currLevel_;
                    adjustRange_.second = nextLevel_;
                    range_.second = nextLevel_;
                    int newUpperBound = currAdjustment_->upperBound;
                    int newLowerBound = currLevel_;
                    currAdjustment_ = new MetaAdjustment(nextLevel_, newAboveLevel, newBelowLevel,
                                                         newUpperBound, newLowerBound);
                    insertAdjustment(newAboveLevel, newBelowLevel);
                    adjustInfo_[nextLevel_] = currAdjustment_;
                    currLevel_ = nextLevel_;
                    decision = CONTINUE_UP;
                }
            }
        } else if (currLevel_ == size_ || !trustBelow() || !trendBelow()) {
            // decrease the scheduler queue count
            int nextLevel_ = (currAdjustment_->lowerBound + currLevel_) / 2;
            if ((currLevel_ - nextLevel_) <= 2) {
                // too small adjustment, stop
                searchMax(decision);
            } else {
                int newAboveLevel = currLevel_;
                int newBelowLevel = currAdjustment_->lowerBound;
                MetaAdjustment* tmp = currAdjustment_;
                while (tmp->prev != NULL) {
                    tmp = tmp->prev;
                    if (tmp->level > nextLevel_ && tmp->level < newAboveLevel) {
                        newAboveLevel = tmp->level;
                    }
                    if (tmp->level < nextLevel_ && tmp->level > newBelowLevel) {
                        newBelowLevel = tmp->level;
                    }
                }

                if ((newAboveLevel - nextLevel_) <= 2 || (nextLevel_ - newBelowLevel) <= 2) {
                    searchMax(decision);
                } else {
                    adjustRange_.second = currLevel_;
                    adjustRange_.first = nextLevel_;
                    range_.second = nextLevel_;
                    int newUpperBound = currAdjustment_->aboveLevel;
                    int newLowerBound = currAdjustment_->lowerBound;
                    currAdjustment_ = new MetaAdjustment(nextLevel_, newAboveLevel, newBelowLevel,
                                                         newUpperBound, newLowerBound);
                    insertAdjustment(newAboveLevel, newBelowLevel);
                    adjustInfo_[nextLevel_] = currAdjustment_;
                    currLevel_ = nextLevel_;
                    decision = CONTINUE_DOWN;
                }
            }
        } else {
            // trendBelow & !trendAbove
            searchMax(decision);
        }
    }
    return decision;
}

void ElasticPortAdapter::Adjustment::insertAdjustment(int above, int below)
{
    std::map<int, MetaAdjustment*>::iterator it_above = adjustInfo_.find(above);
    std::map<int, MetaAdjustment*>::iterator it_below = adjustInfo_.find(below);
    if (it_above != adjustInfo_.end()) {
        MetaAdjustment* aboveAdjust = it_above->second;
        if (aboveAdjust->prev != NULL) {
            aboveAdjust->prev->next = currAdjustment_;
        }
        currAdjustment_->next = aboveAdjust;
        aboveAdjust->prev = currAdjustment_;
    }

    if (it_below != adjustInfo_.end()) {
        MetaAdjustment* belowAdjust = it_below->second;
        if (belowAdjust->next != NULL) {
            belowAdjust->next->prev = currAdjustment_;
        }
        currAdjustment_->prev = belowAdjust;
        belowAdjust->next = currAdjustment_;
    }
}

void ElasticPortAdapter::Adjustment::extend(std::vector<uint32_t>& ops, int c)
{
    curGroup_ = c;
    operators_.insert(operators_.end(), ops.begin(), ops.end());
    size_ = (int)operators_.size();
    currLevel_ = size_;
    adjustRange_.first = range_.second;
    adjustRange_.second = size_;
    range_.first = 0;
    range_.second = size_;
    for (std::map<int, MetaAdjustment*>::iterator it = adjustInfo_.begin(); it != adjustInfo_.end();
         ++it) {
        delete it->second;
    }
    adjustInfo_.clear();
    currAdjustment_ = new MetaAdjustment(size_, size_, 0, size_, 0);
    adjustInfo_[size_] = currAdjustment_;
}

void ElasticPortAdapter::Adjustment::incStat(double& throughput)
{
    curThroughput_ = throughput;
    currAdjustment_->throughput = throughput;
}

void ElasticPortAdapter::Adjustment::searchMax(ElasticPortAdapter::AdjustDecision& decision)
{
    for (std::map<int, MetaAdjustment*>::iterator it = adjustInfo_.begin(); it != adjustInfo_.end();
         ++it) {
        MetaAdjustment* tmp = it->second;
        if (tmp->throughput > maxThroughput_ &&
            (tmp->throughput - maxThroughput_) > 0.05 * maxThroughput_) {
            maxThroughput_ = tmp->throughput;
            lastPositivePos_ = it->first;
        }
    }
    if (lastPositivePos_ != -1) {
        decision = RESET;
    } else {
        decision = NEG_STOP;
    }
}
/*End Adjustment entry related functions */

/*ElasticPortAdapter related functions */
ElasticPortAdapter::ElasticPortAdapter(ScheduledQueue& sq)
  : _sq(sq)
{
    initialize(UP);
}

void ElasticPortAdapter::initialize(AdjustDirection dir)
{
    _trend = dir;
    _curAdjustment = NULL;
    _recordHist = false;
    _affectedPorts.clear();
}

void ElasticPortAdapter::clear()
{
    _sq.turnOffScheduledPort(_tmpExcludedSet);
    _tmpExcludedSet.clear();
    _affectedPorts.clear();
    initialize(UP);
}

void ElasticPortAdapter::adjust(double& t)
{
    if (_curAdjustment == NULL) {
        addNewAdjustment(t);
    } else {
        refine(t);
    }
}

/*
 *Evaluate whether to continue with QueueElasticity
 */
void ElasticPortAdapter::refine(double& t)
{
    _curAdjustment->incStat(t);
    bool furtherRefine =
      _curAdjustment->getSize() <= GRANULARITY * (int)_sq.getScheduledPortCount() ? false : true;
    AdjustDecision ad = _curAdjustment->nextStep(furtherRefine);
    switch (ad) {
        case CONTINUE:
            break;
        case RESET: {
            resetSchedulerQueuePlacement();
            confirmToRemove(_curAdjustment->getOperators(), _curAdjustment->getRange());

            double maxThroughput = _curAdjustment->getMaxThroughput();
            _affectedPorts.insert(_affectedPorts.end(), _curAdjustment->getOperators().begin(),
                                  _curAdjustment->getOperators().begin() +
                                    _curAdjustment->getEndPos());
            _recordHist = true;

            delete _curAdjustment;
            _curAdjustment = NULL;
            addNewAdjustment(maxThroughput);
        } break;

        case NEG_STOP: {
            bool toExtend =
              (_curAdjustment->getSize() > GRANULARITY * (int)_sq.getScheduledPortCount()) ? false
                                                                                           : true;

            if (toExtend && extendAdjustment()) {
                if (_trend == UP) {
                    _sq.turnOnScheduledPort(_curAdjustment->getOperators(),
                                            _curAdjustment->getAdjustRange());
                } else {
                    _sq.turnOffScheduledPort(_curAdjustment->getOperators(),
                                             _curAdjustment->getAdjustRange());
                }
            } else {
                if (_trend == UP) {
                    _sq.turnOffScheduledPort(_curAdjustment->getOperators(),
                                             _curAdjustment->getRange());
                } else {
                    _sq.turnOnScheduledPort(_curAdjustment->getOperators(),
                                            _curAdjustment->getRange());
                }
                double maxThroughput = _curAdjustment->getMaxThroughput();
                delete _curAdjustment;
                _curAdjustment = NULL;
                _sq.adjustingDone(_recordHist, _trend, maxThroughput, _affectedPorts);
                _affectedPorts.clear();
            }
        } break;

        case CONTINUE_DOWN: {
            if (_trend == UP) {
                _sq.turnOffScheduledPort(_curAdjustment->getOperators(),
                                         _curAdjustment->getAdjustRange());
            } else {
                _sq.turnOnScheduledPort(_curAdjustment->getOperators(),
                                        _curAdjustment->getAdjustRange());
            }
        } break;

        case CONTINUE_UP: {
            if (_trend == UP) {
                _sq.turnOnScheduledPort(_curAdjustment->getOperators(),
                                        _curAdjustment->getAdjustRange());
            } else {
                _sq.turnOffScheduledPort(_curAdjustment->getOperators(),
                                         _curAdjustment->getAdjustRange());
            }
        } break;
    }
}

// reset the placement to the best performance configuration
void ElasticPortAdapter::resetSchedulerQueuePlacement()
{
    int end = _curAdjustment->getEndPos();
    int lastPositiveEnd = _curAdjustment->getLastPositivePos();
    if (_trend == UP) {
        if (lastPositiveEnd > end) {
            std::pair<int, int> ret = std::make_pair(end, lastPositiveEnd);
            _sq.turnOnScheduledPort(_curAdjustment->getOperators(), ret);
        } else if (lastPositiveEnd < end) {
            std::pair<int, int> ret = std::make_pair(lastPositiveEnd, end);
            _sq.turnOffScheduledPort(_curAdjustment->getOperators(), ret);
        }
    } else {
        if (lastPositiveEnd > end) {
            std::pair<int, int> ret = std::make_pair(end, lastPositiveEnd);
            _sq.turnOffScheduledPort(_curAdjustment->getOperators(), ret);
        } else if (lastPositiveEnd < end) {
            std::pair<int, int> ret = std::make_pair(lastPositiveEnd, end);
            _sq.turnOnScheduledPort(_curAdjustment->getOperators(), ret);
        }
    }
    _curAdjustment->reset();
}

bool ElasticPortAdapter::extendAdjustment()
{
    int i;
    if (_trend == UP) {
        i = getPrevGroupNum(_curAdjustment->getCurGroupNum());
    } else {
        i = getNextGroupNum(_curAdjustment->getCurGroupNum());
    }
    if (i != -1) {
        std::vector<uint32_t>& operators = _perfGroups[i];
        _curAdjustment->extend(operators, i);
        return true;
    } else {
        return false;
    }
}

void ElasticPortAdapter::addNewAdjustment(double& t)
{
    int curGroup;
    int i;

    groupOperators();

    if (_trend == UP) {
        i = getHeaviestGroupNum();
    } else {
        i = getLightestGroupNum();
    }

    if (i != -1) {
        curGroup = i;
        std::vector<uint32_t>& operators = _perfGroups[i];
        _curAdjustment = new Adjustment(t, operators, curGroup);
        if (_trend == UP) {
            _sq.turnOnScheduledPort(operators, _curAdjustment->getRange());
        } else {
            _sq.turnOffScheduledPort(operators, _curAdjustment->getRange());
        }
    } else {
        // no available queues for adjustment
        _sq.adjustingDone(false, _trend, 0.0, _affectedPorts);
    }
}

int ElasticPortAdapter::getNextGroupNum(int& i)
{
    if (i == PERF_GROUP_NUM - 1) {
        return -1;
    }
    assert(i + 1 >= 0);
    for (unsigned int j = i + 1; j < PERF_GROUP_NUM; j++) {
        assert(j <= PERF_GROUP_NUM - 1);
        if (_perfGroups[j].size() != 0) {
            return j;
        }
    }
    return -1;
}

int ElasticPortAdapter::getPrevGroupNum(int& i)
{
    if (i == 0) {
        return -1;
    }
    assert(i - 1 >= 0);
    for (unsigned int j = i - 1;; j--) {
        assert(j <= PERF_GROUP_NUM - 1);
        if (_perfGroups[j].size() != 0) {
            return j;
        }
        if (j == 0) {
            break;
        }
    }
    return -1;
}

int ElasticPortAdapter::getHeaviestGroupNum()
{
    int heaviestGroup = -1;
    for (unsigned int i = PERF_GROUP_NUM - 1;; i--) {
        if (_perfGroups[i].size() != 0) {
            heaviestGroup = i;
            break;
        }
        if (i == 0) {
            break;
        }
    }
    return heaviestGroup;
}

int ElasticPortAdapter::getLightestGroupNum()
{
    int cheapestGroup = -1;
    for (unsigned int i = 0; i < PERF_GROUP_NUM; i++) {
        if (_perfGroups[i].size() != 0) {
            cheapestGroup = i;
            break;
        }
    }
    return cheapestGroup;
}

void ElasticPortAdapter::groupOperators()
{
    _perfGroups.clear();
    _perfGroups.resize(PERF_GROUP_NUM);

    ThreadProfiler& tp = PEImpl::instance().getThreadProfiler();
    std::vector<double> relativeCost;
    relativeCost.resize(PEImpl::instance().getOperators().size(), 0);
    tp.getOperatorRelativeCost(relativeCost);

    if (_trend == DOWN) {
        //_trend DOWN fill the min bin first
        std::vector<uint32_t> relatedOperators;
        double min = 1;
        for (unsigned int i = 0; i < relativeCost.size(); i++) {
            if (_sq.hasScheduledPort(i)) {
                std::set<uint32_t>::iterator it = _tmpExcludedSet.find(i);
                if (_trend == DOWN && it != _tmpExcludedSet.end()) {
                    relatedOperators.push_back(i);
                    double counter = relativeCost[i];
                    if (counter > 0.0 && min > counter) {
                        min = counter;
                    }
                }
            }
        }

        for (unsigned int i = 0; i < relatedOperators.size(); i++) {
            unsigned int j = relatedOperators[i];
            int base = int(floor((double)relativeCost[j] / (double)min));
            if (base == 0) {
                _perfGroups[0].push_back(j);
            } else {
                base = floor(log2(base) / log2(2)) + 1;
                if (base >= PERF_GROUP_NUM) {
                    base = PERF_GROUP_NUM - 1;
                }
                _perfGroups[base].push_back(j);
            }
        }
    } else {
        //_trend UP fill the max bin first
        std::vector<uint32_t> relatedOperators;
        double max = 0;
        for (unsigned int i = 0; i < relativeCost.size(); i++) {
            if (_sq.hasScheduledPort(i)) {
                std::set<uint32_t>::iterator it = _tmpExcludedSet.find(i);
                if (_trend == UP && it == _tmpExcludedSet.end()) {
                    relatedOperators.push_back(i);
                    double counter = relativeCost[i];
                    if (counter > 0.0 && counter > max) {
                        max = counter;
                    }
                }
            }
        }

        for (unsigned int i = 0; i < relatedOperators.size(); i++) {
            unsigned int j = relatedOperators[i];
            if (relativeCost[j] <= 0.0) {
                _perfGroups[0].push_back(j);
            } else {
                int base = int(floor((double)max / (double)relativeCost[j]));
                base = PERF_GROUP_NUM - base;
                if (base < 0) {
                    base = 0;
                }
                _perfGroups[base].push_back(j);
            }
        }
    }
}

void ElasticPortAdapter::confirmToRemove(vector<uint32_t>& operators, pair<int, int>& range)
{
    if (_trend == UP) {
        _tmpExcludedSet.insert(operators.begin() + range.first, operators.begin() + range.second);
    } else {
        for (int i = range.first; i < range.second; i++) {
            _tmpExcludedSet.erase(operators[i]);
        }
    }
}

void ElasticPortAdapter::confirmToAdd(vector<uint32_t>& operators, pair<int, int>& range)
{
    if (_trend == DOWN) {
        _tmpExcludedSet.insert(operators.begin() + range.first, operators.begin() + range.second);
    } else {
        for (int i = range.first; i < range.second; i++) {
            _tmpExcludedSet.erase(operators[i]);
        }
    }
}
