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

#if !defined(EF_GY_PARAMETRIC_H)
#define EF_GY_PARAMETRIC_H

#include <ef.gy/polytope.h>

namespace efgy
{
    namespace geometry
    {
        template <typename Q, unsigned int d, unsigned int f, typename render>
        class parametric : public polytope<Q,d,f,render>
        {
            public:
                typedef polytope<Q,d,f,render> parent;

                parametric (render &pRenderer, const parameters<Q> &pParameter, const Q &pMultiplier = 1)
                    : parent(pRenderer, pParameter, pMultiplier)
                    {}

                using parent::precisionMultiplier;
                using parent::parameter;
                using parent::renderWireframe;
                using parent::renderSolid;
                using parent::renderer;
                using parent::lines;
                using parent::faces;

                using parent::modelDimensionMinimum;
                using parent::modelDimensionMaximum;
                using parent::renderDimensionMinimum;
                using parent::renderDimensionMaximum;
        };

        template <typename Q, unsigned int od, typename render, unsigned int d = 3>
        class moebiusStrip : public parametric<Q,d,4,render>
        {
            public:
                typedef parametric<Q,d,4,render> parent;

                moebiusStrip (render &pRenderer, const parameters<Q> &pParameter, const Q &pMultiplier = 1)
                    : parent(pRenderer, pParameter, pMultiplier)
                    {
                        calculateObject();
                    }

                using parent::precisionMultiplier;
                using parent::parameter;
                using parent::renderWireframe;
                using parent::renderSolid;
                using parent::renderer;
                using parent::lines;
                using parent::faces;

                using parent::modelDimensionMinimum;
                static const unsigned int modelDimensionMaximum = 2;
                using parent::renderDimensionMinimum;
                using parent::renderDimensionMaximum;

                static unsigned int depth (void) { return od; }
                static unsigned int renderDepth (void) { return d; }
                static const char *id (void) { return "moebius-strip"; }

                void calculateObject(void)
                {
                    Q radius = parameter.polarRadius;
                    Q precision = parameter.polarPrecision * precisionMultiplier;

                    usedRadius = radius;
                    usedPrecision = precision;

                    precision = ceil(precision);
                    const Q stepU = Q(M_PI/(precision*Q(2)));
                    const Q stepV = Q((2*radius)/precision);

                    lines = std::vector<math::tuple<2,typename euclidian::space<Q,d>::vector> >();
                    faces = std::vector<math::tuple<4,typename euclidian::space<Q,d>::vector> >();

                    for (Q u = 0; u < Q(2*M_PI); u += stepU)
                    {
                        for (Q v = -radius; v <= radius; v += stepV)
                        {
                            Q un = (u + stepU) <= Q(2*M_PI) ? (u + stepU) : Q(2*M_PI);
                            Q vn = (v + stepV) <= Q(radius) ? (v + stepV) : radius;

                            const typename euclidian::space<Q,d>::vector A = getCoordinates(radius, u, v);
                            const typename euclidian::space<Q,d>::vector B = getCoordinates(radius, un, v);

                            math::tuple<2,typename euclidian::space<Q,d>::vector> newLine;

                            newLine.data[0] = A;
                            newLine.data[1] = B;

                            lines.push_back(newLine);

                            const typename euclidian::space<Q,d>::vector C = getCoordinates(radius, u, vn);
                                newLine.data[1] = C;

                            lines.push_back(newLine);

                            const typename euclidian::space<Q,d>::vector D = getCoordinates(radius, un, vn);

                            math::tuple<4,typename euclidian::space<Q,d>::vector> newFace;

                            newFace.data[0] = A;
                            newFace.data[1] = B;
                            newFace.data[2] = D;
                            newFace.data[3] = C;

                            faces.push_back(newFace);

                            if ((v + stepV) > vn)
                            {
                                newLine.data[0] = C;
                                newLine.data[1] = D;

                                lines.push_back(newLine);
                            }
                        }
                    }
                }

                Q getCoordinate (unsigned int i, Q r, Q u, Q v) const
                {
                    switch (i)
                    {
                        case 0:
                            return (r + v/Q(2) * cos(u/Q(2))) * cos(u);
                        case 1:
                            return (r + v/Q(2) * cos(u/Q(2))) * sin(u);
                        case 2:
                            return v/Q(2) * sin(u/Q(2));
                    }
                    return Q(0);
                }

                typename euclidian::space<Q,d>::vector getCoordinates (Q r, Q u, Q v) const
                {
                    typename euclidian::space<Q,d>::vector res;

                    res.data[0] = getCoordinate(0, r, u, v);
                    res.data[1] = getCoordinate(1, r, u, v);
                    res.data[2] = getCoordinate(2, r, u, v);

                    return res;
                }

                Q usedRadius;
                Q usedPrecision;
        };

        template <typename Q, unsigned int od, typename render, unsigned int d = 3>
        class kleinBagel : public parametric<Q,d,4,render>
        {
            public:
                typedef parametric<Q,d,4,render> parent;

                kleinBagel (render &pRenderer, const parameters<Q> &pParameter, const Q &pMultiplier = 1)
                    : parent(pRenderer, pParameter, pMultiplier)
                    {
                        calculateObject();
                    }

                using parent::precisionMultiplier;
                using parent::parameter;
                using parent::renderWireframe;
                using parent::renderSolid;
                using parent::renderer;
                using parent::lines;
                using parent::faces;

                using parent::modelDimensionMinimum;
                static const unsigned int modelDimensionMaximum = 2;
                using parent::renderDimensionMinimum;
                using parent::renderDimensionMaximum;

                static unsigned int depth (void) { return od; }
                static unsigned int renderDepth (void) { return d; }
                static const char *id (void) { return "klein-bagel"; }

                void calculateObject(void)
                {
                    Q radius = parameter.polarRadius;
                    Q precision = parameter.polarPrecision * precisionMultiplier;

                    usedRadius = radius;
                    usedPrecision = precision;

                    precision = ceil(precision*Q(2));
                    const Q stepU = Q(M_PI/(precision));
                    const Q stepV = stepU;

                    lines = std::vector<math::tuple<2,typename euclidian::space<Q,d>::vector> >();
                    faces = std::vector<math::tuple<4,typename euclidian::space<Q,d>::vector> >();

                    for (Q u = 0; u < Q(2*M_PI); u += stepU)
                    {
                        for (Q v = 0; v <= Q(2*M_PI); v += stepV)
                        {
                            Q un = (u + stepU) <= Q(2*M_PI) ? (u + stepU) : Q(2*M_PI);
                            Q vn = (v + stepV) <= Q(2*M_PI) ? (v + stepV) : Q(2*M_PI);

                            const typename euclidian::space<Q,d>::vector A = getCoordinates(radius, u, v);
                            const typename euclidian::space<Q,d>::vector B = getCoordinates(radius, un, v);

                            math::tuple<2,typename euclidian::space<Q,d>::vector> newLine;

                            newLine.data[0] = A;
                            newLine.data[1] = B;

                            lines.push_back(newLine);

                            const typename euclidian::space<Q,d>::vector C = getCoordinates(radius, u, vn);
                            newLine.data[1] = C;

                            lines.push_back(newLine);

                            const typename euclidian::space<Q,d>::vector D = getCoordinates(radius, un, vn);

                            math::tuple<4,typename euclidian::space<Q,d>::vector> newFace;

                            newFace.data[0] = A;
                            newFace.data[1] = B;
                            newFace.data[2] = D;
                            newFace.data[3] = C;

                            faces.push_back(newFace);

                            if ((v + stepV) > vn)
                            {
                                newLine.data[0] = C;
                                newLine.data[1] = D;

                                lines.push_back(newLine);
                            }
                        }
                    }
                }

                Q getCoordinate (unsigned int i, Q r, Q u, Q v) const
                {
                    switch (i)
                    {
                        case 0:
                            return (r + cos(u/Q(2))*sin(v) - sin(u/Q(2))*sin(Q(2)*v)) * cos(u);
                        case 1:
                            return (r + cos(u/Q(2))*sin(v) - sin(u/Q(2))*sin(Q(2)*v)) * sin(u);
                        case 2:
                            return sin(u/Q(2))*sin(v) - cos(u/Q(2))*sin(Q(2)*v);
                    }
                    return Q(0);
                }

                typename euclidian::space<Q,d>::vector getCoordinates (Q r, Q u, Q v) const
                {
                    typename euclidian::space<Q,d>::vector res;

                    res.data[0] = getCoordinate(0, r, u, v);
                    res.data[1] = getCoordinate(1, r, u, v);
                    res.data[2] = getCoordinate(2, r, u, v);

                    return res;
                }

                Q usedRadius;
                Q usedPrecision;
        };
    };
};

#endif
