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

#if !defined(EF_GY_COLOUR_SPACE_RGB_H)
#define EF_GY_COLOUR_SPACE_RGB_H

#include <ef.gy/colour-space.h>

namespace efgy
{
    namespace colour
    {
        template <typename Q>
        class RGBA;

        template <typename Q>
        class RGB : public space<Q,3>
        {
            public:
                using typename space<Q,3>::base;
                using typename space<Q,3>::scalar;

                class value : public space<Q,3>::value
                {
                    public:
                        typedef typename RGB<Q>::scalar scalar;
                        using space<Q,3>::value::data;

                        value ()
                            : space<Q,3>::value(),
                              red((*this)[0]), green((*this)[1]), blue((*this)[2])
                            {}
                        value (const scalar s[3])
                            : space<Q,3>::value(s),
                              red((*this)[0]), green((*this)[1]), blue((*this)[2])
                            {}
                        value (const typename RGB<Q>::value &v)
                            : space<Q,3>::value(),
                              red((*this)[0]), green((*this)[1]), blue((*this)[2])
                            {
                                red   = v.red;
                                green = v.green;
                                blue  = v.blue;
                            }
                        value (const typename RGBA<Q>::value &v)
                            : space<Q,3>::value(),
                              red((*this)[0]), green((*this)[1]), blue((*this)[2])
                            {
                                red   = v.red   * v.alpha;
                                green = v.green * v.alpha;
                                blue  = v.blue  * v.alpha;
                            }
                        value (const scalar &pRed, const scalar &pGreen, const scalar &pBlue)
                            : space<Q,3>::value(),
                              red((*this)[0]), green((*this)[1]), blue((*this)[2])
                            {
                                red   = pRed;
                                green = pGreen;
                                blue  = pBlue;
                            }

                        scalar &red;
                        scalar &green;
                        scalar &blue;
                };
        };

        template <typename Q>
        class RGBA : public space<Q,4>
        {
            public:
                using typename space<Q,4>::base;
                using typename space<Q,4>::scalar;

                class value : public space<Q,4>::value
                {
                    public:
                        typedef typename RGBA<Q>::scalar scalar;
                        using space<Q,4>::vector::data;

                        value ()
                            : space<Q,4>::value(),
                              red((*this)[0]), green((*this)[1]), blue((*this)[2]), alpha((*this)[3])
                            {}
                        value (const scalar s[4])
                            : space<Q,4>::value(s),
                              red((*this)[0]), green((*this)[1]), blue((*this)[2]), alpha((*this)[3])
                            {}
                        value (const typename RGB<Q>::value &v)
                            : space<Q,4>::value(),
                              red((*this)[0]), green((*this)[1]), blue((*this)[2]), alpha((*this)[3])
                            {
                                red   = v.red;
                                green = v.green;
                                blue  = v.blue;
                                alpha = scalar(1);
                            }
                        value (const scalar &pRed, const scalar &pGreen, const scalar &pBlue)
                            : space<Q,4>::value(),
                              red((*this)[0]), green((*this)[1]), blue((*this)[2]), alpha((*this)[3])
                            {
                                red   = pRed;
                                green = pGreen;
                                blue  = pBlue;
                                alpha = scalar(1);
                            }
                        value (const scalar &pRed, const scalar &pGreen, const scalar &pBlue, const scalar &pAlpha)
                            : space<Q,4>::value(),
                              red((*this)[0]), green((*this)[1]), blue((*this)[2]), alpha((*this)[3])
                            {
                                red   = pRed;
                                green = pGreen;
                                blue  = pBlue;
                                alpha = pAlpha;
                            }

                        scalar &red;
                        scalar &green;
                        scalar &blue;
                        scalar &alpha;
                };
        };
    };
};

#endif
