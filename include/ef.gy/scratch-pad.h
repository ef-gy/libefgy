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

#if !defined(EF_GY_SCRATCH_PAD_H)
#define EF_GY_SCRATCH_PAD_H

namespace efgy
{
    namespace data
    {
        template <typename T>
        class referenceCounted
        {
            public:
                class content
                {
                    public:
                        content (const T &pA)
                            : references(1), data(pA)
                            {}

                        T data;
                        unsigned long references;
                };

                referenceCounted (const T &pA)
                    : data(new content(pA)), copyOnWrite(false)
                    {}

                referenceCounted (const referenceCounted &pA)
                    : data(pA.data), copyOnWrite(true)
                    {
                        data->references++;
                    }

                ~referenceCounted (void)
                    {
                        data->references--;

                        if (data->references == 0)
                        {
                            delete data;
                        }
                    }

                referenceCounted &operator = (const referenceCounted &b)
                {
                    if (&b != this)
                    {
                        data->references--;

                        if (data->references == 0)
                        {
                            delete data;
                        }

                        data = b.data;

                        data->references++;

                        copyOnWrite = true;
                    }

                    return *this;
                }

                operator const T& (void) const
                {
                    return data->data;
                }

                operator T& (void)
                {
                    if (copyOnWrite)
                    {
                        if (data->references > 1)
                        {
                            data->references--;
                            data = new content (data->data);
                        }
                        copyOnWrite = false;
                    }
                    return data->data;
                }

            protected:
                content *data;
                bool copyOnWrite;
        };

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
        template <typename T, unsigned int objectBlockSize = 0x10, unsigned int dataAlignment = 0x4>
        class referenceCountedArray
        {
            public:
                class content
                {
                    public:
                        content (unsigned long pCount)
                            : references(1), count(pCount), data(new T[pCount])
                            {}

                        content (const content &pV)
                            : references(1), count(pV.count), data(new T[pV.count])
                            {
                                for (unsigned long i = 0; i < pV.count; i++)
                                {
                                    data[i] = pV.data[i];
                                }
                            }

                        ~content (void)
                            {
                                delete[] data;
                            }

                        content &operator = (const content &pV)
                        {
                            if (this != &pV)
                            {
                                references = 1;
                                count = pV.count;
                                delete[] data;
                                data = new T[count];

                                for (unsigned long i = 0; i < pV.count; i++)
                                {
                                    data[i] = pV.data[i];
                                }
                            }

                            return *this;
                        }

                        T *data;
                        unsigned long references;
                        unsigned long count;
                };

                referenceCountedArray (unsigned long count)
                    : data((count <= internalDataSize) ? 0 : new content(count - internalDataSize)), copyOnWrite(false)
                    {}

                referenceCountedArray (const referenceCountedArray &pA)
                    : data(pA.data), copyOnWrite(true)
                    {
                        for (unsigned long i = 0; i < internalDataSize; i++)
                        {
                            internalData[i] = pA.internalData[i];
                        }

                        if (data)
                        {
                            data->references++;
                        }
                    }

                ~referenceCountedArray (void)
                    {
                        if (data)
                        {
                            data->references--;

                            if (data->references == 0)
                            {
                                delete data;
                            }
                        }
                    }

                referenceCountedArray &operator = (const referenceCountedArray &b)
                {
                    if (&b != this)
                    {
                        for (unsigned long i = 0; i < internalDataSize; i++)
                        {
                            internalData[i] = b.internalData[i];
                        }

                        if (data)
                        {
                            data->references--;

                            if (data->references == 0)
                            {
                                delete data;
                            }
                        }

                        data = b.data;

                        if (data)
                        {
                            data->references++;
                        }

                        copyOnWrite = true;
                    }

                    return *this;
                }

                const T& operator [] (unsigned long i) const
                {
                    if (i < internalDataSize)
                    {
                        return internalData[i];
                    }

                    return data->data[(i - internalDataSize)];
                }

                T& operator [] (unsigned long i)
                {
                    if (i < internalDataSize)
                    {
                        return internalData[i];
                    }

                    if (copyOnWrite)
                    {
                        if (data->references > 1)
                        {
                            data->references--;
                            data = new content (*data);
                        }
                        copyOnWrite = false;
                    }
                    return data->data[(i - internalDataSize)];
                }

            protected:
                content *data;
                bool copyOnWrite;

                static const unsigned long spaceLeft = objectBlockSize - sizeof (content *) - sizeof (bool);
                static const unsigned long alignedSpaceLeft = (spaceLeft / dataAlignment) * dataAlignment;
            public:
                static const unsigned long internalDataSize = alignedSpaceLeft / sizeof(T);
            protected:
                T internalData [internalDataSize];
        };
#pragma clang diagnostic pop

        template <typename T, typename cellType = unsigned long, cellType cellBlockSize = 0xf, cellType objectBlockSize = 0x10, cellType dataAlignment = 0x4>
        class scratchPad
        {
            protected:
                static const unsigned long internalDataSize = referenceCountedArray<T,objectBlockSize,dataAlignment>::internalDataSize;
            public:
                scratchPad (void)
                    : cellCount(0),
                      realCellCount(internalDataSize),
                      data(internalDataSize)
                    {}

                scratchPad (const cellType &pCellCount)
                    : cellCount(0),
                      realCellCount((pCellCount <= internalDataSize) ? internalDataSize : ((1 + pCellCount / cellBlockSize) * cellBlockSize)),
                      data((pCellCount <= internalDataSize) ? internalDataSize : ((1 + pCellCount / cellBlockSize) * cellBlockSize))
                    {
                        resize(pCellCount);
                    }

                scratchPad (const scratchPad &p)
                    : cellCount(p.cellCount),
                      realCellCount(p.realCellCount),
                      data(p.data)
                    {}

                scratchPad &operator = (const scratchPad &p)
                {
                    if (this != &p)
                    {
                        cellCount     = p.cellCount;
                        realCellCount = p.realCellCount;
                        data          = p.data;
                    }

                    return *this;
                }

                scratchPad &resize (cellType pCellCount)
                {
                    if (pCellCount == cellCount)
                    {
                    }
                    else if (pCellCount <= realCellCount)
                    {
                        if (pCellCount > cellCount)
                        {
                            for (cellType i = cellCount; i < pCellCount; i++)
                            {
                                data[i] = T();
                            }
                        }

                        cellCount = pCellCount;
                    }
                    else
                    {
                        realCellCount = (1 + pCellCount / cellBlockSize) * cellBlockSize;

                        const referenceCountedArray<T,objectBlockSize,dataAlignment> original = data;
                        data = referenceCountedArray<T,objectBlockSize,dataAlignment>(realCellCount);
                        for (cellType i = 0; i < cellCount; i++)
                        {
                            data[i] = original[i];
                        }

                        return resize(pCellCount);
                    }

                    return *this;
                }

                const T &operator[] (const cellType &b) const
                {
                    return data[b];
                }

                T &operator[] (const cellType &b)
                {
                    if (b > cellCount)
                    {
                        resize (b);
                    }

                    return data[b];
                }

            protected:
                referenceCountedArray<T,objectBlockSize,dataAlignment> data;
                cellType cellCount;
                cellType realCellCount;
        };
    };
};

#endif
