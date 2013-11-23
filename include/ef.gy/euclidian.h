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

#if !defined(EF_GY_EUCLIDIAN_H)
#define EF_GY_EUCLIDIAN_H

#include <ef.gy/coordinate-space.h>
#include <ef.gy/matrix.h>
#include <array>

namespace efgy
{
    namespace geometry
    {
        namespace euclidian
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
                            vector (const std::array<scalar, n> &t) : math::coordinateSpace<F,n>::vector(t) {}
                            explicit vector (const typename math::coordinateSpace<F,n>::vector &t) : math::coordinateSpace<F,n>::vector(t) {}

                            template<unsigned int nd>
                            typename space<F,nd>::vector expand (void) const
                            {
                                typename space<F,nd>::vector rv;
                                for (unsigned int i = 0; (i < n) && (i < nd); i++)
                                {
                                    rv[i] = (*this)[i];
                                }
                                for (unsigned int i = n; (i < nd); i++)
                                {
                                    rv[i] = typename space<F,nd>::scalar();
                                }
                                return rv;
                            }
                    };
            };

            template <typename F, unsigned int n>
            typename space<F,n>::scalar lengthSquared
                (const typename space<F,n>::vector &pV)
            {
                typename space<F,n>::scalar rv = typename space<F,n>::scalar(F(0));

                for (int i = 0; i < n; i++)
                {
                    rv = rv + (pV[i] * pV[i]);
                }

                return rv;
            }

            template <typename F, unsigned int n>
            typename space<F,n>::vector normalise
                (const typename space<F,n>::vector &pV)
            {
                typename space<F,n>::vector rv;
                
                const F l = sqrt(lengthSquared<F,n>(pV));

                for (unsigned int i = 0; i < n; i++)
                {
                    rv[i] = pV[i] / l;
                }

                return rv;
            }

            template <typename Q, unsigned int n>
            typename space <Q,n>::scalar dotProduct
                (const typename space<Q,n>::vector &a, const typename space<Q,n>::vector &b)
            {
                typename space <Q,n>::scalar rv = Q(0);

                for (unsigned int i = 0; i < n; i++)
                {
                    rv += a[i] * b[i];
                }

                return rv;
            }

            template <typename F>
            typename space<F,3>::vector crossProduct
                (const typename space<F,3>::vector &a, const typename space<F,3>::vector &b)
            {
                typename space<F,3>::vector rv;

                rv[0] = a[1] * b[2] - a[2] * b[1]; 
                rv[1] = a[2] * b[0] - a[0] * b[2]; 
                rv[2] = a[0] * b[1] - a[1] * b[0]; 

                return rv;
            }

            template <typename Q>
            typename space <Q,2>::vector getPerpendicular
                (const typename space<Q,2>::vector &v)
            {
                typename space<Q,2>::vector r;
                
                r[0] = v[1] * Q(-1);
                r[1] = v[0];

                return r;
            }

            template <typename Q>
            typename space <Q,3>::vector getPerpendicular
                (const typename space<Q,3>::vector &v1, const typename space<Q,3>::vector &v2)
            {
                return crossProduct<Q>(v1, v2);
            }

            template <typename Q, unsigned int d>
            typename space <Q,d>::vector getNormal
                (const typename std::array<typename space<Q,d>::vector,d-1> &pV)
            {
                math::matrix<Q,d,d> pM;
                typename std::array<typename space<Q,d>::vector,d> baseVectors;

                for (unsigned int i = 0; i < (d-1); i++)
                {
                    for (unsigned int j = 0; j < d; j++)
                    {
                        pM[i][j] = pV[i][j];
                        baseVectors[i][j] = ((i == j) ? 1 : 0);
                    }
                }

                for (unsigned int j = 0; j < d; j++)
                {
                    const unsigned int i = d-1;
                    baseVectors[i][j] = ((i == j) ? 1 : 0);
                }

                typename space<Q,d>::vector rv = typename space<Q,d>::vector();

                for (unsigned int i = 0; i < d; i++)
                {
                    math::matrix<Q,d-1,d-1> pS;
                    
                    for (unsigned int j = 0, r = 0; j < (d-1); j++, r++)
                    {
                        for (unsigned int k = 0, c = 0; k < d; k++)
                        {
                            if (k == i)
                            {
                                continue;
                            }

                            pS[r][c] = pM[j][k];

                            c++;
                        }
                    }
                    
                    if ((i % 2) == 0)
                    {
                        rv += baseVectors[i] * math::determinant(pS);
                    }
                    else
                    {
                        rv -= baseVectors[i] * math::determinant(pS);
                    }
                }

                return rv;
            }

            template <typename Q>
            typename space <Q,3>::vector getNormal
                (const typename std::array<typename space<Q,3>::vector,2> &pV)
            {
                return crossProduct<Q>(pV[0], pV[1]);
            }

#if defined(EF_GY_FRACTIONS_H)
            typedef space<math::Q,2> Q2;
            typedef space<math::Q,3> Q3;
#endif
        };
    };
};

#endif
