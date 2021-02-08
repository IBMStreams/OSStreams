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

#ifndef SPL_INCREMENTAL_STATISTICS_H
#define SPL_INCREMENTAL_STATISTICS_H

#include <SPL/Toolkit/IncrementalAverage.h>
#include <SPL/Toolkit/Serialization.h>

namespace SPL
{
    namespace IncrementalStatistics
    {
        /**
         * Helper classes to implement standard deviation to handle unsigned types
         */
        template<typename T>
        class IncrementalStatistics
        {
        public:
            IncrementalStatistics() : _oldDev(0), _oldMean(0) {}

            void addValue(const T & v)
            {
                _ia.addValue(v);
                // See Knuth TAOCP vol 2, 3rd edition, page 232
                if (_ia.count() == 1) {
                    _oldMean = v;
                    _oldDev = 0;
                } else {
                    T newMean = _ia();
                    T newDev = _oldDev + (v - _oldMean)*(v - newMean);
                    _oldMean = newMean;
                    _oldDev = newDev;
                }
            }

            T dev() const { return _oldDev;}
            int64_t count() const { return _ia.count();}

        protected:
            friend class SPL::Serialization::Access;
            template<typename Archive>
            void save(Archive & ar) const { SPL::Serialization::save(ar, _ia); ar << _oldDev << _oldMean; }
            template<typename Archive>
            void load(Archive & ar) { SPL::Serialization::load(ar, _ia); ar >> _oldDev >> _oldMean; }

            SPL::IncrementalAverage::IA<T> _ia;
            T _oldDev, _oldMean;
        };

        template<typename T>
        class UnsignedIncrementalStatistics
        {
        public:
            UnsignedIncrementalStatistics() : _oldDev(0), _oldMean(0) {}

            void addValue(const T & v)
            {
                _ia.addValue(v);
                // See Knuth TAOCP vol 2, 3rd edition, page 232
                if (_ia.count() == 1) {
                    _oldMean = v;
                    _oldDev = 0;
                } else {
                    T newMean = _ia();
                    T newDev;
                    if (_oldMean > v) {
                        if (newMean > v)
                            newDev = _oldDev + (_oldMean-v)*(newMean-v);
                        else {
                            T diff = (_oldMean-v)*(v-newMean);
                            newDev = _oldDev > diff ? _oldDev - diff : 0;
                        }
                    } else {
                        if (newMean > v) {
                            T diff = (v-_oldMean)*(newMean-v);
                            newDev = _oldDev > diff ? _oldDev - diff : 0;
                        } else
                            newDev = _oldDev + (v-_oldMean)*(v-newMean);
                    }
                    _oldMean = newMean;
                    _oldDev = newDev;
                }
            }

            T dev() const { return _oldDev;}
            int64_t count() const { return _ia.count();}

        protected:
            friend class SPL::Serialization::Access;
            template<typename Archive>
            void save(Archive & ar) const { SPL::Serialization::save(ar, _ia); ar << _oldDev << _oldMean; }
            template<typename Archive>
            void load(Archive & ar) { SPL::Serialization::load(ar, _ia); ar >> _oldDev >> _oldMean; }

            SPL::IncrementalAverage::IA<T> _ia;
            T _oldDev, _oldMean;
        };

        // Interface for general use
        template<typename T> class IS : public IncrementalStatistics<T> {};

        // Do unsigned specially
        template<> class IS<SPL::uint8>  : public UnsignedIncrementalStatistics<SPL::uint8> {};
        template<> class IS<SPL::uint16> : public UnsignedIncrementalStatistics<SPL::uint16> {};
        template<> class IS<SPL::uint32> : public UnsignedIncrementalStatistics<SPL::uint32> {};
        template<> class IS<SPL::uint64> : public UnsignedIncrementalStatistics<SPL::uint64> {};
    } // namespace IncrementalStatistics
} // namespace SPL

#endif /*SPL_INCREMENTAL_STATISTICS_H */
