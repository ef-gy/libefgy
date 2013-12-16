/**\file
 * \brief VT100 terminal frontend implementation
 *
 * This file contains code to drive a vt100-like terminal using the data in a
 * terminal::terminal instance.
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
        /**\brief VT100 terminal frontend
         *
         * A frontend for the terminal::terminal template, this class
         * implements a very simple optimiser to output the screen buffer to a
         * VT100, or any of the many newer terminals that support the basic
         * escape codes the VT100 supported - which is pretty much all of them.
         *
         * \tparam T Data type for an individual cell's contents. The default
         *           is a "long", which should be enough to cover all possible
         *           unicode codepoints, ever. Hopefully, anyway.
         */
        template<typename T = long>
        class vt100 : public terminal<T>
        {
            public:
                /**\brief Construct with streams
                 *
                 * Calls the parent constructor, which will set up the streams
                 * connecting the terminal; this constructor will also set the
                 * current foreground and background text colours to invalid
                 * values, so they're reset the first time they're used.
                 *
                 * \param[out] pInput  Input stream for the terminal, e.g.
                 *                     std::cin
                 * \param[out] pOutput Output stream for the terminal, e.g.
                 *                     std::cout
                 * \param[in]  doSetup Whether to disable echo and canonical
                 *                     mode on the terminal connected to stdin
                 */
                vt100
                    (std::istream &pInput  = std::cin,
                     std::ostream &pOutput = std::cout,
                     const bool &doSetup = true)
                    : terminal<T>(pInput, pOutput, doSetup),
                      currentForegroundColour(-1),
                      currentBackgroundColour(-1),
                      didSetup(doSetup)
                    {}

                /*\brief Destructor
                 *
                 * Resets the terminal's colour and text attributes and starts
                 * a new line, but only if the "doSetup" was true when the
                 * constructor ran.
                 */
                ~vt100(void)
                    {
                        if (didSetup)
                        {
                            std::cout << "\e[39;49;0m\n";
                        }
                    }

                using terminal<T>::current;
                using terminal<T>::target;
                using terminal<T>::input;
                using terminal<T>::output;
                using terminal<T>::queue;
                using terminal<T>::resize;
                using terminal<T>::size;
                using terminal<T>::read;
                using terminal<T>::cursor;

                std::size_t flush (std::function<cell<T>(const terminal<T>&,const std::size_t&,const std::size_t&)> postProcess = 0, std::size_t targetOps = 1024)
                {
                    std::size_t ops = 0;

                    for (unsigned int l = 0; l < target.screen<T>::parent::size(); l++)
                    {
                        for (unsigned int c = 0; c < target[l].size(); c++)
                        {
                            const cell<T> &ccell = current[l][c];
                            const cell<T>  tcell = postProcess ? postProcess(*this, l, c) : target[l][c];

                            if (ccell != tcell)
                            {
                                if ((c == 0) && (cursor[1] == (l-1)))
                                {
                                    output << "\n";
                                    ops++;
                                }
                                else if ((cursor[1] != l) && (cursor[0] != c))
                                {
                                    std::size_t vtl = l+1;
                                    std::size_t vtc = c+1;
                                    if (vtl == 1)
                                    {
                                        if (vtc == 1)
                                        {
                                            output << "\e[H";
                                            ops++;
                                        }
                                        else
                                        {
                                            output << "\e[" << ";" << vtc << "H";
                                            ops++;
                                        }
                                    }
                                    else if (vtc == 1)
                                    {
                                        output << "\e[" << vtl << "H";
                                        ops++;
                                    }
                                    else
                                    {
                                        output << "\e[" << vtl << ";" << vtc << "H";
                                        ops++;
                                    }
                                }
                                else if (cursor[1] != l)
                                {
                                    signed long sp = (signed long)cursor[1] - (signed long)l;
                                    if (sp > 0)
                                    {
                                        output << "\e[" << sp << "A";
                                        ops++;
                                    }
                                    else
                                    {
                                        output << "\e[" << -sp << "B";
                                        ops++;
                                    }
                                }
                                else if (cursor[0] != c)
                                {
                                    signed long sp = (signed long)cursor[0] - (signed long)c;
                                    if (sp > 0)
                                    {
                                        output << "\e[" << sp << "D";
                                        ops++;
                                    }
                                    else
                                    {
                                        output << "\e[" << -sp << "C";
                                        ops++;
                                    }
                                }

                                if (tcell.foregroundColour != currentForegroundColour)
                                {
                                    if (tcell.foregroundColour < 8)
                                    {
                                        output << "\e[3" << tcell.foregroundColour << "m";
                                        ops++;
                                    }
                                    else
                                    {
                                        output << "\e[38;5;" << tcell.foregroundColour << "m";
                                        ops++;
                                    }
                                }
                                if (tcell.backgroundColour != currentBackgroundColour)
                                {
                                    if (tcell.backgroundColour < 8)
                                    {
                                        output << "\e[4" << tcell.backgroundColour << "m";
                                        ops++;
                                    }
                                    else
                                    {
                                        output << "\e[48;5;" << tcell.backgroundColour << "m";
                                        ops++;
                                    }
                                }

                                if ((tcell.content < 0x20) || (tcell.content == 0x7f))
                                {
                                    output << ".";
                                    ops++;
                                    /* don't print control characters */
                                }
                                else if (tcell.content < 0x80)
                                {
                                    output << char(tcell.content);
                                    ops++;
                                }
                                else if (tcell.content < 0x800)
                                {
                                    output << char(((tcell.content >>  6) & 0x1f) | 0xc0)
                                           << char(( tcell.content        & 0x3f) | 0x80);
                                    ops++;
                                }
                                else if (tcell.content < 0x10000)
                                {
                                    output << char(((tcell.content >> 12) & 0x0f) | 0xe0)
                                           << char(((tcell.content >>  6) & 0x3f) | 0x80)
                                           << char(( tcell.content        & 0x3f) | 0x80);
                                    ops++;
                                }
                                else if (tcell.content < 0x200000)
                                {
                                    output << char(((tcell.content >> 18) & 0x07) | 0xf0)
                                           << char(((tcell.content >> 12) & 0x3f) | 0x80)
                                           << char(((tcell.content >>  6) & 0x3f) | 0x80)
                                           << char(( tcell.content        & 0x3f) | 0x80);
                                    ops++;
                                }
                                else if (tcell.content < 0x4000000)
                                {
                                    output << char(((tcell.content >> 24) & 0x03) | 0xf8)
                                           << char(((tcell.content >> 18) & 0x3f) | 0x80)
                                           << char(((tcell.content >> 12) & 0x3f) | 0x80)
                                           << char(((tcell.content >>  6) & 0x3f) | 0x80)
                                           << char(( tcell.content        & 0x3f) | 0x80);
                                    ops++;
                                }
                                else /* if (tcell.content < 0x80000000) */
                                {
                                    output << char(((tcell.content >> 30) & 0x01) | 0xfc)
                                           << char(((tcell.content >> 24) & 0x3f) | 0x80)
                                           << char(((tcell.content >> 18) & 0x3f) | 0x80)
                                           << char(((tcell.content >> 12) & 0x3f) | 0x80)
                                           << char(((tcell.content >>  6) & 0x3f) | 0x80)
                                           << char(( tcell.content        & 0x3f) | 0x80);
                                    ops++;
                                }

                                current[l][c] = tcell;
                                cursor[1] = l;
                                cursor[0] = c+1;
                                currentForegroundColour = tcell.foregroundColour;
                                currentBackgroundColour = tcell.backgroundColour;
                                if (ops >= targetOps)
                                {
                                    return ops;
                                }
                            }
                        }
                    }

                    return ops;
                }

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
                                    cursor[1] = 0;
                                    cursor[0] = 0;
                                    break;
                                case 1:
                                    st.clear();
                                    st.str(vtparams[0]);
                                    st >> cursor[1];
                                    cursor[1]--;
                                    cursor[0] = 0;
                                    break;
                                default:
                                    st.clear();
                                    st.str(vtparams[0]);
                                    st >> cursor[1];
                                    st.clear();
                                    st.str(vtparams[1]);
                                    st >> cursor[0];
                                    cursor[1]--;
                                    cursor[0]--;
                                    break;
                            }
                        }
                    }
                    while (!haveCommand);

                    return true;
                }

            private:
                /**\brief Has the constructor set up the terminal?
                 *
                 * Set to 'true' if the constructor has modified terminal
                 * settings which it should reverse in the destructor.
                 */
                const bool didSetup;

                /**\brief Terminal's current foreground colour
                 *
                 * Set as part of running flush; VT100s have 'current' colours
                 * and other text attributes, which are used when printing a
                 * character at the current position. The flush() function
                 * needs to know what this colour is so it can issue commands
                 * to change that colour, if necessary.
                 */
                std::size_t currentForegroundColour;

                /**\brief Terminal's current background colour
                 *
                 * Set as part of running flush; VT100s have 'current' colours
                 * and other text attributes, which are used when printing a
                 * character at the current position. The flush() function
                 * needs to know what this colour is so it can issue commands
                 * to change that colour, if necessary.
                 */
                std::size_t currentBackgroundColour;

                enum parserState
                {
                    text,
                    escape1,
                    escape2
                };

        };
    };
};

#endif
