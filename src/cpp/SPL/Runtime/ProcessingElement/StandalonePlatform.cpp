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

#include <SPL/Runtime/Common/RuntimeDebugAspect.h>
#include <SPL/Runtime/ProcessingElement/StandalonePlatform.h>
#include <TRC/ConsoleTracer.h>
#include <TRC/DistilleryDebug.h>
#include <UTILS/SupportFunctions.h>

#include <pwd.h>

using namespace SPL;
using namespace std;
using namespace Distillery;

StandalonePlatform::StandalonePlatform()
{
    initEnvironment();
}

std::string const StandalonePlatform::getUserName() const
{
    return userId_;
}

std::string const StandalonePlatform::getUserSessionId() const
{
    APPTRC(L_DEBUG, "Acquiring session for " << userId_, SPL_APP_DBG);
    return userId_ + "_session";
}

void StandalonePlatform::closeUserSession(const std::string& sessionToken)
{
    APPTRC(L_DEBUG, "Closing session " << sessionToken, SPL_APP_DBG);
}

void StandalonePlatform::initEnvironment()
{
    setenv(SystemStatics::get_STREAMS_NS_envVar().c_str(), "FS:", 0);

    if (!getenv(SystemStatics::get_STREAMS_INSTANCE_ID_envVar().c_str())) {
        long usernameLength = sysconf(_SC_GETPW_R_SIZE_MAX);

        struct passwd mypwent;
        memset(&mypwent, 0, sizeof(mypwent));

        struct passwd* mypwent_p = NULL;
        char* pwentbuf = new char[usernameLength + 1];
        memset(pwentbuf, 0, usernameLength + 1);

        if (getpwuid_r(getuid(), &mypwent, pwentbuf, usernameLength, &mypwent_p) == 0) {
            if (NULL == mypwent_p) {
                userId_ = "user";
                ostringstream s;
                s << getuid();
                userId_ += s.str();
                APPTRC(L_INFO, "Unable to get user id from uid. Setting it to " << userId_,
                       SPL_APP_DBG);
            } else {
                userId_ = mypwent_p->pw_name;
            }
            setenv(SystemStatics::get_STREAMS_INSTANCE_ID_envVar().c_str(), userId_.c_str(), 0);
            APPTRC(L_INFO,
                   "Environment variable " + SystemStatics::get_STREAMS_INSTANCE_ID_envVar() +
                       " is not set, setting it to "
                     << userId_,
                   SPL_APP_DBG);
        } else {
            APPTRC(L_INFO,
                   "Environment variable " + SystemStatics::get_STREAMS_INSTANCE_ID_envVar() +
                     " is not set and an error occurred retrieving user name",
                   SPL_APP_DBG);
        }

        delete[] pwentbuf;
    }
}

std::string const StandalonePlatform::getCheckpointRepository() const
{
    return "FS";
}

std::string const StandalonePlatform::getCheckpointRepositoryConfiguration() const
{
    return "";
}

void StandalonePlatform::registerOperatorIGC(const map<string, string>& nameValuePairs) const {}

void StandalonePlatform::addStreamProperties(uint32_t opid,
                                             const ADL::streamPropertiesType& properties)
{}
void StandalonePlatform::addStreamProperty(uint32_t opid, const ADL::streamPropertyType& property)
{}

void StandalonePlatform::getStreamProperties(
  uint32_t opid,
  const SAM_INTERFACE::streamPropertyNamesType& propertyNames,
  ADL::streamPropertiesType& properties)
{}
void StandalonePlatform::getStreamProperty(
  uint32_t opid,
  const SAM_INTERFACE::streamPropertyNameType& propertyName,
  ADL::streamPropertyType& property)
{}

void StandalonePlatform::removeStreamProperties(
  uint32_t opid,
  const SAM_INTERFACE::streamPropertyNamesType& propertyNames)
{}
void StandalonePlatform::removeStreamProperty(
  uint32_t opid,
  const SAM_INTERFACE::streamPropertyNameType& propertyName)
{}

void StandalonePlatform::setStreamProperties(uint32_t opid,
                                             const ADL::streamPropertiesType& properties)
{}
void StandalonePlatform::setStreamProperty(uint32_t opid, const ADL::streamPropertyType& property)
{}

void StandalonePlatform::getFilter(uint32_t ipid, std::string& filterExpression) {}
void StandalonePlatform::setFilter(uint32_t ipid, const std::string& filterExpression) {}

void StandalonePlatform::getSubscription(uint32_t ipid, ADL::importedStreamsType& importedStreams)
{}
void StandalonePlatform::setSubscription(uint32_t ipid,
                                         const ADL::importedStreamsType& importedStreams)
{}

void StandalonePlatform::addCustomMetric(const string& name,
                                         const string& kind,
                                         const string& desc,
                                         const string& operName)
{}

void StandalonePlatform::getAppConfig(const string& aasToken,
                                      const string& cfgName,
                                      map<string, string>& properties)
{}

void StandalonePlatform::getRestrictedConfig(const vector<string>& cfgNames,
                                             map<string, string>& properties)
{}

std::string StandalonePlatform::getTransportSecurityType() const
{
    return "none";
}

void StandalonePlatform::checkpointCompleted(const int32_t regionId, const int64_t seqId) {}

void StandalonePlatform::blockingCheckpointCompleted(const int32_t regionId, const int64_t seqId) {}

void StandalonePlatform::resetCompleted(const int32_t regionId,
                                        const int64_t seqId,
                                        const int32_t resetAttempt)
{}

void StandalonePlatform::drain(const int32_t regionId,
                               const std::string opName,
                               const int64_t seqId)
{}

void StandalonePlatform::reset(const int32_t regionId, const std::string opName) {}
