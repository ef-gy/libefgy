/* Markov chains
 *
 * Contains a simple implementation of higher order markov chains of arbitrary
 * data types.
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

#if !defined(EF_GY_MARKOV_H)
#define EF_GY_MARKOV_H

#include <algorithm>
#include <array>
#include <map>
#include <numeric>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace efgy {
/* Namespace for markov models
 *
 * Currently only contains an implementation of higher order markov chains.
 */
namespace markov {
/* Higher order markov chains
 * @T Data type for the model.
 * @order The order of the markov model to work with.
 * @rng Functor class for the PRNG to use.
 * @counter Type for a counter; used when training a model and when using the
 * model data to generate things.
 *
 * This class can both train a model and generate data based on such a model;
 * the order of the markov model and the data type must be set at compile time
 * with the template parameters.
 *
 * Unlike the default algorithm for markov chains, this implementation does not
 * use anything but integers. There is no finalisation step when training the
 * model where occurences are transformed into probabilities, and it's perfectly
 * fine to keep training the model with new data as it becomes available while
 * simultaneously generating data.
 */
template <typename T, unsigned int order, typename rng = std::mt19937,
          typename counter = unsigned long>
class chain {
 public:
  /* Data type for the model
   *
   * This is a convenient alias for the T template parameter.
   */
  using element = T;

  /* PRNG functor
   *
   * An alias for the rng template parameter, which could make it
   * easier to initialise an instance of the PRNG if you only have
   * the the model type to work with.
   */
  using random = rng;

  /* Input data
   *
   * Input to train the model with must be a vector of Ts.
   */
  using input = std::vector<element>;

  /* Output data
   *
   * Output that is generated has the same type as the input that the model is
   * trained with, but this separate type alias may keep things easier to read.
   */
  using output = std::vector<element>;

  /* State data
   *
   * Holds the data used to determine the next transition when generating data
   * and to identify the possible transitions in the model.
   */
  using memory = std::array<std::optional<T>, order>;

  /* Next state and associated probability
   *
   * This is what the transitions table maps to; uses a std::optional<T> as
   * opposed to a T to allow for terminating transitions by using a value of
   * nothing in the std::optional<T> as opposed to a T.
   *
   * The probability is only recorded indirectly by recording how often a
   * transition occured while training the model. To get the actual probability,
   * you'll need to sum up all the possible transitions after looking up a state
   * in the transitions map.
   */
  using transition = std::map<std::optional<T>, counter>;

  /* Construct with RNG
   * @pRNG The random number generator to use.
   *
   * Creates a copy of the given RNG to initialise the model. This is the only
   * constructor, as RNGs usually can't be initialised sanely without a seed
   * and creating a markov chain without an RNG to create data with later is
   * utterly pointless.
   */
  chain(const random &pRNG) : RNG(pRNG) {}

  /* Construct with RNG and data array
   * @Array The type of the data array that is passed.
   * @pRNG The random number generator to use.
   * @pData The data to train the model with.
   *
   * Creates a copy of the given RNG to initialise the model. This is required,
   * as RNGs usually can't be initialised sanely without a seed and creating a
   * markov chain without an RNG to create data with later is utterly pointless.
   *
   * The model is then trained with the array given in the pData parameter by
   * sequentially feeding the elements in that array into the model.
   */
  chain(const random &pRNG, const std::vector<input> &pData) : RNG(pRNG) {
    for (const auto &data : pData) {
      train(data);
    }
  }

  /* Construct with RNG and data array
   * @Array The type of the data array that is passed.
   * @pRNG The random number generator to use.
   * @pData The data to train the model with.
   *
   * Creates a copy of the given RNG to initialise the model. This is required,
   * as RNGs usually can't be initialised sanely without a seed and creating a
   * markov chain without an RNG to create data with later is utterly pointless.
   *
   * The model is then trained with the array given in the pData parameter by
   * sequentially feeding the elements in that array into the model.
   */
  chain(const random &pRNG, const std::map<input, counter> &pData) : RNG(pRNG) {
    for (const auto &data : pData) {
      train(data.first, data.second);
    }
  }
  /**\brief Generate data with model
   *
   * Use this function after training the model sufficiently; this will then
   * create a sequence of Ts based on what you trained the model with.
   *
   * @return A vector of Ts based on the training data.
   *
   * Throwds a std::runtime_error iff the code manages to generate a state with
   * no successors. This should only ever happen if you try to generate data
   * without training the model first.
   */
  output operator()(void) {
    output rv;
    memory m;

    for (typename std::map<memory, transition>::iterator it =
             transitions.find(m);
         it != transitions.end(); it = transitions.find(m)) {
      counter c = std::accumulate(
          it->second.begin(), it->second.end(), 0,
          [](const counter &v, const std::pair<std::optional<T>, counter> &t)
              -> counter { return v + t.second; });

      c = RNG() % c;

      for (const std::pair<std::optional<T>, counter> &t : it->second) {
        if (c > t.second) {
          c -= t.second;
        } else if (t.first) {
          rv.push_back(*t.first);

          std::transform(
              m.begin() + 1, m.end(), m.begin(),
              [](std::optional<T> &i) -> std::optional<T> { return i; });

          *(m.end() - 1) = t.first;
          break;
        } else {
          return rv;
        }
      }
    }

    throw std::runtime_error("impossible memory state in markov chain.");

    return output();
  }

  /* Generate data based on model
   * @output A reference to the variable to extract to.
   *
   * This is an overload of the 'extraction operator' as C++-ers
   * like to call it. This is purely for convenience if you're
   * used to working with streams a lot.
   *
   * @return A reference to this instance, much like a stream would behave.
   */
  chain &operator>>(output &output) {
    output = (*this)();
    return *this;
  }

  /* Generate string data based on model
   * @pOutput A reference to a variable to extract to.
   *
   * This is an overload of the 'extraction operator' as C++-ers
   * like to call it. This is purely for convenience if you're
   * used to working with streams a lot and would like to get a
   * string instead of a vector.
   *
   * @return A reference to this instance, much like a stream would behave.
   */
  chain &operator>>(std::basic_string<T> &pOutput) {
    output out = (*this)();
    pOutput = std::basic_string<T>(out.begin(), out.end());
    return *this;
  }

  /* Train model with rate of occurence
   * @input An input vector for the model.
   * @c How often the vector occurs.
   *
   * Use the given input to refine the model.
   *
   * The model is trained by constructing a null state and then folding along
   * the input data, shifting new Ts in and increasing counters as appropriate
   * for the transitions that occur in the input.
   *
   * @return A reference to this instance, much like a stream would behave.
   */
  chain &train(const input &input, const counter &c) {
    memory m = std::accumulate(
        input.begin(), input.end(), memory(),
        [this, c](memory s, const T &v) -> memory {
          transitions[s][v] += c;

          std::transform(
              s.begin() + 1, s.end(), s.begin(),
              [](std::optional<T> &i) -> std::optional<T> { return i; });

          *(s.end() - 1) = v;

          return s;
        });

    transitions[m][std::optional<T>()]++;

    return *this;
  }

  /* Train model
   * @input A vector of Ts to train the model with.
   *
   * Use the given input to refine the model; this is provided as
   * an 'insertion operator', as C++-ers like to call it, so that
   * it feels familiar to programmers used to working with C++
   * streams.
   *
   * The model is trained by constructing a null state and then
   * folding along the input data, shifting new Ts in and
   * increasing counters as appropriate for the transitions that
   * occur in the input.
   *
   * @return A reference to this instance, much like a stream would behave.
   */
  chain &operator<<(const input &input) { return train(input, 1); }

  /* Train model with string
   * @pInput A string of Ts to train the model with.
   *
   * An overload of the plain training function; this variant
   * makes it possible to pass a regular string type if the T
   * parameter makes it possible to do so.
   *
   * @return A reference to this instance, much like a stream would behave.
   */
  chain &operator<<(const std::basic_string<T> &pInput) {
    return train(input(pInput.begin(), pInput.end()), 1);
  }

  /**\brief Random number generator
   *
   * This is a copy of the PRNG you pass to the constructor; used
   * extensively when generating data.
   */
  random RNG;

  /**\brief Actual model data
   *
   * This map contains the actual transition data that you trained
   * with. Each encountered state maps to a series of possible
   * transitions, which in turn map to a counter indicating how
   * that particular transition occured while training the model.
   */
  std::map<memory, transition> transitions;
};
}  // namespace markov
}  // namespace efgy

#endif
