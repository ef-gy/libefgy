/**\file
 * \brief Matrix handling
 *
 * Defines a template to hold matrices with cells of arbitrary data types and
 * functions to calculate with matrices of different sizes. The sizes are set at
 * compile time.
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

#if !defined(EF_GY_MATRIX_H)
#define EF_GY_MATRIX_H

#include <array>
#include <iostream>
#include <iterator>

namespace efgy {
namespace math {
namespace iterator {
template <typename Q, std::size_t m, typename Source>
class matrix : public std::iterator<std::random_access_iterator_tag, Q> {
 public:
  matrix(const Source &pSource, unsigned long pPosition)
      : source(pSource), position(pPosition) {}

  constexpr bool operator==(const matrix &b) const {
    return (position == b.position) && (&source == &b.source);
  }

  constexpr bool operator!=(const matrix &b) const {
    return (position != b.position) || (&source != &b.source);
  }

  Q operator*(void)const {
    unsigned long j = position % m;
    unsigned long i = (position - j) / m;
    return source[i][j];
  }

  matrix &operator++(void) {
    ++position;
    return *this;
  }

  matrix &operator--(void) {
    --position;
    return *this;
  }

  matrix &operator+=(const std::ptrdiff_t &b) {
    position += b;
    return *this;
  }

  constexpr matrix operator+(const std::ptrdiff_t &b) const {
    return matrix(source, position + b);
  }

  matrix &operator-=(const std::ptrdiff_t &b) {
    position -= b;
    return *this;
  }

  constexpr matrix operator-(const std::ptrdiff_t &b) const {
    return matrix(source, position - b);
  }

  constexpr std::ptrdiff_t operator-(const matrix &b) const {
    return std::ptrdiff_t(position) - std::ptrdiff_t(b.position);
  }

  constexpr Q &operator[](const std::ptrdiff_t &b) const {
    return *((*this) + b);
  }

  constexpr bool operator<(const matrix &b) const {
    return position < b.position;
  }

  constexpr bool operator<=(const matrix &b) const {
    return position <= b.position;
  }

  constexpr bool operator>(const matrix &b) const {
    return position > b.position;
  }

  constexpr bool operator>=(const matrix &b) const {
    return position >= b.position;
  }

 protected:
  std::size_t position;
  const Source &source;
};
}  // namespace iterator

template <typename Q, std::size_t n, std::size_t m>
class matrix;

template <typename Q, std::size_t n, std::size_t m>
matrix<Q, n, m> operator+(const matrix<Q, n, m> &a, const matrix<Q, n, m> &b) {
  matrix<Q, n, m> r;

  for (std::size_t i = 0; i < n; i++) {
    for (std::size_t j = 0; j < m; j++) {
      r[i][j] = a[i][j] + b[i][j];
    }
  }

  return r;
}

template <typename Q, std::size_t n, std::size_t m>
matrix<Q, n, m> operator-(const matrix<Q, n, m> &a, const matrix<Q, n, m> &b) {
  matrix<Q, n, m> r;

  for (std::size_t i = 0; i < n; i++) {
    for (std::size_t j = 0; j < m; j++) {
      r[i][j] = a[i][j] - b[i][j];
    }
  }

  return r;
}

template <typename Q, std::size_t n, std::size_t m, std::size_t p>
matrix<Q, n, p> operator*(const matrix<Q, n, m> &a, const matrix<Q, m, p> &b) {
  matrix<Q, n, p> r;

  for (std::size_t i = 0; i < n; i++) {
    for (std::size_t j = 0; j < p; j++) {
      r[i][j] = a[i][0] * b[0][j];

      for (std::size_t k = 1; k < m; k++) {
        r[i][j] += a[i][k] * b[k][j];
      }
    }
  }

  return r;
}

template <typename Q, std::size_t n, std::size_t m>
matrix<Q, n, m> operator/(const matrix<Q, n, m> &a, const Q &b) {
  matrix<Q, n, m> r;

  for (std::size_t i = 0; i < n; i++) {
    for (std::size_t j = 0; j < m; j++) {
      r[i][j] = a[i][j] / b;
    }
  }

  return r;
}

namespace ghost {
template <typename Q, std::size_t n, std::size_t m,
          template <typename, std::size_t, std::size_t> class gen>
class matrix {
 public:
  using iterator = iterator::matrix<Q, m, matrix>;

  matrix(void) = default;

  gen<Q, n, m> generator;

 private:
  class row {
   public:
    row(const matrix &ma, const std::size_t &i) : mat(ma), pos(i) {}

    const Q operator[](const std::size_t &j) const {
      return mat.generator(pos, j);
    }

   private:
    const matrix &mat;
    const std::size_t &pos;
  };

 public:
  const row operator[](const std::size_t &i) const { return row(*this, i); }

  iterator begin(void) const { return iterator(*this, 0); }
  iterator end(void) const { return iterator(*this, n * m); }
  constexpr std::size_t size(void) const { return n; }
};
}  // namespace ghost

/**\brief Matrix with fixed size.
 *
 * This template is used to store and calculate with matrices of arbitrary but
 * fixed and finite sizes.
 *
 * \tparam Q The data type for individual matrix cells.
 * \tparam n Number of rows in the matrix.
 * \tparam m Number of columns in the matrix.
 */
template <typename Q, std::size_t n, std::size_t m>
class matrix : public std::array<std::array<Q, m>, n> {
 public:
  using iterator = iterator::matrix<Q, m, matrix>;

  /**\brief Default constructor
   *
   * This is the default constructor that does not explicitly initialise the
   * matrix to anything.
   */
  matrix(void) = default;

  /**\brief Copy constructor
   *
   * Copies the given matrix's elements to the new instance. The source matrix
   * may have different dimensions than the newly created matrix; if it does,
   * then any excess elements are discarded and any new elements are initialised
   * with their default constructor.
   *
   * Example: you have a 2x2 matrix and create a new 1x3 matrix from that. This
   * will lead to the fields (0,0) and (0,1) to be coped to the target matrix;
   * (0,2) will be initialised with its default constructor and (1,0) and (1,1)
   * in the source matrix will be ignored.
   *
   * \tparam rn Number of rows in the matrix to copy.
   * \tparam rm Number of columns in the matrix to copy.
   *
   * \param[in] b The matrix to copy.
   */
  template <std::size_t rn, std::size_t rm>
  matrix(const matrix<Q, rn, rm> &b) {
    for (std::size_t i = 0; i < n; i++) {
      for (std::size_t j = 0; j < m; j++) {
        if ((i < rn) && (j < rm)) {
          (*this)[i][j] = b[i][j];
        } else {
          (*this)[i][j] = Q();
        }
      }
    }
  }

  template <std::size_t rn, std::size_t rm,
            template <typename, std::size_t, std::size_t> class gen>
  matrix(const ghost::matrix<Q, rn, rm, gen> &b) {
    for (std::size_t i = 0; i < n; i++) {
      for (std::size_t j = 0; j < m; j++) {
        if ((i < rn) && (j < rm)) {
          (*this)[i][j] = b[i][j];
        } else {
          (*this)[i][j] = Q();
        }
      }
    }
  }

  template <typename it>
  matrix(const it &pBegin, const it &pEnd) {
    it k = pBegin;
    for (std::size_t i = 0; i < n; i++) {
      for (std::size_t j = 0; j < m; i++) {
        if (k != pEnd) {
          (*this)[i][j] = *k;
          k++;
        } else {
          (*this)[i][j] = Q();
        }
      }
    }
  }

  matrix<Q, n, m> &operator=(const matrix<Q, n, m> &b) {
    for (std::size_t i = 0; i < n; i++) {
      for (std::size_t j = 0; j < m; j++) {
        (*this)[i][j] = b[i][j];
      }
    }

    return *this;
  }

  iterator begin(void) const { return iterator(*this, 0); }
  iterator end(void) const { return iterator(*this, n * m); }
  constexpr std::size_t size(void) const { return n; }
};

/**\brief Write matrix contents to stream.
 *
 * Displays each matrix row in a separate line. Values within rows are separated
 * by tabs.
 */
template <typename Q, std::size_t n, std::size_t m, typename C>
std::ostream &operator<<(std::basic_ostream<C> &stream,
                         const matrix<Q, n, m> &matrix) {
  for (int i = 0; i < n; i++) {
    for (int k = 0; k < m; k++) {
      stream << matrix[i][k] << "\t";
    }
    stream << "\n";
  }
  return stream;
}

template <typename Q, std::size_t d>
Q determinant(const matrix<Q, d, d> &pM) {
  Q rv;

  for (std::size_t i = 0; i < d; i++) {
    matrix<Q, d - 1, d - 1> pS;

    for (std::size_t j = 1, r = 0; j < d; j++, r++) {
      for (std::size_t k = 0, c = 0; k < d; k++) {
        if (k == i) {
          continue;
        }

        pS[r][c] = pM[j][k];

        c++;
      }
    }

    if (i == 0) {
      rv = pM[0][i] * determinant(pS);
    } else if ((i % 2) == 0) {
      rv += pM[0][i] * determinant(pS);
    } else {
      rv -= pM[0][i] * determinant(pS);
    }
  }

  return rv;
}

template <typename Q>
Q determinant(const matrix<Q, 2, 2> &pM) {
  return pM[0][0] * pM[1][1] - pM[1][0] * pM[0][1];
}

template <typename Q>
Q determinant(const matrix<Q, 1, 1> &pM) {
  return pM[0][0];
}

template <typename Q, std::size_t d>
bool isIdentity(const matrix<Q, d, d> &pM) {
  for (std::size_t i = 0; i < d; i++) {
    for (std::size_t j = 0; j < d; j++) {
      if (pM[i][j] != (i == j ? Q(1) : Q(0))) {
        return false;
      }
    }
  }

  return true;
}

template <typename Q, std::size_t n, std::size_t m>
matrix<Q, m, n> transpose(const matrix<Q, n, m> &pM) {
  matrix<Q, m, n> rv;

  for (std::size_t i = 0; i < n; i++) {
    for (std::size_t j = 0; j < m; j++) {
      rv[j][i] = pM[i][j];
    }
  }

  return rv;
}

template <typename Q>
matrix<Q, 3, 3> invert(const matrix<Q, 3, 3> &pM) {
  matrix<Q, 3, 3> rv;

  const Q &a = pM[0][0];
  const Q &b = pM[0][1];
  const Q &c = pM[0][2];
  const Q &d = pM[1][0];
  const Q &e = pM[1][1];
  const Q &f = pM[1][2];
  const Q &g = pM[2][0];
  const Q &h = pM[2][1];
  const Q &i = pM[2][2];

  rv[0][0] = e * i - f * h;
  rv[0][1] = -(d * i - f * g);
  rv[0][2] = d * h - e * g;
  rv[1][0] = -(b * i - c * h);
  rv[1][1] = a * i - c * g;
  rv[1][2] = -(a * h - b * g);
  rv[2][0] = b * f - c * e;
  rv[2][1] = -(a * f - c * d);
  rv[2][2] = a * e - b * d;

  return rv / determinant(pM);
}
}  // namespace math
}  // namespace efgy

#endif
