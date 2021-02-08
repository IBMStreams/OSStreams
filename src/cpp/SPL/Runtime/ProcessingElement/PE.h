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

#ifndef SPL_RUNTIME_PROCESSING_ELEMENT_PE_H
#define SPL_RUNTIME_PROCESSING_ELEMENT_PE_H

#include <SPL/Runtime/ProcessingElement/BasePE.h>
#include <SPL/Runtime/Utility/Singleton.h>
#include <SPL/Runtime/Utility/Visibility.h>

#include <string>
#include <vector>

namespace SPL {
class PEImpl;
class Tuple;
class Punctuation;

/// Class that represents a processing element in pre v4.2 releases
/// It remains in order to facilitate access to the jvm args and runtime
/// yield behavior which is embedded in the PE baked into the application
/// bundle
class DLL_PUBLIC PE
  : public BasePE
  , public Singleton<PE>
{
  public:
    //        PE(); // Empty constructor for mock-up tests

    /// Constructor
    /// @param standalone true if the PE is in standalone mode, false otherwise
    //        PE(bool standalone = false);

    /// Constructor
    /// @param standalone true if the PE is in standalone mode, false otherwise
    /// @param javaVMArgs NULL if no Java required; otherwise, JVM arguments
    PE(bool standalone, std::vector<std::string>* javaVMArgs);

    /// Destructor
    virtual ~PE();

    virtual void toolkitsWithResources(std::vector<std::string>& toolkits) {}

    BasePEImpl& getImpl();

    // Only used when submitting pre 4.2 apps on a 4.2 or greater release.
    std::vector<std::string>* getJavaVMArgs() { return javaVMArgs_; }

  private:
    //        std::auto_ptr<PEImpl> impl_;
    std::vector<std::string>* javaVMArgs_;
};

/// This class replaces the use of PE in v4.2 and newer releases
class DLL_PUBLIC SplPE
  : public BasePE
  , public Singleton<SplPE>
{
  public:
    SplPE(); // Empty constructor for mock-up tests

    /// Constructor
    /// @param standalone true if the PE is in standalone mode, false otherwise
    SplPE(bool standalone = false);

    /// Constructor
    /// @param standalone true if the PE is in standalone mode, false otherwise
    /// @param javaVMArgs NULL if no Java required; otherwise, JVM arguments
    //        PE(bool standalone, std::vector<std::string> * javaVMArgs);

    /// Destructor
    virtual ~SplPE();

    virtual void toolkitsWithResources(std::vector<std::string>& toolkits) {}

    BasePEImpl& getImpl();

    // Only used when submitting pre 4.2 apps on a 4.2 or greater release.
    //        std::vector<std::string> * getJavaVMArgs() { return javaVMArgs_; }

  private:
    std::auto_ptr<PEImpl> impl_;
    //        std::vector<std::string> * javaVMArgs_;
};
};

#endif /* SPL_RUNTIME_PROCESSING_ELEMENT_PE_H */
