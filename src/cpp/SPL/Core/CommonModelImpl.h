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

#ifndef LIBRARY_MODEL_IMPL_H
#define LIBRARY_MODEL_IMPL_H

#include <SPL/Core/ModelHelper.h>
#include <SPL/Core/commonModel.h> // xsd-generated

namespace SPL {

DECL_EXCEPTION(SPL, SPLCompilerInvalidLibraryModel, SPLCompiler);

namespace Common {

using namespace xmlns::prod::streams::spl::common;

class ManagedLibrary
{
  public:
    ManagedLibrary(const managedLibraryType& managed, const std::string& baseDir);

    const std::vector<std::string>& getLibs() const { return libs_; }
    const std::vector<std::string>& getLibPaths() const { return libPaths_; }
    const std::vector<std::string>& getIncludePaths() const { return includePaths_; }
    const std::string& getCommand() const { return command_; }

  private:
    std::vector<std::string> libs_;
    std::vector<std::string> libPaths_;
    std::vector<std::string> includePaths_;
    std::string command_;
};

class Library
{
  public:
    Library(const libraryType& xml, const std::string& baseDir);
    ~Library();
    const std::string& getDescription() const { return description_; }
    const ManagedLibrary& managed() const { return *_managed; }
    bool isManaged() const { return true; }
    std::auto_ptr<libraryType> toXsdInstance() const;

  private:
    std::string description_;
    ManagedLibrary* _managed;
};

};

};

#endif /* LIBRARY_MODEL_IMPL_ */
