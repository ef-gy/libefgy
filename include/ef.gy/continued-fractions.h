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

#if !defined(EF_GY_CONTINUED_FRACTIONS_H)
#define EF_GY_CONTINUED_FRACTIONS_H

#include <ef.gy/fractions.h>
#include <vector>
#include <ostream>

namespace efgy {
namespace math {
namespace numeric {
template <typename N> class continuedFractional : public numeric {
public:
  typedef N integer;

  continuedFractional() : coefficient(), negative(false) {}
  continuedFractional(const N &t) : coefficient(), negative(false) {
    coefficient.push_back(t);
  }
  continuedFractional(const fractional<N> &pF)
      : coefficient(), negative(false) {
    N i = N(0);
    for (fractional<N> f = pF;
         (f.numerator != zero()) && (f.denominator != zero());
         f = reciprocal(f - i)) {
      i = N(f);

      if (i < zero()) {
        coefficient.push_back(-i);
        negative = true;
      } else {
        coefficient.push_back(i);
      }

      if (f < zero()) {
        f *= N(-1);
        i = -i;
      }
    }

    if ((coefficient.size() > one()) && (coefficient.back() == N(1))) {
      coefficient.push_back(N(1));
    }
  }
  continuedFractional(const continuedFractional &cf)
      : coefficient(cf.coefficient), negative(cf.negative) {}

  continuedFractional &operator=(const continuedFractional &b) {
    coefficient = b.coefficient;
    negative = b.negative;
    return *this;
  }
  continuedFractional &operator=(const N &b) {
    coefficient.clear();
    if (b < zero()) {
      coefficient.push_back(-b);
      negative = true;
    } else {
      coefficient.push_back(b);
      negative = false;
    }
    return *this;
  }
  continuedFractional &operator=(const fractional<N> &b) {
    return (*this = continuedFractional(b));
  }

  continuedFractional operator+(const continuedFractional &b) const {
    binaryOperator op = binaryOperator::addition();
    return op(*this, b);
  }
  continuedFractional &operator+=(const continuedFractional &b) {
    (*this) = (*this + b);
  }

  continuedFractional operator-(const continuedFractional &b) const {
    binaryOperator op = binaryOperator::subtraction();
    return op(*this, b);
  }
  continuedFractional &operator-=(const continuedFractional &b) {
    (*this) = (*this - b);
  }

  continuedFractional operator*(const continuedFractional &b) const {
    binaryOperator op = binaryOperator::multiplication();
    return op(*this, b);
  }
  continuedFractional &operator*=(const continuedFractional &b) {
    (*this) = (*this * b);
  }

  // missing: %, ^

  continuedFractional operator/(const continuedFractional &b) const {
    binaryOperator op = binaryOperator::division();
    return op(*this, b);
  }
  continuedFractional &operator/=(const continuedFractional &b) {
    (*this) = (*this / b);
  }

  // missing: >, >=, <, <=
  //
  bool operator>(const continuedFractional &b) const;
  bool operator>(const zero &b) const {
    return ((coefficient.size() > N(1)) ||
            ((coefficient.size() == N(1)) && (coefficient[N(0)] > zero()))) &&
           !negative;
  }
  bool operator>(const one &b) const {
    return (((coefficient.size() > N(1)) && (coefficient[N(0)] >= N(1))) ||
            ((coefficient.size() >= N(1)) && (coefficient[N(0)] > N(1)))) &&
           !negative;
  }
  bool operator>(const negativeOne &b) const {
    return !negative || (coefficient.size() == zero()) ||
           (coefficient[N(0)] == zero());
  }

  bool operator==(const continuedFractional &b) const;
  bool operator==(const zero &b) const {
    return (coefficient.size() == zero()) || (coefficient[N(0)] == zero());
  }
  bool operator==(const one &b) const {
    return !negative && (coefficient.size() == N(1)) &&
           (coefficient[N(0)] == N(1));
  }
  bool operator==(const negativeOne &b) const {
    return negative && (coefficient.size() == N(1)) &&
           (coefficient[N(0)] == N(1));
  }

  continuedFractional operator, (const N &pB) const {
    continuedFractional rv = *this;
    N b = pB;
    if (b < zero()) {
      rv.negative = true;
      rv.coefficient.push_back(-b);
    } else {
      rv.coefficient.push_back(b);
    }
    return rv;
  }

  // missing: !

  operator fractional<N>(void) const {
    fractional<N> rv;
    N j = coefficient.size() - N(1);
    for (N i = j; i >= N(0); i--) {
      if (i == j) {
        rv = fractional<N>(N(coefficient[i]));
      } else {
        rv = reciprocal(rv) + coefficient[i];
      }
    }
    if (negative) {
      rv *= N(-1);
    }
    return rv;
  }

  std::vector<N> coefficient;
  bool negative;

protected:
  class binaryOperator {
  public:
    binaryOperator(void)
        : a(N(0)), b(N(0)), c(N(0)), d(N(0)), e(N(0)), f(N(0)), g(N(0)),
          h(N(0)) {}

    binaryOperator(N pA, N pB, N pC, N pD, N pE, N pF, N pG, N pH)
        : a(pA), b(pB), c(pC), d(pD), e(pE), f(pF), g(pG), h(pH) {}

    continuedFractional operator()(const continuedFractional &x,
                                   const continuedFractional &y) const {
      binaryOperator op = *this;
      continuedFractional rv;
      N px = N(0), py = N(0);
      bool xInf = false, yInf = false;
      while (1) {
        fractional<N> ae = fractional<N>(op.a, op.e),
                      bf = fractional<N>(op.b, op.f),
                      cg = fractional<N>(op.c, op.g),
                      dh = fractional<N>(op.d, op.h);

        N rae, rbf, rcg, rdh;

        if (op.e != zero()) {
          rae = N(ae);
        }
        if (op.f != zero()) {
          rbf = N(bf);
        }
        if (op.g != zero()) {
          rcg = N(cg);
        }
        if (op.h != zero()) {
          rdh = N(dh);
        }

        if ((op.e != zero()) && (op.f != zero()) && (op.g != zero()) &&
            (op.h != zero())) {
          if ((rae == rbf) && (rbf == rcg) && (rcg == rdh)) {
            rv = (rv, rae);
            if ((op.a < zero()) && (op.b < zero()) && (op.c < zero()) &&
                (op.d < zero())) {
              rv.negative = true;
            }
            op = op.output(rae);
            continue;
          }
        }

        if ((op.e == zero()) && (op.f == zero()) && (op.g == zero()) &&
            (op.h == zero())) {
          break;
        }

        if (xInf && yInf) {
          if (op.h != zero()) {
            continuedFractional<N> cfdh = dh;
            for (N i = N(0); i < cfdh.coefficient.size(); i++) {
              rv = (rv, cfdh.coefficient[i]);
            }
          }
          break;
        }

        fractional<N> bfae = bf - ae;
        fractional<N> cgae = cg - ae;

        if (bfae < zero()) {
          bfae *= N(-1);
        }
        if (cgae < zero()) {
          cgae *= N(-1);
        }

        if ((op.f == zero()) || (op.h == zero())) {
          goto processY;
        }
        if ((op.e == zero()) || (op.g == zero())) {
          goto processX;
        }

        if (bfae > cgae) {
        processX:
          if (px < x.coefficient.size()) {
            op = op.insertX(x.coefficient[px]);
            px++;
          } else if (!xInf) {
            op = op.insertXinf();
            xInf = true;
          } else {
            goto processY;
          }
        } else {
        processY:
          if (py < y.coefficient.size()) {
            op = op.insertY(y.coefficient[py]);
            py++;
          } else if (!yInf) {
            op = op.insertYinf();
            yInf = true;
          } else {
            goto processX;
          }
        }
      }
      return rv;
    }

    binaryOperator insertX(N P) const {
      return binaryOperator(b, a + b * P, d, c + d * P, f, e + f * P, h,
                            g + h * P);
    }

    binaryOperator insertXinf() const {
      return binaryOperator(a, b, a, b, e, f, e, f);
    }

    binaryOperator insertY(N Q) const {
      return binaryOperator(c, d, a + c * Q, b + d * Q, g, h, e + g * Q,
                            f + h * Q);
    }

    binaryOperator insertYinf() const {
      return binaryOperator(c, d, c, d, g, h, g, h);
    }

    binaryOperator output(N R) const {
      return binaryOperator(e, f, g, h, a - e * R, b - f * R, c - g * R,
                            d - h * R);
    }

    static binaryOperator addition(void) {
      return binaryOperator(0, 1, 1, 0, 1, 0, 0, 0);
    }

    static binaryOperator subtraction(void) {
      return binaryOperator(0, 1, -1, 0, 1, 0, 0, 0);
    }

    static binaryOperator multiplication(void) {
      return binaryOperator(0, 0, 0, 1, 1, 0, 0, 0);
    }

    static binaryOperator division(void) {
      return binaryOperator(0, 1, 0, 0, 0, 0, 1, 0);
    }

  protected:
    N a, b, c, d, e, f, g, h;
  };
};

template <typename N>
fractional<N> round(const fractional<N> &pQ,
                    const unsigned long pPrecision = 24) {
  unsigned long precision = pPrecision == 0 ? 1 : pPrecision;
  const unsigned long long maxNumerator = (1 << precision) - 1;
  const unsigned long long maxDenominator = (1 << precision) - 1;
  continuedFractional<N> cf = pQ;
  fractional<N> q = cf;
  bool negative = q < zero();
  while ((negative ? (q.numerator < -N(maxNumerator))
                   : (q.numerator > N(maxNumerator))) ||
         (q.denominator > N(maxDenominator))) {
    cf.coefficient.pop_back();
    q = cf;
  }
  return q;
}

template <typename C, typename N>
std::basic_ostream<C> &operator<<(std::basic_ostream<C> &out,
                                  const continuedFractional<N> &f) {
  if (f.coefficient.size() == N(0)) {
    return out << "[ 0 ]";
  }

  if (f.negative) {
    out << "- ";
  }
  out << "[";
  for (N i = N(0); i < f.coefficient.size(); i++) {
    if (i == N(0)) {
      out << " " << f.coefficient[i];
    } else if (i == N(1)) {
      out << "; " << f.coefficient[i];
    } else {
      out << ", " << f.coefficient[i];
    }
  }

  return out << " ]";
}
};
};
};

#endif
