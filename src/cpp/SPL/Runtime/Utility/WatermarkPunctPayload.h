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

#ifndef SPL_RUNTIME_UTILITY_WATERMARKPUNCTPAYLOAD_H
#define SPL_RUNTIME_UTILITY_WATERMARKPUNCTPAYLOAD_H

#include <SPL/Runtime/Utility/Payload.h>
#include <SPL/Runtime/Utility/TimeUtils.h>
#include <SPL/Runtime/Utility/Visibility.h>

#include <iostream>

namespace SPL {
class Punctuation;

PUSH_DLL_PUBLIC

/* StreamIdType is composed of {job Id, source operator global index,
 * source port index}. We assume that for each inbound stream, there is only
 * one connection element (whether inter PE or intra PE) defined in the
 * operator instance model which carries a given stream id.
 */
typedef struct stream_id_t
{
    uint64_t jobId;          // submitting job Id
    uint32_t operInstanceId; // submitting operator instance Id
    uint32_t oportIdx;       // submitting operator output port index

    stream_id_t()
      : jobId(0)
      , operInstanceId(0)
      , oportIdx(0)
    {}

    stream_id_t(uint64_t job, uint32_t op, uint32_t p)
    {
        jobId = job;
        operInstanceId = op;
        oportIdx = p;
    }

    stream_id_t(const stream_id_t& sid)
      : jobId(sid.jobId)
      , operInstanceId(sid.operInstanceId)
      , oportIdx(sid.oportIdx)
    {}

    stream_id_t& operator=(const stream_id_t& sid)
    {
        jobId = sid.jobId;
        operInstanceId = sid.operInstanceId;
        oportIdx = sid.oportIdx;
        return *this;
    }

    void serialize(NativeByteBuffer& buf) const { buf << jobId << operInstanceId << oportIdx; }

    void deserialize(NativeByteBuffer& buf) { buf >> jobId >> operInstanceId >> oportIdx; }

    void serialize(NetworkByteBuffer& buf) const { buf << jobId << operInstanceId << oportIdx; }

    void deserialize(NetworkByteBuffer& buf) { buf >> jobId >> operInstanceId >> oportIdx; }

    void serialize(std::ostream& s) const
    {
        s << "jobId=" << jobId << ", operatorId=" << operInstanceId << ", oportIdx=" << oportIdx;
    }
} StreamIdType;

/// @brief Formatted output function for @c StreamIdType.
inline std::ostream& operator<<(std::ostream& os, const StreamIdType& sid)
{
    sid.serialize(os);
    return os;
}

/// @brief Two StreamIdType are equal iff their members are equal
inline bool operator==(const SPL::StreamIdType& x, const SPL::StreamIdType& y)
{
    return x.jobId == y.jobId && x.operInstanceId == y.operInstanceId && x.oportIdx == y.oportIdx;
}

POP_DLL_PUBLIC
}; // end namespace SPL

namespace std {
namespace tr1 {
PUSH_DLL_PUBLIC
/// Explicit hash specialization for StreamIdType.
template<>
struct hash<SPL::StreamIdType> : public std::unary_function<SPL::StreamIdType, std::size_t>
{
    std::size_t operator()(const SPL::StreamIdType& sid) const
    {
        return hash<uint64_t>()(sid.jobId) ^ hash<uint32_t>()(sid.operInstanceId) ^
               hash<uint32_t>()(sid.oportIdx);
    }
};
POP_DLL_PUBLIC
}
} // end std::tr1

namespace SPL {
/**
 * @brief Watermark punctuation payload implementation.
 */
class WatermarkPunctPayload : public Payload
{
  public:
    /// @brief Payload name
    static const std::string name;

    /// @brief Constructor for an empty watermark.
    WatermarkPunctPayload()
      : wm_(0)
      , sid_()
    {}

    /**
     * @brief Constructs a watermark with the given value.
     *
     * Operator and port identifiers are set to zero.
     * @param value watermark value
     */
    WatermarkPunctPayload(timestamp value)
      : wm_(value)
      , sid_()
    {}

    /**
     * @brief Constructs a watermark with the given values.
     *
     * @param value watermark value
     * @param jobId submitting job identifier
     * @param operatorId submitting operator instance identifier
     * @param oportIdx submitting operator output port index
     */
    WatermarkPunctPayload(timestamp value, uint64_t jobId, uint32_t operatorId, uint32_t oportIdx)
      : wm_(value)
      , sid_(jobId, operatorId, oportIdx)
    {}

    /**
     * @brief Clone this instance.
     * @return new copy
     */
    WatermarkPunctPayload* clone() const
    {
        return new WatermarkPunctPayload(wm_, sid_.jobId, sid_.operInstanceId, sid_.oportIdx);
    }

    /**
     * @brief Serialize this instance into a binary buffer.
     * @param buf serialization buffer
     */
    void serialize(NativeByteBuffer& buf) const
    {
        buf << wm_;
        sid_.serialize(buf);
    }

    /**
     * @brief Deserialize this instance from a binary buffer.
     * @param buf serialization buffer
     */
    void deserialize(NativeByteBuffer& buf)
    {
        buf >> wm_;
        sid_.deserialize(buf);
    }

    /**
     * @brief Serialize this instance into a network buffer.
     * @param buf serialization buffer
     */
    void serialize(NetworkByteBuffer& buf) const
    {
        buf << wm_;
        sid_.serialize(buf);
    }

    /**
     * @brief Deserialize this instance from a network buffer.
     * @param buf serialization buffer
     */
    void deserialize(NetworkByteBuffer& buf)
    {
        buf >> wm_;
        sid_.deserialize(buf);
    }

    /**
     * @brief Serialize this instance into the specified character stream.
     * @param s output stream
     */
    void serialize(std::ostream& s) const { s << "watermark=" << wm_ << ", " << sid_; }

    /**
     * @brief Returns the watermark value.
     * @return the watermark value
     */
    Watermark getValue() const { return wm_; }

    /**
     * @brief Sets the watermark value.
     * @param value the value
     */
    void setValue(Watermark value) { wm_ = value; }

    /**
     * @brief Returns the submitting Job identifier.
     * @return submitting job identifier
     */
    uint64_t getJobId() const { return sid_.jobId; }

    /**
     * @brief Sets the submitting job identifier.
     * @param id  the submitting job identifier
     */
    void setJobId(uint64_t id) { sid_.jobId = id; }

    /**
     * @brief Returns the submitting operator instance identifier.
     * @return submitting operator instance identifier
     */
    uint32_t getOperatorInstanceId() const { return sid_.operInstanceId; }

    /**
     * @brief Sets the submitting operator instance identifier.
     * @param id  the submitting operator instance identifier
     */
    void setOperatorInstanceId(uint32_t id) { sid_.operInstanceId = id; }

    /**
     * @brief Returns the submitting operator output port identifier.
     * @return submitting operator output port identifier
     */
    uint32_t getOperatorOPortIdx() const { return sid_.oportIdx; }

    /**
     * @brief Sets the submitting operator output port index.
     * @param idx the submitting operator output port index
     */
    void setOperatorOPortIdx(uint32_t idx) { sid_.oportIdx = idx; }

    /**
     * @brief Returns the size of the payload.
     * @return payload size
     */
    size_t getSize() const { return sizeof(wm_) + sizeof(StreamIdType); }

    /**
     * @brief Returns the payload of the specified Punctuation assuming that
     * it is of type @c WatermarkPunctPayload.
     *
     * The implementation asserts that the payload container is not null.
     *
     * @return pointer to the attached WatermarkPunctPayload object, or NULL if
     * no such payload is attached to @c punct
     */
    DLL_PUBLIC
    static WatermarkPunctPayload* getPayload(Punctuation const& punct);

  private:
    timestamp wm_;     // event-time value
    StreamIdType sid_; // stream Id
};
}

#endif /*SPL_RUNTIME_UTILITY_WATERMARKPUNCTPAYLOAD_H*/
