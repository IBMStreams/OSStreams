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

#include <UTILS/SPCException.h>

UTILS_NAMESPACE_USE
SPC_NAMESPACE_USE

IMPL_EXCEPTION(SPC, SPC, UTILS_NAMESPACE::Distillery);
IMPL_EXCEPTION(SPC, NC, SPC);
IMPL_EXCEPTION(SPC, InvalidPEID, SPC);
IMPL_EXCEPTION(SPC, InvalidPortID, SPC);
IMPL_EXCEPTION(SPC, ParseError, SPC);
IMPL_EXCEPTION(SPC, InternalError, SPC);
IMPL_EXCEPTION(SPC, ApplicationBundleFailure, SPC);
