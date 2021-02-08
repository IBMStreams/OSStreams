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

#ifndef SPL_DSA_FLEXBUFF_H_
#define SPL_DSA_FLEXBUFF_H_

#include <cstddef>
#include <string>
#include <vector>

namespace SPL {

/// \brief Class that represents a buffer to represent container data with integrated c types
/// No std library types are handed over from one method to another
class FlexBuff
{
  public:
    /// Constructor
    FlexBuff();

    /// Destructor
    ~FlexBuff();

    /// Puts a single c-string into the buffer. Previous content is overwritten.
    /// @param val the c-string to put
    void put(char const* val);

    /// Puts a single string into the buffer with len indicator.  Previous content is overwritten.
    /// @param val the string to put
    /// @param size the string size
    void put(char const* val, size_t size);

    /// Puts data from a vector container into the buffer.  Previous content is overwritten.
    /// @param val the input data
    void put(std::vector<std::string> const& val);

    /// Get one data entry from the buffer
    /// @param elem handle (pointer) of the data element to get
    /// @param chars this parameter returns the pointer to the binary data. This data may contain 0
    /// characters and there is no terminating 0 character
    /// @param size this parameter returns the size of the data. If FlexBuff::npos is returned an
    /// error has occurred (wrong input data)
    /// @return returns the handle (pointer) of next element. Returns NULL if the buffer end is
    /// reached
    unsigned char const* get(unsigned char const* elem, char const*& chars, size_t& size) const;

    /// Checks whether the buffer is empty.
    /// The buffer is empty when start() == NULL.
    /// @return true is the buffer is empty, false otherwise.
    bool empty() const;

    /// Get the handle of the first element
    /// @return the handle (pointer) of first element. Returns NULL if the buffer is empty
    unsigned char const* start() const;

    /// Convert the whole buffer into string
    /// @param delim the delimiter between elements. Default value is a newline character.
    /// @return the whole buffer into a string. The last entry has no delimiter
    std::string str(char const delim = '\n') const;

    /// The constant that indicates an invalid length.
    static const size_t npos;

  private:
    unsigned char* buffer_;
    size_t bufferLen_;

    /// Write a size_t value to the memory pointed to by dest.
    /// @param dest the write pointer to which to write the size_t value
    /// @param s the size_t value to write
    /// @return the write pointer advanced past the write.
    unsigned char* put_size_t(unsigned char* dest, size_t s);

    /// Read a size_t value from the memory pointed to by dest.
    /// There must be enough memory to write a size_t.
    /// @param src the read pointer from which to read the size_t value
    /// @param s the returned size_t value.  The value on input is ignored.
    /// @return the read pointer advanced past the read.
    unsigned char const* get_size_t(unsigned char const* src, size_t& s) const;

    void mallocBuffer(size_t const len);
    void freeBuffer();
};

} // namespace SPL

#endif /* SPL_DSA_FLEXBUFF_H_ */
