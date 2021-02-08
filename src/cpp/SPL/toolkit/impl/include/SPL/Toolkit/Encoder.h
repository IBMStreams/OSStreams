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
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Common/RuntimeDebugAspect.h>
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <SPL/Toolkit/RuntimeException.h>
#include <SPL/Runtime/Type/Blob.h>


/// Character set converter - blob to blob
namespace SPL {
    class Encoder {
      public:
        Encoder(const std::string& from, const std::string& to)
        : ibufNContents_(0), from_(from), to_(to)
        {
            converter_ = iconv_open (to.c_str(), from.c_str());
            if (converter_ == (iconv_t) -1)
                THROW (SPL::SPLRuntimeOperator, SPL_APPLICATION_RUNTIME_UNABLE_OPEN_CONVERSION(from, to));
        }

        ~Encoder() {
            iconv_close (converter_);
        }

        void convert (const blob& input, blob& output)
        {
            size_t inBytes;
            char * inputData = (char *) input.getData (inBytes);
            char *originalInputData = inputData;
            std::auto_ptr<char> tmp_buf;
            if (ibufNContents_) {
                // We have leftover input data.  Make a new buffer with data combined
                tmp_buf.reset(new char[ibufNContents_+inBytes]);
                memcpy (tmp_buf.get(), iBuf_, ibufNContents_);
                memcpy (tmp_buf.get()+ibufNContents_, inputData, inBytes);
                inputData = tmp_buf.get();
                inBytes += ibufNContents_;
            }
            // Now do the conversion
            for (;;) {
                char outputBuffer[64*1024];
                char *oPtr = outputBuffer;
                size_t oBytesLeft = sizeof (outputBuffer);
                int ret = iconv (converter_, &inputData, &inBytes, &oPtr, &oBytesLeft);
                int ecode = errno;
                // Save what we have so far
                output.append ((unsigned char *) outputBuffer,
                               sizeof (outputBuffer)-oBytesLeft);

                // How did we do?
                if (ret >= 0) {
                    return; // success
                }
                if (ecode == E2BIG) {
                    // Output buffer is full - just continue from where we left off
                    continue;
                } else if (ecode == EILSEQ) {
                    // Invalid sequence
                    SPLTRACEMSGANDTHROW(SPL::SPLRuntimeOperator, L_ERROR, SPL_APPLICATION_RUNTIME_INVALID_MULTIBYTE_SEQUENCE_DURING_CONVERSION(from_, to_), SPL_OPER_DBG);
                } else if (ecode == EINVAL) {
                    // Ran out of input in the middle of a multi-byte sequence
                    memcpy (iBuf_, inputData, inBytes);
                    ibufNContents_ = inBytes;
                    return;
                }
            }
        }
      private:
        enum { ibufSize_ = 1024 };
        size_t ibufNContents_;
        char iBuf_[ibufSize_];
        std::string from_, to_;
        iconv_t converter_;
    };
}
