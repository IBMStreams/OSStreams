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

#ifndef TCPUTILS_H_
#define TCPUTILS_H_

#include <UTILS/UTILSTypes.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/DistilleryApplication.h>

#include <string>
#include <sstream>
#include <unistd.h>
#include <time.h>


UTILS_NAMESPACE_BEGIN

class TimeUtils {
public:
    static double systime()
    {
        struct timespec ts ;
        clock_gettime(CLOCK_REALTIME, &ts) ;
        return ts.tv_sec + (ts.tv_nsec * 1e-9) ;
    }
};

class BlockHandler {
public:
    BlockHandler() : block_(0), awake_(0), lastWakeup_(0), shutdown_(false) {}

    void setBlock(const std::string& optionName, const char* value);

    bool isActive() const
    {   return (block_ != 0); }

    bool isReadyToBlock() const
    { return ((TimeUtils::systime() * 1e3) - lastWakeup_ > ((double)awake_));}

    uint32_t blockTimeMillis() const
    {   return block_; }

    uint32_t awakeTimeMillis() const
    {   return awake_; }

    void block()
    {
        uint32_t remain = block_;
        for (; !shutdown_ && remain >= BLOCK_RESOLUTION;
                remain -= BLOCK_RESOLUTION) {
            usleep(BLOCK_RESOLUTION * 1000);
        }
        if (!shutdown_) usleep(remain * 1000);
        lastWakeup_ = TimeUtils::systime() * 1e3;
    }

    /// If called from another thread, BlockHandler returns from block()
    /// after maximum one BLOCK_RESOLUTION interval.
    void shutdown()
    {   shutdown_ = true; }

private:
    static const unsigned int BLOCK_RESOLUTION=100; ///< block time slice (millis)
    /// Handler will block 'block_' millisecs then stay awake 'awake_' millisecs.
    static const unsigned int BLOCK_MAX=1000000; ///< maximum block and awake values (millis)
    uint32_t block_;          ///< duration to block (millisecs)
    uint32_t awake_;          ///< duration to stay awake (millisecs)
    double lastWakeup_;       ///< last time when receiver woke up from sleep (nanosecs)
    volatile bool shutdown_;
};


///////////////////////////////////////////////////////////////////////////
inline
void BlockHandler::setBlock(const std::string& optionName, const char* value)
{
    std::string s(value);
    std::string optName("' for option ");
    optName.append(optionName);

    if (s.size() == 0 || s.size() > 20) {
        THROW(InvalidOption, "Invalid value '" << s << optName);
    }
    uint32_t blockPeriod = 0;
    size_t delimPos = s.find_first_of(':', 0);
    if (delimPos != std::string::npos) {
        s[delimPos] = ' ';
        std::istringstream is(s);
        is >> block_ >> blockPeriod;
        if (block_ == 0 || blockPeriod == 0) {
            THROW(InvalidOption, "Invalid value '0" << optName);
        }
        if (block_ > blockPeriod) {
            THROW(InvalidOption, "Invalid value '" << block_ << optName <<
                    ".  Value should be smaller than " << blockPeriod);
        }
        if (block_ > BLOCK_MAX) {
            THROW(InvalidOption, "Invalid value '" << block_ << optName);
        }
        if (blockPeriod > BLOCK_MAX) {
            THROW(InvalidOption, "Invalid value '" << blockPeriod << optName);
        }
    }
    else {
        THROW(InvalidOption, "Invalid value '" << s << optName);
    }
    // success
    awake_ = blockPeriod - block_;
    lastWakeup_ = TimeUtils::systime() * 1e3;
}

UTILS_NAMESPACE_END

UTILS_NAMESPACE_USE;
const unsigned int BlockHandler::BLOCK_RESOLUTION;
const unsigned int BlockHandler::BLOCK_MAX;

#endif /* TCPUTILS_H_ */
