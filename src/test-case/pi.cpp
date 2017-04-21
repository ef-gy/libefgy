/**\file
 * \brief Test cases for the 'pi' template
 *
 * Test cases for the 'pi' template to make sure instances of that class compile
 * properly and generate reasonable approximations of 'pi'.
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
#include <ef.gy/pi.h>
#include <ef.gy/primitive.h>
#include <ef.gy/test-case.h>

using namespace efgy::math;
using std::string;

typedef primitive<long double, unsigned long long> longDouble;

/**\brief 'Pi' initialisation and type cast tests
 * \test Initialises several instances of the 'pi' template with different base
 *       types and precisions. The instances are then cast to different types
 *       and written to the log to verify that the template compiles cleanly.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testPi(std::ostream &log) {
  pi<longDouble> piD1(1, 1);
  pi<longDouble> piD2(1, 2);
  pi<longDouble> piD3(1, 3);
  pi<longDouble> piD4(1, 4);

  log << "pi<longDouble,1> = " << (long double)longDouble(piD1) << "\n";
  log << "pi<longDouble,2> = " << (long double)longDouble(piD2) << "\n";
  log << "pi<longDouble,3> = " << (long double)longDouble(piD3) << "\n";
  log << "pi<longDouble,4> = " << (long double)longDouble(piD4) << "\n";

  if (longDouble(piD1) != pi<longDouble>::get(1)) {
    log << "constexpr method returned unexpected result\n";
    return 1;
  }

  if (longDouble(piD2) != pi<longDouble>::get(2)) {
    log << "constexpr method returned unexpected result\n";
    return 2;
  }

  if (longDouble(piD3) != pi<longDouble>::get(3)) {
    log << "constexpr method returned unexpected result\n";
    return 3;
  }

  if (longDouble(piD4) != pi<longDouble>::get(4)) {
    log << "constexpr method returned unexpected result\n";
    return 4;
  }

  pi<long double> piDL1(1, 1);
  pi<long double> piDL2(1, 2);
  pi<long double> piDL3(1, 3);
  pi<long double> piDL4(1, 4);

  if ((long double)(piDL1) != pi<long double>::get(1)) {
    log << "constexpr method returned unexpected result\n";
    return 11;
  }

  if ((long double)(piDL2) != pi<long double>::get(2)) {
    log << "constexpr method returned unexpected result\n";
    return 12;
  }

  if ((long double)(piDL3) != pi<long double>::get(3)) {
    log << "constexpr method returned unexpected result\n";
    return 13;
  }

  if ((long double)(piDL4) != pi<long double>::get(4)) {
    log << "constexpr method returned unexpected result\n";
    return 14;
  }

  log << "pi<long double,1> = " << (long double)piDL1 << "\n";
  log << "pi<long double,2> = " << (long double)piDL2 << "\n";
  log << "pi<long double,3> = " << (long double)piDL3 << "\n";
  log << "pi<long double,4> = " << (long double)piDL4 << "\n";

  pi<fraction> piQ1(fraction(1), 1);
  pi<fraction> piQ2(fraction(1), 2);
  pi<fraction> piQ3(fraction(1), 3);
  pi<fraction> piQ4(fraction(1), 4);

  log << "pi<fraction,1> = " << fraction(piQ1) << "\n";
  log << "pi<fraction,2> = " << fraction(piQ2) << "\n";
  log << "pi<fraction,3> = " << fraction(piQ3) << "\n";
  log << "pi<fraction,4> = " << fraction(piQ4) << "\n";

  if (fraction(piQ1) != pi<fraction>::get(1)) {
    log << "constexpr method returned unexpected result\n";
    return 21;
  }

  if (fraction(piQ2) != pi<fraction>::get(2)) {
    log << "constexpr method returned unexpected result\n";
    return 22;
  }

  if (fraction(piQ3) != pi<fraction>::get(3)) {
    log << "constexpr method returned unexpected result\n";
    return 23;
  }

  if (fraction(piQ4) != pi<fraction>::get(4)) {
    log << "constexpr method returned unexpected result\n";
    return 24;
  }

  return 0;
}

namespace test {
using efgy::test::function;

static function pi(testPi);
}
