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

#ifndef TRANS_VIEWSTREAM_MSG_HEADER_H_
#define TRANS_VIEWSTREAM_MSG_HEADER_H_

#include <UTILS/UTILSTypes.h>
#include <stdint.h>
#include <string.h>

static const char ViewStreamMsgHeaderEyeCatcher[] = "#VSMHE#";
static const uint8_t ViewStreamMsgHeaderVersion = 1;

// gcc versions < 4.4.x do not include htobexx macros
#ifndef be64toh
#include <byteswap.h>

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define htobe64(x) __bswap_64(x)
#define htole64(x) (x)
#define be64toh(x) __bswap_64(x)
#define le64toh(x) (x)
#else
#define htobe64(x) (x)
#define htole64(x) __bswap_64(x)
#define be64toh(x) (x)
#define le64toh(x) __bswap_64(x)
#endif

#endif // ! be64toh

#ifndef be32toh
#include <byteswap.h>

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define htobe32(x) __bswap_32(x)
#define htole32(x) (x)
#define be32toh(x) __bswap_32(x)
#define le32toh(x) (x)
#else
#define htobe32(x) (x)
#define htole32(x) __bswap_32(x)
#define be32toh(x) (x)
#define le32toh(x) __bswap_32(x)
#endif

#endif // ! be32toh

UTILS_NAMESPACE_BEGIN

/**
 * @brief Maps an externally allocated buffer to the message header of a
 * stream view tuple.
 *
 * A ViewStreamMsgHeader instance contains a tuple sequence number and an
 * array of view ids identifying the views the view tuple is associated with.
 * Integer values are stored in the memory buffer using Internet network byte
 * ordering (big-endian).
 *
 * The serialize and deserialize methods do not copy data, instead they return a
 * pointer to the serialization buffer, cast to a ViewStreamMsgHeader.  The
 * client should use this pointer to get or set the header data.
 *
 * @note This class is optimized for speed.  Only the serialization methods
 * perform bounds checking (the accessors do not).
 */
class ViewStreamMsgHeader
{
  public:
    enum
    {
        E_SUCCESS = 0,
        /** Serialization buffer is NULL. */
        E_NULL = 1,
        /** The serialization buffer size is too small to fit the number of
         * views decoded from the buffer (or too small to fit one view). */
        E_SIZE = 2
    };

    /** Get a string representation for the ViewStreamMsgHeader error codes. */
    static const char* errCodeToString(int code);

    /**
     * Deserialize a ViewStreamMsgHeader from the specified buffer buf.
     * The buffer's structure is validated and the specified error code
     * holder is set if the buffer does not hold a valid message header.
     *
     * This method does not copy data.  If the buffer is valid, it returns
     * the buffer pointer cast to ViewStreamMsgHeader* which the client can
     * later use to extract the header attributes.
     *
     * @param buf data buffer containing a ViewStreamMsgHeader representation
     *  using network byte ordering.
     * @param bufSize data buffer size
     * @param ec  pointer to an integer where the failure-specific error code
     *  will be copied (output).  Not changed if function is successful.
     * @return pointer to ViewStreamMsgHeader
     */
    static const ViewStreamMsgHeader* deserialize(const void* buf, const size_t bufSize, int& ec);

    /**
     * Deserialize a ViewStreamMsgHeader from the specified buffer buf.
     *
     * This method does not validate the buffer, instead it simply casts the
     * buffer pointer to a ViewStreamMsgHeader* which the client can
     * later use to extract the header attributes.
     *
     * @param buf data buffer containing a ViewStreamMsgHeader representation
     *  using network byte ordering.
     * @return pointer to ViewStreamMsgHeader
     */
    static const ViewStreamMsgHeader* deserialize(const void* buf);

    /**
     * Validates the specified buffer buf in preparation for serialization.
     *
     * This method checks if the specified buffer size is large enough to
     * include a serialized buffer with a number of view Ids equal to the
     * value specified by numIds.  If buffer size is valid, it sets the
     * number of view ids.
     *
     * @param buf data buffer containing a ViewStreamMsgHeader representation
     *  using network byte ordering.
     * @param bufSize data buffer size
     * @param channel parallel channel index of the originating operator
     * @param numIds number of view Ids contained in this header
     * @param ec  pointer to an integer where the failure-specific error code
     *  will be copied (output).  Not changed if function is successful.
     * @param numAllChannels number of parallel channel indexes
     * @return pointer to ViewStreamMsgHeader
     */
    static ViewStreamMsgHeader* serialize(void* buf,
                                          size_t bufSize,
                                          int channel,
                                          size_t numIds,
                                          int& ec,
                                          size_t numAllChannels = 0);

    /**
     * Casts the specified buffer buf as a ViewStreamMsgHeader and sets the
     * header data.
     *
     * This method does no bounds checking.
     *
     * @param buf data buffer containing a ViewStreamMsgHeader representation
     *  using network byte ordering.
     * @param seq  tuple sequence number
     * @param channel parallel channel index of the originating operator
     *   (defaults to -1)
     * @param numIds number of view Ids contained in this header
     *   (defaults to 1)
     * @return pointer to the data buffer cast as a ViewStreamMsgHeader
     */
    static ViewStreamMsgHeader* serialize(void* buf,
                                          unsigned long long seq,
                                          int channel = -1,
                                          size_t numIds = 1);

    void serializeExtendedHeader(const int32_t* allChannels, size_t allChannelsLen);

    /** Get the tuple sequence number. */
    unsigned long long getSequenceNumber() const { return be64toh(data_.seqNum); }

    /** Get the channel index of the originating operator. */
    int getChannelIndex() const { return be32toh(data_.channelIndex); }

    /** Get the number of view Ids from the header. */
    size_t getNumViewIds() const { return be32toh(data_.numViewIds); }

    char* getEyeCatcher() const { return (char*)getExtendedHeader(); }

    uint8_t getVersion() const { return *getVersionPtr(); }

    size_t getNumAllChannels() const { return be32toh(*getNumAllChannelsPtr()); }

    int32_t getAllChannelsIndex(size_t pos) const { return be32toh(*getAllChannelsIndexPtr(pos)); };

    /** Get the view Id from the specified location pos (no bounds checking). */
    unsigned long long getViewId(size_t pos = 0) const
    {
        const uint64_t* viewIds = data_.viewIds;
        return be64toh(viewIds[pos]);
    }

    /** Get the message header size after serialization. */
    size_t size() const { return ViewStreamMsgHeader::size(getNumViewIds(), getNumAllChannels()); }

    /** Set the view Id at the specified location pos (no bounds checking). */
    void setViewId(unsigned long long value, size_t pos = 0)
    {
        data_.viewIds[pos] = htobe64(value);
    }

    /** Set the tuple sequence number. */
    void setSequenceNumber(unsigned long long value) { data_.seqNum = htobe64(value); }

    /** Set the channel index of the originating operator. */
    void setChannelIndex(int value) { data_.channelIndex = htobe32(value); }

    /**
     * Returns the serialization size of a ViewStreamMsgHeader with the
     * specified number of view Ids.
     */
    static size_t size(size_t numViewIds, size_t numAllChannels = 0)
    {
        return originalHeaderSize(numViewIds) + extendedHeaderSize(numAllChannels);
    }

    /** Print internal state.     */
    friend std::ostream& operator<<(std::ostream& o, const ViewStreamMsgHeader& ref)
    {
        return ref.print(o);
    }

  private:
    ViewStreamMsgHeader() {}
    ViewStreamMsgHeader(const ViewStreamMsgHeader&) {}
    ~ViewStreamMsgHeader() {}
    ViewStreamMsgHeader& operator=(const ViewStreamMsgHeader&) { return *this; }

    // Clients can only set number of view ids via the serialize method
    void setNumViewIds(size_t value) { data_.numViewIds = htobe32(value); }

    // seqNum + channelIndex + numViewIds + viewIds
    static size_t originalHeaderSize(size_t numViewIds)
    {
        return sizeof(uint64_t) + 2 * sizeof(uint32_t) + numViewIds * sizeof(uint64_t);
    }

    // eye catcher + version + numAllChannels + allChannels
    static size_t extendedHeaderSize(size_t numAllChannels)
    {
        return sizeof(ViewStreamMsgHeaderEyeCatcher) + sizeof(ViewStreamMsgHeaderVersion) +
               sizeof(uint32_t) + numAllChannels * sizeof(int32_t);
    }

    uint8_t* getExtendedHeader() const
    {
        uint8_t* dataPtr = (uint8_t*)&data_;
        return (dataPtr + originalHeaderSize(getNumViewIds()));
    }

    void setEyeCatcher()
    {
        memcpy(getEyeCatcher(), ViewStreamMsgHeaderEyeCatcher,
               sizeof(ViewStreamMsgHeaderEyeCatcher));
    }

    uint8_t* getVersionPtr() const
    {
        return (uint8_t*)(getEyeCatcher() + sizeof(ViewStreamMsgHeaderEyeCatcher));
    }

    void setVersion() { *getVersionPtr() = ViewStreamMsgHeaderVersion; }

    uint32_t* getNumAllChannelsPtr() const { return (uint32_t*)(getVersionPtr() + 1); }

    void setNumAllChannels(uint32_t value) { *getNumAllChannelsPtr() = htobe32(value); }

    int32_t* getAllChannelsIndexPtr(size_t pos) const
    {
        int32_t* allChannels = (int32_t*)(getNumAllChannelsPtr() + 1);
        return allChannels + pos;
    }

    void setAllChannelsIndex(int32_t value, size_t pos)
    {
        *getAllChannelsIndexPtr(pos) = htobe32(value);
    }

    std::ostream& print(std::ostream& o) const;

    struct
    {
        ///< Begin original header
        uint64_t seqNum;      ///< Message sequence number
        int32_t channelIndex; ///< Index of the parallel channel of the
                              ///< operator originating the tuple
        uint32_t numViewIds;  ///< Number of view Ids
        uint64_t viewIds[0];  ///< Variable-length array of view Ids
                              ///< ...
                              ///< Begin extended header
        // char eyeCatcher[8];      ///< #VSMHE#\0
        // uint8_t version;         ///< Header version
        // uint32_t numAllChannels; ///< Number of parallel channel indexes
        // int32_t allChannels[0];  ///< Variable-length array of parallel channel indexes
        ///< ...
    } __attribute__((packed)) data_;
};

/////////////////////////////////////////////////////////////////////////////
// Implementation
inline const ViewStreamMsgHeader* ViewStreamMsgHeader::deserialize(const void* buf)
{
    return reinterpret_cast<const ViewStreamMsgHeader*>(buf);
}

inline ViewStreamMsgHeader* ViewStreamMsgHeader::serialize(void* buf,
                                                           unsigned long long seq,
                                                           int channel,
                                                           size_t numIds /*= 1*/)
{
    ViewStreamMsgHeader* hdr = reinterpret_cast<ViewStreamMsgHeader*>(buf);
    hdr->setSequenceNumber(seq);
    hdr->setChannelIndex(channel);
    hdr->setNumViewIds(numIds);
    return hdr;
}

inline void ViewStreamMsgHeader::serializeExtendedHeader(const int32_t* allChannels,
                                                         size_t allChannelsLen)
{
    setEyeCatcher();
    setVersion();
    setNumAllChannels(allChannelsLen);
    // serialize allChannels
    for (size_t i = 0; i < allChannelsLen; i++) {
        setAllChannelsIndex(*allChannels++, i);
    }
}

UTILS_NAMESPACE_END

#endif // ! TRANS_VIEWSTREAM_MSG_HEADER_H_
