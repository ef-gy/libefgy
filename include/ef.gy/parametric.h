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
        namespace formula
        {
            template <typename Q, unsigned int od, unsigned int d>
            class moebiusStrip
            {
                public:
                    static const unsigned int faceVertices           = 4;
                    static const unsigned int modelDimensionMaximum  = 2;
                    static const unsigned int renderDimensionMinimum = od + 1;

                    static const char *id (void) { return "moebius-strip"; }

                    static range<Q> getRange (const parameters<Q> &parameter, unsigned int i)
                    {
                        return i == 0 ? range<Q>(0, M_PI * Q(2), parameter.polarPrecision*Q(2), false)
                                      : range<Q>(-parameter.polarRadius, parameter.polarRadius, parameter.polarPrecision, false);
                    }

                    static math::vector<Q,d> getCoordinates (const parameters<Q> &parameter, Q u, Q v)
                    {
                        Q r = parameter.polarRadius;

                        return {{ Q((r + v/Q(2) * cos(u/Q(2))) * cos(u)),
                                  Q((r + v/Q(2) * cos(u/Q(2))) * sin(u)),
                                  Q(v/Q(2) * sin(u/Q(2))) }};
                    }
            };

            template <typename Q, unsigned int od, unsigned int d>
            class kleinBagel
            {
                public:
                    static const unsigned int faceVertices           = 4;
                    static const unsigned int modelDimensionMaximum  = 2;
                    static const unsigned int renderDimensionMinimum = od + 1;
                    
                    static const char *id (void) { return "klein-bagel"; }
                    
                    constexpr static range<Q> getRange (const parameters<Q> &parameter, unsigned int)
                    {
                        return range<Q>(0, M_PI * Q(2), parameter.polarPrecision*Q(2), false);
                    }

                    static math::vector<Q,d> getCoordinates (const parameters<Q> &parameter, Q u, Q v)
                    {
                        Q r = parameter.polarRadius;

                        return {{ Q((r + cos(u/Q(2))*sin(v) - sin(u/Q(2))*sin(Q(2)*v)) * cos(u)),
                                  Q((r + cos(u/Q(2))*sin(v) - sin(u/Q(2))*sin(Q(2)*v)) * sin(u)),
                                  Q(sin(u/Q(2))*sin(v) - cos(u/Q(2))*sin(Q(2)*v)) }};
                    }
            };
        };

        template <typename Q, unsigned int od, unsigned int d, template <typename, unsigned int, unsigned int> class formula, typename render>
        class parametric : public polytope<Q,od,d,formula<Q,od,d>::faceVertices,render>
        {
            public:
                typedef formula<Q,od,d> source;
                typedef polytope<Q,od,d,source::faceVertices,render> parent;
            
                parametric (render &pRenderer, const parameters<Q> &pParameter)
                    : parent(pRenderer, pParameter)
                    {
                        calculateObject();
                    }
                
                using parent::parameter;
                using parent::faces;
                
                static const unsigned int modelDimensionMaximum = source::modelDimensionMaximum;

                static const char *id (void) { return source::id(); }

                void calculateObject(void)
                {
                    faces.clear();

                    const range<Q> us = source::getRange(parameter, 0);
                    const range<Q> vs = source::getRange(parameter, 1);

                    for (const Q &u : us)
                    {
                        for (const Q &v : vs)
                        {
                            Q un = u + us.stride;
                            Q vn = v + vs.stride;
                            
                            faces.push_back
                                ({{ source::getCoordinates(parameter, u,  v),
                                    source::getCoordinates(parameter, un, v),
                                    source::getCoordinates(parameter, un, vn),
                                    source::getCoordinates(parameter, u,  vn) }});
                        }
                    }
                }
        };

        template <typename Q, unsigned int od, typename render, unsigned int d>
        using moebiusStrip = parametric<Q,od,d,formula::moebiusStrip,render>;

        template <typename Q, unsigned int od, typename render, unsigned int d>
        using kleinBagel = parametric<Q,od,d,formula::kleinBagel,render>;
    };
};

#endif
