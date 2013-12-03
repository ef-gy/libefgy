/**\file Markov chains
 *
 * Contains a simple implementation of higher order markov chains of arbitrary
 * data types.
 *
 * \copyright
 * Copyright (c) 2012-2013, ef.gy Project Members
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
 * \see Project Source Code: http://git.becquerel.org/jyujin/libefgy.git
 */

#if !defined(EF_GY_MARKOV_H)
#define EF_GY_MARKOV_H

#include <ef.gy/maybe.h>
#include <ef.gy/random.h>
#include <vector>
#include <array>
#include <map>
#include <numeric>
#include <algorithm>

namespace efgy
{
    /**\brief Namespace for markov models
     *
     * Currently only contains an implementation of higher order markov chains.
     */
    namespace markov
    {
        /**\brief Higher order markov chains
         *
         * This class can both train a model and generate data based on such a
         * model; the order of the markov model and the data type must be set at
         * compile time with the template parameters.
         *
         * Unlike the default algorithm for markov chains, this implementation
         * does not use anything but integers. There is no finalisation step
         * training the model where occurences are transformed into
         * probabilities, and it's perfectly fine to keep training the model
         * with new data as it becomes available while simultaneously generating
         * data.
         *
         * \tparam T       Data type for the model.
         * \tparam order   The order of the markov model to work with.
         * \tparam rng     Functor class for the PRNG to use.
         * \tparam counter Type for a counter; used when training a model and
         *                 when using the model data to generate things.
         */
        template<typename T, unsigned int order, typename rng = random::mersenneTwister<>, typename counter = unsigned long>
        class chain
        {
            public:
                /**\brief Data type for the model
                 *
                 * This is a convenient alias for the T template parameter.
                 */
                typedef T element;

                /**\brief PRNG functor
                 *
                 * An alias for the rng template parameter, which could make it
                 * easier to initialise an instance of the PRNG if you only have
                 * the the model type to work with.
                 */
                typedef rng random;

                /**\brief Input data
                 *
                 * Input to train the model with must be a vector of Ts.
                 */
                typedef std::vector<T> input;

                /**\brief Output data
                 *
                 * Output that is generated has the same type as the input that
                 * the model is trained with, but this separate type alias may
                 * keep things easier to read.
                 */
                typedef std::vector<T> output;

                typedef std::array<maybe<T>, order> memory;
                typedef std::map<maybe<T>, counter> transition;

                chain(const random &pRNG) : RNG(pRNG) {}

                output operator () (void)
                {
                    output rv;
                    memory m;

                    for (typename std::map<memory, transition>::iterator it = transitions.find(m);
                         it != transitions.end();
                         it = transitions.find(m))
                    {
                        counter c = std::accumulate
                            (it->second.begin(), it->second.end(), 0,
                             [] (const counter &v, const std::pair<maybe<T>, counter> &t) -> counter
                             {
                                 return v + t.second;
                             });

                        c = RNG() % c;

                        for (const std::pair<maybe<T>, counter> &t : it->second)
                        {
                            if (c > t.second)
                            {
                                c -= t.second;
                            }
                            else if (t.first)
                            {
                                rv.push_back(T(t.first));

                                std::transform
                                    (m.begin()+1, m.end(), m.begin(),
                                     [] (maybe<T> &i) -> maybe<T>
                                     {
                                         return i;
                                     });

                                *(m.end()-1) = t.first;
                                break;
                            }
                            else
                            {
                                return rv;
                            }
                        }
                    }

                    throw std::runtime_error("impossible memory state in markov chain.");

                    return output();
                }

                chain &operator >> (output &output)
                {
                    output = (*this)();
                    return *this;
                }

                chain &operator << (const input &input)
                {
                    memory m = std::accumulate
                        (input.begin(), input.end(), memory(),
                         [this] (memory s, const T &v) -> memory
                         {
                            transitions[s][v]++;

                            std::transform
                                (s.begin()+1, s.end(), s.begin(),
                                 [] (maybe<T> &i) -> maybe<T>
                                 {
                                    return i;
                                 });

                            *(s.end()-1) = v;

                            return s;
                         });

                    transitions[m][maybe<T>()]++;

                    return *this;
                }

                chain operator << (const input &input) const
                {
                    chain cs = *this;
                    return (cs << input);
                }

                random RNG;
                std::map<memory, transition> transitions;
        };
    };
};

#endif
