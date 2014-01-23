/**\file
 * \brief RGB colour vectors
 *
 * Contains colour vectors in the (s)RGB colour space. Currently only 3D and 4D
 * vectors are supported directly, which refer to RGB and RGBA vectors,
 * respectively.
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

#include <ef.gy/vector.h>

namespace efgy
{
    namespace math
    {
        namespace format
        {
            /**\brief RGB colour format tag
             *
             * This tag indicates that a vector is of the RGB(A) colour space;
             * since all vectors are tagged with a vector format tag class, this
             * makes it possible for vectors of different colour spaces to be
             * converted properly.
             */
            class RGB {};
        };

        template <typename Q>
        class vector<Q,3,format::RGB> : public std::array<Q,3>
        {
            public:
                vector (const std::array<Q,3> &v = {{}},
                        const format::RGB & = format::RGB())
                    : std::array<Q,3>(v),
                      red((*this)[0]), green((*this)[1]), blue((*this)[2])
                    {}
                vector (const Q &pRed, const Q &pGreen, const Q &pBlue)
                    : std::array<Q,3>({ pRed, pGreen, pBlue }),
                      red((*this)[0]), green((*this)[1]), blue((*this)[2])
                    {}

                Q &red;
                Q &green;
                Q &blue;
        };

        template <typename Q>
        class vector<Q,4,format::RGB> : public std::array<Q,4>
        {
            public:
                vector (const std::array<Q,4> &v = {{}},
                        const format::RGB & = format::RGB())
                    : std::array<Q,4>(v),
                      red((*this)[0]), green((*this)[1]), blue((*this)[2]), alpha((*this)[3])
                    {}
                vector (const Q &pRed, const Q &pGreen, const Q &pBlue, const Q &pAlpha = Q(1))
                    : std::array<Q,4>({ pRed, pGreen, pBlue, pAlpha }),
                      red((*this)[0]), green((*this)[1]), blue((*this)[2]), alpha((*this)[3])
                    {}

                Q &red;
                Q &green;
                Q &blue;
                Q &alpha;
        };
    };
};

#endif
