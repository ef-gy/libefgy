/**\file
 * \deprecated Will be removed in libefgy-5 (or rewritten properly, whichever
 *             happens first).
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

#if !defined(EF_GY_SET_H)
#define EF_GY_SET_H

#include <vector>

namespace efgy
{
    namespace math
    {
	/** \brief Sets with elements of a given type.	
	*  
	*  \tparam T The type of the set elements.
	*/
	template <typename T>
        class set
        {
            public:
                /** \brief Constructs the empty set.
		* 
		*/
		set()
                    : data()
                    {}
		
		/** \brief Constructs a set from an array of elements of type T */
                set(const T *pData, unsigned int pCount)
                    : data(pCount)
                    {
                        for (unsigned int i = 0; i < pCount; i++)
                        {
                            data[i] = pData[i];
                        }
                    }
		/** \brief Copy constructor */
                set(const set &pT)
                    : data(pT.data)
                    {}

		/**  */
                template<typename U>
                set(const set<U> &pT)
                    : data(pT.data)
                    {}

		/** \brief Assignment operator
		 *  
		 *  Performs a comparison of the current object and the set to be assigned
		 *  before assignment, to avoid unnecessary copying of data if the sets are
		 *  already equal.
		 */
                set &operator = (const set &b)
                {
                    if (this != &b)
                    {
                        data  = b.data;
                    }

                    return *this;
                }

		/** \brief Set union */
                set operator + (const T &b) const
                {
                    if (data.size() > 0)
                    {
                        unsigned int i = 0;

                        for (; i < data.size(); i++)
                        {
                            if (data[i] == b)
                            {
                                return *this;
                            }
                        }

                        set rv = *this;

                        rv.data.resize (data.size() + 1);

                        rv.data[data.size()] = b;

                        return rv;
                    }

                    return set(&b, 1);
                }

		/** \brief Set difference */
                set operator - (const T &b) const
                {
                    if (data.size() > 0)
                    {
                        unsigned int i = 0, c = 0;

                        for (; i < data.size(); i++)
                        {
                            if (data[i] == b)
                            {
                                c++;
                            }
                        }

                        if (c > 0)
                        {
                            set rv = set();

                            rv.data.resize(data.size() - c);

                            if (rv.data.size() == 0)
                            {
                                return rv;
                            }

                            for (c = 0, i = 0; i < data.size(); i++)
                            {
                                if (data[i] != b)
                                {
                                    rv.data[c] = data[i];
                                    c++;
                                }
                            }

                            return rv;
                        }
                    }

                    return set(*this);
                }

		/** \brief Set comparison */
                bool operator == (const set &b) const
                {
                    if (data.size() != b.data.size())
                    {
                        return false;
                    }

                    for (unsigned int i = 0; i < data.size(); i++)
                    {
                        if (data[i] != b.data[i])
                        {
                            return false;
                        }
                    }

                    return true;
                }

                std::vector<T> data;
        };
    };
};

#endif
