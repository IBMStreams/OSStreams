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

#include <cassert>
#include <iostream>
#include <string>
#include <typeinfo>

#include <UTILS/DistilleryException.h>
#include <UTILS/DistilleryKnobs.h>

UTILS_NAMESPACE_USE
using namespace std;

// --------- DistilleryKnobSet class
//
DistilleryKnobSet::DistilleryKnobSet(DistilleryKnobs& mydk,
                                     const string& myKnobSetName,
                                     const string& myParentKnobSetName)
  : knobSetName(myKnobSetName)
  , parentKnobSetName(myParentKnobSetName)
{

    // if there is a dot, we should barf as that is a reserved character
    if (knobSetName.find(".") != string::npos) {
        THROW(InvalidKnobSet, "invalid character in knob set name '" + knobSetName + "'");
    }

    // reserved name
    if (knobSetName == "root") {
        THROW(InvalidKnobSet, "name 'root' is reserved");
    }

    // is this set already declared
    ConstDistilleryKnobSetIterator dksi = mydk.knobSets.find(knobSetName);
    if (dksi != mydk.knobSets.end()) {
        THROW(InvalidKnobSet, "knob set '" + knobSetName + "' is already defined");
    }

    // defining same knobs as defined by this knob's root
    ConstDistilleryKnobSetIterator pdksi;
    if (parentKnobSetName != "root") {
        // checking if parent knob exists
        pdksi = mydk.knobSets.find(parentKnobSetName);
        if (pdksi == mydk.knobSets.end()) {
            THROW(InvalidKnobSet, "knob set's parent '" + parentKnobSetName + "' is not defined");
        }

        // storing the parentage information
        pdksi->second->children.push_back(this);

        // retrieve parent's knobs and replicate
        for (ConstDistilleryKnobIterator dki = pdksi->second->knobs.begin();
             dki != pdksi->second->knobs.end(); ++dki) {
            knobs.insert(make_pair(dki->first, dki->second->clone()));
        }
    }
}

DistilleryKnobSet::DistilleryKnobSet(const DistilleryKnobSet& dks)
  : knobSetName(dks.knobSetName)
  , parentKnobSetName(dks.parentKnobSetName)
{

    for (ConstDistilleryKnobIterator dki = dks.knobs.begin(); dki != dks.knobs.end(); ++dki) {
        knobs.insert(make_pair(dki->first, dki->second->clone()));
    }

    for (ConstDistilleryKnobSetChildrenIterator ci = dks.children.begin(); ci != dks.children.end();
         ++ci) {
        children.push_back(*ci);
    }
}

void DistilleryKnobSet::newIntKnob(const string& knobName, const int& knobValue)
{
    ConstDistilleryKnobIterator dki = knobs.find(knobName);
    if (dki != knobs.end()) {
        THROW(InvalidKnob, "knob '" + knobName + "' is already defined");
    }
    knobs.insert(make_pair(knobName, new DistilleryIntKnob(knobName, knobValue)));
}

void DistilleryKnobSet::newFloatKnob(const string& knobName, const float& knobValue)
{
    ConstDistilleryKnobIterator dki = knobs.find(knobName);
    if (dki != knobs.end()) {
        THROW(InvalidKnob, "knob '" + knobName + "' is already defined");
    }
    knobs.insert(make_pair(knobName, new DistilleryFloatKnob(knobName, knobValue)));
}

void DistilleryKnobSet::newStringKnob(const string& knobName, const string& knobValue)
{
    ConstDistilleryKnobIterator dki = knobs.find(knobName);
    if (dki != knobs.end()) {
        THROW(InvalidKnob, "knob '" + knobName + "' is already defined");
    }
    knobs.insert(make_pair(knobName, new DistilleryStringKnob(knobName, knobValue)));
}

void DistilleryKnobSet::setIntKnob(const string& knobName,
                                   const int& knobValue,
                                   const bool propagateToChildren)
{
    DistilleryKnobIterator dki = knobs.find(knobName);
    if (dki == knobs.end()) {
        THROW(InvalidKnob, "knob '" + knobName + "' is not defined");
    }
    try {
        DistilleryIntKnob& d = dynamic_cast<DistilleryIntKnob&>(*(dki->second));
        d.kvalue = knobValue;
    } catch (bad_cast) {
        THROW(InvalidKnob, "knob '" + knobName + "' is not an integer value");
    }

    // propagating to children knobs
    if (propagateToChildren) {
        for (DistilleryKnobSetChildrenIterator ci = children.begin(); ci != children.end(); ++ci) {
            (*ci)->setIntKnob(knobName, knobValue, true);
        }
    }
}

void DistilleryKnobSet::setFloatKnob(const string& knobName,
                                     const float& knobValue,
                                     const bool propagateToChildren)
{
    DistilleryKnobIterator dki = knobs.find(knobName);
    if (dki == knobs.end()) {
        THROW(InvalidKnob, "knob '" + knobName + "' is not defined");
    }
    try {
        DistilleryFloatKnob& d = dynamic_cast<DistilleryFloatKnob&>(*(dki->second));
        d.kvalue = knobValue;
    } catch (bad_cast) {
        THROW(InvalidKnob, "knob '" + knobName + "' is not a floating point value");
    }

    // propagating to children knobs
    if (propagateToChildren) {
        for (DistilleryKnobSetChildrenIterator ci = children.begin(); ci != children.end(); ++ci) {
            (*ci)->setFloatKnob(knobName, knobValue, true);
        }
    }
}

void DistilleryKnobSet::setStringKnob(const string& knobName,
                                      const string& knobValue,
                                      const bool propagateToChildren)
{
    DistilleryKnobIterator dki = knobs.find(knobName);
    if (dki == knobs.end()) {
        THROW(InvalidKnob, "knob '" + knobName + "' is not defined");
    }
    try {
        DistilleryStringKnob& d = dynamic_cast<DistilleryStringKnob&>(*(dki->second));
        d.kvalue = knobValue;
    } catch (bad_cast) {
        THROW(InvalidKnob, "knob '" + knobName + "' is not a string value");
    }

    // propagating to children knobs
    if (propagateToChildren) {
        for (DistilleryKnobSetChildrenIterator ci = children.begin(); ci != children.end(); ++ci) {
            (*ci)->setStringKnob(knobName, knobValue, true);
        }
    }
}

int DistilleryKnobSet::getIntKnob(const string& knobName) const
{
    ConstDistilleryKnobIterator dki = knobs.find(knobName);
    if (dki == knobs.end()) {
        THROW(InvalidKnob, "knob '" + knobName + "' is not defined");
    }
    try {
        DistilleryIntKnob& d = dynamic_cast<DistilleryIntKnob&>(*(dki->second));
        return d.kvalue;
    } catch (bad_cast) {
        THROW(InvalidKnob, "knob '" + knobName + "' is not an integer value");
    }
    return 0;
}

float DistilleryKnobSet::getFloatKnob(const string& knobName) const
{
    ConstDistilleryKnobIterator dki = knobs.find(knobName);
    if (dki == knobs.end()) {
        THROW(InvalidKnob, "knob '" + knobName + "' is not defined");
    }
    try {
        DistilleryFloatKnob& d = dynamic_cast<DistilleryFloatKnob&>(*(dki->second));
        return d.kvalue;
    } catch (bad_cast) {
        THROW(InvalidKnob, "knob '" + knobName + "' is not a floating point value");
    }
    return 0;
}

string DistilleryKnobSet::getStringKnob(const string& knobName) const
{
    ConstDistilleryKnobIterator dki = knobs.find(knobName);
    if (dki == knobs.end()) {
        THROW(InvalidKnob, "knob '" + knobName + "' is not defined");
    }
    try {
        DistilleryStringKnob& d = dynamic_cast<DistilleryStringKnob&>(*(dki->second));
        return d.kvalue;
    } catch (bad_cast) {
        THROW(InvalidKnob, "knob '" + knobName + "' is not a string value");
    }
    return 0;
}

void DistilleryKnobSet::print(ostream& o) const
{
    for (ConstDistilleryKnobIterator dki = knobs.begin(); dki != knobs.end(); ++dki) {
        o << knobSetName << "." << *(dki->second) << endl;
    }
}

ostream& UTILS_NAMESPACE::operator<<(ostream& o, const DistilleryKnobSet& dks)
{
    dks.print(o);
    return o;
}

ostream& UTILS_NAMESPACE::operator<<(ostream& o, const DistilleryKnob& dk)
{
    dk.print(o);
    return o;
}

DistilleryKnobSet::~DistilleryKnobSet(void)
{
    for (DistilleryKnobIterator dki = knobs.begin(); dki != knobs.end(); ++dki) {
        delete dki->second;
        dki->second = NULL;
    }
    knobs.clear();
}

// --------- DistilleryKnobs class
//

DistilleryKnobs::DistilleryKnobs(void) {}

void DistilleryKnobs::splitQualifiedKnobName(const string& qualifiedKnobName,
                                             string& knobSetName,
                                             string& knobName)
{
    int t = qualifiedKnobName.find(".");
    if (qualifiedKnobName.find(".") == string::npos) {
        THROW(InvalidKnob, "cannot parse '" + qualifiedKnobName + "'");
    }
    knobSetName = qualifiedKnobName.substr(0, t);
    knobName = qualifiedKnobName.substr(t + 1);
}

void DistilleryKnobs::newKnobSet(DistilleryKnobSet* ks)
{
    assert(ks);
    // is this set already declared
    ConstDistilleryKnobSetIterator dksi = knobSets.find(ks->knobSetName);
    if (dksi != knobSets.end()) {
        THROW(InvalidKnobSet, "knob set '" + ks->knobSetName + "' is already defined");
    }
    // inserting new knob set
    knobSets.insert(make_pair(ks->knobSetName, ks));
}

void DistilleryKnobs::setIntKnob(const string& qualifiedKnobName,
                                 const int& knobValue,
                                 const bool propagateToChildren)
{
    string knobSetName, knobName;
    splitQualifiedKnobName(qualifiedKnobName, knobSetName, knobName);
    ConstDistilleryKnobSetIterator dksi = knobSets.find(knobSetName);
    if (dksi == knobSets.end()) {
        THROW(InvalidKnobSet, "knob set '" + knobSetName + "' is not defined");
    }
    dksi->second->setIntKnob(knobName, knobValue);
}

void DistilleryKnobs::setFloatKnob(const string& qualifiedKnobName,
                                   const float& knobValue,
                                   const bool propagateToChildren)
{
    string knobSetName, knobName;
    splitQualifiedKnobName(qualifiedKnobName, knobSetName, knobName);
    ConstDistilleryKnobSetIterator dksi = knobSets.find(knobSetName);
    if (dksi == knobSets.end()) {
        THROW(InvalidKnobSet, "knob set '" + knobSetName + "' is not defined");
    }
    dksi->second->setFloatKnob(knobName, knobValue, propagateToChildren);
}

void DistilleryKnobs::setStringKnob(const string& qualifiedKnobName,
                                    const string& knobValue,
                                    const bool propagateToChildren)
{
    string knobSetName, knobName;
    splitQualifiedKnobName(qualifiedKnobName, knobSetName, knobName);
    ConstDistilleryKnobSetIterator dksi = knobSets.find(knobSetName);
    if (dksi == knobSets.end()) {
        THROW(InvalidKnobSet, "knob set '" + knobSetName + "' is not defined");
    }
    dksi->second->setStringKnob(knobName, knobValue, propagateToChildren);
}

int DistilleryKnobs::getIntKnob(const string& qualifiedKnobName)
{
    string knobSetName, knobName;
    splitQualifiedKnobName(qualifiedKnobName, knobSetName, knobName);
    ConstDistilleryKnobSetIterator dksi = knobSets.find(knobSetName);
    if (dksi == knobSets.end()) {
        THROW(InvalidKnobSet, "knob set '" + knobSetName + "' is not defined");
    }
    return dksi->second->getIntKnob(knobName);
}

float DistilleryKnobs::getFloatKnob(const string& qualifiedKnobName)
{
    string knobSetName, knobName;
    splitQualifiedKnobName(qualifiedKnobName, knobSetName, knobName);
    ConstDistilleryKnobSetIterator dksi = knobSets.find(knobSetName);
    if (dksi == knobSets.end()) {
        THROW(InvalidKnobSet, "knob set '" + knobSetName + "' is not defined");
    }
    return dksi->second->getFloatKnob(knobName);
}

string DistilleryKnobs::getStringKnob(const string& qualifiedKnobName)
{
    string knobSetName, knobName;
    splitQualifiedKnobName(qualifiedKnobName, knobSetName, knobName);
    ConstDistilleryKnobSetIterator dksi = knobSets.find(knobSetName);
    if (dksi == knobSets.end()) {
        THROW(InvalidKnobSet, "knob set '" + knobSetName + "' is not defined");
    }
    return dksi->second->getStringKnob(knobName);
}

void DistilleryKnobs::print(ostream& o) const
{
    for (ConstDistilleryKnobSetIterator dksi = knobSets.begin(); dksi != knobSets.end(); ++dksi) {
        o << "[" << dksi->first << "]" << endl;
        dksi->second->print(o);
    }
}

DistilleryKnobs::~DistilleryKnobs(void)
{
    for (DistilleryKnobSetIterator dksi = knobSets.begin(); dksi != knobSets.end(); ++dksi) {
        delete dksi->second;
        dksi->second = NULL;
    }
    knobSets.clear();
}

ostream& UTILS_NAMESPACE::operator<<(ostream& o, const DistilleryKnobs& dk)
{
    dk.print(o);
    return o;
}

IMPL_EXCEPTION(Distillery, InvalidKnobSet, Utils);
IMPL_EXCEPTION(Distillery, InvalidKnob, Utils);
