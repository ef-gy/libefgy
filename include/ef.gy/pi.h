/*
 * This file is part of the ef.gy project.
 * See the appropriate repository at http://ef.gy/.git for exact file
 * modification records.
*/

/*
 * Copyright (c) 2012-2013, ef.gy Project Members
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

#if !defined(EF_GY_PI_H)
#define EF_GY_PI_H

/**\file
 * \brief Calculate 'pi'
 *
 * This file contains a template class to calculate arbitrarily accurate
 * approximations of 'pi' based on the algorithm described by Bailey et al in
 * 1997.
 */

/**\brief Base namespace
 *
 * This is the base namespace for all the headers contained within libefgy.
 * This namespace is used to prevent namespace clashes with your own code - if
 * you don't want to use fully qualified names in your code, use the 'using'
 * directive after including the headers, like so:
 *
 * \code
 * using namespace efgy;
 * \endcode
 */
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
                 */
                typedef typename Q::integer integer;
                typedef Q rational;

                pi(void)
                    : factor(Q(1))
                    {}

                pi(const Q &pFactor)
                    : factor(pFactor)
                    {}

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

            protected:
                Q getSequenceMemberAt (unsigned int n) const
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
        };
    };
};

#endif
