/**
 * \file
 *
 * \brief Provides generic functionaltiy used by each optimisation
 * implementation.
 *
 * Examples of optimisation implementations are genetic.h and
 * particle-swarm-optimisation.h (future work).
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

#if !defined(EF_GY_OPTIMISE_H)
#define EF_GY_OPTIMISE_H

#include <random>

namespace efgy {
namespace optimise {

/** \brief Implements GA termination after N generations.

            \tparam N is the number of generations after which the GA should
    terminate.
        */
template <int N>
class terminateGenerations {
 public:
  terminateGenerations() : current(0){};
  bool operator()(void) { return (++current > N); }

 private:
  int current;
};

/** \brief Functor that initialises an array of float with randomly generated
* numbers between 0 and 1.

        */
class initialiseFloatsRandomly {
 public:
  initialiseFloatsRandomly(){};

  void operator()(float *array, int length) {
    std::random_device rd;
    std::mt19937 rng(rd());
    for (int i = 0; i < length; i++) {
      float r = std::generate_canonical<float, 10>(rng);
      array[i] = r;
    }
  }
};

/** \brief Functor that initialises an array of booleans randomly..*/
class initialiseBooleansRandomly {
 public:
  initialiseBooleansRandomly(){};

  void operator()(bool *array, int length) {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<> dis(0, 1);
    for (int i = 0; i < length; i++) {
      bool r = (dis(rng) % 2) == 0;
      array[i] = r;
    }
  }
};
}
}

#endif
