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

#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Common/RuntimeExceptionImpl.h>
#include <UTILS/BacktraceDumper.h>

using namespace SPL;

IMPL_EXCEPTION(SPL, SPLRuntime, Distillery::Distillery)

IMPL_EXCEPTION(SPL, SPLRuntimeBlobNotOwned, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeAssertionFailed, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeDeserialization, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeInvalidArgument, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeInvalidOperation, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeInvalidCheckpoint, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeInvalidIndex, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeInvalidMetric, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeInvalidOperator, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeSerialization, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeTypeMismatch, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeInvalidSlidingWindow, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeInvalidTumblingWindow, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeUnableToLoadADL, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeUnableToParseAADL, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeUnableToCreateAADL, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeImport, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeStreamProperty, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeStreamPropertyNotFound, SPLRuntimeStreamProperty)
IMPL_EXCEPTION(SPL, SPLRuntimeSubscription, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeMissingSubmissionValue, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeSubscriptionNoSubscriptWithIn, SPLRuntimeSubscription)
IMPL_EXCEPTION(SPL, SPLRuntimeSubscriptionInvalidClause, SPLRuntimeSubscription)
IMPL_EXCEPTION(SPL, SPLRuntimeSubscriptionInvalidLiteral, SPLRuntimeSubscription)
IMPL_EXCEPTION(SPL, SPLRuntimeFilter, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeFilterInvalidClause, SPLRuntimeFilter)
IMPL_EXCEPTION(SPL, SPLRuntimeFilterInvalidLiteral, SPLRuntimeFilter)
IMPL_EXCEPTION(SPL, SPLRuntimeFilterNoSubscriptWithIn, SPLRuntimeFilter)
IMPL_EXCEPTION(SPL, SPLRuntimeFilterNoAttribute, SPLRuntimeFilter)
IMPL_EXCEPTION(SPL, SPLRuntimeFilterInvalidAttributeType, SPLRuntimeFilter)
IMPL_EXCEPTION(SPL, SPLRuntimeFilterTypeMismatch, SPLRuntimeFilter)
IMPL_EXCEPTION(SPL, SPLRuntimeFilterModNotInt64, SPLRuntimeFilter)
IMPL_EXCEPTION(SPL, SPLRuntimeFilterNegativeSubscript, SPLRuntimeFilter)
IMPL_EXCEPTION(SPL, SPLRuntimeFilterValidationFailed, SPLRuntimeFilter)
IMPL_EXCEPTION(SPL, SPLRuntimeFilterParseFailed, SPLRuntimeFilter)
IMPL_EXCEPTION(SPL, SPLRuntimeMissingArgument, SPLRuntimeSubscription)
IMPL_EXCEPTION(SPL, SPLRuntimePayload, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeFailedOperators, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeInternalError, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeDebuggerError, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeXMLParsing, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeInvalidXML, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeOperator, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeMissingDataDirectory, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeNullValueAccess, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeUnsupportedSubstitutionType, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeJNI, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeIO, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLRuntimeShutdown, SPLRuntime)

IMPL_EXCEPTION(SPL, SPLControl, SPLRuntime)
IMPL_EXCEPTION(SPL, SPLListenerNotFound, SPLControl)
IMPL_EXCEPTION(SPL, SPLControlBean, SPLControl)
IMPL_EXCEPTION(SPL, SPLControlBeanNotFound, SPLControl)
IMPL_EXCEPTION(SPL, SPLRuntimeGetEventTimeMissingEventTimeAttribute, SPLRuntime);

int BacktraceDumper::dump(std::ostream& s)
{
    return Distillery::BacktraceDumper::dump(s, true, 1);
}
