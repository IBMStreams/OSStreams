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

#include <SPL/Runtime/ProcessingElement/ViewProperties.h>
#include <stdlib.h>

using namespace SPL;
using namespace std;

ViewProperties::ViewProperties(uint64_t viewId,
                               OperatorImpl& op,
                               uint32_t port,
                               const vector<Streams::nameValuePair_t>& value)
  : _viewId(viewId)
  , _op(op)
  , _port(port)
  , _rate(10)
{
    update(value);
}

void ViewProperties::update(const vector<Streams::nameValuePair_t>& value)
{
    for (vector<Streams::nameValuePair_t>::const_iterator it = value.begin(); it != value.end();
         it++) {
        if (it->first == "rate") {
            uint32_t rate = strtoul(it->second[0].c_str(), NULL, 10);
            // Max out at 100 per second per port
            _rate = rate > MaxRate ? MaxRate : rate;
        }
    }
}
