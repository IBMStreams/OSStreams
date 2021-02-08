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

#ifndef SPL_RUNTIME_UTILITY_FORMATTABLE_MESSAGE_H
#define SPL_RUNTIME_UTILITY_FORMATTABLE_MESSAGE_H

#include <SPL/Runtime/Type/String.h>
#include <SPL/Runtime/Utility/Message.h>
#include <SPL/Runtime/Utility/MessageFormatter.h>

#include <cassert>
#include <memory>
#include <string>

namespace SPL
{

class MessageImpl;

/// @class FormattableMessage
/// The FormattableMessage class represents a user message and a set of substitution variables that
/// the MessageFormatter will use when formatting the message for display.
class DLL_PUBLIC FormattableMessage
{
public:

    /// @param toolkitNameIn The toolkit containing this message.
    /// @param bundleNameIn The resource bundle containing this message.
    /// @param defaultLangIn The language that the default message text is in.  If this matches the running locale the default text will be used.
    /// @param keyIn The key to the message
    /// @param defaultTextIn The format string for the default message
    FormattableMessage(const std::string & toolkitNameIn, const std::string & bundleNameIn,
                       const std::string & defaultLangIn, const std::string& keyIn, const std::string& defaultTextIn);

    /// @param toolkitNameIn The toolkit containing this message.
    /// @param bundleNameIn The resource bundle containing this message.
    /// @param defaultLangIn The language that the default message text is in.  If this matches the running locale the default text will be used.
    /// @param keyIn The key to the message
    /// @param defaultTextIn The format string for the default message
    /// @param includeKeyIn  Prefix the message with the message key
    FormattableMessage(const std::string & toolkitNameIn, const std::string & bundleNameIn,
                       const std::string & defaultLangIn, const std::string& keyIn, const std::string& defaultTextIn, bool includeKeyIn);

    FormattableMessage(const FormattableMessage& ot);
    virtual ~FormattableMessage();

    typedef std::vector<Message::SubText> SubtextList;

    FormattableMessage & operator%(const rstring& val);
    FormattableMessage & operator%(const ustring& val);
    FormattableMessage & operator%(const char * val);

    FormattableMessage & operator%(int8_t val);
    FormattableMessage & operator%(uint8_t val);
    FormattableMessage & operator%(int16_t val);
    FormattableMessage & operator%(uint16_t val);
    FormattableMessage & operator%(int32_t val);
    FormattableMessage & operator%(uint32_t val);
    FormattableMessage & operator%(int64_t val);
    FormattableMessage & operator%(uint64_t val);

    FormattableMessage & operator%(float val);
    FormattableMessage & operator%(double val);

    FormattableMessage & operator%(const Message::SubText::Date_t & val);

    const SubtextList & substitutions() const;

    const std::string & toolkitName() const;

    const std::string & bundleName() const;

    const std::string & key() const;

    const std::string& defaultText() const;

    const std::string& defaultLang() const;

    operator std::string() const;
    operator rstring() const;

    std::string format() const;
    std::string format(const std::string & locale) const;

private:
    MessageImpl & impl() { return *impl_.get(); }
    const MessageImpl & impl() const { return *impl_.get(); }
    std::auto_ptr<MessageImpl> impl_;
};

class FormattableMessage0 : public FormattableMessage
{
public:
    FormattableMessage0(const std::string & toolkitNameIn, const std::string & bundleNameIn, const std::string & defaultLangIn, const std::string& keyIn, const std::string& defaultTextIn)
     : FormattableMessage(toolkitNameIn, bundleNameIn, defaultLangIn, keyIn, defaultTextIn)
     {
     }

    FormattableMessage0(const std::string & toolkitNameIn, const std::string & bundleNameIn, const std::string & defaultLangIn, const std::string& keyIn, const std::string& defaultTextIn, bool includeKeyIn)
     : FormattableMessage(toolkitNameIn, bundleNameIn, defaultLangIn, keyIn, defaultTextIn, includeKeyIn)
     {
     }
};

template <class A>
class FormattableMessage1 : public FormattableMessage
{
public:
    FormattableMessage1(const std::string & toolkitNameIn, const std::string & bundleNameIn, const std::string & defaultLangIn, const std::string& keyIn, const std::string& defaultTextIn, const A& sub1)
     : FormattableMessage(toolkitNameIn, bundleNameIn, defaultLangIn, keyIn, defaultTextIn)
     {
     (*this) % sub1;
     }

    FormattableMessage1(const std::string & toolkitNameIn, const std::string & bundleNameIn, const std::string & defaultLangIn, const std::string& keyIn, const std::string& defaultTextIn,
     bool includeKeyIn, const A& sub1)
     : FormattableMessage(toolkitNameIn, bundleNameIn, defaultLangIn, keyIn, defaultTextIn, includeKeyIn)
     {
     (*this) % sub1;
     }
};

template <class A, class B>
class FormattableMessage2 : public FormattableMessage
{
public:
   FormattableMessage2(const std::string & toolkitNameIn, const std::string & bundleNameIn, const std::string & defaultLangIn, const std::string& keyIn, const std::string& defaultTextIn, const A& sub1,
     const B& sub2)
     : FormattableMessage(toolkitNameIn, bundleNameIn, defaultLangIn, keyIn, defaultTextIn)
     {
     (*this) % sub1 % sub2;
     }

   FormattableMessage2(const std::string & toolkitNameIn, const std::string & bundleNameIn, const std::string & defaultLangIn,
     const std::string& keyIn, const std::string& defaultTextIn, bool includeKeyIn,
     const A& sub1, const B& sub2)
     : FormattableMessage(toolkitNameIn, bundleNameIn, defaultLangIn, keyIn, defaultTextIn, includeKeyIn)
     {
     (*this) % sub1 % sub2;
     }
};

template <class A, class B, class C>
class FormattableMessage3 : public FormattableMessage
{
public:
    FormattableMessage3(const std::string & toolkitNameIn, const std::string & bundleNameIn, const std::string & defaultLangIn,
                        const std::string& keyIn, const std::string& defaultTextIn, const A& sub1, const B& sub2, const C& sub3)
     : FormattableMessage(toolkitNameIn, bundleNameIn, defaultLangIn, keyIn, defaultTextIn)
     {
     (*this) % sub1 % sub2 % sub3;
     }

    FormattableMessage3(const std::string & toolkitNameIn, const std::string & bundleNameIn, const std::string & defaultLangIn,
                        const std::string& keyIn, const std::string& defaultTextIn, bool includeKeyIn, const A& sub1, const B& sub2,
                        const C& sub3)
     : FormattableMessage(toolkitNameIn, bundleNameIn, defaultLangIn, keyIn, defaultTextIn, includeKeyIn)
     {
     (*this) % sub1 % sub2 % sub3;
     }
};

template <class A, class B, class C, class D>
class FormattableMessage4 : public FormattableMessage
{
public:
    FormattableMessage4(const std::string & toolkitNameIn, const std::string & bundleNameIn, const std::string & defaultLangIn,
            const std::string& keyIn, const std::string& defaultTextIn, const A& sub1,
            const B& sub2, const C& sub3, const D& sub4)
     : FormattableMessage(toolkitNameIn, bundleNameIn, defaultLangIn, keyIn, defaultTextIn)
     {
     (*this) % sub1 % sub2 % sub3 % sub4;
     }

    FormattableMessage4(const std::string & toolkitNameIn, const std::string & bundleNameIn, const std::string & defaultLangIn,
            const std::string& keyIn, const std::string& defaultTextIn, bool includeKeyIn, const A& sub1,
            const B& sub2, const C& sub3, const D& sub4)
     : FormattableMessage(toolkitNameIn, bundleNameIn, defaultLangIn, keyIn, defaultTextIn, includeKeyIn)
     {
     (*this) % sub1 % sub2 % sub3 % sub4;
     }
};

template <class A, class B, class C, class D, class E>
class FormattableMessage5 : public FormattableMessage
{
public:
    FormattableMessage5(const std::string & toolkitNameIn, const std::string & bundleNameIn, const std::string & defaultLangIn,
            const std::string& keyIn, const std::string& defaultTextIn, const A& sub1,
            const B& sub2, const C& sub3, const D& sub4, const E& sub5)
     : FormattableMessage(toolkitNameIn, bundleNameIn, defaultLangIn, keyIn, defaultTextIn)
     {
     (*this) % sub1 % sub2 % sub3 % sub4 % sub5;
     }

    FormattableMessage5(const std::string & toolkitNameIn, const std::string & bundleNameIn, const std::string & defaultLangIn,
            const std::string& keyIn, const std::string& defaultTextIn, bool includeKeyIn, const A& sub1,
            const B& sub2, const C& sub3, const D& sub4, const E& sub5)
     : FormattableMessage(toolkitNameIn, bundleNameIn, defaultLangIn, keyIn, defaultTextIn, includeKeyIn)
     {
     (*this) % sub1 % sub2 % sub3 % sub4 % sub5;
     }
};

template <class A, class B, class C, class D, class E, class F>
class FormattableMessage6 : public FormattableMessage
{
public:
    FormattableMessage6(const std::string & toolkitNameIn, const std::string & bundleNameIn, const std::string & defaultLangIn,
            const std::string& keyIn, const std::string& defaultTextIn, const A& sub1,
            const B& sub2, const C& sub3, const D& sub4, const E& sub5,
            const F& sub6)
     : FormattableMessage(toolkitNameIn, bundleNameIn, defaultLangIn, keyIn, defaultTextIn)
     {
     (*this) % sub1 % sub2 % sub3 % sub4 % sub5 % sub6;
     }

    FormattableMessage6(const std::string & toolkitNameIn, const std::string & bundleNameIn, const std::string & defaultLangIn,
            const std::string& keyIn, const std::string& defaultTextIn, bool includeKeyIn, const A& sub1,
            const B& sub2, const C& sub3, const D& sub4, const E& sub5,
            const F& sub6)
     : FormattableMessage(toolkitNameIn, bundleNameIn, defaultLangIn, keyIn, defaultTextIn, includeKeyIn)
     {
     (*this) % sub1 % sub2 % sub3 % sub4 % sub5 % sub6;
     }
};

template <class A, class B, class C, class D, class E, class F, class G>
class FormattableMessage7 : public FormattableMessage
{
public:
    FormattableMessage7(const std::string & toolkitNameIn, const std::string & bundleNameIn, const std::string & defaultLangIn,
            const std::string& keyIn, const std::string& defaultTextIn, const A& sub1,
            const B& sub2, const C& sub3, const D& sub4, const E& sub5,
            const F& sub6, const G& sub7)
     : FormattableMessage(toolkitNameIn, bundleNameIn, defaultLangIn, keyIn, defaultTextIn)
     {
     (*this) % sub1 % sub2 % sub3 % sub4 % sub5 % sub6 % sub7;
     }

    FormattableMessage7(const std::string & toolkitNameIn, const std::string & bundleNameIn, const std::string & defaultLangIn,
            const std::string& keyIn, const std::string& defaultTextIn, bool includeKeyIn, const A& sub1,
            const B& sub2, const C& sub3, const D& sub4, const E& sub5,
            const F& sub6, const G& sub7)
     : FormattableMessage(toolkitNameIn, bundleNameIn, defaultLangIn, keyIn, defaultTextIn, includeKeyIn)
     {
     (*this) % sub1 % sub2 % sub3 % sub4 % sub5 % sub6 % sub7;
     }
};

template <class A, class B, class C, class D, class E, class F, class G, class H>
class FormattableMessage8 : public FormattableMessage
{
public:
    FormattableMessage8(const std::string & toolkitNameIn, const std::string & bundleNameIn, const std::string & defaultLangIn,
            const std::string& keyIn, const std::string& defaultTextIn, const A& sub1,
            const B& sub2, const C& sub3, const D& sub4, const E& sub5,
            const F& sub6, const G& sub7, const H& sub8)
     : FormattableMessage(toolkitNameIn, bundleNameIn, defaultLangIn, keyIn, defaultTextIn)
     {
     (*this) % sub1 % sub2 % sub3 % sub4 % sub5 % sub6 % sub7 % sub8;
     }

    FormattableMessage8(const std::string & toolkitNameIn, const std::string & bundleNameIn, const std::string & defaultLangIn,
            const std::string& keyIn, const std::string& defaultTextIn, bool includeKeyIn, const A& sub1,
            const B& sub2, const C& sub3, const D& sub4, const E& sub5,
            const F& sub6, const G& sub7, const H& sub8)
     : FormattableMessage(toolkitNameIn, bundleNameIn, defaultLangIn, keyIn, defaultTextIn, includeKeyIn)
     {
     (*this) % sub1 % sub2 % sub3 % sub4 % sub5 % sub6 % sub7 % sub8;
     }
};

template <class A, class B, class C, class D, class E, class F, class G, class H, class I>
class FormattableMessage9 : public FormattableMessage
{
public:
    FormattableMessage9(const std::string & toolkitNameIn, const std::string & bundleNameIn, const std::string & defaultLangIn,
            const std::string& keyIn, const std::string& defaultTextIn, const A& sub1,
            const B& sub2, const C& sub3, const D& sub4, const E& sub5,
            const F& sub6, const G& sub7, const H& sub8, const I& sub9)
     : FormattableMessage(toolkitNameIn, bundleNameIn, defaultLangIn, keyIn, defaultTextIn)
     {
     (*this) % sub1 % sub2 % sub3 % sub4 % sub5 % sub6 % sub7 % sub8 % sub9;
     }

    FormattableMessage9(const std::string & toolkitNameIn, const std::string & bundleNameIn, const std::string & defaultLangIn,
            const std::string& keyIn, const std::string& defaultTextIn, bool includeKeyIn, const A& sub1,
            const B& sub2, const C& sub3, const D& sub4, const E& sub5,
            const F& sub6, const G& sub7, const H& sub8, const I& sub9)
     : FormattableMessage(toolkitNameIn, bundleNameIn, defaultLangIn, keyIn, defaultTextIn, includeKeyIn)
     {
     (*this) % sub1 % sub2 % sub3 % sub4 % sub5 % sub6 % sub7 % sub8 % sub9;
     }
};

template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J>
class FormattableMessage10 : public FormattableMessage
{
public:
    FormattableMessage10(const std::string & toolkitNameIn, const std::string & bundleNameIn, const std::string & defaultLangIn,
            const std::string& keyIn, const std::string& defaultTextIn, const A& sub1,
            const B& sub2, const C& sub3, const D& sub4, const E& sub5,
            const F& sub6, const G& sub7, const H& sub8, const I& sub9,
            const J& sub10)
     : FormattableMessage(toolkitNameIn, bundleNameIn, defaultLangIn, keyIn, defaultTextIn)
     {
     (*this) % sub1 % sub2 % sub3 % sub4 % sub5 % sub6 % sub7 % sub8 % sub9 % sub10;
     }

    FormattableMessage10(const std::string & toolkitNameIn, const std::string & bundleNameIn, const std::string & defaultLangIn,
            const std::string& keyIn, const std::string& defaultTextIn, bool includeKeyIn, const A& sub1,
            const B& sub2, const C& sub3, const D& sub4, const E& sub5,
            const F& sub6, const G& sub7, const H& sub8, const I& sub9,
            const J& sub10)
     : FormattableMessage(toolkitNameIn, bundleNameIn, defaultLangIn, keyIn, defaultTextIn, includeKeyIn)
     {
     (*this) % sub1 % sub2 % sub3 % sub4 % sub5 % sub6 % sub7 % sub8 % sub9 % sub10;
     }
};

inline std::ostream& operator<<(std::ostream & ostr, const FormattableMessage& msg)
    {
    std::string formattedMessage(msg);
    ostr << formattedMessage;
    return ostr;
    }
}  // namespace SPL

#endif /* SPL_RUNTIME_UTILITY_FORMATTABLE_MESSAGE_H */
