/**\file
 * \brief Test cases for the Mersenne Twister PRNG
 *
 * These test cases test the random number generator in the random.h file.
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

#include <iostream>

#include <ef.gy/test-case.h>
#include <ef.gy/random.h>

using namespace efgy::random;

/**\brief Update a bitwise histogram
 *
 * This function is used to update a histogram of all the bits in a set of
 * numbers; this information can then be used to determine how 'random' the
 * distribution of the inidividual bits in a PRNG's output is.
 */
template<typename T = unsigned long long, unsigned int size = 32>
void updateBitHistogram (const T &number, T histogram[size])
{
    for (unsigned int i = 0; i < size; i++)
    {
        if (number & (1L<<i))
        {
            histogram[i]++;
        }
    }
}

/**\brief Mersenne Twister test case
 * \test This test case creates a series of random numbers to see if the MT
 *       implementation produces reliably random results.
 *
 * \tparam seed The seed to use for the tests. This is a template argument so
 *              as not to pollute the function prototype for the test case
 *              runner.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
template<typename T = unsigned long long, T seed, unsigned int size = 32, unsigned int iterations = 1000, unsigned int maxdeviation = 64>
int testMersenneTwister (std::ostream &log)
{
    mersenneTwister<> MT(seed);
    T histogram[size];

    for (unsigned int i = 0; i < iterations; i++)
    {
        T rn = MT.rand();

        updateBitHistogram (rn, histogram);
    }

    T min = histogram[0];
    T max = histogram[0];
    T sum = histogram[0];

    for (unsigned int i = 1; i < size; i++)
    {
        if (min > histogram[i])
        {
            min = histogram[i];
        }
        if (max < histogram[i])
        {
            max = histogram[i];
        }

        sum += histogram[i];
    }

    T avg = sum / size;
    T mindev = avg - min;
    T maxdev = max - avg;

    log << "MT results: seed=" << seed << "; (avg,min,max) = (" << avg << "; -" << mindev << ", +" << maxdev << "); ";

    if ((mindev > maxdeviation) || (maxdev > maxdeviation))
    {
        log << "bit histogram deviation in PRNG results exceeds tolerance=" << maxdeviation<< "\n";
        return -1;
    }

    return 0;
}

TEST_BATCH
    (testMersenneTwister<unsigned long long,     42, 32, 100000, 768>,
     testMersenneTwister<unsigned long long,     23, 32, 100000, 768>,
     testMersenneTwister<unsigned long long, 123456, 32, 100000, 768>)
