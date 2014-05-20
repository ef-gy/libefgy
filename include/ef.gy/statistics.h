/**\file
 * \brief Contains the basic statistics tools
 *
 * Contains the efgy::statistics namespace and some basic tools that come in
 * handy with test cases.
 *
 * \copyright
 * Copyright (c) 2012-2014, ef.gy Project Members
 * \copyright
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * \copyright
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * \copyright
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * \see Project Documentation: http://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 */

#if !defined(EF_GY_STATISTICS_H)
#define EF_GY_STATISTICS_H

#include <array>
#include <ef.gy/maybe.h>
#include <ef.gy/numeric.h>
#include <numeric>

namespace efgy
{
    /**\brief Statistical functions
     *
     * Contains functions related to the field of statistics, which among other
     * uses tends to come in handy when writing test cases over random input.
     */
    namespace statistics
    {
        /**\brief Calculate the average of a vector
         *
         * Calculates the average of all values in a vector by adding all of the
         * items and then dividing by the number of items in the vector.
         *
         * \tparam Q The data type of the vector elements.
         *
         * \param[in] input The vector over which to calculate the average.
         *
         * \returns The average of the values in the input vector.
         */
        template<typename Q>
        static maybe<Q> average (const std::vector<Q> &input)
        {
            if (input.size() == 0)
            {
                return maybe<Q>();
            }

            Q s = 0;
            for (const Q &i : input)
            {
                s += i;
            }
            return maybe<Q>(s / Q(input.size()));
        }

        /**\brief Calculates the variance of a range.
         *
         * Calculates the variance of a range given by a two iterators.
         *
         * \tparam T The type of the actual values.
         * \tparam _InputIterator The type of iterators for begin and end.
         *
         * \param[in] begin The start of the range.
         * \param[in] end The end of the range.
         *
         * \code
         * vector<double> list = {1.0, 2.0, 1.0};
         * double var = variance<double>(list.begin(), list.end());
         * \endcode
         *
         * \returns The resulting variance.
         */
        template<typename T, typename _InputIterator>
        static maybe<T> variance(_InputIterator begin, const _InputIterator &end)
        {
            if (begin == end)
            {
                return maybe<T>();
            }

            T sum = std::accumulate(begin, end, 0.0);
            T sum_square = std::accumulate(begin, end, 0.0, [](T first, T element) { return first + element * element; });
            unsigned int count_elements = end - begin;

            T var = (sum_square - sum * sum / count_elements) / count_elements;

            return maybe<T>(var);
        }

        /**\brief Calculate the variance of a vector
         *
         * Same as variance() over a range, but over a vector instead of two
         * iterators.
         *
         * \tparam T The type of the actual values.
         *
         * \param[in] input The input vector to process.
         *
         * \returns The resulting variance.
         */
        template<typename T>
        static maybe<T> variance(const std::vector<T> &input)
        {
            return variance<T>(input.begin(), input.end());
        }

        /**
         * Calculates chi^2 for statistics.
         */
        template<typename T, typename _InputIterator>
        static maybe<T> chi_square(_InputIterator begin, _InputIterator end, _InputIterator measurement_begin, _InputIterator measurement_end)
        {
            T var = variance<double>(measurement_begin, measurement_end);
            // TODO Check for 0 variances.
            if (begin == end)
            {
                return maybe<T>();
            }

            unsigned int count_elements = end - begin;

            T sum = 0.0;
            for (; begin != end && measurement_begin != measurement_end; ++begin, ++measurement_begin)
            {
                sum += efgy::math::numeric::pow2((*measurement_begin - *begin) / (T) var);
            }

            return sum;
        }

        template<typename T>
        static maybe<T> chi_square(const std::vector<T> values, const std::vector<T> measurements)
        {
           return chi_square<T>(values.begin(), values.end(), measurements.begin(), measurements.end());
        }
    };
};

#endif
