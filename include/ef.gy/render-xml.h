/**\file
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
#include <ef.gy/continued-fractions.h>
#include <string>
#include <sstream>

namespace efgy
{
    namespace render
    {
        template <class Q>
        std::string xml (const Q &value, bool small = false, const long &precision = 24)
        {
            throw "no conversion to XML known";
            return "";
        }

        template <>
        std::string xml (const math::vector<math::fraction,3,math::format::HSL> &pValue, bool small, const long &precision)
        {
            math::vector<math::fraction,3,math::format::HSL> value = pValue;
            value.hue = math::numeric::round(value.hue, precision);
            value.saturation = math::numeric::round(value.saturation, precision);
            value.lightness = math::numeric::round(value.lightness, precision);

            std::ostringstream s("");
            
            s << std::string("<colour xmlns='http://colouri.se/2012' space='hsl'")
              << (small ? " type='small'" : "")
              << " hue='" << value.hue.numerator << "'"
              << " saturation='" << value.saturation.numerator << "'"
              << " lightness='" << value.lightness.numerator << "'";

            if (value.hue.denominator != math::numeric::one())
            {
                s << " hueDenominator='" << value.hue.denominator << "'";
            }
            if (value.saturation.denominator != math::numeric::one())
            {
                s << " saturationDenominator='" << value.saturation.denominator << "'";
            }
            if (value.lightness.denominator != math::numeric::one())
            {
                s << " lightnessDenominator='" << value.lightness.denominator << "'";
            }

            s << "/>";

            return s.str();
        }

        template <>
        std::string xml (const math::vector<math::fraction,3,math::format::RGB> &pValue, bool small, const long &precision)
        {
            math::vector<math::fraction,3,math::format::RGB> value = pValue;
            value.red = math::numeric::round(value.red, precision);
            value.green = math::numeric::round(value.green, precision);
            value.blue = math::numeric::round(value.blue, precision);

            std::ostringstream s("");
            
            s << std::string("<colour xmlns='http://colouri.se/2012' space='rgb'")
              << (small ? " type='small'" : "")
              << " red='" << value.red.numerator << "'"
              << " green='" << value.green.numerator << "'"
              << " blue='" << value.blue.numerator << "'";

            if (value.red.denominator != math::numeric::one())
            {
                s << " redDenominator='" << value.red.denominator << "'";
            }
            if (value.green.denominator != math::numeric::one())
            {
                s << " greenDenominator='" << value.green.denominator << "'";
            }
            if (value.blue.denominator != math::numeric::one())
            {
                s << " blueDenominator='" << value.blue.denominator << "'";
            }

            s << "/>";

            return s.str();
        }

        template <class Q>
        std::string xmlpicker (const Q &value, const long &precision = 24)
        {
            throw "no conversion to XML known";
            return "";
        }

        static const int pickerResolution = 8;

        template <class Q, unsigned int d, typename space>
        std::string xmlpicker3d (const math::vector<Q,d,space> &value, const long &precision = 24)
        {
            std::string s = "<picker xmlns='http://colouri.se/2012'>";
            math::vector<Q,d,space> v = value;
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
            Q al = v[0] / Q(pickerResolution);
            Q ar = (Q(1) - v[0]) / Q(pickerResolution);
            Q bl = v[1] / Q(pickerResolution);
            Q br = (Q(1) - v[1]) / Q(pickerResolution);
            Q cl = v[2] / Q(pickerResolution);
            Q cr = (Q(1) - v[2]) / Q(pickerResolution);

            v[1] = Q(0);
            for (int y = -pickerResolution; y <= pickerResolution; y++)
            {
                s += "<set>";
                v[1] = (y < 0) ? (value[1] - bl * Q(-y)) : (value[1] + br * Q(y));
                for (int x = -pickerResolution; x <= pickerResolution; x++)
                {
                    v[0] = (x < 0) ? (value[0] - al * Q(-x)) : (value[0] + ar * Q(x));
                    s += xml (v, true, precision);
                }
                s += "</set>";
            }

            s += "<set>";
            v[0] = value[0];
            v[1] = value[1];
            for (int z = -pickerResolution; z <= pickerResolution; z++)
            {
                v[2] = (z < 0) ? (value[2] - cl * Q(-z)) : (value[2] + cr * Q(z));
                s += xml (v, true, precision);
            }
            s += "</set>";

            return s += "</picker>";
        }

        template <>
        std::string xmlpicker (const math::vector<math::fraction,3,math::format::HSL> &value, const long &precision)
        {
            return xmlpicker3d(value, precision);
        }

        template <>
        std::string xmlpicker (const math::vector<math::fraction,3,math::format::RGB> &value, const long &precision)
        {
            return xmlpicker3d(value, precision);
        }
    };
};

#endif
