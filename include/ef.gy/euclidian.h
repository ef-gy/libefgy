/**\file
 * \brief Common operations in Euclidian space
 *
 * The templates in this header provide some of the common operations on
 * euclidian space vectors. We implicitly assume that 'n-dimensional real
 * vector space' and 'n-dimensional euclidian space' refer to the same thing, so
 * these operations simply work on any math::vector instances that use the
 * default (real) space tag.
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

#if !defined(EF_GY_EUCLIDIAN_H)
#define EF_GY_EUCLIDIAN_H

#include <ef.gy/matrix.h>
#include <ef.gy/vector.h>

#include <array>
#include <cmath>

namespace efgy {
namespace math {
/**\brief Calculate squared length of a vector
 *
 * Calculates the squared length of a vector with the default Euclidian
 * metric; It might seem weird to use this instead of the proper length,
 * but calculating the proper length of a vector involves taking a
 * square root, which tends to be a rather involved operation and is not
 * really necessary when merely comparing the length of two vectors.
 *
 * \tparam F The base type for calculations.
 * \tparam n The number of dimensions of the input vector.
 *
 * \param[in] pV The vector to calculate the length of.
 *
 * \returns The squared length of the given vector.
 */
template <typename F, unsigned int n>
constexpr F lengthSquared(const math::vector<F, n> &pV) {
  return pV * pV;
}

/**\brief Calculate length of a vector
 *
 * Calculates the proper length of a vector with the default Euclidian
 * metric. As noted in lengthSquared(), if you only need to compare
 * vectors to sort them by length, then the square root that this
 * function involves is overkill.
 *
 * \tparam F The base type for calculations.
 * \tparam n The number of dimensions of the input vector.
 *
 * \param[in] pV The vector to calculate the length of.
 *
 * \returns The squared length of the given vector.
 */
template <typename F, unsigned int n>
constexpr F length(const math::vector<F, n> &pV) {
  return std::sqrt(lengthSquared(pV));
}

/**\brief Calculate unit vector
 *
 * Given an input vector, this function returns the unit vector,
 * sometimes also called the 'norm', not to be confused with the a
 * 'normal', which is a vector pointing in the same direction but being
 * of length '1' - or of 'unit length'.
 *
 * \tparam F The base type for calculations.
 * \tparam n The number of dimensions of the input vector.
 *
 * \param[in] pV The vector to calculate the unit vector of.
 *
 * \returns The unit vector for the given input vector.
 */
template <typename F, unsigned int n>
math::vector<F, n> normalise(const math::vector<F, n> &pV) {
  math::vector<F, n> rv;
  const F l = length(pV);

  for (unsigned int i = 0; i < n; i++) {
    rv[i] = pV[i] / l;
  }

  return rv;
}

/**\brief Calculate cross product
 *
 * Given two 3-space real vectors, calculate the cross product of the
 * two. The cross product is only defined in 3 and 7 dimensions; see the
 * math::normal and math::perpendicular functions for the operation that
 * is commonly intended when using the cross product, which are also
 * defined in higher-dimensional euclidian spaces.
 *
 * \tparam F The base type for calculations
 *
 * \param[in] a First input vector.
 * \param[in] b Second input vector.
 *
 * \returns The cross product of the two input vectors.
 */
template <typename F>
constexpr math::vector<F, 3> crossProduct(const math::vector<F, 3> &a,
                                          const math::vector<F, 3> &b) {
  return {{a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2],
           a[0] * b[1] - a[1] * b[0]}};
}

/**\brief Calculate normal
 *
 * Given any (d-1) d-space vectors, this function will calculate one of
 * the vectors that is perpendicular to all of them. Since there's no
 * such thing as a cross product in arbitrary real spaces, we use an
 * implicit Laplace expansion of a specially crafted matrix to calculate
 * the normal.
 *
 * \note We only calculate the first step of the Laplace expansion
 *       manually. However, since the matrix code's determinant()
 *       also uses a Laplace expansion at this point, the code will run
 *       very slowly with 10-space vectors or higher.
 *
 * \tparam Q The base type for calculations
 * \tparam d The number of dimensions of the input vectors.
 *
 * \param[in] pV An array of (d-1) d-space vectors.
 *
 * \returns One of the vectors perpendicular to the given input vectors.
 *
 * \see http://ef.gy/linear-algebra:normal-vectors-in-higher-dimensional-spaces
 *      for an explanation of the algorithm.
 */
template <typename Q, unsigned int d>
math::vector<Q, d> normal(const std::array<math::vector<Q, d>, d - 1> &pV) {
  math::matrix<Q, d, d> pM;
  std::array<math::vector<Q, d>, d> baseVectors;

  for (unsigned int i = 0; i < (d - 1); i++) {
    for (unsigned int j = 0; j < d; j++) {
      pM[i][j] = pV[i][j];
      baseVectors[i][j] = ((i == j) ? 1 : 0);
    }
  }

  for (unsigned int j = 0; j < d; j++) {
    const unsigned int i = d - 1;
    baseVectors[i][j] = ((i == j) ? 1 : 0);
  }

  math::vector<Q, d> rv = math::vector<Q, d>();

  for (unsigned int i = 0; i < d; i++) {
    math::matrix<Q, d - 1, d - 1> pS;

    for (unsigned int j = 0, r = 0; j < (d - 1); j++, r++) {
      for (unsigned int k = 0, c = 0; k < d; k++) {
        if (k == i) {
          continue;
        }

        pS[r][c] = pM[j][k];

        c++;
      }
    }

    if ((i % 2) == 0) {
      rv += baseVectors[i] * math::determinant(pS);
    } else {
      rv -= baseVectors[i] * math::determinant(pS);
    }
  }

  return rv;
}

/**\brief Calculate normal (3-space)
 *
 * Given any two 3D real-space vectors, this function will calculate one
 * of their normals and return it by means of the 3-space cross product.
 * This function is identical to math::perpendicular, except that the
 * input type matches that of the arbitrary normal function.
 *
 * \tparam Q The base type for calculations
 *
 * \param[in] pV An array of two 3-space vectors.
 *
 * \returns One of the vectors perpendicular to the given input vectors.
 */
template <typename Q>
constexpr math::vector<Q, 3> normal(
    const std::array<math::vector<Q, 3>, 2> &pV) {
  return crossProduct(pV[0], pV[1]);
}

/**\brief Calculate perpendicular vector (2-space)
 *
 * Given any one 2D real-space vector, this function will return one of
 * its two perpendicular vectors. In 2-space, this is calculated by
 * something that is colloquially referred to as the '2D cross product',
 * even though 2-space does not support proper cross products.
 *
 * \tparam Q The base type for calculations
 *
 * \param[in] v The vector to calculate the "cross product" of.
 *
 * \returns One of the perpendicular vectors of the input vector.
 */
template <typename Q>
constexpr math::vector<Q, 2> perpendicular(const math::vector<Q, 2> &v) {
  return {{v[1] * Q(-1), v[0]}};
}

/**\brief Calculate perpendicular vector (3-space)
 *
 * Given any two 3D real-space vectors, this function will return one of
 * the two perpendicular vectors to these vectors. In 3-space, this is
 * calculated by means of the 3D cross product.
 *
 * \tparam Q The base type for calculations
 *
 * \param[in] v1 First input vector.
 * \param[in] v2 Second input vector.
 *
 * \returns One of the perpendicular vectors of the two input vectors.
 */
template <typename Q>
constexpr math::vector<Q, 3> perpendicular(const math::vector<Q, 3> &v1,
                                           const math::vector<Q, 3> &v2) {
  return crossProduct(v1, v2);
}
};  // namespace math
};  // namespace efgy

#endif
