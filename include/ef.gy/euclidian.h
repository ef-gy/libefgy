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

#if !defined(EF_GY_EUCLIDIAN_H)
#define EF_GY_EUCLIDIAN_H

#include <ef.gy/matrix.h>
#include <ef.gy/vector.h>
#include <array>

namespace efgy
{
    namespace geometry
    {
        namespace euclidian
        {
            template <typename F, unsigned int n>
            F lengthSquared
                (const math::vector<F,n> &pV)
            {
                F rv = F(F(0));

                for (int i = 0; i < n; i++)
                {
                    rv = rv + (pV[i] * pV[i]);
                }

                return rv;
            }

            template <typename F, unsigned int n>
            math::vector<F,n> normalise
                (const math::vector<F,n> &pV)
            {
                math::vector<F,n> rv;
                
                const F l = sqrt(lengthSquared<F,n>(pV));

                for (unsigned int i = 0; i < n; i++)
                {
                    rv[i] = pV[i] / l;
                }

                return rv;
            }

            template <typename Q, unsigned int n>
            Q dotProduct
                (const math::vector<Q,n> &a, const math::vector<Q,n> &b)
            {
                Q rv = Q(0);

                for (unsigned int i = 0; i < n; i++)
                {
                    rv += a[i] * b[i];
                }

                return rv;
            }

            template <typename F>
            math::vector<F,3> crossProduct
                (const math::vector<F,3> &a, const math::vector<F,3> &b)
            {
                math::vector<F,3> rv;

                rv[0] = a[1] * b[2] - a[2] * b[1]; 
                rv[1] = a[2] * b[0] - a[0] * b[2]; 
                rv[2] = a[0] * b[1] - a[1] * b[0]; 

                return rv;
            }

            template <typename Q>
            math::vector<Q,2> getPerpendicular
                (const math::vector<Q,2> &v)
            {
                math::vector<Q,2> r;
                
                r[0] = v[1] * Q(-1);
                r[1] = v[0];

                return r;
            }

            template <typename Q>
            math::vector<Q,3> getPerpendicular
                (const math::vector<Q,3> &v1, const math::vector<Q,3> &v2)
            {
                return crossProduct<Q>(v1, v2);
            }

            template <typename Q, unsigned int d>
            math::vector<Q,d> getNormal
                (const std::array<math::vector<Q,d>,d-1> &pV)
            {
                math::matrix<Q,d,d> pM;
                std::array<math::vector<Q,d>,d> baseVectors;

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

                math::vector<Q,d> rv = math::vector<Q,d>();

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
            math::vector<Q,3> getNormal
                (const std::array<math::vector<Q,3>,2> &pV)
            {
                return crossProduct<Q>(pV[0], pV[1]);
            }
        };
    };
};

#endif
