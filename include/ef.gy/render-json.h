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

#if !defined(EF_GY_RENDER_JSON_H)
#define EF_GY_RENDER_JSON_H

#include <sstream>
#include <ef.gy/euclidian.h>
#include <ef.gy/projection.h>

namespace efgy
{
    namespace render
    {
        template<typename Q, unsigned int d>
        class json
        {
            public:
                json
                    (const geometry::perspectiveProjection<Q,d> &pProjection,
                     const json<Q,d-1> &pLowerRenderer)
                    : projection(pProjection), lowerRenderer(pLowerRenderer)
                    {}

                void drawLine
                    (const typename geometry::euclidian::space<Q,d>::vector &pA,
                     const typename geometry::euclidian::space<Q,d>::vector &pB) const;

            protected:
                const geometry::perspectiveProjection<Q,d> &projection;
                const json<Q,d-1> &lowerRenderer;
        };

        template<typename Q>
        class json<Q,2>
        {
            public:
                json(std::stringstream &pOutput)
                    : output(pOutput)
                    {}

                void drawLine
                    (const typename geometry::euclidian::space<Q,2>::vector &pA,
                     const typename geometry::euclidian::space<Q,2>::vector &pB) const;

            protected:
                std::stringstream &output;
        };

        template<typename Q, unsigned int d>
        void json<Q,d>::drawLine
            (const typename geometry::euclidian::space<Q,d>::vector &pA,
             const typename geometry::euclidian::space<Q,d>::vector &pB) const
        {
            typename geometry::euclidian::space<Q,d-1>::vector A;
            typename geometry::euclidian::space<Q,d-1>::vector B;

            A = projection.project(pA);
            B = projection.project(pB);

            lowerRenderer.drawLine(A, B);
        }

        template<typename Q>
        void json<Q,2>::drawLine
            (const typename geometry::euclidian::space<Q,2>::vector &pA,
             const typename geometry::euclidian::space<Q,2>::vector &pB) const
        {
            const double a0 = Q(pA.data[0]);
            const double a1 = Q(pA.data[1]);
            const double b0 = Q(pB.data[0]);
            const double b1 = Q(pB.data[1]);
            
            output << ", { 'type': 'line', 'x1' : " << a0 << ", 'x2' : " << b0 << ", 'y1' : " << a1 << ", 'y2' : " << b1 << " }";
        }
    };
};

#endif
