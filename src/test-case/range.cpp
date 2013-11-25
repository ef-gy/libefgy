/**\file
 * \brief Test cases for the static range sequencer
 *
 * Test cases in this file verify that the code in the range.h header work
 * correctly.
 *
 * \copyright
 * Copyright (c) 2013, ef.gy Project Members
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
#include <string>

#include <ef.gy/test-case.h>
#include <ef.gy/range.h>
#include <algorithm>

using namespace efgy;

/**\brief Test case for the static ranged sequence generator
 * \test Creates a series of ranged sequences and verifies they're correct by
 *       comparing the results to reference data.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testRange (std::ostream &log)
{
    if (range<int,5>::get() != std::array<int,5>({{ 0, 1, 2, 3, 4 }}))
    {
        log << "5-element sequence does not have the expected value.\n";
        return -1;
    }

    if (range<int,5>::get(1000) != std::array<int,5>({{ 1000, 1001, 1002, 1003, 1004 }}))
    {
        log << "5-element sequence does not have the expected value.\n";
        return -2;
    }

    if (sequence<int,49,42>() != std::array<int,8>({{ 49, 48, 47, 46, 45, 44, 43, 42 }}))
    {
        log << "8-element sequence does not have the expected value.\n";
        return -3;
    }

    std::array<int,8> a1 = sequence<int,42,49>();
    std::size_t p = 0;

    for (int i : range<int,8>(42))
    {
        if (i != a1[p])
        {
            log << "range iterator did not produce the expected results.\n";
            return -4;
        }

        p++;
    }

    if (!std::equal (a1.begin(), a1.end(), range<int,8>(42).begin()))
    {
        log << "range iterator did not produce the expected results.\n";
        return -5;
    }

    std::array<int,8> a2 = sequence<int,49,42>();

    if (!std::equal (a2.begin(), a2.end(), range<int,8>(49,42).begin()))
    {
        log << "range iterator did not produce the expected results.\n";
        return -6;
    }

    return 0;
}

TEST_BATCH(testRange)
