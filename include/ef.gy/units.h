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
 * \see Project Source Code: http://git.becquerel.org/jyujin/libefgy.git
 */

#if !defined(EF_GY_UNITS_H)
#define EF_GY_UNITS_H

#include <ef.gy/exponential.h>
#include <ef.gy/traits.h>

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
        class unitValue : public math::numeric::traits<Q>::derivable
        {
            public:
                /**\brief Original base datatype
                 *
                 * Alias for Q, the base datatype in use. This is not quite the
                 * same as the 'base' typedef, as that is the one this class was
                 * derived from and this is the one that was passed as a
                 * template parameter.
                 */
                typedef Q original;

                /**\brief Base datatype
                 *
                 * Alias for the base datatype in use. This makes it possible
                 * for derived classes to figure out the base datatype without
                 * needing a separate template parameter for that.
                 */
                typedef typename math::numeric::traits<Q>::derivable base;

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
                 * \param[in] pV The value to scale and assign.
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

        /**\brief Calculate metric scale factor
         *
         * This class calculates multiplier for metric prefixes - kilo-, mega-,
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
                /**\brief Calculate scale factor
                 *
                 * This calculates the scale value and returns it. The scale is
                 * 10^(exponent*unitExponent).
                 *
                 * \return The calculated scale.
                 */
                static Q get (void)
                {
                    return math::exponentiate::integral<Q,exponent*unitExponent>::raise(Q(10));
                }
        };

        /**\brief Calculate binary scale factor
         *
         * This class calculates multiplier for binary prefixes - kibi-, mebi-,
         * etc. This is used by the exponentialScaledUnitValue template to
         * convert values between different binary scales.
         *
         * \tparam Q            The base type to generate the multiplier in.
         * \tparam exponent     The exponent to use, e.g. '1' for 'kibi'.
         * \tparam unitExponent The exponent for the unit value, e.g. '2' for
         *                      square metres, '3' for cubic metres and so on.
         */
        template<typename Q, int exponent, int unitExponent>
        class binaryMultiplier
        {
            public:
                /**\brief Calculate scale factor
                 *
                 * This calculates the scale value and returns it. The scale is
                 * 1024^(exponent*unitExponent).
                 *
                 * \return The calculated scale.
                 */
                static Q get (void)
                {
                    return math::exponentiate::integral<Q,exponent*unitExponent>::raise(Q(1024));
                }
        };

        /**\brief Scaled values template
         *
         * Represents a scaled value based on a unit type and a scale functor.
         * This is used to implement the various unit prefixes in common use.
         *
         * \tparam U          The type of the unit value to use.
         * \tparam exponent   Exponent to calculate the scale with.
         * \tparam multiplier A functor to calculate the scale value.
         */
        template<typename U, int exponent, template<typename, int, int> class multiplier>
        class exponentialScaledUnitValue : public U::base
        {
            public:
                /**\brief Base value type
                 *
                 * This is the base value type of the unitValue template; the
                 * separate typedef here makes it easier to use.
                 */
                typedef typename U::base base;

                /**\brief Exponent of the unit value
                 *
                 * This is the unit type's exponent, e.g. '2' for square
                 * metres, '3' for cubic metres and so on. This is used in
                 * calculating the actual scale to use when converting values
                 * of different scales.
                 */
                static const int unitExponent = U::exponent;

                /**\brief Convert unit value
                 *
                 * Scales a unit value to fit the scale factor for this class.
                 *
                 * \param[in] value Unit value to scale.
                 *
                 * \return Value after calculating and applying the intrinsic
                 *         scale factor.
                 */
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

                /**\brief Default constructor
                 *
                 * Initialises a class instance to a value of zero.
                 */
                exponentialScaledUnitValue()
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
                explicit exponentialScaledUnitValue(const base &pV)
                    : base(pV)
                    {}

                /**\brief Unit value constructor
                 *
                 * Scales the value of an instance of the unit type so that its
                 * value is appropriate for this class.
                 *
                 * \param[in] pV The value to initialise the instance to.
                 */
                exponentialScaledUnitValue(const U &pV)
                    : base(convert(base(pV)))
                    {}

                /**\brief Copy value of different scale
                 *
                 * Initialises an instance of the class after scaling a value
                 * of a different scale. The other value must be of the same
                 * unit type.
                 *
                 * \tparam rExponent The exponent of the other scaled value.
                 *
                 * \param[in] pV The value to scale and assign.
                 */
                template<int rExponent>
                exponentialScaledUnitValue(const exponentialScaledUnitValue<U, rExponent, multiplier> &pV)
                    : base(exponentialScaledUnitValue<U,(exponent - rExponent), multiplier>::convert(base(pV)))
                    {}

                /**\brief Copy value of different scale and scale functor
                 *
                 * Initialises an instance of the class after scaling a value
                 * of a different scale, calculated by a different scale
                 * functor. The other value must be of the same unit type.
                 *
                 * \tparam rExponent   The exponent of the other scaled value.
                 * \tparam rMultiplier The scale functor of the other scaled
                 *                     value.
                 *
                 * \param[in] pV The value to scale and assign.
                 */
                template<int rExponent, template<typename, int, int> class rMultiplier>
                exponentialScaledUnitValue(const exponentialScaledUnitValue<U, rExponent, rMultiplier> &pV)
                    : base(base(pV)
                         * exponentialScaledUnitValue<U,  exponent,  multiplier>::convert(base(1))
                         / exponentialScaledUnitValue<U, rExponent, rMultiplier>::convert(base(1)))
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
                    return U(exponentialScaledUnitValue<U,-exponent,multiplier>::convert(base(*this)));
                }
        };

        /**\brief Base class for metric types
         *
         * This template defines a typical metric unit type along with all the
         * common metric prefixes to go with that unit type.
         *
         * \tparam Q            Base unit data type.
         * \tparam unitSymbol   Symbol for the unit type, e.g. 'm' for metres,
         *                      's' for seconds and so on.
         * \tparam unitExponent Exponent for the unit type, e.g. '2' for square
         *                      metres, or '3' for cubic metres.
         */
        template<typename Q, const char unitSymbol, int unitExponent>
        class metric
        {
            public:
                /**\brief Unit type
                 *
                 * This is the generated unit type, based on the given template
                 * parameters. This would be the type for values in 'metres'
                 * for a metric<Q,'m',1> class.
                 */
                typedef unitValue<Q, unitSymbol, unitExponent> unit;

/*
                typedef scaledUnitValue<unit, unsigned long long, 1, 1000> milli;
                typedef scaledUnitValue<unit, unsigned long long, 1, 1000000> micro;
                */

                /**\brief Yotta prefix
                 *
                 * Type for values with a yotta prefix. A yotta-something is
                 * one unit multiplied by 1000^8.
                 */
                typedef exponentialScaledUnitValue<unit, 24, metricMultiplier> yotta;

                /**\brief Zetta prefix
                 *
                 * Type for values with a zetta prefix. A zetta-something is
                 * one unit multiplied by 1000^7.
                 */
                typedef exponentialScaledUnitValue<unit, 21, metricMultiplier> zetta;

                /**\brief Exa prefix
                 *
                 * Type for values with an exa prefix. An exa-something is
                 * one unit multiplied by 1000^6.
                 */
                typedef exponentialScaledUnitValue<unit, 18, metricMultiplier> exa;

                /**\brief Peta prefix
                 *
                 * Type for values with a peta prefix. A peta-something is
                 * one unit multiplied by 1000^5.
                 */
                typedef exponentialScaledUnitValue<unit, 15, metricMultiplier> peta;

                /**\brief Tera prefix
                 *
                 * Type for values with a tera prefix. A tera-something is
                 * one unit multiplied by 1000^4.
                 */
                typedef exponentialScaledUnitValue<unit, 12, metricMultiplier> tera;

                /**\brief Giga prefix
                 *
                 * Type for values with a giga prefix. A giga-something is
                 * one unit multiplied by 1000^3.
                 */
                typedef exponentialScaledUnitValue<unit,  9, metricMultiplier> giga;

                /**\brief Mega prefix
                 *
                 * Type for values with a mega prefix. A mega-something is
                 * one unit multiplied by 1000^2.
                 */
                typedef exponentialScaledUnitValue<unit,  6, metricMultiplier> mega;

                /**\brief Myria prefix
                 *
                 * Type for values with a myria prefix. A myria-something is
                 * one unit multiplied by 10^4.
                 */
                typedef exponentialScaledUnitValue<unit,  4, metricMultiplier> myria;

                /**\brief Kilo prefix
                 *
                 * Type for values with a kilo prefix. A kilo-something is
                 * one unit multiplied by 1000^1.
                 */
                typedef exponentialScaledUnitValue<unit,  3, metricMultiplier> kilo;

                /**\brief Hecto prefix
                 *
                 * Type for values with a hecto prefix. A hecto-something is
                 * one unit multiplied by 10^2.
                 */
                typedef exponentialScaledUnitValue<unit,  2, metricMultiplier> hecto;

                /**\brief Deca prefix
                 *
                 * Type for values with a deca prefix. A deca-something is
                 * one unit multiplied by 10^1.
                 */
                typedef exponentialScaledUnitValue<unit,  1, metricMultiplier> deca;

                /**\brief Deci prefix
                 *
                 * Type for values with a deci prefix. A deci-something is
                 * one unit multiplied by 10^-1.
                 */
                typedef exponentialScaledUnitValue<unit, -1, metricMultiplier> deci;

                /**\brief Centi prefix
                 *
                 * Type for values with a centi prefix. A cento-something is
                 * one unit multiplied by 10^-2.
                 */
                typedef exponentialScaledUnitValue<unit, -2, metricMultiplier> centi;

                /**\brief Milli prefix
                 *
                 * Type for values with a milli prefix. A milli-something is
                 * one unit multiplied by 1000^-1.
                 */
                typedef exponentialScaledUnitValue<unit, -3, metricMultiplier> milli;

                /**\brief Micro prefix
                 *
                 * Type for values with a micro prefix. A micro-something is
                 * one unit multiplied by 1000^-2.
                 */
                typedef exponentialScaledUnitValue<unit, -6, metricMultiplier> micro;

                /**\brief Nano prefix
                 *
                 * Type for values with a nano prefix. A nano-something is
                 * one unit multiplied by 1000^-3.
                 */
                typedef exponentialScaledUnitValue<unit, -9, metricMultiplier> nano;

                /**\brief Pico prefix
                 *
                 * Type for values with a pico prefix. A pico-something is
                 * one unit multiplied by 1000^-4.
                 */
                typedef exponentialScaledUnitValue<unit,-12, metricMultiplier> pico;

                /**\brief Femto prefix
                 *
                 * Type for values with a femto prefix. A femto-something is
                 * one unit multiplied by 1000^-5.
                 */
                typedef exponentialScaledUnitValue<unit,-15, metricMultiplier> femto;

                /**\brief Atto prefix
                 *
                 * Type for values with an atto prefix. An atto-something is
                 * one unit multiplied by 1000^-6.
                 */
                typedef exponentialScaledUnitValue<unit,-18, metricMultiplier> atto;

                /**\brief Zepto prefix
                 *
                 * Type for values with a zepto prefix. A zepto-something is
                 * one unit multiplied by 1000^-7.
                 */
                typedef exponentialScaledUnitValue<unit,-21, metricMultiplier> zepto;

                /**\brief Yocto prefix
                 *
                 * Type for values with a yocto prefix. A yocto-something is
                 * one unit multiplied by 1000^-8.
                 */
                typedef exponentialScaledUnitValue<unit,-24, metricMultiplier> yocto;
        };

        /**\brief Base class for binary types
         *
         * This template defines a typical binary unit unit type along with all
         * the not-so-common binary prefixes to go with that unit type. The
         * template is based on the metric template, because it's fairly common
         * for binary types to be expressed with metric prefixes and the
         * conversion between the types tends to be performed incorrectly.
         *
         * \tparam Q            Base unit data type.
         * \tparam unitSymbol   Symbol for the unit type, e.g. 'm' for metres,
         *                      's' for seconds and so on.
         * \tparam unitExponent Exponent for the unit type, e.g. '2' for square
         *                      metres, or '3' for cubic metres.
         */
        template<typename Q, const char unitSymbol, int unitExponent>
        class binary : public metric<Q,unitSymbol,unitExponent>
        {
            public:
                /**\brief Unit type
                 *
                 * This is the generated unit type, based on the given template
                 * parameters. This would be the type for values in 'bytes'
                 * for a metric<Q,'B',1> class.
                 */
                typedef typename metric<Q,unitSymbol,unitExponent>::unit unit;

                /**\brief Yobi prefix
                 *
                 * Type for values with a yobi prefix. A yobi-something is one
                 * unit multiplied by 1024^8.
                 */
                typedef exponentialScaledUnitValue<unit, 8, binaryMultiplier> yobi;

                /**\brief Zebi prefix
                 *
                 * Type for values with a zebi prefix. A zebi-something is one
                 * unit multiplied by 1024^7.
                 */
                typedef exponentialScaledUnitValue<unit, 7, binaryMultiplier> zebi;

                /**\brief Exbi prefix
                 *
                 * Type for values with an exbi prefix. An exbi-something is
                 * one unit multiplied by 1024^6.
                 */
                typedef exponentialScaledUnitValue<unit, 6, binaryMultiplier> exbi;

                /**\brief Pebi prefix
                 *
                 * Type for values with a pebi prefix. A pebi-something is one
                 * unit multiplied by 1024^5.
                 */
                typedef exponentialScaledUnitValue<unit, 5, binaryMultiplier> pebi;

                /**\brief Tebi prefix
                 *
                 * Type for values with a tebi prefix. A tebi-something is one
                 * unit multiplied by 1024^4.
                 */
                typedef exponentialScaledUnitValue<unit, 4, binaryMultiplier> tebi;

                /**\brief Gibi prefix
                 *
                 * Type for values with a gibi prefix. A gibi-something is one
                 * unit multiplied by 1024^3.
                 */
                typedef exponentialScaledUnitValue<unit, 3, binaryMultiplier> gibi;

                /**\brief Mebi prefix
                 *
                 * Type for values with a mebi prefix. A mebi-something is one
                 * unit multiplied by 1024^2.
                 */
                typedef exponentialScaledUnitValue<unit, 2, binaryMultiplier> mebi;

                /**\brief Kibi prefix
                 *
                 * Type for values with a kibi prefix. A kibi-something is one
                 * unit multiplied by 1024^1.
                 */
                typedef exponentialScaledUnitValue<unit, 1, binaryMultiplier> kibi;
        };

        /**\brief Grammes
         *
         * This class template defines weight values in 'grammes'.
         *
         * \tparam Q            Base unit type.
         * \tparam unitExponent Unit type exponent, e.g. '2' for grammes
         *                      squared.
         */
        template<typename Q, int unitExponent = 1>
        using gramme = metric<Q, 'g', unitExponent>;

        /**\brief Metres
         *
         * This class template defines length values in 'metres'.
         *
         * \tparam Q            Base unit type.
         * \tparam unitExponent Unit type exponent, e.g. '2' for squared
         *                      metres.
         */
        template<typename Q, int unitExponent = 1>
        using metre = metric<Q, 'm', unitExponent>;

        /**\brief Seconds
         *
         * This class template defines time values in 'seconds'.
         *
         * \tparam Q            Base unit type.
         * \tparam unitExponent Unit type exponent, e.g. '2' for squared
         *                      seconds.
         */
        template<typename Q, int unitExponent = 1>
        using second = metric<Q, 's', unitExponent>;

        /**\brief Bytes
         *
         * This class template defines time values in 'bytes'.
         *
         * \tparam Q            Base unit type.
         * \tparam unitExponent Unit type exponent, e.g. '2' for squared
         *                      bytes.
         */
        template<typename Q, int unitExponent = 1>
        using byte = binary<Q, 'B', unitExponent>;

        /**\brief Bits
         *
         * This class template defines time values in 'bits'.
         *
         * \tparam Q            Base unit type.
         * \tparam unitExponent Unit type exponent, e.g. '2' for squared
         *                      bits.
         */
        template<typename Q, int unitExponent = 1>
        using bit = binary<Q, 'b', unitExponent>;
    };
};

#endif
