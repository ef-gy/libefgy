/**\file
 * \brief Models based on parametric formulae
 *
 * This header contains a template to generate models using very simple
 * parametric formulae, which describes a wide range of very common models.
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

#if !defined(EF_GY_PARAMETRIC_H)
#define EF_GY_PARAMETRIC_H

#include <ef.gy/polytope.h>

namespace efgy
{
    namespace geometry
    {
        /**\brief Formulae for meshes
         *
         * Contains assorted formulae that are used to calculate meshes for
         * common shapes, such as moebius strips. The classes in here are
         * intended to be used together with some sort of container template,
         * such as the geometry::parametric template.
         */
        namespace formula
        {
            template <typename Q, unsigned int od, unsigned int d>
            class moebiusStrip
            {
                public:
                    typedef dimensions<2, 2, od + 1, 0> dimensions;

                    static const char *id (void) { return "moebius-strip"; }

                    constexpr static range<Q> getRange (const parameters<Q> &parameter, std::size_t i)
                    {
                        return i == 0 ? range<Q>(0, M_PI * Q(2), parameter.precision*Q(2), false)
                                      : range<Q>(-parameter.radius, parameter.radius, parameter.precision, false);
                    }

                    static math::vector<Q,d> getCoordinates (const parameters<Q> &parameter, const math::vector<Q,od> &ve)
                    {
                        const Q &r = parameter.radius;
                        const Q &u = ve[0];
                        const Q &v = ve[1];

                        return {{ Q((r + v/Q(2) * cos(u/Q(2))) * cos(u)),
                                  Q((r + v/Q(2) * cos(u/Q(2))) * sin(u)),
                                  Q(v/Q(2) * sin(u/Q(2))) }};
                    }
            };

            template <typename Q, unsigned int od, unsigned int d>
            class kleinBagel
            {
                public:
                    typedef dimensions<2, 2, od + 1, 0> dimensions;

                    static const char *id (void) { return "klein-bagel"; }

                    constexpr static range<Q> getRange (const parameters<Q> &parameter, std::size_t)
                    {
                        return range<Q>(0, M_PI * Q(2), parameter.precision*Q(2), false);
                    }

                    static math::vector<Q,d> getCoordinates (const parameters<Q> &parameter, const math::vector<Q,od> &ve)
                    {
                        const Q &r = parameter.radius;
                        const Q &u = ve[0];
                        const Q &v = ve[1];

                        return {{ Q((r + cos(u/Q(2))*sin(v) - sin(u/Q(2))*sin(Q(2)*v)) * cos(u)),
                                  Q((r + cos(u/Q(2))*sin(v) - sin(u/Q(2))*sin(Q(2)*v)) * sin(u)),
                                  Q(sin(u/Q(2))*sin(v) - cos(u/Q(2))*sin(Q(2)*v)) }};
                    }
            };

            template <typename Q, unsigned int od, unsigned int d>
            class sphere
            {
                public:
                    typedef dimensions<2, d - 1, 3, 0> dimensions;

                    static const char *id (void) { return "sphere"; }

                    constexpr static range<Q> getRange (const parameters<Q> &parameter, std::size_t i)
                    {
                        return i == 0 ? range<Q>(0, M_PI * Q(2), parameter.precision*Q(2), false)
                                      : range<Q>(0, M_PI, parameter.precision, false);
                    }

                    static math::vector<Q,d> getCoordinates (const parameters<Q> &parameter, const math::vector<Q,od> &ve)
                    {
                        math::vector<Q,d,math::format::polar> vp {{ parameter.radius }};
                        for (std::size_t i = 0; i < od; i++)
                        {
                            vp[(i+1)] = ve[i];
                        }
                        return vp;
                    }
            };
            
            template <typename Q, unsigned int od, unsigned int d>
            class plane
            {
                public:
                    typedef dimensions<2, 2, 3, 0> dimensions;

                    static const char *id (void) { return "plane"; }

                    constexpr static range<Q> getRange (const parameters<Q> &parameter, std::size_t i)
                    {
                        return range<Q>(-parameter.radius*Q(2), parameter.radius*Q(2), parameter.precision, false);
                    }

                    static math::vector<Q,d> getCoordinates (const parameters<Q> &parameter, const math::vector<Q,od> &ve)
                    {
                        math::vector<Q,d> v {{}};
                        for (std::size_t i = 0; i < od; i++)
                        {
                            v[i] = ve[i];
                        }
                        return v;
                    }
            };
        };

        template <typename Q, unsigned int od, unsigned int d, template <typename, unsigned int, unsigned int> class formula, typename render>
        class parametric : public polytope<Q,od,d,4,render>
        {
            public:
                typedef formula<Q,od,d> source;
                typedef polytope<Q,od,d,4,render> parent;
            
                parametric (render &pRenderer, const parameters<Q> &pParameter)
                    : parent(pRenderer, pParameter)
                    {
                        calculateObject();
                    }

                using parent::renderer;
                using parent::parameter;
                using parent::faces;

                typedef typename source::dimensions dimensions;

                static const char *id (void) { return source::id(); }

                void recurse
                    (cube<Q,od,render> &cube,
                     math::vector<Q,od> v,
                     math::vector<Q,od> a)
                {
                    for (std::array<math::vector<Q,od>,4> f : cube.faces)
                    {
                        std::array<math::vector<Q,d>,4> g;

                        for (std::size_t i = 0; i < 4; i++)
                        {
                            for (std::size_t q = 0; q < od; q++)
                            {
                                f[i][q] *= a[q];
                            }

                            g[i] = source::getCoordinates(parameter, v+f[i]);
                        }

                        faces.push_back (g);
                    }
                }

                void recurse
                    (cube<Q,od,render> &cube,
                     std::size_t dim,
                     math::vector<Q,od> v,
                     math::vector<Q,od> a)
                {
                    if (dim == od)
                    {
                        recurse (cube, v, a);
                    }
                    else
                    {
                        const range<Q> qs = source::getRange(parameter, dim);
                        a[dim] = qs.stride;
                        
                        for (const Q &q : qs)
                        {
                            v[dim] = q;
                            recurse (cube, dim+1, v, a);
                        }
                    }
                }

                void calculateObject(void)
                {
                    parameters<Q> cubeParameter;
                    cube<Q,od,render> cube (renderer, cubeParameter);

                    faces.clear();

                    const Q r2 = cubeParameter.radius / Q(2);

                    for (std::array<math::vector<Q,od>,4> &f : cube.faces)
                    {
                        for (std::size_t i = 0; i < 4; i++)
                        {
                            for (std::size_t q = 0; q < od; q++)
                            {
                                f[i][q] += r2;
                            }
                        }
                    }

                    recurse (cube, 0, {{}}, {{}});
                }
        };

        template <typename Q, unsigned int od, typename render, unsigned int d>
        using moebiusStrip = parametric<Q,od,d,formula::moebiusStrip,render>;

        template <typename Q, unsigned int od, typename render, unsigned int d>
        using kleinBagel = parametric<Q,od,d,formula::kleinBagel,render>;

        template <typename Q, unsigned int od, typename render, unsigned int d>
        using sphere = parametric<Q,od,d,formula::sphere,render>;
        
        template <typename Q, unsigned int od, typename render, unsigned int d>
        using plane = parametric<Q,od,d,formula::plane,render>;
    };
};

#endif
