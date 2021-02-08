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

/*
 * Implementation of SPL::IncrementalCkptIntervalSetter class
 */

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Operator/State/CheckpointLogTrace.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/Operator/State/IncrementalCkptIntervalSetter.h>
#include <assert.h>

using namespace SPL;
using namespace std;

IncrementalCkptIntervalSetter::IncrementalCkptIntervalSetter()
  : interval_(INITIAL_INTERVAL)
  , numCkpts_(0)
  , probeLimit_(PROBE_LIMIT)
  , probeCount_(0)
  , sizeRatio_(0.0)
  , baseCkptSize_(0)
  , sumDeltaCkptSize_(0)
  , weightedSumDeltaCkptSize_(0)
{}

IncrementalCkptIntervalSetter::~IncrementalCkptIntervalSetter() {}

void IncrementalCkptIntervalSetter::initialize(void* param)
{
    if (param == NULL) {
        sizeRatio_ = DEFAULT_SIZE_RATIO;
    } else {
        double sizeRatio = *((double*)param);
        if (sizeRatio < 0) {
            THROW_CHAR(DataStore, "size ratio cannot be a negative value");
        }
        sizeRatio_ = sizeRatio;
    }
}

void IncrementalCkptIntervalSetter::addCheckpointInfo(const IncrementalCkptInfo& ckptInfo)
{
    if (ckptInfo.isBase() == false) {
        onAddingDeltaCheckpointInfo(ckptInfo);
    } else {
        if (interval_ == 1) {
            probeCount_++;
        }
        onAddingBaseCheckpointInfo(ckptInfo);
    }
    numCkpts_++;
}

void IncrementalCkptIntervalSetter::onAddingBaseCheckpointInfo(const IncrementalCkptInfo& ckptInfo)
{
    baseCkptSize_ = ckptInfo.getDeltaSize() + ckptInfo.getIndexSize();
}

void IncrementalCkptIntervalSetter::onAddingDeltaCheckpointInfo(const IncrementalCkptInfo& ckptInfo)
{
    // running sum of delta ckpt size, new value gets more weight
    double size = ckptInfo.getDeltaSize() + ckptInfo.getIndexSize();
    sumDeltaCkptSize_ += size;
    weightedSumDeltaCkptSize_ += numCkpts_ * size;
}

uint32_t IncrementalCkptIntervalSetter::determineInterval()
{
    if (interval_ == 1) {                 // incremental ckpt is disabled for now
        if (probeCount_ == probeLimit_) { // re-enable incremental ckpt and see if it's beneficial
            interval_ = 2;
            probeCount_ = 0;
        }
        numCkpts_ = 1;
        return interval_;
    }
    if (numCkpts_ <= 1) { // the first base checkpoint ever made or during operator initialization
        return interval_;
    }
    interval_ = onDetermineInterval();
    if (interval_ == 1) {
        probeCount_ = 0;
    }
    numCkpts_ = 1;
    return interval_;
}

uint32_t IncrementalCkptIntervalSetter::onDetermineInterval()
{
    // calculate a new interval
    uint32_t newInterval;
    if (baseCkptSize_ * sizeRatio_ <=
        sumDeltaCkptSize_) {          // incremental ckpt is not beneficial, decrease interval
        newInterval = interval_ >> 1; // halve it
        if (newInterval == 0) {
            newInterval = 1;
        }
    } else {                          // incremental ckpt is beneficial, incrase interval
        newInterval = interval_ << 1; // double it
        double weightSum = double(interval_ - 1) * interval_ / 2; // sum of 1, 2, ..., interval_-1
        double avgDeltaCkptSize_ =
          weightedSumDeltaCkptSize_ / weightSum; // weighted average of delta checkpoint size
        avgDeltaCkptSize_ *= LOGGING_PENALTY;    // account for logging overhead
        double maxInterval = baseCkptSize_ * sizeRatio_ / avgDeltaCkptSize_ + 1;
        if (maxInterval < 2) {
            newInterval = 1;
        } else if (newInterval > maxInterval) {
            newInterval = uint32_t(maxInterval);
        }
    }
    sumDeltaCkptSize_ = 0;
    weightedSumDeltaCkptSize_ = 0;
    return newInterval;
}

uint32_t IncrementalCkptIntervalSetter::reset(const uint32_t interval)
{
    assert(interval >= 1);

    interval_ = interval;
    numCkpts_ = 0;
    return interval_;
}

bool IncrementalCkptIntervalSetter::shouldEnableLogging() const
{
    // whether to enable logging depends on whether the next checkpoint wil be base or delta
    if (interval_ == 1) {
        return (probeCount_ + 1 == probeLimit_);
    } else {
        if (numCkpts_ == interval_) { // this ckpt is a new base
            // if interval_ == 2 || interval_ == 3
            //   after this base ckpt is done, interval may change to 1 (the next ckpt will be
            //   base), or 4 or 6 (the next ckpt will be delta); so it's safe to keep logging. If
            //   the next ckpt is base, it will disable logging after the subsequent ckpt
            // if interval_ > 3
            //   the interval will still be larger than 2, so the next ckpt must be delta
            return true;
        } else if (numCkpts_ < interval_ - 1) {
            // two possible cases:
            // 1) this ckpt is delta and the next will also be delta
            // 2) this ckpt is base and the next will be delta
            return true;
        } else { // this ckpt is delta and the next will be a new base
            return false;
        }
    }
}

bool IncrementalCkptIntervalSetter::isBaseCheckpoint() const
{
    return (numCkpts_ == interval_ || numCkpts_ == 0);
}
