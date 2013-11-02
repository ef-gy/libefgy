/**\file
 * \brief Unit conversion helpers
 *
 * This file contains templates that help to ensure that values specified using
 * different units get converted correctly to the units you're expecting. By
 * using these types instead of regular types, the compiler will know when to
 * convert values and when to leave them like they are.
 *
 * Letting the compiler handle unit conversions automatically eliminates quite
 * a few types of very nasty errors. Metric vs. imperial length units in
 * spacecrafts, anyone? ;)
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
    /**\brief Unit conversion templates
     *
     * Unit conversions and specifications get their own namespace because they
     * don't really fit in anywhere else.
     */
    namespace unit
    {
        /**\brief Unit values
         *
         * This class template is used to distinguish between values in
         * different units without any multiplying prefixes, like metres or
         * seconds.
         *
         * The advantage of this template is that it ensures that you can't
         * assign quantities of incompatible units to each other - i.e. you
         * can't assign a value in seconds to one where metres are expected,
         * and you can't assign a value in square metres to one where cubic
         * metres are expected.
         *
         * \tparam Q            The data type to use.
         * \tparam unitSymbol   The symbol for this unit, e.g. 'm' or 's'.
         * \tparam unitExponent The exponent for this unit, e.g. '2' for
         *                      square metres.
         */
        template<typename Q, const char unitSymbol, int unitExponent>
        class unitValue : public Q
        {
            public:
                /**\brief Base datatype
                 *
                 * Alias for the base datatype in use. This makes it possible
                 * for derived classes to figure out the base datatype without
                 * needing a separate template parameter for that.
                 */
                typedef Q base;

                /**\brief Unit symbol
                 *
                 * Alias for the unitSymbol template parameter. Intended to be
                 * used when turning values into strings to present to users.
                 */
                static const char   symbol = unitSymbol;

                /**\brief Unit exponent
                 *
                 * Alias for the unitExponent template parameter. Used in some
                 * conversion formulas by derived classes.
                 */
                static const int  exponent = unitExponent;

                /**\brief Default constructor
                 *
                 * Initialises an instance with a numeric value of zero.
                 */
                unitValue()
                    : base(0)
                    {}

                /**\brief Explicit value constructor
                 *
                 * Initialises an instance with a specified value.
                 *
                 * Explicit so that you can't accidentally assign two unitValue
                 * instances of different units or with different exponents to
                 * each other with a silent conversion by the compiler.
                 *
                 * \brief[in] pV The value to use.
                 */
                explicit unitValue(const base &pV)
                    : base(pV)
                    {}

                /**\brief Copy constructor
                 *
                 * Copies a given value of the exact same type.
                 *
                 * This is specified separately as a fallback for the explicit
                 * value constructor so that you don't get any warnings when
                 * assigning values of the exact same type to each other.
                 *
                 * \brief[in] pV The value to copy.
                 */
                unitValue(const unitValue<base,symbol,exponent> &pV)
                    : base(base(pV))
                    {}
        };

        /**\brief Scaled value
         *
         * Represents a value in a fixed scale to a unitValue class. This
         * template is designed for use with non-metric and non-binary
         * scales.
         *
         * \tparam U       A unit type that the scaled values are relative to.
         *                 Supply an class based on the unitValue template.
         * \tparam I       The type for the scale values.
         * \tparam factor  The nominator part of the scale value.
         * \tparam divisor The denominator part of the scale value.
         */
        template<typename U, typename I, I factor, I divisor>
        class scaledUnitValue : public U::base
        {
            public:
                /**\brief Base value type
                 *
                 * This is the base value type of the unitValue template; the
                 * separate typedef here makes it easier to use.
                 */
                typedef typename U::base base;

                /**\brief Default constructor
                 *
                 * Initialises a class instance to a value of zero.
                 */
                scaledUnitValue()
                    : base(0)
                    {}

                /**\brief Explicit value constructor
                 *
                 * Initialise an instance of the class with a specific value.
                 * This constructor is declared explicit so as not to be used
                 * automatically when assigning values of different scales.
                 *
                 * \param[in] pV The value to initialise the instance to.
                 */
                explicit scaledUnitValue(const base &pV)
                    : base(pV)
                    {}

                /**\brief Unit value constructor
                 *
                 * Scales the value of an instance of the unit type so that its
                 * value is appropriate for this class.
                 *
                 * \param[in] pV The value to initialise the instance to.
                 */
                scaledUnitValue(const U &pV)
                    : base(base(pV) * base(divisor) / base(factor))
                    {}

                /**\brief Copy value of different scale
                 *
                 * Initialises an instance of the class after scaling a value
                 * of a different scale. The other value must be of the same
                 * unit type.
                 *
                 * \tparam rFactor  The other value's scale's nominator.
                 * \tparam rDivisor The other value's scale's denominator.
                 *
                 * \paramïin] pV The value to scale and assign.
                 */
                template<I rFactor, I rDivisor>
                scaledUnitValue(const scaledUnitValue<U, I, rFactor, rDivisor> &pV)
                    : base(base(pV) * base(divisor) / base(rDivisor) * base(rFactor) / base(factor))
                    {}

                /**\brief Convert to unit value
                 *
                 * Scales the stored value back to the unit type and returns
                 * it.
                 *
                 * \return An instance of the unitType template this class is
                 *         based on.
                 */
                operator U (void) const
                {
                    return U(base(*this) * base(factor) / base(divisor));
                }
        };

        /**\brief Calculate metric multipliers
         *
         * This class calculates multiplier for metric prefix - kilo-, mega-,
         * etc. This is used by the exponentialScaledUnitValue template to
         * convert values between different metric scales.
         *
         * \tparam Q            The base type to generate the multiplier in.
         * \tparam exponent     The exponent to use, e.g. '3' for 'kilo'.
         * \tparam unitExponent The exponent for the unit value, e.g. '2' for
         *                      square metres, '3' for cubic metres and so on.
         */
        template<typename Q, int exponent, int unitExponent>
        class metricMultiplier
        {
            public:
                /**\brief Calculate scale
                 *
                 * This calculates the scale value and returns it. Basically
                 * 10^(exponent*unitExponent).
                 *
                 * \return The calculated scale.
                 */
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
