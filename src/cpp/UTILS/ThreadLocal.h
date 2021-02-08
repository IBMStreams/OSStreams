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

#ifndef THREADLOCAL_H_
#define THREADLOCAL_H_

#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/SupportFunctions.h>
#include <UTILS/UTILSTypes.h>

#include <pthread.h>

/*
 * This macro defines a function used to initialize only once a ThreadLocal
 * variable's key.  Note that in pthread_key_create() we're not setting a key
 * value destructor - the client is responsible for destructing
 * thread-specific values.
 *
 * fun  - name of a static function which is executed only once to initialize a
 *        ThreadLocal variable's key
 * pVar - pointer to the ThreadLocal variable
 */
#define THREAD_LOCAL_INIT(fun, pVar)                                                               \
    static void fun()                                                                              \
    {                                                                                              \
        std::cout << "Enter " << __PRETTY_FUNCTION__ << std::endl;                                 \
        int rc = pthread_key_create(&((pVar)->key_), NULL);                                        \
        if (rc != 0) {                                                                             \
            /* Throw simple exception and log to cerr.  This function may be */                    \
            /* called before logging is up.  */                                                    \
            std::cerr << __PRETTY_FUNCTION__ << " failed with error code " << rc << std::endl;     \
            std::exception ex;                                                                     \
            throw ex;                                                                              \
        }                                                                                          \
        std::cout << "Exit " << __PRETTY_FUNCTION__ << std::endl;                                  \
    }

UTILS_NAMESPACE_BEGIN

/**
 * This class provides thread-specific variables. These variables differ from
 * their normal counterparts in that each thread that accesses one (via its
 * get or set method) has its own, independently initialized copy of the
 * variable.
 *
 * ThreadLocal instances are static members in classes that wish to associate
 * state with a thread.
 *
 * Each ThreadLocal instance uses a map to store per-thread values,
 * where the map key is a combination of this instance's key_ member and the
 * thread id.  The client must make sure the key is initialized before the
 * set and get methods are invoked.  Moreover, initialization and get/set
 * invocation should happen in the same process (not sure if we can
 * initialize a ThreadLocal static member in the parent and use it after
 * fork() in the child).
 *
 * The eager initialization constructor is simpler to use and works fine at
 * most times.  The lazy initialization happens at the first invocation of
 * get, set, remove, and it requires the client to define both a static
 * ThreadLocal variable and a static method for the key initialization and
 * pass the method to the ThreadLocal constructor.
 */
template<typename T>
class ThreadLocal
{
  public:
    /**
     * The thread-specific key associated with type T.  It has public access
     * in order to be accessible to the init_once method (see constructor).
     */
    pthread_key_t key_;

    /**
     * Creates a thread-specific variable.  Use this constructor with a static
     * function which creates the variable's thread-specific key.
     * Usage example:
     * @code
     *   class A {
     *     ...
     *     static ThreadLocalString tls_;
     *   };
     *   ...
     *   // Define static initialization function and variable.
     *   THREAD_LOCAL_INIT(tls_init, &A::tls_);
     *   ThreadLocalString A::tls_(tls_init);
     * @endcode
     *
     * Use this initialization method when it cannot be guaranteed that key
     * creation is done only once, or that it is done in the same process where
     * the variable is used.
     *
     * @param init_once external static method which is guaranteed to be
     * invoked only once when called from multiple threads.  This method is
     * called only once upon the first invocation of get(), set(), or remove().
     */
    ThreadLocal(void (*init_once)(void));

    /**
     * Creates a thread-specific variable and immediately initializes its
     * thread-specific key.
     *
     * This constructor creates the thread-specific key visible to all
     * threads by calling pthread_key_create().  Usage:
     * @code
     *   class A {
     *     ...
     *     static ThreadLocalInt tli;
     *   };
     *   ...
     *   ThreadLocalInt A::tli;  //
     * @endcode
     *
     * Use this constructor when you want to achieve eager thread-specific key
     * initialization.
     */
    ThreadLocal();

    virtual ~ThreadLocal() {}

    /**
     * Returns the value in the current thread's copy of this thread-specific
     * variable.  If the variable has no value for the current thread, it is
     * first initialized to the value returned by an invocation of the
     * initialValue() method.
     */
    T* const get();

    /**
     * Sets the current thread's copy of this thread-local variable to the
     * specified value. Most subclasses will have no need to override this
     * method, relying solely on implementing the initialValue() method to
     * set the values of thread-specific variables.
     */
    void set(const T* value);

    /**
     * Removes the current thread's value for this thread-specific variable.
     * If this variable is subsequently read by the current thread, its
     * value will be reinitialized by invoking its initialValue() method,
     * unless its value is set by the current thread in the interim. This may
     * result in multiple invocations of the initialValue method in the
     * current thread.
     */
    void remove();

  protected:
    /**
     * Returns the current thread's initial value for this thread-specific
     * variable. This method will be invoked the first time a thread accesses
     * the variable with the get() method, unless the thread previously
     * invoked the set(T) method, in which case the initialValue method will
     * not be invoked for the thread.  Normally, this method is invoked at
     * most once per thread, but it may be invoked again in case of
     * subsequent invocations of remove() followed by get().
     *
     * This implementation throws an exception; override for a different
     * behavior.
     */
    virtual T* const initialValue();

  private:
    // non-copyable
    ThreadLocal(const ThreadLocal&);
    const ThreadLocal& operator=(const ThreadLocal&);

    /*
     * Variant of set() to establish initialValue.  Used internally to allow
     * the user to override set().
     *
     * @return the initial value
     */
    T* const setInitialValue();

    void internalSet(const void* value);

    pthread_once_t key_once_;
    void (*init_once_)(void);
};

/////////////////////////////////////////////////////////////////////////////

template<typename T>
ThreadLocal<T>::ThreadLocal(void (*init_once)(void))
  : key_(0)
  , key_once_(PTHREAD_ONCE_INIT)
  , init_once_(init_once)
{
    //    (void) pthread_once(&key_once_, init_once);
}

template<typename T>
ThreadLocal<T>::ThreadLocal()
  : key_(0)
  , key_once_(PTHREAD_ONCE_INIT)
  , init_once_(NULL)
{
    int rc = pthread_key_create(&key_, NULL);
    if (rc != 0) {
        // Throw simple exception and log to cerr.  Constructor is called
        // to initialize static variables before logging is up.
        std::cerr << __PRETTY_FUNCTION__ << " failed with error code " << rc << std::endl;
        std::exception ex;
        throw ex;
    }
}

template<typename T>
T* const ThreadLocal<T>::get()
{
    SPCDBG(L_TRACE, "Enter " << __PRETTY_FUNCTION__, DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);
    if (init_once_ != NULL) {
        (void)pthread_once(&key_once_, init_once_);
    }
    void* value;
    if ((value = pthread_getspecific(key_)) == NULL) {
        return setInitialValue();
    }
    return static_cast<T* const>(value);
}

template<typename T>
void ThreadLocal<T>::set(const T* value)
{
    SPCDBG(L_TRACE, "Enter " << __PRETTY_FUNCTION__ << "(" << value << ")",
           DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);
    ThreadLocal::internalSet(static_cast<const void*>(value));
    SPCDBG(L_TRACE,
           __PRETTY_FUNCTION__ << " has successfully set thread-specific value "
                               << static_cast<const void*>(value),
           DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);
}

template<typename T>
void ThreadLocal<T>::remove()
{
    SPCDBG(L_TRACE, "Enter " << __PRETTY_FUNCTION__, DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);
    ThreadLocal::internalSet(NULL);
    SPCDBG(L_TRACE, __PRETTY_FUNCTION__ << " has successfully removed the thread-specific value",
           DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);
}

template<typename T>
void ThreadLocal<T>::internalSet(const void* value)
{
    if (init_once_ != NULL) {
        (void)pthread_once(&key_once_, init_once_);
    }
    int rc = pthread_setspecific(key_, value);
    ERRNO_EX(rc != 0, Distillery,
             "failed to associate value " << value << " with thread-specific key " << key_);
}

template<typename T>
inline T* const ThreadLocal<T>::initialValue()
{
    return NULL;
}

template<typename T>
inline T* const ThreadLocal<T>::setInitialValue()
{
    T* const value = initialValue();
    ThreadLocal::internalSet(static_cast<const void*>(value));
    return value;
}

UTILS_NAMESPACE_END

#endif /* THREADLOCAL_H_ */
