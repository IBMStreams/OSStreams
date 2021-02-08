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
#include <sys/types.h>
#include <unistd.h>

#include <TRC/DistilleryDebug.h>
#include <UTILS/DistilleryException.h>
#include <iostream>
#include <sstream>

#include <UTILS/DistilleryApplication.h>
#include <UTILS/Socket.h>

using namespace std;
UTILS_NAMESPACE_USE;

class regex : public DistilleryApplication
{

  public:
    regex(void) { setKeepRemainingArguments(); }

    virtual int run(const vector<string>& remainings_args)
    {
        switch (remainings_args[0][0]) {
            case 'S': {
                // server: S <port>

                InetSocket sock(AF_INET, SOCK_DGRAM);

                sock.bind(atoi(remainings_args[1].c_str()));

                cout << "Listening on port " << sock.getPort() << endl;

                int fd = *sock;

                while (1) {
                    char message[64 * 1024];
                    /*int size =*/read(fd, message, 64 * 1024);
                    // cout << "message: " /*<< message*/ << " [" << size << "]" << endl ;
                }
            } break;

            case 'C': {
                // client: C <address> <port> <size>

                InetSocket sock(AF_INET, SOCK_DGRAM);

                sock.connect(remainings_args[1].c_str(), atoi(remainings_args[2].c_str()));

                cout << "Local port " << sock.getPort() << endl;

                char message[64 * 1024];
                int fd = *sock;
                int bytes = atoi(remainings_args[3].c_str());

                for (unsigned int i = 0; i < 100; i++) {
                    write(fd, message, bytes);
                    usleep(100 * 1000); // 100ms
                }
            } break;

            case 'M': {
                // server: M <port> <address> <next port>

                InetSocket ssock(AF_INET, SOCK_DGRAM);

                ssock.bind(atoi(remainings_args[1].c_str()));

                cout << "Listening on port " << ssock.getPort() << endl;

                InetSocket csock(AF_INET, SOCK_DGRAM);

                csock.connect(remainings_args[2].c_str(), atoi(remainings_args[3].c_str()));

                cout << "Local port " << csock.getPort() << endl;

                int sfd = *ssock;
                int cfd = *csock;

                while (1) {
                    char message[64 * 1024];
                    int size = read(sfd, message, 64 * 1024);

                    // cout << "message: " /*<< message*/ << " [" << size << "]" << endl ;

                    write(cfd, message, size);
                }
            } break;

            default:
                abort();
        }

        return 0;
    }
};

MAIN_APP(regex);
