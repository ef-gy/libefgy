/**\file
 * \brief Test cases the geometry factory
 *
 * The test cases in this file test the functionality of the geometry factory.
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
 * \see Project Source Code: http://git.becquerel.org/jyujin/libefgy.git
 */

#include <iostream>

#include <ef.gy/test-case.h>
#include <ef.gy/factory.h>

using namespace efgy::geometry;
using namespace efgy::math;

/**\brief Geometry factory tests
 * \test Uses some functions of the geometry factory to verify that it compiles
 *       and works as intended.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testGeometryFactory (std::ostream &log)
{
    std::cerr << "\n";
    model<float,functor::echo,cube,7,7,format::cartesian>::with(log, 5);

    std::set<const char *> models;
    models = with<float,functor::models,7>(models, "*", 0, 0);

    bool haveCube = false;

    for (std::string m : models)
    {
        haveCube || (haveCube = (m == "cube"));
        std::cerr << m << "\n";
    }

    if (!haveCube)
    {
        log << "no cube model\n";
        return 1;
    }

    return 0;
}

TEST_BATCH(testGeometryFactory)
