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

#ifndef DISTILLERY_SERVICE_CLIENT_H
#define DISTILLERY_SERVICE_CLIENT_H

#include <string>

#include <UTILS/UTILSTypes.h>

UTILS_NAMESPACE_BEGIN

class DistilleryServiceClient
{
  protected:
    std::string _service; // service persistence name
    std::string _timeoutKey;
    int _timeout;

    std::string _serviceHost;            // host that the active service running on
    std::string _serviceStateLeaderPath; // active service path

    DistilleryServiceClient();
    virtual ~DistilleryServiceClient() {}

    /// Set clientConnection flag to false with the service state path in ZooKeeper.
    virtual void setClientConnectionFalse();

    /// Check clientConnection flag setting.
    virtual bool checkClientConnection();

    /// Note: better not use interface IPathDataWatcher, since service path may change.
    /// Wait till service become active again.
    /// @param seconds maximum waiting time in seconds.
    /// @return true if an active service available; false otherwise.
    virtual bool waitForServiceActive(int seconds = 0);

    virtual int getServiceStartTimeout();
};

UTILS_NAMESPACE_END

#endif
