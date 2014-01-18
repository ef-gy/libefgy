/**\file
 * \brief Calculate 'pi'
 *
 * This file contains a template class to calculate arbitrarily accurate
 * approximations of 'pi' based on the algorithm described by Bailey et al in
 * 1997.
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

#if !defined(EF_GY_PI_H)
#define EF_GY_PI_H

#include <ef.gy/exponential.h>
#include <ef.gy/series.h>

namespace efgy
{
    /**\brief Classes and functions dealing with mathematics
     *
     * This namespace contains all sorts of classes and functions more or less
     * loosely related to mathematics in one way or another. This does not
     * include code that fits better into some other namespace.
     *
     * Use the 'using' directive after including the headers you need if you
     * don't want to use fully qualified names everywhere:
     *
     * \code
     * using namespace efgy::math;
     * \endcode
     */
    namespace math
    {
        /**\brief Contains assorted algorithms
         *
         * This namespace contains various algorithms that came up when
         * implementing the functionality of the library.
         */
        namespace algorithm
        {
            /**\brief Bailey et al's pi algorithm
             *
             * This is an implementation of the algorithm described by Bailey et
             * al in 1997, which calculates an approximation of pi using an
             * infinite series. This class in particular implements the part of
             * the algorithm that comes up with the individual sequence members
             * for the series to sum up.
             *
             * \tparam Q The data type to use in the calculations, e.g. double.
             * \tparam N Sequence index type. 
             */
            template<typename Q, typename N>
            class bailey1997
            {
                public:
                    /**\brief Default number of iterations
                     *
                     * This is the number of iterations used when approximating
                     * the sequence and the number of iterations to use is not
                     * specified explicitly.
                     */
                    static const N defaultSeriesIterations = 3;

                    /**\brief Get sequence member
                     *
                     * Calculates a member of the sequence described in Bailey's
                     * algorithm.
                     *
                     * \param[in] n The sequence member to calculate.
                     *
                     * \returns The sequence member that was to be calculated.
                     */
                    static constexpr Q at (const N &n)
                    {
                        return exponentiate::integral<Q>::raise(Q(1) / Q(16), n)
                           * ( Q(4)/(Q(8)*Q(n)+Q(1))
                             - Q(2)/(Q(8)*Q(n)+Q(4))
                             - Q(1)/(Q(8)*Q(n)+Q(5))
                             - Q(1)/(Q(8)*Q(n)+Q(6)) );
                    }
            };
        };

        /**\brief Calculate 'pi' with arbitrary precision
         *
         * This class is used to handle (and calculate) pi with arbitrary
         * precision (up to the chosen data type's limits, anyway). Results
         * start to get unstable with iterations > 4 when used with the
         * default fraction data type. pi<Q,4> results in an approximation
         * that is already accurate to +/- 1.960*10^-7. If you use a larger
         * rational base for Q you should be able to get an arbitrarily precise
         * approximation to pi.
         *
         * To calculate pi we use the power series expansion described by
         * Bailey et al in 1997.
         *
         * \tparam Q          The data type to use in the calculations -
         *                    should be rational or similar. Must be a type
         *                    with numeric::traits<Q> defined.
         * \tparam N          Base integral type; used to specify the
         *                    precision.
         *
         * \section pi-usage Usage
         *
         * This class may look rather curious, so I should probably explain how
         * to use it. The idea is to create an instance of the pi class with the
         * parameters you need, and to then cast or assign it to the data type
         * you needed. Basically, the class is supposed to act like a function
         * so to use it do something like this:
         *
         * \code{.cpp}
         * double myPi = math::pi<double>();
         * \endcode
         *
         * You could, of course, add additional parameters as needed. This was
         * written before C++11 was all popular, so nowadays we could just use
         * a constexpr static function instead... which is where pi::get comes
         * in:
         *
         * \code{.cpp}
         * double myPi = math::pi<double>::get();
         * \endcode
         *
         * This is slightly more verbose, but it is a true constexpr function so
         * you'll be able to use it in a compile-time context. You can still
         * specify the number of iterations, either as a template argument or as
         * as the first parameter to the function. Adjusting the factor by which
         * pi is multiplied is also supported.
         *
         * Since the template is fairly clean you shouldn't have any issues
         * using it as a temporary, either:
         *
         * \code{.cpp}
         * (double)math::pi<double>();
         * \endcode
         *
         * And finally, in case you're wondering why this has been implemented
         * as a class template that acts like a function template, as opposed to
         * an actual function template, consider this: you can re-specialise
         * templates and operators based on the input types. Which means, you
         * could specialise a multiplication by pi using this template, if your
         * numeric type would do something special then - like, say, you had a
         * type that represents sines or cosines, and you multiplied the result
         * of that with pi, you could just reverse the result's sign as opposed
         * to performing a potentially lengthy floating point operation. I know
         * that at this point that's all theoretical, but it's getting there and
         * I really do think this would be a good thing in the end.
         */
        template <typename Q, typename N = unsigned long long>
        using pi = series::series<Q,algorithm::bailey1997,N>;
    };
};

#endif
