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

#include <SPL/Runtime/ProcessingElement/StandaloneApplication.h>
#include <SPL/Runtime/ProcessingElement/StandaloneApplicationImpl.h>

#include <openssl/evp.h>
#include <openssl/ssl.h>

using namespace std;
using namespace SPL;

StandaloneApplication::StandaloneApplication()
  : impl_(new StandaloneApplicationImpl())
{}

StandaloneApplication::~StandaloneApplication() {}

Distillery::DistilleryApplication* StandaloneApplication::getImpl()
{
    return impl_.get();
}

Distillery::DistilleryApplication* StandaloneApplication::releaseImpl()
{
    return impl_.release();
}

int StandaloneApplication::main(int argc, char** argv, char** envp)
{
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_digests();
    return UTILS_NAMESPACE_QUALIFIER DistilleryApplication::main(argc, argv, envp);
}
