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

// Class Description:
//
// Implement a simple scoped resource management class

#ifndef __RAII_FCT_H__
#define __RAII_FCT_H__

#include <UTILS/UTILSTypes.h>
#include <cassert>

UTILS_NAMESPACE_BEGIN

/***
    This class is used to implement resource management, implementing
    the "Resource Acquisition Is Initialization" paradym.

    It deals with resource deallocation and is critical when throwing
    exceptions. A resource can be a region of memory, a file, etc.

    This template class gives the ability to call an arbitrary
    function when a resource needs to be freed. Instance of this class
    must be allocated on the stack, so that resources are freed when
    the execution path leave the function or block where the resource
    was allocated. This is accomplish by calling the freeing function
    in the destructor of the RAII object.

    This class is called RAIIFct becuse it can only call a function to
    free the resource. It cannot call another destructor or free
    memory allocated with new.
 */

template<typename T>
class RAIIFct
{
  public:
    /// This is the type of the function to call to free the
    /// resource. It should be a function that takes only one parameter
    /// of the same time as the template argument T and no return value.
    typedef void (*fct_t)(T);
    /// This is the type of the function to call to free the
    /// resource. It should be a function that takes only one parameter
    /// of the same time as the template argument T and returns an
    /// integer, like an error code. The error code will be ignored,
    /// since there is no error handling possible in the destructor
    /// (throwing exceptions is forbidden).
    typedef int (*fct_err_t)(T);

  protected:
    /// The resource to free. It can be a file descriptor, a stucture,
    /// etc.
    T _val;
    /// Pointer to the function to call to free the resource _val for
    /// the case where the function does not have a return code, like
    /// free (NULL otherwise)
    fct_t _fct;
    /// Pointer to the function to call to free the resource _val for
    /// the case where the function has a return code like close (NULL
    /// otherwise)
    fct_err_t _fct_e;
    /// Turn off the resource management for this object. If set, the
    /// destructor will do nothing.
    bool _disabled;

  public:
    /// Constructor of the resource management object. The object should
    /// always be on the stack, since it's "being out of scope" that
    /// cause the destructor to be called and the resource to be
    /// freed. Use this constructor if the resource function returns no
    /// result (void), like free.
    /// @param val the resource to be freed.
    /// @param fct pointer to the function to call to free the resource
    RAIIFct(T val, fct_t fct)
      : _val(val)
      , _fct(fct)
      , _fct_e(NULL)
      , _disabled(false)
    {
        assert(_fct);
    }

    /// Constructor of the resource management object. The object should
    /// always be on the stack, since it's "being out of scope" that
    /// cause the destructor to be called and the resource to be
    /// freed. Use this function if the resource function return an
    /// integer, like close.
    /// @param val the resource to be freed
    /// @param fct pointer to the function to call to free the resource
    RAIIFct(T val, fct_err_t fct)
      : _val(val)
      , _fct(NULL)
      , _fct_e(fct)
      , _disabled(false)
    {
        assert(_fct_e);
    }

    /// Destructor of the resource management object. This is where the
    /// magic happens. When the object goes out of scope, because of an
    /// exception, a return/break or simply because the block of code
    /// reach its end, the function is called to free the resource.
    ~RAIIFct() { now(); }

    /// Turn off the resource managemenet for this object. The
    /// destructor will not perform any action once this is called.
    void disable() { _disabled = true; }

    /// Perform the resrouce management action, i.e. calling the
    /// function on the resource being tracked. This can be called early
    /// by the part that instanciated this object, or else it will be
    /// called by the destructor. If this is called early, it will not
    /// be called again in the destructor. If the resource action was
    /// disabled, this will have no effect.
    void now()
    {
        if (!_disabled) {
            _disabled = true;

            if (_fct) {
                _fct(_val);
            } else {
                if (_fct_e) {
                    _fct_e(_val);
                } else {
                    // what??? pesky gamma ray bit flippers!!
                }
            }
        }
    }
};

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
