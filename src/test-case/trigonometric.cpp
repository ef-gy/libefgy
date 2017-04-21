/**\file
 * \brief Test cases for trigonometric
 *
 * Contains test cases that test libefgy's trigonometric.h and the functions
 * therein.
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

#include <ef.gy/fractions.h>
#include <ef.gy/range.h>
#include <ef.gy/test-case.h>
#include <ef.gy/trigonometric.h>

using namespace efgy::math;
using efgy::range;

/**\brief Tests sines
 * \test Calculates sines using libefgy's sine() function and compares the
 *       results with reference data.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testSine(std::ostream &log) {
  double s, c;

  for (double x : range<double>(0, 3.14, 30, true)) {
    s = sines(x, c);

    log << "sine(" << x << ") = " << s << ":" << c << "\n";
  }

  return 0;
}

namespace test {
using efgy::test::function;

static function sine(testSine);
}
