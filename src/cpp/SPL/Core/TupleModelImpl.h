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

#ifndef TUPLE_MODEL_IMPL_H
#define TUPLE_MODEL_IMPL_H

#include <SPL/Core/ModelHelper.h>
#include <SPL/Core/tupleModel.h> // xsd-generated

#include <vector>

namespace SPL {

namespace Tuple_ {

using namespace xmlns::prod::streams::spl::tuple;
#include <SPL/Core/TupleModelForwardDeclarations.h>

class TupleModel
{
  public:
    // Default ctor
    TupleModel() {}

    // Copy ctor
    TupleModel(const TupleModel& other);

    const TupleModel& operator=(const TupleModel& other);

    // Access the model's class name
    const std::string& getClass() const { return class_; }
    std::string& getClass() { return class_; }

    // Access the SPL tuple type
    const std::string& getSPLType() const { return splType_; }
    std::string& getSPLType() { return splType_; }

    // Access the model's facade
    const bool& getFacade() const { return facade_; }
    bool& getFacade() { return facade_; }

    // Access the model's equalitySupported
    const bool& getEqualitySupported() const { return equalitySupported_; }
    bool& getEqualitySupported() { return equalitySupported_; }

    // Access the headers needed by this tuple
    // Note that the header name does not include any file extension (.h) at this point
    // The extension will be added at code generation time.
    const std::vector<std::string>& getHeaders() const { return headers_; }
    std::vector<std::string>& getHeaders() { return headers_; }

    // Access the set of attributes for this tuple
    const std::vector<AttributePtr>& getAttributes() const { return attributes_; }
    std::vector<AttributePtr>& getAttributes() { return attributes_; }

    // Serialize the model
    std::auto_ptr<tupleModelType> toXsdInstance() const;

  private:
    // The class of the model
    std::string class_;

    // The SPL type name
    std::string splType_;

    // Is this tuple a facade?
    bool facade_;

    // Does this tuple support ==, !=?
    bool equalitySupported_;

    // The set of headers needed by this tuple
    std::vector<std::string> headers_;

    // The set of attributes associated with this tuple
    std::vector<AttributePtr> attributes_;
};

class Attribute
{
  public:
    Attribute() { sizeSet_ = false; }

    // Access the attribute's name
    const std::string& getName() const { return name_; }
    std::string& getName() { return name_; }

    // Access the attribute's type
    const std::string& getCPPType() const { return cppType_; }
    std::string& getCPPType() { return cppType_; }

    // Access the attribute's SPL type
    const std::string& getSPLType() const { return splType_; }
    std::string& getSPLType() { return splType_; }

    // fixed size field;
    void setFixedSize(int32_t size)
    {
        sizeSet_ = true;
        size_ = size;
    }
    int32_t getFixedSize() const
    {
        assert(sizeSet_);
        return size_;
    }
    bool hasFixedSize() const { return sizeSet_; }

    // Serialize the attribute
    std::auto_ptr<xmlns::prod::streams::spl::tuple::attributeType> toXsdInstance() const;

  private:
    // Attribute name
    std::string name_;

    // Attribute type
    std::string cppType_;

    // Attribute spl type
    std::string splType_;

    // fixedSize
    bool sizeSet_;
    int32_t size_;
};

} // namespace Tuple

} // namespace SPL

#endif // TUPLE_MODEL_IMPL_H
