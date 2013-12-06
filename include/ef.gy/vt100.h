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

#if !defined(EF_GY_VT100_H)
#define EF_GY_VT100_H

#include <ef.gy/terminal.h>
#include <sstream>

namespace efgy
{
    namespace terminal
    {
        template<typename T = long>
        class vt100 : public terminal<T>
        {
            public:
                vt100(void)
                    : terminal<T>(), currentLine(-1), currentColumn(-1) {}

                using terminal<T>::current;
                using terminal<T>::target;
                using terminal<T>::resize;
                using terminal<T>::size;

                std::size_t currentLine;
                std::size_t currentColumn;

                std::string transform (std::size_t maxLength = 0)
                {
                    std::stringstream rv;
                    std::array<std::size_t,2> s = size();

                    for (unsigned int l = 0; l < s[1]; l++)
                    {
                        for (unsigned int c = 0; c < s[0]; c++)
                        {
                            if (current[l][c] != target[l][c])
                            {
                                if ((currentLine != l) && (currentColumn != c))
                                {
                                    std::size_t vtl = l+1;
                                    std::size_t vtc = c+1;
                                    if (vtl == 1)
                                    {
                                        if (vtc == 1)
                                        {
                                            rv << "\e[H";
                                        }
                                        else
                                        {
                                            rv << "\e[" << ";" << vtc << "H";
                                        }
                                    }
                                    else if (vtc == 1)
                                    {
                                        rv << "\e[" << vtl << "H";
                                    }
                                    else
                                    {
                                        rv << "\e[" << vtl << ";" << vtc << "H";
                                    }
                                    currentLine = l;
                                    currentColumn = c;
                                }
                                else if (currentLine != l)
                                {
                                    signed long sp = (signed long)currentLine - (signed long)l;
                                    if (sp > 0)
                                    {
                                        rv << "\e[" << sp << "A";
                                    }
                                    else
                                    {
                                        rv << "\e[" << -sp << "B";
                                    }
                                    currentLine = l;
                                }
                                else if (currentColumn != c)
                                {
                                    signed long sp = (signed long)currentColumn - (signed long)c;
                                    if (sp > 0)
                                    {
                                        rv << "\e[" << sp << "D";
                                    }
                                    else
                                    {
                                        rv << "\e[" << -sp << "C";
                                    }
                                    currentColumn = c;
                                }

                                if ((target[l][c].content < 0x20) || (target[l][c].content == 0x7f))
                                {
                                    rv << ".";
                                    /* don't print control characters */
                                }
                                else if (target[l][c].content < 0x80)
                                {
                                    rv << char(target[l][c].content);
                                }
                                else if (target[l][c].content < 0x800)
                                {
                                    rv << char(((target[l][c].content >>  6) & 0x1f) | 0xc0)
                                       << char(( target[l][c].content        & 0x3f) | 0x80);
                                }
                                else if (target[l][c].content < 0x10000)
                                {
                                    rv << char(((target[l][c].content >> 12) & 0x0f) | 0xe0)
                                       << char(((target[l][c].content >>  6) & 0x3f) | 0x80)
                                       << char(( target[l][c].content        & 0x3f) | 0x80);
                                }
                                else if (target[l][c].content < 0x200000)
                                {
                                    rv << char(((target[l][c].content >> 18) & 0x07) | 0xf0)
                                       << char(((target[l][c].content >> 12) & 0x3f) | 0x80)
                                       << char(((target[l][c].content >>  6) & 0x3f) | 0x80)
                                       << char(( target[l][c].content        & 0x3f) | 0x80);
                                }
                                else if (target[l][c].content < 0x4000000)
                                {
                                    rv << char(((target[l][c].content >> 24) & 0x03) | 0xf8)
                                       << char(((target[l][c].content >> 18) & 0x3f) | 0x80)
                                       << char(((target[l][c].content >> 12) & 0x3f) | 0x80)
                                       << char(((target[l][c].content >>  6) & 0x3f) | 0x80)
                                       << char(( target[l][c].content        & 0x3f) | 0x80);
                                }
                                else /* if (target[l][c].content < 0x80000000) */
                                {
                                    rv << char(((target[l][c].content >> 30) & 0x01) | 0xfc)
                                       << char(((target[l][c].content >> 24) & 0x3f) | 0x80)
                                       << char(((target[l][c].content >> 18) & 0x3f) | 0x80)
                                       << char(((target[l][c].content >> 12) & 0x3f) | 0x80)
                                       << char(((target[l][c].content >>  6) & 0x3f) | 0x80)
                                       << char(( target[l][c].content        & 0x3f) | 0x80);
                                }

                                currentColumn++;
                                /*
                                if (currentColumn >= s[0])
                                {
                                    currentColumn = 0;
                                    currentLine++;
                                }
                                */
                            }
                        }
                    }

                    return rv.str();
                }
        };
    };
};

#endif
