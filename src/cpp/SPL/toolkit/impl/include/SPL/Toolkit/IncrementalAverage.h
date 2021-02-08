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

#ifndef SPL_TOOLKIT_INCREMENTAL_AVERAGE_H
#define SPL_TOOLKIT_INCREMENTAL_AVERAGE_H

#include <SPL/Toolkit/Serialization.h>

namespace SPL
{
    namespace IncrementalAverage
    {
        /*
         * Helper classes to implement Averages with more numerical stability
         */
        template<typename T>
        class FloatingPointAverage {
          public:
            FloatingPointAverage() : _currentAverage(0), _count(0) {}
            void addValue (T newValue)
            {
                _currentAverage += (newValue - _currentAverage) / static_cast<T>(_count + 1);
                _count++;
            }

            T operator() () const { return _currentAverage;}
            int64_t count() const { return _count; };

          protected:
            friend class SPL::Serialization::Access;
            template<typename Archive>
            void save(Archive & ar) const { ar << _count << _currentAverage; };
            template<typename Archive>
            void load(Archive & ar) { ar >> _count >> _currentAverage; };

          private:
            T _currentAverage;
            int64_t _count;
        };

        template<typename T>
        class IntegerAverage {
          public:
            IntegerAverage() : _extra(0), _count(0), _currentAverage(0) {}
            void addValue (T newValue)
            {
                if (newValue >= _currentAverage)
                    _extra += newValue - _currentAverage;
                else
                    _extra -= _currentAverage - newValue;
                int64_t absoluteExtra = (_extra>=0) ? _extra : -_extra;
                if (absoluteExtra >= (_count+1)) {
                    _currentAverage += _extra / (_count+1);
                    absoluteExtra = absoluteExtra % (_count+1);
                    _extra = (_extra>=0) ? absoluteExtra : -absoluteExtra;
                }
                _count++;
            }

            T operator() () const
            {
                if (_currentAverage>0) {
                    if (_extra<0)
                        return _currentAverage-1;
                } else if (_currentAverage<0) {
                    if (_extra>0)
                        return _currentAverage+1;
                }
                return _currentAverage;
            }
            int64_t count() const { return _count;};

          protected:
            friend class SPL::Serialization::Access;
            template<typename Archive>
            void save(Archive & ar) const { ar << _extra << _count << _currentAverage; };
            template<typename Archive>
            void load(Archive & ar) { ar >> _extra >> _count >> _currentAverage; };

          private:
            int64_t _extra;
            int64_t _count;
            T _currentAverage;
        };

        template<typename T>
        class UnsignedIntegerAverage {
          public:
            UnsignedIntegerAverage() : _extra(0), _count(0), _currentAverage(0) {}
            void addValue (T newValue)
            {
                if (newValue >= _currentAverage)
                    _extra += newValue - _currentAverage;
                else
                    _extra -= _currentAverage - newValue;
                int64_t absoluteExtra = (_extra>=0) ? _extra : -_extra;
                if (absoluteExtra >= (_count+1)) {
                    _currentAverage += _extra / (_count+1);
                    absoluteExtra = absoluteExtra % (_count+1);
                    _extra = (_extra>=0) ? absoluteExtra : -absoluteExtra;
                }
                _count++;
            }

            T operator() () const
            {
                if (_currentAverage>0) {
                    if (_extra<0)
                        return _currentAverage-1;
                }
                return _currentAverage;
            }
            int64_t count() const { return _count;};

          protected:
            friend class SPL::Serialization::Access;
            template<typename Archive>
            void save(Archive & ar) const { ar << _extra << _count << _currentAverage; };
            template<typename Archive>
            void load(Archive & ar) { ar >> _extra >> _count >> _currentAverage; };

          private:
            int64_t _extra;
            int64_t _count;
            T _currentAverage;
        };

        template<typename T>
        class ComplexAverage {
          public:
            ComplexAverage() : _count(0), _sum(0) {}
            void addValue (T newValue)
            {
                _sum += newValue;
                _count++;
            }

            T operator() () const
            {
                return _sum / static_cast<T>(_count);
            }
            int64_t count() const { return _count;};

          protected:
            friend class SPL::Serialization::Access;
            template<typename Archive>
            void save(Archive & ar) const { ar << _count << _sum; };
            template<typename Archive>
            void load(Archive & ar) { ar >> _count >> _sum; };

          private:
            int64_t _count;
            T _sum;
        };

        // Interface for general use
        template<typename T> class IA : public IntegerAverage<T> {};

        // Handle the FP one using FP code
        template<> class IA<SPL::float32> : public FloatingPointAverage<SPL::float32> {};
        template<> class IA<SPL::float64> : public FloatingPointAverage<SPL::float64> {};
        template<> class IA<SPL::decimal32> : public FloatingPointAverage<SPL::decimal32> {};
        template<> class IA<SPL::decimal64> : public FloatingPointAverage<SPL::decimal64> {};
        template<> class IA<SPL::decimal128> : public FloatingPointAverage<SPL::decimal128> {};

        // Complex isn't ordered, so we have to do it the 'bad' way
        template<> class IA<SPL::complex32> : public ComplexAverage<SPL::complex32> {};
        template<> class IA<SPL::complex64> : public ComplexAverage<SPL::complex64> {};

        // Unsigned integers have to avoid a warning
        template<> class IA<SPL::uint8> : public UnsignedIntegerAverage<SPL::uint8> {};
        template<> class IA<SPL::uint16> : public UnsignedIntegerAverage<SPL::uint16> {};
        template<> class IA<SPL::uint32> : public UnsignedIntegerAverage<SPL::uint32> {};
        template<> class IA<SPL::uint64> : public UnsignedIntegerAverage<SPL::uint64> {};

    } // namespace IncrementalAverage
} // namespace SPL

#endif /*SPL_TOOLKIT_INCREMENTAL_AVERAGE_H */
