/**\file
 * \brief XML renderer
 *
 * The templates in this header file allow tagging output streams as XML output
 * streams so they can be written to like any other C++ stream but producing
 * proper XML instead of plain text.
 *
 * \copyright
 * Copyright (c) 2012-2015, ef.gy Project Members
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

#if !defined(EF_GY_RENDER_XML_H)
#define EF_GY_RENDER_XML_H

#include <ef.gy/render.h>
#include <ef.gy/vector.h>
#include <ef.gy/stream-xml.h>
#include <ef.gy/colour-space-hsl.h>
#include <ef.gy/continued-fractions.h>
#include <sstream>

namespace efgy
{
    namespace render
    {
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
        static inline xml::ostream<C> operator << (xml::ostream<C> stream, const math::vector<math::fraction,3,math::format::HSL> &pValue)
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
        static inline xml::ostream<C> operator << (xml::ostream<C> stream, const math::vector<math::fraction,3,math::format::RGB> &pValue)
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
         * \param[in]  value The reference colour value to at the centre.
         *
         * \returns A new copy of the input stream.
         */
        template <typename C, class Q, typename format>
        static inline xml::ostream<C> operator |= (xml::ostream<C> stream, const math::vector<Q,3,format> &value)
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
