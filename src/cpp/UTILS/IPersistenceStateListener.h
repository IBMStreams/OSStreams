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

#ifndef IPERSISTENCESTATELISTENER_H_
#define IPERSISTENCESTATELISTENER_H_

#include <UTILS/UTILSTypes.h>
#include <boost/shared_ptr.hpp>

UTILS_NAMESPACE_BEGIN

class IPersistenceStateListener
{
  public:
    IPersistenceStateListener() {}

    virtual ~IPersistenceStateListener() {}

    /**
     * Invoked when persistence connection state of ITreePersistence changes
     *
     * @param state - (ITreePersistence::CONNECTED_STATE, ITreePersistence::DISCONNECTED_STATE)
     */
    virtual void persistenceStateChanged(int state) = 0;
};

UTILS_NAMESPACE_END

#endif /* IPERSISTENCESTATELISTENER_H_ */
