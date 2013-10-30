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

#if !defined(EF_GY_INTEGERS_H)
#define EF_GY_INTEGERS_H

#include <ef.gy/numeric.h>

namespace efgy
{
    namespace math
    {
        namespace numeric
        {
            template <typename N>
            class fractional;

            template <typename Ts = signed long long, typename Tu = unsigned long long>
            class integers : public numeric
            {
                public:
                    typedef Ts integer;

                    integers ()
                        : value(0), negative (false)
                        {}
                    integers (Ts pInteger)
                        : negative (pInteger < Ts(0)),
                          value((pInteger < Ts(0)) ? -pInteger : pInteger)
                        {}
                    integers (Tu pInteger, bool pNegative)
                        : value(pInteger), negative ((pInteger == 0) ? false : pNegative)
                        {}

                    integers &operator = (const integers &b)
                    {
                        value = b.value;
                        negative = b.negative;

                        return *this;
                    }

                    integers operator + (const integers &b) const
                    {
                        if (negative == b.negative)
                        {
                            return integers (value + b.value, negative);
                        }
                        else
                        {
                            if (value > b.value)
                            {
                                return integers (value - b.value, negative);
                            }
                            else
                            {
                                return integers (b.value - value, !negative);
                            }
                        }
                    }

                    integers &operator += (const integers &b)
                    {
                        integers r = (*this) + b;

                        return ((*this) = r);
                    }
                    
                    integers &operator ++ (void)
                    {
                        integers r = ((*this) + integers(1));

                        return ((*this) = r);
                    }

                    integers operator ++ (int)
                    {
                        integers r = (*this);

                        (*this) += integers(1);

                        return r;
                    }

                    integers operator - (const integers &b) const
                    {
                        return *this + integers (b.value, !b.negative);
                    }

                    integers &operator -= (const integers &b)
                    {
                        integers r = (*this) - b;

                        return ((*this) = r);
                    }

                    integers &operator -- (void)
                    {
                        integers r = ((*this) - integers(1));

                        return ((*this) = r);
                    }

                    integers operator -- (int)
                    {
                        integers r = (*this);

                        (*this) -= integers(1);

                        return r;
                    }

                    integers operator - (void) const
                    {
                        integers r = *this;

                        r.negative = !r.negative;

                        return r;
                    }

                    integers operator * (const integers &b) const
                    {
                        return integers (value * b.value, negative != b.negative);
                    }

                    fractional<integers> operator * (const fractional<integers> &b) const
                    {
                        return b * (*this);
                    }

                    integers &operator *= (const integers &b)
                    {
                        value *= b.value;
                        negative = (negative != b.negative);

                        return *this;
                    }

                    integers operator % (const integers &b) const
                    {
                        return integers (value % b.value, false);
                    }

                    integers &operator %= (const integers &b)
                    {
                        value %= b.value;
                        negative = false;

                        return *this;
                    }

                    fractional<integers> operator / (const integers &b) const
                    {
                        return fractional<integers> (*this, b);
                    }

                    fractional<integers> operator / (const fractional<integers> &b) const
                    {
                        return fractional<integers> (*this) / b;
                    }

                    integers &operator /= (const integers &b)
                    {
                        value /= b.value;
                        negative = (negative != b.negative);

                        return *this;
                    }

                    bool operator > (const integers &b) const
                    {
                        return ((negative && b.negative) && (value < b.value))
                            || ((!negative && !b.negative) && (value > b.value))
                            || (!negative && b.negative);
                    }

                    bool operator > (const zero &b) const
                    {
                        return !negative
                            && (value > 0);
                    }

                    bool operator > (const one &b) const
                    {
                        return !negative
                            && (value > 1);
                    }

                    bool operator > (const negativeOne &b) const
                    {
                        return (*this == zero())
                            || !negative;
                    }

                    bool operator == (const integers &b) const
                    {
                        return (value == b.value)
                            && (negative == b.negative);
                    }

                    bool operator == (const zero &b) const
                    {
                        return value == 0;
                    }

                    bool operator == (const one &b) const
                    {
                        return !negative
                            && (value == 1);
                    }

                    bool operator == (const negativeOne &b) const
                    {
                        return negative
                            && (value == 1);
                    }

                    operator Ts (void) const
                    {
                        return negative ? -value : value;
                    }

                    Tu value;

                    bool negative;
            };
        };

        typedef numeric::integers<signed long long, unsigned long long> number;
    };
};

#endif
