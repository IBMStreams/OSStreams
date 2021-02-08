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

#include "../Utility/UtlTestCommon.h"

#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Utility/LinuxPipe.h>
#include <SPL/Runtime/Utility/Thread.h>
#include <UTILS/DistilleryApplication.h>

#include <sstream>
#include <stdlib.h>

using namespace std;
using namespace SPL;
using namespace Distillery;

namespace SPL {

class LinuxPipeTest : public DistilleryApplication
{
  public:
    LinuxPipeTest() {}

    class Reader : public Thread
    {
      public:
        Reader(LinuxPipe& pipe)
          : pipe_(pipe)
        {}
        void* run(void* args)
        {
            try {
                LinuxPipe::LineOutput res;
                while (!pipe_.readLine(res)) {
                    if (res.hasStdOutLine()) {
                        lines_.push_back(res.getStdOutLine());
                    }
                }
            } catch (SPLRuntimeException const& e) {
                cerr << "Error during pipe read: " << e << endl;
                throw;
            }
            return NULL;
        }
        vector<string> const& getLines() const { return lines_; }

      private:
        LinuxPipe& pipe_;
        vector<string> lines_;
    };

    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        LinuxPipe pipe;
        try {
            pipe.setup("sed 's/streams/STREAMS/g' | sed 's/spl/SPL/g'");
        } catch (SPLRuntimeException const& e) {
            cerr << "Error during pipe setup: " << e.getExplanation() << endl;
            throw;
        }
        Reader reader(pipe);
        reader.create();
        vector<string> input;
        for (int i = 0; i < 10000; ++i) {
            ostringstream ostr;
            ostr << "streams " << i << " spl";
            input.push_back(ostr.str());
        }
        try {
            for (size_t i = 0, iu = input.size(); i < iu; i++) {
                pipe.writeLine(input[i]);
            }
        } catch (SPLRuntimeException const& e) {
            cerr << "Error during pipe write: " << e.getExplanation() << endl;
            throw;
        }
        pipe.shutdown();
        FASSERT(reader.getLines().size() == input.size());
        for (size_t i = 0, iu = input.size(); i < iu; i++) {
            ostringstream ostr;
            ostr << "STREAMS " << i << " SPL";
            FASSERT(reader.getLines()[i] == ostr.str());
        }
        reader.join();
        return 0;
    }
};

};

MAIN_APP(SPL::LinuxPipeTest)
