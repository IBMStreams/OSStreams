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
 * SAMXmlHelper.h
 *
 */

#ifndef SAMHELPERFUNCTIONS_H
#define SAMHELPERFUNCTIONS_H

#include <SAM/SAMForwardDeclXSDTypes.h>
#include <SAM/SAMNamespaceDefs.h>
#include <SAM/SAMTypes.h>
#include <UTILS/HashMapHelpers.h>
#include <string>
#include <utility>

SAM_NAMESPACE_BEGIN

/// Convert an applicationSetType object to its corresponding XML string
/// @param appset the object
std::string toString(const ADL::applicationSetType& appset);

/// Create an applicationSetType object from an XML representation string
/// @param appset the object
/// @param s the XML string representation
void fromString(ADL::applicationSetType& appset, const std::string& s);

/// Create an applicationSetType object from an XML representation stream
/// @param appset the object
/// @param s the XML stream representation
void fromString(ADL::applicationSetType& appset, std::istream& s);

/// Convert an applicationType object to its corresponding XML string
/// @param app the object
std::string toString(const ADL::applicationType& app);

/// Create an applicationType object from an XML representation string
/// @param appSet the object
/// @param s the XML string representation
void fromString(ADL::applicationType& app, const std::string& s);

/// Convert an augmentedApplicationSetType object to its corresponding XML
/// string
/// @param appset the object
std::string toString(const AADL::augmentedApplicationSetType& appset);

/// Create an augmentedApplicationSetType object from an XML representation
/// string
/// @param appset the object
/// @param s the XML string representation
void fromString(AADL::augmentedApplicationSetType& appset, const std::string& s);

/// Convert an augmentedPEInputPortType object to its corresponding XML string
/// @param augip the object
std::string toString(const AADL::augmentedPEInputPortType& augip);

/// Convert an augmentedPEOutputPortType object to its corresponding XML string
/// @param augop the object
std::string toString(const AADL::augmentedPEOutputPortType& augop);

/// Convert an augmentedPEType object to its corresponding XML string
/// @param augpe the object
std::string toString(const AADL::augmentedPEType& augpe);

/// Create an augmentedPEType object from an XML representation string
/// @param augpe the object
/// @param s the XML string representation
void fromString(AADL::augmentedPEType& augpe, const std::string& s);

/// Convert an managedPEType object to its corresponding XML string
/// @param augpe the object
std::string toString(const AADL::managedPEType& mpe);

/// Create an managedPEType object from an XML representation string
/// @param augpe the object
/// @param s the XML string representation
void fromString(AADL::managedPEType& mpe, const std::string& s);

/// Convert a contextType object to its corresponding XML string
/// @param context the object
std::string toString(const ADL::contextType& context);

/// Convert an importedStreamsType object to its corresponding XML string
/// @param subscription the object
std::string toString(const ADL::importedStreamsType& subscription);

/// Create a importedStreamsType object from an XML representation string
/// @param importedStreamsType the object
/// @param s the XML string representation
void fromString(ADL::importedStreamsType& importedStreams, const std::string& s);

/// Convert an inputPortInfoType object to its corresponding XML string
/// @param subscription the object
std::string toString(const SAM_INTERFACE::inputPortInfoType& subscription);

/// Create a inputPortInfoType object from an XML representation string
/// @param inputPortInfoType the object
/// @param s the XML string representation
void fromString(SAM_INTERFACE::inputPortInfoType& inputPortInfo, const std::string& s);

/// Convert a jobInfoType object to its corresponding XML string
/// @param job the object
std::string toString(const SAM_INTERFACE::jobInfoType& job);

/// Create a jobInfoType object from an XML representation string
/// @param job the object
/// @param s the XML string representation
void fromString(SAM_INTERFACE::jobInfoType& job, const std::string& s);

/// Convert a jobsInfoType object to its corresponding XML string
/// @param jobs the object
std::string toString(const SAM_INTERFACE::jobsInfoType& jobs);

/// Create a jobsInfoType object from an XML representation string
/// @param jobs the object
/// @param s the XML string representation
void fromString(SAM_INTERFACE::jobsInfoType& jobs, const std::string& s);

/// Convert an jobType object to its corresponding XML string
/// @param job the object
std::string toString(const AADL::jobType& job);

/// Create a jobType object from an XML representation string
/// @param job the object
/// @param s the XML string representation
void fromString(AADL::jobType& job, const std::string& s);

/// Convert an outputPortInfoType object to its corresponding XML string
/// @param subscription the object
std::string toString(const SAM_INTERFACE::outputPortInfoType& subscription);

/// Create a outputPortInfoType object from an XML representation string
/// @param outputPortInfoType the object
/// @param s the XML string representation
void fromString(SAM_INTERFACE::outputPortInfoType& outputPortInfo, const std::string& s);

/// Convert a peInfoType object to its corresponding XML string
/// @param pe the object
std::string toString(const SAM_INTERFACE::peInfoType& pe);

/// Create a peInfoType object from an XML representation string
/// @param pe the object
/// @param s the XML string representation
void fromString(SAM_INTERFACE::peInfoType& pe, const std::string& s);

/// Convert a pesInfoType object to its corresponding XML string
/// @param pes the object
std::string toString(const SAM_INTERFACE::pesInfoType& pes);

/// Create a pesInfoType object from an XML representation string
/// @param pes the object
/// @param s the XML string representation
void fromString(SAM_INTERFACE::pesInfoType& pes, const std::string& s);

/// Convert a peCollectionOperationStatusType object to its corresponding XML string
/// @param peColOpStatus the object
std::string toString(const SAM_INTERFACE::peCollectionOperationStatusType& peColOpStatus);

/// Create a peCollectionOperationStatusType object from an XML representation string
/// @param peColOpStatus the object
/// @param s the XML string representation
void fromString(SAM_INTERFACE::peCollectionOperationStatusType& peColOpStatus,
                const std::string& s);

/// Convert a peStateType object to its corresponding XML string
/// @param state the object
std::string toString(const AADL::peStateType& state);

/// Create a peStateType object from an XML representation string
/// @param state the object
/// @param s the XML string representation
void fromString(AADL::peStateType& state, const std::string& s);

/// Convert a peStateReasonType object to its corresponding XML string
/// @param reason the object
std::string toString(const AADL::peStateReasonType& reason);

/// Create a peStateReasonType object from an XML representation string
/// @param reason the object
/// @param s the XML string representation
void fromString(AADL::peStateReasonType& reason, const std::string& s);

/// Convert a peStateInfoType object to its corresponding XML string
/// @param stateInfo the object
std::string toString(const SAM_INTERFACE::peStateInfoType& stateInfo);

/// Create a peStateInfoType object from an XML representation string
/// @param stateInfo the object
/// @param s the XML string representation
void fromString(SAM_INTERFACE::peStateInfoType& stateInfo, const std::string& s);

/// Convert a peStateInfoType object to its corresponding XML string
/// @param stateInfo the object
std::string toString(const SAM_INTERFACE::pesStateInfoType& stateInfo);

/// Create a peStateInfoType object from an XML representation string
/// @param stateInfo the object
/// @param s the XML string representation
void fromString(SAM_INTERFACE::pesStateInfoType& stateInfo, const std::string& s);

/// Convert a peConfigStateInfoType object to its corresponding XML string
/// @param stateInfo the object
std::string toString(const SAM_INTERFACE::peConfigStateInfoType& stateInfo);

/// Create a peConfigStateInfoType object from an XML representation string
/// @param stateInfo the object
/// @param s the XML string representation
void fromString(SAM_INTERFACE::peConfigStateInfoType& stateInfo, const std::string& s);

/// Convert a portConnectionsInfoType object to its corresponding XML string
/// @param pcInfo the object
std::string toString(const SAM_INTERFACE::portConnectionsInfoType& pcInfo);

/// Create a portConnectionsInfoType object from an XML representation string
/// @param pcInfo the object
/// @param s the XML string representation
void fromString(SAM_INTERFACE::portConnectionsInfoType& pcInfo, const std::string& s);

/// Convert a samSummaryStateType object to its corresponding XML string
/// @param sss the object
std::string toString(const SAM_INTERFACE::samSummaryStateType& sss);

/// Create a samSummaryStateType object from an XML representation string
/// @param sss the object
/// @param s the XML string representation
void fromString(SAM_INTERFACE::samSummaryStateType& sss, const std::string& s);

/// Convert a streamPropertyNamesType object to its corresponding XML string
/// @param spns the object
std::string toString(const SAM_INTERFACE::streamPropertyNamesType& spns);

/// Create a streamPropertyNamesType object from an XML representation string
/// @param spns the object
/// @param s the XML string representation
void fromString(SAM_INTERFACE::streamPropertyNamesType& spns, const std::string& s);

/// Convert a streamPropertyNameType object to its corresponding XML string
/// @param spn the object
std::string toString(const SAM_INTERFACE::streamPropertyNameType& spn);

/// Create a streamPropertyNamesType object from an XML representation string
/// @param spn the object
/// @param s the XML string representation
void fromString(SAM_INTERFACE::streamPropertyNameType& spn, const std::string& s);

/// Convert a streamPropertiesType object to its corresponding XML string
/// @param sps the object
std::string toString(const ADL::streamPropertiesType& sps);

/// Create a streamPropertiesType object from an XML representation string
/// @param properties the object
/// @param s the XML string representation
void fromString(ADL::streamPropertiesType& properties, const std::string& s);

/// Convert a streamPropertyType object to its corresponding XML string
/// @param sp the object
std::string toString(const ADL::streamPropertyType& sp);

/// Convert a streamPropertyType object to its corresponding XML string
/// @param sp the object
/// @param s the XML string representation
void fromString(ADL::streamPropertyType& sp, const std::string& s);

/// Convert a systemStateType object into an XML string
/// @param ss the system state object
/// @return the corresponding XML string
std::string toString(const SAM_INTERFACE::systemStateType& ss);

/// Convert a systemStateType object to its corresponding XML string
/// @param st the object
/// @param s the XML string representation
void fromString(SAM_INTERFACE::systemStateType& ss, const std::string& s);

/// Convert a systemStateConstraintType object into an XML string
/// @param ssc the system state object
/// @return the corresponding XML string
std::string toString(const SAM_INTERFACE::systemStateConstraintType& ssc);

/// Convert a systemStateConstraintType object to its corresponding XML string
/// @param ssc the object
/// @param ss the XML string representation
void fromString(SAM_INTERFACE::systemStateConstraintType& ssc, const std::string& ss);

/// Convert a systemStateScopingType object into an XML string
/// @param sss the system state object
/// @return the corresponding XML string
std::string toString(const SAM_INTERFACE::systemStateScopingType& sss);

/// Convert a systemStateScopingType object to its corresponding XML string
/// @param sss the object
/// @param ss the XML string representation
void fromString(SAM_INTERFACE::systemStateScopingType& sss, const std::string& ss);

/// Convert a systemTopologyScopingType object into an XML string
/// @param sts the system topology object
/// @return the corresponding XML string
std::string toString(const SAM_INTERFACE::systemTopologyScopingType& sts);

/// Convert a systemTopologyScopingType object to its corresponding XML string
/// @param sts the object
/// @param s the XML string representation
void fromString(SAM_INTERFACE::systemTopologyScopingType& sts, const std::string& s);

/// Convert a systemTopologyType object into an XML string
/// @param st the system topology object
/// @return the corresponding XML string
std::string toString(const SAM_INTERFACE::systemTopologyType& st);

/// Convert a systemTopologyType object to its corresponding XML string
/// @param st the object
/// @param s the XML string representation
void fromString(SAM_INTERFACE::systemTopologyType& st, const std::string& s);

/// Convert a excludeJobsType object to its corresponding XML string
/// @param st the object
/// @param s the XML string representation
void fromString(SAM_INTERFACE::excludeJobsType& ej, const std::string& s);

/// Convert a excludeJobsType object into an XML string
/// @param st the object
/// @return the corresponding XML string
std::string toString(const SAM_INTERFACE::excludeJobsType& ejt);

// DRH Note toString2 was needed to avoid a signature collision with JobIdType
// also note the macro SAM_STRINGFN_IMPL_TWO created for this reason also.
/// Convert a eTagType object to its corresponding XML string
/// @param st the object
/// @param s the XML string representation
void fromString2(SAM_INTERFACE::eTagType& et, const std::string& s);

/// Convert a eTagType object into an XML string
/// @param sp the submission parameters object
/// @return the corresponding XML string
std::string toString2(const SAM_INTERFACE::eTagType& ets);

/// Convert a submitParamsType object into an XML string
/// @param sp the submission parameters object
/// @return the corresponding XML string
std::string toString(const SAM_INTERFACE::submitParamsType& sp);

/// Convert a submitParamsType object to its corresponding XML string
/// @param sp the object
/// @param s the XML string representation
void fromString(SAM_INTERFACE::submitParamsType& sp, const std::string& s);

/// Convert a cancelParamsType object into an XML string
/// @param cp the cancellation parameters object
/// @return the corresponding XML string
std::string toString(const SAM_INTERFACE::cancelParamsType& cp);

/// Convert a cancelParamsType object to its corresponding XML string
/// @param cp the object
/// @param s the XML string representation
void fromString(SAM_INTERFACE::cancelParamsType& cp, const std::string& s);

/// Convert a importIndexesType object into an XML string
/// @param iin the import index type
/// @return the corresponding XML string
std::string toString(const AADL::importIndexesType& iin);

/// Convert a importIndexesType object to its corresponding XML string
/// @param iin the import index type object
/// @param s the XML string representation
void fromString(AADL::importIndexesType& iin, const std::string& s);

/// Convert a streamsType object into an XML string
/// @param var the streams type
/// @return the corresponding XML string
std::string toString(const AADL::streamsType& var);

/// Convert a streamsType object to its corresponding XML string
/// @param var the streams type object
/// @param s the XML string representation
void fromString(AADL::streamsType& var, const std::string& s);

/// Convert a streamType object into an XML string
/// @param var the stream type
/// @return the corresponding XML string
std::string toString(const AADL::streamType& var);

/// Convert a streamType object to its corresponding XML string
/// @param var the stream type object
/// @param s the XML string representation
void fromString(AADL::streamType& var, const std::string& s);

/// Convert a streamIdType object into an XML string
/// @param var the streamId type
/// @return the corresponding XML string
std::string toString(const AADL::streamIdType& sid);

/// Convert a streamIdType object into a string using only required attributes
/// @param var the streamId type
/// @return the corresponding string
std::string genKey(const AADL::streamIdType& sid);

/// Convert a streamIdType object to its corresponding XML string
/// @param var the streamId type object
/// @param s the XML string representation
void fromString(AADL::streamIdType& var, const std::string& s);

/// Convert a nameValuePairsType object into an XML string
/// @param var the name-value pairs type
/// @return the corresponding XML string
std::string toString(const AADL::nameValuePairsType& var);

/// Convert a nameValuePairsType object to its corresponding XML string
/// @param var the name-value pairs type object
/// @param s the XML string representation
void fromString(AADL::nameValuePairsType& var, const std::string& s);

/// Convert a streamViewsType object into an XML string
/// @param var the stream views type
/// @return the corresponding XML string
std::string toString(const AADL::streamViewsType& var);

/// Convert a streamViewsType object to its corresponding XML string
/// @param var the stream views type object
/// @param s the XML string representation
void fromString(AADL::streamViewsType& var, const std::string& s);

/// Convert a streamViewType object into an XML string
/// @param var the stream views type
/// @return the corresponding XML string
std::string toString(const AADL::streamViewType& var);

/// Convert a streamViewType object to its corresponding XML string
/// @param var the stream views type object
/// @param s the XML string representation
void fromString(AADL::streamViewType& var, const std::string& s);

/// Convert a ConnectionIndexKey object into an XML string
/// @param var the ConnectionIndexKey type
/// @return the corresponding XML string
//std::string toString(const ConnectionIndexKey & var);

/// Convert a ConnectionIndexKey object to its corresponding XML string
/// @param var the jConnectionIndexKey type object
/// @param s the XML string representation
//void fromString(ConnectionIndexKey & var, const std::string & s);

/// Convert a jobNotificationType object into an XML string
/// @param var the job notification type
/// @return the corresponding XML string
std::string toString(const SAM_INTERFACE::jobNotificationType& var);

/// Convert a jobNotificationType object to its corresponding XML string
/// @param var the job notification type object
/// @param s the XML string representation
void fromString(SAM_INTERFACE::jobNotificationType& var, const std::string& s);

/// Make the following relative paths inside the PE elements within the ADL
/// absolute, using the \a path argument as the base:
///   - application directory
///   - data directory
///   - PE binary path
//  Also set the output directory path inside the PE element to \a path
/// @param appset application set
/// @param path base of the relative paths
void updateADLPaths(ADL::applicationSetType& appset, std::string const& path);

/// Update the ADL applications for hostpool set --
/// changing resource definition to resource ID.
//void updateADLHostpools(ADL::applicationSetType& appset);

/// Find the names of the required submission time values as well as the names
/// and the values of the optional submission time values
/// @param adl application set
/// @param requiredValues names of the required submission time values (out
/// argument)
/// @param optionalValues names and values of the required submission time
/// values (out argument)
void findSubmissionTimeValues(const ADL::applicationSetType& adl,
                              std::tr1::unordered_set<std::string>& requiredValues,
                              std::tr1::unordered_map<std::string, std::string>& optionalValues);

/// Set the submission time value fields in the ADL
/// @param adl application set
/// @param namedValues list of name/value pairs. First element in the pair is
/// the submission time value name and the second one is the actual value
void setSubmissionTimeValues(ADL::applicationSetType& adl,
                             const std::vector<std::pair<std::string, std::string> >& namedValues);

/// Set the submission time configs in the ADL
/// @param adl application set
/// @param namedValues list of name/value pairs. First element in the pair is
/// the config name and the second one is the actual value
void setSubmissionAppConfigs(ADL::applicationSetType& adl,
                             const std::vector<std::pair<std::string, std::string> >& namedValues);

/// Set the submission time ADL file absolute path
/// @param adl application set
/// @param adlPath submission time adl file absolute path
void setSubmissionAdlPath(ADL::applicationSetType& adl, const std::string& adlPath);

/// Helper class to record submission time errors for later perusal
class SubmissionTimeErrors
{
  public:
    enum Error
    {
        ExtraNameValue, //! Name provided that isn't used (usually warning)
        MissingName,    //! Name is required, but not present
        AmbiguousName,  //! Name clash
        DuplicateName
    }; //! Duplicate names in supplied parms
    /// Add an error message
    /// @param k      Error kind
    /// @param value     Strings associated with error
    void add(Error k, const std::vector<std::string>& value)
    {
        _errors.push_back(ErrInfo(k, value));
    }

    /// Return the number of errors
    /// @return the number of saved errors
    uint32_t numErrors() const { return _errors.size(); }

    /// Return the error kind for a given error
    /// @pre e < _errors.size()
    /// @param e Error number
    /// @return Error kind
    Error kind(uint32_t e) const { return _errors[e]._kind; }

    /// Return the error strings for a given error
    /// @pre e < _errors.size()
    /// @param e Error number
    /// @return vector of associated strings
    const std::vector<std::string>& strings(uint32_t e) const { return _errors[e]._strings; }

  private:
    struct ErrInfo
    {
        ErrInfo() {}
        ErrInfo(Error kind, const std::vector<std::string>& value)
          : _kind(kind)
          , _strings(value)
        {}

        Error _kind;
        std::vector<std::string> _strings;
    };

    std::vector<ErrInfo> _errors;
};

// TODO: There is much to clean-up here. replaceSubmissionTimeValues function
// should be broken up to separate error checking from runtime constant
// replacement. See RTC 6141.

/// Validate submission time values passed and write to PE runtimeConstants in
/// ADL if updatePeConstants flag is set to true
/// @param adl application set
/// @param namedValues list of name/value pairs. First element in each pair is
/// the submission time value name and the second one is the actual value
/// @param problems problems found while matching submission time values (out
/// argument)
/// @param updatePeConstants flag indicates write submissionTimeValues to
/// PE runtime constants
void replaceSubmissionTimeValues(
  ADL::applicationSetType& adl,
  const std::vector<std::pair<std::string, std::string> >& namedValues,
  SubmissionTimeErrors& problems,
  bool updatePeConstants = true);

/// Create missing 'tuple' objects for older ADL.
/// @param tt the existing tuple type
void ensureTupleTypeCreated(ADL::tupleTypeType& tt);

SAM_NAMESPACE_END

#define SAM_XSD_FLAGS                                                                              \
    static_cast<xml_schema::flags>(xml_schema::flags::dont_validate |                              \
                                   xml_schema::flags::dont_initialize)

#define SAM_XML_SERIALIZE(_stream, _func, _var)                                                    \
    do {                                                                                           \
        xml_schema::namespace_infomap map;                                                         \
        map[""].name = "http://www.ibm.com/xmlns/prod/streams/application";                        \
        map["sam"].name = "http://www.ibm.com/xmlns/prod/streams/runtime/sam";                     \
        map["augmented"].name = "http://www.ibm.com/xmlns/prod/streams/application/augmented";     \
        map["tt"].name = "http://www.ibm.com/xmlns/prod/streams/spl/tupleType";                    \
        map["xs"].name = "http://www.w3.org/2001/XMLSchema";                                       \
        map["xsi"].name = "http://www.w3.org/2001/XMLSchema-instance";                             \
        _func(_stream, _var, map, "UTF-8", xml_schema::flags::dont_initialize);                    \
    } while (0);

#endif /* SAMHELPERFUNCTIONS_H */

#define DESER_FROM_BLOB(vartype, varid, buf)                                                       \
    auto_ptr<vartype##Type> varid;                                                                 \
    do {                                                                                           \
        uint32_t __iblobsize;                                                                      \
        const char* __blb = reinterpret_cast<const char*>(buf.getBlob(__iblobsize));               \
        std::stringstream __ss;                                                                    \
        __ss.write(__blb, __iblobsize);                                                            \
        varid = vartype(__ss, SAM_XSD_FLAGS);                                                      \
    } while (0);
