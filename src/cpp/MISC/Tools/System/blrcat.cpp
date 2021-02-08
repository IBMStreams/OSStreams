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

#include <UTILS/BufferedLineReader.h>
#include <UTILS/DistilleryApplication.h>
#include <fcntl.h>
#include <iostream>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;
UTILS_NAMESPACE_USE

class blrcat : public DistilleryApplication
{
  public:
    blrcat(void)
      : _maxbufsize(512)
      , _blocksize(64 * 1024)
      , _DOSEOL(false)
      , _MACEOL(false){};

    void setInputFile(const option_t* option, const char* value) { _inputFile = string(value); }

    void setDOSEOL(const option_t* /*option*/, const char* /*value*/) { _DOSEOL = true; }

    void setMACEOL(const option_t* /*option*/, const char* /*value*/) { _MACEOL = true; }

    void setBlockSize(const option_t* option, const int value)
    {
        if (value < 0) {
            cerr << "ERROR: block size must be greater than zero" << endl;
            exit(EXIT_FAILURE);
        }
        _blocksize = static_cast<uint32_t>(value);
    }

    void setBufferSize(const option_t* option, const int value)
    {
        if (value < 0) {
            cerr << "ERROR: buffer size must be greater than zero" << endl;
            exit(EXIT_FAILURE);
        }
        _maxbufsize = static_cast<uint32_t>(value);
    }

    void getArguments(option_vector_t& options)
    {
        option_t args[] = {
            { 'B', "buffer-size", ARG, "", "read buffer size", INT_OPT(blrcat::setBufferSize), 0,
              0 },
            { 'b', "block-size", ARG, "", "read block size", INT_OPT(blrcat::setBlockSize), 0, 0 },
            { 'd', "dos", 0, "", "DOS-style end of line marker (CR+LF)", STR_OPT(blrcat::setDOSEOL),
              0, 0 },
            { 'f', "file", ARG, "", "input file", STR_OPT(blrcat::setInputFile), 0, 0 },
            { 'm', "dos", 0, "", "MAC-style end of line marker (CR)", STR_OPT(blrcat::setMACEOL), 0,
              0 },
        };
        APPEND_OPTIONS(options, args);
    }

    virtual int run(const arg_vector_t& args)
    {
        if (_inputFile.empty()) {
            cerr << "ERROR: file name not specified" << endl;
            return EXIT_FAILURE;
        }
        int fd = open(_inputFile.c_str(), O_RDONLY);
        BufferedLineReader blr(fd, _blocksize);
        while (!blr.isEof()) {
            char buf[_maxbufsize];
            uint32_t linelen;
            if (_DOSEOL) {
                linelen = blr.dosgetLine(buf, _maxbufsize - 1);
            } else {
                linelen = blr.getLine(buf, _maxbufsize - 1, _MACEOL ? '\r' : '\n');
            }
            if (blr.isEof() && linelen == 0) {
                break;
            }
            buf[linelen] = '\0';
            if (_MACEOL) {
                cout << buf << endl;
            } else {
                cout << buf;
            }
        }
        close(fd);
        return EXIT_SUCCESS;
    }

  private:
    string _inputFile;
    uint32_t _maxbufsize;
    uint32_t _blocksize;
    bool _DOSEOL;
    bool _MACEOL;
};

MAIN_APP(blrcat);
