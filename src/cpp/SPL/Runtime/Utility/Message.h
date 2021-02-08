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

#ifndef SPL_RUNTIME_UTILITY_MESSAGE_H
#define SPL_RUNTIME_UTILITY_MESSAGE_H

#include <SPL/Runtime/Utility/MessageFormatter.h>

#include <cassert>
#include <string>

namespace SPL {

/// @class Message
/// The Message class represents a user message and a set of substitution variables that
/// the MessageFormatter will use when formatting the message for display.
class Message
{
  public:
    /// An informational or diagnostic intended for the user and subject to translation
    /// @param formatter The formatter to be used for this message.
    /// @param key The key to the message
    /// @param englishText The format string for the english message
    Message(MessageFormatter& formatterIn,
            const std::string& keyIn,
            const std::string& englishTextIn)
      : formatter_(formatterIn)
      , key_(keyIn)
      , englishText_(englishTextIn)
    {}

    Message(const Message& ot); // Prevent copying

    class SubText
    {
      public:
        enum Type
        {
            String,
            Int64,
            Float32,
            Double64,
            Date
        };
        struct Date_t
        {
            Date_t(double d)
              : date_(d)
            {}
            double date_;
        };
        SubText(const std::string& text)
          : type_(String)
          , str_(text)
        {}
        SubText(int64_t text)
          : type_(Int64)
          , int64_(text)
        {}
        SubText(float text)
          : type_(Float32)
          , float32_(text)
        {}
        SubText(double text)
          : type_(Double64)
          , double64_(text)
        {}
        SubText(const Date_t& text)
          : type_(Date)
          , double64_(text.date_)
        {}

        Type type() const { return type_; }
        std::string const& asString() const
        {
            assert(type_ == String);
            return str_;
        }
        int64_t asInt64() const
        {
            assert(type_ == Int64);
            return int64_;
        }
        float asFloat32() const
        {
            assert(type_ == Float32);
            return float32_;
        }
        double asDouble64() const
        {
            assert(type_ == Double64);
            return double64_;
        }
        double asDate() const
        {
            assert(type_ == Date);
            return double64_;
        }

      private:
        Type type_;
        std::string str_;
        int64_t int64_;
        float float32_;
        double double64_;
    };

    Message& operator%(const std::string& text)
    {
        subTexts_.push_back(SubText(text));
        return *this;
    }

    Message& operator%(int64_t text)
    {
        subTexts_.push_back(SubText(text));
        return *this;
    }

    typedef std::vector<SubText> SubtextList;
    const SubtextList& substitutions() const { return subTexts_; }

    const std::string& key() const { return key_; }

    const std::string& englishText() const { return englishText_; }

    operator std::string() const { return formatter_.format(*this); }

    MessageFormatter& formatter() const { return formatter_; }

  private:
    MessageFormatter& formatter_;
    std::string key_;
    std::string englishText_;
    SubtextList subTexts_;
};

template<class Formatter>
class Message0 : public Message
{
  public:
    Message0(const std::string& keyIn, const std::string& englishTextIn)
      : Message(Formatter::instance(), keyIn, englishTextIn)
    {}

  private:
};

template<class Formatter, class A>
class Message1 : public Message
{
  public:
    Message1(const std::string& keyIn, const std::string& englishTextIn, const A& sub1)
      : Message(Formatter::instance(), keyIn, englishTextIn)
    {
        (*this) % sub1;
    }

  private:
};

template<class Formatter, class A, class B>
class Message2 : public Message
{
  public:
    Message2(const std::string& keyIn,
             const std::string& englishTextIn,
             const A& sub1,
             const B& sub2)
      : Message(Formatter::instance(), keyIn, englishTextIn)
    {
        (*this) % sub1 % sub2;
    }

  private:
};

template<class Formatter, class A, class B, class C>
class Message3 : public Message
{
  public:
    Message3(const std::string& keyIn,
             const std::string& englishTextIn,
             const A& sub1,
             const B& sub2,
             const C& sub3)
      : Message(Formatter::instance(), keyIn, englishTextIn)
    {
        (*this) % sub1 % sub2 % sub3;
    }

  private:
};

template<class Formatter, class A, class B, class C, class D>
class Message4 : public Message
{
  public:
    Message4(const std::string& keyIn,
             const std::string& englishTextIn,
             const A& sub1,
             const B& sub2,
             const C& sub3,
             const D& sub4)
      : Message(Formatter::instance(), keyIn, englishTextIn)
    {
        (*this) % sub1 % sub2 % sub3 % sub4;
    }

  private:
};

template<class Formatter, class A, class B, class C, class D, class E>
class Message5 : public Message
{
  public:
    Message5(const std::string& keyIn,
             const std::string& englishTextIn,
             const A& sub1,
             const B& sub2,
             const C& sub3,
             const D& sub4,
             const E& sub5)
      : Message(Formatter::instance(), keyIn, englishTextIn)
    {
        (*this) % sub1 % sub2 % sub3 % sub4 % sub5;
    }

  private:
};

template<class Formatter, class A, class B, class C, class D, class E, class F>
class Message6 : public Message
{
  public:
    Message6(const std::string& keyIn,
             const std::string& englishTextIn,
             const A& sub1,
             const B& sub2,
             const C& sub3,
             const D& sub4,
             const E& sub5,
             const F& sub6)
      : Message(Formatter::instance(), keyIn, englishTextIn)
    {
        (*this) % sub1 % sub2 % sub3 % sub4 % sub5 % sub6;
    }

  private:
};

template<class Formatter, class A, class B, class C, class D, class E, class F, class G>
class Message7 : public Message
{
  public:
    Message7(const std::string& keyIn,
             const std::string& englishTextIn,
             const A& sub1,
             const B& sub2,
             const C& sub3,
             const D& sub4,
             const E& sub5,
             const F& sub6,
             const G& sub7)
      : Message(Formatter::instance(), keyIn, englishTextIn)
    {
        (*this) % sub1 % sub2 % sub3 % sub4 % sub5 % sub6 % sub7;
    }

  private:
};

inline std::ostream& operator<<(std::ostream& ostr, const Message& msg)
{
    std::string formattedMessage(msg);
    ostr << formattedMessage;
    return ostr;
}
} // namespace SPL

#endif /* SPL_RUNTIME_UTILITY_MESSAGE_H */
