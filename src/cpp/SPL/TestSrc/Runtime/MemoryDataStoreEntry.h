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

#ifndef TEST_RUNTIME_MEMORYDATASTOREENTRY_H_
#define TEST_RUNTIME_MEMORYDATASTOREENTRY_H_

#include <SPL/Runtime/Operator/State/DataStoreAdapter.h>
#include <SPL/Runtime/Operator/State/DataStoreEntryImpl.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>

#include <string>
#include <cassert>
#include <stdexcept>
#include <tr1/unordered_map>

namespace SPL {
/// @ingroup OperatorTest
/// @{
/**
 * Test @c DataStoreEntry backed by a std::unordered_map<string,string>
 *
 * There is no difference between batched and non-batched operations.
 */
class DLL_PUBLIC MemoryDataStoreEntry : public DataStoreEntryImpl
{
private:
    typedef std::tr1::unordered_map<std::string, std::string> EntryStore;
    typedef std::tr1::shared_ptr<EntryStore> EntryStorePtr;
    typedef std::tr1::unordered_map<std::string, EntryStorePtr> DataStore;

public:
    // Invoked by the Checkpoint destructor via DataStoreEntry
    ~MemoryDataStoreEntry()
    {   clear(); }

    /**
     * Put (write) a key-value pair to this Data Store Entry.  If the key does
     * not exist, add it.
     *
     * @param key key name
     * @param value pointer to the value buffer
     * @param size size of the value buffer in bytes
     * @param batch batch handle if the operation is in a batch; NULL if the operation is not in a batch
     */
    void put(const std::string & key, const char * value, const uint64_t & size,
            DataStoreUpdateBatchImpl * batch)
    {
        APPTRC(L_DEBUG, "Enter: key=" << key << " value=" << (void*)value << " size=" << size, SPL_CKPT);
        std::string s(value, size);
        entries_->operator[](key) = s;
    }

    /**
     * Get(read) the value of the given key from this Data Store Entry into a
     * dynamically allocated buffer.
     *
     * @param key key to get
     * @param value address of the value buffer
     * @param [out] size size of the value buffer
     * @param [out] isExisting returns true if the key was found, otherwise false.
     */
    void get(const std::string & key, char * & value, uint64_t & size, bool & isExisting)
    {
        APPTRC(L_TRACE, "Enter: key=" << key, SPL_CKPT);
        EntryStore::iterator it = entries_->find(key);
        if (it != entries_->end()) {
            std::string & v = it->second;
            size = v.length();
            value = new char[size];
            memcpy(value, v.c_str(), sizeof(char) * size);
            isExisting = true;
        }
        else {
            size = 0;
            isExisting = false;
        }
        APPTRC(L_DEBUG, "Exit: key=" << key << " value=" << (void*)value << " size=" << size <<
                " existing=" << isExisting, SPL_CKPT);
    }

    /**
     * Get(read) the value of the given key from this Data Store Entry into
     * a user-provided buffer.
     *
     * @param key key to get
     * @param value buffer which contains the retrieved data upon return
     * @param size size of the buffer pointed by value
     * @param [out] returnSize return size of retrieved value in char
     * @param [out] isExisting returns true if the key was found, otherwise false.
     */
    void get(const std::string & key, char * value, const uint64_t & size,
            uint64_t & returnSize, bool & isExisting)
    {
        if (value == NULL) {
            throw std::invalid_argument("value");
        }

        APPTRC(L_TRACE, "Enter: key=" << key << " value buffer=" << (void*)value << " size=" << size, SPL_CKPT);
        EntryStore::iterator it = entries_->find(key);
        if (it != entries_->end()) {
            std::string & v = it->second;
            returnSize = (v.length() <= size) ? v.length() : size;
            memcpy(value, v.c_str(), sizeof(char) * returnSize);
            isExisting = true;
        }
        else {
            returnSize = 0;
            isExisting = false;
        }
        APPTRC(L_DEBUG, "Exit: key=" << key << " returnSize=" << returnSize <<
                " existing=" << isExisting, SPL_CKPT);
    }

    /**
     * Get(read) all values of the given key from this Data Store Entry.
     * If the backend store uses replication and only offer weak consistency between replicas, then
     * this function is used to return all replicas to application and allow application to resolve
     * inconsistency between replicas.
     * @param key key to get
     * @param values return a vector of values, in which each value is a pair of memory address and
     *        size in Bytee; user is responsible for de-allocating the memory of each each by delete[]
     * @param isExisting return whether the key exists (true) or not (false)
     * @throws DataStoreException if the value referenced by the key cannot be retrieved from the Data Store Entry
     */
    void get(const std::string & key, std::vector<std::pair<char *, uint64_t> > & values, bool & isExisting)
    {
        std::pair<char *, uint64_t> pr;
        get(key, pr.first, pr.second, isExisting);
        if (isExisting == true) {
            values.push_back(pr);
        }
    }

    /**
     * Get the size limit of a key in Bytes
     */
    uint64_t getKeySizeLimit() const
    {
        return 1048576UL; // 1MB
    }

    /**
     * Get the size limit of a value in Bytes, which is the maximum size of a value
     * which can be written or read in a single put() or get() call.
     */
    uint64_t getValueSizeLimit() const
    {
        return 1073741824UL; // 1GB
    }

    /**
     * Get the maximum size which can be written in one single
     * operation (1 MByte).
     */
    uint32_t getDefaultChunkSize() const
    {   return 1000000UL; }

    /**
     * Delete the given key and its associate value from this
     * Data Store Entry.
     */
    void remove(const std::string & key, DataStoreUpdateBatchImpl * batch)
    {   entries_->erase(key); }

    /** Delete all key-value pairs in this Data Store Entry */
    void clear()
    {   entries_->clear(); }

    /**
     * Test if a key exists in this Data Store entry.
     * @param key the key to query
     * @return true if key exists, false otherwise.
     */
    bool isExistingKey(const std::string & key)
    {   return entries_->count(key) > 0; }

    static std::string toString(EntryStorePtr & entries) {
        std::stringstream ss;
        for(EntryStore::const_iterator it = entries->begin(); it != entries->end(); ++it) {
            ss << "<" << it->first << ", " << it->second.length() << ">, ";
        }
        return ss.str();
    }

    void loadEntries()
    {
        APPTRC(L_TRACE, "Enter", SPL_CKPT);
        DataStore::const_iterator it = store_.find(getName());
        if (it != store_.end()) {
            entries_ = it->second;
            APPTRC(L_DEBUG, "Found store entry " << it->first << " with " << entries_->size() << " entries: " <<
                    MemoryDataStoreEntry::toString(entries_), SPL_CKPT);
        }
    }

    friend class MemoryDataStoreAdapter;
private:
    MemoryDataStoreEntry(const std::string & name, DataStore & store) :
            DataStoreEntryImpl(name), entries_(new EntryStore()), store_(store)
    {
        // Clone the entry store
        EntryStorePtr clone(new EntryStore(*(entries_.get())));
        store_.insert(std::make_pair(getName(), clone));
    }

    EntryStorePtr entries_;
    DataStore & store_;

public:
    void getKeys(std::tr1::unordered_set<std::string> & keys)
    { throw std::logic_error("Not implemented"); }
};
/// @}
}

#endif /* TEST_RUNTIME_MEMORYDATASTOREENTRY_H_ */
