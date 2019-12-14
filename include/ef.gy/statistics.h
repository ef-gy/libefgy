/* Contains the basic statistics tools
 *
 * Contains the efgy::statistics namespace and some basic tools that come in
 * handy with test cases.
 *
 * See also:
 * * Project Documentation: https://ef.gy/documentation/libefgy
 * * Project Source Code: https://github.com/ef-gy/libefgy
 * * Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 *
 * @copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 */

#if !defined(EF_GY_STATISTICS_H)
#define EF_GY_STATISTICS_H

#include <ef.gy/numeric.h>

#include <array>
#include <numeric>
#include <optional>
#include <vector>

namespace efgy {
/* Statistical functions
 *
 * Contains functions related to the field of statistics, which among other
 * uses tends to come in handy when writing test cases over random input.
 */
namespace statistics {
/* Calculate the average of a vector
 * @Q The data type of the vector elements.
 * @input The vector over which to calculate the average.
 *
 * Calculates the average of all values in a vector by adding all of the
 * items and then dividing by the number of items in the vector.
 *
 * @return The average of the values in the input vector.
 */
template <typename Q>
static std::optional<Q> average(const std::vector<Q> &input) {
  if (input.size() == 0) {
    return std::optional<Q>();
  }

  Q s = 0;
  for (const Q &i : input) {
    s += i;
  }
  return std::optional<Q>(s / Q(input.size()));
}

/* Calculates the variance of a range.
 * @T The type of the actual values.
 * @_InputIterator The type of iterators for begin and end.
 * @begin The start of the range.
 * @end The end of the range.
 *
 * Calculates the variance of a range given by a two iterators.
 *
 *     vector<double> list = {1.0, 2.0, 1.0};
 *     double var = variance<double>(list.begin(), list.end());
 *
 * @return The resulting variance.
 */
template <typename T, typename _InputIterator>
static std::optional<T> variance(_InputIterator begin,
                                 const _InputIterator &end) {
  if (begin == end) {
    return std::optional<T>();
  }

  T sum = std::accumulate(begin, end, 0.0);
  T sum_square = std::accumulate(begin, end, 0.0, [](T first, T element) {
    return first + element * element;
  });
  unsigned int count_elements = end - begin;

  T var = (sum_square - sum * sum / count_elements) / count_elements;

  return std::optional<T>(var);
}

/* Calculate the variance of a vector
 * @T The type of the actual values.
 * @input The input vector to process.
 *
 * Same as variance() over a range, but over a vector instead of two iterators.
 *
 * @return The resulting variance.
 */
template <typename T>
static std::optional<T> variance(const std::vector<T> &input) {
  return variance<T>(input.begin(), input.end());
}

/* Calculates chi^2 for statistics. */
template <typename T, typename _InputIterator>
static std::optional<T> chi_square(_InputIterator begin, _InputIterator end,
                                   _InputIterator measurement_begin,
                                   _InputIterator measurement_end) {
  T var = *variance<double>(measurement_begin, measurement_end);
  // TODO Check for 0 variances.
  if (begin == end) {
    return std::optional<T>();
  }

  unsigned int count_elements = end - begin;

  T sum = 0.0;
  for (; begin != end && measurement_begin != measurement_end;
       ++begin, ++measurement_begin) {
    sum += efgy::math::numeric::pow2((*measurement_begin - *begin) / (T)var);
  }

  return sum;
}

template <typename T>
static std::optional<T> chi_square(const std::vector<T> values,
                                   const std::vector<T> measurements) {
  return chi_square<T>(values.begin(), values.end(), measurements.begin(),
                       measurements.end());
}
}  // namespace statistics
}  // namespace efgy

#endif
