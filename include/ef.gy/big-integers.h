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

#if !defined(EF_GY_BIG_INTEGERS_H)
#define EF_GY_BIG_INTEGERS_H

#include <ef.gy/numeric.h>
#include <ef.gy/string.h>
#include <vector>

namespace efgy
{
    namespace math
    {
        namespace numeric
        {
            template <typename Ts = signed long long, typename Tu = unsigned long long, typename cellType = unsigned int, unsigned int cellBitCount = 32>
            class bigIntegers;
        };
    };

    template <typename Ts, typename Tu, typename cellType, unsigned int cellBitCount>
    std::string intToString (math::numeric::bigIntegers<Ts,Tu,cellType,cellBitCount> pNumber, const int pBase = 10);

    namespace math
    {
        namespace numeric
        {
            template <typename N>
            class fractional;

            template <typename N>
            class factorial;

            // template <typename Ts = signed long long, typename Tu = unsigned long long, typename cellType = unsigned short, unsigned int cellBitCount = 16>
            template <typename Ts, typename Tu, typename cellType, unsigned int cellBitCount>
            class bigIntegers : public numeric
            {
                public:
                    typedef Ts integer;

                    bigIntegers ()
                        : cell(0), negative (false)
                        {}
                    bigIntegers (Ts pInteger)
                        : negative (pInteger < Ts(0)),
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

                                cell.resize(s);

                                for (cellType i = 0; i < s; i++)
                                {
                                    cell[i] = v[i];
                                }
                            }
                        }
                    bigIntegers (Tu pInteger, bool pNegative)
                        : cell(0),
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

                                cell.resize(s);

                                for (cellType i = 0; i < s; i++)
                                {
                                    cell[i] = v[i];
                                }
                            }
                        }
                    bigIntegers (const bigIntegers &pB)
                        : cell(pB.cell),
                          negative (pB.negative)
                        {
                            if (cell.size() == 0)
                            {
                                negative = false;
                            }
                        }

                    bigIntegers &operator = (const bigIntegers &b)
                    {
                        if (this != &b)
                        {
                            negative  = b.negative;
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
                                r.negative = negative && (r.cell.size() > 0);
                            }
                            else
                            {
                                r.doSubtract (q, p);
                                r.negative = !negative && (r.cell.size() > 0);
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
                        r.negative = (r.cell.size() > 0) && (negative != b.negative);

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

                        if ((cell.size() <= cellsPerLong) && (b.cell.size() <= cellsPerLong))
                        {
                            return bigIntegers((*this).toInteger() % (b).toInteger(), negative);
                        }

                        bigIntegers r;
                        const bigIntegers &ap =   negative ? -*this : *this;
                        const bigIntegers &bp = b.negative ? -b     : b;

                        if (b.cell.size() == 1)
                        {
                            r.doModuloHorner(ap,b.cell[0]);
                        }
/*
                        else if (b.cell.size() < (cellsPerLong - 1))
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

                        if ((cell.size() <= cellsPerLong) && (b.cell.size() <= cellsPerLong))
                        {
                            *this = bigIntegers((*this).toInteger() / (b).toInteger(), (negative != b.negative));
                            return *this;
                        }

                        bool rnegative = (negative != b.negative);
                        const bigIntegers &aa =   negative ? -*this : *this;
                        const bigIntegers &ba = b.negative ? -b     : b;
                        doDivide (aa, ba);
                        negative = rnegative && (cell.size() > 0);
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
                            if  (cell.size() > b.cell.size())
                            {
                                return true;
                            }

                            if  (cell.size() < b.cell.size())
                            {
                                return false;
                            }

                            for (cellType j = cell.size(); j > 0; j--)
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
                            if  (cell.size() < b.cell.size())
                            {
                                return true;
                            }

                            if  (cell.size() > b.cell.size())
                            {
                                return false;
                            }

                            for (cellType j = cell.size(); j > 0; j--)
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
                            && (cell.size() > 0);
                    }

                    bool operator > (const one &b) const
                    {
                        return !negative
                            && (cell.size() >= 1)
                            && (*this > bigIntegers(1));
                    }

                    bool operator > (const negativeOne &b) const
                    {
                        return (*this == zero())
                            || !negative;
                    }

                    bool operator == (const bigIntegers &b) const
                    {
                        if (cell.size() != b.cell.size())
                        {
                            return false;
                        }

                        if (negative != b.negative)
                        {
                            return false;
                        }

                        for (cellType i = 0; i < cell.size(); i++)
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
                        return cell.size() == 0;
                    }

                    bool operator == (const one &b) const
                    {
                        return !negative
                            && (cell.size() == 1)
                            && (cell[0] == cellType(1));
                    }

                    bool operator == (const negativeOne &b) const
                    {
                        return negative
                            && (cell.size() == 1)
                            && (cell[0] == cellType(1));
                    }

                    operator std::string (void) const
                    {
                        return intToString((*this));
                    }

                    Tu toInteger (void) const
                    {
                        Tu r = 0;

                        if (cell.size() == 0)
                        {
                            return 0;
                        }

                        for (cellType i = 0; i < cell.size(); i++)
                        {
                            r |= Tu(cell[i]) << (i * cellBitCount);
                        }

                        return r;
                    }

                    Ts toSignedInteger (void) const
                    {
                        Tu r = 0;

                        if (cell.size() == 0)
                        {
                            return 0;
                        }

                        for (cellType i = 0; i < cell.size(); i++)
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

                        if (cell.size() == 0)
                        {
                            return 0;
                        }

                        for (cellType i = 0; i < cell.size(); i++)
                        {
                            r += (long double)(cell[i]) * mr;
                            mr *= m;
                        }

                        return negative ? -r : r;
                    }

                    bigIntegers operator >> (const cellType &b) const
                    {
                        if (cell.size() == 0)
                        {
                            return *this;
                        }

                        if (cell.size() <= cellsPerLong)
                        {
                            return bigIntegers(toInteger() >> b, false);
                        }


                        cellType nukeCells = 0;
                        cellType q = b;

                        while (q > cellBitCount)
                        {
                            nukeCells++;
                            q -= cellBitCount;
                        }

                        if (nukeCells >= cell.size())
                        {
                            return bigIntegers();
                        }

                        bigIntegers r;
                        
                        r.cell.resize(cell.size() - nukeCells);

                        for (cellType i = 0; i < r.cell.size(); i++)
                        {
                            cellType j = i + 1;

                            if (j < r.cell.size())
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
                        if (cell.size() == 0)
                        {
                            return *this;
                        }

                        if ((cell.size() * cellBitCount + b) <= longBitCount)
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

                            if ((cell[(cell.size() - 1)] & mask) == 0)
                            {
                                /* no additional cells needed */
                                bigIntegers r = *this;
                                cellType overflow = 0;

                                for (cellType i = 0; i < cell.size(); i++)
                                {
                                    cellType newContent = (cell[i] << q) | overflow;
                                    overflow = (cell[i] & mask) >> negQ;
                                    r.cell[i] = newContent;
                                }

                                return r;
                            }
                        }

                        bigIntegers r;
                        
                        r.cell.resize(cell.size() + pushCells + 1);

                        for (cellType i = 0; i < r.cell.size(); i++)
                        {
                            if (i < pushCells)
                            {
                                r.cell[i] = 0;
                            }
                            else if (i == (r.cell.size() - 1))
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

                        if ((cell.size() * cellBitCount + b) <= longBitCount)
                        {
                            *this = bigIntegers((*this).toInteger() << b, false);
                            return *this;
                        }

                        if (b < cellBitCount)
                        {
                            const cellType &q = b;
                            const cellType negQ = (cellBitCount - q);
                            const cellType mask = ((1 << q) - 1) << negQ;

                            if ((cell[(cell.size() - 1)] & mask) == 0)
                            {
                                /* no additional cells needed */
                                cellType overflow = 0;

                                for (cellType i = 0; i < cell.size(); i++)
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

                    std::vector<cellType> cell;

                protected:
                    static const Tu overflowMask = (1L << cellBitCount);
                    static const Tu lowMask = (1L << cellBitCount) - 1;
                    static const Tu highMask = ((1L << cellBitCount) - 1) << cellBitCount;

                    static const Tu cellsPerLong = sizeof(Tu) / sizeof(cellType);
                    static const Tu longBitCount = cellsPerLong * cellBitCount;

                    void extend (cellType pValue)
                    {
                        cell.push_back(pValue);
                    }

                    void shrink (void)
                    {
                        cellType i = cell.size();
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

                        cell.resize (cell.size() - rem);

                        if (cell.size() == 0)
                        {
                            negative = false;
                        }
                    }

                    void doAdd (const bigIntegers &a, const bigIntegers &b, bool allocate = true)
                    {
                        /*
                        if ((a.cell.size() <= cellsPerLong) && (b.cell.size() <= cellsPerLong))
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
                            cell.resize((a.cell.size() > b.cell.size()) ? a.cell.size() : b.cell.size());
                            if (cell.size() == 0)
                            {
                                return;
                            }
                        }

                        if (cell.size() == 0)
                        {
                            return;
                        }

                        for (cellType i = 0; i < cell.size(); i++)
                        {
                            cellType av = (i < a.cell.size()) ? a.cell[i] : 0;
                            cellType bv = (i < b.cell.size()) ? b.cell[i] : 0;

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
                        if ((a.cell.size() <= cellsPerLong) && (b.cell.size() <= cellsPerLong))
                        {
                            *this = bigIntegers((a).toInteger() - (b).toInteger(), negative);
                            return;
                        }

                        bool overflow = true;

                        if (allocate)
                        {
                            cell.resize(a.cell.size());
                        }

                        if (cell.size() == 0)
                        {
                            negative = false;
                            return;
                        }

                        for (cellType i = 0; i < cell.size(); i++)
                        {
                            cellType av =    (i < a.cell.size()) ? a.cell[i] : 0;
                            cellType bv = ~ ((i < b.cell.size()) ? b.cell[i] : 0);

                            if (overflow)
                            {
                                bv = bv + 1;

                                overflow = (bv == 0);
                            }

                            cell[i] = (overflow ? av : (av + bv));

                            overflow = overflow || ((cell[i] < av) || (cell[i] < bv));
                        }

                        if (a.cell.size() > 0)
                        {
                            cellType &l = cell[(cell.size() - 1)];
                            cellType  o = a.cell[(a.cell.size() - 1)];

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
                            cell.resize(0);
                            return;
                        }

                        if ((a.cell.size() + b.cell.size()) <= cellsPerLong)
                        {
                            *this = bigIntegers((a).toInteger() * (b).toInteger(), negative);
                            return;
                        }

                        if (allocate)
                        {
                            cell.resize(a.cell.size() + b.cell.size() + 1);
                        }

                        bigIntegers c;

                        c.cell.resize (cell.size());

                        for (cellType k = 0; k < cell.size(); k++)
                        {
                            cell[k] = cellType(0);
                            c.cell[k] = cellType(0);
                        }

                        for (cellType i = 0; i < a.cell.size(); i++)
                        {
                            for (cellType j = 0; j < b.cell.size(); j++)
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
                            cell.resize(0);
                            return;
                        }

                        if (b == zero())
                        {
                            /* ebil */
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
                            cell.resize(0);
                            return;
                        }

                        cell.resize(0);

                        bigIntegers r = bigIntegers();

                        for (cellType i = a.cell.size(); i > 0; i--)
                        {
                            cellType ir = i - 1;
                            cellType c = a.cell[ir];

                            for (cellType j = cellBitCount; j > 0; j--)
                            {
                                cellType jr = j - 1;
                                r <<= 1;

                                if (c & (1 << jr))
                                {
                                    if (r.cell.size() == 0)
                                    {
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

                        for (cellType i = a.cell.size(); i > 0; i--)
                        {
                            result *= factor;
                            result %= modulus;
                            result += (Tu(a.cell[(i-1)]) % modulus);
                            result %= modulus;
                        }

                        cell.resize(1);

                        cell[0] = result;

                        shrink();
                    }

                    void doModuloHorner (const bigIntegers &a, const bigIntegers &b, bool allocate = true)
                    {
                        cellType q  = a.cell.size() / b.cell.size();
                        cellType qm = a.cell.size() % b.cell.size();

                        Tu modulus = Tu(b.cell[0]);
                        Tu result = Tu(0);

                        for (cellType i = 1; i < b.cell.size(); i++)
                        {
                            modulus |= (Tu(b.cell[i]) << (i * cellBitCount));
                        }

                        const Tu factor = (1 << (cellBitCount * b.cell.size())) % modulus;

                        for (cellType j = q; j > 0; j--)
                        {
                            Tu value;

                            if ((j == q) && (qm > 0))
                            {
                                cellType i = (j - 1) * b.cell.size();
                                value = Tu(a.cell[i]);

                                for (cellType k = 1; k < qm; k++)
                                {
                                    value |= (Tu(a.cell[(i + k)]) << (k * cellBitCount));
                                }
                            }
                            else
                            {
                                cellType i = (j - 1) * b.cell.size();
                                value = Tu(a.cell[i]);

                                for (cellType k = 1; k < b.cell.size(); k++)
                                {
                                    value |= (Tu(a.cell[(i + k)]) << (k * cellBitCount));
                                }
                            }

                            result *= factor;
                            result %= modulus;
                            result += value % modulus;
                            result %= modulus;
                        }

                        cell.resize(b.cell.size());

                        for (cellType i = 0; i < cell.size(); i++)
                        {
                            cell[i] = result & lowMask;
                            result >>= cellBitCount;
                        }

                        shrink();
                    }

                    void doModulo (const bigIntegers &a, const bigIntegers &b, bool allocate = true)
                    {
                        cell.resize(0);

                        for (cellType i = a.cell.size(); i > 0; i--)
                        {
                            cellType ir = i - 1;
                            cellType c = a.cell[ir];

                            for (cellType j = cellBitCount; j > 0; j--)
                            {
                                cellType jr = j - 1;

                                *this <<= 1;

                                if (c & (1 << jr))
                                {
                                    if (cell.size() == 0)
                                    {
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

    template <typename Ts, typename Tu, typename cellType, unsigned int cellBitCount>
    std::string intToString (math::numeric::bigIntegers<Ts,Tu,cellType,cellBitCount> pNumber, const int pBase)
    {
        bool negative = pNumber < math::numeric::zero();
        std::string rv = "";

        if (negative)
        {
            pNumber = -pNumber;
        }

        while (pNumber > math::numeric::zero())
        {
            const char t[2] = { "0123456789abcdefghijklmnopqrstuvwxyz"
                                "ABCDEFGHIJKLMNOPQRSTUVWXYZ#,."[(pNumber % math::numeric::bigIntegers<Ts,Tu,cellType,cellBitCount>(pBase)).toInteger()],
                                0 };
            std::string tq(t);

            rv = tq + rv;
            pNumber /= pBase;
        }

        if (rv == "")
        {
            rv = "0";
        }

        if (negative)
        {
            std::string m("-");
            rv = m + rv;
        }

        return rv;
    }
};

#endif
