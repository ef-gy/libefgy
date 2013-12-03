/**\file
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
    namespace markov
    {
        template<typename T, unsigned int order, typename rng = random::mersenneTwister<>, typename counter = unsigned long>
        class chain
        {
            public:
                typedef T element;
                typedef rng random;
                typedef std::vector<T> input;
                typedef std::vector<T> output;

                typedef std::array<maybe<T>, order> memory;
                typedef std::map<maybe<T>, counter> transition;

                chain(const random &pRNG)
                    : RNG(pRNG) {}

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

            protected:
                random RNG;

                std::map<memory, transition> transitions;
        };
    };
};

#endif
