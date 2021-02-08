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

#include <SPL/Runtime/Utility/FormattableMessage.h>

using namespace std;

namespace SPL {

class MessageImpl
{
  public:
    MessageImpl(const string& toolkitNameIn,
                const string& bundleNameIn,
                const std::string& defaultLangIn,
                const string& keyIn,
                const string& defaultTextIn,
                bool includeKeyIn)
      : _formatter(RuntimeMessageFormatter::instance())
      , _toolkitName(toolkitNameIn)
      , _bundleName(bundleNameIn)
      , _defaultLang(defaultLangIn)
      , _key(keyIn)
      , _defaultText(defaultTextIn)
      , _includeKey(includeKeyIn)
    {}

    MessageImpl(const MessageImpl& ot);

    typedef std::vector<Message::SubText> SubtextList;

    MessageFormatter& formatter() const { return _formatter; }

    const string& toolkitName() const { return _toolkitName; }

    const string& bundleName() const { return _bundleName; }

    const string& key() const { return _key; }

    const string& defaultText() const { return _defaultText; }

    const string& defaultLang() const { return _defaultLang; }

    bool includeKey() const { return _includeKey; }

    const Message::SubtextList& substitutions() const { return _subTexts; }
    Message::SubtextList& substitutions() { return _subTexts; }

  private:
    MessageFormatter& _formatter;
    string _toolkitName;
    string _bundleName;
    string _defaultLang;
    string _key;
    string _defaultText;
    bool _includeKey;
    Message::SubtextList _subTexts;
};

MessageImpl::MessageImpl(const MessageImpl& ot)
  : _formatter(ot._formatter)
  , _toolkitName(ot._toolkitName)
  , _bundleName(ot._bundleName)
  , _defaultLang(ot._defaultLang)
  , _key(ot._key)
  , _defaultText(ot._defaultText)
  , _includeKey(ot._includeKey)
{
    const Message::SubtextList& subs = ot.substitutions();
    for (size_t i = 0, sz = subs.size(); i < sz; ++i) {
        const Message::SubText& st = subs[i];
        switch (st.type()) {
            case Message::SubText::String:
                _subTexts.push_back(Message::SubText(st.asString()));
                break;
            case Message::SubText::Int64:
                _subTexts.push_back(Message::SubText(st.asInt64()));
                break;
            case Message::SubText::Float32:
                _subTexts.push_back(Message::SubText(st.asFloat32()));
                break;
            case Message::SubText::Double64:
                _subTexts.push_back(Message::SubText(st.asDouble64()));
                break;
            case Message::SubText::Date:
                _subTexts.push_back(Message::SubText(st.asDate()));
                break;
            default:
                assert(false); // can't get here
        }
    }
}

FormattableMessage::FormattableMessage(const string& toolkitNameIn,
                                       const string& bundleNameIn,
                                       const std::string& defaultLangIn,
                                       const std::string& keyIn,
                                       const string& defaultTextIn)
  : impl_(new MessageImpl(toolkitNameIn, bundleNameIn, defaultLangIn, keyIn, defaultTextIn, false))
{}

FormattableMessage::FormattableMessage(const string& toolkitNameIn,
                                       const string& bundleNameIn,
                                       const std::string& defaultLangIn,
                                       const std::string& keyIn,
                                       const string& defaultTextIn,
                                       bool includeKeyIn)
  : impl_(new MessageImpl(toolkitNameIn,
                          bundleNameIn,
                          defaultLangIn,
                          keyIn,
                          defaultTextIn,
                          includeKeyIn))
{}

FormattableMessage::FormattableMessage(const FormattableMessage& ot)
  : impl_(new MessageImpl(ot.impl()))
{}

FormattableMessage::~FormattableMessage() {}

FormattableMessage& FormattableMessage::operator%(const char* val)
{
    impl().substitutions().push_back(Message::SubText(string(val)));
    return *this;
}

FormattableMessage& FormattableMessage::operator%(const ustring& val)
{
    rstring s = ustring::toUTF8(val);
    impl().substitutions().push_back(Message::SubText(string(s)));
    return *this;
}

FormattableMessage& FormattableMessage::operator%(const rstring& val)
{
    impl().substitutions().push_back(Message::SubText(string(val)));
    return *this;
}

FormattableMessage& FormattableMessage::operator%(int8_t val)
{
    impl().substitutions().push_back(Message::SubText((int64_t)val));
    return *this;
}

FormattableMessage& FormattableMessage::operator%(uint8_t val)
{
    impl().substitutions().push_back(Message::SubText((int64_t)val));
    return *this;
}

FormattableMessage& FormattableMessage::operator%(int16_t val)
{
    impl().substitutions().push_back(Message::SubText((int64_t)val));
    return *this;
}

FormattableMessage& FormattableMessage::operator%(uint16_t val)
{
    impl().substitutions().push_back(Message::SubText((int64_t)val));
    return *this;
}

FormattableMessage& FormattableMessage::operator%(int32_t val)
{
    impl().substitutions().push_back(Message::SubText((int64_t)val));
    return *this;
}

FormattableMessage& FormattableMessage::operator%(uint32_t val)
{
    impl().substitutions().push_back(Message::SubText((int64_t)val));
    return *this;
}

FormattableMessage& FormattableMessage::operator%(int64_t val)
{
    impl().substitutions().push_back(Message::SubText(val));
    return *this;
}

FormattableMessage& FormattableMessage::operator%(uint64_t val)
{
    impl().substitutions().push_back(Message::SubText((int64_t)val));
    return *this;
}

FormattableMessage& FormattableMessage::operator%(float val)
{
    impl().substitutions().push_back(Message::SubText(val));
    return *this;
}

FormattableMessage& FormattableMessage::operator%(double val)
{
    impl().substitutions().push_back(Message::SubText(val));
    return *this;
}

FormattableMessage& FormattableMessage::operator%(const Message::SubText::Date_t& val)
{
    impl().substitutions().push_back(Message::SubText(val));
    return *this;
}

const FormattableMessage::SubtextList& FormattableMessage::substitutions() const
{
    return impl().substitutions();
}

const string& FormattableMessage::toolkitName() const
{
    return impl().toolkitName();
}

const string& FormattableMessage::bundleName() const
{
    return impl().bundleName();
}

const string& FormattableMessage::key() const
{
    return impl().key();
}

const string& FormattableMessage::defaultText() const
{
    return impl().defaultText();
}

const string& FormattableMessage::defaultLang() const
{
    return impl().defaultLang();
}

FormattableMessage::operator string() const
{
    return impl().formatter().format(*this, impl().includeKey());
}

FormattableMessage::operator rstring() const
{
    return impl().formatter().format(*this, impl().includeKey());
}

string FormattableMessage::format() const
{
    return impl().formatter().format(*this, impl().includeKey());
}

string FormattableMessage::format(const string& locale) const
{
    return impl().formatter().format(*this, impl().includeKey(), locale);
}

} // namespace SPL
