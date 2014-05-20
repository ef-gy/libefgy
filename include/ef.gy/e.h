/**\file
 * \brief Calculate 'e'
 *
 * Contains a template class to calculate 'e' with arbitrary precision based on
 * the infinite series expansion of 'e'.
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

#if !defined(EF_GY_E_H)
#define EF_GY_E_H

#include <ef.gy/factorial.h>
#include <ef.gy/series.h>

namespace efgy
{
    namespace math
    {
        namespace algorithm
        {
            /**\brief 'e' power series sequence
             *
             * This algorithm calculates the power series sequence members to
             * calculate the exponential function.
             *
             * \tparam Q The data type to use in the calculations, e.g. double.
             * \tparam N Sequence index type. 
             */
            template<typename Q, typename N>
            class powerSeriesE
            {
                public:
                    /**\copydoc bailey1997::defaultSeriesIterations */
                    static const N defaultSeriesIterations = 10;

                    /**\brief Get sequence member
                     *
                     * Calculates a single member of the sequence used to
                     * calculate the exponential function.
                     *
                     * \param[in] n The sequence member to calculate.
                     *
                     * \returns The sequence member that was to be calculated.
                     */
                    static constexpr Q at (const N &n)
                    {
                        return Q(1)
                             / Q(factorial<integer>(integer(n)));
                    }

                protected:
                    /**\brief The base data type's integer type
                     *
                     * Used to make sure that type casts work as intended.
                     */
                    typedef typename numeric::traits<Q>::integral integer;
            };
        };

        /**\brief Calculate 'e' with arbitrary precision
         *
         * This class is used to handle (and calculate) e^q with arbitrary
         * precision (up to the chosen base datatypes' limits). Results start
         * to get unstable with iterations > 14 when using 64-bit integer
         * fractions and an exponent of 1. 10 iterations result in an accuracy
         * of about +/- 3.028*10^7.
         *
         * To calculate e we use the popular infinite series expansion of e.
         *
         * \tparam Q          The data type to use in the calculations -
         *                    should be rational or similar. Must be a class
         *                    that defines an integer subtype and behaves like
         *                    a typical numeric data type.
         * \tparam N          Base integral type; used to specify the
         *                    precision.
         *
         * Usage of this class, and the reasoning behind that usage, is the same
         * as for the math::pi template. In short, you can use either of these:
         *
         * \code{.cpp}
         * double myE = math::e<double>();
         * double myE = math::e<double>::get();
         * (double)math::e<double>();
         * \endcode
         *
         * \see For more details, see math::pi.
         */
        template <typename Q, typename N = unsigned long long>
        using e = series::power<Q,algorithm::powerSeriesE,N>;
    };
};

#endif
