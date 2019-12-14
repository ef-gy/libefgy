/* Test cases for the global object helpers.
 *
 * Tests the global object registration and its properties.
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

#include <ef.gy/global.h>
#include <ef.gy/test-case.h>

using namespace efgy;

/* Beacon registration tests.
 * @log Where to write log messages to.
 *
 * testBeacon() plays with initialising some beacons and cross-examines that the
 * behavior of the registration set is as expected.
 *
 * @return 'true' on success, 'false' otherwise.
 */
bool testBeacon(std::ostream &log) {
  int a, b, c, d;
  auto &root = global<beacons<int>>();

  if (root.size() != 0) {
    log << "beacon set size before registering anything should be 0 but is: "
        << root.size() << "\n";
    return false;
  }

  {
    beacon<int> ba(a);

    if (root.size() != 1) {
      log << "beacon set size after adding one element should be 1 but is: "
          << root.size() << "\n";
      return false;
    }

    {
      beacon<int> bb(b);
      beacon<int> bc(c);
      beacon<int> bd(d);

      if (root.size() != 4) {
        log << "beacon set size at this scope should be 4 but is: "
            << root.size() << "\n";
        return false;
      }
    }

    if (root.size() != 1) {
      log << "beacon set size after expiring three elements should be 1 but "
             "is: "
          << root.size() << "\n";
      return false;
    }
  }

  if (root.size() != 0) {
    log << "beacon set size after expiring all elements should be 0 but is: "
        << root.size() << "\n";
    return false;
  }

  return true;
}

namespace test {
using efgy::test::function;

static function beacon(testBeacon);
}  // namespace test
