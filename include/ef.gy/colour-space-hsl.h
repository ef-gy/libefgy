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

#if !defined(EF_GY_COLOUR_SPACE_HSL_H)
#define EF_GY_COLOUR_SPACE_HSL_H

#include <ef.gy/colour-space-rgb.h>

namespace efgy
{
    namespace colour
    {
        template <typename Q>
        class HSLA;

        template <typename Q>
        class HSL : public space<Q,3>
        {
            public:
                using typename space<Q,3>::base;
                using typename space<Q,3>::scalar;

                class value : public space<Q,3>::value
                {
                    public:
                        typedef typename HSL<Q>::scalar scalar;

                        value ()
                            : space<Q,3>::value(),
                              hue((*this)[0]), saturation((*this)[1]), lightness((*this)[2])
                            {}
                        value (const scalar s[3])
                            : space<Q,3>::value(s),
                              hue((*this)[0]), saturation((*this)[1]), lightness((*this)[2])
                            {}
                        value (const typename HSL<Q>::value &v)
                            : space<Q,3>::value(),
                              hue((*this)[0]), saturation((*this)[1]), lightness((*this)[2])
                            {
                                hue        = v.hue;
                                saturation = v.saturation;
                                lightness  = v.lightness;
                            }
                        value (const typename HSLA<Q>::value &v)
                            : space<Q,3>::value(),
                              hue((*this)[0]), saturation((*this)[1]), lightness((*this)[2])
                            {
                                hue        = v.hue        * v.alpha;
                                saturation = v.saturation * v.alpha;
                                lightness  = v.lightness  * v.alpha;
                            }
                        value (const scalar &pHue, const scalar &pSaturation, const scalar &pLightness)
                            : space<Q,3>::value(),
                              hue((*this)[0]), saturation((*this)[1]), lightness((*this)[2])
                            {
                                hue        = pHue;
                                saturation = pSaturation;
                                lightness  = pLightness;
                            }

                        scalar &hue;
                        scalar &saturation;
                        scalar &lightness;

                        value (const typename RGB<Q>::value &v)
                            : space<Q,3>::value(),
                              hue((*this)[0]), saturation((*this)[1]), lightness((*this)[2])
                            {
                                scalar min = v.red;
                                scalar max = v.red;
                                if (v.green < min) { min = v.green; }
                                if (v.blue  < min) { min = v.blue;  }
                                if (v.green > max) { max = v.green; }
                                if (v.blue  > max) { max = v.blue;  }

                                scalar C = max - min;
                                scalar h = scalar(0);
                                if (max == v.red)
                                {
                                    h = (v.green - v.blue) / C;
                                }
                                else if (max == v.green)
                                {
                                    h = (v.blue - v.red) / C + scalar(2);
                                }
                                else if (max == v.blue)
                                {
                                    h = (v.red - v.green) / C + scalar(4);
                                }
                                while (h >= scalar(6))
                                {
                                    h -= scalar(6);
                                }

                                hue = h / scalar(6);
                                lightness = (max + min) / scalar(2);
                                if (C == math::numeric::zero())
                                {
                                    saturation = scalar(0);
                                }
                                else
                                {
                                    scalar a = scalar(2) * lightness - 1;
                                    if (a < math::numeric::zero())
                                    {
                                        a = -a;
                                    }
                                    saturation = C/(scalar(1) - a);
                                }
                            }

                        operator typename RGB<Q>::value (void) const
                        {
                            scalar a = scalar(2)*lightness - 1;
                            if (a < math::numeric::zero())
                            {
                                a = -a;
                            }

                            scalar chroma = (scalar(1) - a) * saturation;
                            scalar h = hue * scalar(6);
                            scalar b = h - scalar(1);
                            while (b >= scalar(2))
                            {
                                b -= scalar(2);
                            }
                            if (b < math::numeric::zero())
                            {
                                b = -b;
                            }

                            scalar x = chroma * (scalar(1) - b);
                            scalar r1, g1, b1;
                            if (h < scalar(1))
                            {
                                r1 = chroma;
                                g1 = x;
                            }
                            else if (h < scalar(2))
                            {
                                r1 = x;
                                g1 = chroma;
                            }
                            else if (h < scalar(3))
                            {
                                g1 = chroma;
                                b1 = x;
                            }
                            else if (h < scalar(4))
                            {
                                g1 = x;
                                b1 = chroma;
                            }
                            else if (h < scalar(5))
                            {
                                r1 = x;
                                b1 = chroma;
                            }
                            else if (h < scalar(6))
                            {
                                r1 = chroma;
                                b1 = x;
                            }

                            scalar m = lightness - chroma / scalar(2);

                            return typename RGB<Q>::value(r1 + m, g1 + m, b1 + m);
                        }
                };
        };

        template <typename Q>
        class HSLA : public space<Q,4>
        {
            public:
                using typename space<Q,4>::base;
                using typename space<Q,4>::scalar;

                class value : public space<Q,4>::value
                {
                    public:
                        typedef typename HSLA<Q>::scalar scalar;

                        value ()
                            : space<Q,4>::value(),
                              hue((*this)[0]), saturation((*this)[1]), lightness((*this)[2]), alpha((*this)[3])
                            {}
                        value (const scalar s[4])
                            : space<Q,4>::value(s),
                              hue((*this)[0]), saturation((*this)[1]), lightness((*this)[2]), alpha((*this)[3])
                            {}
                        value (const typename HSL<Q>::value &v)
                            : space<Q,4>::value(),
                              hue((*this)[0]), saturation((*this)[1]), lightness((*this)[2]), alpha((*this)[3])
                            {
                                hue        = v.hue;
                                saturation = v.saturation;
                                lightness  = v.lightness;
                                alpha      = scalar(1);
                            }
                        value (const scalar &pHue, const scalar &pSaturation, const scalar &pLightness)
                            : space<Q,4>::value(),
                              hue((*this)[0]), saturation((*this)[1]), lightness((*this)[2]), alpha((*this)[3])
                            {
                                hue        = pHue;
                                saturation = pSaturation;
                                lightness  = pLightness;
                                alpha      = scalar(1);
                            }
                        value (const scalar &pHue, const scalar &pSaturation, const scalar &pLightness, const scalar &pAlpha)
                            : space<Q,4>::value(),
                              hue((*this)[0]), saturation((*this)[1]), lightness((*this)[2]), alpha((*this)[3])
                            {
                                hue        = pHue;
                                saturation = pSaturation;
                                lightness  = pLightness;
                                alpha      = pAlpha;
                            }
                        value (const value &pV)
                            : space<Q,4>::value(pV),
                              hue((*this)[0]), saturation((*this)[1]), lightness((*this)[2]), alpha((*this)[3])
                            {
                            /*
                                hue        = pV.hue;
                                saturation = pV.saturation;
                                lightness  = pV.lightness;
                                alpha      = pV.alpha;
                            */
                            }

                        value &operator = (const value &pV)
                            {
                                hue        = pV.hue;
                                saturation = pV.saturation;
                                lightness  = pV.lightness;
                                alpha      = pV.alpha;
                                return *this;
                            }

                        scalar &hue;
                        scalar &saturation;
                        scalar &lightness;
                        scalar &alpha;

                        operator typename RGBA<Q>::value (void) const
                        {
                            scalar a = scalar(2)*lightness - 1;
                            if (a < math::numeric::zero())
                            {
                                a = -a;
                            }

                            scalar chroma = (scalar(1) - a) * saturation;
                            scalar h = hue * scalar(6);
                            scalar b = h - scalar(1);
                            while (b >= scalar(2))
                            {
                                b -= scalar(2);
                            }
                            if (b < math::numeric::zero())
                            {
                                b = -b;
                            }

                            scalar x = chroma * (scalar(1) - b);
                            scalar r1, g1, b1;
                            if (h < scalar(1))
                            {
                                r1 = chroma;
                                g1 = x;
                            }
                            else if (h < scalar(2))
                            {
                                r1 = x;
                                g1 = chroma;
                            }
                            else if (h < scalar(3))
                            {
                                g1 = chroma;
                                b1 = x;
                            }
                            else if (h < scalar(4))
                            {
                                g1 = x;
                                b1 = chroma;
                            }
                            else if (h < scalar(5))
                            {
                                r1 = x;
                                b1 = chroma;
                            }
                            else if (h < scalar(6))
                            {
                                r1 = chroma;
                                b1 = x;
                            }

                            scalar m = lightness - chroma / scalar(2);

                            return typename RGBA<Q>::value(r1 + m, g1 + m, b1 + m, alpha);
                        }
                };
        };
    };
};

#endif
