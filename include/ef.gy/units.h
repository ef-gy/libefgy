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

#if !defined(EF_GY_UNITS_H)
#define EF_GY_UNITS_H

#include <ef.gy/exponential.h>

namespace efgy
{
    namespace unit
    {
        template<typename Q, const char unitSymbol, int unitExponent>
        class unitValue : public Q
        {
            public:
                typedef Q base;
                static const char   symbol = unitSymbol;
                static const int  exponent = unitExponent;

                unitValue()
                    : base(0)
                    {}

                explicit unitValue(const base &pV)
                    : base(pV)
                    {}

                unitValue(const unitValue<base,symbol,exponent> &pV)
                    : base(base(pV))
                    {}
        };

        template<typename U, typename I, I factor, I divisor>
        class scaledUnitValue : public U::base
        {
            public:
                typedef typename U::base base;

                scaledUnitValue()
                    : base(0)
                    {}

                explicit scaledUnitValue(const base &pV)
                    : base(pV)
                    {}

                scaledUnitValue(const U &pV)
                    : base(base(pV) * base(divisor) / base(factor))
                    {}

                template<I rFactor, I rDivisor>
                scaledUnitValue(const scaledUnitValue<U, I, rFactor, rDivisor> &pV)
                    : base(base(pV) * base(divisor) / base(rDivisor) * base(rFactor) / base(factor))
                    {}

                operator U (void) const
                {
                    return U(base(*this) * base(factor) / base(divisor));
                }
        };

        template<typename Q, int exponent, int unitExponent>
        class metricMultiplier
        {
            public:
                static Q get (void)
                {
                    return math::exponential::integral<Q,exponent*unitExponent>::raise(Q(10));
                }
        };

        template<typename Q, int exponent, int unitExponent>
        class binaryMultiplier
        {
            public:
                static Q get (void)
                {
                    return math::exponential::integral<Q,exponent*unitExponent>::raise(Q(1024));
                }
        };

        template<typename U, int exponent, template<typename, int, int> class multiplier>
        class exponentialScaledUnitValue : public U::base
        {
            public:
                typedef typename U::base base;
                static const int unitExponent = U::exponent;

                static base convert (const base &value)
                {
                    return ((exponent * unitExponent) <= 0)
                         ? value * multiplier
                                        <base,
                                         exponent < 0 ? -exponent : exponent,
                                         unitExponent < 0 ? -unitExponent : unitExponent>::get()
                         : value / multiplier
                                        <base,
                                         exponent < 0 ? -exponent : exponent,
                                         unitExponent < 0 ? -unitExponent : unitExponent>::get();
                }

                exponentialScaledUnitValue()
                    : base(0)
                    {}

                explicit exponentialScaledUnitValue(const base &pV)
                    : base(pV)
                    {}

                exponentialScaledUnitValue(const U &pV)
                    : base(convert(base(pV)))
                    {}

                template<int rExponent>
                exponentialScaledUnitValue(const exponentialScaledUnitValue<U, rExponent, multiplier> &pV)
                    : base(exponentialScaledUnitValue<U,(exponent - rExponent), multiplier>::convert(base(pV)))
                    {}

                template<int rExponent, template<typename, int, int> class rMultiplier>
                exponentialScaledUnitValue(const exponentialScaledUnitValue<U, rExponent, rMultiplier> &pV)
                    : base(base(pV)
                         * exponentialScaledUnitValue<U,  exponent,  multiplier>::convert(base(pV))
                         / exponentialScaledUnitValue<U, rExponent, rMultiplier>::convert(base(pV)))
                    {}

                operator U (void) const
                {
                    return U(exponentialScaledUnitValue<U,-exponent,multiplier>::convert(base(*this)));
                }
        };

        template<typename Q, const char unitSymbol, int unitExponent>
        class metric
        {
            public:
                typedef unitValue<Q, unitSymbol, unitExponent> unit;

/*
                typedef scaledUnitValue<unit, unsigned long long, 1, 1000> milli;
                typedef scaledUnitValue<unit, unsigned long long, 1, 1000000> micro;
                */
                typedef exponentialScaledUnitValue<unit, 24, metricMultiplier> yotta;
                typedef exponentialScaledUnitValue<unit, 21, metricMultiplier> zetta;
                typedef exponentialScaledUnitValue<unit, 18, metricMultiplier> exa;
                typedef exponentialScaledUnitValue<unit, 15, metricMultiplier> peta;
                typedef exponentialScaledUnitValue<unit, 12, metricMultiplier> tera;
                typedef exponentialScaledUnitValue<unit,  9, metricMultiplier> giga;
                typedef exponentialScaledUnitValue<unit,  6, metricMultiplier> mega;
                typedef exponentialScaledUnitValue<unit,  4, metricMultiplier> myria;
                typedef exponentialScaledUnitValue<unit,  3, metricMultiplier> kilo;
                typedef exponentialScaledUnitValue<unit,  2, metricMultiplier> hecto;
                typedef exponentialScaledUnitValue<unit,  1, metricMultiplier> deca;
                typedef exponentialScaledUnitValue<unit, -1, metricMultiplier> deci;
                typedef exponentialScaledUnitValue<unit, -2, metricMultiplier> centi;
                typedef exponentialScaledUnitValue<unit, -3, metricMultiplier> milli;
                typedef exponentialScaledUnitValue<unit, -6, metricMultiplier> micro;
                typedef exponentialScaledUnitValue<unit, -9, metricMultiplier> nano;
                typedef exponentialScaledUnitValue<unit,-12, metricMultiplier> pico;
                typedef exponentialScaledUnitValue<unit,-15, metricMultiplier> femto;
                typedef exponentialScaledUnitValue<unit,-18, metricMultiplier> atto;
                typedef exponentialScaledUnitValue<unit,-21, metricMultiplier> zepto;
                typedef exponentialScaledUnitValue<unit,-24, metricMultiplier> yocto;
        };

        template<typename Q, const char unitSymbol, int unitExponent>
        class binary : public metric<Q,unitSymbol,unitExponent>
        {
            public:
                typedef typename metric<Q,unitSymbol,unitExponent>::unit unit;

                typedef exponentialScaledUnitValue<unit, 8, binaryMultiplier> yobi;
                typedef exponentialScaledUnitValue<unit, 7, binaryMultiplier> zebi;
                typedef exponentialScaledUnitValue<unit, 6, binaryMultiplier> exbi;
                typedef exponentialScaledUnitValue<unit, 5, binaryMultiplier> pebi;
                typedef exponentialScaledUnitValue<unit, 4, binaryMultiplier> tebi;
                typedef exponentialScaledUnitValue<unit, 3, binaryMultiplier> gibi;
                typedef exponentialScaledUnitValue<unit, 2, binaryMultiplier> mebi;
                typedef exponentialScaledUnitValue<unit, 1, binaryMultiplier> kibi;
        };

        template<typename Q, int unitExponent = 1>
        class gramme : public metric<Q, 'g', unitExponent> {};

        template<typename Q, int unitExponent = 1>
        class metre : public metric<Q, 'm', unitExponent> {};

        template<typename Q, int unitExponent = 1>
        class second : public metric<Q, 's', unitExponent> {};

        template<typename Q, int unitExponent = 1>
        class byte : public binary<Q, 'B', unitExponent> {};

        template<typename Q, int unitExponent = 1>
        class bit : public binary<Q, 'b', unitExponent> {};
    };
};

#endif
