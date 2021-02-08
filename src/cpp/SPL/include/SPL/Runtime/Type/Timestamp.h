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

#ifndef SPL_RUNTIME_TYPE_TIMESTAMP_H
#define SPL_RUNTIME_TYPE_TIMESTAMP_H

/*!
 * \file Timestamp.h \brief Definition of the SPL::timestamp class.
 */

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Serialization/NativeByteBuffer.h>
#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Type/TypeHelper.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <stdint.h>
#include <limits>
#include <boost/config.hpp>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

#include <math.h>
PUSH_DLL_PUBLIC
#define TS_NS_IN_SEC 1000000000
namespace SPL
{
    /// \brief Class that represents a timestamp type
    ///
    /// Common usage is to store the seconds and nanoseconds since the
    /// Unix Epoch (Jan 1, 1970), but this is not enforced by the timestamp class.
    /// machineId may be used to detect if two timestamps were created on the same
    /// machine, as there may be variations in the clocks on different machines.
    /// machineId's are ignored for comparisons and arithmetic operations on timestamps.
    class timestamp
    {
    public:
        /// Empty constructor
        ///
        BOOST_CONSTEXPR
        timestamp() : secs_(0), nanosecs_(0), machineId_(0) {}

        /// Constructor
        /// @param secs seconds since epoch
        /// @param nanosecs nanoseconds
        /// @param machineId machine id
        timestamp(int64_t secs, uint32_t nanosecs, uint32_t machineId=0)
            : secs_(secs), nanosecs_(nanosecs), machineId_(machineId)
        {
            normalize();
        }

        /// Constructor
        /// @param time time in seconds, could be fractional
        timestamp(double time)
            : machineId_(0)
        {
            double secs = ::floor(time);
            secs_ = (int64_t) secs;
            double nsecs = (time-secs)*TS_NS_IN_SEC;
            nanosecs_ = (uint32_t) nsecs;
            normalize();
        }

        /// Copy contructor
        /// @param ot other timestamp
        BOOST_CONSTEXPR
        timestamp(const timestamp& ot)
            : secs_(ot.secs_),
              nanosecs_(ot.nanosecs_),
              machineId_(ot.machineId_) {}

        /// Set seconds since epoch and nanoseconds
        /// @param secs seconds since epoch
        /// @param nanosecs nanoseconds
        /// @param machineId machine id
        void set(int64_t secs, uint32_t nanosecs, uint32_t machineId=0)
        {
            secs_ = secs;
            nanosecs_ = nanosecs;
            machineId_ = machineId;
            normalize();
        }

        /// Set seconds since epoch
        /// @param secs seconds since epoch
        void setSeconds(int64_t secs)
            { secs_ = secs; }

        /// Set nanoseconds
        /// @param nanosecs nanoseconds
        void setNanoSeconds(uint32_t nanosecs)
        {
            if (nanosecs > TS_NS_IN_SEC) {
                SPLAPPTRC (L_ERROR, "Nanoseconds value '" << nanosecs << "' is too large - truncated", SPL_OPER_DBG);
                nanosecs %= TS_NS_IN_SEC;
            }
            nanosecs_ = nanosecs;
        }

        /// Set machine id
        /// @param machineId the machine id
        void setMachineId(int32_t machineId)
            { machineId_ = machineId; }

        /// Get seconds since epoch
        /// @return the seconds since epoch component
        int64_t getSeconds() const
            { return secs_; }

        /// Get nanoseconds
        /// @return the nanosecond component
        uint32_t getNanoseconds() const
            { return nanosecs_; }

        /// Get machine id
        /// @return the machine id
        int32_t getMachineId() const
            { return machineId_; }

        /* arithmetic operations */

        /// Difference. The machine id from the left-hand-size operand is used in result.
        /// @param rhs timestamp to subtract
        /// @return difference between the timestamps
        timestamp operator- (const timestamp& rhs) const
        {
            timestamp diff;
            diff.secs_ = secs_ -  rhs.secs_;
            int64_t nDiff = static_cast<int64_t>(nanosecs_) - rhs.nanosecs_;
            while (nDiff < 0) {
                diff.secs_--;
                nDiff += TS_NS_IN_SEC;
            }
            diff.nanosecs_ = nDiff;
            diff.machineId_ = machineId_;
            return diff;
        }

        /// Difference (assignment)
        /// @param rhs timestamp to subtract
        /// @return *this
        timestamp& operator-= (const timestamp& rhs)
        {
            secs_ -= rhs.secs_;
            int64_t nDiff = static_cast<int64_t>(nanosecs_) - rhs.nanosecs_;
            while (nDiff < 0) {
                secs_--;
                nDiff += TS_NS_IN_SEC;
            }
            nanosecs_ = nDiff;
            return *this;
        }

        /// Sum. The machine id from the left-hand-size operand is used in result.
        /// @param rhs timestamp to add
        /// @return sum of the timestamps
        timestamp operator+ (const timestamp& rhs) const
        {
            timestamp sum;
            sum.secs_ = secs_ +  rhs.secs_;
            sum.nanosecs_ = nanosecs_ + rhs.nanosecs_;
            sum.normalize();
            sum.machineId_ = machineId_;
            return sum;
        }

        /// Sum (assignment)
        /// @param rhs timestamp to add
        /// @return *this
        timestamp& operator+= (const timestamp& rhs)
        {
            secs_ += rhs.secs_;
            nanosecs_ += rhs.nanosecs_;
            normalize();
            return *this;
        }

        /// Equality comparison
        /// @param rhs timestamp to compare with
        /// @return true if the timestamps are the same, false otherwise
        bool operator==(const timestamp& rhs) const
        {
            if (secs_ != rhs.secs_) {
                return false;
            }
            if (nanosecs_ != rhs.nanosecs_) {
                return false;
            }
            return true;
        }

        /// Non-equality comparison
        /// @param rhs timestamp to compare with
        /// @return true if the timestamps are not the same, false otherwise
        bool operator!=(const timestamp& rhs) const
        { return !(*this==rhs); }

        /// Less than comparison
        /// @param rhs timestamp to compare with
        /// @return true if timestamp < rhs, false otherwise
        bool operator<(const timestamp& rhs) const
        {
            if (secs_ > rhs.secs_) {
                return false;
            }
            if (secs_ == rhs.secs_) {
                return nanosecs_ < rhs.nanosecs_;
            }
            return true;
        }

        /// Greater than or equal comparison
        /// @param rhs timestamp to compare with
        /// @return true if timestamp >= rhs, false otherwise
        bool operator>=(const timestamp& rhs) const
        { return !(*this < rhs); }

        /// Greater than comparison
        /// @param rhs timestamp to compare with
        /// @return true if timestamp > rhs, false otherwise
        bool operator>(const timestamp& rhs) const
        {
            if (secs_ < rhs.secs_) {
                return false;
            }
            if (secs_ == rhs.secs_) {
                return nanosecs_ > rhs.nanosecs_;
            }
            return true;
        }

        /// Less than or equal comparison
        /// @param rhs timestamp to compare with
        /// @return true if timestamp <= rhs, false otherwise
        bool operator<=(const timestamp& rhs) const
        {
            return !(*this > rhs);
        }

        /// Swap
        /// @param ot timestamp to swap with
        void swap(timestamp& ot)
        {
            int64_t tmp64 = secs_;
            secs_ = ot.secs_;
            ot.secs_ = tmp64;
            uint32_t tmp32 = nanosecs_;
            nanosecs_ = ot.nanosecs_;
            ot.nanosecs_ = tmp32;
            tmp32 = machineId_;
            machineId_ = ot.machineId_;
            ot.machineId_ = machineId_;
        }

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Serialize (binary)
        /// @param buf serialization buffer to use
        void serialize(VirtualByteBuffer & buf) const
        {
            buf << secs_;
            buf << nanosecs_;
            buf << machineId_;
        }

        /// Deserialize (binary)
        /// @param buf serialization buffer to use
        void deserialize(VirtualByteBuffer & buf)
        {
            buf >> secs_;
            buf >> nanosecs_;
            buf >> machineId_;
        }
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Serialize (binary)
        /// @param buf byte buffer to use
        template <class BufferType>
        void serialize(ByteBuffer<BufferType> & buf) const
        {
            buf << secs_;
            buf << nanosecs_;
            buf << machineId_;
        }

        /// Deserialize (binary)
        /// @param buf byte buffer to use
        template <class BufferType>
        void deserialize(ByteBuffer<BufferType> & buf)
        {
            buf >> secs_;
            buf >> nanosecs_;
            buf >> machineId_;
        }

        /// Serialize (character)
        /// @param ostr output stream
        void serialize(std::ostream & ostr) const
        {
            ostr << "(" << secs_
                 << "," << nanosecs_
                 << "," << machineId_ << ")";
        }

        /// Deserialize (character)
        /// @param istr input stream
        void deserialize(std::istream & istr);

        /// Get hash code
        /// @return hash code
        size_t hashCode() const
        {
            size_t r = 17;
            std::tr1::hash<int64_t> hs;
            r = 37 * r + hs(secs_);
            r = 37 * r + hs(nanosecs_);
            return r;
        }

        /// Get the size in bytes when serialized
        /// @return size in bytes when serialized
        size_t getSerializedSize() const
        {
            return sizeof(secs_) +
                sizeof(nanosecs_) +
                sizeof(machineId_);
        }

        /// The minimum timestamp value
        // (since the seconds part can be negative, this is not really
        // the minimum)
         BOOST_CONSTEXPR
        static
        timestamp min() { return timestamp(); }
        /// The maximum timestamp value
        BOOST_CONSTEXPR
        static timestamp max() { return timestamp(std::numeric_limits<int64_t>::max(), TS_NS_IN_SEC - 1, NoNormalize()); }

    private:
#ifndef DOXYGEN_SKIP_FOR_USERS
        struct NoNormalize {};
        // constexpr contructor for when we know that the call to
        // normalize is not needed.
        BOOST_CONSTEXPR
        timestamp(int64_t secs, uint32_t nanosecs, NoNormalize const &)
            : secs_(secs), nanosecs_(nanosecs), machineId_(0)
        {
        }
#endif /* DOXYGEN_SKIP_FOR_USERS */
        int64_t secs_;
        uint32_t nanosecs_;
        int32_t machineId_;
        void normalize()
        {
            while (nanosecs_ >= TS_NS_IN_SEC) {
                secs_++;
                nanosecs_ -= TS_NS_IN_SEC;
            }
        }
    };

    inline std::ostream & operator<<(std::ostream & ostr, const timestamp & x)
        { x.serialize(ostr); return ostr; }

    inline std::istream & operator>>(std::istream & istr, timestamp & x)
        { x.deserialize(istr); return istr; }

#ifndef DOXYGEN_SKIP_FOR_USERS
    inline VirtualByteBuffer & operator<<(VirtualByteBuffer & buf, const timestamp & x)
        { x.serialize(buf); return buf; }

    inline VirtualByteBuffer & operator>>(VirtualByteBuffer & buf, timestamp & x)
        { x.deserialize(buf); return buf; }
#endif /* DOXYGEN_SKIP_FOR_USERS */

    template <class S>
    inline ByteBuffer<S> & operator<<(ByteBuffer<S> & buf, const timestamp & x)
        { x.serialize(buf); return buf; }

    inline NetworkByteBuffer & operator<<(NetworkByteBuffer & buf, const timestamp & x)
        { x.serialize(buf); return buf; }

    inline NativeByteBuffer & operator<<(NativeByteBuffer & buf, const timestamp & x)
        { x.serialize(buf); return buf; }

    template <class S>
    inline ByteBuffer<S> & operator>>(ByteBuffer<S> & buf, timestamp & x)
        { x.deserialize(buf); return buf; }

    inline NetworkByteBuffer & operator>>(NetworkByteBuffer & buf, timestamp & x)
        { x.deserialize(buf); return buf; }

    inline NativeByteBuffer & operator>>(NativeByteBuffer & buf, timestamp & x)
        { x.deserialize(buf); return buf; }
};

#ifndef DOXYGEN_SKIP_FOR_USERS
namespace std {
  template<>
  inline void swap(SPL::timestamp& a, SPL::timestamp& b)
    { a.swap( b ); }

/// numeric_limits<SPL::timestamp> specialization.
  template<>
  struct numeric_limits<SPL::timestamp>
  {
     BOOST_CONSTEXPR
    static SPL::timestamp
    min()
#ifdef BOOST_NO_NOEXCEPT
       throw()
#else
       noexcept
#endif
    { return SPL::timestamp::min(); }

    BOOST_CONSTEXPR
    static SPL::timestamp
    max()
#ifdef BOOST_NO_NOEXCEPT
       throw()
#else
       noexcept
#endif
    { return SPL::timestamp::max(); }
  };
} // end namespace std


// Hashability requirements
namespace std {
    namespace tr1 {
        template<>
        struct hash<SPL::timestamp> {
            inline size_t operator()(const SPL::timestamp& t) const
            { return t.hashCode(); }
        };
    }
}
#endif /* DOXYGEN_SKIP_FOR_USERS */
#undef TS_NS_IN_SEC

POP_DLL_PUBLIC
#endif /* SPL_RUNTIME_TYPE_TIMESTAMP_H */
