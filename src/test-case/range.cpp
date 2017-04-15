/**\file
 * \brief Test cases for the static range sequencer
 *
 * Test cases in this file verify that the code in the range.h header work
 * correctly.
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
#include <string>

#include <ef.gy/range.h>
#include <ef.gy/test-case.h>
#include <algorithm>

using namespace efgy;

/**\brief Test case for the static ranged sequence generator
 * \test Creates a series of ranged sequences and verifies they're correct by
 *       comparing the results to reference data.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testRange(std::ostream &log) {
  if (range<int, 5>::get() != std::array<int, 5>({{0, 1, 2, 3, 4}})) {
    log << "5-element sequence does not have the expected value.\n";
    return -1;
  }

  if (range<int, 5>::get(1000) !=
      std::array<int, 5>({{1000, 1001, 1002, 1003, 1004}})) {
    log << "5-element sequence does not have the expected value.\n";
    return -2;
  }

  const auto res3 = std::array<int, 8>({{49, 48, 47, 46, 45, 44, 43, 42}});
  if (range<int, 8>::get(49, -1) != res3) {
    log << "8-element sequence does not have the expected value.\n";
    return -3;
  }

  for (int i : range<int>(0, 4)) {
    std::cerr << i << "\n";
  }

  std::array<int, 8> a1 = range<int, 8>::get(42);
  std::size_t p = 0;

  for (int i : range<int, 8>(42)) {
    if (i != a1[p]) {
      log << "range iterator did not produce the expected results.\n";
      return -4;
    }

    p++;
  }

  if (!std::equal(a1.begin(), a1.end(), range<int, 8>(42).begin())) {
    log << "range iterator did not produce the expected results.\n";
    return -5;
  }

  std::array<int, 8> a2 = range<int, 8>::get(49, -1);

  if (!std::equal(a2.begin(), a2.end(), range<int, 8>(49, 42).begin())) {
    log << "range iterator did not produce the expected results.\n";
    return -6;
  }

  if (!std::equal(a2.begin(), a2.end(), range<int>(49, 42, 8, true).begin())) {
    log << "range iterator did not produce the expected results.\n";
    return -7;
  }

  if (!std::equal(a1.begin(), a1.end(), range<int>(42, 49, true).begin())) {
    log << "range iterator did not produce the expected results.\n";
    return -8;
  }

  if (!std::equal(a1.begin(), a1.end(), range<int>(42, 50, false).begin())) {
    log << "range iterator did not produce the expected results.\n";
    return -9;
  }

  if (range<int, 0>::get() != std::array<int, 0>({{}})) {
    log << "empty range was not an empty array.\n";
    return -10;
  }

  if (range<int, 0>::get(1, 1) != std::array<int, 0>({{}})) {
    log << "an empty range with a non-default start was not empty.\n";
    return -11;
  }

  if (range<int, 3>::get(3, -1) != std::array<int, 3>({{3, 2, 1}})) {
    log << "a range with negative stepsize worked as expected.\n";
    return -12;
  }
  return 0;
}

TEST_BATCH(testRange)
