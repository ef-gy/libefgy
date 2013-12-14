/**\file
 *
 * \copyright
 * Copyright (c) 2012-2013, ef.gy Project Members
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

#if !defined(EF_GY_IFS_H)
#define EF_GY_IFS_H

#include <ef.gy/polytope.h>
#include <ef.gy/projection.h>
#include <ef.gy/random.h>
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
        class ifs : public polytope<Q,od,d,primitive<Q,pd,render,d>::faceVertices,render>
        {
            public:
                typedef polytope<Q,od,d,primitive<Q,pd,render,d>::faceVertices,render> parent;

                ifs (render &pRenderer, const parameters<Q> &pParameter)
                    : parent(pRenderer, pParameter)
                    {}

                using typename parent::face;

                /**\brief Render mesh with renderer; use indices
                 *
                 * Passes all of the faces to the renderer for processing; see
                 * the renderers' documentation for more information on the
                 * results of doing so.
                 *
                 * Unlike the generic polytope renderer, the IFS renderer will
                 * pass index hints to the renderers, which are needed by the
                 * fractal flame colouring algorithm to produce even prettier
                 * output.
                 */
                void renderSolid (void) const
                {
                    if (faces.size() != indices.size())
                    {
                        for (const face &p : faces)
                        {
                            renderer.drawFace(p);
                        }
                    }
                    else
                    {
                        typename std::vector<Q>::const_iterator itIndex = indices.begin();

                        std::for_each (faces.begin(), faces.end(), [&] (const face &p)
                        {
                            renderer.drawFace (p, *itIndex);
                            itIndex++;
                        });
                    }
                }
                using parent::parameter;
                using parent::renderSolid;
                using parent::renderer;
                using parent::faces;
                std::vector<Q> indices;

                using parent::modelDimensionMinimum;
                using parent::modelDimensionMaximum;
                using parent::renderDimensionMinimum;
                using parent::renderDimensionMaximum;

                using parent::faceVertices;

                std::vector<trans<Q,d> > functions;

                void calculateObject (void)
                {
                    primitive<Q,pd,render,d> source(parent::renderer, parameter);

                    faces = source.faces;
                    while (faces.size() > indices.size())
                    {
                        indices.push_back(Q(0.5));
                    }

                    unsigned long long vertices = faces.size() * faceVertices;

                    for (unsigned int i = 0; i < parameter.iterations; i++)
                    {
                        if ((vertices * functions.size()) > parameter.vertexLimit)
                        {
                            break;
                        }

                        std::vector<Q> rindices = indices;
                        indices.clear();
                        std::vector<std::array<typename euclidian::space<Q,d>::vector,faceVertices> > rfaces;

                        while (faces.size() > 0)
                        {
                            for (unsigned int j = 0; j < functions.size(); j++)
                            {
                                rfaces.push_back(apply(j,faces.back()));
                                indices.push_back(((Q(j)/Q(functions.size()))+rindices.back())/Q(2));
                            }
                            faces.pop_back();
                            rindices.pop_back();
                        }
                        
                        faces = rfaces;
                        vertices *= functions.size();
                    }
                }
            
                typename euclidian::space<Q,d>::vector apply
                    (const unsigned int &f,
                     const typename euclidian::space<Q,d>::vector &v)
                {
                    return functions[f] * v;
                }
            
                template<std::size_t fdim>
                std::array<typename euclidian::space<Q,d>::vector,fdim> apply
                    (const unsigned int &f,
                     const std::array<typename euclidian::space<Q,d>::vector,fdim> &l)
                {
                    std::array<typename euclidian::space<Q,d>::vector,fdim> r;

                    for (int i = 0; i < fdim; i++)
                    {
                        r[i] = apply (f, l[i]);
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

                    gasket(render &pRenderer, const parameters<Q> &pParameter)
                        : parent(pRenderer, pParameter)
                        {
                            const unsigned int nfunctions = (1<<(od-1))+1;
                            std::array<typename euclidian::space<Q,d>::vector,nfunctions> translations;

                            translations[0][0] = Q(0.25);

                            for (unsigned int i = 1; i < nfunctions; i++)
                            {
                                translations[i][0] = Q(-0.25);
                                for (unsigned int j = 1; j < od; j++)
                                {
                                    const unsigned int k = i-1;
                                    const unsigned int l = j-1;
                                    const unsigned int b = 1 << l;
                                    const bool s = k & b;
                                    translations[i][j] = Q(s ? -0.25 : 0.25);
                                }
                            }

                            for (unsigned int i = 0; i < nfunctions; i++)
                            {
                                functions.push_back(transformation::scale<Q,d>(0.5) * transformation::translation<Q,d>(translations[i]));
                            }

                            calculateObject();
                        }

                    using parent::parameter;
                    using parent::renderSolid;
                    using parent::renderer;
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

                    carpet(render &pRenderer, const parameters<Q> &pParameter)
                        : parent(pRenderer, pParameter)
                        {
                            const unsigned int nfunctions = od == 2 ? 8 : 20;
                            std::array<typename euclidian::space<Q,d>::vector,nfunctions> translations;

                            if (od > 1)
                            {
                                translations[0][0] = Q(-1)/Q(3);
                                translations[0][1] = Q(-1)/Q(3);
                                translations[1][0] = Q(-1)/Q(3);
                                translations[1][1] = Q(0);
                                translations[2][0] = Q(-1)/Q(3);
                                translations[2][1] = Q(1)/Q(3);
                                translations[3][0] = Q(1)/Q(3);
                                translations[3][1] = Q(-1)/Q(3);
                                translations[4][0] = Q(1)/Q(3);
                                translations[4][1] = Q(0);
                                translations[5][0] = Q(1)/Q(3);
                                translations[5][1] = Q(1)/Q(3);
                                translations[6][0] = Q(0);
                                translations[6][1] = Q(-1)/Q(3);
                                translations[7][0] = Q(0);
                                translations[7][1] = Q(1)/Q(3);
                            }
                            if (od > 2)
                            {
                                for (int i = 0; i < 8; i++)
                                {
                                    translations[(i+8)] = translations[i];
                                    translations[i][2] = Q(-1)/Q(3);
                                    translations[(i+8)][2] = Q(1)/Q(3);
                                }

                                translations[16][0] = Q(1)/Q(3);
                                translations[16][1] = Q(1)/Q(3);

                                translations[17][0] = Q(-1)/Q(3);
                                translations[17][1] = Q(1)/Q(3);

                                translations[18][0] = Q(1)/Q(3);
                                translations[18][1] = Q(-1)/Q(3);

                                translations[19][0] = Q(-1)/Q(3);
                                translations[19][1] = Q(-1)/Q(3);
                            }

                            for (unsigned int i = 0; i < nfunctions; i++)
                            {
                                functions.push_back(transformation::scale<Q,d>(Q(1)/Q(3)) * transformation::translation<Q,d>(translations[i]));
                            }

                            calculateObject();
                        }

                    using parent::parameter;
                    using parent::renderSolid;
                    using parent::renderer;
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
                    randomAffine(const parameters<Q> &pParameter, const unsigned long long &pSeed)
                        : parameter(pParameter), seed(pSeed)
                        {
                            updateMatrix();
                        }

                    void updateMatrix (void)
                    {
                        random::mersenneTwister<> PRNG (seed);

                        typename euclidian::space<Q,d>::vector V;
                        const Q s(Q(PRNG.rand()%6000)/Q(10000)+Q(.2));
                        const Q r1(Q(PRNG.rand()%20000)/Q(10000)*Q(M_PI));
                        unsigned int a1 = PRNG.rand() % od;
                        unsigned int a2 = PRNG.rand() % od;
                        const Q r2(Q(PRNG.rand()%20000)/Q(10000)*Q(M_PI));
                        unsigned int a4 = PRNG.rand() % od;
                        unsigned int a5 = PRNG.rand() % od;
                        
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
                                a2 = PRNG.rand() % (od-1) + 1;
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
                                a5 = PRNG.rand() % (od-1) + 1;
                            }
                            else
                            {
                                a4--;
                            }
                        }
                        
                        for (unsigned int j = 0; j < od; j++)
                        {
                            V[j] = Q(PRNG.rand()%10000)/Q(5000)-Q(1);
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
                const unsigned long long &seed;
            };
        };

        template <typename Q, unsigned int od, typename render, unsigned int d = od>
        class randomAffineIFS : public ifs<Q,od,render,d,cube,2,transformation::affine>
        {
            public:
                typedef ifs<Q,od,render,d,cube,2,transformation::affine> parent;

                randomAffineIFS(render &pRenderer, const parameters<Q> &pParameter)
                    : parent(pRenderer, pParameter)
                    {
                        calculateObject();
                    }

                    void calculateObject (void)
                    {
                        functions.clear();
                        
                        random::mersenneTwister<> PRNG (parameter.seed);

                        const unsigned int nfunctions = parameter.functions;

                        for (unsigned int i = 0; i < nfunctions; i++)
                        {
                            functions.push_back (transformation::randomAffine<Q,d,od>(parameter, PRNG.rand()));
                        }

                        parent::calculateObject();
                    }

                using parent::parameter;
                using parent::renderSolid;
                using parent::renderer;
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
