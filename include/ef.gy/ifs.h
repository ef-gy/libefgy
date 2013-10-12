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

#if !defined(EF_GY_IFS_H)
#define EF_GY_IFS_H

#include <ef.gy/polytope.h>

namespace efgy
{
    namespace geometry
    {
        template <typename Q, unsigned int od, typename render, unsigned int d = od>
        class ifs : public polytope<Q,d,4,render>
        {
            public:
                typedef polytope<Q,d,4,render> parent;

                ifs (render &pRenderer, const parameters<Q> &pParameter, const Q &pMultiplier = 1)
                    : parent(pRenderer, pParameter, pMultiplier)
                    {}

                using parent::parameter;
                using parent::renderWireframe;
                using parent::renderSolid;
                using parent::renderer;
                using parent::lines;
                using parent::faces;
        };

        namespace sierpinski
        {
            template <typename Q, unsigned int od, typename render, unsigned int d = od>
            class gasket : public ifs<Q,od,render,d>
            {
                public:
                    typedef ifs<Q,od,render,d> parent;

                    gasket(render &pRenderer, const parameters<Q> &pParameter, const Q &pMultiplier = 1)
                        : parent(pRenderer, pParameter, pMultiplier)
                        {
                            translation.data[0].data[0] = Q(0.25);

                            for (unsigned int i = 1; i < functions; i++)
                            {
                                translation.data[i].data[0] = Q(-0.25);
                                for (unsigned int j = 1; j < od; j++)
                                {
                                    const unsigned int k = i-1;
                                    const unsigned int l = j-1;
                                    const unsigned int b = 1 << l;
                                    const bool s = k & b;
                                    translation.data[i].data[j] = Q(s ? -0.25 : 0.25);
                                }
                            }

                            calculateObject();
                        }

                    using parent::parameter;
                    using parent::renderWireframe;
                    using parent::renderSolid;
                    using parent::renderer;
                    using parent::lines;
                    using parent::faces;

                    static unsigned int depth (void) { return od; }
                    static unsigned int renderDepth (void) { return d; }
                    static const char *id (void) { return "sierpinski-gasket"; }

                    static const unsigned int functions = (1<<(od-1))+1;
                    math::tuple<functions, typename euclidian::space<Q,d>::vector> translation;

                    void calculateObject (void)
                    {
                        cube<Q,od,render,d> source(parent::renderer, parameter, parent::precisionMultiplier);

                        lines = source.lines;
                        faces = source.faces;

                        for (unsigned int i = 0; i < parameter.iterations; i++)
                        {
                            decltype(lines) rlines;
                            decltype(faces) rfaces;

                            while (lines.size() > 0)
                            {
                                for (unsigned int j = 0; j < functions; j++)
                                {
                                    rlines.push_back(apply(j,lines.back()));
                                }
                                lines.pop_back();
                            }

                            while (faces.size() > 0)
                            {
                                for (unsigned int j = 0; j < functions; j++)
                                {
                                    rfaces.push_back(apply(j,faces.back()));
                                }
                                faces.pop_back();
                            }

                            lines = rlines;
                            faces = rfaces;
                        }
                    }

                    typename euclidian::space<Q,d>::vector apply
                        (const unsigned int &f,
                         const typename euclidian::space<Q,d>::vector &v)
                    {
                        typename euclidian::space<Q,d>::vector r = v * Q(0.5);

                        r = r + translation.data[f];

                        return r;
                    }

                    template<unsigned int fdim>
                    math::tuple<fdim,typename euclidian::space<Q,d>::vector> apply
                        (const unsigned int &f,
                         const math::tuple<fdim,typename euclidian::space<Q,d>::vector> &l)
                    {
                        math::tuple<fdim,typename euclidian::space<Q,d>::vector> r;

                        for (int i = 0; i < fdim; i++)
                        {
                            r.data[i] = apply (f, l.data[i]);
                        }

                        return r;
                    }
            };

            template <typename Q, unsigned int od, typename render, unsigned int d = od>
            class carpet : public ifs<Q,od,render,d>
            {
                public:
                    typedef ifs<Q,od,render,d> parent;

                    carpet(render &pRenderer, const parameters<Q> &pParameter, const Q &pMultiplier = 1)
                        : parent(pRenderer, pParameter, pMultiplier)
                        {
                            if (od > 1)
                            {
                                translation.data[0].data[0] = Q(-1)/Q(3);
                                translation.data[0].data[1] = Q(-1)/Q(3);
                                translation.data[1].data[0] = Q(-1)/Q(3);
                                translation.data[1].data[1] = Q(0);
                                translation.data[2].data[0] = Q(-1)/Q(3);
                                translation.data[2].data[1] = Q(1)/Q(3);
                                translation.data[3].data[0] = Q(1)/Q(3);
                                translation.data[3].data[1] = Q(-1)/Q(3);
                                translation.data[4].data[0] = Q(1)/Q(3);
                                translation.data[4].data[1] = Q(0);
                                translation.data[5].data[0] = Q(1)/Q(3);
                                translation.data[5].data[1] = Q(1)/Q(3);
                                translation.data[6].data[0] = Q(0);
                                translation.data[6].data[1] = Q(-1)/Q(3);
                                translation.data[7].data[0] = Q(0);
                                translation.data[7].data[1] = Q(1)/Q(3);
                            }
                            if (od > 2)
                            {
                                for (int i = 0; i < 8; i++)
                                {
                                    translation.data[(i+8)] = translation.data[i];
                                    translation.data[i].data[2] = Q(-1)/Q(3);
                                    translation.data[(i+8)].data[2] = Q(1)/Q(3);
                                }

                                translation.data[16].data[0] = Q(1)/Q(3);
                                translation.data[16].data[1] = Q(1)/Q(3);

                                translation.data[17].data[0] = Q(-1)/Q(3);
                                translation.data[17].data[1] = Q(1)/Q(3);

                                translation.data[18].data[0] = Q(1)/Q(3);
                                translation.data[18].data[1] = Q(-1)/Q(3);

                                translation.data[19].data[0] = Q(-1)/Q(3);
                                translation.data[19].data[1] = Q(-1)/Q(3);
                            }

                            calculateObject();
                        }

                    using parent::parameter;
                    using parent::renderWireframe;
                    using parent::renderSolid;
                    using parent::renderer;
                    using parent::lines;
                    using parent::faces;

                    static unsigned int depth (void) { return od; }
                    static unsigned int renderDepth (void) { return d; }
                    static const char *id (void) { return "sierpinski-carpet"; }

                    static const unsigned int functions = od == 2 ? 8 : 20;
                    math::tuple<functions, typename euclidian::space<Q,d>::vector> translation;

                    void calculateObject (void)
                    {
                        cube<Q,od,render,d> source(parent::renderer, parameter, parent::precisionMultiplier);

                        lines = source.lines;
                        faces = source.faces;

                        for (unsigned int i = 0; i < parameter.iterations; i++)
                        {
                            decltype(lines) rlines;
                            decltype(faces) rfaces;

                            while (lines.size() > 0)
                            {
                                for (unsigned int j = 0; j < functions; j++)
                                {
                                    rlines.push_back(apply(j,lines.back()));
                                }
                                lines.pop_back();
                            }

                            while (faces.size() > 0)
                            {
                                for (unsigned int j = 0; j < functions; j++)
                                {
                                    rfaces.push_back(apply(j,faces.back()));
                                }
                                faces.pop_back();
                            }

                            lines = rlines;
                            faces = rfaces;
                        }
                    }

                    typename euclidian::space<Q,d>::vector apply
                        (const unsigned int &f,
                         const typename euclidian::space<Q,d>::vector &v)
                    {
                        typename euclidian::space<Q,d>::vector r = v / Q(3);

                        r = r + translation.data[f];

                        return r;
                    }

                    template<unsigned int fdim>
                    math::tuple<fdim,typename euclidian::space<Q,d>::vector> apply
                        (const unsigned int &f,
                         const math::tuple<fdim,typename euclidian::space<Q,d>::vector> &l)
                    {
                        math::tuple<fdim,typename euclidian::space<Q,d>::vector> r;

                        for (int i = 0; i < fdim; i++)
                        {
                            r.data[i] = apply (f, l.data[i]);
                        }

                        return r;
                    }
            };
        };
    };
};

#endif
