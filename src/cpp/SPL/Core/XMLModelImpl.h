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

#ifndef XML_MODEL_IMPL_H
#define XML_MODEL_IMPL_H

#include <SPL/Core/ModelHelper.h>
#include <SPL/Core/xmlModel.h> // xsd-generated

#include <vector>

namespace SPL {

namespace XML_ {

using namespace xmlns::prod::streams::spl::xml;

class XMLModel
{
  public:
    // Default ctor
    XMLModel()
      : schema_()
      , toolkitIndex_(0)
    {}

    // Copy ctor
    XMLModel(const XMLModel& other);

    const XMLModel& operator=(const XMLModel& other);

    // Access the model's class name
    const std::string& getClass() const { return class_; }
    std::string& getClass() { return class_; }

    // Access the model's schema name
    const std::string& getSchema() const { return schema_; }
    std::string& getSchema() { return schema_; }

    const uint32_t getToolkitIndex() const { return toolkitIndex_; }
    uint32_t getToolkitIndex() { return toolkitIndex_; }

    // Serialize the model
    std::auto_ptr<xmlModelType> toXsdInstance() const;

  private:
    // The class of the model
    std::string class_;
    std::string schema_;
    uint32_t toolkitIndex_;
};

} // namespace XML_

} // namespace SPL

#endif // XML_MODEL_IMPL_H
