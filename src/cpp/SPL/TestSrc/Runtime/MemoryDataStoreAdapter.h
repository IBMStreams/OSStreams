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

#ifndef TEST_RUNTIME_MEMORYDATASTOREDAPTER_H_
#define TEST_RUNTIME_MEMORYDATASTOREDAPTER_H_

#include <SPL/Runtime/Operator/State/DataStoreAdapter.h>
#include <SPL/TestSrc/Runtime/MemoryDataStoreEntry.h>
#include <SPL/Runtime/Utility/Visibility.h>

#include <string>
#include <stdexcept>
#include <vector>
#include <tr1/unordered_map>
#include <tr1/memory>

namespace SPL {
/**
 * @internal @file MemoryDataStoreAdapter.h
 * @brief Definition of the SPL::MemoryDataStoreAdapter class.
 */
/// @ingroup OperatorTest
/// @{

/**
 * Simple @c DataStoreAdapter for in-memory checkpointing data stores, used
 * for testing.
 */
class DLL_PUBLIC MemoryDataStoreAdapter : public DataStoreAdapter
{
public:
    MemoryDataStoreAdapter() : store_() {}
    ~MemoryDataStoreAdapter() {}

    std::string getDSAType()
    { return "Memory"; }

    /// Options are ignored.
    DataStoreEntry * getDataStoreEntry(const std::string & name, const Option & option)
    {
        MemoryDataStoreEntry* impl = new MemoryDataStoreEntry(name, store_);
        if (isExistingDataStoreEntry(name)) {
            try {
                impl->loadEntries();
            }
            catch (...) {
                delete impl;
                throw;
            }
        }

        APPTRC(L_TRACE, "new MemoryDataStoreEntry = " << (void*)impl << " for name " << name, SPL_CKPT);
        std::stringstream s;
        s << "MemoryDataStoreEntry::DataStore: size = " << store_.size() << std::endl;
        for (MemoryDataStoreEntry::DataStore::const_iterator it = store_.begin(); it != store_.end(); it++) {
            s << "    name=" << it->first << ", store=" << (void*)(it->second.get()) << std::endl;
        }
        APPTRC(L_TRACE, s.str(), SPL_CKPT);

        DataStoreEntry* e = new DataStoreEntry(impl);
        entries_.push_back(std::tr1::shared_ptr<DataStoreEntry>(e));
        return e;
    }

    void removeDataStoreEntry(const std::string & name)
    {   store_.erase(name); }

    bool isExistingDataStoreEntry(const std::string & name)
    {
        size_t count = store_.count(name);
        APPTRC(L_TRACE, "store_ contains " << count << " entries with name " << name, SPL_CKPT);
        return (store_.count(name) > 0);
    }

    // ////////// DataStoreEntrySet not supported ////////////////////////////////

    void getDataStoreEntryNames(const std::string & prefix, std::tr1::unordered_set<std::string> & names)
    { throw std::logic_error("Not implemented"); }

    void removeDataStoreEntries(const std::string & prefix)
    { throw std::logic_error("Not implemented"); }

private:
    std::vector<std::tr1::shared_ptr<DataStoreEntry> > entries_;
    MemoryDataStoreEntry::DataStore store_;
};
/// @}
}

#endif /* TEST_RUNTIME_MEMORYDATASTOREDAPTER_H_ */
