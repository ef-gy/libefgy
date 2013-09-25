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

#if !defined(EF_GY_BUFFER_H)
#define EF_GY_BUFFER_H

#include <ef.gy/scratch-pad.h>

namespace efgy
{
    namespace data
    {
        template<typename T, typename cellType = unsigned long>
        class buffer : public scratchPad<T,cellType>
        {
            protected:
                using scratchPad<T,cellType>::cellCount;
                cellType &length;

            public:
                using scratchPad<T,cellType>::operator [];
                using scratchPad<T,cellType>::resize;

                buffer(void)
                    : length(cellCount), scratchPad<T,cellType>()
                    {}

                buffer(const buffer &pB)
                    : length(cellCount), scratchPad<T,cellType>(pB)
                    {}

                buffer &operator = (const buffer &b)
                {
                    scratchPad<T,cellType>::operator=(b);
                    return *this;
                }

                buffer &operator += (const buffer &b)
                {
                    cellType pl = length;

                    resize(length + b.length);

                    for (unsigned long i = 0; i < b.length; i++)
                    {
                        (*this)[(pl + i)] = b[i];
                    }

                    return *this;
                }

                buffer operator + (const buffer &b) const
                {
                    buffer r = *this;

                    r.resize(length + b.length);

                    for (unsigned long i = 0; i < b.length; i++)
                    {
                        r[(length + i)] = b[i];
                    }

                    return r;
                }
        };
    };
};

#endif
