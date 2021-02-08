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

#include <K8S/K8STypes.h>

K8S_NAMESPACE_BEGIN

IMPL_EXCEPTION(K8S_NAMESPACE, K8S, UTILS_NAMESPACE::Distillery);

IMPL_EXCEPTION(K8S_NAMESPACE, K8SIO, K8S);
IMPL_EXCEPTION(K8S_NAMESPACE, K8SIOServiceUnreachable, K8SIO);

IMPL_EXCEPTION(K8S_NAMESPACE, K8SMetrics, K8S);
IMPL_EXCEPTION(K8S_NAMESPACE, K8SMetricsUnknownMetric, K8SMetrics);

IMPL_EXCEPTION(K8S_NAMESPACE, K8SPlatform, K8S);
IMPL_EXCEPTION(K8S_NAMESPACE, K8SPlatformExportHasNoProperty, K8SPlatform);
IMPL_EXCEPTION(K8S_NAMESPACE, K8SPlatformExportNotFound, K8SPlatform);
IMPL_EXCEPTION(K8S_NAMESPACE, K8SPlatformExportPropertyNotFound, K8SPlatform);
IMPL_EXCEPTION(K8S_NAMESPACE, K8SPlatformImportFilterNotFound, K8SPlatform);
IMPL_EXCEPTION(K8S_NAMESPACE, K8SPlatformImportHasNoSubscription, K8SPlatform);
IMPL_EXCEPTION(K8S_NAMESPACE, K8SPlatformAppConfigNotFound, K8SPlatform);
IMPL_EXCEPTION(K8S_NAMESPACE, K8SPlatformInvalidAppConfig, K8SPlatform);

K8S_NAMESPACE_END
