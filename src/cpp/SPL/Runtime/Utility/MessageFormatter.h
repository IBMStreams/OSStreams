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

#ifndef MESSAGE_FORMATTER_H
#define MESSAGE_FORMATTER_H

#include <SPL/Runtime/Utility/Singleton.h>

#include <SPL/Runtime/Utility/Visibility.h>
#include <memory>
#include <string>

namespace SPL {

class FormattableMessage;
class Message;
class MessageFormatterImpl;

/// @class MessageFormatter
/// This is a utility class which, given a Message, finds the message text in
/// a locale-appropriate resource bundle, formats the substitution variables
/// in a locale-appropriate way, and renders the message into a string.
class DLL_PUBLIC MessageFormatter
{

  public:
    MessageFormatter(const std::string& bundleName,
                     const std::string& cantLoadBundle,
                     const std::string& cantLoadMsg);
    ~MessageFormatter();

    /// This method takes a Message, finds the text of the message in the
    /// appropriate resource bundle, formats and substitutes in any substitution
    /// variables and returns it as a string.
    /// @param msg The message to be formatted.
    /// @return Returns a string containing the formatted message.
    std::string format(const Message& msg);

    std::string format(const FormattableMessage& msg, bool includeKey);
    std::string format(const FormattableMessage& msg, bool includeKey, const std::string& locale);

    void registerToolkit(const std::string& toolkitName, const std::string& toolkitPath);

  private:
    MessageFormatterImpl& impl() { return *_impl.get(); }
    const MessageFormatterImpl& impl() const { return *_impl.get(); }
    std::auto_ptr<MessageFormatterImpl> _impl;
};

class DLL_PUBLIC RuntimeMessageFormatter
  : public MessageFormatter
  , public Singleton<RuntimeMessageFormatter>
{
  public:
    RuntimeMessageFormatter()
      : MessageFormatter("StreamsSPLRuntimeMessages", "CDISR5153W", "CDISR5000E")
      , Singleton<RuntimeMessageFormatter>(*this)
    {}
};

class DLL_PUBLIC CoreMessageFormatter
  : public MessageFormatter
  , public Singleton<CoreMessageFormatter>
{
  public:
    CoreMessageFormatter()
      : MessageFormatter("StreamsSPLCompilerMessages", "CDISP0716W", "CDISP0000E")
      , Singleton<CoreMessageFormatter>(*this)
    {}
};

} // namespace SPL

#endif
