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

#ifndef SPL_ANNOTATION_H
#define SPL_ANNOTATION_H

#include <SPL/Core/OperatorInstanceModelImpl.h>
#include <SPL/Runtime/Utility/Singleton.h>

#include <cassert>
#include <string>
#include <vector>

namespace SPL {

class RootTyp;
class SourceLocation;

/// This class represents an instance of an SPL annotation
class SPLAnnotation
{
  public:
    enum Kind
    {
        Simple,
        Parallel,
        View,
        Threading,
        EventTime,
        numAnnotationKinds
    };

    virtual ~SPLAnnotation() {}

    Kind getKind() const { return _kind; }

    const std::string& tag() const { return _tag; }

    /// Declares the primary template for a method to query if the object is one of a specific kind.
    /// @return Returns true if the object is of the specific kind, otherwise false.
    template<class T>
    bool is() const;

    /// Declares the primary template for a const downcaster
    /// Before the downcaster is used you must verify that the object is of the desired type using
    /// the as method
    template<class T>
    const T& as() const;

    /// Declares the primary template for a non-const downcaster
    /// Before the downcaster is used you must verify that the object is of the desired type using
    /// the as method
    template<class T>
    T& as();

    /// Creates an instance of an Operator Instance model Annotation object
    virtual Operator::Instance::Annotation& createOIAnnotationModel() const = 0;

    /// Determines if this annotation needs to be propagated to the Operator Instance Model
    bool shouldBePropagatedToTheOIM() const;

    /// Get the defining source location of this annotation
    const SourceLocation& sourceLocation() const { return _loc; }

  protected:
    SPLAnnotation(const std::string& t, Kind kind, const SourceLocation& loc)
      : _tag(t)
      , _kind(kind)
      , _loc(loc)
    {}

  private:
    std::string _tag;
    Kind _kind;
    const SourceLocation& _loc;
};
#define ANNOT_IS_AS_SPECIALIZATIONS(isEnum, asType)                                                \
    template<>                                                                                     \
    inline bool SPLAnnotation::is<asType>() const                                                  \
    {                                                                                              \
        return getKind() == SPLAnnotation::isEnum;                                                 \
    }                                                                                              \
    template<>                                                                                     \
    inline asType& SPLAnnotation::as<asType>()                                                     \
    {                                                                                              \
        assert(dynamic_cast<asType*>(this) != 0);                                                  \
        return *static_cast<asType*>(this);                                                        \
    }                                                                                              \
    template<>                                                                                     \
    inline const asType& SPLAnnotation::as<asType>() const                                         \
    {                                                                                              \
        assert(dynamic_cast<const asType*>(this) != 0);                                            \
        return *static_cast<const asType*>(this);                                                  \
    }

/// An SPL annotation appears as @Tag(key1="value1", key2="value2")
/// The parenthesized expression is optional and contains one or more key/value pairs.
/// The values must be either a string or a ustring.
class SPLSimpleAnnotation : public SPLAnnotation
{
  public:
    /// @param t The name of this annotation
    /// TODO: Add the type of the value (rstring or ustring)
    virtual ~SPLSimpleAnnotation()
    {
        std::vector<KeyValuePair*>::const_iterator it;
        for (it = _values.begin(); it != _values.end(); ++it) {
            delete *it;
        }
    }

    class KeyValuePair
    {
      public:
        KeyValuePair(const std::string& k,
                     const std::string& v,
                     const std::string& cn,
                     const std::string& sn,
                     const std::string& def,
                     const RootTyp& t,
                     const SourceLocation& loc)
          : _key(k)
          , _value(v)
          , _compositeName(cn)
          , _substitutionName(sn)
          , _defaultValue(def)
          , _type(t)
          , _loc(loc)
        {}

        KeyValuePair(const std::string& k,
                     const std::string& v,
                     const RootTyp& t,
                     const SourceLocation& loc)
          : _key(k)
          , _value(v)
          , _compositeName()
          , _substitutionName()
          , _defaultValue()
          , _type(t)
          , _loc(loc)
        {}

        KeyValuePair(const KeyValuePair& other)
          : _key(other._key)
          , _value(other._value)
          , _compositeName(other._compositeName)
          , _substitutionName(other._substitutionName)
          , _defaultValue(other._defaultValue)
          , _type(other._type)
          , _loc(other._loc)
        {}

        const std::string& key() const { return _key; }
        const std::string& value() const { return _value; }
        const std::string& compositeName() const { return _compositeName; }
        const std::string& submissionTimeName() const { return _substitutionName; }
        const std::string& defaultValue() const { return _defaultValue; }
        const RootTyp& type() const { return _type; }
        void setValue(const std::string& newVal) const { _value = newVal; }
        const SourceLocation& sourceLocation() const { return _loc; }

      private:
        std::string _key;
        mutable std::string _value;
        std::string _compositeName; // For substitution value
        std::string _substitutionName;
        std::string _defaultValue;
        const RootTyp& _type;
        const SourceLocation& _loc;
    };

    /// ctor
    SPLSimpleAnnotation(const std::string& t,
                        const std::vector<KeyValuePair*>& keyValues,
                        const SourceLocation& loc);

    /// Access the key/value pairs
    /// @return
    const std::vector<KeyValuePair*>& values() const { return _values; }
    std::vector<KeyValuePair*>& values() { return _values; }

    virtual Operator::Instance::Annotation& createOIAnnotationModel() const;

  protected:
    SPLSimpleAnnotation(const std::string& t,
                        const std::vector<KeyValuePair*>& keyValues,
                        const SourceLocation& loc,
                        Kind kind);

  private:
    void diagnoseConsistentAnnotation();
    void diagnoseCatchAnnotation();
    void diagnoseTagCategoryOrNoteAnnotation(const std::string& tag);

    std::vector<KeyValuePair*> _values;
};
ANNOT_IS_AS_SPECIALIZATIONS(Simple, SPLSimpleAnnotation)

class SPLViewAnnotation : public SPLSimpleAnnotation
{
  public:
    /// ctor
    SPLViewAnnotation(const std::vector<SPLSimpleAnnotation::KeyValuePair*>& keyValues,
                      const SourceLocation& loc,
                      const std::string& compositeInstanceName);

  private:
    void diagnoseViewAnnotation();
    std::string _compositeInstanceName;
};
ANNOT_IS_AS_SPECIALIZATIONS(View, SPLViewAnnotation)

/// This class represents an instance of an spl_parallel annotation
class SPLParallelAnnotation : public SPLAnnotation
{
  public:
    class Partition
    {
      public:
        Partition(const std::string& portIn, const std::vector<std::string>& attributesIn)
          : _port(portIn)
          , _attributes(attributesIn)
        {}
        const std::string& port() const { return _port; }
        const std::vector<std::string>& attributes() const { return _attributes; }

      private:
        std::string _port;
        std::vector<std::string> _attributes;
    };

    /// This struct is used to pass constructor params because boost only allows 3 when using pools
    struct Params
    {
        Params(int32_t width_,
               const std::string& compositeName_,
               const std::string& submissionTimeName_,
               const std::string& defaultWidth_,
               const std::vector<SPLParallelAnnotation::Partition>& partitions_,
               const std::vector<std::string>& hostTags_,
               const std::vector<std::string>& broadcasts_,
               const SourceLocation& loc_)
          : _width(width_)
          , _compositeName(compositeName_)
          , _submissionTimeName(submissionTimeName_)
          , _defaultWidth(defaultWidth_)
          , _partitions(partitions_)
          , _hostTags(hostTags_)
          , _broadcasts(broadcasts_)
          , _loc(loc_)
        {}

        int32_t _width;
        const std::string& _compositeName;
        const std::string& _submissionTimeName;
        const std::string& _defaultWidth;
        const std::vector<SPLParallelAnnotation::Partition>& _partitions;
        const std::vector<std::string>& _hostTags;
        const std::vector<std::string>& _broadcasts;
        const SourceLocation& _loc;
    };

    /// Create an SPLParallelAnnotation object
    /// @param widthIn The width of the parallel region.  This is valid if submissionTimeName is
    /// empty, or if defaultWidthSpecified
    /// @param submissionTimeName If not empty then it contains the name for the submission time
    /// value
    /// @param defaultWidthSpecified If true then the width is actually the default to be used in
    /// the absense of a submission-time value
    SPLParallelAnnotation(const Params& params)
      : SPLAnnotation("parallel", SPLAnnotation::Parallel, params._loc)
      , _width(params._width)
      , _defaultWidth(params._defaultWidth)
      , _compositeName(params._compositeName)
      , _submissionTimeName(params._submissionTimeName)
      , _partitions(params._partitions)
      , _broadcasts(params._broadcasts)
      , _hostTags(params._hostTags)
    {}

    /// Access the width for this parallel region
    int32_t width() const { return _width; }

    /// The default with if it was specified
    const std::string& defaultWidth() const { return _defaultWidth; }

    /// Access the sumission-time name.  The name was not specified it the returned string is empty
    const std::string& submissionTimeName() const { return _submissionTimeName; }

    /// Access the name of the composite in which the @parallel was specified
    const std::string& compositeName() const { return _compositeName; }

    /// Access the list of partitions for this region
    const std::vector<Partition>& partitions() const { return _partitions; }

    /// Find any partitioning attributes associated with the give stream name
    bool parallelRegionAttributes(const std::string& streamName,
                                  std::vector<std::string>& attributes) const;

    /// Access the list of broadcasts for this region
    const std::vector<std::string>& broadcasts() const { return _broadcasts; }

    /// Return the set of host tags associated with this annotation
    /// The list will be empty if no host tags are specified
    const std::vector<std::string>& hostTags() const { return _hostTags; }

    virtual Operator::Instance::Annotation& createOIAnnotationModel() const;

  private:
    int32_t _width;
    std::string _defaultWidth;
    std::string _compositeName;
    std::string _submissionTimeName;
    std::vector<Partition> _partitions;
    std::vector<std::string> _broadcasts;
    std::vector<std::string> _hostTags;
};
ANNOT_IS_AS_SPECIALIZATIONS(Parallel, SPLParallelAnnotation)

class SPLThreadingAnnotation : public SPLSimpleAnnotation
{
  public:
    SPLThreadingAnnotation(const std::vector<SPLSimpleAnnotation::KeyValuePair*>& keyValues,
                           const SourceLocation& loc);

  private:
    void diagnoseThreadingAnnotation();
};
ANNOT_IS_AS_SPECIALIZATIONS(Threading, SPLThreadingAnnotation)

class SPLEventTimeAnnotation : public SPLSimpleAnnotation
{
  public:
    SPLEventTimeAnnotation(std::vector<SPLSimpleAnnotation::KeyValuePair*> const& keyValues,
                           SourceLocation const& loc);
    void diagnoseEventTimeAnnotation();
};
ANNOT_IS_AS_SPECIALIZATIONS(EventTime, SPLEventTimeAnnotation)

class AnnotationFactory : public Singleton<AnnotationFactory>
{
  public:
    AnnotationFactory();
    virtual ~AnnotationFactory();
    static SPLSimpleAnnotation& createSimpleAnnotation(
      const std::string& t,
      const std::vector<SPLSimpleAnnotation::KeyValuePair*>& keyValues,
      const SourceLocation& loc);
    static SPLViewAnnotation& createViewAnnotation(
      const std::vector<SPLSimpleAnnotation::KeyValuePair*>& keyValues,
      const SourceLocation& loc,
      const std::string& compositeInstanceName);
    static SPLParallelAnnotation& createParallelAnnotation(
      int32_t width_,
      const std::string& compositeName_,
      const std::string& submissionTimeName_,
      const std::string& defaultWidth_,
      const std::vector<SPLParallelAnnotation::Partition>& partitions_,
      const std::vector<std::string>& hostTags_,
      const std::vector<std::string>& broadcast,
      const SourceLocation& loc);
    static SPLThreadingAnnotation& createThreadingAnnotation(
      const std::vector<SPLSimpleAnnotation::KeyValuePair*>& keyValues,
      const SourceLocation& loc);
    static SPLEventTimeAnnotation& createEventTimeAnnotation(
      const std::vector<SPLSimpleAnnotation::KeyValuePair*>& keyValues,
      const SourceLocation& loc);
};

} // namespace SPL

#endif // SPL_ANNOTATION_H
