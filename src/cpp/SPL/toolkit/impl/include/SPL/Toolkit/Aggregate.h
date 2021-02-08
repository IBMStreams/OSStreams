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

#ifndef SPL_RUNTIME_AGGREGATE_H
#define SPL_RUNTIME_AGGREGATE_H

#include <iostream>
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Toolkit/IncrementalAverage.h>
#include <SPL/Toolkit/IncrementalStatistics.h>

#define WARNANDTHROW( x , y ) {       \
        SPLAPPTRC(L_ERROR, y, SPL_TYPE_DBG); \
        THROW(x, y);                      \
}

namespace SPL
{
    namespace Aggregate
    {
        /*
         * Helper classes to implement Aggregates.
         * WindowType is the type of the whole Window
         * ChunkType is the type of a partition (non-grouped) or group
         * ResultType is the type of the result of the Aggregate
         * RecordType is the type of the input tuple
         * GroupType is the type of whole group (or bool)
         */

        /// Result: the number of items in the chunk
        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class GroupType>
        class Count
        {
        public:
            Count(const WindowType & window, const ChunkType & chunk, const GroupType & group)
            {
                _count = chunk.size();
            }
            ResultType operator ()()
            {
                return _count;
            }
        private:
            ResultType _count;
        };

        /// Result: the number of groups in the window
        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class GroupType>
        class CountGroups
        {
        public:
            CountGroups(const WindowType & window, const ChunkType & chunk,
                        const GroupType & group)
            {
                _count = group.size();
            }
            ResultType operator ()()
            {
                return _count;
            }
        private:
            ResultType _count;
        };

        /// Result: the number of groups in the window (non grouped)
        template <class WindowType, class ChunkType, class ResultType, class RecordType>
        class CountGroups<WindowType, ChunkType, ResultType, RecordType, bool>
        {
        public:
            CountGroups(const WindowType & window, const ChunkType & chunk, const bool & group) {}
            ResultType operator ()()
            {
                return 1;
            }
        };

        /// Result: the number of items in all partitions
        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class GroupType>
        class CountAll
        {
        public:
            CountAll(const WindowType & window, const ChunkType & chunk, const GroupType & group)
            {
                const typename WindowType::StorageType & map = window.getWindowStorage();
                typename WindowType::StorageType::const_iterator it;
                _count = 0;
                for (it = map.begin(); it != map.end(); it++) {
                    const typename WindowType::DataType & data = it->second;
                    _count += data.size();
                }
            }
            ResultType operator ()()
            {
                return _count;
            }
        private:
            ResultType _count;
        };

        /// Result: list of counts in each chunk
        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class GroupType>
        class CountByGroup
        {
        public:
            CountByGroup(const WindowType & window, const ChunkType & chunk, const GroupType & group) : _group(group) {}
            ResultType operator ()()
            {
                ResultType list;
                typename GroupType::const_iterator it;
                for (it = _group.begin(); it != _group.end(); it++)
                    list.push_back(it->second.size());
                return list;
            }
        private:
            const GroupType & _group;
        };

        /// Result: the number of groups in the window (non grouped)
        template <class WindowType, class ChunkType, class ResultType, class RecordType>
        class CountByGroup<WindowType, ChunkType, ResultType, RecordType, bool>
        {
        public:
            CountByGroup(const WindowType & window, const ChunkType & chunk, const bool & group)
                : _mcnt(window, chunk, group) {}
            ResultType operator ()()
            {
                ResultType list;
                list.push_back(_mcnt());
                return list;
            }
        private:
            CountAll<WindowType, ChunkType, SPL::uint64, RecordType, bool> _mcnt;
        };

        /// Result: First item in the chunk
        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class ElementType, class GroupType>
        class First
        {
        public:
            First(const WindowType & window, const ChunkType & chunk, const GroupType & group)
                : _record(**chunk.begin()) {}

            bool needsSeedValue()
            {
                return true;
            }
            const RecordType * getSeedRecord()
            {
                return &_record;
            }
            void setSeedValue(const ResultType & v)
            {
                _value = v;
            }

            const ResultType & operator ()()
            {
                return _value;
            }
        private:
            const RecordType & _record;
            ResultType _value;
        };

        /// Result: Last item in the chunk
        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class ElementType, class GroupType>
        class Last
        {
        public:
            Last(const WindowType & window, const ChunkType & chunk, const GroupType & group)
                : _record(**chunk.rbegin()) {}

            bool needsSeedValue()
            {
                return true;
            }
            const RecordType * getSeedRecord()
            {
                return &_record;
            }
            void setSeedValue(const ResultType & v)
            {
                _value = v;
            }
            const ResultType & operator ()()
            {
                return _value;
            }
        private:
            const RecordType & _record;
            ResultType _value;
        };

        /// Result: Any item in the chunk
        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class ElementType, class GroupType>
        class Any : public Last<WindowType, ChunkType, ResultType, RecordType, ElementType,
                                GroupType>
        {
        public:
            Any(const WindowType & window, const ChunkType & chunk, const GroupType & group)
                : Last<WindowType, ChunkType, ResultType, RecordType,ElementType,GroupType>(window, chunk, group) {}
        };

        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class ElementType, class GroupType>
        class MinMax
        {
        public:
            MinMax(const WindowType & window, const ChunkType & chunk, const GroupType & group)
                : _record(**chunk.begin()) {}

            bool needsSeedValue()
            {
                return true;
            }
            const RecordType * getSeedRecord()
            {
                return &_record;
            }
            void setSeedValue(const ResultType & v)
            {
                _value = v;
            }
            const ResultType & operator ()()
            {
                return _value;
            }
        protected:
            const RecordType & _record;
            ResultType _value;
        };

        /// Result: Minimum value in the chunk
        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class ElementType, class GroupType>
        class Min : public MinMax<WindowType, ChunkType, ResultType, RecordType, ElementType, GroupType>
        {
        public:
            Min(const WindowType & window, const ChunkType & chunk, const GroupType & group)
                : MinMax<WindowType, ChunkType, ResultType, RecordType, ElementType, GroupType>(window, chunk, group) {}

            void process(const ElementType & v)
            {
                this->_value = std::min(this->_value, v);
            }
        };

        template <class T, class WindowType, class ChunkType, class RecordType, class GroupType>
        class Min<WindowType, ChunkType, SPL::list<T>, RecordType, SPL::list<T>, GroupType>
            : public MinMax<WindowType, ChunkType, SPL::list<T>, RecordType, SPL::list<T>, GroupType>
        {
            typedef SPL::list<T> TYPE;
        public:
            Min(const WindowType & window, const ChunkType & chunk, const GroupType & group)
                : MinMax<WindowType, ChunkType, TYPE, RecordType, TYPE, GroupType>(window, chunk, group) {}

            void process(const TYPE & v)
            {
                TYPE& m = this->_value;
                if (m.size() != v.size())
                    WARNANDTHROW(SPLRuntimeInvalidArgument, "Aggregate Min: list size mismatch");
                for (uint32_t i = 0, n = v.size(); i < n; i++)
                    m[i] = std::min(m[i], v[i]);
            }
        };

        template <class T, int msize, class WindowType, class ChunkType, class RecordType, class GroupType>
        class Min<WindowType, ChunkType, SPL::blist<T,msize>, RecordType, SPL::blist<T,msize>, GroupType>
            : public MinMax<WindowType, ChunkType, SPL::blist<T,msize>, RecordType, SPL::blist<T,msize>, GroupType>
        {
            typedef SPL::blist<T,msize> TYPE;
        public:
            Min(const WindowType & window, const ChunkType & chunk, const GroupType & group)
                : MinMax<WindowType, ChunkType, TYPE, RecordType, TYPE, GroupType>(window, chunk, group) {}

            void process(const TYPE & v)
            {
                TYPE& m = this->_value;
                if (m.size() != v.size())
                    WARNANDTHROW(SPLRuntimeInvalidArgument, "Aggregate Min: list size mismatch");
                for (uint32_t i = 0, n = v.size(); i < n; i++)
                    m[i] = std::min(m[i], v[i]);
            }
        };

        /// Result: Maximum value in the chunk
        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class ElementType, class GroupType>
        class Max : public MinMax<WindowType, ChunkType, ResultType, RecordType, ElementType, GroupType>
        {
        public:
            Max(const WindowType & window, const ChunkType & chunk, const GroupType & group)
                : MinMax<WindowType, ChunkType, ResultType, RecordType, ElementType, GroupType>(window, chunk, group) {}

            void process(const ElementType & v)
            {
                this->_value = std::max(this->_value, v);
            }
        };

        template <class T, class WindowType, class ChunkType, class RecordType, class GroupType>
        class Max<WindowType, ChunkType, SPL::list<T>, RecordType, SPL::list<T>, GroupType>
            : public MinMax<WindowType, ChunkType, SPL::list<T>, RecordType, SPL::list<T>, GroupType>
        {
            typedef SPL::list<T> TYPE;
        public:
            Max(const WindowType & window, const ChunkType & chunk, const GroupType & group)
                : MinMax<WindowType, ChunkType, TYPE, RecordType, TYPE, GroupType>(window, chunk, group) {}

            void process(const TYPE & v)
            {
                TYPE& m = this->_value;
                if (m.size() != v.size())
                    WARNANDTHROW(SPLRuntimeInvalidArgument, "Aggregate Max: list size mismatch");
                for (uint32_t i = 0, n = v.size(); i < n; i++)
                    m[i] = std::max(m[i], v[i]);
            }
        };

        template <class T, int msize, class WindowType, class ChunkType, class RecordType, class GroupType>
        class Max<WindowType, ChunkType, SPL::blist<T,msize>, RecordType, SPL::blist<T,msize>, GroupType>
            : public MinMax<WindowType, ChunkType, SPL::blist<T,msize>, RecordType, SPL::blist<T,msize>, GroupType>
        {
            typedef SPL::blist<T,msize> TYPE;
        public:
            Max(const WindowType & window, const ChunkType & chunk, const GroupType & group)
                : MinMax<WindowType, ChunkType, TYPE, RecordType, TYPE, GroupType>(window, chunk, group) {}

            void process(const TYPE & v)
            {
                TYPE& m = this->_value;
                if (m.size() != v.size())
                    WARNANDTHROW(SPLRuntimeInvalidArgument, "Aggregate Max: list size mismatch");
                for (uint32_t i = 0, n = v.size(); i < n; i++)
                    m[i] = std::max(m[i], v[i]);
            }
        };

        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class ElementType, class GroupType>
        class MinMaxCount
        {
        public:
            MinMaxCount(const WindowType & window, const ChunkType & chunk, const GroupType & group)
                : _record(**chunk.begin()) {}

            bool needsSeedValue()
            {
                return true;
            }
            const RecordType * getSeedRecord()
            {
                return &_record;
            }
            void setSeedValue(const ResultType & v)
            {
                _value = v;  _count = 0;
            }

            const ResultType & operator ()()
            {
                return _count;
            }

        protected:
            const RecordType & _record;
            ElementType _value;
            ResultType _count;
        };

        /// Result: Count of minimum value in the chunk
        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class ElementType, class GroupType>
        class MinCount : public MinMaxCount<WindowType, ChunkType, ResultType, RecordType, ElementType, GroupType>
        {
        public:
            MinCount(const WindowType & window, const ChunkType & chunk, const GroupType & group)
                : MinMaxCount<WindowType, ChunkType, ResultType, RecordType, ElementType, GroupType>(window, chunk, group) {}

            void process(const ElementType & v)
            {
                if (this->_value < v) {
                    this->_value = v;
                    this->_count = 1;
                } else if (this->_value == v)
                    this->_count++;
            }
        };

        /// Result: Count of maximum value in the chunk
        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class ElementType, class GroupType>
        class MaxCount : public MinMaxCount<WindowType, ChunkType, ResultType, RecordType, ElementType, GroupType>
        {
        public:
            MaxCount(const WindowType & window, const ChunkType & chunk, const GroupType & group)
                : MinMaxCount<WindowType, ChunkType, ResultType, RecordType, ElementType, GroupType>(window, chunk, group) {}

            void process(const ElementType & v)
            {
                if (this->_value > v) {
                    this->_value = v;
                    this->_count = 1;
                } else if (this->_value == v)
                    this->_count++;
            }
        };

        /// Result: Sum of all values
        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class ElementType, class GroupType>
        class Sum
        {
        public:
            Sum(const WindowType & window, const ChunkType & chunk, const GroupType & group)
                : _sum(0) {}

            bool needsSeedValue()
            {
                return false;
            }
            const RecordType * getSeedRecord()
            {
                return NULL;
            }
            void setSeedValue(const ResultType & v) {}
            void process(const ElementType v)
            {
                _sum += v;
            }

            const ResultType operator ()()
            {
                return _sum;
            }
        protected:
            ResultType _sum;
        };

        template <class T, class WindowType, class ChunkType, class RecordType, class GroupType>
        class Sum<WindowType, ChunkType, SPL::list<T>, RecordType, SPL::list<T>, GroupType>
        {
            typedef SPL::list<T> TYPE;
        public:
            Sum(const WindowType & window, const ChunkType & chunk, const GroupType & group)
                : _first(true) {}

            bool needsSeedValue()
            {
                return false;
            }
            const RecordType * getSeedRecord()
            {
                return NULL;
            }
            void setSeedValue(const TYPE& v) {}
            void process(const TYPE& v)
            {
                if (_first) {
                    _first = false;
                    _sum = v;
                } else
                    _sum += v;
            }

            const TYPE operator ()()
            {
                return _sum;
            }
        protected:
            TYPE _sum;
            bool _first;
        };

        template <class T, int msize, class WindowType, class ChunkType, class RecordType, class GroupType>
        class Sum<WindowType, ChunkType, SPL::blist<T,msize>, RecordType, SPL::blist<T,msize>, GroupType>
        {
            typedef SPL::blist<T,msize> TYPE;
        public:
            Sum(const WindowType & window, const ChunkType & chunk, const GroupType & group)
                : _first(true) {}

            bool needsSeedValue()
            {
                return false;
            }
            const RecordType * getSeedRecord()
            {
                return NULL;
            }
            void setSeedValue(const TYPE& v) {}
            void process(const TYPE& v)
            {
                if (_first) {
                    _first = false;
                    _sum = v;
                } else
                    _sum += v;
            }

            const TYPE operator ()()
            {
                return _sum;
            }
        protected:
            TYPE _sum;
            bool _first;
        };

        template <class WindowType, class ChunkType, class RecordType, class GroupType>
        class Sum<WindowType, ChunkType, SPL::rstring, RecordType, SPL::rstring, GroupType>
        {
        public:
            Sum(const WindowType & window, const ChunkType & chunk, const GroupType & group) {}

            bool needsSeedValue()
            {
                return false;
            }
            const RecordType * getSeedRecord()
            {
                return NULL;
            }
            void setSeedValue(const SPL::rstring & v) {}
            void process(const SPL::rstring & v)
            {
                _sum += v;
            }

            const SPL::rstring & operator ()()
            {
                return _sum;
            }
        protected:
            SPL::rstring _sum;
        };

        template <class WindowType, class ChunkType, class RecordType, class GroupType>
        class Sum<WindowType, ChunkType, SPL::ustring, RecordType, SPL::ustring, GroupType>
        {
        public:
            Sum(const WindowType & window, const ChunkType & chunk, const GroupType & group) {}

            bool needsSeedValue()
            {
                return false;
            }
            const RecordType * getSeedRecord()
            {
                return NULL;
            }
            void setSeedValue(const SPL::ustring & v) {}
            void process(const SPL::ustring & v)
            {
                _sum += v;
            }

            const SPL::ustring & operator ()()
            {
                return _sum;
            }
        protected:
            SPL::ustring _sum;
        };

        /// Result: Average of all values
        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class ElementType, class GroupType>
        class Average
        {
        public:
            Average(const WindowType & window, const ChunkType & chunk, const GroupType & group) {}
            bool needsSeedValue() { return false; }
            const RecordType * getSeedRecord() { return NULL; }
            void setSeedValue(const ResultType & v) {}
            void process(const ElementType v) { _ia.addValue(v); }

            const ResultType operator ()() { return _ia(); }
        private:
            SPL::IncrementalAverage::IA<ResultType> _ia;
        };

        template <class T, class WindowType, class ChunkType, class RecordType, class GroupType>
        class Average<WindowType, ChunkType, SPL::list<T>, RecordType, SPL::list<T>, GroupType>
        {
            typedef SPL::list<T> TYPE;
        public:
            Average(const WindowType & window, const ChunkType & chunk, const GroupType & group)
                : _record(**chunk.begin()) {}

            bool needsSeedValue() { return true; }
            const RecordType * getSeedRecord() { return &_record; }
            void setSeedValue(const TYPE & v) { _ia.resize(v.size());}
            void process(const TYPE& v)
            {
                for (uint32_t i = 0, n = v.size(); i < n; i++)
                   _ia[i].addValue(v[i]);
            }

            const TYPE operator ()()
            {
                TYPE res;
                res.reserve(_ia.size());
                for (uint32_t i = 0, n = _ia.size(); i < n; i++)
                    res.push_back (_ia[i]());
                return res;
            }
        private:
            const RecordType & _record;
            std::vector<SPL::IncrementalAverage::IA<T> > _ia;
        };

        template <class T, int msize, class WindowType, class ChunkType, class RecordType, class GroupType>
        class Average<WindowType, ChunkType, SPL::blist<T,msize>, RecordType, SPL::blist<T,msize>, GroupType>
        {
            typedef SPL::blist<T,msize> TYPE;
        public:
            Average(const WindowType & window, const ChunkType & chunk, const GroupType & group)
                : _record(**chunk.begin()) {}

            bool needsSeedValue() { return true; }
            const RecordType * getSeedRecord() { return &_record; }
            void setSeedValue(const TYPE & v) { _ia.resize(v.size());}
            void process(const TYPE& v)
            {
                for (uint32_t i = 0, n = v.size(); i < n; i++)
                   _ia[i].addValue(v[i]);
            }

            const TYPE operator ()()
            {
                TYPE res;
                for (uint32_t i = 0, n = _ia.size(); i < n; i++)
                    res.push_back (_ia[i]());
                return res;
            }
        private:
            const RecordType & _record;
            std::vector<SPL::IncrementalAverage::IA<T> > _ia;
        };

        /// Result: list of all the expressions
        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class ElementType, class GroupType>
        class Collect
        {
        public:
            Collect(const WindowType & window, const ChunkType & chunk, const GroupType & group) {}

            bool needsSeedValue()
            {
                return false;
            }
            const RecordType * getSeedRecord()
            {
                return NULL;
            }
            void setSeedValue(const typename ResultType::value_type & v) {}
            void process(const ElementType & v)
            {
                _list.push_back(v);
            }

            const ResultType & operator ()()
            {
                return _list;
            }
        private:
            ResultType _list;
        };

        /// Result: number of all distinct expressions
        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class ElementType, class GroupType>
        class CountDistinct
        {
        public:
            CountDistinct(const WindowType & window, const ChunkType & chunk, const GroupType & group) {}

            bool needsSeedValue()
            {
                return false;
            }
            const RecordType * getSeedRecord()
            {
                return NULL;
            }
            void setSeedValue(const ElementType & v) {}
            void process(const ElementType & v)
            {
                _set.insert(v);
            }

            const ResultType operator ()()
            {
                return _set.size();
            }
        private:
            std::tr1::unordered_set<ElementType> _set;
        };

        /// Result: list of all distinct expressions
        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class ElementType, class GroupType>
        class CollectDistinct
        {
        public:
            CollectDistinct(const WindowType & window, const ChunkType & chunk, const GroupType & group) {}

            bool needsSeedValue()
            {
                return false;
            }
            const RecordType * getSeedRecord()
            {
                return NULL;
            }
            void setSeedValue(const ElementType & v) {}
            void process(const ElementType & v)
            {
                _set.insert(v);
            }

            const ResultType operator ()()
            {
                ResultType list;
                typename SetType::iterator it;
                for (it = _set.begin(); it != _set.end(); it++)
                    list.push_back(*it);
                return list;
            }
            typedef std::tr1::unordered_set<ElementType> SetType;
            SetType set() const
            {
                return _set;
            }
        private:
            SetType _set;
        };

        /// Result: list of all counts of distinct expressions
        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class ElementType, class GroupType>
        class CountByDistinct
        {
        public:
            typedef CollectDistinct<WindowType,ChunkType,SPL::list<ElementType>,RecordType,ElementType, GroupType> D;
            CountByDistinct(const WindowType & window, const ChunkType & chunk, const GroupType & group) : _dcol(NULL) {}

            void setCollectDistinct(D & d)
            {
                _dcol = &d;
            }
            bool needsSeedValue()
            {
                return false;
            }
            const RecordType * getSeedRecord()
            {
                return NULL;
            }
            void setSeedValue(const ElementType & v) {}
            void process(const ElementType & v)
            {
                typename MapType::iterator it = _map.find(v);
                if (it == _map.end())
                    _map.insert(typename MapType::value_type(v, 1ul));
                else
                    it->second++;
            }

            const ResultType operator ()()
            {
                ResultType list;
                if (_dcol) {
                    const typename D::SetType & set = _dcol->set();
                    typename D::SetType::const_iterator it;
                    for (it = set.begin(); it != set.end(); it++)
                        list.push_back(_map.find(*it)->second);
                } else {
                    typename MapType::iterator it;
                    for (it = _map.begin(); it != _map.end(); it++)
                        list.push_back(it->second);
                }
                return list;
            }
        private:
            typedef std::tr1::unordered_map<ElementType, uint64_t> MapType;
            MapType _map;
            D * _dcol;
        };

        /// Standard deviation Helper class
        template <class Type, class RecordType>
        class IncrementalStatistics
        {
        public:
            bool needsSeedValue()
            {
                return false;
            }
            const RecordType * getSeedRecord()
            {
                return NULL;
            }
            void setSeedValue(const Type & v) {}

            void process(const Type & v)
            {
                _is.addValue(v);
            }

        protected:
            SPL::IncrementalStatistics::IS<Type> _is;
        };

        /// Result: sample standard deviation
        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class ElementType, class GroupType>
        class SampleStdDev : public IncrementalStatistics<ElementType,RecordType>
        {
        public:
            SampleStdDev(const WindowType & window, const ChunkType & chunk, const GroupType & group) {}

            ResultType operator ()()
            {
                int64_t count = this->_is.count();
                return count > 1 ? SPL::Functions::Math::sqrt(this->_is.dev()/(count-1)) : ElementType(0);
            }
        };

        /// Result: population standard deviation
        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class ElementType, class GroupType>
        class PopulationStdDev : public IncrementalStatistics<ElementType,RecordType>
        {
        public:
            PopulationStdDev(const WindowType & window, const ChunkType & chunk, const GroupType & group) {}

            ResultType operator ()()
            {
                return SPL::Functions::Math::sqrt(this->_is.dev()/this->_is.count());
            }
        };

        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class ElementType, class ElementType2, class GroupType>
        class ArgMinMax
        {
        public:
            ArgMinMax(const WindowType & window, const ChunkType & chunk, const GroupType & group)
                : _record(**chunk.begin()) {}

            bool needsSeedValue()
            {
                return true;
            }
            const RecordType * getSeedRecord()
            {
                return &_record;
            }
            void setSeedValue(const ElementType & v, const ElementType2 & w)
            {
                _value = v;
                _arg = w;
            }

            const ElementType2 & operator ()()
            {
                return _arg;
            }

            void setValues(const ElementType & v, const ElementType2 & w)
            {
                _value = v; _arg = w;
            }

        protected:
            const RecordType & _record;
            ElementType _value;
            ElementType2 _arg;
        };

        /// Result: Minimum value in the chunk
        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class ElementType, class ElementType2, class GroupType>
        class ArgMin : public ArgMinMax<WindowType, ChunkType, ResultType,
                                        RecordType, ElementType, ElementType2, GroupType>
        {
        public:
            ArgMin(const WindowType & window, const ChunkType & chunk, const GroupType & group)
                : ArgMinMax<WindowType, ChunkType, ResultType, RecordType,
                            ElementType, ElementType2, GroupType>(window, chunk, group) {}

            void process(const ElementType & v, const ElementType2 & w)
            {
                if (v < this->_value)
                    this->setValues(v, w);
            }
        };

        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class ElementType, class ElementType2, class GroupType>
        class ArgMax : public ArgMinMax<WindowType, ChunkType, ResultType,
                                        RecordType, ElementType, ElementType2, GroupType>
        {
        public:
            ArgMax(const WindowType & window, const ChunkType & chunk, const GroupType & group)
                : ArgMinMax<WindowType, ChunkType, ResultType, RecordType,
                            ElementType, ElementType2, GroupType>(window, chunk, group) {}

            void process(const ElementType & v, const ElementType2 & w)
            {
                if (v > this->_value)
                    this->setValues(v, w);
            }
        };

        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class ElementType, class ElementType2, class GroupType>
        class CollectArgMinMax
        {
        public:
            CollectArgMinMax(const WindowType & window, const ChunkType & chunk,
                             const GroupType & group)
                : _record(**chunk.begin()) {}

            bool needsSeedValue()
            {
                return true;
            }
            const RecordType * getSeedRecord()
            {
                return &_record;
            }
            void setSeedValue(const ElementType & v, const ElementType2 & w)
            {
                _value = v;
                _args.insert(w);
            }

            typedef std::tr1::unordered_set<ElementType2> SetType;
            const ResultType operator ()()
            {
                ResultType list;
                typename SetType::const_iterator it;
                for (it = _args.begin(); it != _args.end(); it++)
                    list.push_back(*it);
                return list;
            }

        protected:
            const RecordType & _record;
            ElementType _value;
            SetType _args;
        };

        /// Result: Minimum value in the chunk
        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class ElementType, class ElementType2, class GroupType>
        class CollectArgMin : public CollectArgMinMax<WindowType, ChunkType, ResultType,
                                              RecordType, ElementType, ElementType2, GroupType>
        {
        public:
            CollectArgMin(const WindowType & window, const ChunkType & chunk, const GroupType & group)
                : CollectArgMinMax<WindowType, ChunkType, ResultType, RecordType,
                               ElementType, ElementType2, GroupType>(window, chunk, group) {}

            void process(const ElementType & v, const ElementType2 & w)
            {
                if (v < this->_value) {
                    this->_args.clear();
                    this->setSeedValue(v, w);
                } else if (v == this->_value)
                    this->_args.insert(w);
            }
        };

        template <class WindowType, class ChunkType, class ResultType, class RecordType,
                  class ElementType, class ElementType2, class GroupType>
        class CollectArgMax : public CollectArgMinMax<WindowType, ChunkType, ResultType,
                                              RecordType, ElementType, ElementType2, GroupType>
        {
        public:
            CollectArgMax(const WindowType & window, const ChunkType & chunk, const GroupType & group)
                : CollectArgMinMax<WindowType, ChunkType, ResultType, RecordType,
                               ElementType, ElementType2, GroupType>(window, chunk, group) {}

            void process(const ElementType & v, const ElementType2 & w)
            {
                if (v > this->_value) {
                    this->_args.clear();
                    this->setSeedValue(v, w);
                } else if (v == this->_value)
                    this->_args.insert(w);
            }
        };
    } // namespace Aggregate

} // namespace SPL

#endif /*SPL_RUNTIME_AGGREGATE_H */
