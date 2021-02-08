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

#include <SAM/SAMHelperFunctions.h>
#include <SAM/SAMInterfaceTypes.h>
#include <SAM/SAMMessages.h>
#include <SAM/applicationModel.h>
#include <SAM/augmentedApplicationModel.h>
#include <SAM/physicalApplicationModel.h>
#include <TRC/DistilleryDebug.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/Exception.h>
#include <UTILS/HashMapHelpers.h>
#include <UTILS/HostInfo.h>
#include <UTILS/ResourceBundleHelper.h>
#include <cassert>
#include <sstream>
//#include <UTILS/ResourceManagementUtils.h>
#include <UTILS/SupportFunctions.h>

SAM_NAMESPACE_USE
UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE
using namespace AADL;
using namespace ADL;
using namespace PADL;
using namespace SAM_INTERFACE;
using namespace std;
using namespace std::tr1;

#define SAM_OBJ_TO_XML_STRING(_func, _var, _str)                                                   \
    do {                                                                                           \
        try {                                                                                      \
            stringstream _stream;                                                                  \
            xml_schema::namespace_infomap map;                                                     \
            map[""].name = "http://www.ibm.com/xmlns/prod/streams/application";                    \
            map["sam"].name = "http://www.ibm.com/xmlns/prod/streams/runtime/sam";                 \
            map["augmented"].name = "http://www.ibm.com/xmlns/prod/streams/application/augmented"; \
            map["tt"].name = "http://www.ibm.com/xmlns/prod/streams/spl/tupleType";                \
            map["xs"].name = "http://www.w3.org/2001/XMLSchema";                                   \
            map["xsi"].name = "http://www.w3.org/2001/XMLSchema-instance";                         \
            _func(_stream, _var, map, "UTF-8", xml_schema::flags::dont_initialize);                \
            _str.assign(_stream.str());                                                            \
        } catch (xml_schema::exception & e) {                                                      \
            THROW(MalformedXMLObject,                                                              \
                  "failure to serialize object of type '" << #_func << endl                        \
                                                          << " with failure: " << e,               \
                  SAMUnexpectedException);                                                         \
        } catch (exception & e) {                                                                  \
            THROW(MalformedXMLObject,                                                              \
                  "failure to serialize object of type '" << #_func                                \
                                                          << " with failure: " << e.what(),        \
                  SAMUnexpectedException);                                                         \
        } catch (...) {                                                                            \
            THROW(MalformedXMLObject,                                                              \
                  "failure to serialize object of type '" << #_func                                \
                                                          << " with failure: unknown exception",   \
                  SAMUnexpectedException);                                                         \
        }                                                                                          \
    } while (0);

#define SAM_XML_STRING_TO_OBJ(_str, _func, _var)                                                   \
    do {                                                                                           \
        try {                                                                                      \
            stringstream _ts(_str);                                                                \
            auto_ptr<_func##Type> _tobj = _func(_ts, SAM_XSD_FLAGS);                               \
            _var = *_tobj;                                                                         \
        } catch (const xml_schema::exception& e) {                                                 \
            THROW(MalformedXMLObject,                                                              \
                  "failure to deserialize object of type '"                                        \
                    << #_func << "Type' from the following string representation: " << _str        \
                    << endl                                                                        \
                    << " with failure: " << e,                                                     \
                  SAMUnexpectedException);                                                         \
        } catch (exception & e) {                                                                  \
            THROW(MalformedXMLObject,                                                              \
                  "failure to deserialize object of type '"                                        \
                    << #_func << "Type' from a string: " << _str << ": " << e.what(),              \
                  SAMUnexpectedException);                                                         \
        } catch (...) {                                                                            \
            THROW(MalformedXMLObject,                                                              \
                  "failure to deserialize object of type '"                                        \
                    << #_func << "Type' from a string: " << _str << ": unknown error",             \
                  SAMUnexpectedException);                                                         \
        }                                                                                          \
    } while (0);

#define SAM_XML_STREAM_TO_OBJ(_str, _func, _var)                                                   \
    do {                                                                                           \
        try {                                                                                      \
            auto_ptr<_func##Type> _tobj = _func(_str, SAM_XSD_FLAGS);                              \
            _var = *_tobj;                                                                         \
        } catch (const xml_schema::exception& e) {                                                 \
            THROW(MalformedXMLObject,                                                              \
                  "failure to deserialize object of type '"                                        \
                    << #_func << "Type' from a stream representation with failure: " << e,         \
                  SAMUnexpectedException);                                                         \
        } catch (exception & e) {                                                                  \
            THROW(MalformedXMLObject,                                                              \
                  "failure to deserialize object of type '"                                        \
                    << #_func << "Type' from a stream: " << e.what(),                              \
                  SAMUnexpectedException);                                                         \
        } catch (...) {                                                                            \
            THROW(MalformedXMLObject,                                                              \
                  "failure to deserialize object of type '"                                        \
                    << #_func << "Type' from a stream: unknown error",                             \
                  SAMUnexpectedException);                                                         \
        }                                                                                          \
    } while (0);

#define SAM_STRINGFN_IMPL(dtype)                                                                   \
    string toString(const dtype##Type& var)                                                        \
    {                                                                                              \
        string s;                                                                                  \
        SAM_OBJ_TO_XML_STRING(dtype, var, s);                                                      \
        return s;                                                                                  \
    }                                                                                              \
    void fromString(dtype##Type& var, const string& s) { SAM_XML_STRING_TO_OBJ(s, dtype, var); }

// DRH Note this macro was needed to avoid a signature collision with toString(JobIdType)
// also note the toString2 functions also created for this reason.
#define SAM_STRINGFN_IMPL_TWO(dtype)                                                               \
    string toString2(const dtype##Type& var)                                                       \
    {                                                                                              \
        string s;                                                                                  \
        SAM_OBJ_TO_XML_STRING(dtype, var, s);                                                      \
        return s;                                                                                  \
    }                                                                                              \
    void fromString2(dtype##Type& var, const string& s) { SAM_XML_STRING_TO_OBJ(s, dtype, var); }

SAM_NAMESPACE_BEGIN

void fromString(applicationSetType& appset, istream& s)
{
    SAM_XML_STREAM_TO_OBJ(s, applicationSet, appset);
}

/// Convert a ConnectionIndexKey object into an XML string
/// @param var the ConnectionIndexKey type
/// @return the corresponding XML string
//std::string toString(const ConnectionIndexKey & var) {
//	return (var.toString());
//}

/// Convert a ConnectionIndexKey object to its corresponding XML string
/// @param var the jConnectionIndexKey type object
/// @param s the XML string representation
//void fromString(ConnectionIndexKey & var, const std::string & s) {
//    var = ConnectionIndexKey(s);
//}

SAM_STRINGFN_IMPL(applicationSet)
SAM_STRINGFN_IMPL(application)
SAM_STRINGFN_IMPL(augmentedApplicationSet)
SAM_STRINGFN_IMPL(augmentedPE)
SAM_STRINGFN_IMPL(managedPE)
SAM_STRINGFN_IMPL(augmentedPEInputPort)
SAM_STRINGFN_IMPL(augmentedPEOutputPort)
SAM_STRINGFN_IMPL(context)
SAM_STRINGFN_IMPL(importedStreams)
SAM_STRINGFN_IMPL(inputPortInfo)
SAM_STRINGFN_IMPL(jobInfo)
SAM_STRINGFN_IMPL(jobsInfo)
SAM_STRINGFN_IMPL(job)
SAM_STRINGFN_IMPL(outputPortInfo)
SAM_STRINGFN_IMPL(peInfo)
SAM_STRINGFN_IMPL(pesInfo)
SAM_STRINGFN_IMPL(peCollectionOperationStatus)
SAM_STRINGFN_IMPL(peStateInfo)
SAM_STRINGFN_IMPL(pesStateInfo)
SAM_STRINGFN_IMPL(peConfigStateInfo)
SAM_STRINGFN_IMPL(peState)
SAM_STRINGFN_IMPL(peStateReason)
SAM_STRINGFN_IMPL(samSummaryState)
SAM_STRINGFN_IMPL(streamPropertyNames)
SAM_STRINGFN_IMPL(streamPropertyName)
SAM_STRINGFN_IMPL(streamProperties)
SAM_STRINGFN_IMPL(streamProperty)
SAM_STRINGFN_IMPL(systemStateConstraint)
SAM_STRINGFN_IMPL(systemStateScoping)
SAM_STRINGFN_IMPL(systemState)
SAM_STRINGFN_IMPL(systemTopologyScoping)
SAM_STRINGFN_IMPL(systemTopology)
SAM_STRINGFN_IMPL(submitParams)
SAM_STRINGFN_IMPL(cancelParams)
SAM_STRINGFN_IMPL(importIndexes)
SAM_STRINGFN_IMPL(streams)
SAM_STRINGFN_IMPL(stream)
SAM_STRINGFN_IMPL(streamId)
SAM_STRINGFN_IMPL(nameValuePairs)
SAM_STRINGFN_IMPL(streamViews)
SAM_STRINGFN_IMPL(streamView)
SAM_STRINGFN_IMPL(jobNotification)
SAM_STRINGFN_IMPL(portConnectionsInfo)
SAM_STRINGFN_IMPL(excludeJobs)
SAM_STRINGFN_IMPL_TWO(eTag)

const tupleTypeType& retrieveTupleType(const applicationType& app,
                                       const peInputPortType::tupleTypeIndex_type& ttindex)
{
    if (ttindex < app.tupleTypes().tupleType().size()) {
        const tupleTypeType& toret(app.tupleTypes().tupleType().at(ttindex));
        assert(toret.index() == ttindex);
        return toret;
    }
    THROW(Distillery, "failure to locate tupleType with index " << ttindex,
          SAMFailureLocatingtupleType, UTILS_NAMESPACE::toString(ttindex).c_str());
}

const hostpoolType& retrieveHostpool(const applicationType& app,
                                     const poolLocationType::poolIndex_type& pindex)
{
    if (pindex < app.hostpools().hostpool().size()) {
        const hostpoolType& toret(app.hostpools().hostpool().at(pindex));
        assert(toret.index() == pindex);
        return toret;
    }
    THROW(Distillery, "failure to locate hostpool with index " << pindex,
          SAMFailureLocatingHostpool, UTILS_NAMESPACE::toString(pindex).c_str());
}

const libraryType& retrieveLibrary(const applicationType& app,
                                   const libraryDependencyType::libIndex_type& lindex)
{
    if (lindex < app.libraries().library().size()) {
        const libraryType& toret(app.libraries().library().at(lindex));
        assert(toret.index() == lindex);
        return toret;
    }
    THROW(Distillery, "failure to locate library with index " << lindex, SAMFailureLocatingLibrary,
          UTILS_NAMESPACE::toString(lindex).c_str());
}

auto_ptr<librariesType> createLibraries(const applicationType& app, const peType& pe)
{
    auto_ptr<librariesType> libraries(new librariesType);
    for (dependenciesType::dependency_const_iterator dep =
           pe.executable().dependencies().dependency().begin();
         dep != pe.executable().dependencies().dependency().end(); ++dep) {
        auto_ptr<libraryType> library(new libraryType(retrieveLibrary(app, dep->libIndex())));
        libraries->library().push_back(library);
    }

    return libraries;
}

auto_ptr<hostpoolsType> createHostpools(const applicationType& app, const peType& pe)
{
    auto_ptr<hostpoolsType> hostpools(new hostpoolsType());

    // iterate over operators and retrieve all references to specific host
    // pools
    unordered_set<unsigned long> pools;
    for (operInstancesType::operInstance_const_iterator oper =
           pe.operInstances().operInstance().begin();
         oper != pe.operInstances().operInstance().end(); ++oper) {
        if (oper->resources().poolLocation().present()) {
            unsigned long index = oper->resources().poolLocation().get().poolIndex();
            if (pools.find(index) != pools.end()) {
                continue;
            }
            pools.insert(index);
            auto_ptr<hostpoolType> hostpool(new hostpoolType(retrieveHostpool(app, index)));
            hostpools->hostpool().push_back(hostpool);
        }
    }

    return hostpools;
}

auto_ptr<tupleTypesType> createTupleTypes(const applicationType& app, const peType& pe)
{
    auto_ptr<tupleTypesType> tupleTypes(new tupleTypesType());

    // iterate over operators and retrieve all references to specific tuple
    // types
    unordered_set<unsigned long> tuples;
    for (operInstancesType::operInstance_const_iterator oper =
           pe.operInstances().operInstance().begin();
         oper != pe.operInstances().operInstance().end(); ++oper) {
        // iterate over operator's input ports
        for (operInstanceInputPortsType::inputPort_const_iterator ip =
               oper->inputPorts().inputPort().begin();
             ip != oper->inputPorts().inputPort().end(); ++ip) {
            unsigned long index = ip->tupleTypeIndex();
            if (tuples.find(index) != tuples.end())
                continue;
            tuples.insert(index);
            auto_ptr<tupleTypeType> tupleType(new tupleTypeType(retrieveTupleType(app, index)));
            ensureTupleTypeCreated(*tupleType);
            tupleTypes->tupleType().push_back(tupleType);
        }

        // iterate over operator's output ports
        for (operInstanceOutputPortsType::outputPort_const_iterator op =
               oper->outputPorts().outputPort().begin();
             op != oper->outputPorts().outputPort().end(); ++op) {
            unsigned long index = op->tupleTypeIndex();
            if (tuples.find(index) != tuples.end())
                continue;
            tuples.insert(index);
            auto_ptr<tupleTypeType> tupleType(new tupleTypeType(retrieveTupleType(app, index)));
            ensureTupleTypeCreated(*tupleType);
            tupleTypes->tupleType().push_back(tupleType);
        }
    }

    return tupleTypes;
}

const managedPEType& getManagedPETypeWithIndex(const managedPEsType& mpes, uint64_t index)
{
    for (managedPEsType::pe_const_iterator mpe = mpes.pe().begin(); mpe != mpes.pe().end(); ++mpe) {
        if (mpe->index() == index) {
            return *mpe;
        }
    }
    THROW(GDGInternalError, "Could not find managedPEType object with index " << index << ".");
}

void createRuntimeIncomingStaticConnections(const managedPEsType& mpes,
                                            const peInputPortType& ip,
                                            augmentedPEInputPortType& augip)
{
    SPCDBG(L_TRACE, "Enter for pe input port " << augip.id(), SAM_ASP);
    for (incomingStaticConnectionsType::staticConnection_const_iterator sc =
           ip.staticConnections().staticConnection().begin();
         sc != ip.staticConnections().staticConnection().end(); ++sc) {
        if (sc->peIndex() >= mpes.pe().size()) {
            THROW(GDGInternalError, "peIndex " << sc->peIndex()
                                               << " of a static connection of input port "
                                               << ip.index() << " out of application scope.");
        }
        const managedPEType& pe = getManagedPETypeWithIndex(mpes, sc->peIndex());
        if (sc->oportIndex() >= pe.outputPorts().outputPort().size()) {
            THROW(GDGInternalError, "Output port Index "
                                      << sc->oportIndex()
                                      << " of a static connection of input port " << ip.index()
                                      << " out of output port scope of related PE " << pe.index());
        }
        const augmentedPEOutputPortType& op = pe.outputPorts().outputPort().at(sc->oportIndex());
        incomingStaticIPConnectionType::peId_type peId = pe.id();
        incomingStaticIPConnectionType::oportId_type oportId = op.id();
        incomingStaticIPConnectionType::oportIndex_type oportIndex = op.index();
        auto_ptr<incomingStaticIPConnectionType> augsc(
          new incomingStaticIPConnectionType(peId, oportId, oportIndex));
        augip.staticConnections().staticConnection().push_back(*augsc);
    }
    SPCDBG(L_TRACE, "Exit", SAM_ASP);
}

void createRuntimeOutgoingStaticConnections(const managedPEsType& mpes,
                                            const peOutputPortType& op,
                                            augmentedPEOutputPortType& augop)
{
    SPCDBG(L_TRACE, "Enter for pe output port " << augop.id(), SAM_ASP);
    // Create PE outgoing static connections
    for (outgoingStaticConnectionsType::staticConnection_const_iterator sc =
           op.staticConnections().staticConnection().begin();
         sc != op.staticConnections().staticConnection().end(); ++sc) {
        if (sc->peIndex() >= mpes.pe().size()) {
            THROW(GDGInternalError, "peIndex " << sc->peIndex()
                                               << " of a static connection of output port "
                                               << op.index() << " out of application scope.");
        }
        const managedPEType& pe = getManagedPETypeWithIndex(mpes, sc->peIndex());
        if (sc->iportIndex() >= pe.inputPorts().inputPort().size()) {
            THROW(GDGInternalError, "Input port Index "
                                      << sc->iportIndex()
                                      << " of a static connection of output port " << op.index()
                                      << " out of input port scope of related PE " << pe.index());
        }
        const augmentedPEInputPortType& ip = pe.inputPorts().inputPort().at(sc->iportIndex());
        outgoingStaticOPConnectionType::peId_type peId = pe.id();
        outgoingStaticOPConnectionType::iportId_type iportId = ip.id();
        outgoingStaticOPConnectionType::iportIndex_type iportIndex = ip.index();
        auto_ptr<outgoingStaticOPConnectionType> augsc(new outgoingStaticOPConnectionType(
          peId, iportId, iportIndex, ip.operInstanceConnection().operInstanceIndex(),
          ip.operInstanceConnection().iportIndex()));
        augop.staticConnections().staticConnection().push_back(*augsc);
    }
    SPCDBG(L_TRACE, "Exit", SAM_ASP);
}

static void formatRstring(ostringstream& ostr, const string& str)
{
    ostr << '\"';
    string::size_type sz = str.size();
    for (string::size_type i = 0; i < sz; ++i) {
        char ch = str[i];
        if (ch == '"')
            ostr << "\\\"";
        else if (ch == '\\')
            ostr << "\\\\";
        else
            ostr << ch;
    }
    ostr << '\"';
}

#define DISPLAY_VAR_NAME "streams::pe.display"

typedef tr1::unordered_map<uint32_t, pair<string, const submissionTimeValueType*> > IndexMap;

void processingParallelRegions(applicationType& adl, const IndexMap* indexToValue)
{
    // Support for UDP submission value.
    SPCDBG(L_DEBUG, "Enter, indexToValue=" << ((NULL == indexToValue) ? "null" : "non-null"),
           SAM_ASP);
    if (adl.parallelRegions().present()) {
        parallelRegionsType& prts = adl.parallelRegions().get();
        for (parallelRegionsType::parallelRegion_iterator prit = prts.parallelRegion().begin();
             prit != prts.parallelRegion().end(); ++prit) {
            SPCDBG(L_TRACE, "Processing for UDP parallel region: " << prit->name(), SAM_ASP);
            int parallelWidth;
            parallelWidthType& pwt = prit->parallelWidth();
            if (pwt.submissionTimeParameterIndexForWidth().present()) {
                bool found = false;
                int64_t submissionIndex = pwt.submissionTimeParameterIndexForWidth().get();
                SPCDBG(L_TRACE,
                       "Trying to find match for submissionTimeParameterIndexForWidth: "
                         << submissionIndex,
                       SAM_ASP);
                if (NULL != indexToValue) {
                    IndexMap::const_iterator it = indexToValue->find(submissionIndex);
                    if (it != indexToValue->end()) {
                        found = true;
                        string value = it->second.first;
                        UTILS_NAMESPACE::fromString(parallelWidth, value);
                        SPCDBG(L_TRACE,
                               "Found match from submission values with width: " << parallelWidth,
                               SAM_ASP);
                    } else {
                        const submissionTimeValuesType::submissionTimeValue_sequence& subValSeq =
                          adl.submissionTimeValues().get().submissionTimeValue();
                        submissionTimeValuesType::submissionTimeValue_const_iterator svsit;
                        for (svsit = subValSeq.begin(); svsit != subValSeq.end(); svsit++) {
                            const int64_t sIndex = svsit->index();
                            if (sIndex == submissionIndex) {
                                if (svsit->defaultValue().present()) {
                                    found = true;
                                    // using default value
                                    string defaultvalue = svsit->defaultValue().get();
                                    SPCDBG(L_TRACE, "Default value set to " << defaultvalue,
                                           SAM_ASP);
                                    // The default value from SPL getSubmissionTimeValue()
                                    // function is string type, so with "s, such as
                                    // "12". Need to get rid of them.
                                    if (0 == defaultvalue.find('"')) {
                                        if (defaultvalue.length() <= 2) {
                                            SPCDBG(L_ERROR,
                                                   "Default value for '" << svsit->name() << "', "
                                                                         << defaultvalue
                                                                         << ", has wrong format.",
                                                   SAM_ASP);
                                            THROW(GDGInternalError, "Default value for '"
                                                                      << svsit->name() << "', "
                                                                      << defaultvalue
                                                                      << ", has wrong format.");
                                        }
                                        defaultvalue =
                                          defaultvalue.substr(1, defaultvalue.length() - 2);
                                    }
                                    SPCDBG(L_TRACE, "Fixed default value is " << defaultvalue,
                                           SAM_ASP);
                                    UTILS_NAMESPACE::fromString(parallelWidth, defaultvalue);
                                } else {
                                    THROW(GDGInternalError, "Should never get here");
                                    // Shall never be here. Validation will catch
                                    // this error before calling this method.
                                }
                                break;
                            }
                        }
                    }
                }
                if (!found) {
                    // Shall never be here. As it means a SPL compiler error.
                    SPCDBG(L_ERROR,
                           "Could not find matched submission value for UDP submissionParaIndex: "
                             << submissionIndex
                             << ". Please check ADL file for mismatched submission index settings.",
                           SAM_ASP);
                    THROW(GDGInternalError,
                          "Could not find matched submission value for UDP submissionParaIndex: "
                            << submissionIndex
                            << ". Please check ADL file for mismatched submission index settings.");
                }
            } else if (pwt.constantWidth().present()) {
                // using constant width
                parallelWidth = pwt.constantWidth().get();
                SPCDBG(L_TRACE,
                       "Set parallel region width of " << parallelWidth << " from constantWidth.",
                       SAM_ASP);
            } else {
                // Shall never be here. SPL compiler need to
                // check and error if this happens.
            }

            // Set submissionTimeWidth element with parallel width.
            pwt.submissionTimeWidth(parallelWidth);
            SPCDBG(L_DEBUG,
                   "Done for UDP parallel region: " << prit->name()
                                                    << " with width: " << parallelWidth,
                   SAM_ASP);
        }
    }
    SPCDBG(L_DEBUG, "Exit.", SAM_ASP);
}

void setSPLAnnotationValue(splAnnotationValueType& splVal,
                           const IndexMap* indexToValue,
                           const applicationType::submissionTimeValues_optional& subValOptional)
{
    SPCDBG(L_DEBUG, "Enter, with splAnnotationValue=" << splVal, SAM_ASP);
    bool found = false;
    if (splVal.submissionTimeValueIndex().present()) {
        int64_t submissionIndex = splVal.submissionTimeValueIndex().get();
        SPCDBG(L_TRACE, "Trying to find match for submissionTimeValueIndex: " << submissionIndex,
               SAM_ASP);
        IndexMap::const_iterator it = indexToValue->find(submissionIndex);
        if (it != indexToValue->end()) {
            found = true;
            splVal.assign(it->second.first);
            SPCDBG(L_TRACE,
                   "Found match from submission value: " << it->second.first
                                                         << ", new splAnnotationValue=" << splVal,
                   SAM_ASP);
        } else if (subValOptional.present()) {
            const submissionTimeValuesType::submissionTimeValue_sequence& subValSeq =
              subValOptional.get().submissionTimeValue();
            submissionTimeValuesType::submissionTimeValue_const_iterator svsit;
            for (svsit = subValSeq.begin(); svsit != subValSeq.end(); svsit++) {
                const int64_t sIndex = svsit->index();
                if (sIndex == submissionIndex) {
                    if (svsit->defaultValue().present()) {
                        found = true;
                        // using default value
                        splVal.assign(svsit->defaultValue().get());
                        SPCDBG(L_TRACE,
                               "Found default submission value: " << svsit->defaultValue().get()
                                                                  << ", new splAnnotationValue="
                                                                  << splVal,
                               SAM_ASP);
                    } else {
                        THROW(GDGInternalError, "Should never get here");
                        // Shall never be here. Validation will catch
                        // this error before calling this method.
                    }
                    break;
                }
            }
        }
        if (!found) {
            // Shall never be here. As it means a SPL compiler error.
            SPCDBG(L_ERROR,
                   "Could not find matched submissionValueIndex: "
                     << submissionIndex
                     << ". Please check ADL file for mismatched submission index settings.",
                   SAM_ASP);
            THROW(GDGInternalError,
                  "Could not find matched submission submissionValueIndex: "
                    << submissionIndex
                    << ". Please check ADL file for mismatched submission index settings.");
        }
    }
    SPCDBG(L_DEBUG, "Exit, with splAnnotationValue=" << splVal, SAM_ASP);
}

void processingSPLAnnotations(applicationType& adl, const IndexMap* indexToValue)
{
    SPCDBG(L_DEBUG, "Enter, indexToValue=" << ((NULL == indexToValue) ? "null" : "non-null"),
           SAM_ASP);

    // compositeOperType
    compositeOpersType::compositeOper_sequence& compOperSeq =
      adl.composites().compositeOpers().compositeOper();
    for (compositeOpersType::compositeOper_iterator operIt = compOperSeq.begin();
         operIt != compOperSeq.end(); ++operIt) {
        SPCDBG(L_TRACE, "compositeOper index=" << operIt->index() << ", kind=" << operIt->kind(),
               SAM_ASP);
        for (compositeOperType::splAnnotation_iterator splIt = operIt->splAnnotation().begin();
             splIt != operIt->splAnnotation().end(); ++splIt) {
            SPCDBG(L_TRACE, "splAnnotation tag=" << splIt->tag(), SAM_ASP);
            for (splAnnotationType::value_iterator splValIt = splIt->value().begin();
                 splValIt != splIt->value().end(); ++splValIt) {
                setSPLAnnotationValue(*splValIt, indexToValue, adl.submissionTimeValues());
            }
        }
    }

    // compositeOperInstanceType
    compositeOperInstancesType::compositeOperInstance_sequence& compOperInstSeq =
      adl.composites().compositeOperInstances().compositeOperInstance();
    for (compositeOperInstancesType::compositeOperInstance_iterator operInstIt =
           compOperInstSeq.begin();
         operInstIt != compOperInstSeq.end(); ++operInstIt) {
        SPCDBG(L_TRACE,
               "compositeOperInstance index=" << operInstIt->index()
                                              << ", name=" << operInstIt->name(),
               SAM_ASP);
        // Nested compositeOperInstanceType
        for (compositeOperInstanceType::compositeOperInstance_iterator operInstIt1 =
               operInstIt->compositeOperInstance().begin();
             operInstIt1 != operInstIt->compositeOperInstance().end(); ++operInstIt1) {
            SPCDBG(L_TRACE,
                   "Nested compositeOperInstance index=" << operInstIt1->index()
                                                         << ", name=" << operInstIt1->name(),
                   SAM_ASP);
            for (compositeOperInstanceType::splAnnotation_iterator splIt =
                   operInstIt1->splAnnotation().begin();
                 splIt != operInstIt1->splAnnotation().end(); ++splIt) {
                SPCDBG(L_TRACE, "splAnnotation tag=" << splIt->tag(), SAM_ASP);
                for (splAnnotationType::value_iterator splValIt = splIt->value().begin();
                     splValIt != splIt->value().end(); ++splValIt) {
                    setSPLAnnotationValue(*splValIt, indexToValue, adl.submissionTimeValues());
                }
            }
        }
        // primaryOPerInstanceType
        for (compositeOperInstanceType::primitiveOperInstance_iterator operInstIt2 =
               operInstIt->primitiveOperInstance().begin();
             operInstIt2 != operInstIt->primitiveOperInstance().end(); ++operInstIt2) {
            SPCDBG(L_TRACE,
                   "Nested primitiveOperInstance index=" << operInstIt2->index()
                                                         << ", name=" << operInstIt2->name(),
                   SAM_ASP);
            for (primitiveOperInstanceType::splAnnotation_iterator splIt =
                   operInstIt2->splAnnotation().begin();
                 splIt != operInstIt2->splAnnotation().end(); ++splIt) {
                SPCDBG(L_TRACE, "splAnnotation tag=" << splIt->tag(), SAM_ASP);
                for (splAnnotationType::value_iterator splValIt = splIt->value().begin();
                     splValIt != splIt->value().end(); ++splValIt) {
                    setSPLAnnotationValue(*splValIt, indexToValue, adl.submissionTimeValues());
                }
            }
        }
    }

    SPCDBG(L_DEBUG, "Exit.", SAM_ASP);
}

void replaceSubmissionTimeValues(ADL::applicationSetType& adl,
                                 const vector<pair<string, string> >& namedValues,
                                 SubmissionTimeErrors& problems,
                                 bool updatePeConstants)
{
    SPCDBG(L_DEBUG, "ENTER: replaceSubmissionTimeValues", SAM_ASP);

    using namespace xmlns::prod::streams::application;
    // Put the command line arguments into ADL
    applicationSetType::application_sequence::iterator adl_iter;
    for (adl_iter = adl.application().begin(); adl_iter != adl.application().end(); adl_iter++) {
        if (!adl_iter->submissionTimeValues().present()) {
            if (updatePeConstants) {
                // Need to call to set parallel regions with constantWidth
                // even submissionTimeValues not exist.
                processingParallelRegions(*adl_iter, NULL);
            }
            continue;
        }
        // check for duplicate namedValues and missing required parms
        // Iterate over all SubmissionTimeValues and check if one index matchs
        // multiple names supplied
        bool hasMissingOrDupNames = false;
        const submissionTimeValuesType::submissionTimeValue_sequence& subValues =
          adl_iter->submissionTimeValues().get().submissionTimeValue();
        submissionTimeValuesType::submissionTimeValue_const_iterator sit;
        for (sit = subValues.begin(); sit != subValues.end(); sit++) {
            const string sName = sit->compositeName() + '.' + sit->name();
            SPCDBG(L_DEBUG, "Looking for " << sName, SAM_ASP);
            bool matchFound = false;
            vector<pair<string, string> >::const_iterator nit;
            for (nit = namedValues.begin(); nit != namedValues.end(); nit++) {
                const string& n = nit->first;
                SPCDBG(L_DEBUG, "Comparing " << sName << " and " << n, SAM_ASP);
                size_t p = sName.rfind(n);
                if (p != string::npos) {
                    if ((p == 0 && n.size() == sName.size()) ||
                        (p + n.size() == sName.size() &&
                         (sName[p - 1] == '.' || sName[p - 1] == ':'))) {
                        if (matchFound) {
                            vector<string> errs;
                            errs.push_back(sName);
                            problems.add(SubmissionTimeErrors::DuplicateName, errs);
                            hasMissingOrDupNames = true;
                            break;
                        } else {
                            matchFound = true;
                        }
                    }
                }
            }
            if (!matchFound && !sit->defaultValue().present()) {
                string arg = sit->compositeName() + "." + sit->name();
                vector<string> errs;
                errs.push_back(arg);
                problems.add(SubmissionTimeErrors::MissingName, errs);
                hasMissingOrDupNames = true;
            }
        }
        // Check for namedValues that match more than one entry in the submissionTimeValues table
        vector<pair<string, string> >::const_iterator nit;
        IndexMap indexToValue;
        bool ambigFlag = false;
        string displayVar;
        for (nit = namedValues.begin(); nit != namedValues.end(); nit++) {
            const string& n = nit->first;
            if (n == DISPLAY_VAR_NAME) {
                displayVar = nit->second; //capture the display var
                continue;
            }
            const submissionTimeValuesType::submissionTimeValue_sequence& subValues2 =
              adl_iter->submissionTimeValues().get().submissionTimeValue();
            submissionTimeValuesType::submissionTimeValue_const_iterator sit2;
            string lastMatch;
            ambigFlag = false;
            bool gotAmbigName = false;
            vector<string> ambigErrs;
            for (sit2 = subValues2.begin(); sit2 != subValues2.end(); sit2++) {
                const string sName = sit2->compositeName() + '.' + sit2->name();
                size_t p = sName.rfind(n);
                if (p != string::npos) {
                    if ((p == 0 && n.size() == sName.size()) ||
                        (p + n.size() == sName.size() &&
                         (sName[p - 1] == '.' || sName[p - 1] == ':'))) {
                        // matched the whole string or the last component(s)
                        IndexMap::const_iterator it3 = indexToValue.find(sit2->index());
                        if (it3 == indexToValue.end()) { // only save first match
                            // Remember the match for writtng PE constants
                            indexToValue.insert(
                              make_pair(sit2->index(), make_pair(nit->second, &*sit2)));
                        }
                        // is there already an entry (ambiguous parm )
                        if (lastMatch.empty()) {
                            lastMatch = sName;
                        } else {
                            ambigFlag = true;
                            if (!gotAmbigName) {
                                ambigErrs.push_back(n);
                                gotAmbigName = true;
                            }
                            lastMatch = lastMatch + " , " + sName;
                        }
                    }
                }
            }
            if (lastMatch.empty()) {
                vector<string> errs;
                errs.push_back(n);
                problems.add(SubmissionTimeErrors::ExtraNameValue, errs);
            }
            if (ambigFlag) {
                ambigErrs.push_back(lastMatch);
                problems.add(SubmissionTimeErrors::AmbiguousName, ambigErrs);
            }
        }
        // now go update constants if no fatal errors,
        // such as ambiguous error, or missed or duplicated some submission values
        if (updatePeConstants && !ambigFlag && !hasMissingOrDupNames) {
            pesType::pe_sequence& pes = adl_iter->pes().pe();
            for (pesType::pe_iterator pe_iter = pes.begin(); pe_iter != pes.end(); pe_iter++) {
                if (!displayVar.empty()) {
                    //put the display value into the pe display attribute
                    basicPEType::display_type disp;
                    disp.name(DISPLAY_VAR_NAME);
                    disp.value(displayVar);
                    //TODO: do we also need the index and default value?
                    pe_iter->display(disp); //set the display variable
                }
                operInstancesType::operInstance_sequence& ops =
                  pe_iter->operInstances().operInstance();
                for (operInstancesType::operInstance_sequence::iterator it1 = ops.begin();
                     it1 != ops.end(); ++it1) {
                    runtimeConstantsType::runtimeConstant_sequence& rntConsts =
                      it1->runtimeConstants().runtimeConstant();
                    for (runtimeConstantsType::runtimeConstant_sequence::iterator it2 =
                           rntConsts.begin();
                         it2 != rntConsts.end(); ++it2) {

                        if (!it2->submissionTimeValueIndex().present())
                            continue;
                        uint32_t subIndex = it2->submissionTimeValueIndex().get();

                        // Find the matching index in the submission table
                        // If it was not found....and required, the error was flagged above
                        // just write what we have and trust the caller to react to error
                        IndexMap::const_iterator it3 = indexToValue.find(subIndex);
                        if (it3 != indexToValue.end()) {
                            // Okay, we KNOW this is a submission time value
                            submissionTimeValueKindType::value kind =
                              (submissionTimeValueKindType::value)it3->second.second->kind();

                            ostringstream ostr;
                            if (kind == submissionTimeValueKindType::named)
                                formatRstring(ostr, it3->second.first);
                            else {
                                assert(kind == submissionTimeValueKindType::namedList);
                                vector<string> tokens;
                                csvtokenize(it3->second.first, tokens);
                                ostr << '[';
                                for (uint32_t i = 0, n = tokens.size(); i < n; i++) {
                                    if (i > 0)
                                        ostr << ',';
                                    formatRstring(ostr, tokens[i]);
                                }
                                ostr << ']';
                            }
                            assert(ostr);
                            string& s = it2->value();
                            s = ostr.str();
                        }
                    }
                }
            }
            processingParallelRegions(*adl_iter, &indexToValue);
            processingSPLAnnotations(*adl_iter, &indexToValue);
        }
    }
    SPCDBG(L_DEBUG, "Exit.", SAM_ASP);
}

void findSubmissionTimeValues(const ADL::applicationSetType& adl,
                              tr1::unordered_set<string>& requiredValues,
                              tr1::unordered_map<string, string>& optionalValues)
{
    using namespace xmlns::prod::streams::application;
    // Get the required command line arguments from ADL
    applicationSetType::application_sequence::const_iterator adl_iter;
    for (adl_iter = adl.application().begin(); adl_iter != adl.application().end(); adl_iter++) {
        if (!adl_iter->submissionTimeValues().present())
            continue;
        const submissionTimeValuesType::submissionTimeValue_sequence& subValues =
          adl_iter->submissionTimeValues().get().submissionTimeValue();
        submissionTimeValuesType::submissionTimeValue_const_iterator it;
        for (it = subValues.begin(); it != subValues.end(); it++) {
            if (!it->defaultValue().present()) {
                ostringstream ostr;
                ostr << '[' << it->compositeName() << ".]" << it->name();
                requiredValues.insert(ostr.str());
            } else {
                ostringstream ostr;
                ostr << '[' << it->compositeName() << ".]" << it->name();
                optionalValues.insert(make_pair(ostr.str(), it->defaultValue().get()));
            }
        }
    }
}

#define FIX_DISP_HST(strval)                                                                       \
    do {                                                                                           \
        string _lc_ = "localhost";                                                                 \
        if (strval.empty() || UTILS_NAMESPACE::trim(strval).empty()) {                             \
            string _disp_ = get_environment_variable("DISPLAY", "");                               \
            if (_disp_.empty()) {                                                                  \
                THROW(SAM, "DISPLAY is not set in environment", SubmissionDisplayEnvVarMissing);   \
            }                                                                                      \
            strval = _disp_;                                                                       \
        }                                                                                          \
        size_t _found_ = strval.find(_lc_);                                                        \
        string _hst_ = HostInfo::getMyHostName();                                                  \
        /*check for "localhost:xx" and replace the localhost*/                                     \
        if (_found_ != string::npos) {                                                             \
            strval.replace(_found_, _lc_.length(), _hst_);                                         \
        }                                                                                          \
        /*begins with : */                                                                         \
        if (strval.at(0) == ':') {                                                                 \
            strval = _hst_ + strval;                                                               \
        }                                                                                          \
    } while (0);

void setSubmissionTimeValues(ADL::applicationSetType& adl,
                             const vector<pair<string, string> >& namedValues)
{
    SPCDBG(L_DEBUG, "setSubmissionTimeValues", SAM_ASP);
    for (vector<pair<string, string> >::const_iterator i = namedValues.begin();
         i != namedValues.end(); ++i) {
        SPCDBG(L_DEBUG, "NV pair=" << i->first << " - " << i->second, SAM_ASP);
    }
    // first call routine to validate the parameters
    SubmissionTimeErrors problems;
    replaceSubmissionTimeValues(adl, namedValues, problems, false);
    // check for errors
    ResourceBundleHelper rb;
    bool warningsFlag = false;
    // Message list setup for the exception
    // the primary information will be empty and all errors will get added to the secondaryInfomration sequence
    using namespace xmlns::prod::streams::runtime::utils;
    MessageElementType primaryInfo("");
    additionalExceptionDataType messageList(primaryInfo);

    additionalExceptionDataType::secondaryInformation_sequence& secondaryInfoSeq =
      messageList.secondaryInformation();

    // process any errors and add the information to the message list
    SPCDBG(L_INFO, problems.numErrors() << " errors were found", SAM_ASP);
    for (uint32_t i = 0, n = problems.numErrors(); i < n; i++) {
        const vector<string>& strings = problems.strings(i);
        switch (problems.kind(i)) {
            case Streams::SubmissionTimeErrors::ExtraNameValue: {
                SPCDBG(L_INFO,
                       "WARNING: Unrecognized submission parameter supplied: \'"
                         << strings[0] << "\' It will be ignored.",
                       SAM_ASP);
                cerr << PrintMessageToString(rb, SubmissionParameterExtraWarning, strings) << endl;
                warningsFlag = true;
                break;
            }
            case Streams::SubmissionTimeErrors::MissingName: {
                SPCDBG(L_INFO, "Missing submission time value: " << strings[0], SAM_ASP);
                MessageElementType secondaryInfo(SubmissionParameterMissingError);
                MessageElementType::substitutionText_sequence& subText =
                  secondaryInfo.substitutionText();
                vector<string>::const_iterator it;
                for (it = strings.begin(); it != strings.end(); it++) {
                    subText.push_back(*it);
                }
                secondaryInfoSeq.push_back(secondaryInfo);
                break;
            }
            case Streams::SubmissionTimeErrors::AmbiguousName: {
                SPCDBG(L_INFO,
                       "Submission time value for name '"
                         << strings[0] << "' is ambiguous, it matches: " << strings[1],
                       SAM_ASP);
                MessageElementType secondaryInfo(SubmissionParameterAmbiguousError);
                MessageElementType::substitutionText_sequence& subText =
                  secondaryInfo.substitutionText();
                vector<string>::const_iterator it;
                for (it = strings.begin(); it != strings.end(); it++) {
                    subText.push_back(*it);
                }
                secondaryInfoSeq.push_back(secondaryInfo);
                break;
            }
            case Streams::SubmissionTimeErrors::DuplicateName: {
                SPCDBG(
                  L_INFO,
                  "Job submission parameter '"
                    << strings[0]
                    << "' was specified multiple times.  A parameter may only be specified once.",
                  SAM_ASP);
                MessageElementType secondaryInfo(SubmissionParameterDuplicateError);
                MessageElementType::substitutionText_sequence& subText =
                  secondaryInfo.substitutionText();
                vector<string>::const_iterator it;
                for (it = strings.begin(); it != strings.end(); it++) {
                    subText.push_back(*it);
                }
                secondaryInfoSeq.push_back(secondaryInfo);
                break;
            }
        }
    }

    if (secondaryInfoSeq.size())
        THROW_WITH_ADDITIONAL_DATA(
          SAM, "There were one or more problems with the supplied job submission parameters:\n",
          messageList, SubmissionParameterError);

    // parms look good...now go write to the submissionTimeValue.value fields
    for (applicationSetType::application_iterator i = adl.application().begin();
         i < adl.application().end(); i++) {
        if (i->submissionTimeValues().present()) {
            bool wasDisplayFound = false, wasDisplaySet = false;
            vector<pair<string, string> >::const_iterator nit;
            for (nit = namedValues.begin(); nit != namedValues.end(); nit++) {
                // find match and write
                const string& n = nit->first;
                bool matchFound = false;
                for (submissionTimeValuesType::submissionTimeValue_iterator stvi =
                       i->submissionTimeValues().get().submissionTimeValue().begin();
                     stvi != i->submissionTimeValues().get().submissionTimeValue().end(); stvi++) {

                    SPCDBG(L_INFO,
                           "Submission time value compare: " << n << " to: " << stvi->name(),
                           SAM_ASP);
                    if (stvi->name() == DISPLAY_VAR_NAME) {
                        wasDisplayFound = true;
                    }
                    if (n == stvi->name()) {
                        // write it
                        string val = nit->second;
                        if (n == DISPLAY_VAR_NAME) {
                            FIX_DISP_HST(val);
                            wasDisplaySet = true;
                        }
                        stvi->value(val);
                        matchFound = true;
                        continue;
                    } else {
                        string qualifiedName = stvi->compositeName() + '.' + stvi->name();
                        // also check name after :: for partial Compsite name spec'd
                        string partialQualified =
                          qualifiedName.substr(qualifiedName.rfind(":") + 1);
                        SPCDBG(L_INFO,
                               "Submission time value compare: " << n << " to: " << qualifiedName
                                                                 << " or " << partialQualified,
                               SAM_ASP);
                        if (qualifiedName == DISPLAY_VAR_NAME ||
                            partialQualified == DISPLAY_VAR_NAME) {
                            wasDisplayFound = true;
                        }
                        if (n == qualifiedName || n == partialQualified) {
                            string val = nit->second;
                            if (n == DISPLAY_VAR_NAME) {
                                FIX_DISP_HST(val);
                                wasDisplaySet = true;
                            }
                            stvi->value(val);
                            matchFound = true;
                            continue;
                        }
                    }
                }
                if (!matchFound && !warningsFlag) { //warnings for extra parms will not be found
                    THROW(SAM, "Submission parameter validation FAILED:",
                          SubmissionParameterValidationError);
                }
            }
            if ((wasDisplayFound ||
                 namedValues.size() ==
                   0 /*need to set display even if no namedvalues were specified*/
                 ) &&
                !wasDisplaySet) {
                //if display was found but not set, we have to 'fix' the value
                for (submissionTimeValuesType::submissionTimeValue_iterator stvi =
                       i->submissionTimeValues().get().submissionTimeValue().begin();
                     stvi != i->submissionTimeValues().get().submissionTimeValue().end(); ++stvi) {
                    string qualifiedName = stvi->compositeName() + '.' + stvi->name();
                    string partialQualified = qualifiedName.substr(qualifiedName.rfind(":") + 1);
                    if (stvi->name() == DISPLAY_VAR_NAME || qualifiedName == DISPLAY_VAR_NAME ||
                        partialQualified == DISPLAY_VAR_NAME) {
                        string val = "";
                        if (stvi->value().present()) {
                            val = stvi->value().get();
                        }
                        FIX_DISP_HST(val);
                        stvi->value(val);
                    }
                }
            }
        }
    }
}

void setSubmissionAppConfigs(ADL::applicationSetType& adl,
                             const vector<pair<string, string> >& namedValues)
{
    SPCDBG(L_DEBUG, "Enter, setSubmissionAppConfigs", SAM_ASP);
    if (!namedValues.empty()) {
        for (applicationSetType::application_iterator i = adl.application().begin();
             i < adl.application().end(); i++) {
            submissionTimeConfigurationType stct;
            ::xsd::cxx::tree::sequence<submissionTimeConfigurationParameterType> stcpts;
            vector<pair<string, string> >::const_iterator nit;
            for (nit = namedValues.begin(); nit != namedValues.end(); nit++) {
                submissionTimeConfigurationParameterType stcpt(nit->first, nit->second);
                stcpts.push_back(stcpt);
                SPCDBG(L_TRACE,
                       "Added config " << nit->first << "=" << nit->second
                                       << " to submissionTimeConfigurationParameter list.",
                       SAM_ASP);
            }
            stct.submissionTimeConfigParm(stcpts);
            i->submissionTimeConfiguration(stct);
            SPCDBG(L_TRACE, "Done setting configs for application " << i->name(), SAM_ASP);
        }
    }
    SPCDBG(L_DEBUG, "Exit, setSubmissionAppConfigs", SAM_ASP);
}

void setSubmissionAdlPath(applicationSetType& adl, const string& adlPath)
{
    SPCDBG(L_DEBUG, "Enter, setSubmissionAdlPath with adlPath " << adlPath, SAM_ASP);
    for (applicationSetType::application_iterator i = adl.application().begin();
         i < adl.application().end(); i++) {
        i->adlPath(adlPath);
        SPCDBG(L_TRACE, "Done setting adlPath for application " << i->name(), SAM_ASP);
    }
    SPCDBG(L_DEBUG, "Exit, setSubmissionAdlPath", SAM_ASP);
}

/*  not needed anymore.
static string makeAbsolute (const string& opath, const string& path)
{
    stringstream ss;
    if(!opath.empty() && opath.at(0) != '/') {
        ss << path << "/";
    }
    ss << opath;
    return ss.str();
}
*/

void updateADLPaths(ADL::applicationSetType& appset, string const& path)
{
    // for all applications in set
    for (applicationSetType::application_iterator app = appset.application().begin();
         app != appset.application().end(); ++app) {

        //fix the data directory
        app->dataDirectory(path);
    }
}

/**
void updateADLHostpools(ADL::applicationSetType& appset)
{
    SPCDBG(L_DEBUG, "Enter", SAM_ASP);
    for(applicationSetType::application_iterator
        app = appset.application().begin();
        app!= appset.application().end(); ++app) {
        bool distributed = true;
        if(app->standaloneApplicationDirectory().present() && !app->standaloneApplicationDirectory().get().empty()) {
          distributed = false;
        }

    	// Update hostpools
        for(hostpoolsType::hostpool_iterator
            hostpool = app->hostpools().hostpool().begin();
            hostpool != app->hostpools().hostpool().end(); ++hostpool) {
            // Resolve the resource ID for the hosts
            for(hostpoolType::host_iterator hi = hostpool->host().begin();
                hi != hostpool->host().end(); ++hi) {
                string hostname = hi->hostname();
                hi->hostname(getResourceIDFromDefinition(hostname, distributed));
            }
        }

        // Update each PE's hostLocation if any.
        for(pesType::pe_iterator peit = app->pes().pe().begin();
        	peit != app->pes().pe().end(); ++peit) {
            for(operInstancesType::operInstance_sequence::iterator
                opit = peit->operInstances().operInstance().begin();
                opit != peit->operInstances().operInstance().end(); ++opit) {
            	if(opit->resources().hostLocation().present()) {
            	    string hostname = opit->resources().hostLocation().get().hostname();
            	    opit->resources().hostLocation().get().hostname(
            	        getResourceIDFromDefinition(hostname, distributed));
            	}
            }
        }
    }
    SPCDBG(L_DEBUG, "Exit", SAM_ASP);
}**/

using namespace xmlns::prod::streams::spl::tupleType;

static bool isScalar(const string& type)
{
    char first = type[0];
    if (first == 's'    // set
        || first == 'l' // list
        || first == 'm' // map
    )
        return false;
    if (first == 't') {
        char second = type[1];
        if (second == 'u' /* tuple */)
            return false;
    }
    return true;
}

static auto_ptr<listOrSetType> doListOrSetType(const string& type);
static auto_ptr<mapType> doMapType(const string& type);
static auto_ptr<tupleType> doTupleType(const string& type);

static void createType(listOrSetType& ls, const string& type)
{
    if (isScalar(type)) {
        ls.elementType(type);
        return;
    }

    switch (type[0]) {
        case 'l':
            ls.list(doListOrSetType(type));
            break;
        case 's':
            ls.set(doListOrSetType(type));
            break;
        case 'm':
            ls.map(doMapType(type));
            break;
        case 't':
            ls.tuple(doTupleType(type));
            break;
    }
}

static auto_ptr<listOrSetType> doListOrSetType(const string& type)
{
    auto_ptr<listOrSetType> ls(new listOrSetType);
    // ('list'|'set') '<' XX '>' ( '[' bound ']' )?
    size_t start = type[0] == 's' ? 4 : 5;
    size_t size = type.size();
    if (type[size - 1] == ']') {
        size_t bracket = type.rfind('[');
        ls->bound(atoll(&type[bracket + 1]));
        createType(*ls, type.substr(start, bracket - start - 1));
    } else
        createType(*ls, type.substr(start, size - start - 1));
    return ls;
}

static auto_ptr<typeType> createTypeType(const string& type)
{
    auto_ptr<typeType> tt(new typeType);
    switch (type[0]) {
        case 'l':
            tt->list(doListOrSetType(type));
            break;
        case 's':
            tt->set(doListOrSetType(type));
            break;
        case 'm':
            tt->map(doMapType(type));
            break;
        case 't':
            tt->tuple(doTupleType(type));
            break;
    }
    return tt;
}

static auto_ptr<mapType> doMapType(const string& type)
{
    auto_ptr<mapType> m(new mapType);
    // 'map<' XX ',' YY '>' ( '[' bound ']' )?
    size_t start = 4;
    size_t size = type.size();
    size_t end = size - 1;
    if (type[end] == ']') {
        size_t bracket = type.rfind('[');
        m->bound(atoll(&type[bracket + 1]));
    }
    // We have start..end as key,value.  Find the middle comma
    size_t comma = start + 1;
    size_t count = 0;
    for (;;) {
        char c = type[comma];
        if (c == '<' || c == '{')
            count++;
        else if (c == '>' || c == '}')
            count--;
        else if (c == ',' && count == 0)
            break;
        comma++;
    }

    // start..comma-1 is key;  comma+1..end is value
    const string& key = type.substr(start, comma - start);
    if (isScalar(key))
        m->keyType(key);
    else
        m->key(createTypeType(key));

    const string& value = type.substr(comma + 1, end - comma - 1);
    if (isScalar(value))
        m->valueType(value);
    else
        m->value(createTypeType(value));
    return m;
}

static void createType(attrType& attr, const string& type)
{
    switch (type[0]) {
        case 'l':
            attr.list(doListOrSetType(type));
            break;
        case 's':
            attr.set(doListOrSetType(type));
            break;
        case 'm':
            attr.map(doMapType(type));
            break;
        case 't':
            attr.tuple(doTupleType(type));
            break;
    }
}

static auto_ptr<tupleType> doTupleType(const string& type)
{
    auto_ptr<tupleType> tt(new tupleType);

    // break down the tuple type into its pieces
    // 'tuple<' TYPE ' ' NAME (',' TYPE ' ' NAME)* '>'
    size_t start = 6;
    size_t size = type.size();
    size_t end = size - 1;

    // break down the fields
    size_t p = start + 1;
    size_t count = 0;
    size_t blank = 0;
    while (p < end) {
        char c = type[p];
        if ((c == ',' || c == '>') && count == 0) {
            // We have hit the end of the name - create the attribute entry
            const string elemType = type.substr(start, blank - start);

            auto_ptr<attrType> a(new attrType);
            a->name(type.substr(blank + 1, p - blank - 1));
            if (isScalar(elemType))
                a->type(elemType);
            else
                createType(*a, elemType);
            tt->attr().push_back(a);
            start = p + 1;
        } else if (c == '<' || c == '{')
            count++;
        else if (c == '>' || c == '}')
            count--;
        else if (c == ' ' && count == 0) {
            // We have hit the end of the type
            blank = p;
        }
        p++;
    }

    return tt;
}

void ensureTupleTypeCreated(tupleTypeType& tt)
{
    // Already there?
    if (tt.tuple().present())
        return;

    // We need to create a tuple clause based on the attributes
    auto_ptr<tupleType> t(new tupleType);
    const tupleTypeType::attribute_sequence& attrs = tt.attribute();
    for (tupleTypeType::attribute_const_iterator it = attrs.begin(); it != attrs.end(); it++) {
        auto_ptr<attrType> a(new attrType);
        a->name(it->name());
        const string& type = it->type();
        if (isScalar(type))
            a->type(type);
        else
            createType(*a, type);

        t->attr().push_back(a);
    }
    tt.tuple(t);
}

string genKey(const streamIdType& sid)
{

    std::ostringstream oss;
    oss << sid.jobId() << "_" << sid.operInstanceName() << "_" << sid.operInstanceOutputPortIndex();

    return oss.str();
}

SAM_NAMESPACE_END
