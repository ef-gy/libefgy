/*
 * This file is part of the ef.gy project.
 * See the appropriate repository at http://ef.gy/.git for exact file
 * modification records.
*/

/*
 * Copyright (c) 2012, ef.gy Project Members
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

#if !defined(EF_GY_SET_H)
#define EF_GY_SET_H

#include <ef.gy/scratch-pad.h>

namespace efgy
{
    namespace math
    {
        template <typename T>
        class set
        {
            public:
                set()
                    : data(), count(0)
                    {}

                set(const T *pData, unsigned int pCount)
                    : data(pCount), count(pCount)
                    {
                        for (unsigned int i = 0; i < pCount; i++)
                        {
                            data[i] = pData[i];
                        }
                    }

                set(const set &pT)
                    : data(pT.data), count(pT.count)
                    {}

                template<typename U>
                set(const set<U> &pT)
                    : data(pT.count), count(pT.count)
                    {
                        for (unsigned int i = 0; i < count; i++)
                        {
                            data[i] = T(pT.data[i]);
                        }
                    }

                set &operator = (const set &b)
                {
                    if (this != &b)
                    {
                        count = b.count;
                        data  = b.data;
                    }

                    return *this;
                }

                set operator + (const T &b) const
                {
                    if (count > 0)
                    {
                        unsigned int i = 0;

                        for (; i < count; i++)
                        {
                            if (data[i] == b)
                            {
                                return *this;
                            }
                        }

                        set rv = *this;

                        rv.count = count + 1;
                        rv.data.resize (rv.count);

                        rv.data[count] = b;

                        return rv;
                    }

                    return set(&b, 1);
                }

                set operator - (const T &b) const
                {
                    if (count > 0)
                    {
                        unsigned int i = 0, c = 0;

                        for (; i < count; i++)
                        {
                            if (data[i] == b)
                            {
                                c++;
                            }
                        }

                        if (c > 0)
                        {
                            set rv = set();

                            rv.count = count - c;

                            if (rv.count == 0)
                            {
                                return rv;
                            }

                            rv.data = data::scratchPad<T>(rv.count);

                            for (c = 0, i = 0; i < count; i++)
                            {
                                if (data[i] != b)
                                {
                                    rv.data[c] = data[i];
                                    c++;
                                }
                            }

                            return rv;
                        }
                    }

                    return set(*this);
                }

                bool operator == (const set &b) const
                {
                    if (count != b.count)
                    {
                        return false;
                    }

                    for (unsigned int i = 0; i < count; i++)
                    {
                        if (data[i] != b.data[i])
                        {
                            return false;
                        }
                    }

                    return true;
                }

                data::scratchPad<T> data;
                unsigned int count;
        };
    };
};

#endif
