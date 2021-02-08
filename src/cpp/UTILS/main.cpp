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

#if (defined(DST_DBG_INST) || defined(DUMP_LINE_NUMBER))
#ifdef __i386__
#include <bfd.h>
#endif
#endif

#include <iostream>
#include <openssl/evp.h>
#include <openssl/ssl.h>
#include <xercesc/util/PlatformUtils.hpp>

#include <TRC/ConsoleTracer.h>
#include <TRC/DistilleryDebug.h>
#include <UTILS/DistilleryApplication.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/HostInfo.h>
#include <UTILS/SupportFunctions.h>

using namespace std;
XERCES_CPP_NAMESPACE_USE
UTILS_NAMESPACE_USE

extern DistilleryApplication* instantiate_new_app(void);

int main(int argc, char** argv, char** envp)
{
    arg_vector_t remains;
    int rc = 1;

    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_digests();

    set_new_handler(DistAppNoMoreMemoryHandler);

    try {
        XMLPlatformUtils::Initialize();
    } catch (const XMLException& ex) {
        cerr << "Error during Xerces-c Initialization: " << ex.getMessage() << "["
             << ex.getSrcFile() << ":" << ex.getSrcLine() << "]" << endl;
        return 1;
    }

    srand(time(0) + getpid());

#if (defined(DST_DBG_INST) || defined(DUMP_LINE_NUMBER))
#ifdef __i386__
    bfd_init();
#endif
#endif

    debug::cmdline = argv[0];
    debug::fadd_dynamic_aspect("CMD.%s", argv[0]);

    string cmd_with_path = debug::cmdline.substr(0, debug::cmdline.find_first_of(' '));
    string::size_type slash = cmd_with_path.find_last_of('/');

    if (slash == debug::cmd.npos) {
        slash = 0;
    } else {
        slash++;
    }
    debug::cmd = cmd_with_path.substr(slash);

    char host[1024];
    try {
        strcpy(host, HostInfo::getMyHostName().c_str());
        debug::hostname = host;
    } catch (HostInfoException& e) {
        debug::hostname = "-";
    }

    for (int i = 1; i < argc; i++) {
        debug::cmdline += ' ';
        debug::cmdline += argv[i];
    }

    // Set locale from default enviornment
    // setlocale is used here instead of std::locale() because locale has a side effect that causes
    // stringstream (at a minimum) to format integers with digit groupings enabled. eg. 1234 is
    // output as 1,234
    char* localename = setlocale(LC_ALL, "");

    // localename is null if setlocale fails for some reason
    if (NULL == localename) {
        cerr << "setlocale failed" << endl;
        rc = 130;
    } else {

        DistilleryApplication::app = instantiate_new_app();
        DistilleryApplication* app = DistilleryApplication::app;

        app->saveEnvAndArgs(argc, argv, envp);
        app->setARGV0(argv[0]);
        app->installExitSignalHandlers();

        if (app->init()) {
            cerr << "Init failed during startup" << endl;
            rc = 124;
        } else {
            try {
                app->processOptions(argc, argv, remains);
                // If there is no tracer setting from startup options, such as using
                // --Xdebug-file or any other debug setting options, we will use default
                // tracer -- console tracer.
                if (!app->isTracerSwitched()) {
                    app->switchTracer(new debug::ConsoleTracer);
                }

                app->installFaultSignalHandlers();
                rc = app->runSafeCatchall(remains);
            } catch (InvalidOptionException& ex) {
                if (ex.getExplanation().length()) {
                    app->error(ex.getExplanation()) << endl << endl;
                }
                app->printUsage(cerr, false);
                cerr << endl;

                rc = 1;
            }
        }
    }

    DistAppTerminate(rc);
}
