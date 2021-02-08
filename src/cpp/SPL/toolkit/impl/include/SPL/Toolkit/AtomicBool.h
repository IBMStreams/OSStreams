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

#include <SPL/Runtime/Utility/Visibility.h>

#ifndef SPL_RUNTIME_ATOMIC_BOOL_H
#define SPL_RUNTIME_ATOMIC_BOOL_H

namespace SPL {

    // Provides atomicity and thread visibility.
    // (to be replaced with <cstdatomic> for g++ > 4.4.x)
    struct DLL_PUBLIC atomic_bool
    {
    private:
        bool value_;
        atomic_bool(const atomic_bool&) {} // = delete
        atomic_bool& operator=(const atomic_bool& other) { return *this; } // = delete
#if defined(__i386__) || defined(__x86_64__)
	static void write_memory_barrier() { asm volatile("sfence" ::: "memory"); }
        static void full_memory_barrier() { __sync_synchronize(); }
#elif defined(__PPC__) || defined(__PPC64__)
        static void write_memory_barrier() { asm volatile("lwsync" ::: "memory"); }
        static void full_memory_barrier() { __sync_synchronize(); }
#else
   #error "Unsupported architecture."
#endif

    public:
        atomic_bool() : value_(false) {}
        ~atomic_bool() {}

        atomic_bool(bool b) : value_(b) {}

        bool operator=(bool b) volatile
        {   store(b); return b; }

        operator bool() const volatile
        {   return load(); }

        void store(bool b) volatile
        {
            write_memory_barrier();
            value_ = b;
            full_memory_barrier();
        }

        bool load() const volatile
        {
            full_memory_barrier();
            bool ret = value_;
            full_memory_barrier();
            return ret;
        }
    };
}

#endif // SPL_RUNTIME_ATOMIC_BOOL_H
