/**\file
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Cdepthe: https://github.com/ef-gy/libefgy
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
namespace transformation {
template <typename Q, std::size_t d> class randomAffine : public affine<Q, d> {
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

    for (std::size_t j = 0; j < d; j++) {
      V[j] = Q(PRNG() % 10000) / Q(5000) - Q(1);
    }

    matrix =
        (transformation::scale<Q, d>(s) *
         (parameter.preRotate ? transformation::affine<Q, d>(
                                    transformation::rotation<Q, d>(r1, a1, a2))
                              : transformation::affine<Q, d>()) *
         transformation::translation<Q, d>(V) *
         (parameter.postRotate ? transformation::affine<Q, d>(
                                     transformation::rotation<Q, d>(r2, a4, a5))
                               : transformation::affine<Q, d>()))
            .matrix;
  }

  using affine<Q, d>::matrix;

protected:
  const parameters<Q> &parameter;
  const unsigned long long seed;
};
}

namespace generators {
template <typename Q, std::size_t depth, std::size_t renderDepth>
class gasket {
public:
  using translation = transformation::affine<Q, renderDepth>;
  using dimensions = dimensions<2, 0>;
  static constexpr const char *id(void) { return "sierpinski-gasket"; }

  using scale = transformation::scale<Q, renderDepth>;
  using translate = transformation::translation<Q, renderDepth>;

  static std::vector<translation> functions(const parameters<Q> &parameter) {
    std::vector<translation> rv = {};

    const std::size_t nfunctions = (1 << (depth - 1)) + 1;
    std::array<math::vector<Q, renderDepth>, nfunctions> translations;

    translations[0][0] = Q(0.25);

    for (std::size_t i = 1; i < translations.size(); i++) {
      translations[i][0] = Q(-0.25);
      for (std::size_t j = 1; j < depth; j++) {
        const std::size_t k = i - 1;
        const std::size_t l = j - 1;
        const std::size_t b = 1 << l;
        const bool s = k & b;
        translations[i][j] = Q(s ? -0.25 : 0.25);
      }
    }

    for (const auto &t : translations) {
      rv.push_back(scale(0.5) * translate(t));
    }

    return rv;
  }

  static constexpr std::size_t size(const parameters<Q> &) {
    return (1 << (depth - 1)) + 1;
  }
};

template <typename Q, std::size_t depth, std::size_t renderDepth>
class carpet {
public:
  using translation = transformation::affine<Q, renderDepth>;
  using dimensions = dimensions<2, 3>;
  static constexpr const char *id(void) { return "sierpinski-carpet"; }

  using scale = transformation::scale<Q, renderDepth>;
  using translate = transformation::translation<Q, renderDepth>;

  static std::vector<translation> functions(const parameters<Q> &parameter) {
    std::vector<translation> rv = {};

    const std::size_t nfunctions = depth == 2 ? 8 : 20;
    std::array<math::vector<Q, renderDepth>, nfunctions> translations;

    if (depth > 1) {
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
    if (depth > 2) {
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

    for (const auto &t : translations) {
      rv.push_back(scale(Q(1) / Q(3)) * translate(t));
    }

    return rv;
  }

  static constexpr std::size_t size(const parameters<Q> &) {
    return depth == 2 ? 8 : 20;
  }
};

template <typename Q, std::size_t depth, std::size_t renderDepth,
          template <class, std::size_t> class trans,
          template <class, std::size_t> class gen,
          const char *name>
class random {
public:
  using translation = trans<Q, renderDepth>;
  using dimensions = dimensions<2, 0>;
  static constexpr const char *id(void) { return name; }

  static std::vector<translation> functions(const parameters<Q> &parameter) {
    std::vector<translation> rv = {};

    std::mt19937 PRNG(parameter.seed);

    for (std::size_t i = 0; i < parameter.functions; i++) {
      rv.push_back(gen<Q, renderDepth>(parameter, PRNG()));
    }

    return rv;
  }

  static std::size_t size(const parameters<Q> &parameter) {
    return parameter.functions;
  }
};

static constexpr const char randomAffineIFSLabel[] = "random-affine-ifs";

template <typename Q, std::size_t depth, std::size_t renderDepth>
using randomAffine =
  random<Q, depth, renderDepth,
         transformation::affine, transformation::randomAffine,
         randomAffineIFSLabel>;
}

template <typename Q, std::size_t depth,
          template <class, std::size_t> class primitive,
          template <class, std::size_t, std::size_t> class gen>
class ifs : public object<Q, depth, primitive<Q, depth>::renderDepth,
                          primitive<Q, depth>::faceVertices,
                          typename primitive<Q, depth>::format>
{
public:
  using basePrimitive = primitive<Q, depth>;

  using format = typename basePrimitive::format;
  using parent =
    object<Q, depth,
           basePrimitive::renderDepth, basePrimitive::faceVertices, format>;
  using parent::renderDepth;

  using generator = gen<Q, depth, renderDepth>;
  using translation = typename generator::translation;

  using parent::parent;

  using typename parent::face;

  class iterator : public std::iterator<std::forward_iterator_tag, face> {
  public:
    iterator(const parameters<Q> &pParameter,
                const std::vector<translation> &pFunctions)
      : functions(pFunctions),
        base(pParameter, format()),
        basePosition(base.begin()), iterations(0),
        totalIterations(pParameter.iterations),
        limit(std::pow<Q>(functions.size(), pParameter.iterations)) {}

    iterator(const iterator &it)
      : functions(it.functions),
        base(it.base),
        basePosition(base.begin()),
        iterations(it.iterations),
        totalIterations(it.totalIterations),
        limit(it.limit)
    {
      basePosition = base.begin();
    }

    iterator &end(void) {
      iterations = limit;
      return *this;
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

    iterator &operator++(void) {
      if (basePosition != base.end()) {
        basePosition++;
      }

      if (basePosition == base.end()) {
        basePosition = base.begin();
        iterations++;
      }

      return *this;
    }

    iterator operator++(int) {
      iterator c = *this;
      ++(*this);
      return c;
    }

    bool isEnd(void) const {
      return iterations >= limit;
    }

    bool operator!=(const iterator &b) const {
      return !(*this == b);
    }
    
    bool operator==(const iterator &b) const {
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

  iterator begin(void) const {
    return iterator(parent::parameter,
                    generator::functions(parent::parameter));
  }

  iterator end(void) const {
    return begin().end();
  }

  std::size_t size(void) const {
    basePrimitive base(parent::parameter, format());
    return base.size() * std::pow<std::size_t>(
        generator::size(parent::parameter),
        parent::parameter.iterations);
  }

  using dimensions = typename generator::dimensions;
  static constexpr const char *id(void) { return generator::id(); }
};

namespace sierpinski {
template <typename Q, std::size_t depth>
using gasket = ifs<Q, depth, cube, generators::gasket>;

template <typename Q, std::size_t depth>
using carpet = ifs<Q, depth, cube, generators::carpet>;
}

template <typename Q, std::size_t depth>
using randomAffineIFS = ifs<Q, depth, cube, generators::randomAffine>;
}
}

#endif
