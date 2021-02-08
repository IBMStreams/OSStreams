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

//
// Class Description:
//
// This class provides a container for a Distillery-wide set of control
// knobs

#ifndef DISTILLERYKNOBSET_H
#define DISTILLERYKNOBSET_H

#include <UTILS/DistilleryException.h>
#include <UTILS/UTILSTypes.h>
#include <map>
#include <vector>

UTILS_NAMESPACE_BEGIN

// forward declaration
class DistilleryKnobs;

/// This class stores a knob
class DistilleryKnob
{
  public:
    enum DistilleryKnobType
    {
        INT,
        FLOAT,
        STRING
    };
    DistilleryKnob(const std::string& mykname, const DistilleryKnobType& myktype)
      : kname(mykname)
      , ktype(myktype){};
    DistilleryKnob(const DistilleryKnob& dk)
      : kname(dk.kname)
      , ktype(dk.ktype){};
    virtual DistilleryKnob* clone(void) const = 0;
    virtual ~DistilleryKnob(){};

  protected:
    std::string kname;
    DistilleryKnobType ktype;

    /// print the state of the object to an output stream
    /// @param o the output stream
    virtual void print(std::ostream& o) const { o << kname; };
    /// Print internal state through the << operator
    /// @param o output stream
    /// @param dk a reference to the DistilleryKnob object

    // friend declarations
    friend std::ostream& operator<<(std::ostream& o, const DistilleryKnob& dk);
    friend class DistilleryKnobSet;
};

/// This class stores an integer knob
class DistilleryIntKnob : public DistilleryKnob
{
  public:
    DistilleryIntKnob(const std::string& mykname, const int& mykvalue)
      : DistilleryKnob(mykname, DistilleryKnob::INT)
      , kvalue(mykvalue){};
    DistilleryIntKnob(const DistilleryIntKnob& dk)
      : DistilleryKnob(dk)
      , kvalue(dk.kvalue){};
    virtual DistilleryKnob* clone(void) const { return new DistilleryIntKnob(*this); };
    virtual void print(std::ostream& o) const
    {
        DistilleryKnob::print(o);
        o << "=" << kvalue;
    };

  protected:
    int kvalue;
    // friend declarations
    friend class DistilleryKnobSet;
};

/// This class stores a float knob
class DistilleryFloatKnob : public DistilleryKnob
{
  public:
    DistilleryFloatKnob(const std::string& mykname, const float& mykvalue)
      : DistilleryKnob(mykname, DistilleryKnob::FLOAT)
      , kvalue(mykvalue){};
    DistilleryFloatKnob(const DistilleryFloatKnob& dk)
      : DistilleryKnob(dk)
      , kvalue(dk.kvalue){};
    virtual DistilleryKnob* clone(void) const { return new DistilleryFloatKnob(*this); };
    virtual void print(std::ostream& o) const
    {
        DistilleryKnob::print(o);
        o << "=" << kvalue;
    };

  protected:
    float kvalue;
    // friend declarations
    friend class DistilleryKnobSet;
};
//
/// This class stores a string knob
class DistilleryStringKnob : public DistilleryKnob
{
  public:
    DistilleryStringKnob(const std::string& mykname, const std::string& mykvalue)
      : DistilleryKnob(mykname, DistilleryKnob::STRING)
      , kvalue(mykvalue){};
    DistilleryStringKnob(const DistilleryStringKnob& dk)
      : DistilleryKnob(dk)
      , kvalue(dk.kvalue){};
    virtual DistilleryKnob* clone(void) const { return new DistilleryStringKnob(*this); };
    virtual void print(std::ostream& o) const
    {
        DistilleryKnob::print(o);
        o << "=" << kvalue;
    };

  protected:
    std::string kvalue;
    // friend declarations
    friend class DistilleryKnobSet;
};

/// This class stores a set of control knobs
class DistilleryKnobSet
{
  public:
    /// Constructor
    DistilleryKnobSet(DistilleryKnobs& dk,
                      const std::string& knobSetName,
                      const std::string& parentKnobSet);
    /// Copy Constructor
    DistilleryKnobSet(const DistilleryKnobSet& dks);
    virtual void newIntKnob(const std::string& knobName, const int& knobValue = 0);
    virtual void newFloatKnob(const std::string& knobName, const float& knobValue = 0.0);
    virtual void newStringKnob(const std::string& knobName, const std::string& knobValue = "");
    virtual void setIntKnob(const std::string& knobName,
                            const int& knobValue,
                            const bool propagateToChildren = false);
    virtual void setFloatKnob(const std::string& knobName,
                              const float& knobValue,
                              const bool propagateToChildren = false);
    virtual void setStringKnob(const std::string& knobName,
                               const std::string& knobValue,
                               const bool propagateToChildren = false);
    virtual int getIntKnob(const std::string& knobName) const;
    virtual float getFloatKnob(const std::string& knobName) const;
    virtual std::string getStringKnob(const std::string& knobName) const;
    /// Destructor
    virtual ~DistilleryKnobSet(void);

  protected:
    /// print the state of the object to an output stream
    /// @param o the output stream
    virtual void print(std::ostream& o) const;

    /// Print internal state through the << operator
    /// @param o output stream
    /// @param dks a reference to the DistilleryKnobSet object
    friend std::ostream& operator<<(std::ostream& o, const DistilleryKnobSet& dks);

    std::string knobSetName;
    std::string parentKnobSetName;
    std::map<std::string, DistilleryKnob*> knobs;
    std::vector<DistilleryKnobSet*> children;

    // friend class declarations
    friend class DistilleryKnobs;
};

/// This class stores all control knobs
class DistilleryKnobs
{
  public:
    /// Constructor
    DistilleryKnobs(void);
    virtual void newKnobSet(DistilleryKnobSet* ks);
    virtual void setIntKnob(const std::string& qualifiedKnobName,
                            const int& knobValue,
                            const bool propagateToChildren = false);
    virtual void setFloatKnob(const std::string& qualifiedKnobName,
                              const float& knobValue,
                              const bool propagateToChildren = false);
    virtual void setStringKnob(const std::string& qualifiedKnobName,
                               const std::string& knobValue,
                               const bool propagateToChildren = false);
    virtual int getIntKnob(const std::string& qualifiedKnobName);
    virtual float getFloatKnob(const std::string& qualifiedKnobName);
    virtual std::string getStringKnob(const std::string& qualifiedKnobName);
    virtual const std::map<std::string, DistilleryKnobSet*> getKnobSets(void) { return knobSets; }
    /// Destructor
    virtual ~DistilleryKnobs(void);

  protected:
    /// print the state of the object to an output stream
    /// @param o the output stream
    virtual void print(std::ostream& o) const;

    /// Print internal state through the << operator
    /// @param o output stream
    /// @param dks a reference to the DistilleryKnobSet object
    friend std::ostream& operator<<(std::ostream& o, const DistilleryKnobs& dk);

    virtual void splitQualifiedKnobName(const std::string& qualifiedKnobName,
                                        std::string& knobSetName,
                                        std::string& knobName);

    std::map<std::string, DistilleryKnobSet*> knobSets;

    // friend class declarations
    friend class DistilleryKnobSet;
};

typedef std::map<std::string, DistilleryKnobSet*>::const_iterator ConstDistilleryKnobSetIterator;
typedef std::map<std::string, DistilleryKnobSet*>::iterator DistilleryKnobSetIterator;

typedef std::map<std::string, DistilleryKnob*>::const_iterator ConstDistilleryKnobIterator;
typedef std::map<std::string, DistilleryKnob*>::iterator DistilleryKnobIterator;

typedef std::vector<DistilleryKnobSet*>::const_iterator ConstDistilleryKnobSetChildrenIterator;
typedef std::vector<DistilleryKnobSet*>::iterator DistilleryKnobSetChildrenIterator;

DECL_EXCEPTION(UTILS_NAMESPACE, InvalidKnobSet, Utils);
DECL_EXCEPTION(UTILS_NAMESPACE, InvalidKnob, Utils);

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
