/**\file
 * \brief Trigonometrics
 *
 * Contains common trigonometric functions: sine, secant, tangent, cotangent.
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

#if !defined(EF_GY_TRIGONOMETRICS_H)
#define EF_GY_TRIGONOMETRICS_H

#include <ef.gy/complex.h>
#include <ef.gy/e.h>

namespace efgy
{
    namespace math
    {
        template <typename Q, typename N = unsigned long long>
        Q sine (const Q &pTheta, Q &oCosine, const N &iterations = N(10))
        {
            complex<Q> z = e<complex<Q>>(complex<Q>(Q(1),Q(0)), complex<Q>(Q(0),pTheta), complex<Q>(Q(0),Q(0)), iterations);

            oCosine = z.one;

            return z.i;
        }

        template <typename Q, typename N = unsigned long long>
        Q secant (const Q &pTheta, Q &oCosecant, const N &iterations = N(10))
        {
            complex<Q> z = e<complex<Q>>(complex<Q>(Q(1),Q(0)), complex<Q>(Q(0),pTheta), complex<Q>(Q(0),Q(0)), iterations);

            oCosecant = Q(1)/z.i;

            return Q(1)/z.one;
        }

        template <typename Q, typename N = unsigned long long>
        Q tangent (const Q &pTheta, const N &iterations = N(10))
        {
            Q s, c;

            s = sine (pTheta, c);

            return s / c;
        }

        template <typename Q, typename N = unsigned long long>
        Q cotangent (const Q &pTheta, const N &iterations = N(10))
        {
            Q s, c;

            s = sine (pTheta, c);

            return c / s;
        }
    };
};

#endif
