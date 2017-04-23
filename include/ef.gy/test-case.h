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

#include <iostream>
#include <set>
#include <vector>

#include <ef.gy/cli.h>

namespace efgy {
namespace test {
/* Run test case batch
 * @test Type of the tests to run. Should be similar to efgy::test::function.
 * @tests Which tests to run.
 *
 * Runs all test cases in the batch. This produces output on stdout about the
 * progress of running these test cases, and the individual test cases are
 * instructed to output any log messages to stderr.
 *
 * The test cases are run in an unspecified but consistent sequence, until
 * either all of the test cases ran successfully, or until one of them fails.
 *
 * @return 'true' if all the test cases ran successfully, 'false' otherwise.
 */
template <class test>
bool run(std::set<test*> &tests = global<std::set<test*>>()) {
  int i = 0;
  for (const auto &f : tests) {
    i++;
    std::cout << "running test case " << i << " in batch of " << tests.size()
              << ": ";
    if (!f->run(std::cerr)) {
      std::cout << "FAIL\n";
      return false;
    }
    std::cout << "OK\n";
  }

  return true;
}

/* Test function wrapper.
 *
 * Automatically adds a test function to a given (or the global) test function
 * batch. Also automatically removes the handler if it goes out of scope.
 */
class function {
 public:
  /* A batch of test cases.
   *
   * Test cases are globally registered items, so we use the registered<>
   * template
   * to keep track of them.
   */
  using batch = std::set<function*>;

  /* Construct with function and batch.
   * @pFunction The function to add.
   * @pBatch The test batch. Defaults to the common global instance.
   *
   * Adds the function to the test batch.
   */
  function(std::function<bool(std::ostream &)> pFunction,
           batch &pBatch = global<batch>())
      : func(pFunction), root(pBatch) {
    root.insert(this);
  }

  /* Destructor.
   *
   * Removes the reference to this object from the test batch. This is to make
   * things work if the object goes out of scope.
   */
  ~function(void) { root.erase(this); }

  /* Run test function.
   * @log Where to write log output to.
   *
   * Basically just wraps around the 'func' function, but does so in a try/catch
   * block to prevent exceptions during the test from affecting everything else.
   *
   * @return 'true' if the test succeeded, 'false' otherwise.
   */
  bool run(std::ostream &log) {
    try {
      return func(log);
    } catch (std::exception &e) {
      std::cerr << "Exception: " << e.what() << "\n";
    } catch (...) {
      std::cerr << "Unknown Exception\n";
    }
    return false;
  }

 protected:
  /* Test function reference.
   *
   * Called by 'run'.
   */
  const std::function<bool(std::ostream &)> func;

  /* The test batch this function is associated with.
   *
   * Each instance of this class has to be associated with a test batch. Since
   * it also needs to get rid of itself, this member variable keeps track of
   * what it's been added to.
   */
  batch &root;
};

#if defined(RUN_TEST_CASES)
/* Test case runner main stub
 * @argc Argument count.
 * @argv Argument vector.
 *
 * This is a very simple main function that will simply run the test cases
 * defined in the programme including this header. Needless to say you should
 * not define your own main function in test case files.
 *
 * Command line arguments are currently ignored.
 *
 * @return '0' on success, '-1' otherwise.
 */
extern "C" int main(int argc, char **argv) {
  if (cli::options(argc, argv, false).matches == 0) {
    return run<function>() ? 0 : -1;
  } else {
    return 0;
  }
}
#else
/* Run built-in test cases.
 *
 * When we haven't compiled withe the main stub, we add a CLI option to run the
 * test cases anyway.
 */
static cli::option runTest("-{0,2}run-tests",
                           [](std::smatch &) -> bool {
                             run<function>();
                             return true;
                           },
                           "run test cases");

#endif
}
}
