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

#ifndef SUBSTITUTIONOBJECT_H
#define SUBSTITUTIONOBJECT_H

#include <iostream>
#include <langinfo.h>
#include <locale>
#include <sstream>
#include <string>
#include <unicode/msgfmt.h>
#include <unicode/resbund.h>
#include <unicode/udata.h>
#include <unicode/ustream.h>
#include <vector>

#include <UTILS/SupportFunctions.h>
#include <UTILS/UTILSMessages.h>
#include <UTILS/UTILSTypes.h>

UTILS_NAMESPACE_BEGIN

///  This class is used to wrapper substitution text that isn't all strings,
///  so they can be passed in a vector, to the GetMessage class
class SubstitutionObject
{
  public:
    enum SubstitutionType
    {
        SUB_STRING,
        SUB_INT,
        SUB_DATE,
        SUB_NOT_SET
    };

    // Function declarations

    /// Constructor - Default
    SubstitutionObject()
      : _substitutionInteger(0)
      , _substitutionObjectType(SUB_NOT_SET){};

    /// Constructor that takes an int64_t
    /// @param - an int64_t to set in the object
    SubstitutionObject(uint64_t newInt)
      : _substitutionString("")
    {
        _substitutionObjectType = SUB_INT;
        _substitutionInteger = newInt;
    }

    /// Constructor that takes a string
    /// @param - a string to set in the object
    SubstitutionObject(const std::string& newString)
      : _substitutionInteger(0)
    {
        _substitutionObjectType = SUB_STRING;
        _substitutionString.assign(newString);
    }

    /// Constructor that takes a UDate object
    /// @param - a UDate object to set in the object
    SubstitutionObject(const UDate& newDate)
      : _substitutionString("")
      , _substitutionInteger(0)
    {
        _substitutionObjectType = SUB_DATE;
        _substitutionDate = newDate;
    }

    /// Destructor
    ~SubstitutionObject(){};

    /// Set the object data to be an int64_t
    /// @param - an int64_t to set in the object
    void SetInt(int64_t newInt)
    {
        _substitutionObjectType = SUB_INT;
        _substitutionInteger = newInt;
    };

    /// Set the object data to be a UDate obejct
    /// @param - a UDate object to set in the object
    void SetDate(const UDate& newDate)
    {
        _substitutionObjectType = SUB_DATE;
        _substitutionDate = newDate;
    };

    /// Set the object data to be a string obejct
    /// @param - a string to set in the object

    void SetString(const std::string& newString)
    {
        _substitutionObjectType = SUB_STRING;
        _substitutionString.assign(newString);
    };

    /// Get the current ype stored in the objee=
    /// @return - the type stored in the object.
    SubstitutionType GetType() const { return _substitutionObjectType; };

    /// Get the UDate object stored in the object
    /// @return - the UDate object stored in the object
    UDate GetDate() const
    {
        if (_substitutionObjectType != SUB_DATE) {
            THROW(Utils, "Invalid use of SubstitutionObject.", UTILSUnexpectedException);
        }
        return _substitutionDate;
    };

    /// Get the int64_t object stored in the object.
    /// @return - the int64_t object stored in the object
    int64_t GetInt() const
    {
        if (_substitutionObjectType != SUB_INT) {
            THROW(Utils, "Invalid use of SubstitutionObject.", UTILSUnexpectedException);
        }
        return _substitutionInteger;
    }

    /// get the string object stored in the object.
    /// @return - the string object stored in the object
    const std::string& GetString() const
    {
        if (_substitutionObjectType != SUB_STRING) {
            THROW(Utils, "Invalid use of SubstitutionObject.", UTILSUnexpectedException);
        }
        return _substitutionString;
    }

    /// Print the object
    /// @param - ostream object to print to
    void print(std::ostream& o) const
    {
        switch (_substitutionObjectType) {
            case SUB_STRING:
                o << _substitutionString;
                break;
            case SUB_INT:
                o << _substitutionInteger;
                break;
            case SUB_DATE:
                o << _substitutionDate;
                break;
            default:
                o << "<SubstitutionObject not set>";
                break;
        }
    };

    friend std::ostream& operator<<(std::ostream& o, const SubstitutionObject& i)
    {
        i.print(o);
        return o;
    };

  protected:
  private:
    // class variables

    std::string _substitutionString;
    int64_t _substitutionInteger;
    UDate _substitutionDate;
    SubstitutionType _substitutionObjectType;

}; // End SubstitutionObject class

UTILS_NAMESPACE_END

#endif
