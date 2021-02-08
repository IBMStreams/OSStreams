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

#ifndef SPL_RUNTIME_OPERATOR_WATERMARKHANDLER_H
#define SPL_RUNTIME_OPERATOR_WATERMARKHANDLER_H

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Operator/Port/Punctuation.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif

namespace SPL {
/**
 * @brief Interface implemented by operators which should be notified on
 * watermark advances.
 */
class DLL_PUBLIC WatermarkHandler
{
  public:
    /**
     * @brief This function is called by the runtime when the watermark advances
     * on an input port.
     * @param wm the watermark punctuation containing the watermark update
     * @param port index of the input port from which the watermark is
     * received
     */
    virtual void processWatermark(Punctuation const& wm, uint32_t port) = 0;

    virtual ~WatermarkHandler() {}
};
}
#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_RUNTIME_OPERATOR_WATERMARKHANDLER_H
