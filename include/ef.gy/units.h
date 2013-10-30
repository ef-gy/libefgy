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

#if !defined(EF_GY_UNITS_H)
#define EF_GY_UNITS_H

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

        template<typename Q, int exponent>
        class exponential
        {   public: static Q get (const Q &base) { return base * exponential<Q,exponent-1>::get(base); } };

        template<typename Q>
        class exponential<Q,1>
        {   public: static Q get (const Q &base) { return base; } };

        template<typename Q>
        class exponential<Q,0>
        {   public: static Q get (const Q &base) { return Q(1); } };

        template<typename Q, int exponent, int unitExponent>
        class metricMultiplier
        {   public: static Q get (void) { return exponential<Q,exponent*unitExponent>::get(Q(10)); } };

        template<typename U, int exponent>
        class metricScaledUnitValue : public U::base
        {
            public:
                typedef typename U::base base;
                static const int unitExponent = U::exponent;

                static base convert (const base &value)
                {
                    return ((exponent * unitExponent) <= 0)
                         ? value * metricMultiplier
                                        <base,
                                         exponent < 0 ? -exponent : exponent,
                                         unitExponent < 0 ? -unitExponent : unitExponent>::get()
                         : value / metricMultiplier
                                        <base,
                                         exponent < 0 ? -exponent : exponent,
                                         unitExponent < 0 ? -unitExponent : unitExponent>::get();
                }

                metricScaledUnitValue()
                    : base(0)
                    {}

                explicit metricScaledUnitValue(const base &pV)
                    : base(pV)
                    {}

                metricScaledUnitValue(const U &pV)
                    : base(convert(base(pV)))
                    {}

                template<int rExponent>
                metricScaledUnitValue(const metricScaledUnitValue<U, rExponent> &pV)
                    : base(metricScaledUnitValue<U,(exponent - rExponent)>::convert(base(pV)))
                    {}

                operator U (void) const
                {
                    return U(metricScaledUnitValue<U,-exponent>::convert(base(*this)));
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
                typedef metricScaledUnitValue<unit, 24> yotta;
                typedef metricScaledUnitValue<unit, 21> zetta;
                typedef metricScaledUnitValue<unit, 18> exa;
                typedef metricScaledUnitValue<unit, 15> peta;
                typedef metricScaledUnitValue<unit, 12> tera;
                typedef metricScaledUnitValue<unit,  9> giga;
                typedef metricScaledUnitValue<unit,  6> mega;
                typedef metricScaledUnitValue<unit,  3> kilo;
                typedef metricScaledUnitValue<unit,  2> hecto;
                typedef metricScaledUnitValue<unit,  1> deca;
                typedef metricScaledUnitValue<unit, -1> deci;
                typedef metricScaledUnitValue<unit, -2> centi;
                typedef metricScaledUnitValue<unit, -3> milli;
                typedef metricScaledUnitValue<unit, -6> micro;
                typedef metricScaledUnitValue<unit, -9> nano;
                typedef metricScaledUnitValue<unit,-12> pico;
                typedef metricScaledUnitValue<unit,-15> femto;
                typedef metricScaledUnitValue<unit,-18> atto;
                typedef metricScaledUnitValue<unit,-21> zepto;
                typedef metricScaledUnitValue<unit,-24> yocto;
        };

        template<typename Q, int unitExponent = 1>
        class gramme : public metric<Q, 'g', unitExponent> {};

        template<typename Q, int unitExponent = 1>
        class metre : public metric<Q, 'm', unitExponent> {};

        template<typename Q, int unitExponent = 1>
        class second : public metric<Q, 's', unitExponent> {};

        template<typename Q, int unitExponent = 1>
        class byte : public metric<Q, 'B', unitExponent> {};

        template<typename Q, int unitExponent = 1>
        class bit : public metric<Q, 'b', unitExponent> {};
    };
};

#endif
