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

#if !defined(EF_GY_IFS_H)
#define EF_GY_IFS_H

#include <ef.gy/polytope.h>
#include <ef.gy/parametric.h>
#include <ef.gy/projection.h>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <random>
#include <algorithm>

namespace efgy {
namespace geometry {
template <typename Q, unsigned int od, unsigned int d,
          template <class, unsigned int> class primitive, unsigned int pd,
          template <class, unsigned int> class trans>
class ifs : public object<Q, od, d, primitive<Q, pd>::faceVertices,
                          typename primitive<Q, pd>::format>
{
public:
  using basePrimitive = primitive<Q, od>;
  using translation = trans<Q, d>;

  using parent = object<Q, od, d, primitive<Q, pd>::faceVertices,
      typename primitive<Q, pd>::format>;

  using parent::parent;

  using typename parent::face;

  std::vector<translation> functions;

  class ifsIterator : public std::iterator<std::forward_iterator_tag, face> {
  public:
    ifsIterator(const parameters<Q> &pParameter,
                const std::vector<translation> &pFunctions)
      : functions(pFunctions),
        base(pParameter, typename basePrimitive::format()),
        basePosition(base.begin()), iterations(0),
        totalIterations(pParameter.iterations),
        limit(std::pow<Q>(pParameter.functions, pParameter.iterations))
    {
      base.calculateObject();
    }

    ifsIterator(const ifsIterator &it)
      : functions(it.functions),
        base(it.base),
        basePosition(base.begin()),
        iterations(it.iterations),
        totalIterations(it.totalIterations),
        limit(it.limit)
    {
      base.calculateObject();
      basePosition = base.begin();
    }

    static ifsIterator begin(const parameters<Q> &pParameter,
                             const std::vector<translation> &pFunctions) {
      ifsIterator it = ifsIterator(pParameter, pFunctions);
      it.basePosition = it.base.begin();
      return it;
    }

    static ifsIterator end(const parameters<Q> &pParameter,
                           const std::vector<translation> &pFunctions) {
      ifsIterator it = ifsIterator(pParameter, pFunctions);
      it.basePosition = it.base.end();
      it.iterations = it.limit;
      return it;
    }

    const face operator*(void) const {
      auto f = *basePosition;
      face g;
      auto o = g.begin();
      for (auto &p : f) {
        *o = p;
        for (const auto &i : iteration()) {
          *o = functions[i] * (*o);
        }
        o++;
      }
      return g;
    }

    ifsIterator &operator++(void) {
      if (basePosition != base.end()) {
        basePosition++;
      }

      if (basePosition == base.end()) {
        basePosition = base.begin();
        iterations++;
      }

      return *this;
    }

    ifsIterator operator++(int) {
      ifsIterator c = *this;
      ++(*this);
      return c;
    }

    bool isEnd(void) const {
      return iterations >= limit;
    }

    bool operator!=(const ifsIterator &b) const {
      return !(*this == b);
    }
    
    bool operator==(const ifsIterator &b) const {
      return (isEnd() && b.isEnd())
          || ((iterations == b.iterations)
            && (basePosition == b.basePosition));
    }

  protected:
    using baseIterator = typename basePrimitive::iterator;

    basePrimitive base;
    baseIterator basePosition;
    std::vector<translation> functions;

    std::size_t iterations;
    std::size_t totalIterations;
    std::size_t limit;

    std::vector<std::size_t> iteration(void) const {
      std::vector<std::size_t> rv = {};
      std::size_t n = iterations;
      for (std::size_t i = 0; i < totalIterations; i++) {
        rv.push_back(n % functions.size());
        n /= functions.size();
      }
      std::reverse(rv.begin(), rv.end());
      return rv;
    }
  };

  using iterator = ifsIterator;

  iterator begin(void) const {
    return iterator::begin(parent::parameter, functions);
  }

  iterator end(void) const {
    return iterator::end(parent::parameter, functions);
  }

  std::size_t size(void) const {
    basePrimitive base(parent::parameter,
                       typename basePrimitive::format());
    base.calculateObject();
    return base.size() * std::pow<Q>(parent::parameter.functions,
                                     parent::parameter.iterations);
  }
};

namespace sierpinski {
template <typename Q, unsigned int od>
class gasket : public ifs<Q, od, od, cube, od, transformation::affine> {
public:
  typedef ifs<Q, od, od, cube, od, transformation::affine> parent;
  using typename parent::format;

  gasket(const parameters<Q> &pParameter, const format &pFormat)
      : parent(pParameter, pFormat) {
    const unsigned int nfunctions = (1 << (od - 1)) + 1;
    std::array<math::vector<Q, parent::renderDepth>, nfunctions> translations;

    translations[0][0] = Q(0.25);

    for (unsigned int i = 1; i < nfunctions; i++) {
      translations[i][0] = Q(-0.25);
      for (unsigned int j = 1; j < od; j++) {
        const unsigned int k = i - 1;
        const unsigned int l = j - 1;
        const unsigned int b = 1 << l;
        const bool s = k & b;
        translations[i][j] = Q(s ? -0.25 : 0.25);
      }
    }

    for (unsigned int i = 0; i < nfunctions; i++) {
      functions.push_back(
          transformation::scale<Q, parent::renderDepth>(0.5) *
          transformation::translation<Q, parent::renderDepth>(translations[i]));
    }
  }

  using parent::parameter;
  using parent::functions;

  using dimensions = dimensions<2, 0>;

  static constexpr const char *id(void) { return "sierpinski-gasket"; }
};

template <typename Q, unsigned int od>
class carpet : public ifs<Q, od, od, cube, od, transformation::affine> {
public:
  typedef ifs<Q, od, od, cube, od, transformation::affine> parent;
  using typename parent::format;

  carpet(const parameters<Q> &pParameter, const format &pFormat)
      : parent(pParameter, pFormat) {
    const unsigned int nfunctions = od == 2 ? 8 : 20;
    std::array<math::vector<Q, parent::renderDepth>, nfunctions> translations;

    if (od > 1) {
      translations[0][0] = Q(-1) / Q(3);
      translations[0][1] = Q(-1) / Q(3);
      translations[1][0] = Q(-1) / Q(3);
      translations[1][1] = Q(0);
      translations[2][0] = Q(-1) / Q(3);
      translations[2][1] = Q(1) / Q(3);
      translations[3][0] = Q(1) / Q(3);
      translations[3][1] = Q(-1) / Q(3);
      translations[4][0] = Q(1) / Q(3);
      translations[4][1] = Q(0);
      translations[5][0] = Q(1) / Q(3);
      translations[5][1] = Q(1) / Q(3);
      translations[6][0] = Q(0);
      translations[6][1] = Q(-1) / Q(3);
      translations[7][0] = Q(0);
      translations[7][1] = Q(1) / Q(3);
    }
    if (od > 2) {
      for (int i = 0; i < 8; i++) {
        translations[(i + 8)] = translations[i];
        translations[i][2] = Q(-1) / Q(3);
        translations[(i + 8)][2] = Q(1) / Q(3);
      }

      translations[16][0] = Q(1) / Q(3);
      translations[16][1] = Q(1) / Q(3);

      translations[17][0] = Q(-1) / Q(3);
      translations[17][1] = Q(1) / Q(3);

      translations[18][0] = Q(1) / Q(3);
      translations[18][1] = Q(-1) / Q(3);

      translations[19][0] = Q(-1) / Q(3);
      translations[19][1] = Q(-1) / Q(3);
    }

    for (unsigned int i = 0; i < nfunctions; i++) {
      functions.push_back(
          transformation::scale<Q, parent::renderDepth>(Q(1) / Q(3)) *
          transformation::translation<Q, parent::renderDepth>(translations[i]));
    }
  }

  using parent::parameter;
  using parent::functions;

  using dimensions = dimensions<2, 3>;

  static constexpr const char *id(void) { return "sierpinski-carpet"; }
};
}

namespace transformation {
template <typename Q, unsigned int d> class randomAffine : public affine<Q, d> {
public:
  randomAffine(const parameters<Q> &pParameter, const unsigned long long &pSeed)
      : parameter(pParameter), seed(pSeed) {
    updateMatrix();
  }

  void updateMatrix(void) {
    std::mt19937 PRNG((typename std::mt19937::result_type)seed);

    math::vector<Q, d> V;
    const Q s(Q(PRNG() % 6000) / Q(10000) + Q(.2));
    const Q r1(Q(PRNG() % 20000) / Q(10000) * Q(M_PI));
    unsigned int a1 = PRNG() % d;
    unsigned int a2 = PRNG() % d;
    const Q r2(Q(PRNG() % 20000) / Q(10000) * Q(M_PI));
    unsigned int a4 = PRNG() % d;
    unsigned int a5 = PRNG() % d;

    if (a1 > a2) {
      unsigned int a3 = a1;
      a1 = a2;
      a2 = a3;
    } else if (a1 == a2) {
      if (a1 == 0) {
        a2 = PRNG() % (d - 1 >= 1 ? d - 1 : 1) + 1;
      } else {
        a1--;
      }
    }

    if (a4 > a5) {
      unsigned int a6 = a4;
      a4 = a5;
      a5 = a6;
    } else if (a4 == a5) {
      if (a4 == 0) {
        a5 = PRNG() % (d - 1 >= 1 ? d - 1 : 1) + 1;
      } else {
        a4--;
      }
    }

    for (unsigned int j = 0; j < d; j++) {
      V[j] = Q(PRNG() % 10000) / Q(5000) - Q(1);
    }

    transformationMatrix =
        (transformation::scale<Q, d>(s) *
         (parameter.preRotate ? transformation::affine<Q, d>(
                                    transformation::rotation<Q, d>(r1, a1, a2))
                              : transformation::affine<Q, d>()) *
         transformation::translation<Q, d>(V) *
         (parameter.postRotate ? transformation::affine<Q, d>(
                                     transformation::rotation<Q, d>(r2, a4, a5))
                               : transformation::affine<Q, d>()))
            .transformationMatrix;
  }

  using affine<Q, d>::transformationMatrix;

protected:
  const parameters<Q> &parameter;
  const unsigned long long seed;
};
}

template <typename Q, unsigned int d>
using extendedCube = adapt<Q, d, cube<Q, 2>, typename cube<Q, 2>::format>;

template <typename Q, unsigned int d>
using extendedPlane = adapt<Q, d, plane<Q, 2>, typename plane<Q, 2>::format>;

template <typename Q, unsigned int od>
class randomAffineIFS
    : public ifs<Q, od, od, extendedCube, od, transformation::affine> {
public:
  typedef ifs<Q, od, od, extendedCube, od, transformation::affine> parent;
  using typename parent::format;

  randomAffineIFS(const parameters<Q> &pParameter, const format &pFormat)
      : parent(pParameter, pFormat) {
    calculateObject();
  }

  void calculateObject(void) {
    functions.clear();

    std::mt19937 PRNG(parameter.seed);

    for (unsigned int i = 0; i < parameter.functions; i++) {
      functions.push_back(
          transformation::randomAffine<Q, parent::renderDepth>(
              parameter, PRNG()));
    }
  }

  using parent::parameter;
  using parent::functions;

  using dimensions = dimensions<2, 0>;

  static constexpr const char *id(void) { return "random-affine-ifs"; }
};
}
}

#endif
