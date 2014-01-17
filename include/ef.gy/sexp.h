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
#include <iostream>

namespace efgy
{
	
    class sexp
    {
        public:
            virtual std::ostream& operator<<(std::ostream& str) = 0;
            virtual std::istream& operator>>(std::istream& str) = 0;
        
        private:

    };
    
    /** \brief A cons expression
	*
	* \tparam T1 type of first element
	* \tparam T2 type of second element
	*/
	template<typename T1, typename T2>
	class cons : sexp
	{
	    public:
	        /*\brief Constructor for a cons of the form (x.y) where x and y 
	        are both s-expressions not equal to NIL*/
	        cons(T1 car_, T2 cdr_) : car(car_), cdr(cdr_)
	        {}
	
	        /*\brief Constructor for a cons of the form (x.nil)*/
	        cons(T1 car_) : car(car_), cdr(efgy::maybe<T2>())
	        {}

            std::ostream& operator<<(std::ostream& str)
            {
                str << "(" << car << " . " << cdr << ")";
                return str;
            }
	
	    private:
	        T1 car;
	        efgy::maybe<T2> cdr;
	};
	
	template<typename T>
	class atom : sexp
	{
	    public:
	        atom(T data_) : data(data_)
	        {}

            std::ostream& operator<<(std::ostream& str)
            {
                str << data;
                return str;
            }
	
	    private:
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

