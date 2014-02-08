/**\file
 * \brief XML renderer
 *
 * The templates in this header file allow tagging output streams as XML output
 * streams so they can be written to like any other C++ stream but producing
 * proper XML instead of plain text.
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

#if !defined(EF_GY_RENDER_XML_H)
#define EF_GY_RENDER_XML_H

#include <ef.gy/render.h>
#include <ef.gy/vector.h>
#include <ef.gy/colour-space-hsl.h>
#include <ef.gy/continued-fractions.h>
#include <string>
#include <sstream>
#include <ostream>

namespace efgy
{
    namespace render
    {
        /**\brief std::ostream XML tag
         *
         * Used to distinguish between a plain std::ostream, and one where the
         * output should be in XML format.
         *
         * \tparam C Character type for the basic_ostream reference.
         */
        template <typename C>
        class oxmlstream
        {
            public:
                /**\brief Construct with stream reference
                 *
                 * Initialises a new ostream XML tag instance.
                 *
                 * \param[out] pStream The stream to write to.
                 */
                oxmlstream (std::basic_ostream<C> &pStream)
                    : stream(pStream),
                      precision(24),
                      resolution(8)
                    {}

                /**\brief Output stream reference
                 *
                 * This is the stream where the output is written to.
                 */
                std::basic_ostream<C> &stream;

                /**\brief Output precision
                 *
                 * This is the number of bits in the numerator or denominator of
                 * fractions that are output to the stream. The default is '24'.
                 */
                long precision;

                /**\brief Output range resolution
                 *
                 * This is the number of steps used when writing the elements of
                 * ranges to the stream. The default is '8'.
                 */
                long resolution;
        };

        /**\brief XML tag
         *
         * Write this to an ostream to turn it into an oxmlstream. Like this:
         *
         * \code{.cpp}
         * cout << XML();
         * \encode
         */
        class XML {};

        /**\brief Convert std::ostream to XML
         *
         * Converts the given stream to an XML stream so that write operations
         * after that will produce XML instead of plain text.
         *
         * \tparam C Character type for the basic_ostream reference.
         *
         * \param[out] stream The stream to write to.
         */
        template <typename C>
        constexpr inline oxmlstream<C> operator << (std::basic_ostream<C> &stream, const XML &)
        {
            return oxmlstream<C>(stream);
        }

        /**\brief Precision wrapper
         *
         * Used to update the precision field of an XML output stream, like so:
         *
         * \code{.cpp}
         * cout << XML() << precision(24);
         * \encode
         */
        class precision
        {
            public:
                /**\brief Construct with precision
                 *
                 * Initialises the instance with a precision value.
                 *
                 * \param[in] pPrecision The precision to store.
                 */
                constexpr precision (const long &pPrecision)
                    : value(pPrecision) {}

                /**\brief Precision value
                 *
                 * This is the actual precision stored in the wrapper.
                 */
                const long value;
        };

        /**\brief Update precision of XML stream
         *
         * Updates the precision member of an XML output stream.
         *
         * \tparam C Character type for the basic_ostream reference.
         *
         * \param[in] stream The stream to modify.
         * \param[in] p      The new precision value.
         *
         * \returns A new copy of the stream.
         */
        template <typename C>
        constexpr inline oxmlstream<C> operator << (oxmlstream<C> stream, const precision &p)
        {
            return stream.precision = p.value, stream;
        }

        /**\brief Resolution wrapper
         *
         * Used to update the resolution field of an XML output stream, like so:
         *
         * \code{.cpp}
         * cout << XML() << resolution(8);
         * \encode
         */
        class resolution
        {
            public:
                /**\brief Construct with resolution
                 *
                 * Initialises the instance with a resolution value.
                 *
                 * \param[in] pResolution The resolution to store.
                 */
                constexpr resolution (const long &pResolution)
                    : value(pResolution) {}

                /**\brief Resolution value
                 *
                 * This is the actual resolution stored in the wrapper.
                 */
                const long value;
        };

        /**\brief Update resolution of XML stream
         *
         * Updates the resolution member of an XML output stream.
         *
         * \tparam C Character type for the basic_ostream reference.
         *
         * \param[in] stream The stream to modify.
         * \param[in] p      The new resolution value.
         *
         * \returns A new copy of the stream.
         */
        template <typename C>
        constexpr inline oxmlstream<C> operator << (oxmlstream<C> stream, const resolution &p)
        {
            return stream.resolution = p.value, stream;
        }

        /**\brief Write HSL colour to XML stream
         *
         * Writes an XML serialisation of an HSL vector to a stream.
         *
         * \tparam C Character type for the basic_ostream reference.
         *
         * \param[out] stream The XML stream to write to.
         * \param[in]  pValue The HSL colour value to serialise.
         *
         * \returns A new copy of the input stream.
         */
        template <typename C>
        static inline oxmlstream<C> operator << (oxmlstream<C> stream, const math::vector<math::fraction,3,math::format::HSL> &pValue)
        {
            math::vector<math::fraction,3,math::format::HSL> value = pValue;
            value.hue = math::numeric::round(value.hue, stream.precision);
            value.saturation = math::numeric::round(value.saturation, stream.precision);
            value.lightness = math::numeric::round(value.lightness, stream.precision);
            
            stream.stream << std::string("<colour xmlns='http://colouri.se/2012' space='hsl'")
                          << " hue='" << value.hue.numerator << "'"
                          << " saturation='" << value.saturation.numerator << "'"
                          << " lightness='" << value.lightness.numerator << "'";

            if (value.hue.denominator != math::numeric::one())
            {
                stream.stream << " hueDenominator='" << value.hue.denominator << "'";
            }
            if (value.saturation.denominator != math::numeric::one())
            {
                stream.stream << " saturationDenominator='" << value.saturation.denominator << "'";
            }
            if (value.lightness.denominator != math::numeric::one())
            {
                stream.stream << " lightnessDenominator='" << value.lightness.denominator << "'";
            }

            stream.stream << "/>";

            return stream;
        }

        /**\brief Write RGB colour to XML stream
         *
         * Writes an XML serialisation of an RGB vector to a stream.
         *
         * \tparam C Character type for the basic_ostream reference.
         *
         * \param[out] stream The XML stream to write to.
         * \param[in]  pValue The RGB colour value to serialise.
         *
         * \returns A new copy of the input stream.
         */
        template <typename C>
        static inline oxmlstream<C> operator << (oxmlstream<C> stream, const math::vector<math::fraction,3,math::format::RGB> &pValue)
        {
            math::vector<math::fraction,3,math::format::RGB> value = pValue;
            value.red = math::numeric::round(value.red, stream.precision);
            value.green = math::numeric::round(value.green, stream.precision);
            value.blue = math::numeric::round(value.blue, stream.precision);

            stream.stream << std::string("<colour xmlns='http://colouri.se/2012' space='rgb'")
                          << " red='" << value.red.numerator << "'"
                          << " green='" << value.green.numerator << "'"
                          << " blue='" << value.blue.numerator << "'";

            if (value.red.denominator != math::numeric::one())
            {
                stream.stream << " redDenominator='" << value.red.denominator << "'";
            }
            if (value.green.denominator != math::numeric::one())
            {
                stream.stream << " greenDenominator='" << value.green.denominator << "'";
            }
            if (value.blue.denominator != math::numeric::one())
            {
                stream.stream << " blueDenominator='" << value.blue.denominator << "'";
            }

            stream.stream << "/>";

            return stream;
        }

        /**\brief Write colour picker to XML stream
         *
         * Writes an XML colour picker to a stream.
         *
         * \tparam C Character type for the basic_ostream reference.
         *
         * \param[out] stream The XML stream to write to.
         * \param[in]  pValue The reference colour value to at the centre.
         *
         * \returns A new copy of the input stream.
         */
        template <typename C, class Q, typename format>
        static inline oxmlstream<C> operator |= (oxmlstream<C> stream, const math::vector<Q,3,format> &value)
        {
            stream.stream << "<picker xmlns='http://colouri.se/2012'>";
            math::vector<Q,3,format> v = value;
            if (v[0].denominator == math::numeric::zero())
            {
                v[0] = Q(0);
            }
            if (v[1].denominator == math::numeric::zero())
            {
                v[1] = Q(0);
            }
            if (v[2].denominator == math::numeric::zero())
            {
                v[2] = Q(0);
            }
            Q al = v[0] / Q(stream.resolution);
            Q ar = (Q(1) - v[0]) / Q(stream.resolution);
            Q bl = v[1] / Q(stream.resolution);
            Q br = (Q(1) - v[1]) / Q(stream.resolution);
            Q cl = v[2] / Q(stream.resolution);
            Q cr = (Q(1) - v[2]) / Q(stream.resolution);

            v[1] = Q(0);
            for (long y = -stream.resolution; y <= stream.resolution; y++)
            {
                stream.stream << "<set>";
                v[1] = (y < 0) ? (value[1] - bl * Q(-y)) : (value[1] + br * Q(y));
                for (long x = -stream.resolution; x <= stream.resolution; x++)
                {
                    v[0] = (x < 0) ? (value[0] - al * Q(-x)) : (value[0] + ar * Q(x));
                    stream << v;
                }
                stream.stream << "</set>";
            }

            stream.stream << "<set>";
            v[0] = value[0];
            v[1] = value[1];
            for (long z = -stream.resolution; z <= stream.resolution; z++)
            {
                v[2] = (z < 0) ? (value[2] - cl * Q(-z)) : (value[2] + cr * Q(z));
                stream << v;
            }
            stream.stream << "</set></picker>";

            return stream;
        }
    };
};

#endif
