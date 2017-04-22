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
/* Test batch.
 * @test test function wrapper.
 *
 * Represents a set of test functions, which are represented by the 'test'
 * class. The class allows for adding functions automagically, merely by having
 * them defined. This is accomplished with the help of a static 'common'
 * instance, which is global and can be added to at will.
 */
template <class test>
class batch {
 public:
  /* Get single global instance.
   *
   * This is a little hack that establishes a single, global master instance of
   * test batches. Due to how templates work, there will be one master instance
   * per test function type, as there will be one separate function for each of
   * these and the global instance is instantiated by having a 'static' batch
   * in this function.
   */
  static batch &common(void) {
    static batch f;
    return f;
  }

  /* Add test case.
   * @o A test case that will be added.
   *
   * Adds 'o' to the 'funcs' set. 'o' must not go out of scope after it has
   * been added, which means that since this is usually used with the common()
   * instance, it should have global or static scope.
   */
  void add(test &o) { tests.insert(&o); }

  /* Remove test case.
   * @o A test case that will be removed.
   *
   * Removes 'o' from the 'funcs' set. The default function handler from below
   * will automatically call this in the destructor, which makes sure things
   * don't blow up if the test function wrapper does go out of scope.
   */
  void remove(test &o) { tests.erase(&o); }

  /* Run test case batch
   *
   * Runs all test cases in the batch. This produces output on stderr about the
   * progress of running these test cases, and the individual test cases are
   * instructed to output any log messages to stderr as well.
   *
   * The test cases are run in an unspecified but consistent sequence, until
   * either all of the test cases ran successfully, or until one of them fails,
   * in which case execution stops and the function returns whatever that failed
   * test case returned.
   *
   * @return 'true' if all the test cases ran successfully, 'false' otherwise.
   */
  bool run(void) {
    int i = 0;
    for (const auto &f : tests) {
      i++;
      std::cerr << "running test case " << i << " in batch of " << tests.size()
                << ": ";
      if (!f->run(std::cerr)) {
        std::cerr << "FAIL\n";
        return false;
      }
      std::cerr << "OK\n";
    }

    return true;
  }

 protected:
  /* The set of test functions.
   *
   * Set with 'add', removed with 'remove' and used in 'run'.
   */
  std::set<test *> tests;
};

/* Test function wrapper.
 *
 * Automatically adds a test function to a given (or the global) test function
 * batch. Also automatically removes the handler if it goes out of scope.
 */
class function {
 public:
  /* Construct with function and batch.
   * @pFunction The function to add.
   * @pBatch The test batch. Defaults to the common global instance.
   *
   * Adds the function to the test batch.
   */
  function(std::function<bool(std::ostream &)> pFunction,
           batch<function> &pBatch = batch<function>::common())
      : func(pFunction), root(pBatch) {
    root.add(*this);
  }

  /* Destructor.
   *
   * Removes the reference to this object from the test batch. This is to make
   * things work if the object goes out of scope.
   */
  ~function(void) { root.remove(*this); }

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
  batch<function> &root;
};

#if defined(RUN_TEST_CASES)
/* Test case runner main stub
 *
 * This is a very simple main function that will simply run the test cases
 * defined in the programme including this header. Needless to say you should
 * not define your own main function in test case files.
 *
 * Command line arguments are currently ignored.
 *
 * @return '0' on success, '-1' otherwise.
 */
extern "C" int main(int, char **) {
  return batch<function>::common().run() ? 0 : -1;
}
#endif
}
}
