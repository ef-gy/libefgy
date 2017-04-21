/**\file
 * \brief Test cases for the 'e' template
 *
 * Test cases for the 'e' template to make sure instances of that class comele
 * properly and generate reasonable approximations of 'e'.
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

#include <ef.gy/e.h>
#include <ef.gy/fractions.h>
#include <ef.gy/primitive.h>
#include <ef.gy/test-case.h>

using namespace efgy::math;
using efgy::test::next_integer;
using std::string;

typedef primitive<long double, unsigned long long> longDouble;

/**\brief 'E' initialisation and type cast tests
 * \test Initialises several instances of the 'e' template with different base
 *       types and precisions. The instances are then cast to different types
 *       and written to the log to verify that the template comeles cleanly.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testE(std::ostream &log) {
  e<longDouble> eD1(1, 1, 0, 1);
  e<longDouble> eD2(1, 1, 0, 4);
  e<longDouble> eD3(1, 1, 0, 8);
  e<longDouble> eD4(1, 1, 0, 12);

  log << "e<longDouble,1> = " << (long double)longDouble(eD1) << "\n";
  log << "e<longDouble,4> = " << (long double)longDouble(eD2) << "\n";
  log << "e<longDouble,8> = " << (long double)longDouble(eD3) << "\n";
  log << "e<longDouble,12> = " << (long double)longDouble(eD4) << "\n";

  e<long double> eDL1(1, 1, 0, 1);
  e<long double> eDL2(1, 1, 0, 4);
  e<long double> eDL3(1, 1, 0, 8);
  e<long double> eDL4(1, 1, 0, 12);

  log << "e<long double,1> = " << (long double)eDL1 << "\n";
  log << "e<long double,4> = " << (long double)eDL2 << "\n";
  log << "e<long double,8> = " << (long double)eDL3 << "\n";
  log << "e<long double,12> = " << (long double)eDL4 << "\n";

  e<fraction> eQ1(fraction(1), fraction(1), fraction(0), 1);
  e<fraction> eQ2(fraction(1), fraction(1), fraction(0), 4);
  e<fraction> eQ3(fraction(1), fraction(1), fraction(0), 8);
  e<fraction> eQ4(fraction(1), fraction(1), fraction(0), 12);

  log << "e<fraction,1> = " << fraction(eQ1) << "\n";
  log << "e<fraction,4> = " << fraction(eQ2) << "\n";
  log << "e<fraction,8> = " << fraction(eQ3) << "\n";
  log << "e<fraction,12> = " << fraction(eQ4) << "\n";

  return 0;
}

namespace test {
using efgy::test::function;

static function e(testE);
}
