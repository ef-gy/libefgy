/* Common code for test cases
 *
 * Most of the test cases follow a very simple pattern, so this file implements
 * certain basic pieces of code that are shared by all test cases. To write
 * test cases, all that needs to be done is to include this header, define
 * appropriate test functions and to use the TEST_BATCH() macro to point out
 * which of the functions in the file are proper test functions.
 *
 * This latter macro will set everything up so that a default main() function
 * can be used that actually runs those tests. You then simply place this code
 * into a file named src/test-case/my-test-case.cpp, and then it gets picked up
 * by 'make test' automagically.
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

/* Library test cases
 *
 * Library test cases all go in the src/test-case/ directory. The makefile will
 * automatically build them, and you can run them in a batch by using the
 * make target 'test', like so:
 *
 *     $ make test
 *
 * If any of the test cases fail to run then the make command will fail with an
 * error. You could use this to run the test cases automatically after a
 * checkout, for example.
 */
namespace test {}

#include <iostream>
#include <set>
#include <vector>

namespace efgy {
/* Functions and types related to test cases
 *
 * This namespace contains functions and types that make defining automatic unit
 * tests easier.
 */
namespace test {
/* Test case function pointer
 *
 * All test cases in a batch are added to a vector to run, so it's important to
 * get their function prototypes right. This typedef reflects the expected
 * function prototype: return an int and take an ostream.
 */
typedef int (*testCase)(std::ostream &log);

class function;

template <class F = function>
class functions {
 public:
  static functions &common(void) {
    static functions f;
    return f;
  }

  void add(F &o) { funcs.insert(&o); }

  void remove(F &o) { funcs.erase(&o); }

  /* Run test case batch
   * @testCases The test cases to run.
   *
   * Runs all test cases specified in the testCases vector. This produces
   * output on stderr about the progress of running these test cases, and
   * the individual test cases are instructed to output any log messages
   * to stderr as well.
   *
   * The test cases are run in the sequence specified by the testCases
   * vector until either all of the test cases ran successfully, or until
   * one of them fails, in which case execution stops and the function
   * returns whatever that failed test case returned.
   *
   * @returns 0 if all test cases ran successfully, the return value of the
   *     first test that failed otherwise.
   */
  int run(void) {
    int i = 0;
    for (const auto &f : funcs) {
      i++;
      std::cerr << "running test case " << i << " in batch of " << funcs.size()
                << ": ";
      int res = f->run(std::cerr);
      if (res != 0) {
        std::cerr << "failed; code: " << res << "\n";
        return res;
      }
      std::cerr << "OK\n";
    }

    return 0;
  }

 protected:
  std::set<F *> funcs;
};

class function {
 public:
  function(std::function<int(std::ostream &)> pFunction,
           functions<function> &pFunctions = functions<function>::common())
      : func(pFunction) {
    pFunctions.add(*this);
  }

  const std::function<int(std::ostream &)> func;

  int run(std::ostream &log) {
    try {
      return func(log);
    } catch (std::exception &e) {
      std::cerr << "Exception: " << e.what() << "\n";
      return -1;
    } catch (...) {
      std::cerr << "Unknown Exception\n";
      return -1;
    }
  }
};

/* Get next return value
 *
 * This function returns a non-zero integer to return after a test case
 * was unsuccessful. This is useful if one wants to add test cases to an
 * existing test later and avoid returning any one value more than once.
 */
static inline int next_integer(void) {
  static int counter = 1;
  ++counter;
  if (counter == 0) {
    return ++counter;
  } else {
    return counter;
  }
}

#if defined(RUN_TEST_CASES)
/* Test case runner main stub
 *
 * This is a very simple main function that will simply run the test cases
 * defined in the programme including this header. Needless to say you should
 * not define your own main function in test case files.
 */
extern "C" int main(int argc, char **argv) {
  return functions<function>::common().run();
}
#endif
}
}
