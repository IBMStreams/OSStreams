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

#include <SPL/Core/TupleModelImpl.h>

using namespace std;
using namespace std::tr1;
using namespace SPL;
using namespace SPL::Tuple_;

TupleModel::TupleModel(const TupleModel& other)
{
    // Call the assignment operator to perform the copy
    this->operator=(other);
}

const TupleModel& TupleModel::operator=(const TupleModel& other)
{
    class_ = other.class_;
    splType_ = other.splType_;
    facade_ = other.facade_;
    equalitySupported_ = other.equalitySupported_;
    headers_ = other.headers_;
    attributes_.clear(); // Get rid of any existing attributes as this is an assignment

    const vector<AttributePtr>& attributes = other.getAttributes();
    for (size_t i = 0, iu = attributes.size(); i < iu; ++i) {
        attributes_.push_back(AttributePtr(new Attribute(*attributes[i])));
    }
    return *this;
}

auto_ptr<tupleModelType> TupleModel::toXsdInstance() const
{
    auto_ptr<tupleModelType> model(new tupleModelType());
    model->class_(class_);
    model->splType(splType_);
    model->facade(facade_);
    model->equalitySupported(equalitySupported_);
    auto_ptr<headersType> hdrs(new headersType());
    for (size_t i = 0, iu = headers_.size(); i < iu; ++i) {
        hdrs->header().push_back(headers_[i]);
    }
    model->headers(hdrs);
    auto_ptr<attributesType> attrs(new attributesType());
    for (size_t i = 0, iu = attributes_.size(); i < iu; ++i) {
        auto_ptr<attributeType> attr = attributes_[i]->toXsdInstance();
        attrs->attribute().push_back(attr);
    }
    model->attributes(attrs);
    return model;
}

auto_ptr<attributeType> Attribute::toXsdInstance() const
{
    auto_ptr<attributeType> attr(new attributeType());
    attr->name(name_);
    attr->cppType(cppType_);
    attr->splType(splType_);
    if (sizeSet_)
        attr->fixedSize().set(size_);
    return attr;
}
