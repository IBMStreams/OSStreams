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

#include <SPL/CodeGen/SPLAnnotation.h>

#include <SPL/CodeGen/CompositeOperatorInstance.h>
#include <SPL/CodeGen/Literal.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/FrontEnd/FrontEndErrors.h>
#include <SPL/FrontEnd/SupportedAnnotations.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/FrontEnd/TypeFactory.h>
#include <SPL/Utility/SysOut.h>

#include <boost/pool/object_pool.hpp>

#include <SPL/Runtime/Utility/Singleton.t>

using namespace std;
using namespace std::tr1;

namespace SPL {
template class Singleton<AnnotationFactory>;
static AnnotationFactory annotationFactory;

// This is here because there is a timing relation between it and the
// simpleAnnotationPool.  The latter must be destroyed before the former.
// TODO: architect this a bit better.  Perhaps a common file for static stuff.
unordered_map<const Literal*, string> Literal::_enumMap;

static boost::object_pool<SPLSimpleAnnotation> simpleAnnotationPool;
static boost::object_pool<SPLViewAnnotation> viewAnnotationPool;
static boost::object_pool<SPLParallelAnnotation> parallelAnnotationPool;
static boost::object_pool<SPLThreadingAnnotation> threadingAnnotationPool;
static boost::object_pool<SPLEventTimeAnnotation> eventTimeAnnotationPool;

AnnotationFactory::AnnotationFactory()
  : Singleton<AnnotationFactory>(*this)
{}

AnnotationFactory::~AnnotationFactory() {}

SPLSimpleAnnotation& AnnotationFactory::createSimpleAnnotation(
  const std::string& t,
  const vector<SPLSimpleAnnotation::KeyValuePair*>& keyValues,
  const SourceLocation& loc)
{
    return *simpleAnnotationPool.construct(t, keyValues, loc);
}

SPLViewAnnotation& AnnotationFactory::createViewAnnotation(
  const vector<SPLSimpleAnnotation::KeyValuePair*>& keyValues,
  const SourceLocation& loc,
  const string& compositeInstanceName)
{
    return *viewAnnotationPool.construct(keyValues, loc, compositeInstanceName);
}

SPLParallelAnnotation& AnnotationFactory::createParallelAnnotation(
  int32_t width_,
  const string& compositeName_,
  const string& submissionTimeName_,
  const string& defaultWidth_,
  const vector<SPLParallelAnnotation::Partition>& partitions_,
  const vector<string>& hostTags_,
  const vector<string>& broadcast_,
  const SourceLocation& loc_)
{
    SPLParallelAnnotation::Params params(width_, compositeName_, submissionTimeName_, defaultWidth_,
                                         partitions_, hostTags_, broadcast_, loc_);
    return *parallelAnnotationPool.construct(params);
}

SPLThreadingAnnotation& AnnotationFactory::createThreadingAnnotation(
  const std::vector<SPLSimpleAnnotation::KeyValuePair*>& keyValues,
  const SourceLocation& loc)
{
    return *threadingAnnotationPool.construct(keyValues, loc);
}

SPLEventTimeAnnotation& AnnotationFactory::createEventTimeAnnotation(
  const std::vector<SPLSimpleAnnotation::KeyValuePair*>& keyValues,
  const SourceLocation& loc)
{
    return *eventTimeAnnotationPool.construct(keyValues, loc);
}

bool SPLParallelAnnotation::parallelRegionAttributes(const string& streamName,
                                                     vector<string>& attributes) const
{
    // The stream name we get in may be qualified.  Remove any qualification
    size_t pos = streamName.find_last_of('.');
    string stream;
    if (pos != string::npos) {
        stream = streamName.substr(pos + 1);
    } else {
        stream = streamName;
    }
    for (vector<Partition>::const_iterator it = _partitions.begin(); it != _partitions.end();
         ++it) {
        const Partition& partition = *it;
        const string& portName = partition.port();
        if (portName == stream) {
            // We found one.  Copy the attributes (keys) over
            attributes = partition.attributes();
            return true;
        }
    }
    return false;
}

static int64_t getIntegerValue(const string& value)
{
    istringstream istr(value, istringstream::in);
    int64_t i;
    istr >> i;
    return i;
}

static double getFloatValue(const string& value)
{
    istringstream istr(value, istringstream::in);
    double d;
    istr >> d;
    return d;
}

void validateUseOfSubmissionTimeValue(const string& name,
                                      const string& key,
                                      const SourceLocation& loc)
{
    const SupportedAnnotation* ann = SupportedAnnotations::instance().find(name);
    assert(NULL != ann);
    for (SupportedAnnotation::Keys::const_iterator it = ann->keys().begin(),
                                                   itEnd = ann->keys().end();
         it != itEnd; ++it) {
        const SupportedAnnotation::Key& k = **it;
        if (k.name() == key) {
            if (!k.submissionTimeValueAllowed()) {
                SysOut::errorln(SPL_FRONT_END_ANNOT_NO_SUB_TIME(key, name), loc);
            }
            break;
        }
    }
}

void SPLViewAnnotation::diagnoseViewAnnotation()
{
    static std::tr1::unordered_map<string, const SourceLocation*> viewAnnotationNames;
    static const string name = "view";
    const SourceLocation& loc = sourceLocation();

    bool bufferSizeSeen = false;
    bool bufferTimeSeen = false;
    int64_t viewSampleSize = 0;
    int64_t viewBufferSize = 0;
    bool bufferSizeValid = false;
    const SourceLocation* bufferSizeLoc = NULL;
    bool sampleSizeValid = false;
    for (vector<KeyValuePair*>::const_iterator it = values().begin(), itEnd = values().end();
         it != itEnd; ++it) {
        const KeyValuePair& kvp = **it;
        const string& key = kvp.key();
        if (!kvp.submissionTimeName().empty()) {
            validateUseOfSubmissionTimeValue(name, key, loc);
            continue;
        }

        const string& value = kvp.value();
        const SourceLocation& valueLoc = kvp.sourceLocation();
        const RootTyp& valueType = kvp.type();
        const MetaType& mType = valueType.getMetaType();
        if ("name" == key) {
            if (MetaType::RSTRING != mType.getValue() && MetaType::USTRING != mType.getValue()) {
                SysOut::errorln(
                  SPL_FRONT_END_ANNOTATION_INCORRECT_TYPE(key, name, "rstring, ustring"), valueLoc);
                continue;
            }
            string newValue = _compositeInstanceName + "." + value.substr(1, value.size() - 2);
            std::tr1::unordered_map<string, const SourceLocation*>::const_iterator it2 =
              viewAnnotationNames.find(newValue);
            if (it2 != viewAnnotationNames.end()) {
                SysOut::errorln(
                  SPL_CORE_ANNOTATION_UNIQUE_CONSTRAINT(value.substr(1, value.size() - 2), name),
                  valueLoc);
                SysOut::detailsErrorln(SPL_CORE_CONFLICTING_ANNOTATION, *it2->second);
            } else {
                viewAnnotationNames[newValue] = &valueLoc;
            }
        } else if ("description" == key) {
            if (MetaType::RSTRING != mType.getValue() && MetaType::USTRING != mType.getValue()) {
                SysOut::errorln(
                  SPL_FRONT_END_ANNOTATION_INCORRECT_TYPE(key, name, "rstring, ustring"), valueLoc);
            }
        } else if ("sampleSize" == key || "bufferSize" == key) {
            if (MetaType::INT32 != mType.getValue()) {
                SysOut::errorln(SPL_FRONT_END_ANNOTATION_INCORRECT_TYPE(key, name, "int32"),
                                valueLoc);
            }
            if ("bufferSize" == key) {
                bufferSizeSeen = true;
                bufferSizeLoc = &valueLoc;
                viewBufferSize = getIntegerValue(value);
                bufferSizeValid = true;
                if (viewBufferSize < 1 || viewBufferSize > INT_MAX) {
                    SysOut::errorln(
                      SPL_FRONT_END_ANNOTATION_INVALID_PARAMETER_VALUE("bufferSize", name),
                      valueLoc);
                    bufferSizeValid = false;
                }
            } else if ("sampleSize" == key) {
                viewSampleSize = getIntegerValue(value);
                sampleSizeValid = true;
                if (viewSampleSize < 1 || viewSampleSize > INT_MAX) {
                    SysOut::errorln(
                      SPL_FRONT_END_ANNOTATION_INVALID_PARAMETER_VALUE("sampleSize", name),
                      valueLoc);
                    sampleSizeValid = false;
                }
            }
        } else if ("bufferTime" == key) {
            if (MetaType::FLOAT64 != mType.getValue()) {
                SysOut::errorln(SPL_FRONT_END_ANNOTATION_INCORRECT_TYPE(key, name, "float64"),
                                valueLoc);
            }
            bufferTimeSeen = true;
            double viewBufferTime = getFloatValue(value);
            if (viewBufferTime <= 0) {
                SysOut::errorln(
                  SPL_FRONT_END_ANNOTATION_INVALID_PARAMETER_VALUE("bufferTime", name), valueLoc);
            }
        } else if ("filter" == key) {
            bool typeError = true;
            bool attrTypeCorrect = true;
            bool filterTypeCorrect = false;
            if (valueType.is<TupleTyp>()) {
                typeError = false;
                const TupleTyp& tType = valueType.as<TupleTyp>();
                for (uint32_t i = 0; i < tType.getNumberOfAttributes(); ++i) {
                    const RootTyp& attrType = tType.getAttributeType(i);
                    if ("attr" == tType.getAttributeName(i)) {
                    } else if ("regEx" == tType.getAttributeName(i)) {
                        if (MetaType::RSTRING == attrType.getMetaType().getValue()) {
                            filterTypeCorrect = true;
                        }
                    } else {
                        typeError = true;
                    }
                }
            }
            if (typeError || !(attrTypeCorrect && filterTypeCorrect)) {
                SysOut::errorln(SPL_FRONT_END_ANNOTATION_INCORRECT_TYPE(
                                  key, name, "tuple<enum attr, rstring regEx>"),
                                valueLoc);
            }
        } else if ("port" == key || "activateOption" == key) {
            if (MetaType::ENUM != mType.getValue()) {
                SysOut::errorln(SPL_FRONT_END_ANNOTATION_INCORRECT_TYPE(key, name, "enum"),
                                valueLoc);
            }
        }
    }
    if (bufferSizeSeen && bufferTimeSeen) {
        SysOut::errorln(SPL_FRONT_END_MUTUALLY_EXCLUSIVE_PARAMETERS("bufferSize", "bufferTime"),
                        loc);
    } else if (!(bufferSizeSeen || bufferTimeSeen)) {
        SysOut::errorln(SPL_FRONT_END_ANNOTATION_REQUIRE_PARAMETERS(name, "bufferSize, bufferTime"),
                        loc);
    }
    if (bufferSizeValid && sampleSizeValid && viewBufferSize < viewSampleSize) {
        assert(NULL != bufferSizeLoc);
        SysOut::errorln(SPL_FRONT_END_ANNOTATION_INVALID_PARAMETER_VALUE("bufferSize", name),
                        *bufferSizeLoc);
    }
}

void SPLSimpleAnnotation::diagnoseConsistentAnnotation()
{
    static const string name = "consistent";
    const SourceLocation& loc = sourceLocation();

    bool isPeriodic = false;
    bool periodSeen = false;
    for (vector<KeyValuePair*>::const_iterator it = _values.begin(), itEnd = _values.end();
         it != itEnd; ++it) {
        const KeyValuePair& kvp = **it;
        const string& key = kvp.key();
        if (!kvp.submissionTimeName().empty()) {
            validateUseOfSubmissionTimeValue(name, key, loc);
            continue;
        }

        const string& value = kvp.value();
        const SourceLocation& valueLoc = kvp.sourceLocation();
        const RootTyp& valueType = kvp.type();
        const MetaType& mType = valueType.getMetaType();
        if ("period" == key) {
            periodSeen = true;
        }
        if ("trigger" == key) {
            if ("\"periodic\"" == value) {
                isPeriodic = true;
            }
        }
        if ("period" == key || "drainTimeout" == key || "resetTimeout" == key) {
            if (MetaType::FLOAT64 != mType.getValue()) {
                SysOut::errorln(SPL_FRONT_END_ANNOTATION_INCORRECT_TYPE(key, name, "float64"),
                                valueLoc);
            }
        } else if ("maxConsecutiveResetAttempts" == key) {
            if (MetaType::INT32 != mType.getValue()) {
                SysOut::errorln(SPL_FRONT_END_ANNOTATION_INCORRECT_TYPE(key, name, "int32"),
                                valueLoc);
            }
        }
    }
    if (isPeriodic && !periodSeen) {
        SysOut::errorln(SPL_FRONT_END_ANNOTATION_MISSING_IMPLICATION("consistent", "trigger",
                                                                     "periodic", "period"),
                        loc);
    } else if (periodSeen && !isPeriodic) {
        SysOut::errorln(SPL_FRONT_END_ANNOTATION_INVALID_IMPLIED_PARAMETER("consistent", "period",
                                                                           "trigger", "periodic"),
                        loc);
    }
}

void SPLSimpleAnnotation::diagnoseTagCategoryOrNoteAnnotation(const string& name)
{
    for (vector<KeyValuePair*>::const_iterator it = _values.begin(), itEnd = _values.end();
         it != itEnd; ++it) {
        const KeyValuePair& kvp = **it;
        const string& key = kvp.key();
        const SourceLocation& valueLoc = kvp.sourceLocation();
        if (!kvp.submissionTimeName().empty()) {
            validateUseOfSubmissionTimeValue(name, key, valueLoc);
            continue;
        }

        const RootTyp& valueType = kvp.type();
        const MetaType& mType = valueType.getMetaType();
        if (MetaType::RSTRING != mType.getValue() && MetaType::USTRING != mType.getValue()) {
            SysOut::errorln(SPL_FRONT_END_ANNOTATION_INCORRECT_TYPE(key, name, "rstring, ustring"),
                            valueLoc);
        }
    }
}

void SPLSimpleAnnotation::diagnoseCatchAnnotation()
{
    for (vector<KeyValuePair*>::const_iterator it = _values.begin(), itEnd = _values.end();
         it != itEnd; ++it) {
        const KeyValuePair& kvp = **it;
        const string& key = kvp.key();
        const SourceLocation& valueLoc = kvp.sourceLocation();
        if ("tupleTrace" == key || "stackTrace" == key) {
            const RootTyp& valueType = kvp.type();
            const MetaType& mType = valueType.getMetaType();
            if (MetaType::BOOLEAN != mType.getValue()) {
                SysOut::errorln(SPL_FRONT_END_ANNOTATION_INCORRECT_TYPE(key, "catch", "boolean"),
                                valueLoc);
            }
        }
    }
}

void SPLEventTimeAnnotation::diagnoseEventTimeAnnotation()
{
    static const std::string name = "eventTime";
    std::vector<KeyValuePair*> const& attributeValues = values();
    for (vector<KeyValuePair*>::const_iterator it = attributeValues.begin();
         it != attributeValues.end(); ++it) {
        KeyValuePair const& kvp = **it;
        string const& key = kvp.key();
        SourceLocation const& valueLoc = kvp.sourceLocation();
        if (!kvp.submissionTimeName().empty()) {
            validateUseOfSubmissionTimeValue(name, key, valueLoc);
            continue;
        }
        RootTyp const& valueType = kvp.type();
        MetaType const& mType = valueType.getMetaType();

        if (key == "eventTimeAttribute") {
            // TODO if we do validation here, we should ensure that
            // eventTimeAttribute is timestamp, uint64, or int64, but I
            //  think that validation is done elsewhere.
            // Further validation will be done in EventTimeRegionBuilder.
        }
        if (key == "resolution") {
            // Must be one of the supported values.
            std::string resolution = kvp.value();
            // Uugh, why does this have quotes?
            if (resolution[0] == '\"' && resolution[resolution.length() - 1] == '\"') {
                resolution = resolution.substr(1, resolution.length() - 2);
            }
            if (resolution != "Milliseconds" && resolution != "Microseconds" &&
                resolution != "Nanoseconds") {
                SysOut::errorln(SPL_FRONT_END_INVALID_EVENTTIME_RESOLUTION, valueLoc);
            }
            // Must also be an enum (quoted strings not allowed).
            if (MetaType::ENUM != mType.getValue()) {
                SysOut::errorln(SPL_FRONT_END_ANNOTATION_INCORRECT_TYPE(key, name, "enum"),
                                valueLoc);
                continue;
            }
        }
        if (key == "lag") {
            if (MetaType::FLOAT64 != mType.getValue()) {
                SysOut::errorln(SPL_FRONT_END_ANNOTATION_INCORRECT_TYPE(key, name, "float64"),
                                valueLoc);
                continue;
            }
        }
        if (key == "minimumGap") {
            if (MetaType::FLOAT64 != mType.getValue()) {
                SysOut::errorln(SPL_FRONT_END_ANNOTATION_INCORRECT_TYPE(key, name, "float64"),
                                valueLoc);
                continue;
            }
        }
    }

    // Other validations are deferred to later in the code gen.
    // (1) The value of the "eventTimeAttribute" key must match
    // at least one of the output attributes of at least one output port.
    // We don't have access to that information, so that validation
    // is done in CodeGen::validateBucketsAttribute (in CodeGen.pm);
    // (2) If there is more than one output attribute matching the
    // the name of the event-time attribute, they must all have the
    // same type.  The type must be one of (timestamp, int64, uint64).
    // That validation is done in EventTimeRegionBuilder.cpp.
    // (3) The annotation may be applied only to a primitive operator.
    // This is verified in EventTimeRegionBuilder::visitComposite
    // in EventTimeRegionBuilder.cpp.
    // (4) If resolution is specified, and the eventtime attribute
    // type is 'timestamp', the resolution must be Nanoseconds.
}

void SPLThreadingAnnotation::diagnoseThreadingAnnotation()
{
    const KeyValuePair* model = NULL;
    const KeyValuePair* threads = NULL;
    const std::vector<KeyValuePair*>& kvps = values();
    for (vector<KeyValuePair*>::const_iterator it = kvps.begin(), itEnd = kvps.end(); it != itEnd;
         ++it) {
        const KeyValuePair& kvp = **it;
        const string& key = kvp.key();
        if ("model" == key) {
            model = &kvp;
        } else if ("threads" == key) {
            threads = &kvp;
        }
    }
    if ("\"dynamic\"" == model->value()) {
        if (NULL != threads) {
            int64_t threadsSize = getIntegerValue(threads->value());
            if (threadsSize < 1 || threadsSize > INT_MAX) {
                const SourceLocation& valueLoc = threads->sourceLocation();
                SysOut::errorln(
                  SPL_FRONT_END_ANNOTATION_INVALID_PARAMETER_VALUE("threads", "threading"),
                  valueLoc);
            }
        }
    }
}

SPLSimpleAnnotation::SPLSimpleAnnotation(
  const std::string& t,
  const std::vector<SPLSimpleAnnotation::KeyValuePair*>& keyValues,
  const SourceLocation& loc)
  : SPLAnnotation(t, SPLAnnotation::Simple, loc)
  , _values(keyValues)
{
    // We need to do diagnostics here because this is the first place we have all the information
    // consolidated
    if ("view" == t) {
        assert(false);
    } else if ("consistent" == t) {
        diagnoseConsistentAnnotation();
    } else if ("spl_tag" == t || "spl_category" == t || "spl_note" == t) {
        diagnoseTagCategoryOrNoteAnnotation(t);
    } else if ("catch" == t) {
        diagnoseCatchAnnotation();
    } else if ("eventTime" == t) {
        assert(false);
    }
}

SPLSimpleAnnotation::SPLSimpleAnnotation(
  const std::string& t,
  const std::vector<SPLSimpleAnnotation::KeyValuePair*>& keyValues,
  const SourceLocation& loc,
  Kind kind)
  : SPLAnnotation(t, kind, loc)
  , _values(keyValues)
{}

SPLViewAnnotation::SPLViewAnnotation(
  const std::vector<SPLSimpleAnnotation::KeyValuePair*>& keyValues,
  const SourceLocation& loc,
  const std::string& compositeInstanceName)
  : SPLSimpleAnnotation("view", keyValues, loc, SPLAnnotation::View)
  , _compositeInstanceName(compositeInstanceName)
{
    diagnoseViewAnnotation();
}

Operator::Instance::Annotation& SPLSimpleAnnotation::createOIAnnotationModel() const
{
    Operator::Instance::Annotation* newAnnot = new Operator::Instance::Annotation;
    newAnnot->name() = tag();
    if (!_values.empty()) {
        // Walk the key/value pairs and build a tuple type
        Operator::Instance::AnnotationTuple* tuple = new Operator::Instance::AnnotationTuple();

        TupleTyp::Attributes attrs;
        for (vector<KeyValuePair*>::const_iterator it = _values.begin(), itEnd = _values.end();
             it != itEnd; ++it) {
            const KeyValuePair& kvp = **it;
            attrs.push_back(make_pair(kvp.key(), &kvp.type()));
            Operator::Instance::AnnotationAttribute* attr =
              new Operator::Instance::AnnotationAttribute();
            attr->name() = kvp.key();
            ostringstream ss;
            ss << kvp.type();
            attr->type() = ss.str();
            attr->value() = kvp.value();
            tuple->attributes().push_back(Operator::Instance::AnnotationAttributePtr(attr));
        }

        const TupleTyp& tt = TypeFactory::instance().tupleT(attrs);
        ostringstream ss1;
        ss1 << tt;
        tuple->type() = ss1.str();
        newAnnot->setTuple(*tuple);
    }
    return *newAnnot;
}

Operator::Instance::Annotation& SPLParallelAnnotation::createOIAnnotationModel() const
{
    Operator::Instance::Annotation* newAnnot = new Operator::Instance::Annotation;
    // This should never be called at this point
    assert(false);
    return *newAnnot;
}

bool SPLAnnotation::shouldBePropagatedToTheOIM() const
{
    // TODO: This will eventully need to be determined from the annotation specification
    if (_tag == "consistentRegionEntry" || _tag == "catch" || _tag == "eventTimeContext" ||
        _tag == "eventTime") {
        return true;
    }
    return false;
}

SPLThreadingAnnotation::SPLThreadingAnnotation(
  const std::vector<SPLSimpleAnnotation::KeyValuePair*>& keyValues,
  const SourceLocation& loc)
  : SPLSimpleAnnotation("threading", keyValues, loc, SPLAnnotation::Threading)
{
    diagnoseThreadingAnnotation();
}

SPLEventTimeAnnotation::SPLEventTimeAnnotation(
  const std::vector<SPLSimpleAnnotation::KeyValuePair*>& keyValues,
  const SourceLocation& loc)
  : SPLSimpleAnnotation("eventTime", keyValues, loc, SPLAnnotation::EventTime)
{
    diagnoseEventTimeAnnotation();
}

} // namsepace SPL
