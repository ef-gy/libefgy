/**\file
 * \brief Complex numbers
 *
 * Contains templates dealing with complex numbers.
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

#if !defined(EF_GY_COMPLEX_H)
#define EF_GY_COMPLEX_H

#include <ef.gy/fractions.h>
#include <ef.gy/traits.h>

namespace efgy
{
    namespace math
    {
        template <typename Q>
        class complex
        {
            public:
                typedef typename numeric::traits<Q>::integral integral;
                typedef Q rational;

                complex ()
                    : one(Q(0)), i(Q(0))
                    {}

                complex (const Q &pOne)
                    : one(pOne), i(Q(0))
                    {}

                complex (const Q &pOne, const Q &pI)
                    : one(pOne), i(pI)
                    {}

                complex (const integral &pOne, const integral &pI)
                    : one(Q(pOne)), i(Q(pI))
                    {}

                complex &operator = (const complex &b)
                {
                    one = b.one;
                    i = b.i;

                    return *this;
                }

                complex &operator = (const Q &b)
                {
                    one = b;
                    i = integral(0);

                    return *this;
                }

                complex operator + (const complex &b) const
                {
                    return complex (one + b.one, i + b.i);
                }

                complex &operator += (const complex &b)
                {
                    complex r = (*this) + b;

                    return ((*this) = r);
                }

                complex operator - (const complex &b) const
                {
                    return complex (one - b.one, i - b.i);
                }

                complex &operator -= (const complex &b)
                {
                    complex r = (*this) - b;

                    return ((*this) = r);
                }

                complex operator * (const complex &b) const
                {
                    // (a + bi) * (c + di) = (ac - bd) + (bc + ad)*i
                    return complex (one * b.one - i * b.i, i * b.one + one * b.i);
                }

                complex &operator *= (const complex &b)
                {
                    complex r = (*this) * b;

                    return ((*this) = r);
                }

                complex operator ^ (const integral &b) const
                {
                    if (b == integral(0))
                    {
                        return complex(integral(1));
                    }
                    else
                    {
                        complex rv = *this;

                        for (integral i = integral(1); i < b; i++)
                        {
                            rv *= (*this);
                        }

                        return rv;
                    }
                }

                complex &operator ^= (const integral &b)
                {
                    complex r = (*this) ^ b;

                    return ((*this) = r);
                }

                complex operator / (const complex &b) const
                {
                    // (a + bi) / (c + di) = (ac + bd)/(c*c + d*d) + (bc - ad)/(c*c + d*d)*i
                    return complex
                        ((one * b.one + i * b.i) / (b.one * b.one + b.i * b.i),
                         (i * b.one - one * b.i) / (b.one * b.one + b.i * b.i));
                }

                complex &operator /= (const complex &b)
                {
                    complex r = (*this) / b;

                    return ((*this) = r);
                }

                Q one;
                Q i;
        };

        namespace numeric
        {
            template<typename Q>
            class traits<complex<Q>>
            {
                public:
                    typedef typename traits<Q>::integral integral;
                    typedef typename traits<Q>::rational rational;
                    typedef complex<Q> self;
                    typedef complex<Q> derivable;

                    static const bool stable = false;
            };
        };
    };
};

#endif
