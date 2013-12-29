/**\file
 * \brief Test cases for matrices
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
#include <ef.gy/matrix.h>

using namespace efgy::math;
using efgy::test::next_integer;

int testConstruction (std::ostream &log)
{
     matrix<int, 3, 4> m;
     for(int i = 0; i < 3; i++)
     {
	for(int k = 0; k < 4; k++)
        {
		m[i][k] = i * k;
	}
     }

     matrix<int, 3, 4> m2(m);
     
     bool allCorrect = true;
     for(int i = 0; i < 3; i++)
     {
	for(int k = 0; k < 4; k++)
        {
	    allCorrect &= (m2[i][k] == (i * k));
	}
     }

     if(!allCorrect) 
     {
	log << "Actual and expected value in result of copy constructor are different";
	return next_integer();
     }
     return 0;
}

TEST_BATCH(testConstruction)
