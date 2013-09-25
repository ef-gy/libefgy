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

#if !defined(EF_GY_EUCLIDIAN_H)
#define EF_GY_EUCLIDIAN_H

#include <ef.gy/coordinate-space.h>
#include <ef.gy/matrix.h>

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
                            vector (const math::tuple<n, scalar> &t) : math::coordinateSpace<F,n>::vector(t) {}
                            explicit vector (const typename math::coordinateSpace<F,n>::vector &t) : math::coordinateSpace<F,n>::vector(t) {}

                            using math::coordinateSpace<F,n>::vector::data;
                    };
            };

            template <typename F, unsigned int n>
            typename space<F,n>::scalar lengthSquared
                (const typename space<F,n>::vector &pV)
            {
                typename space<F,n>::scalar rv = typename space<F,n>::scalar(F(0));

                for (int i = 0; i < n; i++)
                {
                    rv = rv + (pV.data[i] * pV.data[i]);
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
                    rv.data[i] = pV.data[i] / l;
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
                    rv += a.data[i] * b.data[i];
                }

                return rv;
            }

            template <typename F>
            typename space<F,3>::vector crossProduct
                (const typename space<F,3>::vector &a, const typename space<F,3>::vector &b)
            {
                typename space<F,3>::vector rv;

                rv.data[0] = a.data[1] * b.data[2] - a.data[2] * b.data[1]; 
                rv.data[1] = a.data[2] * b.data[0] - a.data[0] * b.data[2]; 
                rv.data[2] = a.data[0] * b.data[1] - a.data[1] * b.data[0]; 

                return rv;
            }

            template <typename Q>
            typename space <Q,2>::vector getPerpendicular
                (const typename space<Q,2>::vector &v)
            {
                typename space<Q,2>::vector r;
                
                r.data[0] = v.data[1] * Q(-1);
                r.data[1] = v.data[0];

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
                (const typename math::tuple<d-1,typename space<Q,d>::vector> &pV)
            {
                math::matrix<Q,d,d> pM;
                typename math::tuple<d,typename space<Q,d>::vector> baseVectors;

                for (unsigned int i = 0; i < (d-1); i++)
                {
                    for (unsigned int j = 0; j < d; j++)
                    {
                        pM.data[i][j] = pV.data[i].data[j];
                        baseVectors.data[i].data[j] = ((i == j) ? 1 : 0);
                    }
                }

                for (unsigned int j = 0; j < d; j++)
                {
                    const unsigned int i = d-1;
                    baseVectors.data[i].data[j] = ((i == j) ? 1 : 0);
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
                            
                            pS.data[r][c] = pM.data[j][k];
                            
                            c++;
                        }
                    }
                    
                    if ((i % 2) == 0)
                    {
                        rv += baseVectors.data[i] * math::determinant(pS);
                    }
                    else
                    {
                        rv -= baseVectors.data[i] * math::determinant(pS);
                    }
                }

                return rv;

                /*
                if ((d % 2) == 0)
                {
                    return rv * Q(-1);
                }
                else
                {
                    return rv;
                }
                 */
            }

            template <typename Q>
            typename space <Q,3>::vector getNormal
                (const typename math::tuple<2,typename space<Q,3>::vector> &pV)
            {
                return crossProduct<Q>(pV.data[0], pV.data[1]);
            }

            /*
             typedef space<math::Q,2> Q2;
             typedef space<math::Q,3> Q3;
             */
        };
    };
};

#endif
