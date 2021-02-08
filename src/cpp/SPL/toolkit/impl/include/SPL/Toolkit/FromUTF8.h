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

#include <iconv.h>
#include <ios>
#include <string>
#include <errno.h>
#include <boost/iostreams/operations.hpp>
#include <boost/iostreams/concepts.hpp>
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <SPL/Toolkit/RuntimeException.h>

/// Converter from UTF8 to encoded file - Helper function for FileSink
namespace SPL {
    class FromUTF8Filter : public boost::iostreams::multichar_output_filter
    {
      public:
        FromUTF8Filter(const std::string& encoding)
        : ibufNContents_(0), encoding_(encoding)
        {
            converter_ = iconv_open (encoding_.c_str(), "UTF8");
            if (converter_ == (iconv_t) -1) {
                SPLTRACEMSGANDTHROW(SPL::SPLRuntimeOperator, L_ERROR, SPL_APPLICATION_RUNTIME_UNABLE_OPEN_ENCODING(encoding_), SPL_OPER_DBG);
            }
        }

        FromUTF8Filter(const FromUTF8Filter& rhs)
        : ibufNContents_(0), encoding_(rhs.encoding_)
        {
            converter_ = iconv_open (encoding_.c_str(), "UTF8");
            if (converter_ == (iconv_t) -1) {
                SPLTRACEMSGANDTHROW(SPL::SPLRuntimeOperator, L_ERROR, SPL_APPLICATION_RUNTIME_UNABLE_OPEN_ENCODING(encoding_), SPL_OPER_DBG);
            }
        }

        ~FromUTF8Filter() {
            iconv_close (converter_);
        }

        const std::string& encoding() const { return encoding_;}

        template<typename Sink>
        std::streamsize write(Sink& sink, char const * src, std::streamsize n)
        {
            std::streamsize nRemaining = n;
            while(nRemaining > 0) {
                if(ibufNContents_==0) {
                    // convert all directly from source, put any remaining
                    // partial character bytes into the input buffer
                    serveFromSource(sink, src, nRemaining);
                    break;
                } else {
                    std::streamsize nRead = readIntoInputBuffer(src, nRemaining);
                    // convert all from the input buffer, move any remaining
                    // partial character bytes to the beginning of the input
                    // buffer
                    serveFromSource(sink, inputBuffer_, ibufNContents_);
                    nRemaining -= nRead;
                    src += nRead;
                }
            }
            return n;
        }

    private:
        template<typename Sink>
        void serveFromSource(Sink& sink, char const * src, std::streamsize n)
        {
            char * iptr = const_cast<char *>(src);
            size_t inbytesleft = n;
            while (true) {
                char * optr = outputBuffer_;
                size_t outbytesleft = obufSize_;
                size_t res = iconv (converter_, &iptr, &inbytesleft, &optr, &outbytesleft);
                int ecode = errno;
                // iptr: begining of non-converted bytes
                // optr: begining of free bytes in output
                // inbytesleft: remaining bytes that are not converted
                // outbytesleft: remaining bytes that are free in output
                size_t obufNContents = obufSize_ - outbytesleft;
                writeAll (sink, outputBuffer_, obufNContents);
                if(res == (size_t) -1) {
                    if(ecode == EILSEQ) { // error, invalid sequence
                        ibufNContents_ = 0;
                        SPLAPPTRC(L_ERROR, SPL_APPLICATION_RUNTIME_INVALID_MULTIBYTE_SEQUENCE_DURING_FROM_UTF8, SPL_OPER_DBG);
                        THROW (SPL::SPLRuntimeOperator, SPL_APPLICATION_RUNTIME_INVALID_MULTIBYTE_SEQUENCE_DURING_FROM_UTF8);
                    } else if (ecode == EINVAL) { // partial character data
                        memmove(inputBuffer_, iptr, inbytesleft);
                        ibufNContents_ = inbytesleft;
                        return;
                    } else {// output full (E2BIG)
                        continue;
                    }
                } else {
                    ibufNContents_ = 0;
                    return;
                }
            }
        }

        std::streamsize readIntoInputBuffer(char const * src, std::streamsize n)
        {
            size_t ibufNFree = ibufSize_ - ibufNContents_;
            size_t nToRead = std::min(size_t(n), ibufNFree);
            char * dst = inputBuffer_+ibufNContents_;
            memcpy(dst, src, nToRead);
            ibufNContents_ += nToRead;
            return nToRead;
        }

        template<typename Sink>
        void writeAll(Sink& sink, char const * src, std::streamsize n)
        {
            while(n>0) {
                std::streamsize w = boost::iostreams::write (sink, src, n);
                n -= w;
                src += w;
            }
        }

      private:
        enum { obufSize_ = 1024, ibufSize_ = 2048 };
        size_t ibufNContents_;
        char inputBuffer_[ibufSize_];
        char outputBuffer_[obufSize_];
        std::string encoding_;
        iconv_t converter_;
    };
}
