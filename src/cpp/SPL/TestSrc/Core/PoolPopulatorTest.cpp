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

#include "../Utility/UtlTestCommon.h"
#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerException.h>
#include <SPL/Core/PoolCollection.h>
#include <SPL/Core/PoolPopulator.h>
#include <SPL/Utility/SourceLocation.h>

using namespace std;
using namespace SPL;

namespace SPL {
class PoolPopulatorTest : public CompilerApp
{
  public:
    PoolPopulatorTest() {}

    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        uint32_t dsize = 2;
        _config->setIntValue(CompilerConfiguration::DefaultPoolSize, dsize);

        PoolCollection pools(true);

        vector<string> expph;
        expph.push_back("localhost");
        pools.addExplicitPool("exp", expph, loc_);

        vector<string> tags;
        pools.addImplicitPool("noTagsNoSize", 0, false, tags, false, loc_);
        pools.addImplicitPool("noTagsSize", 100, true, tags, false, loc_);
        pools.addImplicitPool("noTagsSizeExclusive", 1, true, tags, true, loc_);
        tags.push_back("first_tag");
        tags.push_back("another_tag");
        pools.addImplicitPool("tagsSizeExclusive", 2, true, tags, true, loc_);
        pools.addImplicitPool("tagsSizeShared", 2, true, tags, false, loc_);
        pools.addImplicitPool("tagsNoSizeShared", 0, false, tags, false, loc_);

        PoolPopulator populator;
        populator.populate(pools);
        pools.print(cerr);
        FASSERT(pools.getSize(PoolCollection::defaultPoolIndex) == dsize);
        FASSERT(!pools.hasSize(pools.getIndex("noTagsNoSize")));
        FASSERT(pools.hasSize(pools.getIndex("noTagsSize")));
        FASSERT(pools.getSize(pools.getIndex("noTagsSize")) == 100);
        FASSERT(pools.getExclusive(pools.getIndex("tagsSizeExclusive")));
        FASSERT(pools.getTags(pools.getIndex("tagsSizeExclusive")) == tags);

        string thishost = populator.convertToCanonicalName("localhost");
        cerr << thishost << endl;
        FASSERT(thishost != "localhost");

        return EXIT_SUCCESS;
    }

  private:
    SourceLocation loc_;
};
};

MAIN_APP(PoolPopulatorTest)
