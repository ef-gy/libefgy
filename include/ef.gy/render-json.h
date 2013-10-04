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
                    (const geometry::transformation<Q,d> &pTransformation,
                     const geometry::perspectiveProjection<Q,d> &pProjection,
                     json<Q,d-1> &pLowerRenderer)
                    : transformation(pTransformation), projection(pProjection), lowerRenderer(pLowerRenderer)
                    {}

                void drawLine
                    (const typename geometry::euclidian::space<Q,d>::vector &pA,
                     const typename geometry::euclidian::space<Q,d>::vector &pB) const;

                template<unsigned int q>
                void drawFace
                    (const math::tuple<q, typename geometry::euclidian::space<Q,d>::vector> &pV) const;

            protected:
                const geometry::transformation<Q,d> &transformation;
                const geometry::perspectiveProjection<Q,d> &projection;
                json<Q,d-1> &lowerRenderer;
        };

        template<typename Q>
        class json<Q,2>
        {
            public:
                json
                    (const typename geometry::transformation<Q,2> &pTransformation)
                    : transformation(pTransformation)
                    {}

                void drawLine
                    (const typename geometry::euclidian::space<Q,2>::vector &pA,
                     const typename geometry::euclidian::space<Q,2>::vector &pB);

                template<unsigned int q>
                void drawFace
                    (const math::tuple<q, typename geometry::euclidian::space<Q,2>::vector> &pV);

                void reset()
                {
                    output.str("");
                    previousX = Q();
                    previousY = Q();
                }

                std::stringstream output;

            protected:
                const geometry::transformation<Q,2> &transformation;
                Q previousX, previousY;
        };

        template<typename Q, unsigned int d>
        void json<Q,d>::drawLine
            (const typename geometry::euclidian::space<Q,d>::vector &pA,
             const typename geometry::euclidian::space<Q,d>::vector &pB) const
        {
            typename geometry::euclidian::space<Q,d-1>::vector A;
            typename geometry::euclidian::space<Q,d-1>::vector B;

            A = projection.project(transformation * pA);
            B = projection.project(transformation * pB);

            lowerRenderer.drawLine(A, B);
        }

        template<typename Q, unsigned int d>
        template<unsigned int q>
        void json<Q,d>::drawFace
            (const math::tuple<q, typename geometry::euclidian::space<Q,d>::vector> &pV) const
        {
            math::tuple<q, typename geometry::euclidian::space<Q,d-1>::vector> V;

            for (unsigned int i = 0; i < q; i++)
            {
                V.data[i] = projection.project(transformation * pV.data[i]);
            }

            lowerRenderer.drawFace(V);
        }

        template<typename Q>
        void json<Q,2>::drawLine
            (const typename geometry::euclidian::space<Q,2>::vector &pA,
             const typename geometry::euclidian::space<Q,2>::vector &pB)
        {
            std::stringstream sbuf1;
            std::stringstream sbuf2;

            const typename geometry::euclidian::space<Q,2>::vector &A = transformation * pA;
            const typename geometry::euclidian::space<Q,2>::vector &B = transformation * pB;

            const double a0 = -Q(A.data[0]);
            const double a1 = -Q(A.data[1]);
            const double b0 = -Q(B.data[0]);
            const double b1 = -Q(B.data[1]);

            const double a0r = a0 - previousX;
            const double a1r = a1 - previousY;
            const double b0r = b0 - a0;
            const double b1r = b1 - a1;

            char s[128];
            char sr[128];
            if ((a0 == previousX) && (a1 == previousY))
            {
                if (B.data[1] == A.data[1])
                {
                    sbuf1 << "H" << b0;
                    sbuf2 << "h" << b0r;
                }
                else if (B.data[0] == A.data[0])
                {
                    sbuf1 << "V" << b1;
                    sbuf2 << "v" << b1r;
                }
                else
                {
                    sbuf1 << "L" << b0 << "," << b1;
                    sbuf2 << "l" << b0r << "," << b1r;
                }
            }
            else
            {
                sbuf1 << "M" << a0 << "," << a1;
                sbuf2 << "m" << a0r << "," << a1r;
                if (sbuf1.str().size() >= sbuf2.str().size())
                {
                    output << sbuf2.str();
                }
                else
                {
                    output << sbuf1.str();
                }
                sbuf1.str("");
                sbuf2.str("");

                if (B.data[1] == A.data[1])
                {
                    sbuf1 << "H" << b0;
                    sbuf2 << "h" << b0r;
                }
                else if (B.data[0] == A.data[0])
                {
                    sbuf1 << "V" << b1;
                    sbuf2 << "v" << b1r;
                }
                else
                {
                    sbuf1 << "L" << b0 << "," << b1;
                    sbuf2 << "l" << b0r << "," << b1r;
                }
            }
            if (sbuf1.str().size() >= sbuf2.str().size())
            {
                output << sbuf2.str();
            }
            else
            {
                output << sbuf1.str();
            }
            previousX = b0;
            previousY = b1;
        }

        template<typename Q>
        template<unsigned int q>
        void json<Q,2>::drawFace
            (const math::tuple<q, typename geometry::euclidian::space<Q,2>::vector> &pV)
        {
            output << ",'";
            for (unsigned int i = 0; i < q; i++)
            {
                std::stringstream sbuf1;
                std::stringstream sbuf2;

                const typename geometry::euclidian::space<Q,2>::vector V = transformation * pV.data[i];

                const double a0 = -Q(V.data[0]);
                const double a1 = -Q(V.data[1]);

                if (i == 0)
                {
                    sbuf1 << "M" << a0 << "," << a1;
                    sbuf2 << "M" << a0 << "," << a1;
                }
                else
                {
                    const typename geometry::euclidian::space<Q,2>::vector V1 = transformation * pV.data[(i-1)];

                    const double a0r = a0 + Q(V1.data[0]);
                    const double a1r = a1 + Q(V1.data[1]);

                    if (pV.data[i].data[1] == V1.data[1])
                    {
                        sbuf1 << "H" << a0;
                        sbuf2 << "h" << a0r;
                    }
                    else if (pV.data[i].data[0] == V1.data[0])
                    {
                        sbuf1 << "V" << a1;
                        sbuf2 << "v" << a1r;
                    }
                    else
                    {
                        sbuf1 << "L" << a0 << "," << a1;
                        sbuf2 << "l" << a0r << "," << a1r;
                    }
                }
                if (sbuf1.str().size() >= sbuf2.str().size())
                {
                    output << sbuf2.str();
                }
                else
                {
                    output << sbuf1.str();
                }
            }
            output << "Z'";
        }
    };
};

#endif
