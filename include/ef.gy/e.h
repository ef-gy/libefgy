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

#if !defined(EF_GY_E_H)
#define EF_GY_E_H

namespace efgy
{
    namespace math
    {
        /* This class is used to handle (and calculate) e^q with arbitrary precision (up to
         * the chosen base datatypes' limits). Results start to get unstable with iterations
         * > 14 when using 64-bit integer fractions and an exponent of 1. 10 iterations
         * result in an accuracy of about +/- 3.028*10^7.
         *
         * To calculate e we use the fairly popular infinite series expansion of e.
         */
        template <typename Q, unsigned int iterations = 10>
        class e
        {
            public:
                typedef typename Q::integer integer;
                typedef Q rational;

                e()
                    : factor(integer(1)), exponent(integer(1))
                    {}

                e(const Q &pFactor)
                    : factor(pFactor), exponent(Q(1))
                    {}

                e(const Q &pFactor, const Q &pExponent)
                    : factor(pFactor), exponent(pExponent)
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
                Q exponent;

            protected:
                Q getSequenceMemberAt (unsigned int n) const
                {
                    return factor * ((exponent^(integer(n))) / Q(factorial<integer>(integer(n))));
                }
        };
    };
};

#endif
