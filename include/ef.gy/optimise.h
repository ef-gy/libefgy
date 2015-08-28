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
 * Copyright (c) 2012-2015, ef.gy Project Members
 * \copyright
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * \copyright
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * \copyright
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * \see Project Documentation: http://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
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
template <int N> class terminateGenerations {
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
