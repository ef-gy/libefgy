/**\file
 * \brief Series
 *
 * Contains supporting types for (potentially infinite) series.
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
 * \see Project Source Code: http://git.becquerel.org/jyujin/libefgy.git
 */

#if !defined(EF_GY_SERIES_H)
#define EF_GY_SERIES_H

#include <ef.gy/sequence.h>

namespace efgy
{
    namespace math
    {
        /**\brief Mathematical series
         *
         * Represents a (potentially infinite) series. Infinite series will be
         * truncated in the process of casting this to the base type.
         *
         * \tparam Q         Base type for calculations.
         * \tparam algorithm The algorithm to calculate the sequence members.
         * \tparam N         Base integral type; used for indices into the
         *                   sequence.
         */
        template<typename Q, template<typename, typename> class algorithm, typename N = unsigned long long>
        class series : public sequence<Q,algorithm,N>
        {
            public:
                using typename sequence<Q,algorithm,N>::sequenceAlgorithm;

                /**\brief Construct with factor and iterations
                 *
                 * Initialises a new series instance with the given factor and
                 * number of iterations.
                 *
                 * \param[in] pFactor     The factor to apply to the sequence
                 *                        members. Defaults to '1'.
                 * \param[in] pIterations The default number of iterations to
                 *                        use when approximating the series.
                 */
                series
                    (const Q pFactor = Q(1),
                     const N &pIterations = sequenceAlgorithm::defaultSeriesIterations)
                    : factor(pFactor), iterations(pIterations) {}

                /**\brief Get sum of first n+1 items
                 *
                 * Used to sum up the first n+1 sequence members when it becomes
                 * necessary to provide an approximation of the sequence.
                 *
                 * \param[in] n   Up to which sequence member to accumulate.
                 * \param[in] f   Factor to multiply the sequence members with.
                 *
                 * \returns The sum of the 0th to the nth sequence member.
                 */
                constexpr static Q get
                    (const N &n = sequenceAlgorithm::defaultSeriesIterations,
                     const Q &f = Q(1))
                {
                    return sumTo (n, f, Q(0));
                }

                /**\brief Calculate approximation
                 *
                 * Cast an actual instance of the series to its base type to get
                 * an approximation with the parameters stored in the type.
                 *
                 * \return The approximation of the series with the parameters
                 *         in the instance.
                 */
                constexpr operator Q (void) const
                {
                    return get (iterations, factor);
                }

            protected:
                /**\brief Base sequence
                 *
                 * The sequence that is used as the basis for the series.
                 */
                typedef sequence<Q,algorithm,N> sequence;

                /**\brief Constant summation function
                 *
                 * Used to sum up the first n+1 members of the sequence. This
                 * method is tail-recursive so it shouldn't make your stack
                 * explode. It is also static and constexpr, meaning it should
                 * be evaluated at compile time.
                 *
                 * \param[in] n   Up to which sequence member to accumulate.
                 * \param[in] f   Factor to multiply the sequence members with.
                 * \param[in] acc The initial (or current) value; used for tail
                 *                recursion.
                 *
                 * \returns The sum of the 0th to the nth sequence member.
                 */
                constexpr static Q sumTo (const N &n, const Q &f, const Q &acc)
                {
                    return n == 0
                         ?                acc + (sequence::at (0) * f)
                         : sumTo (n-1, f, acc + (sequence::at (n) * f));
                }

                /**\brief Number of iterations
                 *
                 * When approximating the sequence, this determines up to which
                 * element the series is summed up to.
                 */
                const N iterations;

                /**\brief Series factor
                 *
                 * This factor is applied to each sequence member when
                 * approximating an instance of the series.
                 */
                const Q factor;
        };
    };
};

#endif
