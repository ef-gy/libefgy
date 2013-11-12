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

#if !defined(EF_GY_POLAR_H)
#define EF_GY_POLAR_H

#include <ef.gy/euclidian.h>

namespace efgy
{
    namespace geometry
    {
        namespace polar
        {
            template <typename F, unsigned int n>
            class space : public math::coordinateSpace<F,n>
            {
                public:
                    typedef typename math::coordinateSpace<F,n>::base base;
                    typedef typename math::coordinateSpace<F,n>::scalar scalar;

                    class vector : public math::coordinateSpace<F,n>::vector
                    {
                        public:
                            vector () : math::coordinateSpace<F,n>::vector() {}
                            vector (const scalar t[n]) : math::coordinateSpace<F,n>::vector(t) {}
                            vector (const math::tuple<n, scalar> &t) : math::coordinateSpace<F,n>::vector(t) {}
                            explicit vector (const typename math::coordinateSpace<F,n>::vector &t) : math::coordinateSpace<F,n>::vector(t) {}

                            using math::coordinateSpace<F,n>::vector::data;

                            operator typename euclidian::space<F,n>::vector () const
                            {
                                typename euclidian::space<F,n>::vector v;

                                for (unsigned int i = 0; i < n; i++)
                                {
                                    v.data[i] = data[0];
                                }

                                for (unsigned int i = 0; i < (n-1); i++)
                                {
                                    const unsigned int p = i + 1;

                                    v.data[i] *= cos(data[p]);

                                    for (unsigned int j = p; j < n; j++)
                                    {
                                        v.data[j] *= sin(data[p]);                                            
                                    }
                                }

                                return v;
                            }
                    };
            };
        };
    };
};

#endif
