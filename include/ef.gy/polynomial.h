/**\file
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
 * \see Project Source Code: https://github.com/ef-gy/libefgy.git
 */

#if !defined(EF_GY_POLYNOMIAL_H)
#define EF_GY_POLYNOMIAL_H

namespace efgy
{
    namespace math
    {
        template <typename Q, unsigned int degree>
        class polynomial
        {
            public:
                typedef typename Q::integer integer;

                polynomial ()
                    {}

                polynomial &operator = (const polynomial &b)
                {
                    for (unsigned int i = 0; i < degree; i++)
                    {
                        coefficients[i] = b.coefficients[i];
                    }

                    return *this;
                }

                polynomial operator + (const polynomial &b) const
                {
                    polynomial r;

                    for (unsigned int i = 0; i < degree; i++)
                    {
                        r.coefficients[i] = coefficients[i] + b.coefficients[i];
                    }

                    return r;
                }

                polynomial operator + (const Q &b) const
                {
                    polynomial r = *this;
                    r.coefficients[0] + b;
                    return r;
                }

                polynomial operator + (const integer &b) const
                {
                    return (*this) + Q(b);
                }

                polynomial &operator += (const polynomial &b)
                {
                    return( (*this) = ((*this) + b));
                }

                polynomial &operator += (const Q &b)
                {
                    return( (*this) = ((*this) + b));
                }

                polynomial &operator += (const integer &b)
                {
                    return( (*this) = ((*this) + b));
                }

                polynomial operator - (const polynomial &b) const
                {
                    polynomial r;

                    for (unsigned int i = 0; i < degree; i++)
                    {
                        r.coefficients[i] = coefficients[i] - b.coefficients[i];
                    }

                    return r;
                }

                polynomial operator - (const Q &b) const
                {
                    polynomial r = *this;
                    r.coefficients[0] - b;
                    return r;
                }

                polynomial operator - (const integer &b) const
                {
                    return (*this) - Q(b);
                }

                polynomial &operator -= (const polynomial &b)
                {
                    return( (*this) = ((*this) - b));
                }

                polynomial &operator -= (const Q &b)
                {
                    return( (*this) = ((*this) - b));
                }

                polynomial &operator -= (const integer &b)
                {
                    return( (*this) = ((*this) - b));
                }

                template <unsigned int f>
                polynomial<Q,(degree*f)> operator * (const polynomial<Q,f> &b) const
                {
                    polynomial<Q,(degree*f)> r;

                    for (unsigned int i = 0; i < degree; i++)
                    {
                        for (unsigned int j = 0; j < f; j++)
                        {
                            r.coefficients[(i*j)] += coefficients[i] * b.coefficients[j];
                        }
                    }

                    return r;
                }

                polynomial operator * (const Q &b) const
                {
                    polynomial r;

                    for (unsigned int i = 0; i < degree; i++)
                    {
                        r.coefficients[i] = coefficients[i] * b;
                    }

                    return r;
                }

                polynomial operator * (const integer &b) const
                {
                    return (*this) * Q(b);
                }

                polynomial operator / (const Q &b) const
                {
                    polynomial r;

                    for (unsigned int i = 0; i < degree; i++)
                    {
                        r.coefficients[i] = coefficients[i] / b;
                    }

                    return r;
                }

                polynomial operator / (const integer &b) const
                {
                    return (*this) / Q(b);
                }

                template <unsigned int f>
                operator polynomial<Q,f> (void) const
                {
                    polynomial<Q,f> r;

                    for (unsigned int i = 0; (i < f); i++)
                    {
                        r.coefficients[i] = ((i < degree) ? coefficients[i] : Q());
                    }

                    return r;
                }

                Q operator () (const Q &x) const
                {
                    Q r = coefficients[0] * x;

                    for (unsigned int i = 1; i < degree; i++)
                    {
                        r += coefficients[i] * (x^integer(i));
                    }

                    return r;
                }

                Q coefficients[degree];
        };
    };
};

#endif
