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

#if !defined(EF_GY_MATRIX_H)
#define EF_GY_MATRIX_H

namespace efgy
{
    namespace math
    {
        template <typename Q, unsigned int n, unsigned int m>
        class matrix
        {
            public:
                matrix ()
                    {}

                matrix (const matrix<Q,n,m> &b)
                    {
                        for (unsigned int i = 0; i < n; i++)
                        {
                            for (unsigned int j = 0; j < m; j++)
                            {
                                data[i][j] = b.data[i][j];
                            }
                        }
                    }

                matrix<Q,n,m> &operator = (const matrix<Q,n,m> &b)
                {
                    for (unsigned int i = 0; i < n; i++)
                    {
                        for (unsigned int j = 0; j < m; j++)
                        {
                            data[i][j] = b.data[i][j];
                        }
                    }

                    return *this;
                }

                matrix<Q,n,m> operator + (const matrix<Q,n,m> &b) const
                {
                    matrix<Q,m,n> r;

                    for (unsigned int i = 0; i < n; i++)
                    {
                        for (unsigned int j = 0; j < m; j++)
                        {
                            r.data[i][j] = data[i][j] + b.data[i][j];
                        }
                    }

                    return r;
                }

                matrix<Q,n,m> operator - (const matrix<Q,n,m> &b) const
                {
                    matrix<Q,n,m> r;

                    for (unsigned int i = 0; i < n; i++)
                    {
                        for (unsigned int j = 0; j < m; j++)
                        {
                            r.data[i][j] = data[i][j] - b.data[i][j];
                        }
                    }

                    return r;
                }

                template <unsigned int p>
                matrix<Q,n,p> operator * (const matrix<Q,m,p> &b) const
                {
                    matrix<Q,n,p> r;
                    
                    for (unsigned int i = 0; i < n; i++)
                    {
                        for (unsigned int j = 0; j < p; j++)
                        {
                            r.data[i][j] = 0;
                            
                            for (unsigned int k = 0; k < m; k++)
                            {
                                r.data[i][j] += data[i][k] * b.data[k][j];
                            }
                        }
                    }
                    
                    return r;
                }

                matrix<Q,n,m> operator + (const Q &b) const
                {
                    matrix<Q,n,m> r;
                    
                    for (unsigned int i = 0; i < n; i++)
                    {
                        for (unsigned int j = 0; j < m; j++)
                        {
                            r.data[i][j] = data[i][j] + b;
                        }
                    }
                    
                    return r;
                }

                matrix<Q,n,m> operator - (const Q &b) const
                {
                    matrix<Q,n,m> r;
                    
                    for (unsigned int i = 0; i < n; i++)
                    {
                        for (unsigned int j = 0; j < m; j++)
                        {
                            r.data[i][j] = data[i][j] - b;
                        }
                    }
                    
                    return r;
                }

                matrix<Q,n,m> operator * (const Q &b) const
                {
                    matrix<Q,n,m> r;
                    
                    for (unsigned int i = 0; i < n; i++)
                    {
                        for (unsigned int j = 0; j < m; j++)
                        {
                            r.data[i][j] = data[i][j] * b;
                        }
                    }
                    
                    return r;
                }

                matrix<Q,n,m> operator / (const Q &b) const
                {
                    matrix<Q,n,m> r;
                    
                    for (unsigned int i = 0; i < n; i++)
                    {
                        for (unsigned int j = 0; j < m; j++)
                        {
                            r.data[i][j] = data[i][j] / b;
                        }
                    }
                    
                    return r;
                }

                Q data[n][m];
        };

        template <typename Q, unsigned int d>
        Q determinant (const matrix<Q,d,d> &pM)
        {
            Q rv = Q(0);

            for (unsigned int i = 0; i < d; i++)
            {
                matrix<Q,d-1,d-1> pS;

                for (unsigned int j = 1, r = 0; j < d; j++, r++)
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
                    rv += pM.data[0][i] * determinant(pS);
                }
                else
                {
                    rv -= pM.data[0][i] * determinant(pS);
                }
            }

            return rv;
        }

        template <typename Q>
        Q determinant (const matrix<Q,2,2> &pM)
        {
            return pM.data[0][0] * pM.data[1][1] - pM.data[1][0] * pM.data[0][1];
        }

        template <typename Q>
        Q determinant (const matrix<Q,1,1> &pM)
        {
            return pM.data[0][0];
        }

        template <typename Q, unsigned int d>
        bool isIdentity (const matrix<Q,d,d> &pM)
        {
            for (unsigned int i = 0; i < d; i++)
            {
                for (unsigned int j = 0; j < d; j++)
                {
                    if (pM.data[i][j] != (i == j ? Q(1) : Q(0)))
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        template <typename Q, unsigned int n, unsigned int m>
        matrix<Q,m,n> transpose (const matrix<Q,n,m> &pM)
        {
            matrix<Q,m,n> rv;

            for (unsigned int i = 0; i < n; i++)
            {
                for (unsigned int j = 0; j < m; j++)
                {
                    rv.data[j][i] = pM.data[i][j];
                }
            }

            return rv;
        }

        template <typename Q>
        matrix<Q,3,3> invert (const matrix<Q,3,3> &pM)
        {
            matrix<Q,3,3> rv;

            const Q &a = pM.data[0][0];
            const Q &b = pM.data[0][1];
            const Q &c = pM.data[0][2];
            const Q &d = pM.data[1][0];
            const Q &e = pM.data[1][1];
            const Q &f = pM.data[1][2];
            const Q &g = pM.data[2][0];
            const Q &h = pM.data[2][1];
            const Q &i = pM.data[2][2];

            rv.data[0][0] =   e * i - f * h;
            rv.data[0][1] = -(d * i - f * g);
            rv.data[0][2] =   d * h - e * g;
            rv.data[1][0] = -(b * i - c * h);
            rv.data[1][1] =   a * i - c * g;
            rv.data[1][2] = -(a * h - b * g);
            rv.data[2][0] =   b * f - c * e;
            rv.data[2][1] = -(a * f - c * d);
            rv.data[2][2] =   a * e - b * d;

            return rv / determinant(pM);
        }
    };
};

#endif
