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

#ifndef SPL_SUPPORTED_ANNOTATIONS_H
#define SPL_SUPPORTED_ANNOTATIONS_H

#include <SPL/Runtime/Utility/Singleton.h>
#include <UTILS/HashMapHelpers.h>

#include <string>
#include <vector>

namespace SPL {

class SupportedAnnotation
{
  public:
    class Key
    {
      public:
        Key(const std::string& n, bool r, bool s)
          : _name(n)
          , _required(r)
          , _submissionTimeValueAllowed(s)
        {}
        const std::string name() const { return _name; }
        bool required() const { return _required; }
        bool submissionTimeValueAllowed() const { return _submissionTimeValueAllowed; }

      private:
        std::string _name;
        bool _required;
        bool _submissionTimeValueAllowed;
    };

    SupportedAnnotation(const std::string& n, unsigned maxO, bool standalonePermitted)
      : _name(n)
      , _maxOccur(maxO)
      , _standalonePermitted(standalonePermitted)
    {}
    void addKey(const std::string& n, bool r, bool s);
    const std::string& name() const { return _name; }

    const SupportedAnnotation::Key* findKey(const std::string& keyName) const;

    // A vector is being used at the moment because there are expected to be relatively few keys
    typedef std::vector<Key*> Keys;
    const Keys& keys() const { return _keys; }

    // Specifies the maximum times an annotation may be specified
    unsigned maxOccur() const { return _maxOccur; }

    bool permittedInStandaloneMode() const { return _standalonePermitted; }

  private:
    std::string _name;
    unsigned _maxOccur;
    bool _standalonePermitted;
    Keys _keys;
};

class SupportedAnnotations : public Singleton<SupportedAnnotations>
{
  public:
    SupportedAnnotations();
    const SupportedAnnotation* find(const std::string& name);
    const SupportedAnnotation* add(const std::string& name,
                                   unsigned maxO,
                                   bool standalonePermitted);
    void addKey(const std::string& name, bool r, bool s);

  private:
    typedef std::tr1::unordered_map<std::string, SupportedAnnotation*> SupportedAnnotationsMap;
    SupportedAnnotationsMap _supportedAnnotations;
};

} // namespace SPL

#endif // SPL_SUPPORTED_ANNOTATIONS_H
