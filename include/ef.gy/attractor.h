/**\file
 *
 * \copyright
 * Copyright (c) 2014, ef.gy Project Members
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

#if !defined(EF_GY_ATTRACTOR_H)
#define EF_GY_ATTRACTOR_H

#include <ef.gy/polytope.h>
#include <ef.gy/projection.h>
#include <ef.gy/euclidian.h>
#include <vector>
#include <cstdlib>
#include <random>

namespace efgy
{
    namespace geometry
    {
        template <typename Q, unsigned int od>
        class attractor : public polytope<Q,od,od,4,math::format::cartesian>
        {
            public:
                typedef polytope<Q,od,od,4,math::format::cartesian> parent;
                using typename parent::format;

                attractor (const parameters<Q> &pParameter, const format &pFormat)
                    : parent(pParameter, pFormat)
                    {
                        calculateObject();
                    }

                using parent::parameter;
                using parent::faces;

                void calculateObject (void)
                {
                    faces.clear();

                    std::mt19937 PRNG (parameter.seed);

                    std::vector<math::vector<Q,parent::renderDepth,format>> seeds;

                    math::vector<Q,parent::renderDepth,format> vec {{}};

                    for (unsigned int i = 0; i < 4; i++)
                    {
                        for (auto &v : vec)
                        {
                            v = Q(PRNG()%20000)/Q(10000) - Q(1.0);
                        }
                        seeds.push_back(vec);
                    }

                    std::vector<math::vector<Q,parent::renderDepth,format>> points;
                    for (const auto &i : range<Q>(-parameter.radius/Q(2), parameter.radius/Q(2), parameter.precision, false))
                    {
                        points.push_back({{i}});
                    }

                    std::vector<math::vector<Q,parent::renderDepth,format>> points2 = points;
                    
                    for (const auto &i : range<int>(0, parameter.iterations*10, false))
                    {
                        for (auto &p : points)
                        {
                            auto o = p;

                            for (auto &s : seeds)
                            {
                                p = p + (s - o) / math::length(o - s);
                            }
                        }

                        for (unsigned int j = 0; j < (points.size()-1); j++)
                        {
                            faces.push_back({{points[j],points[(j+1)],points2[(j+1)],points2[j]}});
                        }

                        points2 = points;
                    }
                }

                typedef dimensions<2, 0> dimensions;

                /**\copydoc polytope::id() */
                static constexpr const char *id (void)
                {
                    return "attractor";
                }
        };
    };
};

#endif
