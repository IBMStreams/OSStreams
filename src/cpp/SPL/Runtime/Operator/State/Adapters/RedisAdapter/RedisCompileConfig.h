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
 * \file RedisCompileConfig.h \brief Compile-time Configuration
 */
#ifndef SPL_DSA_REDIS_COMPILE_CONFIG_H
#define SPL_DSA_REDIS_COMPILE_CONFIG_H

// event polling tmeout (in milliseconds)
#ifndef REDIS_EVENT_POLLING_TIMEOUT
#define REDIS_EVENT_POLLING_TIMEOUT 60 * 1000
#endif

#endif // SPL_DSA_REDIS_COMPILE_CONFIG_H
