/**\file
 * \brief Fractal flame transformation algorithm
 *
 * This file implements the fractal flame IFS 'Variations' as described in the
 * fractal flame algorithm paper.
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 * \see The paper 'Fractal Flame Algorithm' by Scott Draves and Eric Reckase:
 *      http://flam3.com/flame_draves.pdf
 */

#if !defined(EF_GY_FLAME_H)
#define EF_GY_FLAME_H

#include <ef.gy/ifs.h>

namespace efgy {
namespace geometry {
namespace transformation {
/**\brief Fractal flame transformation
 *
 * These transformations are based on the 'Fractal Flame Algorithm' paper by
 * Scott Draves and Eric Reckase.
 *
 * \todo This implementation is still incomplete, and currently only implements
 *     the first 19 variations (up to "exponential"), so this could do with some
 *     extra love.
 *
 * \see http://flam3.com/flame_draves.pdf for the original paper.
 */
template <typename Q, std::size_t d>
class flame : public affine<Q, d> {
 public:
  flame(std::size_t pDepth = d) : depth(pDepth) {}

  using affine<Q, d>::matrix;

  math::vector<Q, d> operator*(const math::vector<Q, d> &pV) const {
    const math::vector<Q, d> V = affine<Q, d>(*this) * pV;
    math::vector<Q, d> rv = V * coefficient[0];

    for (int i : range<int>(1, coefficients, false)) {
      rv = rv + apply(i, V);
    }

    return rv;
  }

  static const std::size_t coefficients = 19;
  Q coefficient[coefficients];

 protected:
  math::vector<Q, d> apply(std::size_t f, const math::vector<Q, d> &V) const {
    math::vector<Q, d> rv;

    if (coefficient[f] <= Q(0)) {
      return rv;
    }

    // The commented variables are in the Fractal Flame
    // paper, but won't be used until the remaining
    // variations are implemented

    const Q theta = atan(V[0] / V[1]);
    // const Q phi   = atan(V[1]/V[0]);
    const Q r2 = math::lengthSquared(V);
    const Q r = sqrt(r2);
    const Q omega = Q(std::rand() % 2) * Q(M_PI);
    // const Q delta = (std::rand() % 2) == 1 ? Q(1) : Q(-1);
    // const Q psi   = Q(std::rand() % 10000) / Q(10000);

    switch (f) {
      case 0:  // "linear"
        rv = V;
        break;
      case 1:  // "sinusoidal"
        for (std::size_t i : range<std::size_t>(0, depth, depth, false))
          rv[i] = sin(V[i]);
        break;
      case 2:  // "spherical"
        rv = V / r2;
        break;
      case 3:  // "swirl"
      {
        const Q sinrsq = sin(r2);
        const Q cosrsq = cos(r2);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"
        for (std::size_t i : range<std::size_t>(0, depth, depth, false))
          if ((i % 2 == 0) && (i < (d - 1)))
            rv[i] = V[i] * sinrsq - V[(i + 1)] * cosrsq;
          else
            rv[i] = V[(i - 1)] * cosrsq + V[i] * sinrsq;
#pragma clang diagnostic pop
      } break;
      case 4:  // "horseshoe"
        rv = V;
        rv[0] = (V[0] - V[1]) * (V[0] + V[1]);
        rv[1] = Q(2) * V[0] * V[1];
        rv = rv / r;
        break;
      case 5:  // "polar"
        rv = V;
        rv[0] = theta / Q(M_PI);
        rv[1] = sqrt(math::lengthSquared(V)) - Q(1);
        break;
      case 6:  // "handkerchief"
        for (std::size_t i : range<std::size_t>(0, depth, depth, false))
          switch (i % 4) {
            case 0:
              rv[i] = sin(theta + r);
            case 1:
              rv[i] = cos(theta - r);
            case 3:
              rv[i] = sin(theta - r);
            case 4:
              rv[i] = cos(theta + r);
          }
        rv = rv * r;
        break;
      case 7:  // "heart"
        for (std::size_t i : range<std::size_t>(0, depth, depth, false))
          switch (i % 4) {
            case 0:
              rv[i] = sin(theta * r);
              break;
            case 1:
              rv[i] = -cos(theta * r);
              break;
            case 2:
              rv[i] = -sin(theta * r);
              break;
            case 3:
              rv[i] = cos(theta * r);
              break;
          }
        rv = rv * r;
        break;
      case 8:  // "disc"
        for (std::size_t i : range<std::size_t>(0, depth, depth, false))
          switch (i % 2) {
            case 0:
              rv[i] = sin(Q(M_PI) * r);
              break;
            case 1:
              rv[i] = cos(Q(M_PI) * r);
              break;
          }
        rv = rv * theta / Q(M_PI);
        break;
      case 9:  // "spiral"
        for (std::size_t i : range<std::size_t>(0, depth, depth, false))
          switch (i % 4) {
            case 0:
              rv[i] = cos(theta) + sin(r);
              break;
            case 1:
              rv[i] = sin(theta) - cos(r);
              break;
            case 2:
              rv[i] = cos(theta) - sin(r);
              break;
            case 3:
              rv[i] = sin(theta) + cos(r);
              break;
          }
        rv = rv / r;
        break;
      case 10:  // "hyperbolic"
        for (std::size_t i : range<std::size_t>(0, depth, depth, false))
          switch (i % 4) {
            case 0:
              rv[i] = sin(theta) / r;
              break;
            case 1:
              rv[i] = cos(theta) * r;
              break;
            case 2:
              rv[i] = sin(theta) * r;
              break;
            case 3:
              rv[i] = cos(theta) / r;
              break;
          }
        break;
      case 11:  // "diamond"
        for (std::size_t i : range<std::size_t>(0, depth, depth, false))
          switch (i % 2) {
            case 0:
              rv[i] = sin(theta) * cos(r);
              break;
            case 1:
              rv[i] = cos(theta) * sin(r);
              break;
          }
        break;
      case 12:  // "ex"
      {
        const Q p0 = sin(theta + r), p1 = cos(theta - r), p2 = sin(theta - r),
                p3 = cos(theta + r);
        for (std::size_t i : range<std::size_t>(0, depth, depth, false))
          switch (i % 4) {
            case 0:
              rv[i] = p0 * p0 * p0 + p1 * p1 * p1;
              break;
            case 1:
              rv[i] = p0 * p0 * p0 - p1 * p1 * p1;
              break;
            case 2:
              rv[i] = p2 * p2 * p2 + p3 * p3 * p3;
              break;
            case 3:
              rv[i] = p2 * p2 * p2 - p3 * p3 * p3;
              break;
          }
        rv = rv / r;
      } break;
      case 13:  // "julia"
      {
        const Q thpo = theta / Q(2) + omega;
        for (std::size_t i : range<std::size_t>(0, depth, depth, false))
          switch (i % 2) {
            case 0:
              rv[i] = cos(thpo);
              break;
            case 1:
              rv[i] = sin(thpo);
              break;
          }
        rv = rv * Q(sqrt(r));
      } break;
      case 14:  // "bent"
        for (std::size_t i : range<std::size_t>(0, depth, depth, false))
          switch ((i % 2) + ((V[0] < Q(0)) << 1) + ((V[1] < Q(0)) << 2)) {
            case 0:
              rv[i] = V[i];
              break;
            case 1:
              rv[i] = V[i];
              break;
            case 2:
              rv[i] = V[i] * Q(2);
              break;
            case 3:
              rv[i] = V[i];
              break;
            case 4:
              rv[i] = V[i];
              break;
            case 5:
              rv[i] = V[i] / Q(2);
              break;
            case 6:
              rv[i] = V[i] * Q(2);
              break;
            case 7:
              rv[i] = V[i] / Q(2);
              break;
          }
        break;
      case 15:  // "waves"
        for (std::size_t i : range<std::size_t>(0, depth, depth, false))
          if (i == (d - 1))
            rv[i] =
                V[i] * matrix[i][0] * sin(V[0] / (matrix[d][i] * matrix[d][i]));
          else
            rv[i] = V[i] * matrix[i][0] *
                    sin(V[(i + 1)] / (matrix[d][i] * matrix[d][i]));
        break;
      case 16:  // "fisheye"
        for (std::size_t i : range<std::size_t>(0, depth, depth, false))
          rv[i] = V[(d - i)];
        rv = rv * Q(2) / (r + Q(1));
        break;
      case 17:  // "popcorn"
        for (std::size_t i : range<std::size_t>(0, depth, depth, false))
          rv[i] = V[i] + matrix[d][i] * sin(tan(Q(3) * V[i]));
        break;
      case 18:  // "exponential"
        for (std::size_t i : range<std::size_t>(0, depth, depth, false))
          switch (i % 2) {
            case 0:
              rv[i] = cos(M_PI * V[1]);
              break;
            case 1:
              rv[i] = sin(M_PI * V[1]);
              break;
          }
        rv = rv * Q(exp(V[0] - Q(1)));
        break;
      default:
        return rv;
    }

    return rv * coefficient[f];
  }

  std::size_t depth;
};

template <typename Q, std::size_t d>
class randomFlame : public flame<Q, d> {
 public:
  randomFlame(const parameters<Q> &pParameter, const unsigned long long &pSeed)
      : flame<Q, d>(d), seed(pSeed) {
    std::mt19937 PRNG((typename std::mt19937::result_type)pSeed);

    matrix = randomAffine<Q, d>(pParameter, 0).matrix;

    for (std::size_t i = 0; i < coefficients; i++) {
      coefficient[i] = Q(PRNG() % 10000) / Q(10000);
    }

    for (std::size_t nonzero = pParameter.flameCoefficients + 1;
         nonzero > pParameter.flameCoefficients;) {
      nonzero = 0;
      for (std::size_t i = 0; i < coefficients; i++) {
        if (coefficient[i] > Q(0.)) {
          nonzero++;
        } else if (coefficient[i] < Q(0.)) {
          coefficient[i] = Q(0.);
        }
      }

      if (nonzero > pParameter.flameCoefficients) {
        coefficient[(PRNG() % coefficients)] = Q(0.);
      }
    }

    Q coefficientsum = coefficient[0];

    for (std::size_t i = 1; i < coefficients; i++) {
      coefficientsum = coefficientsum + coefficient[i];
    }

    for (std::size_t i = 0; i < coefficients; i++) {
      coefficient[i] = coefficient[i] / coefficientsum;
    }
  }

  using flame<Q, d>::matrix;
  using flame<Q, d>::coefficient;
  using flame<Q, d>::coefficients;

 protected:
  const unsigned long long &seed;
};
}

namespace generators {
static constexpr const char randomFlameIFSLabel[] = "random-flame";

template <typename Q, std::size_t depth, std::size_t renderDepth>
using randomFlame = random<Q, depth, renderDepth, transformation::flame,
                           transformation::randomFlame, randomFlameIFSLabel>;
}

template <typename Q, std::size_t d>
using extendedPlane = adapt<Q, d, plane<Q, 2>, typename plane<Q, 2>::format>;

namespace flame {
/**\ingroup libefgy-extended-geometric-primitives
 * \brief Random Fractal Flame primitive
 *
 * This class template creates random fractal flame-esque geometry
 * based on the current values of the seed and functions
 * parameters. Tis geometry can take all sorts of shapes, depending
 * on the transformations it creates; here's an example:
 *
 * \htmlonly
 * <iframe width="100%" height="600"
 *  src="http://www.youtube.com/embed/XzObtC66g3c"
 *  frameborder="0" allowfullscreen="true"/>
 * \endhtmlonly
 *
 * \tparam Q  The base data type to use for calculations.
 * \tparam od The depth of the model.
 */
template <typename Q, std::size_t od>
using random = ifs<Q, od, extendedPlane, generators::randomFlame>;
}
}
}

#endif
