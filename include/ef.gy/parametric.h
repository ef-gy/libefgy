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

  using usedParameters = parameterFlags<true, false, false, true>;

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

  using usedParameters = parameterFlags<true, false, false, true>;

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

  using usedParameters = parameterFlags<true, true, true, true>;

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

  using usedParameters = parameterFlags<true, false, false, true>;

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
  typedef dimensions<2, 0> dimensions;
  typedef math::format::cartesian format;

  static constexpr const unsigned int renderDepth = od;
  static constexpr const char *id(void) { return "plane"; }

  using usedParameters = parameterFlags<true, false, false, true>;

  constexpr static range<Q> getRange(const parameters<Q> &parameter,
                                     std::size_t i) {
    return range<Q>(-parameter.radius * Q(2), parameter.radius * Q(2),
                    parameter.precision, false);
  }

  static math::vector<Q, renderDepth>
  getCoordinates(const parameters<Q> &parameter,
                 const math::vector<Q, od> &ve) {
    math::vector<Q, renderDepth> vp{{ve[0]}};
    for (std::size_t i = 0; i < od; i++) {
      vp[i] = ve[i];
    }
    return vp;
  }
};

template <typename Q, unsigned int od> class torus {
public:
  typedef dimensions<2, 2> dimensions;
  typedef math::format::cartesian format;

  static constexpr const unsigned int renderDepth = 3;
  static constexpr const char *id(void) { return "torus"; }

  using usedParameters = parameterFlags<true, true, false, true>;

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

  using usedParameters = parameterFlags<false, false, true, true>;

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

  using usedParameters = parameterFlags<true, true, true, true>;

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
}

template <typename Q, unsigned int od,
          template <typename, unsigned int> class formula>
class parametricIterator : public std::iterator<std::forward_iterator_tag,
  typename object<Q, od, formula<Q, od>::renderDepth, 4,
    typename formula<Q, od>::format>::face>
{
protected:
  using source = formula<Q, od>;
  using parent = object<Q, od, source::renderDepth, 4, typename source::format>;
  using face = typename parent::face;
  using baseObject = cube<Q, od>;
  using faces = std::vector<typename baseObject::face>;
  using vector = math::vector<Q, od>;

public:
  parametricIterator(const parameters<Q> &pParameter)
    : parameter(pParameter) {
    for (unsigned int dim = 0; dim < od; dim++) {
      range<Q> qs = source::getRange(parameter, dim);
      positions.push_back(qs.begin());
      starts.push_back(qs.begin());
      ends.push_back(qs.end());
      strides[dim] = qs.stride;
    }

    base = getBase(strides);
    basePosition = 0;
  }

  parametricIterator &end(void) {
    for (unsigned int dim = 0; dim < od; dim++) {
      positions[dim] = (dim == 0) ? ends[dim] : starts[dim];
    }
    return *this;
  }

  const face operator*(void) const {
    auto f = base[basePosition];
    face g;
    auto o = g.begin();
    for (auto &p : f) {
      *o = std::array<Q, parent::renderDepth>(
          source::getCoordinates(parameter, getPosition() + p));
      o++;
    }
    return g;
  }

  parametricIterator &operator++(void) {
    basePosition++;

    if (basePosition < base.size()) {
      return *this;
    } else {
      basePosition = 0;
    }

    for (int dim = (od-1); dim >= 0; dim--) {
      if (positions[dim] < ends[dim]) {
        positions[dim]++;
      }

      if (positions[dim] < ends[dim]) {
        break;
      } else if (dim > 0) {
        positions[dim] = starts[dim];
      }
    }
    return *this;
  }

  parametricIterator operator++(int) {
    parametricIterator c = *this;
    ++(*this);
    return c;
  }

  constexpr bool operator!=(const parametricIterator &b) const {
    return !(*this == b);
  }

  constexpr bool operator==(const parametricIterator &b) const {
    return (getPosition() == b.getPosition())
        && (strides == b.strides)
        && (basePosition == b.basePosition);
  }

protected:
  std::vector<typename range<Q>::iterator> positions;
  std::vector<typename range<Q>::iterator> starts;
  std::vector<typename range<Q>::iterator> ends;
  vector strides;
  faces base;
  std::size_t basePosition;
  parameters<Q> parameter;

  static const faces getBase(const vector &strides) {
    parameters<Q> cubeParameter;
    cube<Q, od> cube(cubeParameter);
    faces base;

    const Q r2 = cubeParameter.radius / Q(2);

    for (auto f : cube) {
      auto cf = f;
      for (auto &v : cf) {
        auto s = strides.begin();
        for (auto &q : v) {
          q = (q + r2) * (*s);
          s++;
        }
      }
      base.push_back(cf);
    }

    return base;
  }

  const vector getPosition(void) const {
    vector r;
    for (unsigned int dim = 0; dim < od; dim++) {
      r[dim] = *(positions[dim]);
    }
    return r;
  }
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
class parametric : public object<Q, od, formula<Q, od>::renderDepth, 4,
                                 typename formula<Q, od>::format>
{
public:
  using source = formula<Q, od>;
  using dimensions = typename source::dimensions;

  using parent = object<Q, od, source::renderDepth, 4, typename source::format>;
  using typename parent::format;
  using typename parent::face;

  parametric(const parameters<Q> &pParameter, const format &pFormat)
    : parent(pParameter, pFormat) {}

  static constexpr const char *id(void) { return source::id(); }

  void calculateObject(void) {}

  using iterator = parametricIterator<Q, od, formula>;
  using usedParameters = typename source::usedParameters;

  constexpr iterator begin(void) const { return iterator(parent::parameter); }
  constexpr iterator end(void) const { return begin().end(); }

  std::size_t size(void) const {
#if 0
    std::size_t s = source::getRange(parent::parameter, 0).size();
    for (std::size_t i = 1; i < od; i++) {
      s *= source::getRange(parent::parameter, i).size();
    }
#else
    // TODO: this is really bad, but the closed form currently eludes me.
    std::size_t s = 0;
    for (auto &t : *this) {
      s++;
    }
#endif
    return s;
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
}
}

#endif
