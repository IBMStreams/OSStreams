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

#ifndef _SECURITY_H_
#define _SECURITY_H_

#include <UTILS/UTILSTypes.h>
#include <openssl/ssl.h>
#include <string>

UTILS_NAMESPACE_BEGIN

class Security
{
  public:
    enum Type
    {
        None,
        SSLv3,
        TLSv1,
        TLSv1_1,
        TLSv1_2
    };

    static Security::Type parseType(std::string const& value);

    static std::string getError();

    static std::string getError(SSL* ssl, int code);
};

UTILS_NAMESPACE_END

#endif
