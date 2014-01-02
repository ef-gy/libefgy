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
 * \see Project Source Code: http://git.becquerel.org/jyujin/libefgy.git
 */

#if !defined(EF_GY_COORDINATE_SPACE_H)
#define EF_GY_COORDINATE_SPACE_H

#include <ef.gy/traits.h>
#include <array>

namespace efgy
{
    namespace math
    {
        template <typename F, unsigned int n>
        class coordinateSpace
        {
            public:
                typedef F base;
                typedef F scalar;

                class vector : public std::array<scalar, n>
                {
                    public:
                        constexpr vector () : std::array<scalar, n>() {}
                        vector (const scalar t[n]) : std::array<scalar, n>() { std::copy(t, t+n, std::array<scalar, n>::begin()); }
                        constexpr vector (const std::array<scalar, n> &t) : std::array<scalar, n>(t) {}

                        using std::array<scalar, n>::operator =;

                        vector operator * (const scalar &s) const
                        {
                            vector r = *this;

                            for (unsigned int i = 0; i < n; i++)
                            {
                                r[i] *= s;
                            }

                            return r;
                        }

                        scalar operator * (const vector &v) const
                        {
                            scalar s = scalar();

                            for (unsigned int i = 0; i < n; i++)
                            {
                                s = s + ((*this)[i] * v[i]);
                            }

                            return s;
                        }

                        vector operator / (const scalar &s) const
                        {
                            vector r = *this;

                            for (unsigned int i = 0; i < n; i++)
                            {
                                r[i] /= s;
                            }

                            return r;
                        }

                        scalar operator / (const vector &v) const
                        {
                            scalar s = scalar();

                            for (unsigned int i = 0; i < n; i++)
                            {
                                s = s + ((*this)[i] / v[i]);
                            }

                            return s;
                        }

                        vector operator + (const vector &v) const
                        {
                            scalar r[n];

                            for (unsigned int i = 0; i < n; i++)
                            {
                                r[i] = ((*this)[i] + v[i]);
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
                                r[i] = ((*this)[i] - v[i]);
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
