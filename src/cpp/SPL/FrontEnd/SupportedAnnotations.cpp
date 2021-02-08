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

#include <SPL/FrontEnd/SupportedAnnotations.h> // Include this header first

#include <SPL/Runtime/Utility/Singleton.t>

#include <boost/pool/object_pool.hpp>

using namespace std;

boost::object_pool<SPL::SupportedAnnotation> annotationPool;
boost::object_pool<SPL::SupportedAnnotation::Key> keyPool;

namespace SPL {

SupportedAnnotations supportedAnnotations; // create the single instance here

template class Singleton<SupportedAnnotations>; // explicitly instantiate the singleton here

void SupportedAnnotation::addKey(const std::string& n, bool r, bool s)
{
    _keys.push_back(keyPool.construct(n, r, s));
}

const SupportedAnnotation::Key* SupportedAnnotation::findKey(const string& keyName) const
{
    Keys::const_iterator it = _keys.begin();
    for (; it != _keys.end(); ++it) {
        if ((*it)->name() == keyName) {
            return *it;
        }
    }
    return NULL;
}

SupportedAnnotations::SupportedAnnotations()
  : Singleton<SupportedAnnotations>(*this)
#include "SPLAnnotationsGen.cpp"

      const SupportedAnnotation
  * SupportedAnnotations::add(const string& name, unsigned maxO, bool standalonePermitted)
{
    SupportedAnnotation* a = annotationPool.construct(name, maxO, standalonePermitted);
    _supportedAnnotations.insert(make_pair(name, a));
    return a;
}

const SupportedAnnotation* SupportedAnnotations::find(const string& name)
{
    SupportedAnnotationsMap::const_iterator it = _supportedAnnotations.find(name);
    if (it != _supportedAnnotations.end()) {
        return it->second;
    }
    return NULL;
}

void SupportedAnnotations::addKey(const string& name, bool r, bool s)
{
    SupportedAnnotationsMap::const_iterator it = _supportedAnnotations.find(name);
    if (it != _supportedAnnotations.end()) {
        (it->second)->addKey(name, r, s);
    }
}

}
