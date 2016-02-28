/**\file
 * \brief Complex numbers
 *
 * Contains templates dealing with complex numbers.
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

#if !defined(EF_GY_COMPLEX_H)
#define EF_GY_COMPLEX_H

#include <ef.gy/fractions.h>

namespace efgy {
namespace math {
template <typename Q> class complex {
public:
  typedef typename numeric::traits<Q>::integral integral;
  typedef Q rational;

  constexpr complex() : one(Q(0)), i(Q(0)) {}

  constexpr complex(const Q &pOne) : one(pOne), i(Q(0)) {}

  constexpr complex(const Q &pOne, const Q &pI) : one(pOne), i(pI) {}

  complex &operator=(const complex &b) {
    one = b.one;
    i = b.i;

    return *this;
  }

  complex &operator=(const Q &b) {
    one = b;
    i = integral(0);

    return *this;
  }

  constexpr complex operator+(const complex &b) const {
    return complex(one + b.one, i + b.i);
  }

  complex &operator+=(const complex &b) {
    complex r = (*this) + b;

    return ((*this) = r);
  }

  constexpr complex operator-(const complex &b) const {
    return complex(one - b.one, i - b.i);
  }

  complex &operator-=(const complex &b) {
    complex r = (*this) - b;

    return ((*this) = r);
  }

  constexpr complex operator*(const complex &b) const {
    // (a + bi) * (c + di) = (ac - bd) + (bc + ad)*i
    return complex(one * b.one - i * b.i, i * b.one + one * b.i);
  }

  complex &operator*=(const complex &b) {
    complex r = (*this) * b;

    return ((*this) = r);
  }

  complex operator^(const integral &b) const {
    if (b == integral(0)) {
      return complex(integral(1));
    } else {
      complex rv = *this;

      for (integral i = integral(1); i < b; i++) {
        rv *= (*this);
      }

      return rv;
    }
  }

  complex &operator^=(const integral &b) {
    complex r = (*this) ^ b;

    return ((*this) = r);
  }

  constexpr complex operator/(const complex &b) const {
    // (a + bi) / (c + di) = (ac + bd)/(c*c + d*d) + (bc - ad)/(c*c + d*d)*i
    return complex((one * b.one + i * b.i) / (b.one * b.one + b.i * b.i),
                   (i * b.one - one * b.i) / (b.one * b.one + b.i * b.i));
  }

  complex &operator/=(const complex &b) {
    complex r = (*this) / b;

    return ((*this) = r);
  }

  Q one;
  Q i;
};

namespace numeric {
template <typename Q> class traits<complex<Q>> {
public:
  typedef typename traits<Q>::integral integral;
  typedef typename traits<Q>::rational rational;
  typedef complex<Q> self;
  typedef complex<Q> derivable;

  static const bool stable = false;
};
};
};
};

#endif
