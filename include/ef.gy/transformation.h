/**\file
 * \brief Handles vector transformations.
 *
 * Contains templates for transformations of vector space elements.
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

#if !defined(EF_GY_TRANSFORMATION_H)
#define EF_GY_TRANSFORMATION_H

#include <ef.gy/euclidian.h>
#include <ef.gy/matrix.h>

namespace efgy {
namespace geometry {
/**\brief Contains vector transformation templates
 *
 * This namespace groups all templates that transform vector space
 * elements to other vector space elements. Or, in non-mathemese:
 * things that turn 3D coordinates to 2D coordinates, or things that
 * move stuff around in 3D space. Except it's not just 3D and 2D
 * vectors that we'd want to work with.
 */
namespace transformation {
namespace generator {
template <typename Q, std::size_t, std::size_t>
class identity {
 public:
  const Q operator()(const std::size_t &i, const std::size_t &j) const {
    return (i == j) ? Q(1) : Q(0);
  }
};

template <typename Q, std::size_t d, std::size_t>
class scale {
 public:
  scale(void) : targetScale(Q(1)) {}

  const Q operator()(const std::size_t &i, const std::size_t &j) const {
    if (i == j) {
      return i != (d - 1) ? targetScale : Q(1);
    } else {
      return Q(0);
    }
  }

  Q targetScale;
};

template <typename Q, std::size_t d, std::size_t>
class rotate {
 public:
  rotate(void) : angle(Q(0)), axis1(0), axis2(1) {}

  const Q operator()(const std::size_t &i, const std::size_t &j) const {
    bool transpose = (axis1 + axis2 + d) % 2 == 1;

    if ((i == axis1) && (j == axis1)) {
      return cos(angle);
    } else if ((i == axis1) && (j == axis2)) {
      return transpose ? sin(angle) : -sin(angle);
    } else if ((i == axis2) && (j == axis2)) {
      return cos(angle);
    } else if ((i == axis2) && (j == axis1)) {
      return transpose ? -sin(angle) : sin(angle);
    } else if (i == j) {
      return Q(1);
    } else {
      return Q(0);
    }
  }

  Q angle;
  std::size_t axis1;
  std::size_t axis2;
};

template <typename Q, std::size_t d, std::size_t>
class translate {
 public:
  const Q operator()(const std::size_t &i, const std::size_t &j) const {
    if ((i == (d - 1)) && (j < (d - 1))) {
      return from[j];
    } else if (i == j) {
      return Q(1);
    } else {
      return Q(0);
    }
  }

  math::vector<Q, d - 1> from;
};
}

/**\brief Template for linear maps on the vector space Q^d
 *
 * Handles linear maps, or endomorphisms, on Q^d. Maps are
 * described via their transformation matrices, using the
 * standard basis on Q^d.
 *
 * \tparam Q The data type that describes the underlying field of
 * the vector space.
 *
 * \tparam d The dimension of the vector space.
 */
template <typename Q, std::size_t d>
class linear {
 public:
  /**\brief Constructor for the identity map
   *
   * Constructs a transformation whose matrix is the identity matrix.
   */
  linear(void) : matrix(math::ghost::matrix<Q, d, d, generator::identity>()) {}

  /**\brief Constructor to copy a matrix
   *
   * Copies an appropriately-sized matrix
   */
  linear(const math::matrix<Q, d, d> &pMatrix) : matrix(pMatrix) {}

  /**\brief Applies a transformation to a vector.
   *
   * Applies a transformation to a vector by
   * multiplying the transformation matrix to it.
   *
   * \tparam format The vector format to use.
   *
   * \param pV a vector from Q^d
   *
   * \returns The transformed vector, which is also
   * in Q^d.
   */
  template <typename format>
  math::vector<Q, d, format> operator*(
      const math::vector<Q, d, format> &pV) const {
    math::vector<Q, d, format> rv;

    math::matrix<Q, 1, d> vectorMatrix;

    for (std::size_t i = 0; i < d; i++) {
      vectorMatrix[0][i] = pV[i];
    }

    vectorMatrix = vectorMatrix * matrix;

    for (std::size_t i = 0; i < d; i++) {
      rv[i] = vectorMatrix[0][i];
    }

    return rv;
  }

  /*\brief The transformation matrix.
   *
   * The transformation matrix of a linear map.
   */
  math::matrix<Q, d, d> matrix;
};

/* \brief Affine transformation on Q^d
 *
 * Template for affine transformations on Q^d. An affine transformation
 * is considered to be composed of a translation and a linear map.
 *
 * \tparam Q The underlying field of the corresponding
 * vector space
 *
 * \tparam d The dimension of the corresponding vector space
 */
template <typename Q, std::size_t d>
class affine {
 public:
  /* \brief Constructor for the identity transformation
   *
   * Constructor for the identity transformation
   */
  affine(void)
      : matrix(math::ghost::matrix<Q, d + 1, d + 1, generator::identity>()) {}

  affine(const math::matrix<Q, d + 1, d + 1> &m) : matrix(m) {}

  /* \brief Constructs an affine transformation from a linear map
   *
   * Constructs an affine transformation from a linear map,
   * with a translation of zero.
   *
   * \param L a linear map on Q^d
   */
  affine(const linear<Q, d> &L) {
    for (std::size_t i = 0; i < d; i++) {
      for (std::size_t j = 0; j < d; j++) {
        matrix[i][j] = L.matrix[i][j];
      }
    }
    for (std::size_t i = 0; i < d; i++) {
      matrix[i][d] = Q(0);
      matrix[d][i] = Q(0);
    }
    matrix[d][d] = Q(1);
  }

  /* \brief Applies a transformation to a vector.
   *
   * Applies an affine transformation to a vector.
   *
   * \tparam format The vector format to use.
   * \param pV The vector the transformation is applied to.
   *
   * \returns The transformed vector.
   */
  template <typename format>
  math::vector<Q, d, format> operator*(
      const math::vector<Q, d, format> &pV) const {
    math::vector<Q, d, format> rv;

    math::matrix<Q, 1, d + 1> vectorMatrix;

    for (std::size_t i = 0; i < d; i++) {
      vectorMatrix[0][i] = pV[i];
    }

    vectorMatrix[0][d] = Q(1);

    vectorMatrix = vectorMatrix * matrix;

    for (std::size_t i = 0; i < d; i++) {
      rv[i] = vectorMatrix[0][i] / vectorMatrix[0][d];
    }

    return rv;
  }

  template <typename format>
  math::vector<Q, d, format> operator*(
      const math::vector<Q, d + 1, format> &pV) const {
    math::vector<Q, d, format> rv;

    math::matrix<Q, 1, d + 1> vectorMatrix;

    for (std::size_t i = 0; i <= d; i++) {
      vectorMatrix[0][i] = pV[i];
    }

    vectorMatrix = vectorMatrix * matrix;

    for (std::size_t i = 0; i < d; i++) {
      rv[i] = vectorMatrix[0][i] / vectorMatrix[0][d];
    }

    return rv;
  }

  math::matrix<Q, d + 1, d + 1> matrix;
};

template <typename Q, std::size_t d>
class projective : public affine<Q, d> {
 public:
  using affine<Q, d>::affine;

  template <typename format>
  math::vector<Q, d - 1, format> operator*(
      const math::vector<Q, d, format> &pP) const {
    math::vector<Q, d - 1, format> result;

    math::vector<Q, d> R = affine<Q, d>(*this) * pP;

    for (std::size_t i = 0; i < (d - 1); i++) {
      result[i] = R[i] / R[(d - 1)];
    }

    return result;
  }

  template <typename format>
  math::vector<Q, d - 1, format> operator*(
      const math::vector<Q, d + 1, format> &pP) const {
    math::vector<Q, d - 1, format> result;

    math::vector<Q, d> R = affine<Q, d>(*this) * pP;

    for (std::size_t i = 0; i < (d - 1); i++) {
      result[i] = R[i] / R[(d - 1)];
    }

    return result;
  }

  using affine<Q, d>::matrix;
};

/*\brief Composes two linear maps.
 *
 * Composes two linear maps on Q^d by multiplying their transformation matrices.
 *
 * \param a The object providing the left-hand side of the composition.
 * \param b The object providing the right-hand side of the composition.
 *
 * \returns An affine transformation that is the composite of the two
 *    transformations.
 */
template <typename Q, std::size_t d>
linear<Q, d> operator*(const linear<Q, d> &a, const linear<Q, d> &b) {
  return linear<Q, d>(a.matrix * b.matrix);
}

/*\brief Composes a linear and a projective map.
 *
 * Composes a linear and a projective transformation.
 *
 * \param a A linear transformation.
 * \param b A projective transformation.
 *
 * \returns A projective transformation that is the composition of a and b.
 */
template <typename Q, std::size_t d>
projective<Q, d> operator*(const linear<Q, d> &a, const projective<Q, d> &b) {
  return b * a;
}

/* \brief Composes two affine transformations.
 *
 * Composes two affine transformations by multiplying their transformation
 * matrices.
 *
 * \param a The object providing the left-hand side of the composition.
 * \param b The object providing the right-hand side of the composition.
 *
 * \returns An affine transformation that is the composite of the two
 *    transformations.
 */
template <typename Q, std::size_t d>
affine<Q, d> operator*(const affine<Q, d> &a, const affine<Q, d> &b) {
  return affine<Q, d>(a.matrix * b.matrix);
}

/* \brief Composes an affine and a projective transformation.
 *
 * Composes an affine and projective transformation by multiplying their
 * transformation matrices.
 *
 * \param a The object providing the left-hand side of the composition.
 * \param b The object providing the right-hand side of the composition.
 *
 * \returns A projective transformation that is the composite of the two
 *    transformations.
 */
template <typename Q, std::size_t d>
projective<Q, d> operator*(const affine<Q, d> &a, const projective<Q, d> &b) {
  return projective<Q, d>(a.matrix * b.matrix);
}

template <typename Q, std::size_t d>
projective<Q, d> operator*(const projective<Q, d> &a,
                           const projective<Q, d> &b) {
  return projective<Q, d>(a.matrix * b.matrix);
}

template <typename Q, std::size_t d>
projective<Q, d> operator*(const projective<Q, d> &a, const affine<Q, d> &b) {
  return projective<Q, d>(a.matrix * b.matrix);
}

/* \brief The identity map on Q^d.
 *
 * Wrapper class for the identity map on Q^d.
 *
 * \tparam Q The underlying field of the vector space.
 * \tparam d The dimension of the vector space
 *
 * \bug Assignments to the transformation matrix of objects of this class are
 *     possible, so an object to this class can have a non-identity
 *     transformation matrix (and thus not behave like an identity map).
 */
template <typename Q, std::size_t d>
class identity : public linear<Q, d> {
 public:
  identity() : linear<Q, d>() {}
};

template <typename Q, std::size_t d>
class scale : public affine<Q, d> {
 public:
  scale(const Q &pScale) {
    ghost.generator.targetScale = pScale;
    affine<Q, d>::matrix = ghost;
  }

 protected:
  math::ghost::matrix<Q, d + 1, d + 1, generator::scale> ghost;
};

template <typename Q, std::size_t d>
class rotation : public affine<Q, d> {
 public:
  rotation(const Q &pAngle, const std::size_t &pAxis1,
           const std::size_t &pAxis2) {
    ghost.generator.angle = pAngle;
    ghost.generator.axis1 = pAxis1;
    ghost.generator.axis2 = pAxis2;
    affine<Q, d>::matrix = ghost;
  }

 protected:
  math::ghost::matrix<Q, d + 1, d + 1, generator::rotate> ghost;
};

template <typename Q, std::size_t d>
class translation : public affine<Q, d> {
 public:
  translation(const math::vector<Q, d> &pFrom) : from(ghost.generator.from) {
    from = pFrom;
    affine<Q, d>::matrix = ghost;
  }

 protected:
  math::ghost::matrix<Q, d + 1, d + 1, generator::translate> ghost;
  math::vector<Q, d> &from;
};
}
}
}

#endif
