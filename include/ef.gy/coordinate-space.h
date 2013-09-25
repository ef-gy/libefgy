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

#if !defined(EF_GY_COORDINATE_SPACE_H)
#define EF_GY_COORDINATE_SPACE_H

#include <ef.gy/tuple.h>

namespace efgy
{
    namespace math
    {
        template <typename F, unsigned int n>
        class coordinateSpace
        {
            public:
                typedef F base;

                class vector;

                class scalar : public F
                {
                    public:
                        typedef typename F::integer integer;

                        scalar () : F() {}
                        scalar (const F &t) : F(t) {}
                        scalar (const integer &t) : F(t) {}

                        using F::operator *;
                        using F::operator ^;
                        using F::operator /;

                        using F::operator =;
                        using F::operator +=;
                        using F::operator -=;
                        using F::operator *=;
                        using F::operator ^=;
                        using F::operator /=;

/*
                        using F::operator >;
                        using F::operator >=;
                        using F::operator <;
                        using F::operator <=;
*/

                        vector operator * (const vector &v) const
                        {
                            scalar r[n];

                            for (unsigned int i = 0; i < n; i++)
                            {
                                r[i] = F(v.data[i]) * F(*this);
                            }

                            return vector(r);
                        }

                        vector operator / (const vector &v) const
                        {
                            scalar r[n];

                            for (unsigned int i = 0; i < n; i++)
                            {
                                r[i] = F(v.data[i]) / F(*this);
                            }

                            return vector(r);
                        }
                };

                class vector : public tuple<n, scalar>
                {
                    public:
                        vector () : tuple<n, scalar>() {}
                        vector (const scalar t[n]) : tuple<n, scalar>(t) {}
                        vector (const tuple<n, scalar> &t) : tuple<n, scalar>(t) {}

                        using tuple<n,scalar>::data;

                        using tuple<n,scalar>::operator =;

                        vector operator * (const scalar &s) const
                        {
                            return s * (*this);
                        }

                        scalar operator * (const vector &v) const
                        {
                            scalar s = scalar();

                            for (unsigned int i = 0; i < n; i++)
                            {
                                s = s + (data[i] * v.data[i]);
                            }

                            return s;
                        }

                        vector operator / (const scalar &s) const
                        {
                            return s / (*this);
                        }

                        scalar operator / (const vector &v) const
                        {
                            scalar s = scalar();

                            for (unsigned int i = 0; i < n; i++)
                            {
                                s = s + (data[i] / v.data[i]);
                            }

                            return s;
                        }

                        vector operator + (const vector &v) const
                        {
                            scalar r[n];

                            for (unsigned int i = 0; i < n; i++)
                            {
                                r[i] = (data[i] + v.data[i]);
                            }

                            return vector(r);
                        }

                        vector &operator += (const vector &v)
                        {
                            return (*this) = ((*this) + v);
                        }

                        vector operator - (const vector &v) const
                        {
                            scalar r[n];

                            for (unsigned int i = 0; i < n; i++)
                            {
                                r[i] = (data[i] - v.data[i]);
                            }

                            return vector(r);
                        }

                        vector &operator -= (const vector &v)
                        {
                            return (*this) = ((*this) - v);
                        }
                };
        };
    };
};

#endif
