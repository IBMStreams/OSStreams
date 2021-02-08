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

#include <SPL/Core/EnumerationModelImpl.h>

using namespace std;
using namespace std::tr1;
using namespace SPL;
using namespace SPL::Enumeration_;

EnumerationModel::EnumerationModel(const EnumerationModel& other)
{
    // Call the assignment operator to perform the copy
    this->operator=(other);
}

const EnumerationModel& EnumerationModel::operator=(const EnumerationModel& other)
{
    class_ = other.class_;

    enumerators_.clear(); // Get rid of any existing enumerators as this is an assignment

    const vector<EnumeratorPtr>& enumerators = other.getEnumerators();
    for (size_t i = 0, iu = enumerators.size(); i < iu; ++i) {
        enumerators_.push_back(EnumeratorPtr(new Enumerator(*enumerators[i])));
    }
    return *this;
}

auto_ptr<enumerationModelType> EnumerationModel::toXsdInstance() const
{
    auto_ptr<enumerationModelType> model(new enumerationModelType());
    model->class_(class_);
    auto_ptr<enumeratorsType> enums(new enumeratorsType());
    for (size_t i = 0, iu = enumerators_.size(); i < iu; ++i) {
        auto_ptr<enumeratorType> e = enumerators_[i]->toXsdInstance();
        enums->enumerator().push_back(e);
    }
    model->enumerators(enums);
    return model;
}

Enumerator::Enumerator(const Enumerator& other)
{
    // Call the assignment operator to perform the copy
    this->operator=(other);
}

const Enumerator& Enumerator::operator=(const Enumerator& other)
{
    name_ = other.name_;
    return *this;
}

auto_ptr<enumeratorType> Enumerator::toXsdInstance() const
{
    auto_ptr<enumeratorType> e(new enumeratorType());
    e->name(name_);
    return e;
}
