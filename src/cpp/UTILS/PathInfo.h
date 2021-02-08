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

#ifndef PATHINFO_H_
#define PATHINFO_H_

#include <UTILS/UTILSTypes.h>
#include <boost/shared_ptr.hpp>
#include <string>

UTILS_NAMESPACE_BEGIN

class PathInfo
{
  public:
    PathInfo()
      : _path("")
      , _createTime(0)
      , _modifiedTime(0)
      , _isEphemeral(false)
      , _dataVersion(0)
      , _dataLength(0)
      , _childrenVersion(0)
      , _numChildren()
    {}

    PathInfo(const std::string& path)
      : _path(path)
      , _createTime(0)
      , _modifiedTime(0)
      , _isEphemeral(false)
      , _dataVersion(0)
      , _dataLength(0)
      , _childrenVersion(0)
      , _numChildren(0)
    {}

    PathInfo(const std::string& path,
             int64_t createTime,
             int64_t modifiedTime,
             bool ephemeral,
             int dataVersion,
             int dataLength,
             int childrenVersion,
             int numChildren)
      : _path(path)
      , _createTime(createTime)
      , _modifiedTime(modifiedTime)
      , _isEphemeral(ephemeral)
      , _dataVersion(dataVersion)
      , _dataLength(dataLength)
      , _childrenVersion(childrenVersion)
      , _numChildren(numChildren)
    {}

    PathInfo(const PathInfo& pathInfo)
      : _path(pathInfo._path)
      , _createTime(pathInfo._createTime)
      , _modifiedTime(pathInfo._modifiedTime)
      , _isEphemeral(pathInfo._isEphemeral)
      , _dataVersion(pathInfo._dataVersion)
      , _dataLength(pathInfo._dataLength)
      , _childrenVersion(pathInfo._childrenVersion)
      , _numChildren(pathInfo._numChildren)
    {}

    inline const std::string& getPath() const { return _path; }

    inline void setPath(const std::string& path) { _path = path; }

    inline int64_t getCreateTime() const { return _createTime; }

    inline void setCreateTime(int64_t createTime) { _createTime = createTime; }

    inline int64_t getModifiedTime() const { return _modifiedTime; }

    inline void setModifiedTime(int64_t modifiedTime) { _modifiedTime = modifiedTime; }

    inline bool isEphemeral() const { return _isEphemeral; }

    inline void setEphemeral(bool ephemeral) { _isEphemeral = ephemeral; }

    inline int32_t getDataVersion() const { return _dataVersion; }

    inline void incrementDataVersion() { ++_dataVersion; }

    inline void setDataVersion(int32_t dataVersion) { _dataVersion = dataVersion; }

    inline int32_t getDataLength() const { return _dataLength; }

    inline void setDataLength(int32_t dataLength) { _dataLength = dataLength; }

    inline int32_t getChildrenVersion() const { return _childrenVersion; }

    inline void incrementChildrenVersion() { ++_childrenVersion; }

    inline void setChildrenVersion(int32_t childrenVersion) { _childrenVersion = childrenVersion; }

    inline int32_t getNumChildren() const { return _numChildren; }

    inline void setNumChildren(int32_t numChildren) { _numChildren = numChildren; }

    inline void incrementNumChildren() { ++_numChildren; }

    inline void decrementNumChildren() { --_numChildren; }

  private:
    std::string _path;
    int64_t _createTime;
    int64_t _modifiedTime;
    bool _isEphemeral;
    int32_t _dataVersion;
    int32_t _dataLength;
    int32_t _childrenVersion;
    int32_t _numChildren;
};

typedef boost::shared_ptr<PathInfo> PathInfoPtr;

UTILS_NAMESPACE_END

#endif /* PATHINFO_H_ */
