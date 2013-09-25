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

#if !defined(EF_GY_BIG_INTEGERS_H)
#define EF_GY_BIG_INTEGERS_H

#include <ef.gy/numeric.h>
#include <ef.gy/scratch-pad.h>
#include <ef.gy/string.h>

namespace efgy
{
    namespace math
    {
        namespace numeric
        {
            template <typename N>
            class fractional;

            template <typename N>
            class factorial;

            // template <typename Ts = signed long long, typename Tu = unsigned long long, typename cellType = unsigned short, unsigned int cellBitCount = 16>
            template <typename Ts = signed long long, typename Tu = unsigned long long, typename cellType = unsigned int, unsigned int cellBitCount = 32>
            class bigIntegers : public numeric
            {
                public:
                    typedef Ts integer;

                    bigIntegers ()
                        : cellCount(cellType(0)),
                          cell(0), negative (false)
                        {}
                    bigIntegers (Ts pInteger)
                        : negative (pInteger < Ts(0)),
                          cellCount(cellType(0)),
                          cell(0)
                        {
                            if (pInteger == Ts(0))
                            {
                                negative = false;
                            }
                            else
                            {
                                cellType v[cellsPerLong];
                                Tu q = ((pInteger < Ts(0)) ? -pInteger : pInteger);
                                cellType s = 0;

                                for (cellType i = 0; (q != 0) && (i < cellsPerLong); i++)
                                {
                                    v[i] = cellType(q & lowMask);
                                    q >>= cellBitCount;
                                    if (v[i] != 0)
                                    {
                                        s = i + 1;
                                    }
                                }

                                cellCount = s;
                                cell.resize(s);

                                for (cellType i = 0; i < s; i++)
                                {
                                    cell[i] = v[i];
                                }
                            }
                        }
                    bigIntegers (Tu pInteger, bool pNegative)
                        : cellCount(cellType(0)),
                          cell(0),
                          negative ((pInteger == 0) ? false : pNegative)
                        {
                            if (pInteger == Tu(0))
                            {
                                negative = false;
                            }
                            else
                            {
                                cellType v[cellsPerLong];
                                Tu q = pInteger;

                                cellType s = 0;

                                for (cellType i = 0; (q != 0) && (i < cellsPerLong); i++)
                                {
                                    v[i] = cellType(q & lowMask);
                                    q >>= cellBitCount;
                                    if (v[i] != 0)
                                    {
                                        s = i + 1;
                                    }
                                }

                                cellCount = s;
                                cell.resize(s);

                                for (cellType i = 0; i < s; i++)
                                {
                                    cell[i] = v[i];
                                }
                            }
                        }
                    bigIntegers (const bigIntegers &pB)
                        : cellCount(pB.cellCount),
                          cell(pB.cellCount),
                          negative (pB.negative)
                        {
                            if (cellCount > 0)
                            {
                                for (cellType i = 0; i < cellCount; i++)
                                {
                                    cell[i] = pB.cell[i];
                                }
                            }
                            else
                            {
                                negative = false;
                            }
                        }

                    bigIntegers &operator = (const bigIntegers &b)
                    {
                        if (this != &b)
                        {
                            negative  = b.negative;
                            cellCount = b.cellCount;
                            cell      = b.cell;
                        }

                        return *this;
                    }

                    bigIntegers operator + (const bigIntegers &b) const
                    {
                        if (*this == zero())
                        {
                            return b;
                        }
                        else if (b == zero())
                        {
                            return *this;
                        }

                        bigIntegers r;

                        if (negative == b.negative)
                        {
                            r.negative = negative;

                            r.doAdd (*this, b);
                        }
                        else
                        {
                            const bigIntegers &p =   negative ? -*this : *this;
                            const bigIntegers &q = b.negative ? -b     : b;

                            if (p > q)
                            {
                                r.doSubtract (p, q);
                                r.negative = negative && (r.cellCount > 0);
                            }
                            else
                            {
                                r.doSubtract (q, p);
                                r.negative = !negative && (r.cellCount > 0);
                            }
                        }

                        return r;
                    }

                    bigIntegers &operator += (const bigIntegers &b)
                    {
                        return ((*this) = (*this + b));
                    }
                    
                    bigIntegers &operator ++ (void)
                    {
                        return ((*this) = (*this + one()));
                    }

                    bigIntegers operator ++ (int)
                    {
                        bigIntegers r = (*this);

                        (*this) += one();

                        return r;
                    }

                    bigIntegers operator - (const bigIntegers &b) const
                    {
                        return *this + -b;
                    }

                    bigIntegers &operator -= (const bigIntegers &b)
                    {
                        return ((*this) = (*this - b));
                    }

                    bigIntegers &operator -- (void)
                    {
                        return ((*this) = (*this - one()));
                    }

                    bigIntegers operator -- (int)
                    {
                        bigIntegers r = (*this);

                        (*this) -= one();

                        return r;
                    }

                    bigIntegers operator - (void) const
                    {
                        bigIntegers r = *this;

                        r.negative = !r.negative;

                        return r;
                    }

                    bigIntegers operator * (const bigIntegers &b) const
                    {
                        if (*this == zero())
                        {
                            return zero();
                        }
                        else if (b == zero())
                        {
                            return zero();
                        }
                        else if (*this == one())
                        {
                            return b;
                        }
                        else if (b == one())
                        {
                            return *this;
                        }
                        else if (*this == negativeOne())
                        {
                            return -b;
                        }
                        else if (b == negativeOne())
                        {
                            return -*this;
                        }

                        bigIntegers r;
                        bigIntegers a = *this;
                        bigIntegers c = b;

                        a.negative = false;
                        c.negative = false;

                        r.doMultiply (a, c);
                        r.negative = (r.cellCount > 0) && (negative != b.negative);

                        return r;
                    }

                    fractional<bigIntegers> operator * (const fractional<bigIntegers> &b) const
                    {
                        return b * (*this);
                    }

                    bigIntegers &operator *= (const bigIntegers &b)
                    {
                        return ((*this) = (*this * b));
                    }

                    bigIntegers operator % (const bigIntegers &b) const
                    {
                        if (*this == zero())
                        {
                            return zero();
                        }
                        else if (b == zero())
                        {
                            return zero();
                        }
                        else if (b == one())
                        {
                            return zero();
                        }

                        if ((cellCount <= cellsPerLong) && (b.cellCount <= cellsPerLong))
                        {
                            return bigIntegers((*this).toInteger() % (b).toInteger(), negative);
                        }

                        bigIntegers r;
                        const bigIntegers &ap =   negative ? -*this : *this;
                        const bigIntegers &bp = b.negative ? -b     : b;

                        if (b.cellCount == 1)
                        {
                            r.doModuloHorner(ap,b.cell[0]);
                        }
/*
                        else if (b.cellCount < (cellsPerLong - 1))
                        {
                            r.doModuloHorner(ap, bp);
                        }
*/
                        else
                        {
                            r.doModulo (ap, bp);
                        }

                        r.negative = negative;

                        return r;
                    }

                    bigIntegers &operator %= (const bigIntegers &b)
                    {
                        return ((*this) = (*this % b));
                    }

                    fractional<bigIntegers> operator / (const bigIntegers &b) const
                    {
                        return fractional<bigIntegers> (*this, b);
                    }

                    fractional<bigIntegers> operator / (const fractional<bigIntegers> &b) const
                    {
                        return fractional<bigIntegers> (*this) / b;
                    }

                    bigIntegers &operator /= (const bigIntegers &b)
                    {
                        if (*this == zero())
                        {
                            return (*this = zero());
                        }
                        else if (b == zero())
                        {
                            return (*this = zero());
                        }
                        else if (b ==one())
                        {
                            return *this;
                        }
                        else if (b == negativeOne())
                        {
                            return (*this = -*this);
                        }

                        if ((cellCount <= cellsPerLong) && (b.cellCount <= cellsPerLong))
                        {
                            *this = bigIntegers((*this).toInteger() / (b).toInteger(), (negative != b.negative));
                            return *this;
                        }

                        bool rnegative = (negative != b.negative);
                        const bigIntegers &aa =   negative ? -*this : *this;
                        const bigIntegers &ba = b.negative ? -b     : b;
                        doDivide (aa, ba);
                        negative = rnegative && (cellCount > 0);
                        return *this;
                    }

                    bool operator > (const bigIntegers &b) const
                    {
                        if (!negative && b.negative)
                        {
                            return true;
                        }

                        if (!negative && !b.negative)
                        {
                            if  (cellCount > b.cellCount)
                            {
                                return true;
                            }

                            if  (cellCount < b.cellCount)
                            {
                                return false;
                            }

                            for (cellType j = cellCount; j > 0; j--)
                            {
                                const cellType &i = (j-1);

                                if (cell[i] > b.cell[i])
                                {
                                    return true;
                                }
                                if (cell[i] < b.cell[i])
                                {
                                    return false;
                                }
                            }

                            return false;
                        }

                        if (negative && b.negative)
                        {
                            if  (cellCount < b.cellCount)
                            {
                                return true;
                            }

                            if  (cellCount > b.cellCount)
                            {
                                return false;
                            }

                            for (cellType j = cellCount; j > 0; j--)
                            {
                                const cellType &i = (j-1);

                                if (cell[i] < b.cell[i])
                                {
                                    return true;
                                }
                                if (cell[i] > b.cell[i])
                                {
                                    return false;
                                }
                            }

                            return false;
                        }

                        return false;
                    }


                    bool operator > (const zero &b) const
                    {
                        return !negative
                            && (cellCount > 0);
                    }

                    bool operator > (const one &b) const
                    {
                        return !negative
                            && (cellCount >= 1)
                            && (*this > bigIntegers(1));
                    }

                    bool operator > (const negativeOne &b) const
                    {
                        return (*this == zero())
                            || !negative;
                    }

                    bool operator == (const bigIntegers &b) const
                    {
                        if (cellCount != b.cellCount)
                        {
                            return false;
                        }

                        if (negative != b.negative)
                        {
                            return false;
                        }

                        for (cellType i = 0; i < cellCount; i++)
                        {
                            if (cell[i] != b.cell[i])
                            {
                                return false;
                            }
                        }

                        return true;
                    }

                    bool operator == (const zero &b) const
                    {
                        return cellCount == 0;
                    }

                    bool operator == (const one &b) const
                    {
                        return !negative
                            && (cellCount == 1)
                            && (cell[0] == cellType(1));
                    }

                    bool operator == (const negativeOne &b) const
                    {
                        return negative
                            && (cellCount == 1)
                            && (cell[0] == cellType(1));
                    }

                    operator std::string (void) const
                    {
                        return data::intToString((*this));
                    }

                    Tu toInteger (void) const
                    {
                        Tu r = 0;

                        if (cellCount == 0)
                        {
                            return 0;
                        }

                        for (cellType i = 0; i < cellCount; i++)
                        {
                            r |= Tu(cell[i]) << (i * cellBitCount);
                        }

                        return r;
                    }

                    Ts toSignedInteger (void) const
                    {
                        Tu r = 0;

                        if (cellCount == 0)
                        {
                            return 0;
                        }

                        for (cellType i = 0; i < cellCount; i++)
                        {
                            r |= Tu(cell[i]) << (i * cellBitCount);
                        }

                        return negative ? -r : r;
                    }

                    long double toDouble (void) const
                    {
                        long double r = 0;
                        long double m = Tu(1) << cellBitCount;
                        long double mr = 1;

                        if (cellCount == 0)
                        {
                            return 0;
                        }

                        for (cellType i = 0; i < cellCount; i++)
                        {
                            r += (long double)(cell[i]) * mr;
                            mr *= m;
                        }

                        return negative ? -r : r;
                    }

                    bigIntegers operator >> (const cellType &b) const
                    {
                        if (cellCount == 0)
                        {
                            return *this;
                        }

                        if (cellCount <= cellsPerLong)
                        {
                            return bigIntegers(Tu(*this) >> b, false);
                        }


                        cellType nukeCells = 0;
                        cellType q = b;

                        while (q > cellBitCount)
                        {
                            nukeCells++;
                            q -= cellBitCount;
                        }

                        if (nukeCells >= cellCount)
                        {
                            return bigIntegers();
                        }

                        bigIntegers r;
                        
                        r.cellCount = cellCount - nukeCells;
                        r.cell.resize(r.cellCount);

                        for (cellType i = 0; i < r.cellCount; i++)
                        {
                            cellType j = i + 1;

                            if (j < r.cellCount)
                            {
                                r.cell[i] = ((Tu(cell[(j + nukeCells)]) << cellBitCount)
                                          |  (Tu(cell[(i + nukeCells)]))) >> q;
                            }
                            else
                            {
                                r.cell[i] = (cell[(i + nukeCells)] >> q);
                            }
                        }

                        r.shrink();

                        return r;
                    }

                    bigIntegers &operator >>= (const cellType &b)
                    {
                        return (*this = (*this >> b));
                    }

                    bigIntegers operator << (const cellType &b) const
                    {
                        if (cellCount == 0)
                        {
                            return *this;
                        }

                        if ((cellCount * cellBitCount + b) <= longBitCount)
                        {
                            return bigIntegers((*this).toInteger() << b, false);
                        }

                        cellType pushCells = 0;
                        cellType q = b;

                        while (q > cellBitCount)
                        {
                            pushCells++;
                            q -= cellBitCount;
                        }

                        if ((pushCells == 0) && (q > 0))
                        {
                            const cellType negQ = (cellBitCount - q);
                            const cellType mask = ((1 << q) - 1) << negQ;

                            if ((cell[(cellCount - 1)] & mask) == 0)
                            {
                                /* no additional cells needed */
                                bigIntegers r = *this;
                                cellType overflow = 0;

                                for (cellType i = 0; i < cellCount; i++)
                                {
                                    cellType newContent = (cell[i] << q) | overflow;
                                    overflow = (cell[i] & mask) >> negQ;
                                    r.cell[i] = newContent;
                                }

                                return r;
                            }
                        }

                        bigIntegers r;
                        
                        r.cellCount = cellCount + pushCells + 1;
                        r.cell.resize(r.cellCount);

                        for (cellType i = 0; i < r.cellCount; i++)
                        {
                            if (i < pushCells)
                            {
                                r.cell[i] = 0;
                            }
                            else if (i == (r.cellCount - 1))
                            {
                                cellType j = i - 1;

                                r.cell[i] = ((Tu(cell[(j - pushCells)])) << q) >> cellBitCount;
                            }
                            else if (i > pushCells)
                            {
                                cellType j = i - 1;

                                r.cell[i] = (((Tu(cell[(j - pushCells)]))
                                          |   (Tu(cell[(i - pushCells)]) << cellBitCount)) << q) >> cellBitCount;
                            }
                            else
                            {
                                r.cell[i] = Tu(cell[(i - pushCells)]) << q;
                            }
                        }

                        r.shrink();

                        return r;
                    }

                    bigIntegers &operator <<= (const cellType &b)
                    {
                        if (*this == zero())
                        {
                            return *this;
                        }

                        if (b == zero())
                        {
                            return *this;
                        }

                        if ((cellCount * cellBitCount + b) <= longBitCount)
                        {
                            *this = bigIntegers((*this).toInteger() << b, false);
                            return *this;
                        }

                        if (b < cellBitCount)
                        {
                            const cellType &q = b;
                            const cellType negQ = (cellBitCount - q);
                            const cellType mask = ((1 << q) - 1) << negQ;

                            if ((cell[(cellCount - 1)] & mask) == 0)
                            {
                                /* no additional cells needed */
                                cellType overflow = 0;

                                for (cellType i = 0; i < cellCount; i++)
                                {
                                    cellType newContent = (cell[i] << q) | overflow;
                                    overflow = (cell[i] & mask) >> negQ;
                                    cell[i] = newContent;
                                }
/*
                                if (overflow)
                                {
                                    extend (overflow);
                                }
*/
                                return *this;
                            }
                        }

                        return (*this = (*this << b));
                    }


                    bool negative;

                    data::scratchPad<cellType> cell;
                    cellType cellCount;

                protected:
                    static const Tu overflowMask = (1L << cellBitCount);
                    static const Tu lowMask = (1L << cellBitCount) - 1;
                    static const Tu highMask = ((1L << cellBitCount) - 1) << cellBitCount;

                    static const Tu cellsPerLong = sizeof(Tu) / sizeof(cellType);
                    static const Tu longBitCount = cellsPerLong * cellBitCount;

                    void extend (cellType pValue)
                    {
                        cell.resize (cellCount + 1);
                        cell[cellCount] = pValue;
                        cellCount++;
                    }

                    void shrink (void)
                    {
                        cellType i = cellCount;
                        cellType rem = 0;

                        if (i == 0)
                        {
                            negative = false;
                            return;
                        }

                        while (i > 0)
                        {
                            i--;

                            if (cell[i] == cellType(0))
                            {
                                rem++;
                            }
                            else
                            {
                                break;
                            }
                        }

                        if (rem == 0)
                        {
                            return;
                        }

                        cellCount = cellCount - rem;

                        cell.resize (cellCount);

                        if (cellCount == 0)
                        {
                            negative = false;
                        }
                    }

                    void doAdd (const bigIntegers &a, const bigIntegers &b, bool allocate = true)
                    {
                        /*
                        if ((a.cellCount <= cellsPerLong) && (b.cellCount <= cellsPerLong))
                        {
                            *this = bigIntegers(Tu(a) + Tu(b), negative);
                            if ((*this >= a) && (*this >= b))
                            {
                                return;
                            }
                        }
                        */

                        bool overflow = false;

                        if (allocate)
                        {
                            cellCount = (a.cellCount > b.cellCount) ? a.cellCount : b.cellCount;
                            cell.resize(cellCount);
                            if (cellCount == 0)
                            {
                                return;
                            }
                        }

                        if (cellCount == 0)
                        {
                            return;
                        }

                        for (cellType i = 0; i < cellCount; i++)
                        {
                            cellType av = (i < a.cellCount) ? a.cell[i] : 0;
                            cellType bv = (i < b.cellCount) ? b.cell[i] : 0;

                            if (overflow)
                            {
                                bv = bv + 1;

                                overflow = (bv == 0);
                            }

                            cell[i] = (overflow ? av : (av + bv));

                            overflow = overflow || ((cell[i] < av) || (cell[i] < bv));
                        }

                        if (overflow) // overflow in the most significant cell, add extra cell
                        {
                            extend (cellType(1));
                        }
                    }

                    void doSubtract (const bigIntegers &a, const bigIntegers &b, bool allocate = true)
                    {
                        if ((a.cellCount <= cellsPerLong) && (b.cellCount <= cellsPerLong))
                        {
                            *this = bigIntegers((a).toInteger() - (b).toInteger(), negative);
                            return;
                        }

                        bool overflow = true;

                        if (allocate)
                        {
                            cellCount = a.cellCount;
                        }

                        if (cellCount == 0)
                        {
                            negative = false;
                            return;
                        }

                        if (allocate)
                        {
                            cell.resize(cellCount);
                        }

                        for (cellType i = 0; i < cellCount; i++)
                        {
                            cellType av =    (i < a.cellCount) ? a.cell[i] : 0;
                            cellType bv = ~ ((i < b.cellCount) ? b.cell[i] : 0);

                            if (overflow)
                            {
                                bv = bv + 1;

                                overflow = (bv == 0);
                            }

                            cell[i] = (overflow ? av : (av + bv));

                            overflow = overflow || ((cell[i] < av) || (cell[i] < bv));
                        }

                        if (a.cellCount > 0)
                        {
                            cellType &l = cell[(cellCount - 1)];
                            cellType  o = a.cell[(a.cellCount - 1)];

                            unsigned int j = cellBitCount;

                            do
                            {
                                j--;

                                if (!(o & (1 << j)))
                                {
                                    l &= ~ cellType(1 << j);
                                }
                                else
                                {
                                    break;
                                }
                            }
                            while (j > 0);
                        }

                        shrink();
                    }

                    void doMultiply (const bigIntegers &a, const bigIntegers &b, bool allocate = true)
                    {
                        if ((a == zero()) || (b == zero()))
                        {
                            cellCount = 0;
                            cell.resize(0);
                            return;
                        }

                        if ((a.cellCount + b.cellCount) <= cellsPerLong)
                        {
                            *this = bigIntegers((a).toInteger() * (b).toInteger(), negative);
                            return;
                        }

                        if (allocate)
                        {
                            cellCount = a.cellCount + b.cellCount + 1;
                            cell.resize(cellCount);
                        }

                        bigIntegers c;

                        c.cellCount = cellCount;
                        c.cell.resize (cellCount);

                        for (cellType k = 0; k < cellCount; k++)
                        {
                            cell[k] = cellType(0);
                            c.cell[k] = cellType(0);
                        }

                        for (cellType i = 0; i < a.cellCount; i++)
                        {
                            for (cellType j = 0; j < b.cellCount; j++)
                            {
                                cellType t = i + j + 1;

                                Tu rx = Tu(a.cell[i]) * Tu(b.cell[j]);

                                cellType low = cellType(rx & lowMask);
                                cellType high = cellType((rx & highMask) >> cellBitCount);

                                c.cell[t]     = high;
                                c.cell[(t-1)] = low;
                                
                                doAdd (*this, c, false);

                                c.cell[t]     = cellType(0);
                                c.cell[(t-1)] = cellType(0);
                            }
                        }

                        shrink();
                    }

                    void doDivide (const bigIntegers &a, const bigIntegers &b, bool allocate = true)
                    {
                        if (a == zero())
                        {
                            cellCount = 0;
                            cell.resize(0);
                            return;
                        }

                        if (b == zero())
                        {
                            /* ebil */
                            cellCount = 0;
                            cell.resize(0);
                            return;
                        }

                        if (b == one())
                        {
                            *this = a;
                            return;
                        }

                        if (b > a)
                        {
                            cellCount = 0;
                            cell.resize(0);
                            return;
                        }

                        cellCount = 0;
                        cell.resize(0);

                        bigIntegers r = bigIntegers();

                        for (cellType i = a.cellCount; i > 0; i--)
                        {
                            cellType ir = i - 1;
                            cellType c = a.cell[ir];

                            for (cellType j = cellBitCount; j > 0; j--)
                            {
                                cellType jr = j - 1;
                                r <<= 1;

                                if (c & (1 << jr))
                                {
                                    if (r.cellCount == 0)
                                    {
                                        r.cellCount = 1;
                                        r.cell.resize(1);
                                        r.cell[0] = cellType(1);
                                    }
                                    else
                                    {
                                        r.cell[0] |= 1;
                                    }
                                }

                                if (r >= b)
                                {
                                    r -= b;

                                    *this += (bigIntegers(1) << cellType(ir * cellBitCount + jr));
                                }
                            }
                        }
                    }

                    void doModuloHorner (const bigIntegers &a, const cellType &b, bool allocate = true)
                    {
                        Tu modulus = Tu(b);
                        Tu result = Tu(0);
                        const Tu factor = (Tu(1) << cellBitCount) % modulus;

                        for (cellType i = a.cellCount; i > 0; i--)
                        {
                            result *= factor;
                            result %= modulus;
                            result += (Tu(a.cell[(i-1)]) % modulus);
                            result %= modulus;
                        }

                        cellCount = 1;
                        cell.resize(cellCount);

                        cell[0] = result;

                        shrink();
                    }

                    void doModuloHorner (const bigIntegers &a, const bigIntegers &b, bool allocate = true)
                    {
                        cellType q  = a.cellCount / b.cellCount;
                        cellType qm = a.cellCount % b.cellCount;

                        Tu modulus = Tu(b.cell[0]);
                        Tu result = Tu(0);

                        for (cellType i = 1; i < b.cellCount; i++)
                        {
                            modulus |= (Tu(b.cell[i]) << (i * cellBitCount));
                        }

                        const Tu factor = (1 << (cellBitCount * b.cellCount)) % modulus;

                        for (cellType j = q; j > 0; j--)
                        {
                            Tu value;

                            if ((j == q) && (qm > 0))
                            {
                                cellType i = (j - 1) * b.cellCount;
                                value = Tu(a.cell[i]);

                                for (cellType k = 1; k < qm; k++)
                                {
                                    value |= (Tu(a.cell[(i + k)]) << (k * cellBitCount));
                                }
                            }
                            else
                            {
                                cellType i = (j - 1) * b.cellCount;
                                value = Tu(a.cell[i]);

                                for (cellType k = 1; k < b.cellCount; k++)
                                {
                                    value |= (Tu(a.cell[(i + k)]) << (k * cellBitCount));
                                }
                            }

                            result *= factor;
                            result %= modulus;
                            result += value % modulus;
                            result %= modulus;
                        }

                        cellCount = b.cellCount;
                        cell.resize(cellCount);

                        for (cellType i = 0; i < cellCount; i++)
                        {
                            cell[i] = result & lowMask;
                            result >>= cellBitCount;
                        }

                        shrink();
                    }

                    void doModulo (const bigIntegers &a, const bigIntegers &b, bool allocate = true)
                    {
                        cellCount = 0;
                        cell.resize(0);

                        for (cellType i = a.cellCount; i > 0; i--)
                        {
                            cellType ir = i - 1;
                            cellType c = a.cell[ir];

                            for (cellType j = cellBitCount; j > 0; j--)
                            {
                                cellType jr = j - 1;

                                *this <<= 1;

                                if (c & (1 << jr))
                                {
                                    if (cellCount == 0)
                                    {
                                        cellCount = 1;
                                        cell.resize(1);
                                        cell[0] = cellType(1);
                                    }
                                    else
                                    {
                                        cell[0] |= 1;
                                    }
                                }

                                if (*this >= b)
                                {
                                    *this -= b;
                                }
                            }
                        }
                    }
            };

        };

        typedef numeric::bigIntegers<> Z;
    };
};

#endif
