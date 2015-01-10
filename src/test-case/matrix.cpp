/**\file
 * \brief Test cases for matrices
 *
 * \copyright
 * Copyright (c) 2012-2015, ef.gy Project Members
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
#include <sstream>

#include <ef.gy/test-case.h>
#include <ef.gy/matrix.h>

using namespace efgy::math;
using efgy::test::next_integer;

/* \brief Tests matrix construction.
*  
* \test Constructs a 3x4 matrix from scratch,
* then copies it to another matrix object.
*
* \param [out] log A stream to copy log messages to.
* 
* \return Zero when everything went as expected, nonzero otherwise.
*/

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

/* \brief Test matrix assignment.
* 
* \test Constructs a 3x4 matrix and assigns it
* to another matrix object, then checks if values
* were copied correctly.
*
* \param [out] log A stream to copy log messages to.
* 
* \return Zero when everything went as expected, nonzero otherwise.
*/
int testAssignment (std::ostream &log)
{
     matrix<int, 3, 4> m;
     for(int i = 0; i < 3; i++)
     {
    	for(int k = 0; k < 4; k++)
        {
	    	m[i][k] = i * k;
	    }
     }

     matrix<int, 3, 4> m2 = m;

     bool allCorrect = true;
     for(int i = 0; i < 3; i++)
     {
         for(int k = 0; k < 3; k++)
         {
            allCorrect &= (m2[i][k] == m[i][k]);
         }
     }

     if(!allCorrect)
     {
         log << "Values were not copied correctly during assignment.";
         return next_integer();
     }

     return 0;
}


/* \brief Test matrix addition
* 
* \test Constructs a matrix from scratch;
* checks whether adding the zero matrix from the left and the right
* yields the expected result,
* then adds two nontrivial matrices and checks the result.
*
* \param [out] log A stream to copy log messages to.
* 
* \return Zero when everything went as expected, nonzero otherwise.
*/
int testAddition (std::ostream &log)
{
     matrix<int, 3, 4> m;
     for(int i = 0; i < 3; i++)
     {
    	for(int k = 0; k < 4; k++)
        {
	    	m[i][k] = i * k;
	    }
     }

     matrix<int, 3, 4> n;

     for(int i = 0; i < 3; i++)
     {
    	for(int k = 0; k < 4; k++)
        {
	    	n[i][k] = 0; 
	    }
     }

     // results of left and right addition of
     // neutral element to m 
     matrix<int, 3, 4> leftNeutral = n + m;
     matrix<int, 3, 4> rightNeutral = m + n;

     // test addition of neutral element;
     bool allCorrectLeft = true;
     bool allCorrectRight = true; 
     for(int i = 0; i < 3; i++)
     {
         for(int k = 0; k < 4; k++)
         {
            allCorrectLeft &= (leftNeutral[i][k] == (i * k));
            allCorrectRight &= (rightNeutral[i][k] == (i * k));
         }
     }
     if(!allCorrectLeft) 
     {
        log << "n + m, where n is the neutral element: ";
        for(int i = 0; i < 3; i++)
        {
            for(int k = 0; k < 4; k++)
            {
                log << leftNeutral[i][k];
            }
            log << "\n";
        }
        log << "\n";
        log << "m (original matrix):";
        for(int i = 0; i < 3; i++)
        {
            for(int k = 0; k < 4; k++)
            {
                log << m[i][k];
            }
            log << "\n";
        }

        log << "Unexpected result of left addition of neutral element to matrix.";
        return next_integer();
     }
     if(!allCorrectRight)
     {
        log << "m + n, where n is the neutral element: ";
        for(int i = 0; i < 3; i++)
        {
            for(int k = 0; k < 4; k++)
            {
                log << rightNeutral[i][k];
            }
            log << "\n";
        }
        
        log << "\n";
        log << "m (original matrix):";
        for(int i = 0; i < 3; i++)
        {
            for(int k = 0; k < 4; k++)
            {
                log << m[i][k];
            }
            log << "\n";
        }
        log << "Unexpected result of right addition of neutral element to matrix.";
        return next_integer();    
     }

     // another test for addition of two non-neutral elements
     matrix<int, 3, 4> m2;

     for(int i = 0; i < 3; i++)
     {
         for(int k = 0; k < 4; k++)
         {
             m2[i][k] = i + k;
         }
     }

     matrix<int, 3, 4> sum = m + m2;
     
     bool allCorrectSum = true;
     for(int i = 0; i < 3; i++)
     {
         for(int k = 0; k < 3; k++)
         {
             allCorrectSum &= (sum[i][k] == (i * k) + (i + k));
         }
     }

     if(!allCorrectSum)
     {
         log << "Unexpected result adding two matrices.";
         return next_integer();
     }

     return 0;
}

/* \brief Tests stream output.
*
* \test Constructs a matrix and inserts it into a std::ostringstream
*
* \param [out] log A stream to copy log messages to.
* 
* \return Zero when everything went as expected, nonzero otherwise.
*/
int testStream(std::ostream & log)
{
    std::ostringstream stream;

    matrix<int, 3, 4> m;
    for(int i = 0; i < 3; i++)
    {
       for (int k = 0; k < 4; k++)
       {
          m[i][k] = i*k;
       }
    }

    stream << m;

    std::stringstream expected;

    expected << "0\t0\t0\t0\t\n";
    expected << "0\t1\t2\t3\t\n";
    expected << "0\t2\t4\t6\t\n";

    if(stream.str() != expected.str())
    {
        log << "Unexpected characters in printable representation of matrix.\n";
        log << "Matrix: \n";
        log << m;
        log << "\nExpected:\n";
        log << expected.str();

        return next_integer();
    }
    return 0;
}
TEST_BATCH(testConstruction, testAssignment, testAddition, testStream)
