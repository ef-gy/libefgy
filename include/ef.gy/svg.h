/**\file
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

#if !defined(EF_GY_SVG_H)
#define EF_GY_SVG_H

#include <ef.gy/render.h>
#include <ef.gy/css.h>

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
        std::string svg (const T &o)
        {
            return std::string("<!-- unknown object, size=") + intToString(sizeof(T)) + std::string(" -->");
        }

        template <>
        std::string svg (const long double &f)
        {
            char buffer[128];
            snprintf (buffer, 128, "%Lf", f);
            return std::string(buffer);
        }

        template <>
        std::string svg (const math::Q &f)
        {
            //return svg((long double)f.numerator / (long double)f.denominator);
            //return std::string(f);
            return svg(f.toDouble());
        }

        template <>
        std::string svg (const geometry::euclidian::Q2::scalar &f)
        {
            return svg(math::Q(f));
        }

        template<>
        std::string svg (const geometry::euclidian::Q2::vector &o)
        {
            return std::string("<circle cx=\"") + svg(o[0]) + std::string("\" cy=\"") + svg(o[1]) + std::string("\" r=\"4\" stroke=\"black\" fill=\"pink\"/>");
        }

        template<>
        std::string svg (const geometry::line<geometry::euclidian::Q2> &o)
        {
            return std::string("<line x1=\"") + svg(o.A[0]) + std::string("\" y1=\"") + svg(o.A[1])
                 + std::string("x2=\"")       + svg(o.B[0]) + std::string("\" y2=\"") + svg(o.B[1])
                 + std::string("\" stroke=\"black\" fill=\"none\"/>");
        }

        template<>
        std::string svg (const typename colour::HSLA<math::Q>::value &c)
        {
            return std::string("hsla(") + svg(c.hue * math::Q(360)) + std::string(",") + svg(c.saturation * math::Q(100)) + std::string("%,") + svg(c.lightness * math::Q(100)) + std::string("%,") + svg(c.alpha * math::Q(1)) + std::string(")");
        }

        template<>
        std::string svg (const geometry::polygon<geometry::euclidian::Q2> &o)
        {
            std::string r = std::string("<polygon points=\"");

            for (unsigned int i = 0; i < o.data.size(); i++)
            {
                if (i != 0)
                {
                    r += std::string(" ");
                }

                r += svg(o.data[i][0]) + std::string(",") + svg(o.data[i][1]);
            }

            return r + std::string("\" stroke=\"black\" fill=\"") + svg(o.colour) + std::string("\"/>");
        }

        template<unsigned int n>
        std::string svg (const geometry::ngon<geometry::euclidian::Q2, n> &o)
        {
            std::string r = std::string("<polygon points=\"");

            for (unsigned int i = 0; i < n; i++)
            {
                if (i != 0)
                {
                    r += std::string(" ");
                }

                r += svg(o.data[i].data[0]) + std::string(",") + svg(o.data[i].data[1]);
            }

            return r + std::string("\" fill=\"") + svg(o.colour) + std::string("\"/>");
        }

        template<typename S>
        std::string svg (const geometry::voronoi<S> &o)
        {
            std::string r;

            for (unsigned int i = 0; i < o.cells.data.size(); i++)
            {
                r += svg(geometry::polygon<S>(o.cells.data[i]));
            }

            return r;
        }
    };
};

#endif
