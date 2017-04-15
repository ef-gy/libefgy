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

#if !defined(EF_GY_TRAITS_H)
#define EF_GY_TRAITS_H

namespace efgy {
namespace math {
template <typename Q, typename I>
class primitive;

namespace numeric {
template <typename T>
class traits {
 public:
  typedef T integral;
  typedef T rational;
  typedef T self;
  typedef primitive<T, unsigned long> derivable;

  static const bool stable = false;
};

template <>
class traits<float> {
 public:
  typedef long integral;
  typedef float rational;
  typedef float self;
  typedef primitive<float, unsigned long> derivable;

  static const bool stable = false;
};

template <>
class traits<double> {
 public:
  typedef long integral;
  typedef double rational;
  typedef double self;
  typedef primitive<double, unsigned long> derivable;

  static const bool stable = false;
};

template <>
class traits<long double> {
 public:
  typedef long long integral;
  typedef long double rational;
  typedef long double self;
  typedef primitive<long double, unsigned long> derivable;

  static const bool stable = false;
};

template <typename Q, typename I>
class traits<primitive<Q, I>> {
 public:
  typedef I integral;
  typedef Q rational;
  typedef primitive<Q, I> self;
  typedef primitive<Q, I> derivable;

  static const bool stable = false;
};
};
};
};

#endif
