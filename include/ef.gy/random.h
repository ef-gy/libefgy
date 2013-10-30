/**\file
 * \brief Random Numbers
 *
 * Contains RNGs and assorted utilities related to random numbers.
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

#if !defined(EF_GY_RANDOM_H)
#define EF_GY_RANDOM_H

namespace efgy
{
    /**\brief Random Numbers
     *
     * This namespace currently only contains an implementation of the Mersenne
     * Twister algorithm to produce random numbers.
     */
    namespace random
    {
        /**\brief Simple Mersenne Twister implementation
         *
         * For now this class implements a 32-bit Mersenne Twister RNG;
         *
         * \see Implementation follows the pseudocode found at
         *      http://en.wikipedia.org/wiki/Mersenne_twister
         */
        template <typename T = unsigned long long>
        class mersenneTwister
        {
            public:
                /**\brief Construct with seed
                 *
                 * Initialise the Mersenne Twister state with the given seed.
                 * Using the same seed should always provide the same sequence
                 * of pseudo-random numbers.
                 *
                 * \param[in] seed The seed to use.
                 */
                mersenneTwister(const T &seed)
                    : index(0)
                    {
                        MT[0] = seed;
                        for (int i = 1; i < stateSize; i++)
                        {
                            MT[i] = bitmask & ((0x6c078965 * (MT[i-1] ^ (MT[i-1] >> 30))) + i);
                        }
                    }

                /**\brief Generate random number
                 *
                 * Generates the next random number based on the current state
                 * with the Mersenne Twister algorithm.
                 *
                 * \return A random number in the range [0, (2^32)).
                 */
                T rand(void)
                {
                    if(index == 0) 
                    {
                        generateNumbers();
                    }
                    T y = MT[index];

                    y = y ^ (y >> 11);
                    y = y ^ ((y << 7) & 0x9d2c5680);
                    y = y ^ ((y << 15) & 0xefc60000);
                    y = y ^ (y >> 18);

                    index = (index + 1) % stateSize;

                    return y;
                }

                /**\brief Generate random number in interval
                 *
                 * This function is the same as rand() but the returned number
                 * is in the range [min, max].
                 *
                 * \param[in] min The minimum value for the generated numbers.
                 * \param[in] max The maximum value for the generated numbers.
                 *
                 * \returns A random number between min and max 
                 */
                T rand(T min, T max)
                {
                    T orig = rand();
            
                    if(min > max)
                    {
                        T swap = min;
                        min = max;
                        max = swap;
                    }

                    double scaled = (((double) orig / (double) bitmask) * ((double) (max - min))) + (double) min;

                    return (T) scaled;
                }

            protected:
                /**\brief Repopulate state array
                 *
                 * Regenerates the state array. The Mersenne Twister algorithm
                 * requires that this step be performed periodically.
                 */
                void generateNumbers(void)
                {
                    for (int i = 0; i < stateSize; i++)
                    {
                        T y = (MT[i] & 0x80000000) + (MT[(i + 1) % stateSize] & 0x7fffffff);
                        MT[i] = MT[(i + 397) % stateSize] ^ (y >> 1);
                        if((y % 2) != 0)
                        {
                            MT[i] = MT[i] ^ 0x9908b0df;
                        }
                    }
                }

                /**\brief The size of the state array
                 *
                 * The Mersenne Twister algorithm uses a fixed-size state
                 * array; this is the size of that array.
                 */
                static const unsigned int stateSize = 624;

                /**\brief Output bitmask
                 *
                 * This implementation of the Mersenne Twister algorithm
                 * produces 32-bit integers, so this bitmask is used to cut off
                 * any excess bits that might have been produced.
                 */
                static const T bitmask = ((1LL<<32)-1);

                /**\brief Current position in the state array
                 *
                 * The Mersenne Twister algorithm walks through the state array
                 * one by one. This is the position in that array.
                 */
                unsigned int index;

                /**\brief Mersenne Twister state array
                 *
                 * This is the state array used by the Mersenne Twister
                 * algorithm. This is periodically regenerated by the
                 * regenerateNumbers() function.
                 */
                T MT[stateSize];
        };
    };
};

#endif
