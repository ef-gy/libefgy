/**\file
 * \brief Matrix handling
 *
 * Defines a template to hold matrices with cells of arbitrary data types and
 * functions to calculate with matrices of different sizes. The sizes are set at
 * compile time.
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

#if !defined(EF_GY_MATRIX_H)
#define EF_GY_MATRIX_H

#include <array>

namespace efgy
{
    namespace math
    {
        /**\brief Matrix with fixed size.
         *
         * This template is used to store and calculate with matrices of
         * arbitrary but fixed and finite sizes.
         *
         * \tparam Q The data type for individual matrix cells.
         * \tparam n Number of rows in the matrix.
         * \tparam m Number of columns in the matrix.
         */
        template <typename Q, unsigned int n, unsigned int m>
        class matrix : public std::array<std::array<Q,m>,n>
        {
            public:
                /**\brief Default constructor
                 *
                 * This is the default constructor that does not explicitly
                 * initialise the matrix to anything.
                 */
                matrix (void) {}

                /**\brief Copy constructor
                 *
                 * Copies the given matrix's elements to the new instance. The
                 * source matrix may have different dimensions than the newly
                 * created matrix; if it does, then any excess elements are
                 * discarded and any new elements are initialised with their
                 * default constructor.
                 *
                 * Example: you have a 2x2 matrix and create a new 1x3 matrix
                 * from that. This will lead to the fields (0,0) and (0,1) to
                 * be coped to the target matrix; (0,2) will be initialised
                 * with its default constructor and (1,0) and (1,1) in the
                 * source matrix will be ignored.
                 *
                 * \tparam rn Number of rows in the matrix to copy.
                 * \tparam rm Number of columns in the matrix to copy.
                 *
                 * \param[in] b The matrix to copy.
                 */
                template<unsigned int rn, unsigned int rm>
                matrix (const matrix<Q,rn,rm> &b)
                    {
                        for (unsigned int i = 0; i < n; i++)
                        {
                            for (unsigned int j = 0; j < m; j++)
                            {
                                if ((i < rn) && (j < rm))
                                {
                                    (*this)[i][j] = b[i][j];
                                }
                                else
                                {
                                    (*this)[i][j] = Q();
                                }
                            }
                        }
                    }

                matrix<Q,n,m> &operator = (const matrix<Q,n,m> &b)
                {
                    for (unsigned int i = 0; i < n; i++)
                    {
                        for (unsigned int j = 0; j < m; j++)
                        {
                            (*this)[i][j] = b[i][j];
                        }
                    }

                    return *this;
                }

                matrix<Q,n,m> operator + (const matrix<Q,n,m> &b) const
                {
                    matrix<Q,n,m> r;

                    for (unsigned int i = 0; i < n; i++)
                    {
                        for (unsigned int j = 0; j < m; j++)
                        {
                            r[i][j] = (*this)[i][j] + b[i][j];
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
                            r[i][j] = (*this)[i][j] - b[i][j];
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
                            r[i][j] = 0;
                            
                            for (unsigned int k = 0; k < m; k++)
                            {
                                r[i][j] += (*this)[i][k] * b[k][j];
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
                            r[i][j] = (*this)[i][j] + b;
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
                            r[i][j] = (*this)[i][j] - b;
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
                            r[i][j] = (*this)[i][j] * b;
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
                            r[i][j] = (*this)[i][j] / b;
                        }
                    }
                    
                    return r;
                }
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

                        pS[r][c] = pM[j][k];

                        c++;
                    }
                }

                if ((i % 2) == 0)
                {
                    rv += pM[0][i] * determinant(pS);
                }
                else
                {
                    rv -= pM[0][i] * determinant(pS);
                }
            }

            return rv;
        }

        template <typename Q>
        Q determinant (const matrix<Q,2,2> &pM)
        {
            return pM[0][0] * pM[1][1] - pM[1][0] * pM[0][1];
        }

        template <typename Q>
        Q determinant (const matrix<Q,1,1> &pM)
        {
            return pM[0][0];
        }

        template <typename Q, unsigned int d>
        bool isIdentity (const matrix<Q,d,d> &pM)
        {
            for (unsigned int i = 0; i < d; i++)
            {
                for (unsigned int j = 0; j < d; j++)
                {
                    if (pM[i][j] != (i == j ? Q(1) : Q(0)))
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
                    rv[j][i] = pM[i][j];
                }
            }

            return rv;
        }

        template <typename Q>
        matrix<Q,3,3> invert (const matrix<Q,3,3> &pM)
        {
            matrix<Q,3,3> rv;

            const Q &a = pM[0][0];
            const Q &b = pM[0][1];
            const Q &c = pM[0][2];
            const Q &d = pM[1][0];
            const Q &e = pM[1][1];
            const Q &f = pM[1][2];
            const Q &g = pM[2][0];
            const Q &h = pM[2][1];
            const Q &i = pM[2][2];

            rv[0][0] =   e * i - f * h;
            rv[0][1] = -(d * i - f * g);
            rv[0][2] =   d * h - e * g;
            rv[1][0] = -(b * i - c * h);
            rv[1][1] =   a * i - c * g;
            rv[1][2] = -(a * h - b * g);
            rv[2][0] =   b * f - c * e;
            rv[2][1] = -(a * f - c * d);
            rv[2][2] =   a * e - b * d;

            return rv / determinant(pM);
        }
    };
};

#endif
