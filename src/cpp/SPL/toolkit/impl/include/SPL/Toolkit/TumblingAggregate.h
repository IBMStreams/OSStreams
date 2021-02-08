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

#ifndef SPL_RUNTIME_TUMBLING_AGGREGATE_H
#define SPL_RUNTIME_TUMBLING_AGGREGATE_H

#include <iostream>
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Toolkit/IncrementalAverage.h>
#include <SPL/Toolkit/IncrementalStatistics.h>
#include <SPL/Toolkit/Serialization.h>

#define WARNANDTHROW( x , y ) {              \
        SPLAPPTRC(L_ERROR, y, SPL_TYPE_DBG); \
        THROW(x, y);                         \
}

namespace SPL
{
    namespace TumblingAggregate
    {
        /*
         * Helper classes to implement Tumbling Aggregates.
         * AttributeType is the type of the attribute being aggregated.
         * ResultType is the type of the result of the Aggregate
         */

        /// Result: the number of items in the partition
        template <typename ResultType>
        class Count
        {
        public:
            Count() : _count(0) {}
            void tupleInserted () { _count++; }
            ResultType operator ()() const { return _count; }
            ResultType & get() { return _count; }

        private:
            ResultType _count;
        };

        /// Result: the number of items in the partition
        template <typename ResultType>
        class CountAll : public Count<ResultType> {
        };

        /// Result: the number of groups in the window (non grouped)
        template <typename ResultType>
        class CountGroups
        {
        public:
            CountGroups() {}
            void tupleInserted () {}
            ResultType operator ()() const { return 1; }
        };

        /// Result: the number of groups in the partition (non grouped)
        template <typename ResultType>
        class CountByGroup
        {
        public:
            CountByGroup() : _count(0) {}

            void tupleInserted () { _count++; }

            ResultType operator ()() const {
                ResultType res;
                res.push_back(_count);
                return res;
            }

            // Used for serialization
            uint64 const & get() const { return _count; }
            uint64 & get() { return _count; }

        private:
            uint64 _count;
        };

        /// Result: First item in the partition
        template <typename AttributeType, typename ResultType>
        class First
        {
        public:
            First() : _first(true) {}

            void tupleInserted (AttributeType const& a)
            {
                if (_first) {
                    _first = false;
                    _value = a;
                }
            }

            const ResultType & operator ()() const { return _value; }

        private:
            friend class SPL::Serialization::Access;
            template<typename Archive>
            void save(Archive & ar) const { ar << _first << _value; }
            template<typename Archive>
            void load(Archive & ar) { ar >> _first >> _value; }

            bool _first;
            ResultType _value;
        };

        /// Result: Last item in the partition
        template <typename AttributeType, typename ResultType>
        class Last
        {
        public:
            Last() {}
            void tupleInserted (AttributeType const& a) { _value = a; }

            const ResultType & operator ()() const { return _value; }
            ResultType & get() { return _value; }

        private:
            ResultType _value;
        };

        /// Result: Any item in the partition - use the last one (to get same answer as non
        // optimizing version)
        template <typename AttributeType, typename ResultType>
        class Any : public Last<AttributeType,ResultType> {
        };

        /// Result: Minimum value in the partition
        template <typename AttributeType, typename ResultType>
        class MinMax {
        public:
            MinMax() : _first(true) {}

            const ResultType & operator ()() const { return _value; }

        protected:
            friend class SPL::Serialization::Access;
            template<typename Archive>
            void save(Archive & ar) const { ar << _first << _value; }
            template<typename Archive>
            void load(Archive & ar) { ar >> _first >> _value; }

            bool _first;
            ResultType _value;
        };

        /// Result: Minimum value in the partition
        template <typename AttributeType, typename ResultType>
        class Min : public MinMax<AttributeType, ResultType> {
        public:
            void tupleInserted (AttributeType const& a)
            {
                if (this->_first) {
                    this->_first = false;
                    this->_value = a;
                } else
                    this->_value = std::min(this->_value, a);
            }
        };

        template <typename T>
        class Min<SPL::list<T>, SPL::list<T> > : public MinMax<SPL::list<T>, SPL::list<T> > {
            typedef SPL::list<T> TYPE;
        public:
            void tupleInserted (TYPE const& a)
            {
                if (this->_first) {
                    this->_first = false;
                    this->_value = a;
                } else {
                    if (this->_value.size() != a.size())
                        WARNANDTHROW(SPLRuntimeInvalidArgument, "Aggregate Min: list size mismatch");
                    for (uint32_t i = 0, n = a.size(); i < n; i++)
                        this->_value[i] = std::min(this->_value[i], a[i]);
                }
            }
        };

        template <typename T, int mSize>
        class Min<SPL::blist<T,mSize>, SPL::blist<T,mSize> >
                : public MinMax<SPL::blist<T,mSize>, SPL::blist<T,mSize> > {
            typedef SPL::blist<T,mSize> TYPE;
        public:
            void tupleInserted (TYPE const& a)
            {
                if (this->_first) {
                    this->_first = false;
                    this->_value = a;
                } else {
                    if (this->_value.size() != a.size())
                        WARNANDTHROW(SPLRuntimeInvalidArgument, "Aggregate Min: list size mismatch");
                    for (uint32_t i = 0, n = a.size(); i < n; i++)
                        this->_value[i] = std::min(this->_value[i], a[i]);
                }
            }
        };

        /// Result: Maximum value in the partition
        template <typename AttributeType, typename ResultType>
        class Max : public MinMax<AttributeType, ResultType> {
        public:
            void tupleInserted (AttributeType const& a)
            {
                if (this->_first) {
                    this->_first = false;
                    this->_value = a;
                } else
                    this->_value = std::max(this->_value, a);
            }
        };

        template <typename T>
        class Max<SPL::list<T>, SPL::list<T> >
                : public MinMax<SPL::list<T>, SPL::list<T> > {
            typedef SPL::list<T> TYPE;
        public:
            void tupleInserted (TYPE const& a)
            {
                if (this->_first) {
                    this->_first = false;
                    this->_value = a;
                } else {
                    if (this->_value.size() != a.size())
                        WARNANDTHROW(SPLRuntimeInvalidArgument, "Aggregate Max: list size mismatch");
                    for (uint32_t i = 0, n = a.size(); i < n; i++)
                        this->_value[i] = std::max(this->_value[i], a[i]);
                }
            }
        };

        template <typename T, int mSize>
        class Max<SPL::blist<T,mSize>, SPL::blist<T,mSize> >
                : public MinMax<SPL::blist<T,mSize>, SPL::blist<T,mSize> > {
            typedef SPL::blist<T,mSize> TYPE;
        public:
            void tupleInserted (TYPE const& a)
            {
                if (this->_first) {
                    this->_first = false;
                    this->_value = a;
                } else {
                    if (this->_value.size() != a.size())
                        WARNANDTHROW(SPLRuntimeInvalidArgument, "Aggregate Max: list size mismatch");
                    for (uint32_t i = 0, n = a.size(); i < n; i++)
                        this->_value[i] = std::max(this->_value[i], a[i]);
                }
            }
        };

        template <typename AttributeType, typename ResultType>
        class MinMaxCount
        {
        public:
            MinMaxCount() : _first(true) {}

            const ResultType & operator ()() const { return _count; }

        protected:
            friend class SPL::Serialization::Access;
            template<typename Archive>
            void save(Archive & ar) const { ar << _first << _value << _count; }
            template<typename Archive>
            void load(Archive & ar) { ar >> _first >> _value >> _count; }

            bool _first;
            AttributeType _value;
            ResultType _count;
        };

        /// Result: Count of minimum value in the partition
        template <typename AttributeType, typename ResultType>
        class MinCount : public MinMaxCount<AttributeType, ResultType>
        {
        public:
            void tupleInserted (AttributeType const& a)
            {
                if (this->_first || this->_value < a) {
                    this->_first = false;
                    this->_value = a;
                    this->_count = 1;
                } else if (this->_value == a)
                    this->_count++;
            }
        };

        /// Result: Count of maximum value in the partition
        template <typename AttributeType, typename ResultType>
        class MaxCount : public MinMaxCount<AttributeType, ResultType>
        {
        public:
            void tupleInserted (AttributeType const& a)
            {
                if (this->_first || this->_value > a) {
                    this->_first = false;
                    this->_value = a;
                    this->_count = 1;
                } else if (this->_value == a)
                    this->_count++;
            }
        };

        /// Result: Sum of all values
        template <typename AttributeType, typename ResultType>
        class Sum
        {
        public:
            Sum() : _sum(0) {}

            void tupleInserted (AttributeType const& a) { _sum += a; }

            const ResultType operator ()() const { return _sum; }

        protected:
            friend class SPL::Serialization::Access;
            template<typename Archive>
            void save(Archive & ar) const { ar << _sum; }
            template<typename Archive>
            void load(Archive & ar) { ar >> _sum; }

            ResultType _sum;
        };

        template <typename T>
        class Sum<SPL::list<T>, SPL::list<T> >
        {
            typedef SPL::list<T> TYPE;
        public:
            Sum() : _first(true) {}

            void tupleInserted (TYPE const& a)
            {
                if (_first) {
                    _first = false;
                    _sum = a;
                } else
                    _sum += a;
            }

            const TYPE operator ()() const { return _sum; }

        protected:
            friend class SPL::Serialization::Access;
            template<typename Archive>
            void save(Archive & ar) const { ar << _first << _sum; }
            template<typename Archive>
            void load(Archive & ar) { ar >> _first >> _sum; }

            TYPE _sum;
            bool _first;
        };

        template <typename T, int mSize>
        class Sum<SPL::blist<T,mSize>, SPL::blist<T,mSize> >
        {
            typedef SPL::blist<T,mSize> TYPE;
        public:
            Sum() : _first(true) {}

            void tupleInserted (TYPE const& a)
            {
                if (_first) {
                    _first = false;
                    _sum = a;
                } else
                    _sum += a;
            }

            const TYPE operator ()() const { return _sum; }

        protected:
            friend class SPL::Serialization::Access;
            template<typename Archive>
            void save(Archive & ar) const { ar << _first << _sum; }
            template<typename Archive>
            void load(Archive & ar) { ar >> _first >> _sum; }

            TYPE _sum;
            bool _first;
        };

        template<>
        class Sum<SPL::rstring, SPL::rstring>
        {
            typedef SPL::rstring AttributeType;
        public:
            Sum() {}

            void tupleInserted (AttributeType const& a) { _sum += a; }

            const AttributeType operator ()() const { return _sum; }

        protected:
            friend class SPL::Serialization::Access;
            template<typename Archive>
            void save(Archive & ar) const { ar << _sum; }
            template<typename Archive>
            void load(Archive & ar) { ar >> _sum; }

            AttributeType _sum;
        };

        template<>
        class Sum<SPL::ustring, SPL::ustring>
        {
            typedef SPL::ustring AttributeType;
        public:
            Sum() {}

            void tupleInserted (AttributeType const& a) { _sum += a; }

            const AttributeType operator ()() const { return _sum; }

        protected:
            friend class SPL::Serialization::Access;
            template<typename Archive>
            void save(Archive & ar) const { ar << _sum; }
            template<typename Archive>
            void load(Archive & ar) { ar >> _sum; }

            AttributeType _sum;
        };


        /// Result: Average of all values
        template <typename AttributeType, typename ResultType>
        class Average
        {
        public:
            void tupleInserted (AttributeType const& a) { _ia.addValue(a); }
            const ResultType operator ()() const { return _ia(); }

        protected:
            friend class SPL::Serialization::Access;
            template<typename Archive>
            void save(Archive & ar) const { SPL::Serialization::save(ar, _ia); };
            template<typename Archive>
            void load(Archive & ar) { SPL::Serialization::load(ar, _ia); };

            SPL::IncrementalAverage::IA<AttributeType> _ia;
        };

        template <typename T>
        class Average<SPL::list<T>, SPL::list<T> >
        {
            typedef SPL::list<T> AttributeType;
        public:
            Average() : _first (true) {}
            void tupleInserted (AttributeType const& a)
            {
                if (_first) {
                    _first = false;
                    _ia.resize(a.size());
                }
                for (uint32_t i = 0, n = a.size(); i < n; i++)
                    _ia[i].addValue(a[i]);
            }

            const AttributeType operator ()() const
            {
                AttributeType res;
                res.reserve(_ia.size());
                for (uint32_t i = 0, n = _ia.size(); i < n; i++)
                    res.push_back (_ia[i]());
                return res;
            }

        protected:
            friend class SPL::Serialization::Access;
            template<typename Archive>
            void save(Archive & ar) const
            {
            	ar << _first;
            	size_t size = _ia.size();
            	ar << size;
            	for (size_t i = 0; i < size; i++) {
            		SPL::Serialization::save(ar, _ia[i]);
            	}
            }

            template<typename Archive>
            void load(Archive & ar)
            {
            	ar >> _first;
            	size_t size; ar >> size;
            	_ia.resize(size);
            	for (size_t i = 0; i < size; i++) {
            		SPL::Serialization::load(ar, _ia[i]);
            	}
            }

        private:
            std::vector<SPL::IncrementalAverage::IA<T> > _ia;
            bool _first;
        };

        template <typename T, int mSize>
        class Average<SPL::blist<T, mSize>, SPL::blist<T, mSize> >
        {
            typedef SPL::blist<T,mSize> AttributeType;
        public:
            Average() : _first(true) {}

            void tupleInserted (AttributeType const& a)
            {
                if (_first) {
                    _first = false;
                    _ia.resize(a.size());
                }
                for (uint32_t i = 0, n = a.size(); i < n; i++)
                    _ia[i].addValue(a[i]);
            }

            const AttributeType operator ()() const
            {
                AttributeType res;
                for (uint32_t i = 0, n = _ia.size(); i < n; i++)
                    res.push_back (_ia[i]());
                return res;
            }

        protected:
            friend class SPL::Serialization::Access;
            template<typename Archive>
            void save(Archive & ar) const
            {
                ar << _first;
                size_t size = _ia.size();
                ar << size;
                for (size_t i = 0; i < size; i++) {
                    SPL::Serialization::save(ar, _ia[i]);
                }
            }

            template<typename Archive>
            void load(Archive & ar)
            {
                ar >> _first;
                size_t size; ar >> size;
                _ia.resize(size);
                for (size_t i = 0; i < size; i++) {
                    SPL::Serialization::load(ar, _ia[i]);
                }
            }

        private:
            std::vector<SPL::IncrementalAverage::IA<T> > _ia;
            bool _first;
        };

        /// Result: list of all the expressions
        template <typename AttributeType, typename ResultType>
        class Collect
        {
        public:
            void tupleInserted (AttributeType const& a) { _list.push_back(a); }
            const ResultType & operator ()() const { return _list; }
            ResultType & get() { return _list; }

        private:
            ResultType _list;
        };

        /// Result: number of all distinct expressions
        template <typename AttributeType, typename ResultType>
        class CountDistinct
        {
        public:
            void tupleInserted (AttributeType const& a) { _set.insert(a); }
            const ResultType operator ()() const { return _set.size(); }

        private:
            friend class SPL::Serialization::Access;
            template<typename Archive> void save(Archive & ar) const { ar << _set; }
            template<typename Archive> void load(Archive & ar) { ar >> _set; }

            std::tr1::unordered_set<AttributeType> _set;
        };

        /// Result: list of all distinct expressions
        template <typename AttributeType, typename ResultType>
        class CollectDistinct
        {
        public:
            void tupleInserted (AttributeType const& a) { _set.insert(a); }

            const ResultType operator ()() const
            {
                ResultType list;
                typename SetType::const_iterator it;
                for (it = _set.begin(); it != _set.end(); it++)
                    list.push_back(*it);
                return list;
            }
            typedef std::tr1::unordered_set<AttributeType> SetType;
            const SetType& set() const { return _set; }

        private:
            friend class SPL::Serialization::Access;
            template<typename Archive> void save(Archive & ar) const { ar << _set; }
            template<typename Archive> void load(Archive & ar) { ar >> _set; }

            SetType _set;
        };

        /// Result: list of all counts of distinct expressions
        template <typename AttributeType, typename ResultType>
        class CountByDistinct
        {
        public:
            typedef CollectDistinct<AttributeType, SPL::list<AttributeType> > D;
            CountByDistinct() : _dcol(NULL) {}

            void setCollectDistinct(D const & d) { _dcol = &d; }

            void tupleInserted (AttributeType const& a)
            {
                typename MapType::iterator it = _map.find(a);
                if (it == _map.end())
                    _map.insert(typename MapType::value_type(a, 1ul));
                else
                    it->second++;
            }

            const ResultType operator ()() const
            {
                ResultType list;
                if (_dcol != NULL) {
                    const typename D::SetType & set = _dcol->set();
                    typename D::SetType::const_iterator it;
                    for (it = set.begin(); it != set.end(); it++)
                        list.push_back(_map.find(*it)->second);
                } else {
                    typename MapType::const_iterator it;
                    for (it = _map.begin(); it != _map.end(); it++)
                        list.push_back(it->second);
                }
                return list;
            }

        private:
            friend class SPL::Serialization::Access;
            template<typename Archive> void save(Archive & ar) const { ar << _map; }
            template<typename Archive> void load(Archive & ar) { ar >> _map; }

            typedef std::tr1::unordered_map<AttributeType, uint64_t> MapType;
            MapType _map;
            D const * _dcol; // not serialized
        };

        /// Standard deviation Helper class
        template <typename AttributeType>
        class IncrementalStatistics
        {
        public:
            void tupleInserted(AttributeType const& a)
            {
                _is.addValue(a);
            }

        protected:
            friend class SPL::Serialization::Access;
            template<typename Archive>
            void save(Archive & ar) const { SPL::Serialization::save(ar, _is); }
            template<typename Archive>
            void load(Archive & ar) { SPL::Serialization::load(ar, _is); }

            SPL::IncrementalStatistics::IS<AttributeType> _is;
        };

        /// Result: sample standard deviation
        template <typename AttributeType, typename ResultType>
        class SampleStdDev : public IncrementalStatistics<AttributeType>
        {
        public:
            ResultType operator ()() const
            {
                int64_t count = this->_is.count();
                return count > 1 ? SPL::Functions::Math::sqrt(this->_is.dev()/(count-1)) : AttributeType(0);
            }
        };

        /// Result: population standard deviation
        template <typename AttributeType, typename ResultType>
        class PopulationStdDev : public IncrementalStatistics<AttributeType>
        {
        public:
            ResultType operator ()() const
            {
                return SPL::Functions::Math::sqrt(this->_is.dev()/this->_is.count());
            }
        };

        template <typename AttributeType, typename ResultType>
        class ArgMinMax
        {
        public:
            ArgMinMax() : _first(true) {}

            const ResultType & operator ()() const { return _arg; }

        protected:
            friend class SPL::Serialization::Access;
            template<typename Archive>
            void save(Archive & ar) const { ar << _first << _value << _arg; }
            template<typename Archive>
            void load(Archive & ar) { ar >> _first >> _value >> _arg; }

            bool _first;
            AttributeType _value;
            ResultType _arg;
        };

        /// Result: Minimum value in the partition
        template <typename AttributeType, typename ResultType>
        class ArgMin : public ArgMinMax<AttributeType, ResultType>
        {
        public:
            void tupleInserted (AttributeType const& a, ResultType const& a2)
            {
                if (this->_first || a < this->_value) {
                    this->_first = false;
                    this->_value = a;
                    this->_arg = a2;
                }
            }
        };

        /// Result: Maximum value in the partition
        template <typename AttributeType, typename ResultType>
        class ArgMax : public ArgMinMax<AttributeType, ResultType>
        {
        public:
            void tupleInserted (AttributeType const& a, ResultType const& a2)
            {
                if (this->_first || a > this->_value) {
                    this->_first = false;
                    this->_value = a;
                    this->_arg = a2;
                }
            }
        };

        template <typename AttributeType, typename ResultType>
        class CollectArgMinMax
        {
        public:
            CollectArgMinMax() : _first(true) {}

            typedef std::tr1::unordered_set<typename ResultType::value_type> SetType;
            const ResultType operator ()() const
            {
                ResultType list;
                typename SetType::const_iterator it;
                for (it = _args.begin(); it != _args.end(); it++)
                    list.push_back(*it);
                return list;
            }

        protected:
            friend class SPL::Serialization::Access;
            template<typename Archive>
            void save(Archive & ar) const { ar << _first << _value << _args; }
            template<typename Archive>
            void load(Archive & ar) { ar >> _first >> _value >> _args; }

            bool _first;
            AttributeType _value;
            SetType _args;
        };

        /// Result: Minimum value in the partition
        template <typename AttributeType, typename ResultType>
        class CollectArgMin : public CollectArgMinMax<AttributeType, ResultType>
        {
        public:
            void tupleInserted (AttributeType const& a, typename ResultType::value_type const& a2)
            {
                if (this->_first || a < this->_value) {
                    this->_first = false;
                    this->_args.clear();
                    this->_value = a;
                    this->_args.insert (a2);
                } else if (a == this->_value)
                    this->_args.insert(a2);
            }
        };

        /// Result: Maximum value in the partition
        template <typename AttributeType, typename ResultType>
        class CollectArgMax : public CollectArgMinMax<AttributeType, ResultType>
        {
        public:
            void tupleInserted (AttributeType const& a, typename ResultType::value_type const& a2)
            {
                if (this->_first || a > this->_value) {
                    this->_first = false;
                    this->_args.clear();
                    this->_value = a;
                    this->_args.insert (a2);
                } else if (a == this->_value)
                    this->_args.insert(a2);
            }
        };

    }; // end namespace TumblingAggregate


    /**
     * Non-intrusive serialization helpers.
     */
    namespace Serialization {
    	// Count<T>
		template<typename Archive, typename T>
		inline void save(Archive & ar, SPL::TumblingAggregate::Count<T> const & t) { ar << t(); }
		template<typename Archive, typename T>
		inline void load(Archive & ar, SPL::TumblingAggregate::Count<T> & t) { ar >> t.get(); }

		// CountAll<T>
		template<typename Archive, typename T>
		inline void save(Archive & ar, SPL::TumblingAggregate::CountAll<T> const & t) { ar << t(); }
		template<typename Archive, typename T>
		inline void load(Archive & ar, SPL::TumblingAggregate::CountAll<T> & t) { ar >> t.get(); }

		// Collect<A,T>
		template<typename Archive, typename A, typename T>
		inline void save(Archive & ar, SPL::TumblingAggregate::Collect<A,T> const & t) { ar << t(); }
		template<typename Archive, typename A, typename T>
		inline void load(Archive & ar,SPL::TumblingAggregate::Collect<A,T> & t) { ar >> t.get(); }

		// CountGroups<T>
		template<typename Archive, typename T>
		inline void save(Archive & ar, SPL::TumblingAggregate::CountGroups<T> const & t) {}
		template<typename Archive, typename T>
		inline void load(Archive & ar,SPL::TumblingAggregate::CountGroups<T> & t) {}

		// CountByGroup<T>
		template<typename Archive, typename T>
		inline void save(Archive & ar, SPL::TumblingAggregate::CountByGroup<T> const & t) { ar << t.get(); }
		template<typename Archive, typename T>
		inline void load(Archive & ar,SPL::TumblingAggregate::CountByGroup<T> & t) { ar >> t.get(); }

		// Last<A,T>
		template<typename Archive, typename A, typename T>
		inline void save(Archive & ar, SPL::TumblingAggregate::Last<A,T> const & t) { ar << t(); }
		template<typename Archive, typename A, typename T>
		inline void load(Archive & ar,SPL::TumblingAggregate::Last<A,T> & t) { ar >> t.get(); }

		// Any<A,T>
		template<typename Archive, typename A, typename T>
		inline void save(Archive & ar, SPL::TumblingAggregate::Any<A,T> const & t) { ar << t(); }
		template<typename Archive, typename A, typename T>
		inline void load(Archive & ar,SPL::TumblingAggregate::Any<A,T> & t) { ar >> t.get(); }
    } // namespace Serialization
}; // namespace SPL

#endif /*SPL_RUNTIME_TUMBLING_AGGREGATE_H */
