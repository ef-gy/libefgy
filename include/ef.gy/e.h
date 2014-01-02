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
 * \see Project Source Code: http://git.becquerel.org/jyujin/libefgy.git
 */

#if !defined(EF_GY_E_H)
#define EF_GY_E_H

namespace efgy
{
    namespace math
    {
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
         * \tparam iterations The number of iterations to use when calculating
         *                    the approximation. This is a compile time
         *                    constant in the hope that the compiler will be
         *                    able to substitute a constant value for base data
         *                    types.
         */
        template <typename Q, unsigned int iterations = 10>
        class e
        {
            public:
                /**\brief The base data type's integer type
                 *
                 * Used to make sure that type casts work as intended.
                 */
                typedef typename Q::integer integer;

                /**\brief Default constructor
                 *
                 * Initialises the object so that it creates an approximation
                 * of 1*e^1.
                 */
                e()
                    : factor(integer(1)), exponent(integer(1))
                    {}

                /**\brief Construct with factor
                 *
                 * Initialises the object so that it creates an approximation
                 * of pFactor * e^1.
                 *
                 * \param[in] pFactor The factor to multiply e with.
                 */
                e(const Q &pFactor)
                    : factor(pFactor), exponent(Q(1))
                    {}

                /**\brief Construct with factor and exponent
                 *
                 * Initialises the object so that it creates an approximation
                 * of pFactor * e^pExponent.
                 *
                 * \param[in] pFactor   The factor to multiply e with.
                 * \param[in] pExponent The exponent to raise e to.
                 */
                e(const Q &pFactor, const Q &pExponent)
                    : factor(pFactor), exponent(pExponent)
                    {}

                /**\brief Calculate approximation of e
                 *
                 * Cast an e object to the type you specified with the template
                 * parameter Q to access an appropriate approximation of e.
                 *
                 * \return The approximation of e with the given parameters.
                 */
                operator Q (void) const
                {
                    Q rv = Q();

                    for (unsigned int i = 0; i < iterations; i++)
                    {
                        rv = rv + getSequenceMemberAt (i);
                    }

                    return rv;
                }

                Q factor;
                Q exponent;

            protected:
                /**\brief Get specific member of sequence
                 *
                 * Calculates the member at position 'n' in the infinite series
                 * we use to calculate 'e'.
                 *
                 * \param[in] n Which sequence member to return.
                 *
                 * \return The requested sequence member.
                 */
                Q getSequenceMemberAt (const unsigned int &n) const
                {
                    return factor * ((exponent^(integer(n))) / Q(factorial<integer>(integer(n))));
                }
        };
    };
};

#endif
