/**\file
 * \brief Test cases for s-expression parsing 
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
 * \see Project Source Code: https://github.com/ef-gy/libefgy.git
 */

#include <iostream>
#include <sstream>

#include <ef.gy/test-case.h>
#include <ef.gy/sexp.h>

using namespace efgy::test;
using efgy::atom;
using efgy::cons;

/* \brief Tests construction of s-expressions
 *
 * \test Constructs an atomic s-expression and a cons s-expression
 *
 * \param [out] log Stream for output messages
 *
 * \returns Zero if everything was okay, a nonzero integer otherwise
 */
int testConstruction(std::ostream &log)
{
   atom<int> a(23);

   if(a.data != 23)
   {
       log << "Expected data value: 23. Actual value: " << a.data;
       return next_integer();
   }
   
   cons<int, int> c(23, 42);
   if(c.car != 23) 
   {
       log << "Expected value of car(cons(23, 42)): 23. Actual value: " << c.car;
       return next_integer();
   }
   if(c.cdr.just != 42) 
   {
       log << "Expected value of cdr(cons(23, 42)): 42. Actual value: " << c.cdr;
       return next_integer();
   }

   return 0;
}

/* \brief Tests stream output of s-expressions
 *
 * \test Constructs atomic and cons s-expressions and inserts them
 * into a std::ostringstream
 *
 * \param [out] log Stream for output messages
 *
 * \returns Zero if everything was okay, a nonzero integer otherwise
 */
int testStreamOutput(std::ostream &log)
{
    atom<int> a(23);
    cons<int, int> c(23, 42);
    
    std::ostringstream test;
    test << a;
    test << "\n" << c;
   
    std::string expected = "23\n(23 . 42)"; 
    if(test.str() != expected)
    {
        log << "Detected error in stream output. Expected: " << expected;
        log << "\nActual: " << test.str();
        return next_integer();
    }
    else
    {
        return 0;
    }
}

TEST_BATCH(testConstruction, testStreamOutput)
