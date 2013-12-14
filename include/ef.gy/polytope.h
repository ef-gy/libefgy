/**\file
 * \brief Basic primitives
 *
 * This file contains code to create the meshes for basic primitives and to
 * present the results with the renderers provided by this library.
 *
 * 'Basic' primitives includes cubes, simplices, planes and spheres.
 *
 * \see parametric.h, ifs.h and flame.h for more complicated shapes, some of
 *      which are based on these primitives.
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

#if !defined(EF_GY_POLYTOPE_H)
#define EF_GY_POLYTOPE_H

#include <ef.gy/euclidian.h>
#include <ef.gy/polar.h>
#include <vector>
#include <array>
#include <algorithm>
#include <ef.gy/range.h>

namespace efgy
{
    namespace geometry
    {
        /**\defgroup libefgy-geometric-primitives Geometric Primitives
         * \brief Class templates that define basic geometric primitives
         *
         * This group contains class templates that calculate meshes for basic
         * geometric primitives, such as simplices, cubes or spheres.
         */

        /**\ingroup libefgy-geometric-primitives
         * \defgroup libefgy-extended-geometric-primitives Extended Geometric Primitives
         * \brief Class templates that define extended geometric primitives
         *
         * Contains "extended" primitives - either those that reuse "basic"
         * primitives, those that are very specialised or those that can get
         * very complex.
         */

        /**\brief Geometry parameters
         *
         * Contains parameters used by the shape-creating templates. You need
         * to have at least one of these for each data type you intend to use.
         *
         * \tparam Q Base data type for calculations.
         */
        template <typename Q>
        class parameters
        {
            public:
                parameters(void)
                    : polarRadius(1),
                      polarPrecision(3),
                      iterations(4),
                      functions(3),
                      seed(0),
                      preRotate(1),
                      postRotate(0),
                      flameCoefficients(3),
                      vertexLimit(1000000)
                    {}

                Q polarRadius;
                Q polarPrecision;
                unsigned int iterations;
                unsigned int functions;
                unsigned int seed;
                bool preRotate;
                bool postRotate;
                unsigned int flameCoefficients;
                unsigned long long vertexLimit;
        };

        /**\brief Polytope base template
         *
         * Contains the base definitions for any polytope, i.e. code that
         * should be shared among most class templates that generate meshes to
         * be rendered.
         *
         * \tparam Q      Base type for calculations; should be a rational type
         * \tparam od     Model depth, e.g. '2' for a square or '3' for a cube
         * \tparam d      Number of dimensions of the vector space to use
         * \tparam f      Number of vertices for mesh faces
         * \tparam render Renderer type; e.g. render::svg<Q,d>
         */
        template <typename Q, unsigned int od, unsigned int d, unsigned int f, typename render>
        class polytope
        {
            public:
                polytope (render &pRenderer, const parameters<Q> &pParameter, const Q &pMultiplier = 1)
                    : renderer(pRenderer), parameter(pParameter), precisionMultiplier(pMultiplier)
                    {}

                /**\brief Render mesh with renderer
                 *
                 * Passes all of the faces to the renderer for processing; see
                 * the renderers' documentation for more information on the
                 * results of doing so.
                 */
                void renderSolid (void) const
                {
                    for (const face &p : faces)
                    {
                        renderer.drawFace(p);
                    }
                }

                static const unsigned int modelDimensionMinimum  = 2;
                static const unsigned int modelDimensionMaximum  = 0;
                static const unsigned int renderDimensionMinimum = 3;
                static const unsigned int renderDimensionMaximum = 0;

                /**\brief Number of face vertices
                 *
                 * An alias for the 'f' template parameter, which contains the
                 * number of vertices that make up an individual 'face' of the
                 * resulting mesh.
                 */
                static const unsigned int faceVertices = f;

                /**\brief Query the model's depth
                 *
                 * This static method returns a model's "depth". The exact
                 * meaning of this value depends on the model in question, but
                 * generally it's the number of dimensions of the surface of a
                 * mesh.
                 *
                 * For example, the depth of a 2-sphere is "2", because while
                 * you do need 3 dimensions to render a 2-sphere, the surface
                 * of such a sphere is 2 dimensional since you can describe any
                 * point on the surface with its (u,v) coordinates.
                 *
                 * A 3-cube, on the other hand, is also renderable in 3
                 * dimensions, but there's no natural way to express a point on
                 * the surface of such a cube with only two coordinates - you
                 * could of course unroll the sides onto a texture, but it
                 * still isn't "natural".
                 *
                 * Yeah the definition is kind of foggy; as a rule of thumb:
                 * have a look at how the models are described in other places,
                 * if it's called an n-something, then this 'depth' here is
                 * 'n'.
                 *
                 * \returns The model's depth.
                 */
                static constexpr unsigned int depth (void)
                {
                    return od;
                }

                /**\brief Query the model's render depth
                 *
                 * This static method returns a model's render depth, which is
                 * basically the number of dimensions in the vector space in
                 * which it was constructed.
                 *
                 * \returns The model's render depth.
                 */
                static constexpr unsigned int renderDepth (void)
                {
                    return d;
                }

                /**\brief Query model ID
                 *
                 * Every primitive has an ID that is used to identify the type
                 * at runtime. This method produces that string.
                 *
                 * \returns The model's ID as a C-style string.
                 */
                static constexpr const char *id (void)
                {
                    return "none";
                }

                /**\brief Query the model's genome
                 *
                 * Every primitive that uses parameters should be able to
                 * produce a genome, which uniquely identifies the settings
                 * that were used to calculate the mesh and which could be used
                 * to recreate that same model later. We're calling this a
                 * genome because some models make perfect candidates for
                 * further processing with a genetic algorithm.
                 *
                 * \returns An aray of Qs describing the object's parameters.
                 */
                constexpr std::array<Q,0> genome (void) const
                {
                    return {{}};
                }

            protected:
                typedef std::array<typename euclidian::space<Q,d>::vector,f> face;
                render &renderer;
                const parameters<Q> &parameter;
                const Q &precisionMultiplier;

                std::vector<face> faces;
        };

        template <typename Q, unsigned int od, typename render, unsigned int d = od>
        class simplex : public polytope<Q,od,d,3,render>
        {
            public:
                typedef polytope<Q,od,d,3,render> parent;

                simplex (render &pRenderer, const parameters<Q> &pParameter, const Q &pMultiplier = 1)
                    : parent(pRenderer, pParameter, pMultiplier)
                    {
                        calculateObject();
                    }

                using parent::parameter;
                using parent::faces;

                static const unsigned int modelDimensionMaximum = d;

                /**\copydoc polytope::id() */
                static const char *id (void)
                {
                    return "simplex";
                }

                void recurse (const int r, typename polar::space<Q,d>::vector v, std::vector<typename euclidian::space<Q,d>::vector> &points)
                {
                    if (r == 0)
                    {
                        typename euclidian::space<Q,d>::vector A = v;
                        points.push_back(A);
                    }
                    else
                    {
                        const int q = r-1;

                        v[r] = 0;
                        recurse (q, v, points);
                        v[r] = Q(M_PI)/Q(1.5);
                        recurse (q, v, points);
                    }
                }

                void calculateObject (void)
                {
                    Q radius = parameter.polarRadius;

                    faces.clear();

                    std::vector<typename euclidian::space<Q,d>::vector> points;

                    typename polar::space<Q,d>::vector v;
                    v[0] = radius;
                    
                    const int r = od-1;
                    const int q = r-1;

                    v[r] = Q(-M_PI)/Q(1.5);
                    recurse (q, v, points);
                    v[r] = 0;
                    recurse (q, v, points);
                    v[r] = Q(M_PI)/Q(1.5);
                    recurse (q, v, points);

                    std::vector<typename euclidian::space<Q,d>::vector> points2;

                    std::for_each (points.begin(), points.end(), [&] (const typename euclidian::space<Q,d>::vector &point)
                    {
                        if (std::find (points2.begin(), points2.end(), point) == points2.end())
                        {
                            points2.push_back (point);
                        }
                    });

                    points = points2;

                    std::vector<typename euclidian::space<Q,d>::vector> usedPoints;

                    std::for_each (points.begin(), points.end(), [&] (const typename euclidian::space<Q,d>::vector &A)
                    {
                        std::vector<typename euclidian::space<Q,d>::vector> usedPoints2;

                        std::for_each (usedPoints.begin(), usedPoints.end(), [&] (const typename euclidian::space<Q,d>::vector &B)
                        {
                            std::for_each (usedPoints2.begin(), usedPoints2.end(), [&] (const typename euclidian::space<Q,d>::vector &C)
                            {
                                faces.push_back ({{ A, B, C }});
                            });

                            usedPoints2.push_back(B);
                        });

                        usedPoints.push_back(A);
                    });
                }
        };

        /**\ingroup libefgy-geometric-primitives
         * \brief The hypercube
         *
         * This template calculates the mesh for a cube in any number of
         * dimensions. For example if you set the 'od' parameter to '3', then
         * you get a regular cube, but if you set the 'od' parameter to '2' the
         * resulting mesh is a 2D square. '4' will get you a so-called
         * "tesseract", which looks like this:
         *
         * \image html geometric-primitive-4-cube.svg "A tesseract, i.e. a 4-cube"
         *
         * Cubes are a fairly simple and common primitive, and they're easy to
         * generalise to higher dimensional spaces. They also make excellent
         * base primitives for iterated function systems, because they don't
         * produce all that much output while still covering a lot of space,
         * and deformations don't lose much information.
         *
         * In terms of parameters, this primitive only makes use of the
         * parameter::polarRadius field, which i used to determine the size of
         * the mesh. Confusingly, this radius is actually used as the edge
         * length for historical reasons.
         *
         * \tparam Q      Base datatype for calculations
         * \tparam od     The 'depth' of the hypercube; e.g. '3' for a cube
         * \tparam render Model renderer type; e.g. render::svg<Q,d>
         * \tparam d      The render depth of the model; must be >= 'od'
         *
         * \see http://ef.gy/documentation/topologic - Topologic, the programme
         *      that was used to render the sample image.
         *
         * \see http://en.wikipedia.org/wiki/Hypercube for more information on
         *      hypercubes in general.
         *
         * \todo Use the parameter::polarRadius field properly; this should
         *       probably be half the diagonal of the resulting model.
         */
        template <typename Q, unsigned int od, typename render, unsigned int d = od>
        class cube : public polytope<Q,od,d,4,render>
        {
            public:
                typedef polytope<Q,od,d,4,render> parent;

                cube (render &pRenderer, const parameters<Q> &pParameter, const Q & = 1)
                    : parent(pRenderer, pParameter)
                    {
                        calculateObject();
                    }

                using parent::parameter;
                using parent::faces;

                static const unsigned int modelDimensionMaximum = d;

                /**\copydoc polytope::id() */
                static const char *id (void)
                {
                    return "cube";
                }

                void calculateObject (void)
                {
                    Q diameter = parameter.polarRadius * Q(0.5);
                    
                    std::vector<std::array<typename euclidian::space<Q,d>::vector,2> > lines;
                    faces.clear();
                    
                    std::vector<typename euclidian::space<Q,d>::vector> points;

                    points.push_back (typename euclidian::space<Q,d>::vector());

                    for (unsigned int i : sequence<unsigned int,0,od,false>())
                    {
                        std::vector<typename euclidian::space<Q,d>::vector> newPoints;
                        std::vector<std::array<typename euclidian::space<Q,d>::vector,2> > newLines;
                        std::vector<std::array<typename euclidian::space<Q,d>::vector,4> > newFaces;
                        
                        std::for_each (lines.begin(), lines.end(), [&] (std::array<typename euclidian::space<Q,d>::vector,2> &line)
                        {
                            line[0][i] = -diameter;
                            line[1][i] = -diameter;

                            std::array<typename euclidian::space<Q,d>::vector,2> newLine = line;

                            newLine[0][i] = diameter;
                            newLine[1][i] = diameter;

                            newLines.push_back(newLine);
                            newFaces.push_back({{newLine[0], newLine[1], line[1], line[0]}});
                        });

                        std::for_each (faces.begin(), faces.end(), [&] (typename parent::face &face)
                        {
                            face[0][i] = -diameter;
                            face[1][i] = -diameter;
                            face[2][i] = -diameter;
                            face[3][i] = -diameter;

                            typename parent::face newFace = face;
                            newFace[0][i] = diameter;
                            newFace[1][i] = diameter;
                            newFace[2][i] = diameter;
                            newFace[3][i] = diameter;
                            newFaces.push_back(newFace);
                        });

                        std::for_each (points.begin(), points.end(), [&] (typename euclidian::space<Q,d>::vector &v)
                        {
                            v[i] = -diameter;

                            std::array<typename euclidian::space<Q,d>::vector,2> newLine {{ v, v }};

                            newLine[1][i] = diameter;
                            
                            newPoints.push_back(newLine[1]);
                            
                            lines.push_back(newLine);
                        });

                        points.insert(points.end(), newPoints.begin(), newPoints.end());
                        lines.insert(lines.end(), newLines.begin(), newLines.end());
                        faces.insert(faces.end(), newFaces.begin(), newFaces.end());
                    }
                }

                /**\copydoc polytope::genome() */
                std::array<Q,1> genome (void) const
                {
                    return {{ parameter.polarRadius }};
                }
        };

        template <typename Q, unsigned int od, typename render, unsigned int d = od>
        class plane : public polytope<Q,od,d,4,render>
        {
            public:
                typedef polytope<Q,od,d,4,render> parent;

                plane (render &pRenderer, const parameters<Q> &pParameter, const Q &pMultiplier = 1)
                    : parent(pRenderer, pParameter, pMultiplier)
                    {
                        calculateObject();
                    }

                using parent::parameter;
                using parent::faces;

                static const unsigned int modelDimensionMaximum = 2;

                /**\copydoc polytope::id() */
                static const char *id (void)
                {
                    return "plane";
                }

                void calculateObject (void)
                {
                    const Q s = parameter.polarRadius * Q(2);
                    const range<Q> r (-s, s, parameter.polarPrecision, false);

                    faces.clear();

                    for (Q i : r)
                    {
                        for (Q j : r)
                        {
                            faces.push_back({{ {{i, j}}, {{i+r.stride, j}}, {{i+r.stride, j+r.stride}}, {{i, j+r.stride}} }});
                        }
                    }
                }
        };

        template <typename Q, unsigned int od, typename render, unsigned int d = od>
        class sphere : public polytope<Q,od,d,3,render>
        {
            public:
                typedef polytope<Q,od,d,3,render> parent;

                sphere (render &pRenderer, const parameters<Q> &pParameter, const Q &pMultiplier = 1)
                    : parent(pRenderer, pParameter, pMultiplier),
                      loopRange(-M_PI, M_PI, 1, true)
                    {
                        calculateObject();
                    }

                using parent::precisionMultiplier;
                using parent::parameter;
                using parent::renderSolid;
                using parent::renderer;
                using parent::faces;

                using parent::modelDimensionMinimum;
                static const unsigned int modelDimensionMaximum = d - 1;
                using parent::renderDimensionMinimum;
                using parent::renderDimensionMaximum;

                using parent::faceVertices;

                /**\copydoc polytope::id() */
                static const char *id (void)
                {
                    return "sphere";
                }

                void recurse (const int r, typename polar::space<Q,d>::vector v)
                {
                    if (r == 0)
                    {
                        const typename euclidian::space<Q,d>::vector A = v;

//                        for (unsigned int i : range<unsigned int>(1, od, true))
                        for (unsigned int i = 1; i <= od; i++)
                        {
                            typename polar::space<Q,d>::vector v1 = v;

                            v1[i] += loopRange.stride;

                            const typename euclidian::space<Q,d>::vector B = v1;

                            std::array<typename euclidian::space<Q,d>::vector,3> newFace {{A, B}};

//                            for (unsigned int j : range<unsigned int>(1, od, true))
                            for (unsigned int j = 1; j <= od; j++)
                            {
                                if (i != j)
                                {
                                    v1 = v;
                                    v1[j] -= loopRange.stride;

                                    const typename euclidian::space<Q,d>::vector C = v1;
                                    newFace[2] = C;

                                    faces.push_back(newFace);
                                }
                            }
                        }
                    }
                    else
                    {
                        const int q = r-1;

                        for (Q i : loopRange)
                        {
                            v[r] = i;
                            recurse (q, v);
                        }
                    }
                }

                void calculateObject (void)
                {
                    Q radius = parameter.polarRadius;
                    loopRange = range<Q>(-M_PI, M_PI, parameter.polarPrecision * precisionMultiplier, false);

                    faces.clear();

                    typename polar::space<Q,d>::vector v {{ radius }};

                    const int r = od;
                    const int q = r-1;

                    for (Q i : loopRange)
                    {
                        v[r] = i;
                        recurse (q, v);
                    }
                }

            protected:
                range<Q> loopRange;
        };
    };
};

#endif
