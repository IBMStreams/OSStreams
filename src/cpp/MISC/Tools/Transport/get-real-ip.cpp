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

#include <UTILS/DistilleryApplication.h>
#include <UTILS/HostInfo.h>
#include <cstring>
#include <iostream>
#include <string>

using namespace std;

UTILS_NAMESPACE_USE

class get_real_ip : public DistilleryApplication
{
  public:
    bool _fqdn;
    bool _name;
    bool _ip;

    get_real_ip()
      : _fqdn(false)
      , _name(false)
      , _ip(true)
    {
        setKeepRemainingArguments();
    }

    void getArguments(option_vector_t& options)
    {
        option_t args[] = {
            { 'f', "fqdn", 0, "", "Fully Qualified Domain Name", INT_OPT(get_real_ip::setFQDN) },
            { 'n', "name", 0, "", "Return the hostname", INT_OPT(get_real_ip::setName) },
            { 'i', "ip", 0, "", "Return the IP Address", INT_OPT(get_real_ip::setIP) },
        };

        APPEND_OPTIONS(options, args);
    }

    void setFQDN(const option_t* option, int value) { _fqdn = true; }

    void setName(const option_t* option, int value)
    {
        _name = true;
        _ip = false;
    }

    void setIP(const option_t* option, int value)
    {
        _name = false;
        _ip = true;
    }

    virtual int run(const vector<string>& remainings_args)
    {
        if (remainings_args.size() != 1) {
            exit("Missing argument", 1);
        }

        HostInfo hi(remainings_args[0]);

        if (_ip) {
            cout << hi.getStringIPAddress() << endl;
            ;
        } else {
            if (_name) {
                if (_fqdn) {
                    cout << hi.getHostName() << endl;
                    ;
                } else {
                    cout << hi.getShortHostName() << endl;
                    ;
                }
            }
        }

        return 0;
    }
};

MAIN_APP(get_real_ip);
