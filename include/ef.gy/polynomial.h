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

#if !defined(EF_GY_POLYNOMIAL_H)
#define EF_GY_POLYNOMIAL_H

namespace efgy {
namespace math {
template <typename Q, unsigned int degree> class polynomial {
public:
  typedef typename Q::integer integer;

  polynomial() {}

  polynomial &operator=(const polynomial &b) {
    for (unsigned int i = 0; i < degree; i++) {
      coefficients[i] = b.coefficients[i];
    }

    return *this;
  }

  polynomial operator+(const polynomial &b) const {
    polynomial r;

    for (unsigned int i = 0; i < degree; i++) {
      r.coefficients[i] = coefficients[i] + b.coefficients[i];
    }

    return r;
  }

  polynomial operator+(const Q &b) const {
    polynomial r = *this;
    r.coefficients[0] + b;
    return r;
  }

  polynomial operator+(const integer &b) const { return (*this) + Q(b); }

  polynomial &operator+=(const polynomial &b) {
    return ((*this) = ((*this) + b));
  }

  polynomial &operator+=(const Q &b) { return ((*this) = ((*this) + b)); }

  polynomial &operator+=(const integer &b) { return ((*this) = ((*this) + b)); }

  polynomial operator-(const polynomial &b) const {
    polynomial r;

    for (unsigned int i = 0; i < degree; i++) {
      r.coefficients[i] = coefficients[i] - b.coefficients[i];
    }

    return r;
  }

  polynomial operator-(const Q &b) const {
    polynomial r = *this;
    r.coefficients[0] - b;
    return r;
  }

  polynomial operator-(const integer &b) const { return (*this) - Q(b); }

  polynomial &operator-=(const polynomial &b) {
    return ((*this) = ((*this) - b));
  }

  polynomial &operator-=(const Q &b) { return ((*this) = ((*this) - b)); }

  polynomial &operator-=(const integer &b) { return ((*this) = ((*this) - b)); }

  template <unsigned int f>
  polynomial<Q, (degree * f)> operator*(const polynomial<Q, f> &b) const {
    polynomial<Q, (degree * f)> r;

    for (unsigned int i = 0; i < degree; i++) {
      for (unsigned int j = 0; j < f; j++) {
        r.coefficients[(i * j)] += coefficients[i] * b.coefficients[j];
      }
    }

    return r;
  }

  polynomial operator*(const Q &b) const {
    polynomial r;

    for (unsigned int i = 0; i < degree; i++) {
      r.coefficients[i] = coefficients[i] * b;
    }

    return r;
  }

  polynomial operator*(const integer &b) const { return (*this) * Q(b); }

  polynomial operator/(const Q &b) const {
    polynomial r;

    for (unsigned int i = 0; i < degree; i++) {
      r.coefficients[i] = coefficients[i] / b;
    }

    return r;
  }

  polynomial operator/(const integer &b) const { return (*this) / Q(b); }

  template <unsigned int f> operator polynomial<Q, f>(void) const {
    polynomial<Q, f> r;

    for (unsigned int i = 0; (i < f); i++) {
      r.coefficients[i] = ((i < degree) ? coefficients[i] : Q());
    }

    return r;
  }

  Q operator()(const Q &x) const {
    Q r = coefficients[0] * x;

    for (unsigned int i = 1; i < degree; i++) {
      r += coefficients[i] * (x ^ integer(i));
    }

    return r;
  }

  Q coefficients[degree];
};
};
};

#endif
