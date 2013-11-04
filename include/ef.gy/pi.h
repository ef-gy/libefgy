/**\file
 * \brief Calculate 'pi'
 *
 * This file contains a template class to calculate arbitrarily accurate
 * approximations of 'pi' based on the algorithm described by Bailey et al in
 * 1997.
 *
 * \copyright
 * Copyright (c) 2012-2013, ef.gy Project Members
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

#if !defined(EF_GY_PI_H)
#define EF_GY_PI_H

namespace efgy
{
    /**\brief Classes and functions dealing with mathematics
     *
     * This namespace contains all sorts of classes and functions more or less
     * loosely related to mathematics in one way or another. This does not
     * include code that fits better into some other namespace.
     *
     * Use the 'using' directive after including the headers you need if you
     * don't want to use fully qualified names everywhere:
     *
     * \code
     * using namespace efgy::math;
     * \endcode
     */
    namespace math
    {
        /**\brief Calculate 'pi' with arbitrary precision
         *
         * This class is used to handle (and calculate) pi with arbitrary
         * precision (up to the chosen data type's limits, anyway). Results
         * start to get unstable with iterations > 4 when used with the
         * default fraction data type. pi<Q,4> results in an approximation
         * that is already accurate to +/- 1.960*10^-7. If you use a larger
         * integer base for Q you should be able to get an arbitrarily precise
         * approximation to pi.
         *
         * To calculate pi we use the power series expansion described by
         * Bailey et al in 1997.
         *
         * \tparam Q          The data type to use in the calculations -
         *                    should be rational or similar. Must be a class
         *                    that defines an integer subtype and behaves like
         *                    a typical numeric data type.
         * \tparam iterations The number of iterations to run Bailey's
         *                    algorithm. This is a compile time constant in
         *                    the hope that the compiler will be able to
         *                    substitute a constant value for base data types.
         */
        template <typename Q, unsigned int iterations = 3>
        class pi
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
                 * of 1*pi.
                 */
                pi(void)
                    : factor(Q(1))
                    {}

                /**\brief Construct with factor
                 *
                 * Initialises the object so that it creates an approximation
                 * of pFactor * pi.
                 *
                 * \param[in] pFactor The factor to multiply pi with.
                 */
                pi(const Q &pFactor)
                    : factor(pFactor)
                    {}

                /**\brief Calculate approximation of pi
                 *
                 * Cast a pi object to the type you specified with the template
                 * parameter Q to access an appropriate approximation of pi.
                 *
                 * \return The approximation of pi with the given parameters.
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

            protected:
                /**\brief Get specific member of sequence
                 *
                 * Bailey's algorithm uses a sequence that converges to pi
                 * fairly quickly. This function generates the individual
                 * sequence members.
                 *
                 * \param[in] n Which sequence member to return.
                 *
                 * \return The requested sequence member.
                 */
                Q getSequenceMemberAt (const unsigned int &n) const
                {
                    const Q one = integer(1);
                    const Q two = integer(2);
                    const Q four = integer(4);
                    const Q five = integer(5);
                    const Q six = integer(6);
                    const Q eight = integer(8);
                    const Q d = Q(one) / Q(16);

                    const Q cn = integer(n);
                    Q rv = integer(1);

                    if (n > 0)
                    {
                        rv = d;
                    }

                    for (int i = 1; i < n; i++)
                    {
                        rv = rv * d;
                    }

                    return factor * rv * (four/(eight*cn+one) - two/(eight*cn+four) - one/(eight*cn+five) - one/(eight*cn+six));
                }

                /**\brief The factor to multiply pi with
                 *
                 * This is set to an appropriate value in the constructor.
                 */
                const Q factor;
        };
    };
};

#endif
