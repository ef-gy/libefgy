/**\file
 * \deprecated Will be removed in libefgy-5
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

#if !defined(EF_GY_CSS_H)
#define EF_GY_CSS_H

#include <ef.gy/render.h>

#include <stdio.h>
#include <ef.gy/geometry.h>
#include <ef.gy/voronoi.h>
#include <ef.gy/colour-space-hsl.h>
#include <ef.gy/string.h>

namespace efgy
{
    namespace render
    {
        template <typename T>
        std::string css (const T &o)
        {
            return std::string("/* unknown object, size=") + intToString(sizeof(T)) + std::string(" */");
        }

        template <>
        std::string css (const long double &f)
        {
            char buffer[128];
            snprintf (buffer, 128, "%Lf", f);
            return std::string(buffer);
        }

        template <>
        std::string css (const math::Q &f)
        {
            //return css((long double)f.numerator / (long double)f.denominator);
            //return std::string(f);
            return css(f.toDouble());
        }

        template <>
        std::string css (const geometry::euclidian::Q2::scalar &f)
        {
            return css(math::Q(f));
        }

        template<>
        std::string css (const typename colour::HSLA<math::Q>::value &c)
        {
            return std::string("hsla(") + css(c.hue * math::Q(360)) + std::string(",") + css(c.saturation * math::Q(100)) + std::string("%,") + css(c.lightness * math::Q(100)) + std::string("%,") + css(c.alpha * math::Q(1)) + std::string(")");
        }
    };
};

#endif
