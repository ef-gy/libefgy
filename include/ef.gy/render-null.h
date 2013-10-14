/*
 * This file is part of the ef.gy project.
 * See the appropriate repository at http://ef.gy/.git for exact file
 * modification records.
*/

/*
 * Copyright (c) 2012-2013, ef.gy Project Members
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

#if !defined(EF_GY_RENDER_NULL_H)
#define EF_GY_RENDER_NULL_H

#include <ef.gy/euclidian.h>
#include <ef.gy/projection.h>

namespace efgy
{
    namespace render
    {
        template<typename Q, unsigned int d>
        class null
        {
            public:
                null
                    (const geometry::transformation<Q,d> &pTransformation,
                     const geometry::projection<Q,d> &pProjection,
                     null<Q,d-1> &pLowerRenderer)
                    : transformation(pTransformation), projection(pProjection), lowerRenderer(pLowerRenderer)
                    {}

                void frameStart (void) const {};
                void frameEnd (void) const {};

                void drawLine
                    (const typename geometry::euclidian::space<Q,d>::vector &pA,
                     const typename geometry::euclidian::space<Q,d>::vector &pB) const {}

                template<unsigned int q>
                void drawFace
                    (const math::tuple<q, typename geometry::euclidian::space<Q,d>::vector> &pV) const {}

                void reset (void) const {}

            protected:
                const geometry::transformation<Q,d> &transformation;
                const geometry::projection<Q,d> &projection;
                geometry::transformation<Q,d> combined;
                null<Q,d-1> &lowerRenderer;
        };

        template<typename Q>
        class null<Q,2>
        {
            public:
                null
                    (const typename geometry::transformation<Q,2> &pTransformation)
                    : transformation(pTransformation)
                    {}

                void frameStart (void) const {};
                void frameEnd (void) const {};

                void drawLine
                    (const typename geometry::euclidian::space<Q,2>::vector &pA,
                     const typename geometry::euclidian::space<Q,2>::vector &pB) const {}

                template<unsigned int q>
                void drawFace
                    (const math::tuple<q, typename geometry::euclidian::space<Q,2>::vector> &pV) const {}

                void reset (void) const {}

            protected:
                const geometry::transformation<Q,2> &transformation;
                Q previousX, previousY;
        };
    };
};

#endif
