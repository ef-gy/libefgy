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

#if !defined(EF_GY_FACTORIAL_H)
#define EF_GY_FACTORIAL_H

namespace efgy {
namespace math {
/**\brief A template to calculate the factorial of a number.
 *
 * Uses a basic iterative algorithm.
 *
 * \tparam Z Basic arithmetic type used for calculations.
 * Z is required to allow integral values and multiplication,
 * but there are no further restrictions on Z imposed by the code,
 * although the factorial function is only defined on the positive
 * integers, so for larger types this template implements a partial
 * function.
 *
 * The current behaviour for negative numbers is to return 1.
 */
template <typename Z>
class factorial {
 public:
  factorial() : integer(Z(0)) {}

  factorial(const Z &pInteger) : integer(pInteger) {}

  operator Z() const {
    Z rv = Z(1);

    for (Z n = Z(2); n <= integer; n++) {
      rv = rv * n;
    }

    return rv;
  }

  Z integer;
};
};
};

#endif
