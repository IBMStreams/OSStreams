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

#ifndef ENUM_MODEL_IMPL_H
#define ENUM_MODEL_IMPL_H

#include <SPL/Core/ModelHelper.h>
#include <SPL/Core/enumerationModel.h> // xsd-generated

#include <vector>

namespace SPL {

namespace Enumeration_ {

using namespace xmlns::prod::streams::spl::enumeration;
#include <SPL/Core/EnumerationModelForwardDeclarations.h>

class EnumerationModel
{
  public:
    // Default ctor
    EnumerationModel() {}

    // Copy ctor
    EnumerationModel(const EnumerationModel& other);

    const EnumerationModel& operator=(const EnumerationModel& other);

    // Access the model's class name
    const std::string& getClass() const { return class_; }
    std::string& getClass() { return class_; }

    // Access the set of enumerators for this enumeration
    const std::vector<EnumeratorPtr>& getEnumerators() const { return enumerators_; }
    std::vector<EnumeratorPtr>& getEnumerators() { return enumerators_; }

    // Serialize the model
    std::auto_ptr<enumerationModelType> toXsdInstance() const;

  private:
    // The class of the model
    std::string class_;

    // The set of enumerators associated with this enumeration
    std::vector<EnumeratorPtr> enumerators_;
};

class Enumerator
{
  public:
    // Default ctor
    Enumerator()
      : name_()
    {}

    // Copy ctor
    Enumerator(const Enumerator& other);

    // Assignment operator
    const Enumerator& operator=(const Enumerator& other);

    // Access the enumerator's name
    const std::string& getName() const { return name_; }
    std::string& getName() { return name_; }

    // Serialize the attribute
    std::auto_ptr<enumeratorType> toXsdInstance() const;

  private:
    // Attribute name
    std::string name_;
};

} // namespace Enumeration_

} // namespace SPL

#endif // ENUM_MODEL_IMPL_H
