/**\file
 * \brief Test cases for the numeric tracer
 *
 * Test cases in this file verify that the code in the tracer.h header works
 * properly.
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
#include <ef.gy/tracer.h>
#include <algorithm>

using namespace efgy::math::tracer;

/**\brief Test case for the numeric tracer
 * \test Uses the numeric tracer to create a parse tree, then compare that to
 *       sample strings.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testTracer (std::ostream &log)
{
    auto frob = 1 - tracer<int>(1) * 2;

    auto frob2 = frob / frob;
    auto frob3 = frob + frob;

    log << std::string(frob) << "\n";
    log << std::string(frob2) << "\n";

    std::shared_ptr<tracer<int,void,0,true>> frobrt(tracer<int,void,0,true>(1));
    runtime frobrt2 (new tracer<int,void,0,true>(1));
    runtime frobrt3 (frobrt);

    auto frobd  = frobrt + 2;
    runtime frobde = frobrt + 3;

    frobde /= frobd;

    log << frobd << "\n";
    log << frobde << "\n";

    return 0;
}

TEST_BATCH(testTracer)
