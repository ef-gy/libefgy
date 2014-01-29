/**\file
 * \brief S-expression parser 
 *
 * Defines a template to handle S-expression parsing.
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

#if !defined(EF_GY_SEXP_H)
#define EF_GY_SEXP_H

#include "maybe.h"

namespace efgy
{
    /* \brief Abstract base class for s-expressions
    *
    *  Abstract base class for s-expressions
    *  In this implementation, you should instantiate
    *  the concrete derived  classes, efgy::cons and efgy::atom.
    */
    class sexp
    {
        public:
            virtual std::ostream& operator<<(std::ostream& str) = 0;
            virtual std::istream& operator>>(std::istream& str) = 0;
        
        private:

    };
    
    /** \brief A cons expression
    *
    * Represents a cons expression which consists of two
    * other S-expressions. A cons can either combine two
    * nontrivial S-expressions, or combine one S-expression
    * with the special S-expression NIL, which is represented
    * through the 'nothing' value of the 'maybe' class here.
	*
	* \tparam T1 type of first element
	* \tparam T2 type of second element
    *
    * \sa \ref maybe
    */
	
	template<typename T1, typename T2>
	class cons : public sexp
	{
	    public:
	        /*\brief Constructor for a cons with two elements
            
            Constructs a cons of the form (x.y) where x and y 
	        are both s-expressions not equal to NIL*/
	        cons(T1 car_, T2 cdr_) : car(car_), cdr(efgy::maybe<T2>(cdr_))
	        {}
	
	        /*\brief Constructor for a cons with one element
            
                Constructs a cons of the form (x.nil)*/
	        cons(T1 car_) : car(car_), cdr(efgy::maybe<T2>())
	        {}
	       
	        /* \brief Stream insertion operator
	        *
	        * Displays the cons expression in a std::ostream.
	        * The expression is displayed as (car . cdr).
	        */
	        std::ostream& operator<<(std::ostream& str)
            	{
        	    str << "(" << car << " . " << cdr << ")";
                    return str;
                }
	
	    private:
	        /* \brief First element of the cons
             *
             * First element of the cons
             */
            T1 car;

            /* \brief Second element of the cons
             *
             * Second element of the cons, represented as a maybe:
             * it can either contain a value of type T2,
             * or 'nothing' to represent the special S-expression
             * NIL (which is used to indicate the end of a list,
             * among other things)
             */
	        efgy::maybe<T2> cdr;
	};
	
    /** \brief An atomic S-expression
    *
    * Represents an atomic S-expression, which is a value
    * of some type T. T ought to be a truly atomic type,
    * not a cons type.
    *
    * \tparam T type of atomic expression
    */
	template<typename T>
	class atom : public sexp
	{
	    public:
	        /* \brief Constructs an atomic S-expression.
            *
            * Constructs an atomic S-expression.
            */
            atom(T data_) : data(data_)
	        {}
	        
	    /* \brief Stream insertion operator
	    * 
	    * Displays the atomic value, as defined in the
	    * stream insertion operator of type T.*/
	    std::ostream& operator<<(std::ostream& str)
            {
                str << data;
                return str;
            }

	
	    private:
            /* \brief The atomic value
             *
             * The atomic value of the S-expression.
             */
	        T data;
	};
	
	class parser
	{
	    public:
            // does nothing for now
            friend std::istream& operator>> (std::istream& is, parser const& p)
            {
                return is;   
            }

	    private:
            
            int openParenCount;
            int closeParenCount;
            	        
	};
}
#endif

