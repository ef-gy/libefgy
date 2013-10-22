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
#include <ef.gy/projection.h>
#include <vector>
#include <cstdlib>

namespace efgy
{
    namespace geometry
    {
        template <typename Q, unsigned int od, typename render, unsigned int d,
                  template <class,unsigned int,class,unsigned int> class primitive,
                  unsigned int pd,
                  template <class,unsigned int> class trans>
        class ifs : public polytope<Q,d,primitive<Q,pd,render,d>::faceVertices,render>
        {
            public:
                typedef polytope<Q,d,primitive<Q,pd,render,d>::faceVertices,render> parent;

                ifs (render &pRenderer, const parameters<Q> &pParameter, const Q &pMultiplier = 1)
                    : parent(pRenderer, pParameter, pMultiplier)
                    {}

                using parent::parameter;
                using parent::renderSolid;
                using parent::renderer;
                using parent::lines;
                using parent::faces;

                using parent::modelDimensionMinimum;
                using parent::modelDimensionMaximum;
                using parent::renderDimensionMinimum;
                using parent::renderDimensionMaximum;

                using parent::faceVertices;

                std::vector<trans<Q,d> > functions;

                void calculateObject (void)
                {
                    primitive<Q,pd,render,d> source(parent::renderer, parameter, parent::precisionMultiplier);
                    
                    lines = source.lines;
                    faces = source.faces;
                    
                    for (unsigned int i = 0; i < parameter.iterations; i++)
                    {
                        decltype(lines) rlines;
                        decltype(faces) rfaces;
                        
                        while (lines.size() > 0)
                        {
                            for (unsigned int j = 0; j < functions.size(); j++)
                            {
                                rlines.push_back(apply(j,lines.back()));
                            }
                            lines.pop_back();
                        }
                        
                        while (faces.size() > 0)
                        {
                            for (unsigned int j = 0; j < functions.size(); j++)
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
                    return functions[f] * v;
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

        namespace sierpinski
        {
            template <typename Q, unsigned int od, typename render, unsigned int d = od>
            class gasket : public ifs<Q,od,render,d,cube,od,transformation::affine>
            {
                public:
                    typedef ifs<Q,od,render,d,cube,od,transformation::affine> parent;

                    gasket(render &pRenderer, const parameters<Q> &pParameter, const Q &pMultiplier = 1)
                        : parent(pRenderer, pParameter, pMultiplier)
                        {
                            const unsigned int nfunctions = (1<<(od-1))+1;
                            math::tuple<nfunctions, typename euclidian::space<Q,d>::vector> translations;

                            translations.data[0].data[0] = Q(0.25);

                            for (unsigned int i = 1; i < nfunctions; i++)
                            {
                                translations.data[i].data[0] = Q(-0.25);
                                for (unsigned int j = 1; j < od; j++)
                                {
                                    const unsigned int k = i-1;
                                    const unsigned int l = j-1;
                                    const unsigned int b = 1 << l;
                                    const bool s = k & b;
                                    translations.data[i].data[j] = Q(s ? -0.25 : 0.25);
                                }
                            }

                            for (unsigned int i = 0; i < nfunctions; i++)
                            {
                                functions.push_back(transformation::scale<Q,d>(0.5) * transformation::translation<Q,d>(translations.data[i]));
                            }

                            calculateObject();
                        }

                    using parent::parameter;
                    using parent::renderSolid;
                    using parent::renderer;
                    using parent::lines;
                    using parent::faces;

                    using parent::modelDimensionMinimum;
                    static const unsigned int modelDimensionMaximum = d;
                    using parent::renderDimensionMinimum;
                    using parent::renderDimensionMaximum;

                    using parent::faceVertices;

                    using parent::functions;
                    using parent::calculateObject;

                    static unsigned int depth (void) { return od; }
                    static unsigned int renderDepth (void) { return d; }
                    static const char *id (void) { return "sierpinski-gasket"; }
            };

            template <typename Q, unsigned int od, typename render, unsigned int d = od>
            class carpet : public ifs<Q,od,render,d,cube,od,transformation::affine>
            {
                public:
                    typedef ifs<Q,od,render,d,cube,od,transformation::affine> parent;

                    carpet(render &pRenderer, const parameters<Q> &pParameter, const Q &pMultiplier = 1)
                        : parent(pRenderer, pParameter, pMultiplier)
                        {
                            const unsigned int nfunctions = od == 2 ? 8 : 20;
                            math::tuple<nfunctions, typename euclidian::space<Q,d>::vector> translations;

                            if (od > 1)
                            {
                                translations.data[0].data[0] = Q(-1)/Q(3);
                                translations.data[0].data[1] = Q(-1)/Q(3);
                                translations.data[1].data[0] = Q(-1)/Q(3);
                                translations.data[1].data[1] = Q(0);
                                translations.data[2].data[0] = Q(-1)/Q(3);
                                translations.data[2].data[1] = Q(1)/Q(3);
                                translations.data[3].data[0] = Q(1)/Q(3);
                                translations.data[3].data[1] = Q(-1)/Q(3);
                                translations.data[4].data[0] = Q(1)/Q(3);
                                translations.data[4].data[1] = Q(0);
                                translations.data[5].data[0] = Q(1)/Q(3);
                                translations.data[5].data[1] = Q(1)/Q(3);
                                translations.data[6].data[0] = Q(0);
                                translations.data[6].data[1] = Q(-1)/Q(3);
                                translations.data[7].data[0] = Q(0);
                                translations.data[7].data[1] = Q(1)/Q(3);
                            }
                            if (od > 2)
                            {
                                for (int i = 0; i < 8; i++)
                                {
                                    translations.data[(i+8)] = translations.data[i];
                                    translations.data[i].data[2] = Q(-1)/Q(3);
                                    translations.data[(i+8)].data[2] = Q(1)/Q(3);
                                }

                                translations.data[16].data[0] = Q(1)/Q(3);
                                translations.data[16].data[1] = Q(1)/Q(3);

                                translations.data[17].data[0] = Q(-1)/Q(3);
                                translations.data[17].data[1] = Q(1)/Q(3);

                                translations.data[18].data[0] = Q(1)/Q(3);
                                translations.data[18].data[1] = Q(-1)/Q(3);

                                translations.data[19].data[0] = Q(-1)/Q(3);
                                translations.data[19].data[1] = Q(-1)/Q(3);
                            }

                            for (unsigned int i = 0; i < nfunctions; i++)
                            {
                                functions.push_back(transformation::scale<Q,d>(Q(1)/Q(3)) * transformation::translation<Q,d>(translations.data[i]));
                            }

                            calculateObject();
                        }

                    using parent::parameter;
                    using parent::renderSolid;
                    using parent::renderer;
                    using parent::lines;
                    using parent::faces;

                    using parent::modelDimensionMinimum;
                    static const unsigned int modelDimensionMaximum = d == 2 ? d : 3;
                    using parent::renderDimensionMinimum;
                    using parent::renderDimensionMaximum;

                    using parent::faceVertices;

                    using parent::functions;
                    using parent::calculateObject;

                    static unsigned int depth (void) { return od; }
                    static unsigned int renderDepth (void) { return d; }
                    static const char *id (void) { return "sierpinski-carpet"; }
            };
        };

        namespace transformation
        {
            template <typename Q, unsigned int d, unsigned int od = d>
            class randomAffine : public affine<Q,d>
            {
                public:
                    randomAffine(const parameters<Q> &pParameter)
                        : parameter(pParameter)
                        {
                            updateMatrix();
                        }

                    void updateMatrix (void)
                    {
                        typename euclidian::space<Q,d>::vector V;
                        const Q s(Q(std::rand()%8000)/Q(10000)+Q(.2));
                        const Q r1(Q(std::rand()%20000)/Q(10000)*Q(M_PI));
                        unsigned int a1 = std::rand() % od;
                        unsigned int a2 = std::rand() % od;
                        const Q r2(Q(std::rand()%20000)/Q(10000)*Q(M_PI));
                        unsigned int a4 = std::rand() % od;
                        unsigned int a5 = std::rand() % od;
                        
                        if (a1 > a2)
                        {
                            unsigned int a3 = a1;
                            a1 = a2;
                            a2 = a3;
                        }
                        else if (a1 == a2)
                        {
                            if (a1 == 0)
                            {
                                a2 = std::rand() % (od-1) + 1;
                            }
                            else
                            {
                                a1--;
                            }
                        }
                        
                        if (a4 > a5)
                        {
                            unsigned int a6 = a4;
                            a4 = a5;
                            a5 = a6;
                        }
                        else if (a4 == a5)
                        {
                            if (a4 == 0)
                            {
                                a5 = std::rand() % (od-1) + 1;
                            }
                            else
                            {
                                a4--;
                            }
                        }
                        
                        for (unsigned int j = 0; j < od; j++)
                        {
                            V.data[j] = Q(std::rand()%10000)/Q(5000)-Q(1);
                        }
                        
                        transformationMatrix =
                                (  transformation::scale<Q,d>(s)
                                 * ( parameter.preRotate
                                        ? transformation::affine<Q,d> (transformation::rotation<Q,d>(r1, a1, a2))
                                        : transformation::affine<Q,d> () )
                                 * transformation::translation<Q,d>(V)
                                 * ( parameter.postRotate
                                        ? transformation::affine<Q,d> (transformation::rotation<Q,d>(r2, a4, a5))
                                        : transformation::affine<Q,d> () )
                                 ).transformationMatrix;
                    }

                    using affine<Q,d>::transformationMatrix;

                protected:
                    const parameters<Q> &parameter;
            };
        };

        template <typename Q, unsigned int od, typename render, unsigned int d = od>
        class randomAffineIFS : public ifs<Q,od,render,d,cube,2,transformation::affine>
        {
            public:
                typedef ifs<Q,od,render,d,cube,2,transformation::affine> parent;

                randomAffineIFS(render &pRenderer, const parameters<Q> &pParameter, const Q &pMultiplier = 1)
                    : parent(pRenderer, pParameter, pMultiplier)
                    {
                        calculateObject();
                    }

                    void calculateObject (void)
                    {
                        functions.clear();

                        std::srand(parameter.seed);

                        const unsigned int nfunctions = parameter.functions;

                        for (unsigned int i = 0; i < nfunctions; i++)
                        {
                            functions.push_back (transformation::randomAffine<Q,d,od>(parameter));
                        }

                        parent::calculateObject();
                    }

                using parent::parameter;
                using parent::renderSolid;
                using parent::renderer;
                using parent::lines;
                using parent::faces;

                using parent::modelDimensionMinimum;
                static const unsigned int modelDimensionMaximum = d;
                using parent::renderDimensionMinimum;
                using parent::renderDimensionMaximum;

                using parent::faceVertices;

                using parent::functions;
                using parent::calculateObject;

                static unsigned int depth (void) { return od; }
                static unsigned int renderDepth (void) { return d; }
                static const char *id (void) { return "random-affine-ifs"; }
        };
    };
};

#endif
