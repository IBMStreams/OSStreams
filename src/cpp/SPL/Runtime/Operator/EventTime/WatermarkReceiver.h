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

#ifndef SRC_SPL_RUNTIME_OPERATOR_WATERMARKRECEIVER_H
#define SRC_SPL_RUNTIME_OPERATOR_WATERMARKRECEIVER_H

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Operator/EventTime/EventTimeContext.h>
#include <SPL/Runtime/Operator/OperatorContextImpl.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <SPL/Runtime/Utility/WatermarkPunctPayload.h>

#include <SAM/applicationModel.h>

#include <limits>

namespace SPL {

// The following classes are DLL_PUBLIC in order to be accessible for unit
// testing: WatermarkReceiver, OneStreamWatermarkReceiver,
// TwoStreamWatermarkReceiver, MultiStreamWatermarkReceiver

/// @brief Receives watermarks from an operator input port.
class DLL_PUBLIC WatermarkReceiver
{
  public:
    /* ConnectionId is composed of {source Job Id, source operator global Index,
     * source port Index, destination port Index}.
     * The assumption is that for each inbound stream there is only one
     * connection element (whether inter PE or intra PE) defined in the
     * operator instance model which carries that stream.
     */
    class ConnectionId
    {
      public:
        uint64_t jobId;
        uint32_t operatorIndex;
        uint32_t oportIndex;
        uint32_t iportIndex;

        ConnectionId(uint64_t jId, uint32_t opIdx, uint32_t oportIdx, uint32_t iportIdx)
          : jobId(jId)
          , operatorIndex(opIdx)
          , oportIndex(oportIdx)
          , iportIndex(iportIdx)
        {}
        ConnectionId()
          : jobId(std::numeric_limits<uint64_t>::max())
          , operatorIndex(std::numeric_limits<uint32_t>::max())
          , oportIndex(std::numeric_limits<uint32_t>::max())
          , iportIndex(std::numeric_limits<uint32_t>::max())
        {}

        bool operator==(ConnectionId const& other) const
        {
            return jobId == other.jobId && operatorIndex == other.operatorIndex &&
                   oportIndex == other.oportIndex && iportIndex == other.iportIndex;
        }

        size_t hashCode() const
        {
            size_t r = 17;
            std::tr1::hash<uint32_t> hs;
            r = 37 * r + std::tr1::hash<uint64_t>()(jobId);
            r = 37 * r + hs(operatorIndex);
            r = 37 * r + hs(oportIndex);
            r = 37 * r + hs(iportIndex);
            return r;
        }
    };

    typedef std::pair<ConnectionId, Watermark> WmEntryType;

    virtual ~WatermarkReceiver();

    /**
     * @brief Receives a watermark wrapped by the specified @c %Punctuation.
     *
     * @param punct the %Punctuation
     * @return the received @c Watermark if the input watermark was advanced,
     *   otherwise a zero @c Watermark.
     */
    virtual Watermark receive(Punctuation const& punct, uint32_t iportIndex) = 0;

    /**
     * @brief Retrieves the current input @c %Watermark.
     *
     * @return the current input watermark.
     */
    virtual Watermark getWatermark() = 0;

    /** @brief Creates new %WatermarkReceiver instance.
     */
    static WatermarkReceiver* newInstance(OperatorContextImpl const& ctx,
                                          EventTimeContextSetup const& setup);

  protected:
    WatermarkReceiver() {}
};

PUSH_DLL_PUBLIC

/// @brief Formatted output function for @c WatermarkReceiver::ConnectionId.
std::ostream& operator<<(std::ostream&, const WatermarkReceiver::ConnectionId&);

POP_DLL_PUBLIC

} // end namespace SPL

namespace std {
namespace tr1 {
PUSH_DLL_PUBLIC
template<>
struct hash<SPL::WatermarkReceiver::ConnectionId>
{
    inline size_t operator()(const SPL::WatermarkReceiver::ConnectionId& t) const
    {
        return t.hashCode();
    }
};
POP_DLL_PUBLIC
}
} // end std::tr1

#endif /* SRC_SPL_RUNTIME_OPERATOR_WATERMARKRECEIVER_H */
