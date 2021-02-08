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
#include <boost/iostreams/operations.hpp>
#include <boost/iostreams/concepts.hpp>
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <SPL/Toolkit/RuntimeException.h>


/// Converter from encoded file to UTF8 - Helper function for FileSource
namespace SPL {
    class ToUTF8Filter : public boost::iostreams::multichar_input_filter
    {
      public:
        ToUTF8Filter(const std::string& encoding)
        : obufCursor_(0), obufNContents_(0), ibufNContents_(0), encoding_(encoding)
        {
            converter_ = iconv_open ("UTF8", encoding_.c_str());
            if (converter_ == (iconv_t) -1) {
                SPLTRACEMSGANDTHROW(SPL::SPLRuntimeOperator, L_ERROR, SPL_APPLICATION_RUNTIME_UNABLE_OPEN_ENCODING(encoding_), SPL_OPER_DBG);
            }
        }

        ToUTF8Filter (const ToUTF8Filter& rhs)
        : obufCursor_(0), obufNContents_(0), ibufNContents_(0), encoding_(rhs.encoding_)
        {
            converter_ = iconv_open ("UTF8", encoding_.c_str());
            if (converter_ == (iconv_t) -1) {
                SPLTRACEMSGANDTHROW(SPL::SPLRuntimeOperator, L_ERROR, SPL_APPLICATION_RUNTIME_UNABLE_OPEN_ENCODING(encoding_), SPL_OPER_DBG);
            }
        }

        ~ToUTF8Filter() {
            iconv_close (converter_);
        }

        template<typename Source>
        std::streamsize read(Source& src, char* dst, std::streamsize n)
        {
            if (obufNContents_ > 0)
                return serveFromOBuffer(dst, n);
            fillBuffers(src);
            if (obufNContents_>0)
                return serveFromOBuffer(dst, n);
            if (lastCharRead_==boost::iostreams::WOULD_BLOCK)
                return 0;
            return -1;
        }

      private:
        std::streamsize serveFromOBuffer(char* dst, size_t n)
        {
            size_t nToServe = std::min(n, obufNContents_);
            memcpy(dst, outputBuffer_ + obufCursor_, nToServe);
            obufNContents_ -= nToServe;
            obufCursor_ += nToServe;
            return nToServe;
        }

        template<typename Source>
        void fillBuffers(Source & src)
        {
            readIntoInputBuffer(src);
            transferToOutputBuffer();
        }

        template<typename Source>
        void readIntoInputBuffer(Source& src)
        {
            size_t nRead  = 0;
            size_t nToRead  =  ibufSize_ - ibufNContents_;
            for (; nRead < nToRead; ++nRead) {
                lastCharRead_ = boost::iostreams::get(src);
                if (lastCharRead_ == EOF || lastCharRead_ == boost::iostreams::WOULD_BLOCK)
                   break;
                inputBuffer_[ibufNContents_+nRead] = lastCharRead_;
            }
            ibufNContents_ += nRead;
        }

        /// @pre obufNContents_ == 0
        void transferToOutputBuffer()
        {
            obufCursor_ = 0;
            char * iptr = inputBuffer_;
            char * optr = outputBuffer_;
            size_t inbytesleft = ibufNContents_;
            size_t outbytesleft = obufSize_;
            size_t res = iconv (converter_, &iptr, &inbytesleft, &optr, &outbytesleft);
            int ecode = errno;
            // iptr: begining of non-converted bytes
            // optr: begining of free bytes in output
            // inbytesleft: remaining bytes that are not converted
            // outbytesleft: remaining bytes that are free in output
            memmove(inputBuffer_, iptr, inbytesleft);
            ibufNContents_ = inbytesleft;
            obufNContents_ = obufSize_ - outbytesleft;
            if(res == (size_t) -1 && ecode == EILSEQ) {
                SPLAPPTRC(L_ERROR, SPL_APPLICATION_RUNTIME_INVALID_MULTIBYTE_SEQUENCE_DURING_TO_UTF8, SPL_OPER_DBG);
                THROW (SPL::SPLRuntimeOperator, SPL_APPLICATION_RUNTIME_INVALID_MULTIBYTE_SEQUENCE_DURING_TO_UTF8);
            }
        }

        enum { obufSize_ = 2048, ibufSize_ = 1024 };
        size_t obufCursor_;
        size_t obufNContents_;
        size_t ibufNContents_;
        char inputBuffer_[ibufSize_];
        char outputBuffer_[obufSize_];
        int lastCharRead_;
        std::string encoding_;
        iconv_t converter_;
    };
}
