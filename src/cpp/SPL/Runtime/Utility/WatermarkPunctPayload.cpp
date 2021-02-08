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

#include <Runtime/Utility/WatermarkPunctPayload.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>

#include <string>

using namespace SPL;

const std::string WatermarkPunctPayload::name("wm");

WatermarkPunctPayload* WatermarkPunctPayload::getPayload(Punctuation const& punct)
{
    assert(punct.getPayloadContainer() != NULL);
    WatermarkPunctPayload* payload = dynamic_cast<WatermarkPunctPayload*>(
      punct.getPayloadContainer()->find(WatermarkPunctPayload::name));
    return payload;
}
