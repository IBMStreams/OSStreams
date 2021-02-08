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

#ifndef SPL_RUNTIME_COMMON_SYSTEM_METRIC_INFO_FACTORY_H
#define SPL_RUNTIME_COMMON_SYSTEM_METRIC_INFO_FACTORY_H

#include <SPL/Runtime/Common/Metric.h>
#include <UTILS/HashMapHelpers.h>

#include <memory>
#include <vector>

namespace SPL {
class SystemMetricInfoFactory;

/// Class that represents metric meta-data
class SystemMetricInfo
{
  public:
    /// Default constructor
    ///
    SystemMetricInfo() {}

    /// Constructor
    /// @param name name of the metric
    /// @param index index number of this metric
    /// @param description description of the metric
    /// @param kind kind of the metric
    SystemMetricInfo(std::string const& name,
                     uint32_t index,
                     std::string const& description,
                     Metric::Kind kind)
      : name_(name)
      , index_(index)
      , description_(description)
      , kind_(kind)
    {}

    /// Get the name of the metric
    /// @return name of the metric
    std::string const& getName() const { return name_; }

    /// Get the index of the metric
    /// @return index of the metric
    uint32_t getIndex() const { return index_; }

    /// Get the description of the metric
    /// @return description of the metric
    std::string const& getDescription() const { return description_; }

    /// Get the kind of the metric
    /// @return kind of the metric
    Metric::Kind getKind() const { return kind_; }

  private:
    std::string name_;
    uint32_t index_;
    std::string description_;
    Metric::Kind kind_;
    friend class SystemMetricInfoFactory;
};

/// Factory class for accessing system metric meta-data
class SystemMetricInfoFactory
{
  public:
    /// Type of the system metric
    enum Type
    {
        PE,                //!< PE level
        PEInputPort,       //!< PE input port level
        PEOutputPort,      //!< PE output port level
        Operator,          //!< Operator level
        OperatorInputPort, //!< Operator input port level
        OperatorOutputPort //!< Operator output port level
    };

    /// The Constructor
    SystemMetricInfoFactory();

    /// Get the meta-data associated with the given metric
    /// @param name name of the metric
    /// @param type type of the metric
    /// @return meta-data associated with the given metric
    /// @throws SPLRuntimeException if the metric does not exist
    static SystemMetricInfo const& getSystemMetricInfo(std::string const& name, Type type);

    /// Return the number of metrics for a given type
    /// @param type type of the metric
    /// @return number of metrics associated with this type
    static uint32_t getSystemMetricInfoCount(Type type);

  private:
    static const int nTypes = 6;
    SystemMetricInfo const& getInfo(std::string const& name, Type type);
    uint32_t getCount(Type type);
    std::vector<std::tr1::unordered_map<std::string, SystemMetricInfo> > info_;
};
}
#endif /*SPL_RUNTIME_COMMON_SYSTEM_METRIC_INFO_FACTORY_H*/
