/**\file
 * \brief Test cases for the numeric tracer
 *
 * Test cases in this file verify that the code in the tracer.h header works
 * properly.
 *
 * \copyright
 * Copyright (c) 2012-2014, ef.gy Project Members
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

#include <iostream>
#include <string>

#include <ef.gy/test-case.h>
#include <ef.gy/tracer.h>
#include <ef.gy/matrix.h>
#include <algorithm>

using namespace efgy::math::tracer;
using efgy::math::matrix;

/**\brief Test case for the numeric tracer
 * \test Uses the numeric tracer to create a parse tree, then compares that to
 *       sample strings.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testTracer (std::ostream &log)
{
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

/**\brief Test case for the numeric tracer with matrix operations
 * \test Uses the numeric tracer to create a parse tree, then compare that to
 *       sample strings, much like testTracer(). Uses matrix manipulations to
 *       see if the calculations work with slightly more complex functions.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testTracerMatrix (std::ostream &log)
{
    matrix<runtime,3,3> m1;
    matrix<runtime,3,3> m2;

    for (unsigned int i = 0; i < 3; i++)
    {
        for (unsigned int j = 0; j < 3; j++)
        {
            std::stringstream s("");
            s << "_" << i << "_" << j;

            m1[i][j] = std::shared_ptr<tracer<void,void,0,true>> (new tracer<void,void,0,true> ("a" + s.str()));
            m2[i][j] = std::shared_ptr<tracer<void,void,0,true>> (new tracer<void,void,0,true> ("b" + s.str()));
        }
    }

    matrix<runtime,3,3> m3 = m1 * m2;

    for (unsigned int i = 0; i < 3; i++)
    {
        for (unsigned int j = 0; j < 3; j++)
        {
            log << "result" << "_" << i << "_" << j << " = "
                << m3[i][j] << "\n";
        }
    }

    return 0;
}

TEST_BATCH(testTracer, testTracerMatrix)
