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
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 */

#if !defined(EF_GY_FRACTIONS_H)
#define EF_GY_FRACTIONS_H

#include <ef.gy/big-integers.h>
#include <ef.gy/traits.h>

#define normalise minimise

namespace efgy
{
    namespace math
    {
        namespace numeric
        {
            template <typename N>
            class fractional : public numeric
            {
                public:
                    typedef N integer;

                    fractional ()
                        : numerator(N(0)), denominator(N(1))
                        {}

                    fractional (N pNumerator)
                        : numerator(pNumerator), denominator(N(1))
                        {}

                    fractional (N pNumerator, N pDenominator)
                        : numerator(pNumerator), denominator(pDenominator)
                        {
                            normalise();
                            // minimise();
                        }

                    fractional &operator = (const fractional &b)
                    {
                        numerator = b.numerator;
                        denominator = b.denominator;

                        return *this;
                    }
                    fractional &operator = (const N &b)
                    {
                        numerator = b;
                        denominator = N(1);

                        return *this;
                    }

                    fractional operator + (const fractional &b) const
                    {
                        return fractional
                            (numerator * b.denominator + b.numerator * denominator,
                             denominator * b.denominator);
                    }
                    fractional &operator += (const fractional &b)
                    {
                        numerator = numerator * b.denominator + b.numerator * denominator;
                        denominator = denominator * b.denominator;
                        normalise();
                        // minimise();
                        return (*this);
                    }
                    fractional operator + (const N &b) const
                    {
                        return fractional
                            (numerator + b * denominator,
                             denominator);
                    }
                    fractional &operator += (const N &b)
                    {
                        numerator += b * denominator;
                        normalise();
                        // minimise();
                        return (*this);
                    }

                    fractional operator - (const fractional &b) const
                    {
                        return fractional
                            (numerator * b.denominator - b.numerator * denominator,
                             denominator * b.denominator);
                    }
                    fractional &operator -= (const fractional &b)
                    {
                        numerator = numerator * b.denominator - b.numerator * denominator;
                        denominator = denominator * b.denominator;
                        normalise();
                        // minimise();
                        return (*this);
                    }
                    fractional operator - (const N &b) const
                    {
                        return fractional
                            (numerator - b * denominator,
                             denominator);
                    }
                    fractional &operator -= (const N &b)
                    {
                        numerator -= b * denominator;
                        normalise();
                        // minimise();
                        return (*this);
                    }

                    fractional operator * (const fractional &b) const
                    {
                        return fractional
                            (numerator * b.numerator,
                             denominator * b.denominator);
                    }
                    fractional &operator *= (const fractional &b)
                    {
                        numerator *= b.numerator;
                        denominator *= b.denominator;
                        normalise();
                        // minimise();
                        return (*this);
                    }
                    fractional operator * (const N &b) const
                    {
                        return fractional
                            (numerator * b,
                             denominator);
                    }
                    fractional &operator *= (const N &b)
                    {
                        numerator *= b;
                        normalise();
                        // minimise();
                        return (*this);
                    }

                    // missing: %

                    fractional operator ^ (const N &b) const
                    {
                        if (b == zero())
                        {
                            return fractional(1);
                        }
                        else
                        {
                            fractional rv = *this;

                            for (N i = 1; i < b; i++)
                            {
                                rv *= *this;
                            }

                            return rv;
                        }
                    }

                    fractional &operator ^= (const N &b)
                    {
                        return *this = (*this ^ b);
                    }

                    // missing: ^ (fraction)

                    fractional operator / (const fractional &b) const
                    {
                        return fractional
                            (numerator * b.denominator,
                             denominator * b.numerator);
                    }
                    fractional &operator /= (const fractional &b)
                    {
                        numerator *= b.denominator;
                        denominator *= b.numerator;
                        normalise();
                        // minimise();
                        return (*this);
                    }
                    fractional operator / (const N &b) const
                    {
                        return fractional
                            (numerator,
                             denominator * b);
                    }
                    fractional &operator /= (const N &b)
                    {
                        denominator *= b;
                        normalise();
                        // minimise();
                        return (*this);
                    }

                    // missing: >, >=, <, <=
                    //
                    bool operator > (const fractional &b) const
                    {
                        if (numerator < zero())
                        {
                            if (b.numerator >= zero())
                            {
                                return false;
                            }
                        }
                        else if (b.numerator < zero())
                        {
                            return true;
                        }

                        return (numerator * b.denominator) > (b.numerator * denominator);
                    }

                    bool operator > (const zero &b) const
                    {
                        return (numerator > b);
                    }

                    bool operator > (const one &b) const
                    {
                        return (numerator >= b) && (denominator >= b) && (numerator > denominator);
                    }

                    bool operator > (const negativeOne &b) const
                    {
                        return (numerator >= zero())
                            || (*this > fractional(-1));
                    }

                    bool operator == (const fractional &b) const
                    {
                        if ((numerator == b.numerator) && (denominator == b.denominator))
                        {
                            return true;
                        }
#if defined(normalise)
                        else
                        {
                            return false;
                        }
#endif

                        fractional p = *this;
                        fractional q = b;

                        p.minimise();
                        q.minimise();

                        const fractional &pc = p;
                        const fractional &qc = q;

                        return (pc.numerator == qc.numerator) && (pc.denominator == qc.denominator);
                    }

                    bool operator == (const zero &b) const
                    {
                        return numerator == b;
                    }

                    bool operator == (const one &b) const
                    {
                        return ((numerator == b) && (denominator == b))
                            || (numerator == denominator);
                    }

                    bool operator == (const negativeOne &b) const
                    {
                        return ((numerator == b) && (denominator == one()))
                            || (numerator == -denominator);
                    }

                    // missing: !

                    long double toDouble (void) const
                    {
                        return numerator.toDouble() / denominator.toDouble();
                    }

                    operator N (void) const
                    {
                        N rv = this->numerator;
                        rv /= denominator;
                        return rv;
                    }

                    N numerator;
                    N denominator;

                protected:
#undef normalise
                    void normalise (void)
                    {
                        if (denominator < zero())
                        {
                            numerator = -numerator;
                            denominator = -denominator;
                        }
                    }

                    void minimise (void)
                    {
                        normalise();

                        N n = (numerator < zero())
                            ? gcdP (-numerator, denominator)
                            : gcdP ( numerator, denominator);

                        if ((n != zero()) && (n != one()))
                        {
                            numerator /= n;
                            denominator /= n;
                        }
                    }
            };

            template <typename C, typename N>
            std::basic_ostream<C> &operator<< (std::basic_ostream<C> &out, const fractional<N> &f)
            {
                return out << f.numerator << "/" << f.denominator;
            }


            template <typename N>
            fractional<N> reciprocal (const fractional<N> &f)
            {
                if ((f.numerator == zero()) || (f.denominator == zero()))
                {
                    return fractional<N>(N(0));
                }

                return fractional<N>(f.denominator, f.numerator);
            }

            template <typename N>
            class traits<fractional<N>>
            {
                public:
                    typedef typename fractional<N>::integer integral;
                    typedef          fractional<N>          rational;
                    typedef          fractional<N>          self;
                    typedef          fractional<N>          derivable;

                    static const bool stable = traits<N>::stable;
            };
        };

        typedef numeric::fractional<long long> fraction;
        typedef numeric::fractional<Z> Q;
    };
};

#endif
