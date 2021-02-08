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

#ifndef SPL_RUNTIME_WINDOW_INTERVAL_H
#define SPL_RUNTIME_WINDOW_INTERVAL_H

/**
 * @file Interval.h
 * @brief Definition of the interval<T> class.
 */

#include <utility>
#include <stdexcept>

namespace SPL {

/**
 * @brief Class that represents an interval.
 *
 * This interval is right-open, which means that it contains
 * all the elements between (and including) its left endpoint, and (excluding)
 * its right endpoint.
 *
 * @tparam T  the type of elements of the interval. The endpoint type must
 *            be totally ordered.
 */
template<typename T>
class interval
{
private:
  T left_;
  T right_;

public:
  /// Constructs an empty interval.
  interval() : left_(), right_() {}

  /**
   * @brief Constructs an empty interval which has a left endpoint specified
   * by @c endpoint.
   *
   * @param endpoint the left interval endpoint
   */
  interval(T endpoint) : left_(endpoint), right_(endpoint) {}

  /**
   * @brief Copy constructor.
   *
   * @param other other interval
   */
  interval(const interval& other)
      : left_(other.left_),
        right_(other.right_) {}

  /**
   * @brief Assignment operator.
   *
   * @param other another interval
   */
  interval const & operator= (interval const & other)
  {
    left_ = other.left();
    right_ = other.right();
    return *this;
  }

  /**
   * @brief Equality comparison.
   * @param other interval to compare with
   * @return @c true if the intervals are the same, otherwise @c false
   */
  bool operator==(interval const & other) const
  {
      if (left_ != other.left_) {
          return false;
      }
      if (right_ != other.right_) {
          return false;
      }
      return true;
  }

  /**
   * @brief Constructs an interval specified by its left endpoint
   * and a size.
   *
   * The endpoint type must be @c %Addable and @c %Subtractable, so it must
   * have <tt>operator +</tt> and an <tt>operator -</tt> defined.
   *
   * @param value the left endpoint
   * @param size the distance between the left and the right endpoint
   * @throws std::invalid_argument if the @c size is negative
   */
  interval(T value, T size) : left_(value), right_(left_ + size)
  {
    if (size < 0) {
      throw std::invalid_argument(std::string("size < 0"));
    }
  }

  /**
   * @brief Get left (low) endpoint.
   * @return the left endpoint
   */
  T const & left() const
  { return left_; }

  /**
   * @brief Get right (high) endpoint.
   * @return the right endpoint
   */
  T const & right() const
  { return right_; }

  /* Boolean tests */

  /**
   * @brief Determines whether the interval is empty.
   *
   * An interval is empty iff its left endpoint equals its right endpoint.
   *
   * @return @c true if the interval is empty, otherwise @c false
   */
  bool empty() const
  { return right_ == left_; }

  /**
   * @brief Determines whether this interval touches the @c other.
   *
   * Interval @c a touches interval @c b iff <tt>a.left() == b.right()</tt>
   * or <tt>a.right() == b.left()</tt>.
   *
   * @param other the other interval
   * @return @c true if this interval touches the @c other, otherwise @c false
   */
  bool touches(interval<T> other) const
  { return right_ == other.left_ || left_ == other.right_; }

  /**
   * @brief Determines whether this overlaps touches the @c other.
   *
   * Interval @c a overlaps interval @c b iff <tt>a.left() < b.right()</tt>
   * or <tt>a.right() > b.left()</tt>.
   *
   * @param other the other interval
   * @return @c true if this interval overlaps the @c other, otherwise @c false
   */
  bool overlaps(interval<T> other) const
  { return right_ > other.left_ || left_ < other.right_; }

  /**
   * @brief Determines whether this interval includes the specified
   *        @c timestamp.
   *
   * This interval includes a timestamp if the timestamp is larger or equal
   * to the left endpoint and it is smaller than the right endpoint.
   *
   * @param ts a @c timestamp
   * @return @c true if this interval includes the givem @c timestamp,
   *        otherwise @c false
   */
  bool includes(SPL::timestamp ts) const
  { return left_ <= ts && ts < right_; }

  /**
   * @brief Swap with another @c interval.
   * @param other interval to swap with
   */
  void swap(interval& other)
  {
    T tmp = left_;
    left_ = other.left_;
    other.left_ = tmp;

    tmp = right_;
    right_ = other.right_;
    other.right_ = tmp;
  }

  /**
   * @brief Calculates hash code.
   * @return hash code
   */
  size_t hashCode() const
  {
      size_t r = 17;
      std::tr1::hash<T> hs;
      r = 37 * r + hs(left_);
      r = 37 * r + hs(right_);
      return r;
  }
};

template<typename T>
inline std::ostream & operator<<(std::ostream & stream, const interval<T> & i)
{
  stream << '[' << i.left() << ',' << i.right() << ')';
  return stream;
}
}    // end namespace SPL

namespace std {
  template<typename T>
  inline void swap(SPL::interval<T>& a, SPL::interval<T>& b)
  { a.swap(b); }
}

namespace std { namespace tr1 {
  template<typename T>
  struct hash<SPL::interval<T> > {
    inline size_t operator()(SPL::interval<T> const & value) const
    { return value.hashCode(); }
  };
}}

#endif // SPL_RUNTIME_WINDOW_INTERVAL_H
