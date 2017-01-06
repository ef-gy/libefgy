/**\file
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

#if !defined(EF_GY_ATTRACTOR_H)
#define EF_GY_ATTRACTOR_H

#include <ef.gy/polytope.h>
#include <ef.gy/projection.h>
#include <ef.gy/euclidian.h>
#include <vector>
#include <cstdlib>
#include <random>

namespace efgy {
namespace geometry {
template <typename Q, unsigned int od>
class randomAttractor : public polytope<Q, od, od, 4, math::format::cartesian> {
public:
  typedef polytope<Q, od, od, 4, math::format::cartesian> parent;
  using typename parent::format;

  randomAttractor(const parameters<Q> &pParameter, const format &pFormat)
      : parent(pParameter, pFormat) {
    calculateObject();
  }

  using parent::parameter;
  using parent::faces;

  void calculateObject(void) {
    faces.clear();

    std::mt19937 PRNG(parameter.seed);

    std::vector<math::vector<Q, parent::renderDepth, format>> seeds;

    math::vector<Q, parent::renderDepth, format> vec{{}};

    for (unsigned int i = 0; i < 4; i++) {
      for (auto &v : vec) {
        v = (Q(PRNG() % 20000) / Q(10000) - Q(1.0)) * parameter.radius2;
      }
      seeds.push_back(vec);
    }

    std::vector<math::vector<Q, parent::renderDepth, format>> points;
    for (const auto &i :
         range<Q>(-parameter.radius / Q(2), parameter.radius / Q(2),
                  parameter.precision, false)) {
      points.push_back({{i}});
    }

    std::vector<math::vector<Q, parent::renderDepth, format>> points2 = points;

    for (const auto &i : range<int>(0, parameter.iterations * 10, false)) {
      for (auto &p : points) {
        auto o = p;

        for (auto &s : seeds) {
          p = p + (s - o) / math::length(o - s);
        }
      }

      for (unsigned int j = 0; j < (points.size() - 1); j++) {
        faces.push_back(
            {{points[j], points[(j + 1)], points2[(j + 1)], points2[j]}});
      }

      points2 = points;
    }
  }

  typedef dimensions<2, 0> dimensions;

  /**\copydoc polytope::id() */
  static constexpr const char *id(void) { return "random-attractor"; }
};
}
}

#endif
