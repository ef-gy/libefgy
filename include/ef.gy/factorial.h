/**\file
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

#if !defined(EF_GY_FACTORIAL_H)
#define EF_GY_FACTORIAL_H

namespace efgy
{
    namespace math
    {
        /**\brief A template to calculate the factorial of a number.
         *
         * Uses a basic iterative algorithm.
         *
         * \tparam Z Basic arithmetic type used for calculations.
         * Z is required to allow integral values and multiplication,
         * but there are no further restrictions on Z imposed by the code,
         * although the factorial function is only defined on the positive 
         * integers, so for larger types this template implements a partial
         * function.
         *
         * The current behaviour for negative numbers is to return 1.
         */
        template <typename Z>
        class factorial
        {
            public:
                factorial()
                    : integer(Z(0))
                    {}

                factorial(const Z &pInteger)
                    : integer(pInteger)
                    {}

                operator Z () const
                {
                    Z rv = Z(1);

                    for (Z n = Z(2); n <= integer; n++)
                    {
                        rv = rv * n;
                    }

                    return rv;
                }

                Z integer;
        };
    };
};

#endif
