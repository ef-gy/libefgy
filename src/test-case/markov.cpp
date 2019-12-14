/* Test cases for the markov chain algorithm
 *
 * These test cases test the markov chains implementation in the markov.h file;
 * they also indirectly rely on the mersenne twister implementation in
 * random.h.
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

#include <ef.gy/markov.h>
#include <ef.gy/test-case.h>

#include <iostream>

using namespace efgy;

/* Markov chain test case
 * @log Where to write log messages to.
 *
 * Trains a model and tries to generate some data with a markov chain of the
 * specified order.
 *
 * @return 'true' on success, 'false' otherwise.
 */
template <unsigned int order>
bool testMarkovChain(std::ostream &log) {
  typename markov::chain<char, order>::random PRNG(1);
  markov::chain<char, order> mc(PRNG);

  const std::vector<std::string> input{"frob", "frib", "frab",
                                       "foo",  "bar",  "baz"};
  for (const auto &i : input) {
    mc << typename markov::chain<char, order>::input(i.begin(), i.end());
  }

  typename markov::chain<char, order>::output out;

  mc >> out;
  std::cerr << std::string(out.begin(), out.end()) << "\n";
  mc >> out;
  std::cerr << std::string(out.begin(), out.end()) << "\n";
  mc >> out;
  std::cerr << std::string(out.begin(), out.end()) << "\n";
  mc >> out;
  std::cerr << std::string(out.begin(), out.end()) << "\n";
  mc >> out;
  std::cerr << std::string(out.begin(), out.end()) << "\n";

  return true;
}

namespace test {
using efgy::test::function;

static function markovChain1(testMarkovChain<1>);
static function markovChain2(testMarkovChain<2>);
static function markovChain3(testMarkovChain<3>);
}  // namespace test
