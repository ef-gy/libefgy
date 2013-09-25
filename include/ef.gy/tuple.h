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

#if !defined(EF_GY_TUPLE_H)
#define EF_GY_TUPLE_H

namespace efgy
{
    namespace math
    {
        template <unsigned int n, typename T>
        class tuple
        {
            public:
                tuple()
                {
                    for (unsigned int i = 0; i < n; i++)
                    {
                        data[i] = T();
                    }
                }

                tuple(const T pData[n])
                {
                    for (unsigned int i = 0; i < n; i++)
                    {
                        data[i] = pData[i];
                    }
                }

                tuple(const tuple &pT)
                {
                    for (unsigned int i = 0; i < n; i++)
                    {
                        data[i] = pT.data[i];
                    }
                }

                template<typename U>
                tuple(const tuple<n,U> &pT)
                {
                    for (unsigned int i = 0; i < n; i++)
                    {
                        data[i] = pT.data[i];
                    }
                }

                tuple &operator = (const tuple &b)
                {
                    for (unsigned int i = 0; i < n; i++)
                    {
                        data[i] = b.data[i];
                    }

                    return *this;
                }

                bool operator == (const tuple &b) const
                {
                    for (unsigned int i = 0; i < n; i++)
                    {
                        if (data[i] != b.data[i])
                        {
                            return false;
                        }
                    }

                    return true;
                }

                bool operator != (const tuple &b) const
                {
                    return !(*this == b);
                }

                T data[n];
        };
    };
};

#endif
