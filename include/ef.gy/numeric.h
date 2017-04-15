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

#if !defined(EF_GY_NUMERIC_H)
#define EF_GY_NUMERIC_H

namespace efgy {
namespace math {
namespace numeric {
template <typename T>
class factorial;

class numeric;
class zero;
class one;
class negativeOne;

class numeric {
 public:
};

class zero : public numeric {
 public:
  template <typename T>
  operator T(void) const {
    return T(0);
  }

  bool operator==(const zero &b) const { return true; }
  bool operator==(const one &b) const { return false; }
  bool operator==(const negativeOne &b) const { return false; }

  bool operator<(const zero &b) const { return false; }
  bool operator<(const one &b) const { return true; }
  bool operator<(const negativeOne &b) const { return false; }

  bool operator>(const zero &b) const { return false; }
  bool operator>(const one &b) const { return false; }
  bool operator>(const negativeOne &b) const { return true; }

  bool operator<=(const zero &b) const { return true; }
  bool operator<=(const one &b) const { return true; }
  bool operator<=(const negativeOne &b) const { return false; }

  bool operator>=(const zero &b) const { return true; }
  bool operator>=(const one &b) const { return false; }
  bool operator>=(const negativeOne &b) const { return true; }
};

class one : public numeric {
 public:
  template <typename T>
  operator T(void) const {
    return T(1);
  }

  bool operator==(const zero &b) const { return false; }
  bool operator==(const one &b) const { return true; }
  bool operator==(const negativeOne &b) const { return false; }

  bool operator<(const zero &b) const { return false; }
  bool operator<(const one &b) const { return false; }
  bool operator<(const negativeOne &b) const { return false; }

  bool operator>(const zero &b) const { return true; }
  bool operator>(const one &b) const { return false; }
  bool operator>(const negativeOne &b) const { return true; }

  bool operator<=(const zero &b) const { return true; }
  bool operator<=(const one &b) const { return true; }
  bool operator<=(const negativeOne &b) const { return false; }

  bool operator>=(const zero &b) const { return true; }
  bool operator>=(const one &b) const { return true; }
  bool operator>=(const negativeOne &b) const { return true; }
};

class negativeOne : public numeric {
 public:
  template <typename T>
  operator T(void) const {
    return T(-1);
  }

  bool operator==(const zero &b) const { return false; }
  bool operator==(const one &b) const { return false; }
  bool operator==(const negativeOne &b) const { return true; }

  bool operator<(const zero &b) const { return true; }
  bool operator<(const one &b) const { return true; }
  bool operator<(const negativeOne &b) const { return false; }

  bool operator>(const zero &b) const { return false; }
  bool operator>(const one &b) const { return false; }
  bool operator>(const negativeOne &b) const { return false; }

  bool operator<=(const zero &b) const { return true; }
  bool operator<=(const one &b) const { return true; }
  bool operator<=(const negativeOne &b) const { return true; }

  bool operator>=(const zero &b) const { return false; }
  bool operator>=(const one &b) const { return false; }
  bool operator>=(const negativeOne &b) const { return true; }
};

/* generic comparison operators against one */

template <typename T>
bool operator==(const T &a, const zero &b) {
  return (a == T(0));
}

template <typename T>
bool operator==(const zero &a, const T &b) {
  return (b == T(0));
}

template <typename T>
bool operator>(const T &a, const zero &b) {
  return (a > T(0));
}

template <typename T>
bool operator>(const zero &a, const T &b) {
  return (T(0) > b);
}

/* generic comparison operators against one */

template <typename T>
bool operator==(const T &a, const one &b) {
  return (a == T(1));
}

template <typename T>
bool operator==(const one &a, const T &b) {
  return (b == T(1));
}

template <typename T>
bool operator>(const T &a, const one &b) {
  return (a > T(1));
}

template <typename T>
bool operator>(const one &a, const T &b) {
  return (T(1) > b);
}

/* generic comparison operators against negativeOne */

template <typename T>
bool operator==(const T &a, const negativeOne &b) {
  return (a == T(-1));
}

template <typename T>
bool operator==(const negativeOne &a, const T &b) {
  return (b == T(-1));
}

template <typename T>
bool operator>(const T &a, const negativeOne &b) {
  return (a > T(-1));
}

template <typename T>
bool operator>(const negativeOne &a, const T &b) {
  return (T(-1) > b);
}

/* generic comparison operators */

template <typename T, typename U>
bool operator!=(const T &a, const U &b) {
  return !(a == b);
}

template <typename T, typename U>
bool operator>=(const T &a, const U &b) {
  return (a == b) || (a > b);
}

template <typename T, typename U>
bool operator<(const T &a, const U &b) {
  return b > a;
}

template <typename T, typename U>
bool operator<=(const T &a, const U &b) {
  return b >= a;
}

/* miscellaneous operators */

template <typename T>
T operator-(const T &a) {
  return a * T(-1);
}

template <typename T>
T operator*(const T &a, const negativeOne &b) {
  return -a;
}

template <typename T>
T operator*(const negativeOne &a, const T &b) {
  return -b;
}

template <typename T>
T operator*(const T &a, const one &b) {
  return a;
}

template <typename T>
T operator*(const one &a, const T &b) {
  return b;
}

template <typename T>
zero operator*(const T &a, const zero &b) {
  return zero();
}

template <typename T>
zero operator*(const zero &a, const T &b) {
  return zero();
}

/**
 * Generic power2 template.
 */
template <typename T>
T pow2(const T &a) {
  return a * a;
}

/* generic factorial operator */

template <typename T>
factorial<T> operator!(const T &a) {
  return factorial<T>(a);
}

/* generic exponentiation operator */

template <typename T, typename Z>
T operator^(const T &a, const Z &b) {
  if (b == zero()) {
    return T(1);
  } else {
    T rv = a;

    for (Z i = 1; i < b; i++) {
      rv *= a;
    }

    return rv;
  }
}

template <typename T, typename Z>
T &operator^=(T &a, const Z &b) {
  T r = a ^ b;

  return (a = r);
}

/* generic functions */

template <typename T>
T gcd(const T &rA, const T &rB) {
  T t;
  T a = (rA < zero()) ? -rA : rA;
  T b = (rB < zero()) ? -rB : rB;

  while (b > zero()) {
    t = b;
    b = a % b;
    a = t;
  }

  return a;
}

template <typename T>
T gcdP(const T &rA, const T &rB) {
  T t;
  T a = rA;
  T b = rB;

  while (b > zero()) {
    t = b;
    b = a % b;
    a = t;
  }

  return a;
}
};
};
};

#endif
