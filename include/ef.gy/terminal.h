/**\file
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

#if !defined(EF_GY_TERMINAL_H)
#define EF_GY_TERMINAL_H

#include <ef.gy/render.h>
#include <vector>

namespace efgy
{
    namespace terminal
    {
        template<typename T = long>
        class cell
        {
            public:
                T content;
                int foregroundColour;
                int backgroundColour;
                int attributes;
        };

        template<typename T = long>
        class screen: public std::vector<std::vector<cell<T> > >
        {
            public:
                screen(int width = 80, int height = 25)
                    {
                        resize(width, height);
                    }

                typedef std::vector<std::vector<cell<T> > > parent;

                void resize(unsigned int columns, unsigned int lines)
                {
                    parent(*this).resize(lines);

                    for (std::vector<cell<T> > &column : parent(*this))
                    {
                        column.resize(columns);
                    }
                }
        };

        template<typename T = long>
        class terminal
        {
            public:
                screen<T> current;
                screen<T> target;

                void resize(unsigned int columns, unsigned int lines)
                {
                    current.resize (columns, lines);
                    target.resize  (columns, lines);
                }
        };
    };
};

#endif
