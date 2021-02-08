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

//----------------------------------------------------------------------------
#ifndef ATOMIC_H_INCLUDED
#define ATOMIC_H_INCLUDED

#include <UTILS/UTILSTypes.h>
#include <stdint.h>

#if defined(__i386__) ||                                                                           \
  defined(__x86_64__) // || defined(__ia64) || defined(__x86_64) || defined(__amd64)
#define ATOMIC_64_BIT_OPS_AVAILABLE 1
#define ATOMIC_32_BIT_OPS_AVAILABLE 1
#elif defined(__powerpc__)
#define ATOMIC_64_BIT_OPS_AVAILABLE 1
#define ATOMIC_32_BIT_OPS_AVAILABLE 1
#else
#undef ATOMIC_64_BIT_OPS_AVAILABLE
#undef ATOMIC_32_BIT_OPS_AVAILABLE
#endif

UTILS_NAMESPACE_BEGIN

//----------------------------------------------------------------------------
// Atomic typedefs
//----------------------------------------------------------------------------
typedef volatile int32_t atomic_int32_t;   // An atomically modified int32_t
typedef volatile int64_t atomic_int64_t;   // An atomically modified int64_t
typedef volatile uint32_t atomic_uint32_t; // An atomically modified uint32_t
typedef volatile uint64_t atomic_uint64_t; // An atomically modified uint64_t

//----------------------------------------------------------------------------
//
// Subroutine-
//       atomic_cs32
//
// Purpose-
//       Compare and swap.
//
/// This subroutine implements the atomic compare-and-swap operation.
/// The word is compared to the oldValue and if and only if the word is
/// equal to the oldValue is atomically replaced with the newValue,
/// otherwise the word is not changed.
/// The return value is 0 if and only if replacement occurred.
///
/// @param ptrValue Pointer to atomic updated word.
/// @param oldValue The value (*ptrValue) is expected to contain.
/// @param newValue The replacement value for (*ptrValue).
// Logic-
//       cc= <some non-zero value, usually ptrValue>
//       ATOMICALLY
//       {{{{
//         if( oldValue == *ptrValue )
//         {
//           *ptrValue= newValue;
//           cc= 0;
//         }
//       }}}}
//       return cc;
//
//----------------------------------------------------------------------------
/// This subroutine implements the atomic compare-and-swap operation.
/// The word is compared to the oldValue and if and only if the word is
/// equal to the oldValue is atomically replaced with the newValue,
/// otherwise the word is not changed.
/// The return value is 0 if and only if replacement occurred.
///
/// @param ptrValue Pointer to atomic updated word.
/// @param oldValue The value (*ptrValue) is expected to contain.
/// @param newValue The replacement value for (*ptrValue).
/// @return A condition code, zero if and only if the swap occurred.
inline int atomic_cs32( // -> atomic int32_t
  atomic_int32_t* ptrValue,
  // Expected old value
  int32_t oldValue,
  // Replacement new value
  int32_t newValue)
{
#if defined(__i386__)

    int cc;

    asm volatile("\n /* --- begin inline assemble code --- */\n"
                 "        lock cmpxchg %%ecx, (%%edi)\n" // COMPARE AND SWAP
                 "        jnz  0f\n"           // if( oldValue != *ptrValue ) skip next instruction
                 "        xorl %%edi, %%edi\n" // edi= 0
                 "0:\n"
                 "        movl %%edi, %0\n" // cc= edi
                 " /* --- end inline assemble code --- */\n"
                 : "=r"(cc)      // %0
                 : "D"(ptrValue) // %1 -- edi= ptrValue
                   ,
                   "c"(newValue) // %2 -- ecx= newValue
                   ,
                   "a"(oldValue) // %3 -- eax= oldValue
                 : "cc", "memory");

    return cc;

#elif defined(__powerpc__)

    // Taken from /usr/include/asm-ppc/system.h

    int prev;

    asm volatile("    sync            \n"
                 "1:  lwarx   %0,0,%2 \n"
                 "    cmpw    0,%0,%3 \n"
                 "    bne-    2f      \n"
                 "    stwcx.  %4,0,%2 \n"
                 "    bne-    1b      \n"
                 "    isync           \n"
                 "2:"
                 : "=&r"(prev), "=m"(*ptrValue)
                 : "r"(ptrValue), "r"(oldValue), "r"(newValue), "m"(*ptrValue)
                 : "cc", "memory");

    return (prev != oldValue);

#elif defined(__x86_64__)
    int32_t prev;

    asm volatile("lock; cmpxchgl %k1,%2"
                 : "=a"(prev)
                 : "r"(newValue), "m"(*ptrValue), "0"(oldValue)
                 : "memory");

    return prev != oldValue;

#else

#error Processor architecture not supported for atomic_cs32. A 32-bit compare-and-swap must be implemented for this architecture to proceed.

#endif
}

//----------------------------------------------------------------------------
//
// Subroutine-
//       atomic_cs64
//
// Purpose-
//       Compare and swap.
//
/// This subroutine implements the atomic compare-and-swap operation.
/// The word is compared to the oldValue and if and only if the word is
/// equal to the oldValue is atomically replaced with the newValue,
/// otherwise the word is not changed.
/// The return value is 0 if and only if replacement occurred.
///
/// @param ptrValue Pointer to atomic updated word.
/// @param oldValue The value (*ptrValue) is expected to contain.
/// @param newValue The replacement value for (*ptrValue).
// Logic-
//       cc= <some non-zero value, usually ptrValue>
//       ATOMICALLY
//       {{{{
//         if( oldValue == *ptrValue )
//         {
//           *ptrValue= newValue;
//           cc= 0;
//         }
//       }}}}
//       return cc;
//
//----------------------------------------------------------------------------
/// This subroutine implements the atomic compare-and-swap operation.
/// The word is compared to the oldValue and if and only if the word is
/// equal to the oldValue is atomically replaced with the newValue,
/// otherwise the word is not changed.
/// The return value is 0 if and only if replacement occurred.
///
/// @param ptrValue Pointer to atomic updated word.
/// @param oldValue The value (*ptrValue) is expected to contain.
/// @param newValue The replacement value for (*ptrValue).
/// @return A condition code, zero if and only if the swap occurred.
inline int atomic_cs64( // -> atomic int32_t
  atomic_int64_t* ptrValue,
  // Expected old value
  int64_t oldValue,
  // Replacement new value
  int64_t newValue)
{
#if defined(__i386__)

    int cc;

    uint32_t o_lo = (uint64_t)oldValue & 0xFFFFFFFF;
    uint32_t o_hi = (uint64_t)oldValue >> 32;
    uint32_t n_lo = (uint64_t)newValue & 0xFFFFFFFF;
    uint32_t n_hi = (uint64_t)newValue >> 32;

    // EBX must be saved for -fPIC

    asm volatile("\n /* --- begin inline assemble code --- */\n"
                 "        xchgl %%ebx, %%esi\n"     // Save EBX in ESI
                 "        lock cmpxchg8b (%%edi)\n" // COMPARE AND SWAP
                 "        jnz  0f\n"           // if( oldValue != *ptrValue ) skip next instruction
                 "        xorl %%edi, %%edi\n" // edi= 0
                 "0:\n"
                 "        movl %%edi, %0\n"     // cc= edi
                 "        xchgl %%esi, %%ebx\n" // Restore EBX from ESI
                 " /* --- end inline assemble code --- */\n"
                 : "=r"(cc)      // %0
                 : "D"(ptrValue) // edi= ptrValue
                   ,
                   "c"(n_hi) // ecx= newValue(hi)
                   ,
                   "S"(n_lo) // esi= newValue(lo) -- will be swapped with ebx
                   ,
                   "d"(o_hi) // edx= oldValue(hi)
                   ,
                   "a"(o_lo) // eax= oldValue(lo)
                 : "cc", "memory");

    return cc;

#elif defined(__powerpc__)
    int64_t prev;

    asm volatile("    lwsync          \n"
                 "1:  ldarx   %0,0,%2 \n"
                 "    cmpd    0,%0,%3 \n"
                 "    bne-    2f      \n"
                 "    stdcx.  %4,0,%2 \n"
                 "    bne-    1b      \n"
                 "    isync           \n"
                 "2:"
                 : "=&r"(prev), "=m"(*ptrValue)
                 : "r"(ptrValue), "r"(oldValue), "r"(newValue), "m"(*ptrValue)
                 : "cc", "memory");

    return (prev != oldValue);

#elif defined(__x86_64__)
    int64_t prev;

    asm volatile("lock; cmpxchgq %1,%2"
                 : "=a"(prev)
                 : "r"(newValue), "m"(*ptrValue), "0"(oldValue)
                 : "memory");

    return prev != oldValue;

#else

#error Processor architecture not supported for atomic_cs64. A 64-bit compare-and-swap must be implemented for this architecture to proceed.

#endif
}

//----------------------------------------------------------------------------
//
// Subroutine-
//       atomic_add32
//
// Purpose-
//       Atomically add to a value
//
//----------------------------------------------------------------------------
/// Atomically add to a value.
///
/// @param ptrValue Pointer to atomically updated word.
/// @param addend The amount to be added.
/// @return The resultant, after the addition.
inline int32_t atomic_add32( // The word to increment
  atomic_int32_t* ptrValue,
  // The addend
  int addend)
{
    int cc;                     // Condition code
    int32_t oldValue, newValue; // Old and new word values

    do {
        oldValue = *ptrValue;                           // Old word value
        newValue = oldValue + addend;                   // New word value
        cc = atomic_cs32(ptrValue, oldValue, newValue); // Increment the value
    } while (cc != 0);

    return newValue;
}

//----------------------------------------------------------------------------
//
// Subroutine-
//       atomic_add64
//
// Purpose-
//       Atomically add to a value
//
//----------------------------------------------------------------------------
/// Atomically add to a value.
///
/// @param ptrValue Pointer to atomically updated word.
/// @param addend The amount to be added.
/// @return The resultant, after the addition.
inline int64_t atomic_add64( // The word to increment
  atomic_int64_t* ptrValue,
  // The addend
  int addend)
{
    int cc;                     // Condition code
    int64_t oldValue, newValue; // Old and new word values

    do {
        oldValue = *ptrValue;                           // Old word value
        newValue = oldValue + addend;                   // New word value
        cc = atomic_cs64(ptrValue, oldValue, newValue); // Increment the value
    } while (cc != 0);

    return newValue;
}

inline uint64_t atomic_addu64(atomic_uint64_t* ptrValue, int addend)
{
    return static_cast<int64_t>(atomic_add64(reinterpret_cast<atomic_int64_t*>(ptrValue), addend));
}

//----------------------------------------------------------------------------
//
// Subroutine-
//       atomic_sub32
//
// Purpose-
//       Atomically subtract from a value
//
//----------------------------------------------------------------------------
/// Atomically subtract from a value.
///
/// @param ptrValue Pointer to atomically updated word.
/// @param subtrahend The amount to be subtracted.
/// @return The resultant, after the subtraction.
inline int32_t atomic_sub32( // The word to modify
  atomic_int32_t* ptrValue,
  // The subtrahend
  int subtrahend)
{
    return atomic_add32(ptrValue, -subtrahend);
}

//----------------------------------------------------------------------------
//
// Subroutine-
//       atomic_sub64
//
// Purpose-
//       Atomically subtract from a value
//
//----------------------------------------------------------------------------
/// Atomically subtract from a value.
///
/// @param ptrValue Pointer to atomically updated word.
/// @param subtrahend The amount to be subtracted.
/// @return The resultant, after the subtraction.
inline int64_t atomic_sub64( // The word to modify
  atomic_int64_t* ptrValue,
  // The subtrahend
  int subtrahend)
{
    return atomic_add64(ptrValue, -subtrahend);
}

//----------------------------------------------------------------------------
//
// Subroutine-
//       atomic_dec32
//
// Purpose-
//       Atomically decrement a value
//
//----------------------------------------------------------------------------
/// Atomically decrement a value.
///
/// @param ptrValue Pointer to atomically updated word.
/// @return The resultant, after the decrement.
inline int32_t atomic_dec32( // The word to decrement
  atomic_int32_t* ptrValue)
{
    int cc;                     // Condition code
    int32_t oldValue, newValue; // Old and new word values

    do {
        oldValue = *ptrValue;                           // Old word value
        newValue = oldValue - 1;                        // New word value
        cc = atomic_cs32(ptrValue, oldValue, newValue); // Decrement the value
    } while (cc != 0);

    return newValue;
}

//----------------------------------------------------------------------------
//
// Subroutine-
//       atomic_dec64
//
// Purpose-
//       Atomically decrement a value
//
//----------------------------------------------------------------------------
/// Atomically decrement a value.
///
/// @param ptrValue Pointer to atomically updated word.
/// @return The resultant, after the decrement.
inline int64_t atomic_dec64( // The word to decrement
  atomic_int64_t* ptrValue)
{
    int cc;                     // Condition code
    int64_t oldValue, newValue; // Old and new word values

    do {
        oldValue = *ptrValue;                           // Old word value
        newValue = oldValue - 1;                        // New word value
        cc = atomic_cs64(ptrValue, oldValue, newValue); // Decrement the value
    } while (cc != 0);

    return newValue;
}

//----------------------------------------------------------------------------
//
// Subroutine-
//       atomic_inc32
//
// Purpose-
//       Atomically increment a value
//
//----------------------------------------------------------------------------
/// Atomically increment a value.
///
/// @param ptrValue Pointer to atomically updated word.
/// @return The resultant, after the increment.
inline int32_t atomic_inc32( // The word to increment
  atomic_int32_t* ptrValue)
{
    int cc;                     // Condition code
    int32_t oldValue, newValue; // Old and new word values

    do {
        oldValue = *ptrValue;                           // Old word value
        newValue = oldValue + 1;                        // New word value
        cc = atomic_cs32(ptrValue, oldValue, newValue); // Increment the value
    } while (cc != 0);

    return newValue;
}

//----------------------------------------------------------------------------
//
// Subroutine-
//       atomic_inc64
//
// Purpose-
//       Atomically increment a value
//
//----------------------------------------------------------------------------
/// Atomically increment a value.
///
/// @param ptrValue Pointer to atomically updated word.
/// @return The resultant, after the increment.
inline int64_t atomic_inc64( // The word to increment
  atomic_int64_t* ptrValue)
{
    int cc;                     // Condition code
    int64_t oldValue, newValue; // Old and new word values

    do {
        oldValue = *ptrValue;                           // Old word value
        newValue = oldValue + 1;                        // New word value
        cc = atomic_cs64(ptrValue, oldValue, newValue); // Increment the value
    } while (cc != 0);

    return newValue;
}

inline uint64_t atomic_incu64(atomic_uint64_t* ptrValue)
{
    return static_cast<int64_t>(atomic_inc64(reinterpret_cast<atomic_int64_t*>(ptrValue)));
}

//----------------------------------------------------------------------------
//
// Subroutine-
//       atomic_set
//
// Purpose-
//       Atomically set to a value
//
//----------------------------------------------------------------------------
/// Atomically set to a value.
///
/// @param ptrValue Pointer to atomically updated word.
/// @param newval Value to set
/// @return The resultant, after the set.
inline void atomic_set32( // The word to increment
  atomic_int32_t* ptrValue,
  // The value to set
  int32_t newval)
{
    int cc;         // Condition code
    int32_t oldval; // Old and new word values

    do {
        oldval = *ptrValue;                         // Old word value
        cc = atomic_cs32(ptrValue, oldval, newval); // Increment the value
    } while (cc != 0);
}

//----------------------------------------------------------------------------
//
// Subroutine-
//       atomic_set
//
// Purpose-
//       Atomically set to a value
//
//----------------------------------------------------------------------------
/// Atomically set to a value.
///
/// @param ptrValue Pointer to atomically updated word.
/// @param newval Value to set
/// @return The resultant, after the set.
inline void atomic_set64( // The word to increment
  atomic_int64_t* ptrValue,
  // The value to set
  int64_t newval)
{
    int cc;         // Condition code
    int64_t oldval; // Old and new word values

    do {
        oldval = *ptrValue;                         // Old word value
        cc = atomic_cs64(ptrValue, oldval, newval); // Increment the value
    } while (cc != 0);
}

// Memory barrier definitions
#ifndef compiler_memory_barrier_
#define compiler_memory_barrier_() asm volatile("" ::: "memory")
#endif

#if defined(__i386__) || defined(__x86_64__)
#ifndef read_memory_barrier_
#define read_memory_barrier_() asm volatile("lfence" ::: "memory")
#endif
#ifndef write_memory_barrier_
#define write_memory_barrier_() asm volatile("sfence" ::: "memory")
#endif
#ifndef full_memory_barrier_
#define full_memory_barrier_() __sync_synchronize();
#endif
#elif defined(__PPC__) || defined(__PPC64__)
#ifndef read_memory_barrier_
#define read_memory_barrier_() asm volatile("lwsync" ::: "memory")
#endif
#ifndef write_memory_barrier_
#define write_memory_barrier_() asm volatile("lwsync" ::: "memory")
#endif
#ifndef full_memory_barrier_
#define full_memory_barrier_() __sync_synchronize();
#endif
#else
#error "Unsupported architecture."
#endif

// Provides atomicity and thread visibility.
// (to be replaced with <cstdatomic> for g++ > 4.4.x)
struct atomic_bool
{
  private:
    bool value_;
    atomic_bool(const atomic_bool&) {}
    atomic_bool& operator=(const atomic_bool& other) { return *this; }

  public:
    atomic_bool()
      : value_(false)
    {}
    ~atomic_bool() {}

    atomic_bool(bool b)
      : value_(b)
    {}

    bool operator=(bool b) volatile
    {
        store(b);
        return b;
    }

    operator bool() const volatile { return load(); }

    void store(bool b) volatile
    {
        write_memory_barrier_();
        value_ = b;
        full_memory_barrier_();
    }

    bool load() const volatile
    {
        full_memory_barrier_();
        bool ret = value_;
        full_memory_barrier_();
        return ret;
    }
};

/**
 * This implementation provides atomicity and thread visibility
 * (unlike the volatile keyword).
 * TODO Use <cstdatomic> for g++ versions > 4.4.x.
 */
struct atomic_int
{
  private:
    int value_;
    atomic_int(const atomic_int&) {}
    atomic_int& operator=(const atomic_int& other) { return *this; }

  public:
    atomic_int()
      : value_(0)
    {}
    ~atomic_int() {}

    atomic_int(int v)
      : value_(v)
    {}

    int operator=(int v) volatile
    {
        store(v);
        return v;
    }

    operator int() const volatile { return load(); }

    void store(int v) volatile
    {
        write_memory_barrier_();
        value_ = v;
        full_memory_barrier_();
    }

    int load() const volatile
    {
        full_memory_barrier_();
        int ret = value_;
        full_memory_barrier_();
        return ret;
    }
};

UTILS_NAMESPACE_END

#endif // ATOMIC_H_INCLUDED

// Local Variables: ***
// mode:c++ ***
// End: ***
