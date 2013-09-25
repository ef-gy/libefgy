/*
 * This file is part of the ef.gy project.
 * See the appropriate repository at http://ef.gy/.git for exact file
 * modification records.
*/

/*
 * Copyright (c) 2012, ef.gy Project Members
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

namespace efgy
{
    namespace math
    {
        /* This class is used to handle (and calculate) pi with arbitrary precision (up to the
         * chosen data type's limits, anyway). Results start to get unstable with iterations > 4
         * when used with the default fraction data type. pi<Q,4> results in an approximation
         * that is already accurate to +/- 1.960*10^-7. If you use a larger integer base for
         * Q, you should be able to get an arbitrarily precise approximation to pi.
         *
         * To calculate pi we use the power series expansion described by Bailey et al in 1997.
         * */
        template <typename Q, unsigned int iterations = 3>
        class pi
        {
            public:
                typedef typename Q::integer integer;
                typedef Q rational;

                pi()
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
                    Q one = integer(1);
                    Q two = integer(2);
                    Q four = integer(4);
                    Q five = integer(5);
                    Q six = integer(6);
                    Q eight = integer(8);
                    Q d = one / integer(16);

                    Q cn = integer(n);
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
