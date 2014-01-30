/**\file
 * \brief Polar coordinate vectors
 *
 * Contains a specialisation of the generic vector interface that uses polar
 * coordinates instead of the regular real/cartesian coordinates.
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

#if !defined(EF_GY_POLAR_H)
#define EF_GY_POLAR_H

#include <ef.gy/euclidian.h>
#include <ef.gy/trigonometric.h>

namespace efgy
{
    namespace math
    {
        namespace format
        {
            /**\brief Polar coordinate format tag
             *
             * This coordinate format tag identifies vectors as containing polar
             * coordinates as opposed to regular cartesian coordinates.
             */
            class polar
            {
                public:
                    /**\brief Construct with precision
                     *
                     * Constructs a coordinate format tag with an optional
                     * precision. The default precision is '10'.
                     *
                     * \param[in] pPrecision The precision to use.
                     */
                    polar(unsigned long pPrecision = 10)
                        : precision(pPrecision) {}

                    /**\brief Desired conversion precision
                     *
                     * Specifies the precision to use for real<>polar vector
                     * format conversions. This precision refers to the
                     * iterations parameter in the trigonometric functions,
                     * which in turn is passed along to the complex e function.
                     *
                     * \note The precision is ignored when using the float,
                     *       double or long double types, as these automatically
                     *       use the trigonometric functions in the std
                     *       namespace which do not have a precision parameter.
                     */
                    unsigned long precision;
            };

            /**\brief Stream output operator for the polar format tag
             *
             * Writes the contents of a polar format tag to an output stream;
             * Polar format tags contain a precision parameter, which is also
             * written to the provided stream.
             *
             * \tparam C The character type of the stream.
             *
             * \param[out] stream The stream to write to.
             * \param[in]  format The format tag to write to the stream.
             *
             + \returns The parameter 'stream' after writing to it.
             */
            template <typename C>
            constexpr inline std::basic_ostream<C> &operator << (std::basic_ostream<C> &stream, const polar &format)
            {
                return stream << "[polar:" << format.precision << "]";
            }
        };

        /**\brief Polar coordinate vector
         *
         * Specialisation of the generic vectors that uses polar coordinates as
         * opposed to real coordinates.
         *
         * \tparam F     Base type for the vector; should have field-like
         *               properties.
         * \tparam n     Number of vector elements.
         */
        template <typename F, unsigned int n>
        class vector<F,n,format::polar> : public std::array<F,n>
        {
            public:
                /**\copydoc vector::vector
                 * \param[in] s An instance of the format tag; may be used to
                 *              specify a precision for conversion operations.
                 */
                constexpr vector
                    (const std::array<F,n> &t = {{}},
                     const format::polar &s = format::polar())
                    : std::array<F, n>(t),
                      spaceTag(s)
                    {}

                /**\brief Construct with real-space vector
                 *
                 * Initialises a new instance of a polar vector by converting
                 * the contents of a real-valued vector to their polar
                 * equivalents. Like any other operation that involves
                 * trigonometrics, this is not going to produce entirely
                 * accurate results. A desired precision can be specified using
                 * the coordinate space tag.
                 *
                 * \param[in] v The source vector.
                 * \param[in] s An instance of the space tag; may be used to
                 *              specify a precision for conversion operations.
                 */
                vector
                    (const vector<F,n,format::cartesian> &v,
                     const format::polar &s = format::polar())
                    : std::array<F, n>(),
                      spaceTag(s)
                    {
                        (*this)[0] = length(v);
                        for (unsigned int k = 0; k < (n-1); k++)
                        {
                            F s = F(0);
                            for (unsigned int i = k; i < n; i++)
                            {
                                s += v[i] * v[i];
                            }
                            s = v[k] / sqrt(s);
                            if (k == (n-2))
                            {
                                (*this)[(n-1)] = arccosine(s);
                                if (v[(n-1)] < F(0))
                                {
                                    (*this)[(n-1)] = M_PI * F(2) - (*this)[(n-1)];
                                }
                            }
                            else
                            {
                                (*this)[(k+1)] = arccosine(s);
                            }
                        }
                    }

                /**\brief Convert to real space vector
                 *
                 * This operator converts the vector to a real space vector.
                 * The conversion that is employed is generally not lossless, so
                 * you should avoid converting back and forth between the two
                 * spaces if you can do so.
                 *
                 * \returns Real space vector with approximation of the polar
                 *          coordinates.
                 */
                operator vector<F,n,format::cartesian> () const
                {
                    vector<F,n,format::cartesian> v;

                    for (unsigned int i = 0; i < n; i++)
                    {
                        v[i] = (*this)[0];
                    }

                    for (unsigned int i = 0; i < (n-1); i++)
                    {
                        const unsigned int p = i + 1;

                        v[i] *= math::cosine((*this)[p], spaceTag.precision);

                        for (unsigned int j = p; j < n; j++)
                        {
                            v[j] *= math::sine((*this)[p], spaceTag.precision);
                        }
                    }

                    return v;
                }

                /**\copydoc vector::tag */
                constexpr const format::polar &tag (void) const { return spaceTag; }

            protected:
                /**\brief Space tag instance
                 *
                 * Polar coordinates need to keep track of the precision to
                 * employ when converting between real and polar coordinates;
                 * the space tag contains a field for this precision.
                 */
                format::polar spaceTag;
        };
    };
};

#endif
