/**\file
 * \brief Test cases for the VT100 I/O code
 *
 * Covers the functionality in the terminal.h and vt100.h headers.
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
#include <ef.gy/vt100.h>
#include <ef.gy/random.h>

using namespace efgy;

/**\brief VT100 output tests
 * \test Tries to create some output on the current terminal using the vt100
 *       helpers.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testVT100 (std::ostream &log)
{
    terminal::vt100<> output;
    random::mersenneTwister<> mt(1337);

    output.resize(output.getOSDimensions());

    std::array<std::size_t,2> s = output.size();

    log << s[0] << "x" << s[1] << "\n";

    for (unsigned int l = 0; l < s[1]; l++)
    {
        for (unsigned int c = 0; c < s[0]; c++)
        {
            output.target[l][c].content = mt() & ((1<<7)-1);
        }
    }

    std::cout << output.transform();

    return 0;
}

TEST_BATCH(testVT100)
