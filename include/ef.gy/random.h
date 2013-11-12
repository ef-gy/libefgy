/*
 * This file is part of the ef.gy project.
 * See the appropriate repository at http://ef.gy/.git for exact file
 * modification records.
*/

/*
 * Copyright (c) 2012-2013, ef.gy Project Members
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

#if !defined(EF_GY_RANDOM_H)
#define EF_GY_RANDOM_H

#include <limits>

/*
    for now this class implements a 32-bit Mersenne Twister RNG;

    implementation follows the pseudocode found at http://en.wikipedia.org/wiki/Mersenne_twister.
*/
template <typename T = unsigned long long>
class MersenneTwister
{
    public:
        MersenneTwister(int seed) : index(0)
        {
            MT[0] = seed;
            for (int i = 1; i < 624; i++)
            {
                MT[i] = bitmask & ((0x6c078965 * (MT[i-1] ^ (MT[i-1] >> 30))) + i);
            }
        }

        T rand()
        {
            if(index == 0) 
            {
                generateNumbers();
            }
            assert(index < 624);
            T y = MT[index];

            y = y ^ (y >> 11);
            y = y ^ ((y << 7) & 0x9d2c5680);
            y = y ^ ((y << 15) & 0xefc60000);
            y = y ^ (y >> 18);

            index = (index + 1) % 624;

            return y;
        }
        
        //! returns a random number between min and max 
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


        void generateNumbers()
        {
            for (int i = 0; i < 624; i++)
            {
                T y = (MT[i] & 0x80000000) + (MT[(i + 1) % 624] & 0x7fffffff);
                MT[i] = MT[(i + 397) % 624] ^ (y >> 1);
                if((y % 2) != 0) {
                    MT[i] = MT[i] ^ 0x9908b0df;
                }
            }
        }

    private:
        static const unsigned long long bitmask = ((1<<32)-1);

        int index;
        T MT[624];
};


#endif
