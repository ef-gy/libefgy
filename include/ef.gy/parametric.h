/**\file
 * \brief Models based on parametric formulae
 *
 * This header contains a template to generate models using very simple
 * parametric formulae, which describes a wide range of very common models.
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

#if !defined(EF_GY_PARAMETRIC_H)
#define EF_GY_PARAMETRIC_H

#include <ef.gy/polytope.h>

namespace efgy {
namespace geometry {
/**\brief Formulae for meshes
 *
 * Contains assorted formulae that are used to calculate meshes for
 * common shapes, such as moebius strips. The classes in here are
 * intended to be used together with some sort of container template,
 * such as the geometry::parametric template.
 */
namespace formula {
template <typename Q, unsigned int od> class moebiusStrip {
public:
  typedef dimensions<2, 2> dimensions;
  typedef math::format::cartesian format;

  static constexpr const unsigned int renderDepth = 3;
  static constexpr const char *id(void) { return "moebius-strip"; }

  constexpr static range<Q> getRange(const parameters<Q> &parameter,
                                     std::size_t i) {
    return i == 0 ? range<Q>(0, M_PI * Q(2), parameter.precision * Q(2), false)
                  : range<Q>(-parameter.radius, parameter.radius,
                             parameter.precision, false);
  }

  constexpr static math::vector<Q, renderDepth>
  getCoordinates(const parameters<Q> &parameter,
                 const math::vector<Q, od> &ve) {
    return {
        {Q((parameter.radius + ve[1] / Q(2) * cos(ve[0] / Q(2))) * cos(ve[0])),
         Q((parameter.radius + ve[1] / Q(2) * cos(ve[0] / Q(2))) * sin(ve[0])),
         Q(ve[1] / Q(2) * sin(ve[0] / Q(2)))}};
  }
};

template <typename Q, unsigned int od> class kleinBagel {
public:
  typedef dimensions<2, 2> dimensions;
  typedef math::format::cartesian format;

  static constexpr const unsigned int renderDepth = 3;
  static constexpr const char *id(void) { return "klein-bagel"; }

  constexpr static range<Q> getRange(const parameters<Q> &parameter,
                                     std::size_t) {
    return range<Q>(0, M_PI * Q(2), parameter.precision * Q(2), false);
  }

  constexpr static math::vector<Q, renderDepth>
  getCoordinates(const parameters<Q> &parameter,
                 const math::vector<Q, od> &ve) {
    return {{Q((parameter.radius + cos(ve[0] / Q(2)) * sin(ve[1]) -
                sin(ve[0] / Q(2)) * sin(Q(2) * ve[1])) *
               cos(ve[0])),
             Q((parameter.radius + cos(ve[0] / Q(2)) * sin(ve[1]) -
                sin(ve[0] / Q(2)) * sin(Q(2) * ve[1])) *
               sin(ve[0])),
             Q(sin(ve[0] / Q(2)) * sin(ve[1]) -
               cos(ve[0] / Q(2)) * sin(Q(2) * ve[1]))}};
  }
};

template <typename Q, unsigned int od> class kleinBottle {
public:
  typedef dimensions<2, 2> dimensions;
  typedef math::format::cartesian format;

  static constexpr const unsigned int renderDepth = 4;
  static constexpr const char *id(void) { return "klein-bottle"; }

  constexpr static range<Q> getRange(const parameters<Q> &parameter,
                                     std::size_t) {
    return range<Q>(0, M_PI * Q(2), parameter.precision * Q(2), false);
  }

  constexpr static math::vector<Q, renderDepth>
  getCoordinates(const parameters<Q> &parameter,
                 const math::vector<Q, od> &ve) {
    return {{Q(parameter.radius * (cos(ve[0] / Q(2)) * cos(ve[1]) -
                                   sin(ve[0] / Q(2)) * sin(Q(2) * ve[1]))),
             Q(parameter.radius * (sin(ve[0] / Q(2)) * cos(ve[1]) +
                                   cos(ve[0] / Q(2)) * sin(Q(2) * ve[1]))),
             Q(parameter.radius2 * cos(ve[0]) *
               (Q(1) + parameter.constant * sin(ve[1]))),
             Q(parameter.radius2 * sin(ve[0]) *
               (Q(1) + parameter.constant * sin(ve[1])))}};
  }
};

template <typename Q, unsigned int od> class sphere {
public:
  typedef dimensions<2, 0> dimensions;
  typedef math::format::cartesian format;

  static constexpr const unsigned int renderDepth = od + 1;
  static constexpr const char *id(void) { return "sphere"; }

  constexpr static range<Q> getRange(const parameters<Q> &parameter,
                                     std::size_t i) {
    return i == 0 ? range<Q>(0, M_PI * Q(2), parameter.precision * Q(2), false)
                  : range<Q>(0, M_PI, parameter.precision, false);
  }

  static math::vector<Q, renderDepth>
  getCoordinates(const parameters<Q> &parameter,
                 const math::vector<Q, od> &ve) {
    math::vector<Q, renderDepth, math::format::polar> vp{{parameter.radius}};
    for (std::size_t i = 0; i < od; i++) {
      vp[(i + 1)] = ve[i];
    }
    return vp;
  }
};

template <typename Q, unsigned int od> class plane {
public:
  typedef dimensions<2, 2> dimensions;
  typedef math::format::cartesian format;

  static constexpr const unsigned int renderDepth = 2;
  static constexpr const char *id(void) { return "plane"; }

  constexpr static range<Q> getRange(const parameters<Q> &parameter,
                                     std::size_t i) {
    return range<Q>(-parameter.radius * Q(2), parameter.radius * Q(2),
                    parameter.precision, false);
  }

  constexpr static math::vector<Q, renderDepth>
  getCoordinates(const parameters<Q> &parameter,
                 const math::vector<Q, od> &ve) {
    return {{ve[0], ve[1]}};
  }
};

template <typename Q, unsigned int od> class torus {
public:
  typedef dimensions<2, 2> dimensions;
  typedef math::format::cartesian format;

  static constexpr const unsigned int renderDepth = 3;
  static constexpr const char *id(void) { return "torus"; }

  constexpr static range<Q> getRange(const parameters<Q> &parameter,
                                     std::size_t i) {
    return range<Q>(0, M_PI * Q(2), parameter.precision * Q(2), false);
  }

  constexpr static math::vector<Q, renderDepth>
  getCoordinates(const parameters<Q> &parameter,
                 const math::vector<Q, od> &ve) {
    return {
        {Q((parameter.radius + parameter.radius2 * cos(ve[1])) * cos(ve[0])),
         Q((parameter.radius + parameter.radius2 * cos(ve[1])) * sin(ve[0])),
         Q(parameter.radius2 * sin(ve[1]))}};
  }
};

template <typename Q, unsigned int od> class cliffordTorus {
public:
  typedef dimensions<2, 2> dimensions;
  typedef math::format::cartesian format;

  static constexpr const unsigned int renderDepth = 4;
  static constexpr const char *id(void) { return "clifford-torus"; }

  constexpr static range<Q> getRange(const parameters<Q> &parameter,
                                     std::size_t i) {
    return range<Q>(0, M_PI * Q(2), parameter.precision * Q(2), false);
  }

  constexpr static math::vector<Q, renderDepth>
  getCoordinates(const parameters<Q> &parameter,
                 const math::vector<Q, od> &ve) {
    return {{Q(cos(parameter.constant) * cos(ve[0])),
             Q(cos(parameter.constant) * sin(ve[0])),
             Q(sin(parameter.constant) * cos(ve[1])),
             Q(sin(parameter.constant) * sin(ve[1]))}};
  }
};

template <typename Q, unsigned int od> class dinisSurface {
public:
  typedef dimensions<2, 2> dimensions;
  typedef math::format::cartesian format;

  static constexpr const unsigned int renderDepth = 3;
  static constexpr const char *id(void) { return "dinis-surface"; }

  constexpr static range<Q> getRange(const parameters<Q> &parameter,
                                     std::size_t i) {
    return range<Q>(0, M_PI * Q(4) * (std::abs(parameter.constant) + 1.0),
                    parameter.precision * Q(8) *
                        (std::abs(parameter.constant) + 1.0),
                    false);
  }

  constexpr static math::vector<Q, renderDepth>
  getCoordinates(const parameters<Q> &parameter,
                 const math::vector<Q, od> &ve) {
    return {{Q(parameter.radius * cos(ve[0]) * sin(ve[1])),
             Q(parameter.radius * sin(ve[0]) * sin(ve[1])),
             Q(parameter.radius * (cos(ve[1]) + log(tan(ve[1] / 2.0))) +
               parameter.radius2 * ve[0])}};
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
 * \tparam formula Formula for the target mesh, e.g. formula::plane
 */
template <typename Q, unsigned int od,
          template <typename, unsigned int> class formula>
class parametric : public polytope<Q, od, formula<Q, od>::renderDepth, 4,
                                   typename formula<Q, od>::format> {
public:
  typedef formula<Q, od> source;
  typedef polytope<Q, od, formula<Q, od>::renderDepth, 4,
                   typename formula<Q, od>::format> parent;
  using typename parent::format;

  parametric(const parameters<Q> &pParameter, const format &pFormat)
      : parent(pParameter, pFormat) {
    calculateObject();
  }

  using parent::parameter;
  using parent::faces;
  using parent::tag;

  typedef typename source::dimensions dimensions;

  static constexpr const char *id(void) { return source::id(); }

  void recurse(const cube<Q, od> &cube, math::vector<Q, od> v,
               math::vector<Q, od> a) {
    for (std::array<math::vector<Q, od>, 4> f : cube.faces) {
      std::array<math::vector<Q, parent::renderDepth, format>, 4> g;

      for (std::size_t i = 0; i < 4; i++) {
        for (std::size_t q = 0; q < od; q++) {
          f[i][q] *= a[q];
        }

        g[i] = std::array<Q, parent::renderDepth>(
            source::getCoordinates(parameter, v + f[i]));
      }

      faces.push_back(g);
    }
  }

  void recurse(cube<Q, od> &cube, std::size_t dim, math::vector<Q, od> v,
               math::vector<Q, od> a) {
    if (dim == od) {
      recurse(cube, v, a);
    } else {
      const range<Q> qs = source::getRange(parameter, dim);
      a[dim] = qs.stride;

      for (const Q &q : qs) {
        v[dim] = q;
        recurse(cube, dim + 1, v, a);
      }
    }
  }

  void calculateObject(void) {
    parameters<Q> cubeParameter;
    cube<Q, od> cube(cubeParameter, math::format::cartesian());

    faces.clear();

    const Q r2 = cubeParameter.radius / Q(2);

    for (std::array<math::vector<Q, od>, 4> &f : cube.faces) {
      for (std::size_t i = 0; i < 4; i++) {
        for (std::size_t q = 0; q < od; q++) {
          f[i][q] += r2;
        }
      }
    }

    recurse(cube, 0, {{}}, {{}});
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
 */
template <typename Q, unsigned int od>
using plane = parametric<Q, od, formula::plane>;
};
};

#endif
