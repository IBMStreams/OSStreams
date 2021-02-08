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

/*
 * Implementation of SPL::CheckpointContext class
 */
#include <SPL/Runtime/Operator/State/CheckpointContext.h>
#include <SPL/Runtime/Operator/State/CheckpointContextImpl.h>

using namespace SPL;
using namespace std;

CheckpointContext::CheckpointContext(const enum Kind& kind,
                                     const double& ckptInterval,
                                     CheckpointContextImpl* impl)
  : name_(CHECKPOINT)
  , kind_(kind)
  , ckptInterval_(ckptInterval)
  , impl_(impl)
{}

bool CheckpointContext::createCheckpoint()
{
    return impl_->createCheckpoint();
}

bool CheckpointContext::isEnabled() const
{
    return impl_->isEnabled();
}

CheckpointContextImpl* CheckpointContext::getImpl() const
{
    return impl_.get();
}
