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

#if !defined(EF_GY_RENDER_OPENGL_H)
#define EF_GY_RENDER_OPENGL_H

#include <ef.gy/euclidian.h>
#include <ef.gy/projection.h>

#undef GL3D
#define GL3D

namespace efgy
{
    namespace render
    {
        template<typename Q, unsigned int d>
        class opengl
        {
            public:
                opengl
                    (const geometry::transformation<Q,d> &pTransformation,
                     const geometry::projection<Q,d> &pProjection,
                     opengl<Q,d-1> &pLowerRenderer)
                    : transformation(pTransformation), projection(pProjection), lowerRenderer(pLowerRenderer)
                    {}

                void frameStart (void)
                {
                    combined = transformation * projection;
                    lowerRenderer.frameStart();
                };
                void frameEnd (void) const {};

                void drawLine
                    (const typename geometry::euclidian::space<Q,d>::vector &pA,
                     const typename geometry::euclidian::space<Q,d>::vector &pB) const;

                template<unsigned int q>
                void drawFace
                    (const math::tuple<q, typename geometry::euclidian::space<Q,d>::vector> &pV) const;

            protected:
                const geometry::transformation<Q,d> &transformation;
                const geometry::projection<Q,d> &projection;
                geometry::transformation<Q,d> combined;
                opengl<Q,d-1> &lowerRenderer;
        };

#if defined(GL3D)
        template<typename Q>
        class opengl<Q,3>
        {
            public:
                opengl
                    (const geometry::transformation<Q,3> &pTransformation,
                     const geometry::projection<Q,3> &,
                     const opengl<Q,2> &)
                    : transformation(pTransformation)
                    {}

                void frameStart (void) const
                {
                };
                void frameEnd (void) const {};

                void drawLine
                    (const typename geometry::euclidian::space<Q,3>::vector &pA,
                     const typename geometry::euclidian::space<Q,3>::vector &pB) const;

                template<unsigned int q>
                void drawFace
                    (const math::tuple<q, typename geometry::euclidian::space<Q,3>::vector> &pV) const;

            protected:
                const geometry::transformation<Q,3> &transformation;
        };
#endif

        template<typename Q>
        class opengl<Q,2>
        {
            public:
                opengl
                    (const geometry::transformation<Q,2> &pTransformation)
                    : transformation(pTransformation)
                    {}

                void frameStart (void) const {};
                void frameEnd (void) const {};

                void drawLine
                    (const typename geometry::euclidian::space<Q,2>::vector &pA,
                     const typename geometry::euclidian::space<Q,2>::vector &pB) const;

                template<unsigned int q>
                void drawFace
                    (const math::tuple<q, typename geometry::euclidian::space<Q,2>::vector> &pV) const;

            protected:
                const geometry::transformation<Q,2> &transformation;
        };

        template<typename Q, unsigned int d>
        void opengl<Q,d>::drawLine
            (const typename geometry::euclidian::space<Q,d>::vector &pA,
             const typename geometry::euclidian::space<Q,d>::vector &pB) const
        {
            typename geometry::euclidian::space<Q,d-1>::vector A = combined.project(pA);
            typename geometry::euclidian::space<Q,d-1>::vector B = combined.project(pB);

            lowerRenderer.drawLine(A, B);
        }

        template<typename Q, unsigned int d>
        template<unsigned int q>
        void opengl<Q,d>::drawFace
            (const math::tuple<q, typename geometry::euclidian::space<Q,d>::vector> &pV) const
        {
            math::tuple<q, typename geometry::euclidian::space<Q,d-1>::vector> V;

            for (unsigned int i = 0; i < q; i++)
            {
                V.data[i] = combined.project(pV.data[i]);
            }

            lowerRenderer.drawFace(V);
        }

#if defined(GL3D)
        template<typename Q>
        void opengl<Q,3>::drawLine
            (const typename geometry::euclidian::space<Q,3>::vector &pA,
             const typename geometry::euclidian::space<Q,3>::vector &pB) const
        {
            const typename geometry::euclidian::space<Q,3>::vector A = transformation * pA;
            const typename geometry::euclidian::space<Q,3>::vector B = transformation * pB;

            const GLfloat vertices[6] =
                { GLfloat(A.data[0]), GLfloat(A.data[1]), GLfloat(A.data[2]),
                  GLfloat(B.data[0]), GLfloat(B.data[1]), GLfloat(B.data[2]) };

            glNormalPointer(GL_FLOAT, 0, vertices);
            glVertexPointer(3, GL_FLOAT, 0, vertices);
            glDrawArrays(GL_LINES, 0, 2);
        }

        template<typename Q>
        template<unsigned int q>
        void opengl<Q,3>::drawFace
            (const math::tuple<q, typename geometry::euclidian::space<Q,3>::vector> &pV) const
        {
            GLfloat vertices[(q*3)];
            for (unsigned int i = 0; i < q; i++)
            {
                const typename geometry::euclidian::space<Q,3>::vector V = transformation * pV.data[i];

                vertices[(i*3)+0] = V.data[0];
                vertices[(i*3)+1] = V.data[1];
                vertices[(i*3)+2] = V.data[2];
            }
            glNormalPointer(GL_FLOAT, 0, vertices);
            glVertexPointer(3, GL_FLOAT, 0, vertices);
            glDrawArrays(GL_TRIANGLE_FAN, 0, q);
        }
#endif

        template<typename Q>
        void opengl<Q,2>::drawLine
            (const typename geometry::euclidian::space<Q,2>::vector &pA,
             const typename geometry::euclidian::space<Q,2>::vector &pB) const
        {
            const typename geometry::euclidian::space<Q,3>::vector A = transformation * pA;
            const typename geometry::euclidian::space<Q,3>::vector B = transformation * pB;

            const GLfloat vertices[6] =
                { GLfloat(A.data[0]), GLfloat(A.data[1]), 0,
                  GLfloat(B.data[0]), GLfloat(B.data[1]), 0 };

            glNormalPointer(GL_FLOAT, 0, vertices);
            glVertexPointer(3, GL_FLOAT, 0, vertices);
            glDrawArrays(GL_LINES, 0, 2);
        }

        template<typename Q>
        template<unsigned int q>
        void opengl<Q,2>::drawFace
            (const math::tuple<q, typename geometry::euclidian::space<Q,2>::vector> &pV) const
        {
            GLfloat vertices[(q*3)];
            for (unsigned int i = 0; i < q; i++)
            {
                const typename geometry::euclidian::space<Q,3>::vector V = transformation * pV.data[i];

                vertices[(i*3)+0] = V.data[0];
                vertices[(i*3)+1] = V.data[1];
                vertices[(i*3)+2] = 0;
            }
            glNormalPointer(GL_FLOAT, 0, vertices);
            glVertexPointer(3, GL_FLOAT, 0, vertices);
            glDrawArrays(GL_TRIANGLE_FAN, 0, q);
        }
    };
};

#endif
