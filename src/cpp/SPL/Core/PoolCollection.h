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

#ifndef POOL_COLLECTION_H
#define POOL_COLLECTION_H

#include <SPL/Runtime/Utility/Visibility.h>
#include <UTILS/HashMapHelpers.h>
#include <iostream>
#include <string>
#include <vector>

namespace SPL {
class SourceLocation;

//! Pool Collection
class DLL_PUBLIC PoolCollection
{
  public:
    static uint32_t defaultPoolIndex;   /*!< index of the default pool */
    static std::string defaultPoolName; /*!< name of the default pool */

    /// Constructor
    /// @param withDefault adds the default pool as an unsized pool
    /// if set to true
    PoolCollection(bool withDefault);

    /// Enum PoolKind
    /// Records pool information
    enum PoolKind
    {
        ExplicitPool, /*!< pool that is fully specified */
        ImplicitPool  /*!< pool to be filled in by runtime */
    };

    /// Descriptor for a single pool
    class OnePool
    {
      public:
        /// Create an explicit pool
        /// @param sloc Source location of pool config
        /// @param name Pool name
        /// @param size Pool size
        /// @param hosts List of host names
        OnePool(SourceLocation const& sloc,
                const std::string& name,
                uint32_t size,
                const std::vector<std::string>& hosts)
          : _sloc(&sloc)
          , _name(name)
          , _kind(ExplicitPool)
          , _size(size)
          , _hasSize(true)
          , _hosts(hosts)
          , _exclusive(false)
        {}

        /// Create a sized, implicit pool
        /// @param sloc Source location of pool config
        /// @param name Pool name
        /// @param size Pool size
        /// @param hasSize 'true' if size is set
        /// @param tags List of tags
        /// @param exclusive 'true' if pool is not to be shared
        OnePool(SourceLocation const& sloc,
                const std::string& name,
                uint32_t size,
                bool hasSizeIn,
                const std::vector<std::string>& tags,
                bool exclusive)
          : _sloc(&sloc)
          , _name(name)
          , _kind(ImplicitPool)
          , _size(size)
          , _hasSize(hasSizeIn)
          , _tags(tags)
          , _exclusive(exclusive)
        {}

        /// Copy Constructor
        /// @param rhs source to copy
        OnePool(const OnePool& rhs)
          : _sloc(rhs._sloc)
          , _name(rhs._name)
          , _kind(rhs._kind)
          , _size(rhs._size)
          , _hasSize(rhs._hasSize)
          , _hosts(rhs._hosts)
          , _tags(rhs._tags)
          , _exclusive(rhs._exclusive)
        {}

        /// Return Pool kind
        /// @return kind
        PoolKind getKind() const { return _kind; }

        /// Return Pool name
        /// @return name
        const std::string& getName() const { return _name; }

        /// Set Pool size
        /// @return size
        void setSize(uint32_t size)
        {
            _size = size;
            _hasSize = size != 0;
        }

        /// Return Pool size
        /// @return size
        uint32_t getSize() const { return _size; }

        /// Is this a sized pool?
        /// @return 'true' if the size was explicitly set
        bool hasSize() const { return _hasSize; }

        /// Is this an exclusive pool?
        /// @return 'true' if the hosts in the pool cannot be shared
        bool getExclusive() const { return _exclusive; }

        /// Return Pool location
        /// @return source location for pool
        const SourceLocation& getLocation() const { return *_sloc; }

        /// Set hosts in pool
        /// @return list of hostnames
        void setHosts(const std::vector<std::string>& hosts) { _hosts = hosts; }

        /// Return hosts in pool
        /// @return list of hostnames
        const std::vector<std::string>& getHosts() const { return _hosts; }

        /// Return hosts in pool
        /// @return list of hostnames
        std::vector<std::string>& getHosts() { return _hosts; }

        /// Return tags in pool
        /// @return list of tag names
        const std::vector<std::string>& getTags() const { return _tags; }

        /// Return tags in pool
        /// @return list of tag names
        std::vector<std::string>& getTags() { return _tags; }

      private:
        SourceLocation const* _sloc;
        std::string _name;
        PoolKind _kind;
        uint32_t _size;
        bool _hasSize;
        std::vector<std::string> _hosts;
        std::vector<std::string> _tags;
        bool _exclusive;
    };
    /// Return the number of pools we are managing
    /// @return the number of pools
    uint32_t numPools() const { return _pools.size(); }

    /// Return information on one pool
    /// @return OnePool
    const OnePool& getPool(uint32_t i) const;

    /// Return information on one pool
    /// @return OnePool
    OnePool& getPool(uint32_t i);

    /// Add a new explicit pool
    /// @param name         Pool Name
    /// @param hosts        vector of hostname strings
    /// @param sloc         Source location
    /// @return pool number
    uint32_t addExplicitPool(const std::string& name,
                             const std::vector<std::string>& hosts,
                             SourceLocation const& sloc);

    /// Add a new implicit, tagged/sized pool
    /// @param name Pool name
    /// @param size Pool size
    /// @param hasSize 'true' if size is set
    /// @param tags List of tags
    /// @param exclusive 'true' if pool is not to be shared
    /// @param sloc Source location of pool config
    uint32_t addImplicitPool(const std::string& name,
                             uint32_t size,
                             bool hasSize,
                             const std::vector<std::string>& tags,
                             bool exclusive,
                             SourceLocation const& sloc);

    /// Return the index of the pool with name n
    /// @param n pool name
    /// @return pool index
    uint32_t getIndex(const std::string& n) const;

    /// Return the name of pool n
    /// @param index    pool number
    /// @return pool name (string)
    std::string getName(uint32_t index) const;

    /// Return the kind of pool n
    /// @param index    pool number
    /// @return pool kind
    PoolKind getKind(uint32_t index) const;

    /// Return the size of the pool
    /// @param index pool index
    /// @return pool size
    uint32_t getSize(uint32_t index) const;

    /// Does the pool have an explicit size?
    /// @param index pool index
    /// @return 'true' if the pool has an explicit size
    bool hasSize(uint32_t index) const;

    /// Is this an exclusive pool?
    /// @param index pool index
    /// @return 'true' if the pool is exclusive
    bool getExclusive(uint32_t index) const;

    /// Return the source location of the pool
    /// @param index pool index
    /// @return source location
    SourceLocation const& getLocation(uint32_t index) const;

    /// Return the hosts in the pool
    /// @param index pool index
    /// @return vector of host strings
    const std::vector<std::string>& getHosts(uint32_t index) const;

    /// Return the tags in the pool
    /// @param index pool index
    /// @return vector of tags names
    const std::vector<std::string>& getTags(uint32_t index) const;

    /// Replace a host in a given pool
    /// @param pindex pool index
    /// @param hindex host index
    /// @param host host name to use
    void replaceHost(uint32_t pindex, uint32_t hindex, std::string const& host);

    /// Set the default pool size
    /// @param size default pool size
    void setDefaultPoolSize(uint32_t size);

    /// Print all the pools
    /// @param str Output Stream
    /// @return Output Stream
    std::ostream& print(std::ostream& str) const;

  private:
    std::vector<OnePool> _pools;
    std::tr1::unordered_map<std::string, uint32_t> _nameToIndex;
    friend class PoolPopulator;
};
};

#endif /* POOL_COLLECTION_H */
