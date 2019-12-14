/* Test cases for the 'statistics.h' templates
 *
 * Test cases in this file test the functions in libefgy's statistics header.
 *
 * See also:
 * * Project Documentation: https://ef.gy/documentation/libefgy
 * * Project Source Code: https://github.com/ef-gy/libefgy
 * * Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 *
 * @copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 */

#include <ef.gy/statistics.h>
#include <ef.gy/test-case.h>
#include <math.h>

#include <iostream>

using namespace efgy;
using namespace efgy::statistics;
using namespace efgy::test;

bool testStatistics(std::ostream &log) {
  std::vector<int> v;
  std::optional<int> avg = average<int>(v);
  if (avg) {
    log << "average of an empty set is not defined";
    return false;
  }

  std::vector<int> v2{1, 3, -10};

  std::optional<int> avg2 = average<int>(v2);
  if ((*avg2) != -2) {
    log << "average of {1, 3, -10} expected to be -2";
    return false;
  }

  std::vector<int> v3{-10, 1, 3};
  std::optional<int> avg3 = average<int>(v3);
  if ((*avg2) != (*avg3)) {
    log << "averages of {1, 3, -10} and {-10, 1, 3} expected to be equal";
    return false;
  }

  std::vector<double> v4;
  auto var0 = variance<double>(v4);
  if (var0) {
    log << "variance of an empty list is not defined.";
    return false;
  }

  std::vector<double> v5 = {1, 1, 1, 1};
  auto var1 = variance<double>(v5.begin(), v5.end());
  if (*var1 != 0.0) {
    log << "variance of equal values (1) differs from zero.\n";
    return false;
  }

  std::vector<double> v6 = {1, 2, 3, 4};
  auto var2 = variance<double>(v6.begin(), v6.end());

  if (fabs(*var2 - 1.25) >= 1e-14) {
    log << "variance of descending values (1...4) differs from expected "
           "value.\n";
    return false;
  }

  std::vector<double> v7 = {1, 2, 3, 4};
  auto var3 = variance(v7);

  if (fabs(*var3 - 1.25) >= 1e-14) {
    log << "variance of a vector (1...4) without iterators differs from "
           "expected value.\n";
    return false;
  }

  std::vector<double> v1111{0.0, 2.0, 4.0};

  std::vector<double> v8_measurement;
  std::vector<double> v8;
  auto chi0 = chi_square(v8, v8_measurement);
  if (chi0) {
    log << "chi_square of no measurements is not defined\n";
    return false;
  }

  std::vector<double> v10_measurement{0.0, 2.0, 4.0};
  std::vector<double> v10{0.0, 1.0, 2.0};
  auto chi2 = chi_square(v10, v10_measurement);
  if (fabs(*chi2 - (45.0 / 64.0)) > 1e-14) {
    log << "chi_square of descending values is not the expected value\n";
    return false;
  }

  return true;
}

namespace test {
using efgy::test::function;

static function statistics(testStatistics);
}  // namespace test
