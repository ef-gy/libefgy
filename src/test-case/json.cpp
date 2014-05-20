/**\file
 * \brief Test cases for the JSON render output
 *
 * The test cases in this file test the functionality of JSON renderer.
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

#include <ef.gy/test-case.h>
#include <ef.gy/render-json.h>
#include <sstream>

using namespace efgy::render;
using namespace efgy;

/**\brief JSON output tests
 * \test Writes some JSON to a stringstream to see if it compiles and works as
 *       intended.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testJSONoutput (std::ostream &log)
{
    std::map<const char*,int> m;

    m["a"] = 42;
    m["b"] = 23;
    m["c"] = 1;

    std::map<const char*,std::map<const char*,int>> mx;

    mx["d"] = m;

    std::ostringstream s("");

    s   << json::tag()
        << false
        << 1
        << "foo"
        << 2
        << "bar \"baz\""
        << 42.23
        << std::array<double,2>({23,42})
        << m
        << mx
        << true;

    if (s.str() != "false1\"foo\"2\"bar \\\"baz\\\"\"42.23[23,42]"
                   "{\"a\":42,\"b\":23,\"c\":1}"
                   "{\"d\":{\"a\":42,\"b\":23,\"c\":1}}true")
    {
        log << "unexpected JSON output: " << s.str();
        return 1;
    }

    return 0;
}

TEST_BATCH(testJSONoutput)