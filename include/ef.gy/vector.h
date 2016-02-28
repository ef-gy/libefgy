/**\file
 * \brief Generic vectors
 *
 * Contains the basic template for generic vectors.
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

#if !defined(EF_GY_VECTOR_H)
#define EF_GY_VECTOR_H

#include <array>
#include <ostream>

namespace efgy {
namespace math {
/**\brief Coordinate format tags
 *
 * Contains tags for different coordinate formats. These are used in the
 * math::vector template to distinguish between different coordinate
 * formats, such as cartesian and polar coordinates, and sometimes also
 * between different semantics, such as geometric vector spaces versus
 * colour spaces.
 */
namespace format {
/**\brief Cartesian coordinate format tag
 *
 * This is the default coordinate format tag, which indicates that
 * the coordinates are cartesian coordinates.
 */
class cartesian {
public:
  /**\brief Query coordinate format ID
   *
   * This is the ID that corresponds to the coordinate format;
   * used to differentiate between format tags of different
   * types.
   */
  static const char *id(void) { return "cartesian"; }
};

/**\brief Stream output operator for the cartesian format tag
 *
 * Writes the contents of a cartesian coordinate format tag to an
 * output stream; since these tags do not contain any actual data,
 * this operator really just writes a string to the stream.
 *
 * \tparam C The character type of the stream.
 *
 * \param[out] stream The stream to write to.
 *
 + \returns The parameter 'stream' after writing to it.
 */
template <typename C>
constexpr inline std::basic_ostream<C> &
operator<<(std::basic_ostream<C> &stream, const cartesian &) {
  return stream << "[cartesian]";
}
};

/**\brief Generic vector
 *
 * Implements a generic vector type over a field, which is tagged with a
 * coordinate format that describes what kind of vector the class
 * represents.
 *
 * \tparam F      Base type for the vector; should have field-like
 *                properties.
 * \tparam n      Number of vector elements.
 * \tparam format Coordinate format tag, defaults to format::cartesian.
 */
template <typename F, unsigned int n, typename format = format::cartesian>
class vector : public std::array<F, n> {
public:
  /**\brief Construct with array
   *
   * Construct an instance of the vector with the elements
   * specified as a C++-style array.
   *
   * \param[in] t The array to copy.
   * \param[in] s An instance of the format tag; typically not
   *              used, but might be used to specify additional
   *              parameters.
   */
  constexpr vector(const std::array<F, n> &t = {{}}, const format &s = format())
      : std::array<F, n>(t) {}

  /**\brief Query format tag
   *
   * Returns an instance of the applicable format tag. Simple
   * vector formats will quite likely return a synthesised
   * instance here. More complicated formats may keep track of
   * things like conversion maps or parameters in such a tag.
   *
   * \returns A format tag instance for this vector.
   */
  constexpr const format tag(void) const { return format(); }
};

/**\brief Scalar multiplication
 *
 * Implements the scalar multiplication of a vector with a scalar; the
 * result in the most common vector spaces is that the vector is scaled
 * by the scalar and retains its orientation.
 *
 * \tparam F      Base type for the vector; should have field-like
 *                properties.
 * \tparam n      Number of vector elements.
 * \tparam format Coordinate format tag, defaults to format::cartesian.
 *
 * \param[in] a The vector to scale.
 * \param[in] s The scale to apply to the vector.
 *
 * \returns The modified, scaled vector.
 */
template <typename F, unsigned int n, typename format>
vector<F, n, format> operator*(vector<F, n, format> a, const F &s) {
  for (unsigned int i = 0; i < n; i++) {
    a[i] *= s;
  }
  return a;
}

/**\brief Dot product
 *
 * Implements the generic dot product, which is the sum of all the
 * products of corresponding vector elements of both vectors.
 *
 * \tparam F      Base type for the vector; should have field-like
 *                properties.
 * \tparam n      Number of vector elements.
 * \tparam format Coordinate format tag, defaults to format::cartesian.
 *
 * \param[in] a First input vector.
 * \param[in] b Second input vector.
 *
 * \returns The dot product of a and b.
 */
template <typename F, unsigned int n, typename format>
F operator*(const vector<F, n, format> &a, const vector<F, n, format> &b) {
  F s = F(0);
  for (unsigned int i = 0; i < n; i++) {
    s += a[i] * b[i];
  }
  return s;
}

/**\brief Scalar multiplication with reciprocal
 *
 * Analogous to the scalar multiplication, this is equivalent to
 * multiplying a vector with the reciprocal of the given vector, except
 * that you don't have to calculate this reciprocal separately. It
 * seemed like a logical extension.
 *
 * \tparam F      Base type for the vector; should have field-like
 *                properties.
 * \tparam n      Number of vector elements.
 * \tparam format Coordinate format tag, defaults to format::cartesian.
 *
 * \param[in] a The vector to scale.
 * \param[in] s The reciprocal of this is multiplied with the vector.
 *
 * \returns The modified, scaled vector.
 */
template <typename F, unsigned int n, typename format>
vector<F, n, format> operator/(vector<F, n, format> a, const F &s) {
  for (unsigned int i = 0; i < n; i++) {
    a[i] /= s;
  }
  return a;
}

/**\brief Dot product with reciprocals of second vector
 *
 * This is an extension of the dot product where the right hand side
 * elements serve as the divisor instead of as a factor.
 *
 * \tparam F      Base type for the vector; should have field-like
 *                properties.
 * \tparam n      Number of vector elements.
 * \tparam format Coordinate format tag, defaults to format::cartesian.
 *
 * \param[in] a First input vector.
 * \param[in] b Second input vector.
 *
 * \returns The "dot quotient" of a and b.
 */
template <typename F, unsigned int n, typename format>
F operator/(const vector<F, n, format> &a, const vector<F, n, format> &b) {
  F s = F(0);
  for (unsigned int i = 0; i < n; i++) {
    s += a[i] / b[i];
  }
  return s;
}

/**\brief Vector addition
 *
 * Implements the addition of two vectors. The two vectors are added
 * component-wise.
 *
 * \tparam F      Base type for the vector; should have field-like
 *                properties.
 * \tparam n      Number of vector elements.
 * \tparam format Coordinate format tag, defaults to format::cartesian.
 *
 * \param[in] a First input vector.
 * \param[in] b Second input vector.
 *
 * \returns The sum of the two provided vectors.
 */
template <typename F, unsigned int n, typename format>
vector<F, n, format> operator+(vector<F, n, format> a,
                               const vector<F, n, format> &b) {
  for (unsigned int i = 0; i < n; i++) {
    a[i] += b[i];
  }
  return a;
}

/**\brief Vector addition
 *
 * Performs the same operation as the regular vector addition, but
 * modifies the first vector in the process.
 *
 * \tparam F      Base type for the vector; should have field-like
 *                properties.
 * \tparam n      Number of vector elements.
 * \tparam format Coordinate format tag, defaults to format::cartesian.
 *
 * \param[in] a First input vector.
 * \param[in] b Second input vector.
 *
 * \returns The first argument, after adding the second to it.
 */
template <typename F, unsigned int n, typename format>
vector<F, n, format> &operator+=(vector<F, n, format> &a,
                                 const vector<F, n, format> &b) {
  for (unsigned int i = 0; i < n; i++) {
    a[i] += b[i];
  }
  return a;
}

/**\brief Vector subtraction
 *
 * Implements the subtraction of two vectors. The values of the second
 * vector are subtracted from the values of the first vector, component
 * by component.
 *
 * \tparam F      Base type for the vector; should have field-like
 *                properties.
 * \tparam n      Number of vector elements.
 * \tparam format Coordinate format tag, defaults to format::cartesian.
 *
 * \param[in] a First input vector.
 * \param[in] b Second input vector.
 *
 * \returns The difference of the two provided vectors.
 */
template <typename F, unsigned int n, typename format>
vector<F, n, format> operator-(vector<F, n, format> a,
                               const vector<F, n, format> &b) {
  for (unsigned int i = 0; i < n; i++) {
    a[i] -= b[i];
  }
  return a;
}

/**\brief Vector subtraction
 *
 * Performs the same operation as the regular vector subtraction, but
 * modifies the first vector in the process.
 *
 * \tparam F      Base type for the vector; should have field-like
 *                properties.
 * \tparam n      Number of vector elements.
 * \tparam format Coordinate format tag, defaults to format::cartesian.
 *
 * \param[in] a First input vector.
 * \param[in] b Second input vector.
 *
 * \returns The first argument, after subtracting the second from it.
 */
template <typename F, unsigned int n, typename format>
vector<F, n, format> &operator-=(vector<F, n, format> &a,
                                 const vector<F, n, format> &b) {
  for (unsigned int i = 0; i < n; i++) {
    a[i] -= b[i];
  }
  return a;
}

/**\brief Vector stream output
 *
 * Writes the contents of a vector to a std::ostream. The function will
 * write both the tag and the actual array contents to the stream.
 *
 * \tparam C      The character type of the stream.
 * \tparam F      Base type for the vector; should have field-like
 *                properties.
 * \tparam n      Number of vector elements.
 * \tparam format Coordinate format tag, defaults to format::cartesian.
 *
 * \param[out] stream The stream to write to.
 * \param[in]  v      The vector to write to the stream.
 *
 * \returns The provided stream after writing to it.
 */
template <typename C, typename F, unsigned int n, typename format>
std::basic_ostream<C> &operator<<(std::basic_ostream<C> &stream,
                                  const vector<F, n, format> &v) {
  stream << "(" << v.tag();
  for (unsigned int i = 0; i < n; i++) {
    stream << (i > 0 ? ", " : " ") << v[i];
  }
  return stream << ")";
}
};
};

#endif
