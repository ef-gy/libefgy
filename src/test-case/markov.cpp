/**\file
 * \brief Test cases for the markov chain algorithm
 *
 * These test cases test the markov chains implementation in the markov.h file;
 * they also indirectly rely on the mersenne twister implementation in
 * random.h.
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

#include <ef.gy/test-case.h>
#include <ef.gy/markov.h>

using namespace efgy;

/**\brief Markov chain test case
 * \test Trains a model and tries to generate some data with a markov chain of
 *       the specified order.
 *
 * \tparam order The order of the markov chain to train and test.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
template <unsigned int order> int testMarkovChain(std::ostream &log) {
  typename markov::chain<char, order>::random PRNG(1);
  markov::chain<char, order> mc(PRNG);

  std::string str1("frob");
  std::string str2("frib");
  std::string str3("frab");
  std::string str4("foo");
  std::string str5("bar");
  std::string str6("baz");

  mc << typename markov::chain<char, order>::input(str1.begin(), str1.end());
  mc << typename markov::chain<char, order>::input(str2.begin(), str2.end());
  mc << typename markov::chain<char, order>::input(str3.begin(), str3.end());
  mc << typename markov::chain<char, order>::input(str4.begin(), str4.end());
  mc << typename markov::chain<char, order>::input(str5.begin(), str5.end());
  mc << typename markov::chain<char, order>::input(str6.begin(), str6.end());

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

  return 0;
}

TEST_BATCH(testMarkovChain<1>, testMarkovChain<2>, testMarkovChain<3>)
