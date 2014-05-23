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
 * \see Project Source Code: https://github.com/ef-gy/libefgy
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
            template <typename Q, unsigned int od, unsigned int d, typename format>
            class moebiusStrip
            {
                public:
                    typedef dimensions<2, 2, 3, 0> dimensions;

                    static constexpr const char *id (void) { return "moebius-strip"; }

                    constexpr static range<Q> getRange (const parameters<Q> &parameter, std::size_t i)
                    {
                        return i == 0 ? range<Q>(0, M_PI * Q(2), parameter.precision*Q(2), false)
                                      : range<Q>(-parameter.radius, parameter.radius, parameter.precision, false);
                    }

                    constexpr static math::vector<Q,d> getCoordinates (const parameters<Q> &parameter, const math::vector<Q,od> &ve)
                    {
                        return {{ Q((parameter.radius + ve[1]/Q(2) * cos(ve[0]/Q(2))) * cos(ve[0])),
                                  Q((parameter.radius + ve[1]/Q(2) * cos(ve[0]/Q(2))) * sin(ve[0])),
                                  Q(ve[1]/Q(2) * sin(ve[0]/Q(2))) }};
                    }
            };

            template <typename Q, unsigned int od, unsigned int d, typename format>
            class kleinBagel
            {
                public:
                    typedef dimensions<2, 2, 3, 0> dimensions;

                    static constexpr const char *id (void) { return "klein-bagel"; }

                    constexpr static range<Q> getRange (const parameters<Q> &parameter, std::size_t)
                    {
                        return range<Q>(0, M_PI * Q(2), parameter.precision*Q(2), false);
                    }

                    constexpr static math::vector<Q,d> getCoordinates (const parameters<Q> &parameter, const math::vector<Q,od> &ve)
                    {
                        return {{ Q((parameter.radius + cos(ve[0]/Q(2))*sin(ve[1]) - sin(ve[0]/Q(2))*sin(Q(2)*ve[1])) * cos(ve[0])),
                                  Q((parameter.radius + cos(ve[0]/Q(2))*sin(ve[1]) - sin(ve[0]/Q(2))*sin(Q(2)*ve[1])) * sin(ve[0])),
                                  Q(sin(ve[0]/Q(2))*sin(ve[1]) - cos(ve[0]/Q(2))*sin(Q(2)*ve[1])) }};
                    }
            };

            template <typename Q, unsigned int od, unsigned int d, typename format>
            class kleinBottle
            {
                public:
                    typedef dimensions<2, 2, 4, 0> dimensions;

                    static constexpr const char *id (void) { return "klein-bottle"; }

                    constexpr static range<Q> getRange (const parameters<Q> &parameter, std::size_t)
                    {
                        return range<Q>(0, M_PI * Q(2), parameter.precision*Q(2), false);
                    }

                    static math::vector<Q,d> getCoordinates (const parameters<Q> &parameter, const math::vector<Q,od> &ve)
                    {
                        math::vector<Q,d> r {{ Q(parameter.radius  * (cos(ve[0]/Q(2)) * cos(ve[1]) - sin(ve[0]/Q(2)) * sin(Q(2)*ve[1]))),
                                               Q(parameter.radius  * (sin(ve[0]/Q(2)) * cos(ve[1]) + cos(ve[0]/Q(2)) * sin(Q(2)*ve[1]))),
                                               Q(parameter.radius2 * cos(ve[0]) * (Q(1) + parameter.constant * sin(ve[1]))) }};

                        if (d >= 4)
                        {
                            r[3] = Q(parameter.radius2 * sin(ve[0]) * (Q(1) + parameter.constant * sin(ve[1])));
                        }
                        
                        return r;
                    }
            };

            template <typename Q, unsigned int od, unsigned int d, typename format>
            class sphere
            {
                public:
                    typedef dimensions<2, d - 1, 3, 0> dimensions;

                    static constexpr const char *id (void) { return "sphere"; }

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
            
            template <typename Q, unsigned int od, unsigned int d, typename format>
            class plane
            {
                public:
                    typedef dimensions<2, 2, 3, 0> dimensions;

                    static constexpr const char *id (void) { return "plane"; }

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

            template <typename Q, unsigned int od, unsigned int d, typename format>
            class torus
            {
                public:
                    typedef dimensions<2, 2, 3, 0> dimensions;

                    static constexpr const char *id (void) { return "torus"; }

                    constexpr static range<Q> getRange (const parameters<Q> &parameter, std::size_t i)
                    {
                        return range<Q>(0, M_PI * Q(2), parameter.precision*Q(2), false);
                    }

                    constexpr static math::vector<Q,d> getCoordinates (const parameters<Q> &parameter, const math::vector<Q,od> &ve)
                    {
                        return {{ Q((parameter.radius + parameter.radius2 * cos(ve[1])) * cos(ve[0])),
                                  Q((parameter.radius + parameter.radius2 * cos(ve[1])) * sin(ve[0])),
                                  Q(parameter.radius2 * sin(ve[1])) }};
                    }
            };
        };

        /**\brief Parametric formula wrapper
         *
         * This class is used to instantiate parametric formulae so they can
         * produce meshes. To calculate meshes, the formulae provide ranges in
         * all of their surface dimensions and the objects are created by
         * sweeping over all points in these ranges, then the individual
         * surfaces are created using the surfaces of a hypercube in the model
         * dimension scaled by the step size of the ranges.
         *
         * For example, if the model depth is '2', then the area that is swept
         * is a plane with the size and step size provided by the formula. The
         * individual surfaces are squares that are scaled according to the step
         * size of the two provided ranges, and each of the vertices is
         * realigned using the formula.
         *
         * \tparam Q       Base type for calculations; should be a rational type
         * \tparam od      Model depth, e.g. '2' for a square or '3' for a cube
         * \tparam d       Number of dimensions of the vector space to use
         * \tparam formula Formula for the target mesh, e.g. formula::plane
         * \tparam format  Vector coordinate format to work in.
         */
        template <typename Q, unsigned int od, unsigned int d,
                  template <typename, unsigned int, unsigned int, typename> class formula,
                  typename format>
        class parametric : public polytope<Q,od,d,4,format>
        {
            public:
                typedef formula<Q,od,d,format> source;
                typedef polytope<Q,od,d,4,format> parent;
            
                parametric (const parameters<Q> &pParameter, const format &pFormat)
                    : parent(pParameter, pFormat)
                    {
                        calculateObject();
                    }

                using parent::parameter;
                using parent::faces;
                using parent::tag;

                typedef typename source::dimensions dimensions;

                static constexpr const char *id (void) { return source::id(); }

                void recurse
                    (const cube<Q,od,od,math::format::cartesian> &cube,
                     math::vector<Q,od> v,
                     math::vector<Q,od> a)
                {
                    for (std::array<math::vector<Q,od>,4> f : cube.faces)
                    {
                        std::array<math::vector<Q,d,format>,4> g;

                        for (std::size_t i = 0; i < 4; i++)
                        {
                            for (std::size_t q = 0; q < od; q++)
                            {
                                f[i][q] *= a[q];
                            }

                            g[i] = std::array<Q,d>(source::getCoordinates(parameter, v+f[i]));
                        }

                        faces.push_back (g);
                    }
                }

                void recurse
                    (cube<Q,od,od,math::format::cartesian> &cube,
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
                    cube<Q,od,od,math::format::cartesian> cube
                        (cubeParameter, math::format::cartesian());

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

        /**\brief The 2D plane
         *
         * One of the most simplest of models, this is a simple 2D plane using
         * the radius and precision model parameters. This used to be its own
         * proper type, but has since been redesigned to use the parametric
         * model template and a very simple formula. This type alias is provided
         * because the plane is often used as a base type for other, more
         * complex models, such as the fractal flames.
         *
         * \tparam Q      Base type for calculations; should be a rational type
         * \tparam od     Model depth, e.g. '2' for a square or '3' for a cube
         * \tparam d      Number of dimensions of the vector space to use
         * \tparam format Vector coordinate format to work in.
         */
        template <typename Q, unsigned int od, unsigned int d,
                  typename format>
        using plane = parametric<Q,od,d,formula::plane,format>;
    };
};

#endif
