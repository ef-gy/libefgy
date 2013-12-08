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
#include <functional>

namespace efgy
{
    namespace terminal
    {
        template<typename T = long>
        class vt100 : public terminal<T>
        {
            public:
                vt100
                    (std::istream &pInput  = std::cin,
                     std::ostream &pOutput = std::cout)
                    : terminal<T>(pInput, pOutput),
                      currentLine(-1), currentColumn(-1),
                      currentForegroundColour(-1),
                      currentBackgroundColour(-1)
                    {}

                using terminal<T>::current;
                using terminal<T>::target;
                using terminal<T>::input;
                using terminal<T>::output;
                using terminal<T>::queue;
                using terminal<T>::resize;
                using terminal<T>::size;
                using terminal<T>::read;

                std::size_t currentLine;
                std::size_t currentColumn;
                std::size_t currentForegroundColour;
                std::size_t currentBackgroundColour;

                std::string flush (std::function<cell<T>(const terminal<T>&,const std::size_t&,const std::size_t&)> postProcess = 0, std::size_t targetOps = 1024)
                {
                    std::stringstream rv("");
                    int ops = 0;

                    for (unsigned int l = 0; l < target.screen<T>::parent::size(); l++)
                    {
                        for (unsigned int c = 0; c < target[l].size(); c++)
                        {
                            const cell<T> &ccell = current[l][c];
                            const cell<T>  tcell = postProcess ? postProcess(*this, l, c) : target[l][c];

                            if (ccell != tcell)
                            {
                                if ((c == 0) && (currentLine == (l-1)))
                                {
                                    rv << "\n";
                                    ops++;
                                }
                                else if ((currentLine != l) && (currentColumn != c))
                                {
                                    std::size_t vtl = l+1;
                                    std::size_t vtc = c+1;
                                    if (vtl == 1)
                                    {
                                        if (vtc == 1)
                                        {
                                            rv << "\e[H";
                                            ops++;
                                        }
                                        else
                                        {
                                            rv << "\e[" << ";" << vtc << "H";
                                            ops++;
                                        }
                                    }
                                    else if (vtc == 1)
                                    {
                                        rv << "\e[" << vtl << "H";
                                        ops++;
                                    }
                                    else
                                    {
                                        rv << "\e[" << vtl << ";" << vtc << "H";
                                        ops++;
                                    }
                                }
                                else if (currentLine != l)
                                {
                                    signed long sp = (signed long)currentLine - (signed long)l;
                                    if (sp > 0)
                                    {
                                        rv << "\e[" << sp << "A";
                                        ops++;
                                    }
                                    else
                                    {
                                        rv << "\e[" << -sp << "B";
                                        ops++;
                                    }
                                }
                                else if (currentColumn != c)
                                {
                                    signed long sp = (signed long)currentColumn - (signed long)c;
                                    if (sp > 0)
                                    {
                                        rv << "\e[" << sp << "D";
                                        ops++;
                                    }
                                    else
                                    {
                                        rv << "\e[" << -sp << "C";
                                        ops++;
                                    }
                                }

                                if (tcell.foregroundColour != currentForegroundColour)
                                {
                                    if (tcell.foregroundColour < 8)
                                    {
                                        rv << "\e[3" << tcell.foregroundColour << "m";
                                        ops++;
                                    }
                                    else
                                    {
                                        rv << "\e[38;5;" << tcell.foregroundColour << "m";
                                        ops++;
                                    }
                                }
                                if (tcell.backgroundColour != currentBackgroundColour)
                                {
                                    if (tcell.backgroundColour < 8)
                                    {
                                        rv << "\e[4" << tcell.backgroundColour << "m";
                                        ops++;
                                    }
                                    else
                                    {
                                        rv << "\e[48;5;" << tcell.backgroundColour << "m";
                                        ops++;
                                    }
                                }

                                if ((tcell.content < 0x20) || (tcell.content == 0x7f))
                                {
                                    rv << ".";
                                    ops++;
                                    /* don't print control characters */
                                }
                                else if (tcell.content < 0x80)
                                {
                                    rv << char(tcell.content);
                                    ops++;
                                }
                                else if (tcell.content < 0x800)
                                {
                                    rv << char(((tcell.content >>  6) & 0x1f) | 0xc0)
                                       << char(( tcell.content        & 0x3f) | 0x80);
                                    ops++;
                                }
                                else if (tcell.content < 0x10000)
                                {
                                    rv << char(((tcell.content >> 12) & 0x0f) | 0xe0)
                                       << char(((tcell.content >>  6) & 0x3f) | 0x80)
                                       << char(( tcell.content        & 0x3f) | 0x80);
                                    ops++;
                                }
                                else if (tcell.content < 0x200000)
                                {
                                    rv << char(((tcell.content >> 18) & 0x07) | 0xf0)
                                       << char(((tcell.content >> 12) & 0x3f) | 0x80)
                                       << char(((tcell.content >>  6) & 0x3f) | 0x80)
                                       << char(( tcell.content        & 0x3f) | 0x80);
                                    ops++;
                                }
                                else if (tcell.content < 0x4000000)
                                {
                                    rv << char(((tcell.content >> 24) & 0x03) | 0xf8)
                                       << char(((tcell.content >> 18) & 0x3f) | 0x80)
                                       << char(((tcell.content >> 12) & 0x3f) | 0x80)
                                       << char(((tcell.content >>  6) & 0x3f) | 0x80)
                                       << char(( tcell.content        & 0x3f) | 0x80);
                                    ops++;
                                }
                                else /* if (tcell.content < 0x80000000) */
                                {
                                    rv << char(((tcell.content >> 30) & 0x01) | 0xfc)
                                       << char(((tcell.content >> 24) & 0x3f) | 0x80)
                                       << char(((tcell.content >> 18) & 0x3f) | 0x80)
                                       << char(((tcell.content >> 12) & 0x3f) | 0x80)
                                       << char(((tcell.content >>  6) & 0x3f) | 0x80)
                                       << char(( tcell.content        & 0x3f) | 0x80);
                                    ops++;
                                }

                                current[l][c] = tcell;
                                currentLine = l;
                                currentColumn = c+1;
                                currentForegroundColour = tcell.foregroundColour;
                                currentBackgroundColour = tcell.backgroundColour;
                                if (ops >= targetOps)
                                {
                                    return rv.str();
                                }
                            }
                        }
                    }

                    return rv.str();
                }

                enum parserState
                {
                    text,
                    escape1,
                    escape2
                };

                bool updatePosition (void)
                {
                    output << "\e[6n";

                    enum parserState state = text;
                    std::vector<T> result;

                    bool haveCommand = false;
                    std::string vtparam;
                    std::vector<std::string> vtparams;
                    T vtcommand;

                    do
                    {
                        read();
                        vtparam = "";
                        vtparams.resize(0);
                        result.resize(0);

                        for (T v : queue)
                        {
                            switch (state)
                            {
                                case text:
                                    switch (v)
                                    {
                                        case '\e':
                                            if (!haveCommand)
                                            {
                                                state = escape1;
                                                break;
                                            }
                                        default:
                                            result.push_back(v);
                                    }
                                    break;
                                case escape1:
                                    switch (v)
                                    {
                                        case '[':
                                            state = escape2;
                                            break;
                                        default:
                                            state = text;
                                            result.push_back(T('\e'));
                                            result.push_back(v);
                                    }
                                    break;
                                case escape2:
                                    switch (v)
                                    {
                                        case '0': vtparam += "0"; break;
                                        case '1': vtparam += "1"; break;
                                        case '2': vtparam += "2"; break;
                                        case '3': vtparam += "3"; break;
                                        case '4': vtparam += "4"; break;
                                        case '5': vtparam += "5"; break;
                                        case '6': vtparam += "6"; break;
                                        case '7': vtparam += "7"; break;
                                        case '8': vtparam += "8"; break;
                                        case '9': vtparam += "9"; break;
                                        case ';':
                                            vtparams.push_back(vtparam);
                                            vtparam = "";
                                            break;
                                        default:
                                            vtparams.push_back(vtparam);
                                            vtcommand = v;
                                            state = text;
                                            haveCommand = true;
                                            break;
                                    }
                                    break;
                            }
                        }

                        queue = result;
                        if (vtcommand == 'R')
                        {
                            std::stringstream st;
                            switch (vtparams.size())
                            {
                                case 0:
                                    currentLine = 0;
                                    currentColumn = 0;
                                    break;
                                case 1:
                                    st.clear();
                                    st.str(vtparams[0]);
                                    st >> currentLine;
                                    currentLine--;
                                    currentColumn = 0;
                                    break;
                                default:
                                    st.clear();
                                    st.str(vtparams[0]);
                                    st >> currentLine;
                                    st.clear();
                                    st.str(vtparams[1]);
                                    st >> currentColumn;
                                    currentLine--;
                                    currentColumn--;
                                    break;
                            }
                        }
                    }
                    while (!haveCommand);

                    return true;
                }
        };
    };
};

#endif
