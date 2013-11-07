/**\file
 * \brief Raise numbers to powers
 *
 * Calculations expressions of the form x^y, i.e. raises numbers to specified
 * powers. Unlike the std::pow() function, these powers are specified at
 * compile time, thus hopefully increasing runtime performance compared to
 * generic algorithms - though quite probably still doing worse than optimised
 * processor instructions for the job.
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

#if !defined(EF_GY_EXPONENTIAL_H)
#define EF_GY_EXPONENTIAL_H

namespace efgy
{
    namespace math
    {
        /**\brief Templates to calculate exponential values
         *
         * The templates in this namespace can be used to raise numbers to
         * specified powers.
         */
        namespace exponential
        {
            /**\brief Integral, compile-time constant power
             *
             * This class defines a method raise() that raises a given number
             * to the power given in the exponent template parameter.
             *
             * We're using a class template as opposed to a function template
             * to allow for partial specialisation of the fix points.
             *
             * \tparam Q        Basic arithmetic type for calculations.
             * \tparam exponent The exponent to raise numbers to with the
             *                  raise() function.
             */
            template<typename Q, long exponent>
            class integral
            {
                public:
                    /**\brief Raise base to power
                     *
                     * Calculates base^exponent, i.e. calculates the
                     * exponent'th power of the base argument. This exponent is
                     * specified as a template argument.
                     *
                     * This function implements the square-and-multiply
                     * algorithm to minimise the number of multiplications
                     * needed to calculate the result.
                     *
                     * \param[in] base The number to raise.
                     *
                     * \return Base raised to the exponent'th power.
                     */
                    static Q raise (const Q &base)
                    {
                        return (exponent % 2 == 0)
                             ?         integral<Q, (exponent    >> 1)>::raise(base*base)
                             : (base * integral<Q,((exponent-1) >> 1)>::raise(base*base));
                    }
            };

            /**\brief Integral, compile-time constant power
             *
             * This is one of the fix points of the integral template. This one
             * handles the case when the exponent is one, i.e. the raise()
             * method simply returns the number without changing it.
             *
             * \tparam Q        Basic arithmetic type for calculations.
             */
            template<typename Q>
            class integral<Q,1>
            {
                public:
                    /**\brief Return value unmodified
                     *
                     * Calculates base^1 = base, i.e. the return value is
                     * always the number you pass to this function.
                     *
                     * \param[in] base The number to raise.
                     *
                     * \return The value of the base argument is passed through
                     *         to the caller.
                     */
                    static Q raise (const Q &base)
                    {
                        return base;
                    }
            };

            /**\brief Integral, compile-time constant power
             *
             * This is one of the fix points of the integral template. This one
             * handles the case when the exponent is negative one, i.e. the
             * raise() method simply returns the reciprocal of its argument.
             *
             * \tparam Q        Basic arithmetic type for calculations.
             */
            template<typename Q>
            class integral<Q,-1>
            {
                public:
                    /**\brief Return reciprocal of value
                     *
                     * Calculates base^(-1) = 1/base, i.e. the return value is
                     * the reciprocal of the argument.
                     *
                     * \param[in] base The number to raise.
                     *
                     * \return Returns the reciprocal of the number passed in.
                     */
                    static Q raise (const Q &base)
                    {
                        return Q(1)/base;
                    }
            };

            /**\brief Integral, compile-time constant power
             *
             * This is one of the fix points of the integral template. This one
             * handles the case when the exponent is zero, i.e. the raise()
             * method simply discards its argument and returns one.
             *
             * \tparam Q        Basic arithmetic type for calculations.
             */
            template<typename Q>
            class integral<Q,0>
            {
                public:
                    /**\brief Raise to the zeroth power
                     *
                     * Discards its argument and returns one, as one is the
                     * zeroth power of any number.
                     *
                     * \param[in] base The number to raise.
                     *
                     * \return Always Q(1).
                     */
                    static Q raise (const Q &base)
                    {
                        return Q(1);
                    }
            };
        };
    };
};

#endif
