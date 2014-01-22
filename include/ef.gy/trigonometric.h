/**\file
 * \brief Trigonometric
 *
 * Contains common trigonometric functions: sine, secant, tangent, cotangent.
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

#if !defined(EF_GY_TRIGONOMETRIC_H)
#define EF_GY_TRIGONOMETRIC_H

#include <ef.gy/complex.h>
#include <ef.gy/e.h>
#include <cmath>

namespace efgy
{
    namespace math
    {
        /**\brief Calculate sine and cosine
         *
         * Uses the complex exponential function to calculate both the sine and
         * cosine at the same time.
         *
         * \tparam Q Base data type, e.g. double.
         * \tparam N Data type for the number of iterations.
         *
         * \param[in]  pTheta     The angle to calculate the sine and cosine of.
         * \param[out] oCosine    Where to write the cosine to.
         * \param[in]  iterations Number of iterations for the e function.
         *
         * \returns The sine of pTheta.
         */
        template <typename Q, typename N = unsigned long long>
        static inline Q sines (const Q &pTheta, Q &oCosine, const N &iterations = N(10))
        {
            complex<Q> z = e<complex<Q>,N>(complex<Q>(Q(1),Q(0)),
                                           complex<Q>(Q(0),pTheta),
                                           complex<Q>(Q(0),Q(0)),
                                           iterations);

            oCosine = z.one;

            return z.i;
        }

        /**\brief Calculate sine
         *
         * Uses the complex exponential function to calculate the sine of a
         * given angle.
         *
         * \tparam Q Base data type, e.g. double.
         * \tparam N Data type for the number of iterations.
         *
         * \param[in] pTheta     The angle to calculate the sine of.
         * \param[in] iterations Number of iterations for the e function.
         *
         * \returns The sine of pTheta.
         */
        template <typename Q, typename N = unsigned long long>
        constexpr static inline Q sine (const Q &pTheta, const N &iterations = N(10))
        {
            return complex<Q>(e<complex<Q>,N>(complex<Q>(Q(1),Q(0)),
                                              complex<Q>(Q(0),pTheta),
                                              complex<Q>(Q(0),Q(0)),
                                              iterations)).i;
        }

        /**\brief Calculate sine
         *
         * Uses the system's std::sin() function to calculate the sine.
         *
         * \tparam N Data type for the number of iterations.
         *
         * \param[in] pTheta The angle to calculate the sine of.
         *
         * \returns The sine of pTheta.
         */
        template <typename N = unsigned long long>
        static inline double sine (const double &pTheta, const N & = N(10))
        {
            return std::sin(pTheta);
        }

        /**\copydoc sine(double,N) */
        template <typename N = unsigned long long>
        static inline long double sine (const long double &pTheta, const N & = N(10))
        {
            return std::sin(pTheta);
        }

        /**\copydoc sine(double,N) */
        template <typename N = unsigned long long>
        static inline float sine (const float &pTheta, const N & = N(10))
        {
            return std::sin(pTheta);
        }

        /**\brief Calculate cosine
         *
         * Uses the complex exponential function to calculate the cosine of a
         * given angle.
         *
         * \tparam Q Base data type, e.g. double.
         * \tparam N Data type for the number of iterations.
         *
         * \param[in] pTheta     The angle to calculate the cosine of.
         * \param[in] iterations Number of iterations for the e function.
         *
         * \returns The cosine of pTheta.
         */
        template <typename Q, typename N = unsigned long long>
        constexpr static inline Q cosine (const Q &pTheta, const N &iterations = N(10))
        {
            return complex<Q>(e<complex<Q>,N>(complex<Q>(Q(1),Q(0)),
                                              complex<Q>(Q(0),pTheta),
                                              complex<Q>(Q(0),Q(0)),
                                              iterations)).one;
        }

        /**\brief Calculate cosine
         *
         * Uses the system's std::cos() function to calculate the cosine.
         *
         * \tparam N Data type for the number of iterations.
         *
         * \param[in] pTheta The angle to calculate the cosine of.
         *
         * \returns The cosine of pTheta.
         */
        template <typename N = unsigned long long>
        static inline double cosine (const double &pTheta, const N & = N(10))
        {
            return std::cos(pTheta);
        }

        /**\copydoc cosine(double,N) */
        template <typename N = unsigned long long>
        static inline long double cosine (const long double &pTheta, const N & = N(10))
        {
            return std::cos(pTheta);
        }

        /**\copydoc cosine(double,N) */
        template <typename N = unsigned long long>
        static inline float cosine (const float &pTheta, const N & = N(10))
        {
            return std::cos(pTheta);
        }

        /**\brief Calculate secant and cosecant
         *
         * Uses the complex exponential function to calculate both the secant
         * and cosecant at the same time.
         *
         * \tparam Q Base data type, e.g. double.
         * \tparam N Data type for the number of iterations.
         *
         * \param[in]  pTheta     The angle to calculate the secant and cosecant
         *                        of.
         * \param[out] oCosecant  Where to write the cosecant to.
         * \param[in]  iterations Number of iterations for the e function.
         *
         * \returns The secant of pTheta.
         */
        template <typename Q, typename N = unsigned long long>
        static inline Q secants (const Q &pTheta, Q &oCosecant, const N &iterations = N(10))
        {
            complex<Q> z = e<complex<Q>,N>(complex<Q>(Q(1),Q(0)),
                                           complex<Q>(Q(0),pTheta),
                                           complex<Q>(Q(0),Q(0)),
                                           iterations);

            oCosecant = Q(1)/z.i;

            return Q(1)/z.one;
        }

        /**\brief Calculate secant
         *
         * Uses the complex exponential function to calculate the secant of a
         * given angle.
         *
         * \tparam Q Base data type, e.g. double.
         * \tparam N Data type for the number of iterations.
         *
         * \param[in] pTheta     The angle to calculate the secant of.
         * \param[in] iterations Number of iterations for the e function.
         *
         * \returns The secant of pTheta.
         */
        template <typename Q, typename N = unsigned long long>
        constexpr static inline Q secant (const Q &pTheta, const N &iterations = N(10))
        {
            return Q(1) / complex<Q>(e<complex<Q>,N>(complex<Q>(Q(1),Q(0)),
                                                     complex<Q>(Q(0),pTheta),
                                                     complex<Q>(Q(0),Q(0)),
                                                     iterations)).i;
        }

        /**\brief Calculate secant
         *
         * Uses the system's std::sin() function to calculate the secant.
         *
         * \tparam N Data type for the number of iterations.
         *
         * \param[in] pTheta The angle to calculate the secant of.
         *
         * \returns The secant of pTheta.
         */
        template <typename N = unsigned long long>
        static inline double secant (const double &pTheta, const N & = N(10))
        {
            return 1./std::sin(pTheta);
        }

        /**\copydoc secant(double,N) */
        template <typename N = unsigned long long>
        static inline long double secant (const long double &pTheta, const N & = N(10))
        {
            return 1./std::sin(pTheta);
        }

        /**\copydoc secant(double,N) */
        template <typename N = unsigned long long>
        static inline float secant (const float &pTheta, const N & = N(10))
        {
            return 1./std::sin(pTheta);
        }

        /**\brief Calculate cosecant
         *
         * Uses the complex exponential function to calculate the cosecant of a
         * given angle.
         *
         * \tparam Q Base data type, e.g. double.
         * \tparam N Data type for the number of iterations.
         *
         * \param[in] pTheta     The angle to calculate the cosecant of.
         * \param[in] iterations Number of iterations for the e function.
         *
         * \returns The cosecant of pTheta.
         */
        template <typename Q, typename N = unsigned long long>
        constexpr static inline Q cosecant (const Q &pTheta, const N &iterations = N(10))
        {
            return Q(1) / complex<Q>(e<complex<Q>,N>(complex<Q>(Q(1),Q(0)),
                                                     complex<Q>(Q(0),pTheta),
                                                     complex<Q>(Q(0),Q(0)),
                                                     iterations)).one;
        }

        /**\brief Calculate cosecant
         *
         * Uses the system's std::cos() function to calculate the cosecant.
         *
         * \tparam N Data type for the number of iterations.
         *
         * \param[in] pTheta The angle to calculate the cosecant of.
         *
         * \returns The cosecant of pTheta.
         */
        template <typename N = unsigned long long>
        static inline double cosecant (const double &pTheta, const N & = N(10))
        {
            return 1./std::cos(pTheta);
        }

        /**\copydoc cosecant(double,N) */
        template <typename N = unsigned long long>
        static inline long double cosecant (const long double &pTheta, const N & = N(10))
        {
            return 1./std::cos(pTheta);
        }

        /**\copydoc cosecant(double,N) */
        template <typename N = unsigned long long>
        static inline float cosecant (const float &pTheta, const N & = N(10))
        {
            return 1./std::cos(pTheta);
        }

        /**\brief Calculate tangent
         *
         * Uses the complex exponential function to calculate the tangent of a
         * given angle.
         *
         * \tparam Q Base data type, e.g. double.
         * \tparam N Data type for the number of iterations.
         *
         * \param[in] pTheta     The angle to calculate the tangent of.
         * \param[in] iterations Number of iterations for the e function.
         *
         * \returns The tangent of pTheta.
         */
        template <typename Q, typename N = unsigned long long>
        constexpr static inline Q tangent (const Q &pTheta, const N &iterations = N(10))
        {
            return sine(pTheta, iterations) / cosine(pTheta, iterations);
        }

        /**\brief Calculate tangent
         *
         * Uses the system's std::tan() function to calculate the tangent.
         *
         * \tparam N Data type for the number of iterations.
         *
         * \param[in] pTheta The angle to calculate the tangent of.
         *
         * \returns The cosecant of pTheta.
         */
        template <typename N = unsigned long long>
        static inline double tangent (const double &pTheta, const N & = N(10))
        {
            return std::tan(pTheta);
        }

        /**\copydoc tangent(double,N) */
        template <typename N = unsigned long long>
        static inline long double tangent (const long double &pTheta, const N & = N(10))
        {
            return std::tan(pTheta);
        }

        /**\copydoc tangent(double,N) */
        template <typename N = unsigned long long>
        static inline float tangent (const float &pTheta, const N & = N(10))
        {
            return std::tan(pTheta);
        }

        /**\brief Calculate cotangent
         *
         * Uses the complex exponential function to calculate the cotangent of a
         * given angle.
         *
         * \tparam Q Base data type, e.g. double.
         * \tparam N Data type for the number of iterations.
         *
         * \param[in] pTheta     The angle to calculate the cotangent of.
         * \param[in] iterations Number of iterations for the e function.
         *
         * \returns The cotangent of pTheta.
         */
        template <typename Q, typename N = unsigned long long>
        constexpr static inline Q cotangent (const Q &pTheta, const N &iterations = N(10))
        {
            return cosine(pTheta, iterations) / sine(pTheta, iterations);
        }

        /**\brief Calculate cotangent
         *
         * Uses the system's std::tan() function to calculate the cotangent.
         *
         * \tparam N Data type for the number of iterations.
         *
         * \param[in] pTheta The angle to calculate the cotangent of.
         *
         * \returns The cosecant of pTheta.
         */
        template <typename N = unsigned long long>
        static inline double cotangent (const double &pTheta, const N & = N(10))
        {
            return 1./std::tan(pTheta);
        }

        /**\copydoc cotangent(double,N) */
        template <typename N = unsigned long long>
        static inline long double cotangent (const long double &pTheta, const N & = N(10))
        {
            return 1./std::tan(pTheta);
        }

        /**\copydoc cotangent(double,N) */
        template <typename N = unsigned long long>
        static inline float cotangent (const float &pTheta, const N & = N(10))
        {
            return 1./std::tan(pTheta);
        }
        
        /**\brief Calculate arc sine
         *
         * Uses the system's std::acos() function to calculate the arc sine.
         *
         * \tparam N Data type for the number of iterations.
         *
         * \param[in] pTheta The angle to calculate the arc sine of.
         *
         * \returns The arc sine of pTheta.
         */
        template <typename N = unsigned long long>
        static inline double arcsine (const double &pTheta, const N & = N(10))
        {
            return std::asin(pTheta);
        }

        /**\copydoc arcsine(double,N) */
        template <typename N = unsigned long long>
        static inline long double arcsine (const long double &pTheta, const N & = N(10))
        {
            return std::asin(pTheta);
        }

        /**\copydoc arcsine(double,N) */
        template <typename N = unsigned long long>
        static inline float arcsine (const float &pTheta, const N & = N(10))
        {
            return std::asin(pTheta);
        }

        /**\brief Calculate arc cosine
         *
         * Uses the system's std::acos() function to calculate the arc cosine.
         *
         * \tparam N Data type for the number of iterations.
         *
         * \param[in] pTheta The angle to calculate the arc cosine of.
         *
         * \returns The arc cosine of pTheta.
         */
        template <typename N = unsigned long long>
        static inline double arccosine (const double &pTheta, const N & = N(10))
        {
            return std::acos(pTheta);
        }

        /**\copydoc arccosine(double,N) */
        template <typename N = unsigned long long>
        static inline long double arccosine (const long double &pTheta, const N & = N(10))
        {
            return std::acos(pTheta);
        }

        /**\copydoc arccosine(double,N) */
        template <typename N = unsigned long long>
        static inline float arccosine (const float &pTheta, const N & = N(10))
        {
            return std::acos(pTheta);
        }

        /**\brief Calculate arc tangent
         *
         * Uses the system's std::atan() function to calculate the arc tangent.
         *
         * \tparam N Data type for the number of iterations.
         *
         * \param[in] pTheta The angle to calculate the arc tangent of.
         *
         * \returns The arc tangent of pTheta.
         */
        template <typename N = unsigned long long>
        static inline double arctangent (const double &pTheta, const N & = N(10))
        {
            return std::atan(pTheta);
        }

        /**\copydoc arctangent(double,N) */
        template <typename N = unsigned long long>
        static inline long double arctangent (const long double &pTheta, const N & = N(10))
        {
            return std::atan(pTheta);
        }

        /**\copydoc arctangent(double,N) */
        template <typename N = unsigned long long>
        static inline float arctangent (const float &pTheta, const N & = N(10))
        {
            return std::atan(pTheta);
        }

        /**\brief Calculate arc tangent
         *
         * Uses the system's std::atan2() function to calculate the arc tangent
         * of pY/pX.
         *
         * \tparam N Data type for the number of iterations.
         *
         * \param[in] pY The numerator of the value to get the arc tangent of.
         * \param[in] pX The denominator of the value to get the arc tangent of.
         *
         * \returns The arc tangent of pY/pX.
         */
        template <typename N = unsigned long long>
        static inline double arctangent2 (const double &pY, const double &pX, const N & = N(10))
        {
            return std::atan2(pY, pX);
        }
        
        /**\copydoc arctangent2(double,double,N) */
        template <typename N = unsigned long long>
        static inline long double arctangent2 (const long double &pY, const long double &pX, const N & = N(10))
        {
            return std::atan2(pY, pX);
        }
        
        /**\copydoc arctangent2(double,double,N) */
        template <typename N = unsigned long long>
        static inline float arctangent2 (const float &pY, const float &pX, const N & = N(10))
        {
            return std::atan2(pY, pX);
        }
    };
};

#endif
