/**\file
 * \brief Test cases for the factorial class template
 *
 * This file contains test cases for the math::factorial template, which is
 * used to calculate the factorial of a number.
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

#include <iostream>

#include <ef.gy/factorial.h>
#include <ef.gy/test-case.h>

using namespace efgy;
using namespace efgy::math;
using namespace std;

/**\brief Factorial tests
 *
 * \test Calculates the factorial of a few numbers and compares those to the
 *       values that the template ought to return.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testFactorial(ostream &log) {
  if (factorial<int>(0) != 1) {
    log << "Factorial of 0 should be 1.";
    return 1;
  }

  if (factorial<int>(1) != 1) {
    log << "Factorial of 1 should be 1.";
    return 2;
  }

  if (factorial<int>(5) != 120) {
    log << "Factorial of 5 should be 120.";
    return 3;
  }

  return 0;
}

namespace test {
using efgy::test::function;

static function factorial(testFactorial);
}
