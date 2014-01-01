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
#include <ef.gy/string.h>
#include <ef.gy/continued-fractions.h>
#include <string>

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
        std::string xml (const colour::HSL<math::fraction>::value &pValue, bool small, const long &precision)
        {
            colour::HSL<math::fraction>::value value = pValue;
            value.hue = math::numeric::round(value.hue, precision);
            value.saturation = math::numeric::round(value.saturation, precision);
            value.lightness = math::numeric::round(value.lightness, precision);

            return std::string("<colour xmlns='http://colouri.se/2012' space='hsl'")
                 + (small ? " type='small'" : "")
                 + " hue='" + intToString(value.hue.numerator) + "'"
                 + (value.hue.denominator != math::numeric::one()
                   ? " hueDenominator='" + intToString(value.hue.denominator) + "'"
                   : "")
                 + " saturation='" + intToString(value.saturation.numerator) + "'"
                 + (value.saturation.denominator != math::numeric::one()
                   ? " saturationDenominator='" + intToString(value.saturation.denominator) + "'"
                   : "")
                 + " lightness='" + intToString(value.lightness.numerator) + "'"
                 + (value.lightness.denominator != math::numeric::one()
                   ? " lightnessDenominator='" + intToString(value.lightness.denominator) + "'"
                   : "")
                 + "/>";
        }

        template <>
        std::string xml (const colour::RGB<math::fraction>::value &pValue, bool small, const long &precision)
        {
            colour::RGB<math::fraction>::value value = pValue;
            value.red = math::numeric::round(value.red, precision);
            value.green = math::numeric::round(value.green, precision);
            value.blue = math::numeric::round(value.blue, precision);

            return std::string("<colour xmlns='http://colouri.se/2012' space='rgb'")
                 + (small ? " type='small'" : "")
                 + " red='" + intToString(value.red.numerator) + "'"
                 + (value.red.denominator != math::numeric::one()
                   ? " redDenominator='" + intToString(value.red.denominator) + "'"
                   : "")
                 + " green='" + intToString(value.green.numerator) + "'"
                 + (value.green.denominator != math::numeric::one()
                   ? " greenDenominator='" + intToString(value.green.denominator) + "'"
                   : "")
                 + " blue='" + intToString(value.blue.numerator) + "'"
                 + (value.blue.denominator != math::numeric::one()
                   ? " blueDenominator='" + intToString(value.blue.denominator) + "'"
                   : "")
                 + "/>";
        }

        template <class Q>
        std::string xmlpicker (const Q &value, const long &precision = 24)
        {
            throw "no conversion to XML known";
            return "";
        }

        static const int pickerResolution = 8;

        template <class Q>
        std::string xmlpicker3d (const typename Q::value &value, const long &precision = 24)
        {
            std::string s = "<picker xmlns='http://colouri.se/2012'>";
            typename Q::value v = value;
            if (v[0].denominator == math::numeric::zero())
            {
                v[0] = typename Q::scalar(0);
            }
            if (v[1].denominator == math::numeric::zero())
            {
                v[1] = typename Q::scalar(0);
            }
            if (v[2].denominator == math::numeric::zero())
            {
                v[2] = typename Q::scalar(0);
            }
            typename Q::scalar al = v[0] / typename Q::scalar(pickerResolution);
            typename Q::scalar ar = (typename Q::scalar(1) - v[0]) / typename Q::scalar(pickerResolution);
            typename Q::scalar bl = v[1] / typename Q::scalar(pickerResolution);
            typename Q::scalar br = (typename Q::scalar(1) - v[1]) / typename Q::scalar(pickerResolution);
            typename Q::scalar cl = v[2] / typename Q::scalar(pickerResolution);
            typename Q::scalar cr = (typename Q::scalar(1) - v[2]) / typename Q::scalar(pickerResolution);

            v[1] = typename Q::scalar(0);
            for (int y = -pickerResolution; y <= pickerResolution; y++)
            {
                s += "<set>";
                v[1] = (y < 0) ? (value[1] - bl * -y) : (value[1] + br * y);
                for (int x = -pickerResolution; x <= pickerResolution; x++)
                {
                    v[0] = (x < 0) ? (value[0] - al * -x) : (value[0] + ar * x);
                    s += xml (v, true, precision);
                }
                s += "</set>";
            }

            s += "<set>";
            v[0] = value[0];
            v[1] = value[1];
            for (int z = -pickerResolution; z <= pickerResolution; z++)
            {
                v[2] = (z < 0) ? (value[2] - cl * -z) : (value[2] + cr * z);
                s += xml (v, true, precision);
            }
            s += "</set>";

            return s += "</picker>";
        }

        template <>
        std::string xmlpicker (const colour::HSL<math::fraction>::value &value, const long &precision)
        {
            return xmlpicker3d<colour::HSL<math::fraction> > (value, precision);
        }

        template <>
        std::string xmlpicker (const colour::RGB<math::fraction>::value &value, const long &precision)
        {
            return xmlpicker3d<colour::RGB<math::fraction> > (value, precision);
        }
    };
};

#endif
