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

/*
 * Implementation of the SPL::RedisSharding class.
 */

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisSharding.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

using namespace std;
using namespace SPL;

uint32_t RedisSharding::seed = 5381;

uint32_t RedisSharding::mapNameToServer(const std::string& name,
                                        const unsigned int& totalNumServers)
{
    assert(totalNumServers > 0);

    /// use Murmurhash2
    /// code borrowed from Redis

    uint32_t seedValue = seed;
    const uint32_t m = 0x5bd1e995;
    const int r = 24;
    uint32_t len = name.size();
    uint32_t h = seedValue ^ len;
    const unsigned char* data = (const unsigned char*)name.c_str();

    while (len >= 4) {
        // uint32_t k = *(uint32_t*)data;
        uint32_t k;
        memcpy(&k, data, sizeof(uint32_t)); // in case the machine requires aligned memory access

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    switch (len) {
        case 3:
            h ^= data[2] << 16;
        case 2:
            h ^= data[1] << 8;
        case 1:
            h ^= data[0];
            h *= m;
    };

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return ((uint32_t)h % totalNumServers);
}

void RedisSharding::setSeedValue(const uint32_t value)
{
    seed = value;
}
