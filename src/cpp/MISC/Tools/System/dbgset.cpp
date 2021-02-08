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

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <TRC/DistilleryDebug.h>
#include <UTILS/DistilleryException.h>
#include <iostream>
#include <sstream>

#include <UTILS/DistilleryApplication.h>
#include <UTILS/SymbolReader.h>

using namespace std;
UTILS_NAMESPACE_USE;

class PTraceCallback : public SymbolReader::Callback
{
  protected:
    pid_t _pid;
    bool _enable;

  public:
    PTraceCallback(pid_t pid, bool enable)
      : _pid(pid)
      , _enable(enable)
    {
        if (ptrace(PTRACE_ATTACH, _pid, NULL, NULL) != 0) {
            perror("ptrace(attach)");
            exit(1);
        }

        int status;
        waitpid(_pid, &status, WUNTRACED);

        if (WIFSTOPPED(status)) {
            cout << "Process " << _pid << " attached" << endl;
        } else {
            cerr << "waitpid(" << _pid << ") did not returned that the child is ready" << endl;
            exit(1);
        }
    }

    virtual ~PTraceCallback()
    {
        if (ptrace(PTRACE_DETACH, _pid, NULL, NULL) != 0) {
            perror("ptrace(detach)");
            exit(1);
        }

        cout << "Process " << _pid << " detached" << endl;
    }

    virtual void set(volatile unsigned long* addr)
    {
        long v = ptrace(PTRACE_PEEKDATA, _pid, addr, NULL);

        cout << "current value[" << addr << "]: " << v << endl;

        if (_enable) {
            v = 1;
        } else {
            v = 0;
        }

        if (ptrace(PTRACE_POKEDATA, _pid, addr, v) != 0) {
            perror("ptrace(pokedata");
            exit(1);
        }
    }
};

class dbgset : public DistilleryApplication
{

  protected:
    pid_t _pid;
    string _file;
    int _line;
    int _level;
    bool _enable;
    string _executable;

  public:
    dbgset(void)
      : _pid(0)
      , _line(0)
      , _level(-1)
      , _enable(true)
    {}

    void getArguments(option_vector_t& options)
    {
        option_t args[] = {
            { 'p', "pid", ARG | REQUIRED, "", "Pid of the process to configure",
              INT_OPT(dbgset::setPid) },
            { 'd', "level", ARG, "", "Level to enable", INT_OPT(dbgset::setLevel) },
            { 'f', "file", ARG, "", "Filename of the debug statements to enable",
              STR_OPT(dbgset::setFile) },
            { 'l', "line", ARG, "", "Line numnber of the debug statement to enable",
              INT_OPT(dbgset::setLine) },
            { 'i', "invert", 0, "", "Turn off debug (default is to turn on)",
              INT_OPT(dbgset::setInvert) },
            { 'e', "executable", ARG, "", "Executable to load (/proc/<pid>/exe by default)",
              STR_OPT(dbgset::setExecutable) },
        };

        APPEND_OPTIONS(options, args);
    }

    void setPid(const option_t* option, int value) { _pid = value; }

    void setLine(const option_t* option, int value) { _line = value; }

    void setFile(const option_t* option, const char* value) { _file = value; }

    void setLevel(const option_t* option, int value) { _level = value; }

    void setInvert(const option_t* option, int value) { _enable = false; }

    void setExecutable(const option_t* option, const char* value) { _executable = value; }

    virtual int run(const vector<string>& remainings_args)
    {
        if (_executable == "") {
            stringstream exe;
            exe << "/proc/" << _pid << "/exe";
            _executable = exe.str();
        }

        SymbolReader sr(_executable);
        PTraceCallback cb(_pid, _enable);

        if (_level >= 0) {
            sr.iterateByLevel(_level, &cb);
        }

        if (_file != "") {
            if (_line > 0) {
                sr.iterateByFileAndLine(_file, _line, &cb);
            } else {
                sr.iterateByFile(_file, &cb);
            }
        }

        return 0;
    }
};

MAIN_APP(dbgset);
