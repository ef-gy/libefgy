/* Test cases for the static range sequencer
 *
 * Test cases in this file verify that the code in the range.h header work
 * correctly.
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

#include <ef.gy/range.h>
#include <ef.gy/test-case.h>

#include <algorithm>
#include <array>
#include <iostream>
#include <string>

using namespace efgy;

/* Test case for the static ranged sequence generator
 * @log Where to write log messages to.
 *
 * Creates a series of ranged sequences and verifies they're correct by
 * comparing the results to reference data.
 *
 * @return 'true' on success, 'false' otherwise.
 */
bool testRange(std::ostream &log) {
  std::array<int, 5> a5{0, 1, 2, 3, 4};
  std::array<int, 5> b5{1000, 1001, 1002, 1003, 1004};
  std::array<int, 8> a8n{49, 48, 47, 46, 45, 44, 43, 42};

  for (int i : range<int>(0, 4)) {
    std::cerr << i << "\n";
  }

  std::array<int, 8> a1{42, 43, 44, 45, 46, 47, 48, 49};
  std::size_t p = 0;

  for (int i : range<int, 8>(42)) {
    if (i != a1[p]) {
      log << "range<int, 8>(42) did not produce the expected results.\n";
      return false;
    }

    p++;
  }

  if (!std::equal(a1.begin(), a1.end(), range<int, 8>(42).begin())) {
    log << "range iterator did not produce the expected results.\n";
    return false;
  }

  std::array<int, 8> a2{49, 48, 47, 46, 45, 44, 43, 42};

  if (!std::equal(a2.begin(), a2.end(), range<int, 8>(49, 42).begin())) {
    log << "range iterator did not produce the expected results.\n";
    return false;
  }

  if (!std::equal(a2.begin(), a2.end(), range<int>(49, 42, 8, true).begin())) {
    log << "range iterator did not produce the expected results.\n";
    return false;
  }

  return true;
}

namespace test {
using efgy::test::function;

static function range(testRange);
}  // namespace test
