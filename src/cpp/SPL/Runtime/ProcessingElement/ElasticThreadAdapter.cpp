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
#include <SPL/Runtime/ProcessingElement/ElasticThreadAdapter.h>
#include <TRC/DistilleryDebug.h>

#include <boost/thread/thread.hpp>

#include <cmath>
#include <fstream>

using namespace SPL;

const int32_t NOISE_LIMIT = 5;
const double THROUGHPUT_TOLERANCE = 0.05;
const double MAX_USAGE = 0.8;
const double LOAD_CHANGE_SENSITIVITY = 0.2;

ElasticThreadAdapter::ElasticThreadAdapter()
  : _belowLevel(0)
  , _currLevel(0)
  , _aboveLevel(0)
  , _minLevel(0)
  , _maxLevel(0)
  , _time(0)
  , _noiseDetector(NOISE_LIMIT)
  , _changeSensitivity(THROUGHPUT_TOLERANCE)
{
    APPTRC(L_INFO, "ElasticThreadAdapter: throughput change tolerance: " << _changeSensitivity,
           SPL_PE_DBG);
}

void ElasticThreadAdapter::initialize(uint32_t minThreads,
                                      uint32_t maxThreads,
                                      uint32_t activeThreads)
{
    _threadInfos.clear();

    // we're assuming that the PE did sanity checks on these values already
    _minLevel = minThreads;
    _currLevel = activeThreads;
    _maxLevel = maxThreads;
    _belowLevel = activeThreads / 2;
    _aboveLevel = activeThreads * 2;

    // 0 counts as a level, starts at 1
    for (uint32_t i = 0; i < _aboveLevel + 1; ++i) {
        _threadInfos.push_back(ThreadInfo());
    }

    // for the "out of bounds" levels, say they are trusted, but have zero throughput
    for (uint32_t i = 0; i < _minLevel; ++i) {
        _threadInfos.at(i).trusted = true;
        _threadInfos.at(i).firstThroughput = 0;
        _threadInfos.at(i).lastThroughput = 0;
    }

    for (uint32_t i = _maxLevel; i < _aboveLevel + 1; ++i) {
        _threadInfos.at(i).trusted = true;
        _threadInfos.at(i).firstThroughput = 0;
        _threadInfos.at(i).lastThroughput = 0;
    }
}

ElasticThreadAdapter::~ElasticThreadAdapter() {}

ElasticThreadAdapter::LoadChange ElasticThreadAdapter::pingNoiseDetector(
  ElasticThreadAdapter::LoadChange direction)
{
    if (_noiseDetector > 0) {
        _noiseDetector--;
        APPTRC(L_DEBUG, "set off noise detector, " << _noiseDetector << " remaining.", SPL_PE_DBG);
        return PotentialNoise;
    } else {
        return direction;
    }
}

ElasticThreadAdapter::LoadChange ElasticThreadAdapter::checkForChangeInLoadBasedOnThroughput(
  double throughput)
{
    // Obviously, if t=0, we can't have a load change. But why t=1? The reason is that the
    // first throughput sample tends to be too high. It counts all of the queues filling up
    // as real throughput, but that's much higher than our steady-state throughput.
    if (_time == 0 || _time == 1) {
        return UnknownLoadChange;
    }

    if (!_threadInfos.at(_currLevel).trusted) {
        return UnknownLoadChange;
    }

    ThreadInfo* ti = &_threadInfos.at(_currLevel);

    // we were were here the last time, but throughput changed
    if (ti->lastTime == _time - 1) {
        double oldThroughput = ti->firstThroughput;
        double throughputDiff = throughput - oldThroughput;
        if (throughputDiff < 0.0) {
            if (-throughputDiff > LOAD_CHANGE_SENSITIVITY * throughput) {
                APPTRC(L_DEBUG, "less load", SPL_PE_DBG);
                return pingNoiseDetector(LessLoad);
            }
        } else {
            if (throughputDiff > LOAD_CHANGE_SENSITIVITY * oldThroughput) {
                APPTRC(L_DEBUG, "more load", SPL_PE_DBG);
                return pingNoiseDetector(MoreLoad);
            }
        }
    }

    _noiseDetector = NOISE_LIMIT;
    return UnknownLoadChange;
}

void ElasticThreadAdapter::untrustOtherData(double throughput)
{
    APPTRC(L_DEBUG, "change in throughput, untrusting all the data", SPL_PE_DBG);
    for (size_t i = _minLevel; i < _threadInfos.size(); ++i) {
        _threadInfos.at(i).trusted = false;
    }
}

void ElasticThreadAdapter::untrustAbove()
{
    for (size_t i = _currLevel + 1; i < _threadInfos.size(); ++i) {
        _threadInfos.at(i).trusted = false;
    }
}

void ElasticThreadAdapter::untrustLevel(uint32_t level, double t)
{
    _threadInfos.at(level).firstThroughput = t;
    _threadInfos.at(level).lastTime = _time;
}

bool ElasticThreadAdapter::trustBelow()
{
    if (_currLevel == _minLevel) {
        return false;
    }
    if (_threadInfos.at(_belowLevel).trusted) {
        APPTRC(L_DEBUG, "trust below", SPL_PE_DBG);
    }
    return _threadInfos.at(_belowLevel).trusted;
}

bool ElasticThreadAdapter::trustAbove()
{
    if (_currLevel == _threadInfos.size() - 1) {
        return false;
    }
    if (_threadInfos.at(_aboveLevel).trusted) {
        APPTRC(L_DEBUG, "trust above; " << _aboveLevel, SPL_PE_DBG);
    }
    return _threadInfos.at(_aboveLevel).trusted;
}

bool ElasticThreadAdapter::improvementTrendBelow(double throughput)
{
    if (_currLevel == 1) {
        return false;
    }

    ThreadInfo& pci = _threadInfos.at(_belowLevel);
    if (!pci.trusted) {
        return false;
    }

    if ((throughput > pci.lastThroughput) &&
        (throughput - pci.lastThroughput) > _changeSensitivity * pci.lastThroughput) {
        APPTRC(L_DEBUG, "improvement trend below: " << pci.lastThroughput << " -> " << throughput,
               SPL_PE_DBG);
        return true;
    }

    return false;
}

bool ElasticThreadAdapter::improvementTrendAbove(double throughput)
{
    ThreadInfo& nci = _threadInfos.at(_aboveLevel);
    if (!nci.trusted) {
        return false;
    }

    if ((nci.lastThroughput > throughput) &&
        (nci.lastThroughput - throughput) > _changeSensitivity * throughput) {
        APPTRC(L_DEBUG, "improvement trend above: " << throughput << " -> " << nci.lastThroughput,
               SPL_PE_DBG);
        return true;
    }
    return false;
}

void ElasticThreadAdapter::decreaseLevel()
{
    if (_currLevel > _minLevel) {
        APPTRC(L_DEBUG, "decreasing level", SPL_PE_DBG);
        if (_currLevel > 3) {
            _aboveLevel = _currLevel;
            uint32_t belowPow2 =
              static_cast<uint32_t>(::exp2(std::ceil(::log2(static_cast<double>(_currLevel)) - 1)));
            uint32_t abovePow2 = belowPow2 * 2;
            uint32_t pow2Counter = belowPow2 / 2;
            uint32_t oldLevel = _currLevel;
            for (uint32_t i = abovePow2 - (belowPow2 / 2); i > belowPow2; i -= pow2Counter) {
                if (i <= _minLevel) {
                    _currLevel = _minLevel;
                    break;
                }
                _currLevel = i;
                pow2Counter = std::max(1u, pow2Counter / 2);
                if (i < oldLevel && !_threadInfos.at(i).trusted) {
                    _belowLevel = belowPow2;
                    break;
                }
            }

            if (_currLevel == belowPow2 + 1) {
                if (!_threadInfos.at(belowPow2 + 1).trusted) {
                    if (belowPow2 >= _minLevel) {
                        _belowLevel = belowPow2;
                        _currLevel = belowPow2 + 1;
                    } else {
                        _belowLevel = _minLevel - 1;
                        _currLevel = _minLevel;
                    }
                } else {
                    if (belowPow2 >= _minLevel) {
                        _belowLevel = belowPow2 / 2;
                        _currLevel = belowPow2;
                    } else {
                        _belowLevel = _minLevel - 1;
                        _currLevel = _minLevel;
                    }
                }
            }
        } else if (_currLevel == 3) {
            if (_minLevel <= 2) {
                _belowLevel = 1;
                _currLevel = 2;
                _aboveLevel = 3;
            }
        } else { // _currLevel must be 2
            if (_minLevel == 1) {
                _belowLevel = 0;
                _currLevel = 1;
                _aboveLevel = 2;
            }
        }
    }
}

void ElasticThreadAdapter::increaseLevel()
{
    if (_currLevel < _maxLevel) {
        APPTRC(L_DEBUG, "increasing level", SPL_PE_DBG);
        _belowLevel = _currLevel;
        uint32_t abovePow2 =
          2 * static_cast<uint32_t>(::exp2(std::floor(::log2(static_cast<double>(_currLevel)))));

        if (_threadInfos.at(_aboveLevel).trusted) {
            // if we trust the level above, then we must have been allowed to visit it before, which
            // means it must be under our maxLevel, so we don't need to check maxLevel
            _currLevel = _aboveLevel;
            if (abovePow2 > _threadInfos.size()) {
                _aboveLevel = abovePow2;
            } else if (abovePow2 == _aboveLevel) {
                _aboveLevel = abovePow2 * 2;
            } else {
                for (uint32_t i = _currLevel + 1; i < abovePow2 + 1; ++i) {
                    _aboveLevel = i;
                    if (_threadInfos.at(i).trusted) {
                        break;
                    }
                }
            }
        } else if (_currLevel > 2) {
            if (abovePow2 <= _maxLevel) {
                _currLevel = abovePow2;
                _aboveLevel = abovePow2 * 2;
            } else {
                _currLevel = _maxLevel;
                _aboveLevel = _maxLevel + 1;
            }
        } else {
            if (_currLevel * 2 <= _maxLevel) {
                _currLevel = _currLevel * 2;
                _aboveLevel = _aboveLevel * 2;
            } else {
                _currLevel = _maxLevel;
                _aboveLevel = _maxLevel + 1;
            }
        }

        if (_aboveLevel >= _threadInfos.size()) {
            while (_threadInfos.size() < _aboveLevel + 1) {
                _threadInfos.push_back(ThreadInfo());
            }
        }
    }
}

ElasticThreadAdapter::CPUStat ElasticThreadAdapter::getCPUStat()
{
    std::ifstream stat("/proc/stat");
    if (!stat) {
        APPTRC(L_ERROR, "error opening /proc/stat", SPL_PE_DBG);
        return CPUStat();
    }

    std::string cpu;
    CPUStat usage;
    stat >> cpu >> usage;
    if (!stat) {
        APPTRC(L_ERROR, "error reading /proc/stat", SPL_PE_DBG);
        return CPUStat();
    }
    return usage;
}

bool ElasticThreadAdapter::isCPUUsageAcceptable()
{
    CPUStat currStat = getCPUStat();
    if (currStat.isDefault()) {
        // error reading /proc/stat, assume the worst
        return false;
    }

    if (_lastStat.isDefault()) {
        _lastStat = currStat;
        // say no until we get some data
        return false;
    }

    CPUStat diff = currStat - _lastStat;
    _lastStat = currStat;
    APPTRC(L_DEBUG, "cpu usage: " << diff.usage(), SPL_PE_DBG);
    return diff.usage() <= MAX_USAGE;
}

uint32_t ElasticThreadAdapter::calculateThreads(double throughput, bool& loadChanged)
{
    switch (checkForChangeInLoadBasedOnThroughput(throughput)) {
        case PotentialNoise:
            return _currLevel;
        case LessLoad:
        case MoreLoad:
            loadChanged = true;
            _noiseDetector = NOISE_LIMIT;
            untrustOtherData(throughput);
            _time = 0;
            return _currLevel;
            break;
        default:;
    }

    // update info for the current channel
    ThreadInfo& ti = _threadInfos.at(_currLevel);
    ti.lastTime = _time++;
    ti.lastThroughput = throughput;
    if (!ti.trusted) {
        ti.firstThroughput = throughput;
    }
    ti.trusted = true;

    if (((improvementTrendBelow(throughput) && !trustAbove()) ||
         improvementTrendAbove(throughput) || (_currLevel == _minLevel && !trustAbove()))) {
        if (isCPUUsageAcceptable()) {
            increaseLevel();
        }
    } else if (!trustBelow() || !improvementTrendBelow(throughput)) {
        decreaseLevel();
    }

    return _currLevel;
}
