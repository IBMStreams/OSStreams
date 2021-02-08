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

#ifndef SPL_RUNTIME_WINDOW_SERIALIZATION_H_
#define SPL_RUNTIME_WINDOW_SERIALIZATION_H_

#ifdef NDEBUG
#define VERIFY_MARKER(stream, data)                                                                \
    {                                                                                              \
        void(0);                                                                                   \
    }
#define VERIFY_HASH(stream)                                                                        \
    {                                                                                              \
        void(0);                                                                                   \
    }
#else
#define VERIFY_MARKER(stream, data)                                                                \
    {                                                                                              \
        std::string mark__;                                                                        \
        stream >> mark__;                                                                          \
        assert(mark__ == std::string(data));                                                       \
    }

#define VERIFY_HASH(stream)                                                                        \
    {                                                                                              \
        std::size_t h__;                                                                           \
        stream >> h__;                                                                             \
        assert(h__ == hash());                                                                     \
    }
#endif

#ifdef NDEBUG
#define WRITE_MARKER(stream, data)                                                                 \
    {                                                                                              \
        void(0);                                                                                   \
    }
#define WRITE_HASH(sream)                                                                          \
    {                                                                                              \
        void(0);                                                                                   \
    }
#else
#define WRITE_MARKER(stream, data)                                                                 \
    {                                                                                              \
        stream << std::string(data);                                                               \
    }
#define WRITE_HASH(stream)                                                                         \
    {                                                                                              \
        stream << hash();                                                                          \
    }
#endif

#endif /* SPL_RUNTIME_WINDOW_SERIALIZATION_H_ */
