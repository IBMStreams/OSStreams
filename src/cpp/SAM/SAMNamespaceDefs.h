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

#ifndef SAMNAMESPACEDEFS_H
#define SAMNAMESPACEDEFS_H

#include <SAM/SAMForwardDeclXSDTypes.h>

#define SPC_NAMESPACE Streams
#define SPC_NAMESPACE_BEGIN namespace SPC_NAMESPACE {
#define SPC_NAMESPACE_END }
#define SPC_NAMESPACE_USE using namespace SPC_NAMESPACE;
#define SPC_NAMESPACE_QUALIFIER SPC_NAMESPACE::

#define SAM_NAMESPACE Streams
#define SAM_NAMESPACE_BEGIN namespace SAM_NAMESPACE {
#define SAM_NAMESPACE_END }
#define SAM_NAMESPACE_USE using namespace SAM_NAMESPACE;

#define SAM_RUNTIME_NAMESPACE_BEGIN                                                                \
    namespace xmlns {                                                                              \
    namespace prod {                                                                               \
    namespace streams {                                                                            \
    namespace runtime {                                                                            \
    namespace sam {
#define SAM_RUNTIME_NAMESPACE_END                                                                  \
    }                                                                                              \
    }                                                                                              \
    }                                                                                              \
    }                                                                                              \
    }

namespace ADL = ::xmlns::prod::streams::application;
namespace PADL = ::xmlns::prod::streams::application::physical;
namespace AADL = ::xmlns::prod::streams::application::augmented;
namespace NAADL = ::xmlns::prod::streams::application::augmented::normalized;
namespace SAM_INTERFACE = ::xmlns::prod::streams::runtime::sam;

#endif
