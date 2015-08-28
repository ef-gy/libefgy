/**\file
 * \brief Test cases for the 'statistics.h' templates
 *
 * Test cases in this file test the functions in libefgy's statistics header.
 *
 * \copyright
 * Copyright (c) 2012-2015, ef.gy Project Members
 * \copyright
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * \copyright
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * \copyright
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * \see Project Documentation: http://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 */

#include <iostream>
#include <math.h>

#include <ef.gy/test-case.h>
#include <ef.gy/statistics.h>

using namespace efgy;
using namespace efgy::statistics;
using namespace efgy::test;

int testStatistics(std::ostream &log) {
  std::vector<int> v;
  maybe<int> avg = average<int>(v);
  if (avg) {
    log << "average of an empty set is not defined";
    return next_integer();
  }

  std::vector<int> v2{1, 3, -10};

  maybe<int> avg2 = average<int>(v2);
  if (((int)avg2) != -2) {
    log << "average of {1, 3, -10} expected to be -2";
    return next_integer();
  }

  std::vector<int> v3{-10, 1, 3};
  maybe<int> avg3 = average<int>(v3);
  if (((int)avg2) != ((int)avg3)) {
    log << "averages of {1, 3, -10} and {-10, 1, 3} expected to be equal";
    return next_integer();
  }

  std::vector<double> v4;
  auto var0 = variance<double>(v4);
  if (var0) {
    log << "variance of an empty list is not defined.";
    return next_integer();
  }

  std::vector<double> v5 = {1, 1, 1, 1};
  auto var1 = variance<double>(v5.begin(), v5.end());
  if ((double)var1 != 0.0) {
    log << "variance of equal values (1) differs from zero.\n";
    return next_integer();
  }

  std::vector<double> v6 = {1, 2, 3, 4};
  auto var2 = variance<double>(v6.begin(), v6.end());

  if (fabs((double)var2 - 1.25) >= 1e-14) {
    log << "variance of descending values (1...4) differs from expected "
           "value.\n";
    return next_integer();
  }

  std::vector<double> v7 = {1, 2, 3, 4};
  auto var3 = variance(v7);

  if (fabs((double)var3 - 1.25) >= 1e-14) {
    log << "variance of a vector (1...4) without iterators differs from "
           "expected value.\n";
    return next_integer();
  }

  std::vector<double> v1111{0.0, 2.0, 4.0};

  std::vector<double> v8_measurement;
  std::vector<double> v8;
  auto chi0 = chi_square(v8, v8_measurement);
  if (chi0) {
    log << "chi_square of no measurements is not defined\n";
    return next_integer();
  }

  //     std::vector<double> v9_measurement {1.0, 1.0, 1.0};
  //     std::vector<double> v9 {1.0, 1.0, 1.0};
  //     auto chi1 = chi_square(v9, v9_measurement);
  //     if (chi1)
  //     {
  //         log << "chi_square of no variance is not defined\n";
  //         return next_integer();
  //     }

  std::vector<double> v10_measurement{0.0, 2.0, 4.0};
  std::vector<double> v10{0.0, 1.0, 2.0};
  auto chi2 = chi_square(v10, v10_measurement);
  if (fabs((double)chi2 - (45.0 / 64.0)) > 1e-14) {
    log << "chi_square of descending values is not the expected value\n";
    return next_integer();
  }

  return 0;
}

TEST_BATCH(testStatistics)
