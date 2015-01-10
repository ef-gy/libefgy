/**\file
 * \brief Series
 *
 * Contains supporting types for (potentially infinite) series.
 *
 * \copyright
 * Copyright (c) 2012-2015, ef.gy Project Members
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

#if !defined(EF_GY_SERIES_H)
#define EF_GY_SERIES_H

#include <ef.gy/sequence.h>
#include <ef.gy/exponential.h>

namespace efgy
{
    namespace math
    {
        /**\brief Mathematical series templates
         *
         * This namespace contains templates for the different types of series
         * that aries in mathematics, starting with the series::series template
         * that describes the most basic form of series.
         */
        namespace series
        {
            /**\brief Basic series
             *
             * Represents a (potentially infinite) series. Infinite series will
             * be truncated in the process of casting this to the base type.
             *
             * \tparam Q         Base type for calculations.
             * \tparam algorithm The algorithm to calculate the sequence
             *                   members.
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
                     * Initialises a new series instance with the given factor
                     * and number of iterations.
                     *
                     * \param[in] pFactor     The factor to apply to the
                     *                        sequence members. Defaults to '1'.
                     * \param[in] pIterations The default number of iterations
                     *                        to use when approximating the
                     *                        series.
                     */
                    series
                        (const Q pFactor = Q(1),
                         const N &pIterations = sequenceAlgorithm::defaultSeriesIterations)
                        : factor(pFactor), iterations(pIterations) {}

                    /**\brief Get sum of first n+1 items
                     *
                     * Used to sum up the first n+1 sequence members when it
                     * becomes necessary to provide an approximation of the
                     * sequence.
                     *
                     * \param[in] n Up to which sequence member to accumulate.
                     * \param[in] f Factor to multiply the sequence members
                     *              with.
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
                     * Cast an actual instance of the series to its base type to
                     * get an approximation with the parameters stored in the
                     * type.
                     *
                     * \return The approximation of the series with the
                     *         parameters in the instance.
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
                     * Used to sum up the first n+1 members of the sequence.
                     * This method is tail-recursive so it shouldn't make your
                     * stack explode. It is also static and constexpr, meaning
                     * it should be evaluated at compile time.
                     *
                     * \param[in] n   Up to which sequence member to accumulate.
                     * \param[in] f   Factor to multiply the sequence members
                     *                with.
                     * \param[in] acc The initial (or current) value; used for
                     *                tail recursion.
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
                     * When approximating the sequence, this determines up to
                     * which element the series is summed up to.
                     */
                    const N iterations;

                    /**\brief Series factor
                     *
                     * This factor is applied to each sequence member when
                     * approximating an instance of the series.
                     */
                    const Q factor;
            };

            /**\brief Power series
             *
             * Based on the regular series, this represents a power series,
             * which is basically like a regular series but with two additional
             * parameters: a power factor and a centre.
             *
             * \tparam Q         Base type for calculations.
             * \tparam algorithm The algorithm to calculate the sequence
             *                   members.
             * \tparam N         Base integral type; used for indices into the
             *                   sequence.
             */
            template<typename Q, template<typename, typename> class algorithm, typename N = unsigned long long>
            class power : public series<Q,algorithm,N>
            {
                using typename series<Q,algorithm,N>::sequence;
                using typename series<Q,algorithm,N>::sequenceAlgorithm;
                using series<Q,algorithm,N>::iterations;
                using series<Q,algorithm,N>::factor;

                public:
                    /**\brief Construct with factors and iterations
                     *
                     * Initialises a new series instance with the given factors
                     * and number of iterations.
                     *
                     * \param[in] pFactor      The factor to apply to the
                     *                         sequence members.
                     * \param[in] pPowerFactor The factor that is raised to the
                     *                         n'th power for each of the
                     *                         sequence members.
                     * \param[in] pCentre      The centre that is subtracted
                     *                         from the power factor.
                     * \param[in] pIterations  The default number of iterations
                     *                         to use when approximating the
                     *                         series.
                     */
                    power
                        (const Q pFactor = Q(1),
                         const Q pPowerFactor = Q(1),
                         const Q pCentre = Q(0),
                         const N &pIterations = sequenceAlgorithm::defaultSeriesIterations)
                        : series<Q,algorithm,N>(pFactor,pIterations),
                          powerFactor(pPowerFactor), centre(pCentre)
                          {}

                    /**\copydoc series::get
                     *
                     * The power series version of this function has two
                     * additional parameters for the power factor and the centre
                     * of the power series:
                     *
                     * \param[in] x The power factor.
                     * \param[in] c The centre of the power series.
                     */
                    constexpr static Q get
                        (const N &n = sequenceAlgorithm::defaultSeriesIterations,
                         const Q &f = Q(1),
                         const Q &x = Q(1),
                         const Q &c = Q(0))
                    {
                        return sumTo (n, f, x, c, Q(0));
                    }

                    /**\brief Calculate approximation
                     *
                     * Cast an actual instance of the series to its base type to
                     * get an approximation with the parameters stored in the
                     * type.
                     *
                     * \return The approximation of the series with the
                     *         parameters in the instance.
                     */
                    constexpr operator Q (void) const
                    {
                        return get (iterations, factor, powerFactor, centre);
                    }

                protected:
                    /**\copydoc series::sumTo
                     *
                     * This varaint of the summation function has two additional
                     * parameters for the power factor and the centre of the
                     * power series:
                     *
                     * \param[in] x The power factor.
                     * \param[in] c The centre of the power series.
                     */
                    constexpr static Q sumTo (const N &n, const Q &f, const Q &x, const Q &c, const Q &acc)
                    {
                        return n == 0
                             ?                      acc + (  sequence::at (0) * f)
                             : sumTo (n-1, f, x, c, acc + (  sequence::at (n) * f
                                                           * exponentiate::integral<Q>::raise(x - c, n)));
                    }

                    /**\brief Centre
                     *
                     * This value is subtracted from the power factor before it
                     * is raised and subsequently multiplied.
                     */
                    const Q centre;

                    /**\brief Power factor
                     *
                     * This factor is applied to each sequence member when
                     * approximating an instance of the series. Unlike the
                     * other factor, this one is raised to the n'th power before
                     * being multiplied with the sequence member.
                     */
                    const Q powerFactor;
            };
        };
    };
};

#endif
