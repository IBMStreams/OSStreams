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

#include "dst-config.h"
#include <UTILS/DistilleryException.h>
#include <UTILS/Product.h>
#include <stdlib.h>

using namespace std;
UTILS_NAMESPACE_USE

// TODO - integrate with install created .product props file?
static const string NAME = "Streams";
#undef VERSION
static const string VERSION = STREAMS_VERSION;

const string& Product::getName()
{
    return NAME;
}

const string& Product::getVersion()
{
    return VERSION;
}

const string Product::getInstallDir()
{
    char* dir = NULL;
    // TODO: dir = getenv("STREAMS_INSTALL");
    if (dir == NULL) {
        dir = getenv("STREAMS_INSTALL");
    }

    if (dir != NULL) {
        return dir;
    }

    THROW(Distillery, "STREAMS_INSTALL environment variable not set");
}
