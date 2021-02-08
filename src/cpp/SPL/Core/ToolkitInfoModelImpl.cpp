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

#include <SPL/Core/CompilerError.h>
#include <SPL/Core/ToolkitInfoModelImpl.h>

using namespace std;
using namespace std::tr1;
using namespace SPL;
using namespace SPL::ToolkitInfo;
using boost::shared_ptr;

IMPL_EXCEPTION(SPL, SPLCompilerInvalidToolkitInfoModel, SPLCompiler)

ToolkitInfoModel::ToolkitInfoModel(const toolkitInfoModelType& xml)
  : name_(xml.identity().name())
  , description_(xml.identity().description())
  , version_(xml.identity().version())
  , requiredProductVersion_(xml.identity().requiredProductVersion())
{
    typedef dependenciesType::toolkit_sequence::const_iterator myiter;
    myiter itbeg = xml.dependencies().toolkit().begin();
    myiter itend = xml.dependencies().toolkit().end();
    for (myiter it = itbeg; it != itend; ++it) {
        DependencyPtr eptr(new Dependency(*it));
        dependencies_.push_back(eptr);
    }
    if (xml.resources().present()) {
        const resourcesType& resources = xml.resources().get();
        typedef resourcesType::messageSet_sequence::const_iterator msit;
        msit msitbeg = resources.messageSet().begin();
        msit msitend = resources.messageSet().end();
        for (msit it = msitbeg; it != msitend; ++it) {
            MessageSetPtr eptr(new MessageSet(*it));
            messageSets_.push_back(eptr);
        }
    }
    if (xml.sabFiles().present()) {
        const sabFilesType& sabFiles = xml.sabFiles().get();
        const sabFilesType::include_sequence& include = sabFiles.include();
        const sabFilesType::exclude_sequence& exclude = sabFiles.exclude();
        for (sabFilesType::include_const_iterator it = include.begin(), itEnd = include.end();
             it != itEnd; ++it) {
            const includeType& i = *it;
            RootType rt = ToolkitDir;
            if (i.root().present()) {
                const string& root = i.root().get();
                if ("applicationDir" == root)
                    rt = ApplicationDir;
                else if ("outputDir" == root)
                    rt = OutputDir;
                else {
                    assert(false);
                }
            }
            includes_.push_back(make_pair(i.path(), rt));
        }
        for (sabFilesType::exclude_const_iterator it1 = exclude.begin(), it1End = exclude.end();
             it1 != it1End; ++it1) {
            const excludeType& e = *it1;
            excludes_.push_back(e.path());
        }
    }
}

Dependency::Dependency(const toolkitDependencyType& xml)
  : name_(xml.name())
  , version_(xml.version())
{}

MessageSet::MessageSet(const messageSetType& xml)
  : name_(xml.name())
{
    typedef messageSetType::lang_const_iterator lit;
    lit itbeg = xml.lang().begin();
    lit itend = xml.lang().end();
    for (lit it = itbeg; it != itend; ++it) {
        langs_.push_back(*it);
        if (it->default_().present() && it->default_().get()) {
            if (defaultLang_.empty())
                defaultLang_ = *it;
            else {
                // Multiple defaults
                THROW(SPLCompilerInvalidToolkitInfoModel,
                      SPL_CORE_TKIMODEL_MULTIPLE_DEFAULT_LANGS(name_, defaultLang_, *it));
            }
        }
    }

    // Do we have a default language?
    if (defaultLang_.empty())
        THROW(SPLCompilerInvalidToolkitInfoModel, SPL_CORE_TKIMODEL_NO_DEFAULT_LANG(name_));
}
