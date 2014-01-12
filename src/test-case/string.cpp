/**\file
 * \brief Test cases for the 'string.h' templates
 *
 * Test cases in this file test the functions in libefgy's string header.
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
#include <sstream>

#include <ef.gy/test-case.h>
#include <ef.gy/string.h>

using std::string;
using namespace efgy;
using std::ostringstream;

int testString(std::ostream &log) {

    {
      ostringstream oss;
      print(oss, "test");
      string output_string = oss.str();

      if (output_string != "test")
      {
          log << "No placeholders and no variables.";
          return 0;
      }
    }

    {
      ostringstream oss;
      print(oss, "test %");
      string output_string = oss.str();

      if (output_string != "test %")
      {
          log << "One placeholders and no variables.";
          return 1;
      }
    }

    {
      ostringstream oss;
      print(oss, "test %", 42);
      string output_string = oss.str();

      if (output_string != "test 42")
      {
          log << "One placeholders and one variable.";
          return 2;
      }
    }

    {
      ostringstream oss;
      print(oss, "test % %", 42);
      string output_string = oss.str();

      if (output_string != "test 42 %")
      {
          log << "Two placeholders and one variable.";
          return 3;
      }
    }

    {
      ostringstream oss;
      print(oss, "% test %", 1, 2);
      string output_string = oss.str();

      if (output_string != "1 test 2")
      {
          log << "Two placeholders and two variables.";
          return 4;
      }
    }

    {
      ostringstream oss;
      print(oss, "test % % %", 1, 2, 3);
      string output_string = oss.str();

      if (output_string != "test 1 2 3")
      {
          log << "3 placeholders at the end did not get replaced.";
          return 5;
      }
    }

    return 0;
}

TEST_BATCH(testString)
