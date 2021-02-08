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

#ifndef TOOLKIT_INFO_MODEL_IMPL_H
#define TOOLKIT_INFO_MODEL_IMPL_H

#include <SPL/Core/ModelHelper.h>
#include <SPL/Core/commonModel.h>
#include <SPL/Core/toolkitInfoModel.h>
#include <SPL/Runtime/Utility/Visibility.h>

namespace SPL {
DECL_EXCEPTION(SPL, SPLCompilerInvalidToolkitInfoModel, SPLCompiler)
namespace ToolkitInfo {
using namespace xmlns::prod::streams::spl::toolkitInfo;
using namespace xmlns::prod::streams::spl::common;

class Dependency;
typedef boost::shared_ptr<Dependency> DependencyPtr;
class MessageSet;
typedef boost::shared_ptr<MessageSet> MessageSetPtr;

class DLL_PUBLIC ToolkitInfoModel
{
  public:
    enum RootType
    {
        ToolkitDir,
        ApplicationDir,
        OutputDir
    };

    ToolkitInfoModel(const toolkitInfoModelType& xml);
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    const std::string& getVersion() const { return version_; }
    const std::string getRequiredProductVersion() const { return requiredProductVersion_; }
    const std::vector<DependencyPtr>& getDependencies() const { return dependencies_; }
    const std::vector<MessageSetPtr>& getMessageSets() const { return messageSets_; }
    const std::vector<std::pair<std::string, RootType> >& includes() const { return includes_; }
    const std::vector<std::string>& excludes() const { return excludes_; }

  private:
    std::string name_;
    std::string description_;
    std::string version_;
    std::string requiredProductVersion_;
    std::vector<DependencyPtr> dependencies_;
    std::vector<MessageSetPtr> messageSets_;
    std::vector<std::pair<std::string, RootType> > includes_;
    std::vector<std::string> excludes_;
};

class Dependency
{
  public:
    Dependency(const toolkitDependencyType& xml);
    Dependency(const std::string& name, const std::string& version)
      : name_(name)
      , version_(version)
    {}
    const std::string& getName() const { return name_; }
    const std::string& getVersion() const { return version_; }

  private:
    std::string name_;
    std::string version_;
};

class MessageSet
{
  public:
    MessageSet(const messageSetType& xml);
    const std::string& name() const { return name_; }
    const std::string& defaultLanguage() const { return defaultLang_; }
    const std::vector<std::string>& languages() const { return langs_; }

  private:
    std::string name_;
    std::string defaultLang_;
    std::vector<std::string> langs_;
};
typedef boost::shared_ptr<ToolkitInfoModel> ToolkitInfoModelPtr;
};
};

#endif /* TOOLKIT_INFO_MODEL_IMPL_H */
