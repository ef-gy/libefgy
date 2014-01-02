/**\file
 * \brief Test cases for the set templates
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
#include <ef.gy/set.h>

using namespace efgy;
using namespace efgy::math;
using efgy::test::next_integer;

int testSet(std::ostream &log)
{
    set<unsigned int> empty();

    set<unsigned int> empty2();
    
    if(empty != empty2)
    {
	log << "two empty sets cannot be inequal to each other ";
	return next_integer();
    }
    
    const int a[4] = {0, 1, 2, 3};

    set<int> s_a(&a[0], 4);

    int b[3] = {0, 1, 2};
    set<int> s_b(&b[0], 3);

    if(s_a == s_b)
    {
	log << "sets of two different sizes cannot be equal to each other";
    	return next_integer();
    }

    const int a_permuted[4] = {3, 2, 1, 0};
    set<int> s_a_permuted(&a_permuted[0], 3);

    if(!(s_a == s_a_permuted))
    {
        log << "sets with the same elements in different order should be equal";
        return next_integer();
    }
    return 0; 
}

TEST_BATCH(testSet)
