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

#ifndef _DISTILLERYCALLBACK_H_
#define _DISTILLERYCALLBACK_H_

#include <UTILS/UTILSTypes.h>

UTILS_NAMESPACE_BEGIN

// num 0
// tmpl_arg_long void
// tmpl_arg_short void
// tmpl_call_long void
// tmpl_call_short void

class Function_0
{

  public:
    virtual ~Function_0() {}

    virtual void call(void) = 0;
    virtual Function_0* dup(void) const = 0;
};

template<class C>
class MemberFunction_0 : public Function_0
{

  public:
    typedef void (C::*fct_ptr_t)(void);

  private:
    C* _obj_ptr;
    fct_ptr_t _fct_ptr;

  public:
    MemberFunction_0(C* obj_ptr, fct_ptr_t fct_ptr)
      : _obj_ptr(obj_ptr)
      , _fct_ptr(fct_ptr)
    {}

    virtual ~MemberFunction_0() {}

    virtual void call(void) { (_obj_ptr->*(this->_fct_ptr))(); }

    virtual MemberFunction_0<C>* dup(void) const
    {
        return new MemberFunction_0(_obj_ptr, _fct_ptr);
    }
};

class Callback_0
{
  protected:
    Function_0* _func;

  public:
    Callback_0()
      : _func(NULL)
    {}

    Callback_0(const Callback_0& cb)
      : _func(cb._func ? cb._func->dup() : NULL)
    {}

    Callback_0(const Function_0& func)
      : _func(func.dup())
    {}

    virtual ~Callback_0() { delete _func; }

    void operator=(const Callback_0& cb)
    {
        delete _func;

        _func = (cb._func ? cb._func->dup() : NULL);
    }

    void operator=(const Function_0& func)
    {
        delete _func;

        _func = func.dup();
    }

    virtual void call(void)
    {
        if (_func) {
            _func->call();
        }
    }

    void operator()(void) { call(); }

    bool isSet(void) const { return _func != NULL; }
};

template<class C>
inline MemberFunction_0<C> mem_cb(C* ptr, void (C::*func)(void))
{
    return MemberFunction_0<C>(ptr, func);
}

// num 1
// tmpl_arg_long typename Arg1
// tmpl_arg_short Arg1
// tmpl_call_long Arg1 arg1
// tmpl_call_short arg1

template<typename Arg1>
class Function_1
{

  public:
    virtual ~Function_1() {}

    virtual void call(Arg1 arg1) = 0;
    virtual Function_1<Arg1>* dup(void) const = 0;
};

template<class C, typename Arg1>
class MemberFunction_1 : public Function_1<Arg1>
{

  public:
    typedef void (C::*fct_ptr_t)(Arg1);

  private:
    C* _obj_ptr;
    fct_ptr_t _fct_ptr;

  public:
    MemberFunction_1(C* obj_ptr, fct_ptr_t fct_ptr)
      : _obj_ptr(obj_ptr)
      , _fct_ptr(fct_ptr)
    {}

    virtual ~MemberFunction_1() {}

    virtual void call(Arg1 arg1) { (_obj_ptr->*(this->_fct_ptr))(arg1); }

    virtual MemberFunction_1<C, Arg1>* dup(void) const
    {
        return new MemberFunction_1(_obj_ptr, _fct_ptr);
    }
};

template<typename Arg1>
class Callback_1
{
  protected:
    Function_1<Arg1>* _func;

  public:
    Callback_1()
      : _func(NULL)
    {}

    Callback_1(const Callback_1<Arg1>& cb)
      : _func(cb._func ? cb._func->dup() : NULL)
    {}

    Callback_1(const Function_1<Arg1>& func)
      : _func(func.dup())
    {}

    virtual ~Callback_1() { delete _func; }

    void operator=(const Callback_1<Arg1>& cb)
    {
        delete _func;

        _func = (cb._func ? cb._func->dup() : NULL);
    }

    void operator=(const Function_1<Arg1>& func)
    {
        delete _func;

        _func = func.dup();
    }

    virtual void call(Arg1 arg1)
    {
        if (_func) {
            _func->call(arg1);
        }
    }

    void operator()(Arg1 arg1) { call(arg1); }

    bool isSet(void) const { return _func != NULL; }
};

template<class C, typename Arg1>
inline MemberFunction_1<C, Arg1> mem_cb(C* ptr, void (C::*func)(Arg1))
{
    return MemberFunction_1<C, Arg1>(ptr, func);
}

// num 2
// tmpl_arg_long typename Arg1, typename Arg2
// tmpl_arg_short Arg1, Arg2
// tmpl_call_long Arg1 arg1, Arg2 arg2
// tmpl_call_short arg1, arg2

template<typename Arg1, typename Arg2>
class Function_2
{

  public:
    virtual ~Function_2() {}

    virtual void call(Arg1 arg1, Arg2 arg2) = 0;
    virtual Function_2<Arg1, Arg2>* dup(void) const = 0;
};

template<class C, typename Arg1, typename Arg2>
class MemberFunction_2 : public Function_2<Arg1, Arg2>
{

  public:
    typedef void (C::*fct_ptr_t)(Arg1, Arg2);

  private:
    C* _obj_ptr;
    fct_ptr_t _fct_ptr;

  public:
    MemberFunction_2(C* obj_ptr, fct_ptr_t fct_ptr)
      : _obj_ptr(obj_ptr)
      , _fct_ptr(fct_ptr)
    {}

    virtual ~MemberFunction_2() {}

    virtual void call(Arg1 arg1, Arg2 arg2) { (_obj_ptr->*(this->_fct_ptr))(arg1, arg2); }

    virtual MemberFunction_2<C, Arg1, Arg2>* dup(void) const
    {
        return new MemberFunction_2(_obj_ptr, _fct_ptr);
    }
};

template<typename Arg1, typename Arg2>
class Callback_2
{
  protected:
    Function_2<Arg1, Arg2>* _func;

  public:
    Callback_2()
      : _func(NULL)
    {}

    Callback_2(const Callback_2<Arg1, Arg2>& cb)
      : _func(cb._func ? cb._func->dup() : NULL)
    {}

    Callback_2(const Function_2<Arg1, Arg2>& func)
      : _func(func.dup())
    {}

    virtual ~Callback_2() { delete _func; }

    void operator=(const Callback_2<Arg1, Arg2>& cb)
    {
        delete _func;

        _func = (cb._func ? cb._func->dup() : NULL);
    }

    void operator=(const Function_2<Arg1, Arg2>& func)
    {
        delete _func;

        _func = func.dup();
    }

    virtual void call(Arg1 arg1, Arg2 arg2)
    {
        if (_func) {
            _func->call(arg1, arg2);
        }
    }

    void operator()(Arg1 arg1, Arg2 arg2) { call(arg1, arg2); }

    bool isSet(void) const { return _func != NULL; }
};

template<class C, typename Arg1, typename Arg2>
inline MemberFunction_2<C, Arg1, Arg2> mem_cb(C* ptr, void (C::*func)(Arg1, Arg2))
{
    return MemberFunction_2<C, Arg1, Arg2>(ptr, func);
}

// num 3
// tmpl_arg_long typename Arg1, typename Arg2, typename Arg3
// tmpl_arg_short Arg1, Arg2, Arg3
// tmpl_call_long Arg1 arg1, Arg2 arg2, Arg3 arg3
// tmpl_call_short arg1, arg2, arg3

template<typename Arg1, typename Arg2, typename Arg3>
class Function_3
{

  public:
    virtual ~Function_3() {}

    virtual void call(Arg1 arg1, Arg2 arg2, Arg3 arg3) = 0;
    virtual Function_3<Arg1, Arg2, Arg3>* dup(void) const = 0;
};

template<class C, typename Arg1, typename Arg2, typename Arg3>
class MemberFunction_3 : public Function_3<Arg1, Arg2, Arg3>
{

  public:
    typedef void (C::*fct_ptr_t)(Arg1, Arg2, Arg3);

  private:
    C* _obj_ptr;
    fct_ptr_t _fct_ptr;

  public:
    MemberFunction_3(C* obj_ptr, fct_ptr_t fct_ptr)
      : _obj_ptr(obj_ptr)
      , _fct_ptr(fct_ptr)
    {}

    virtual ~MemberFunction_3() {}

    virtual void call(Arg1 arg1, Arg2 arg2, Arg3 arg3)
    {
        (_obj_ptr->*(this->_fct_ptr))(arg1, arg2, arg3);
    }

    virtual MemberFunction_3<C, Arg1, Arg2, Arg3>* dup(void) const
    {
        return new MemberFunction_3(_obj_ptr, _fct_ptr);
    }
};

template<typename Arg1, typename Arg2, typename Arg3>
class Callback_3
{
  protected:
    Function_3<Arg1, Arg2, Arg3>* _func;

  public:
    Callback_3()
      : _func(NULL)
    {}

    Callback_3(const Callback_3<Arg1, Arg2, Arg3>& cb)
      : _func(cb._func ? cb._func->dup() : NULL)
    {}

    Callback_3(const Function_3<Arg1, Arg2, Arg3>& func)
      : _func(func.dup())
    {}

    virtual ~Callback_3() { delete _func; }

    void operator=(const Callback_3<Arg1, Arg2, Arg3>& cb)
    {
        delete _func;

        _func = (cb._func ? cb._func->dup() : NULL);
    }

    void operator=(const Function_3<Arg1, Arg2, Arg3>& func)
    {
        delete _func;

        _func = func.dup();
    }

    virtual void call(Arg1 arg1, Arg2 arg2, Arg3 arg3)
    {
        if (_func) {
            _func->call(arg1, arg2, arg3);
        }
    }

    void operator()(Arg1 arg1, Arg2 arg2, Arg3 arg3) { call(arg1, arg2, arg3); }

    bool isSet(void) const { return _func != NULL; }
};

template<class C, typename Arg1, typename Arg2, typename Arg3>
inline MemberFunction_3<C, Arg1, Arg2, Arg3> mem_cb(C* ptr, void (C::*func)(Arg1, Arg2, Arg3))
{
    return MemberFunction_3<C, Arg1, Arg2, Arg3>(ptr, func);
}

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
