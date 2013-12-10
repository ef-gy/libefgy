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
#include <ef.gy/maybe.h>
#include <vector>
#include <array>
#include <iostream>
#if !defined(NO_IOCTL)
#include <sys/ioctl.h>
#endif
#if !defined(NO_TERMIOS)
#include <termios.h>
#endif

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

                constexpr bool operator != (const cell &b) const
                {
                    return (content          != b.content)
                        || (foregroundColour != b.foregroundColour)
                        || (backgroundColour != b.backgroundColour)
                        || (attributes       != b.attributes);
                }
        };

        template<typename T = long>
        class screen: public std::vector<std::vector<cell<T> > >
        {
            public:
                typedef std::vector<std::vector<cell<T> > > parent;
                using parent::resize;

                screen(const parent &p)
                    : parent(p) {}

                screen(int width = 80, int height = 24)
                    {
                        resize(width, height);
                    }

                bool resize(std::size_t columns, std::size_t lines)
                {
                    resize(lines);

                    for (std::vector<cell<T> > &column : *this)
                    {
                        column.resize(columns);
                    }

                    return true;
                }

                bool resize(const std::array<std::size_t,2> &size)
                {
                    return resize(size[0], size[1]);
                }

                constexpr std::array<std::size_t,2> size (void) const
                {
                    return {{ this->parent::size() > 0 ? (*this)[0].size() : 0,
                              this->parent::size() }};
                }
        };

        template<typename T = long>
        class terminal
        {
            public:
                terminal
                    (std::istream &pInput  = std::cin,
                     std::ostream &pOutput = std::cout,
                     const bool &doSetup = true)
                    : input(pInput), output(pOutput),
                      cursor({{(std::size_t)-1,(std::size_t)-1}}),
                      didSetup(doSetup)
                    {
#if defined(TCSANOW)
                        if (doSetup)
                        {
                            std::ios_base::sync_with_stdio(false);

                            termios terminalFlags;
                            tcgetattr (0, &originalTerminalFlags);
                            terminalFlags = originalTerminalFlags;
                            terminalFlags.c_lflag &= ~(ICANON | ECHO);
                            tcsetattr (0, TCSANOW, &terminalFlags);
                            didSetup = true;
                        }
#endif
                    }

#if defined(TCSANOW)
                ~terminal
                    (void)
                    {
                        if (didSetup)
                        {
                            tcsetattr (0, TCSANOW, &originalTerminalFlags);
                        }
                    }
#endif

                screen<T> current;
                screen<T> target;
                std::istream &input;
                std::ostream &output;

                std::vector<T> queue;

                bool resize(std::size_t columns, std::size_t lines)
                {
                    return current.resize (columns, lines)
                        && target.resize  (columns, lines);
                }

                bool resize(const maybe<std::array<std::size_t,2> > &size)
                {
                    return size
                        && current.resize (std::array<std::size_t,2>(size))
                        && target.resize  (std::array<std::size_t,2>(size));
                }

                constexpr std::array<std::size_t,2> size (void) const
                {
                    return current.size();
                }

                maybe<std::array<std::size_t,2> > getOSDimensions (void) const
                {
#if defined(TIOCGWINSZ)
                    winsize w;
                    ioctl (0, TIOCGWINSZ, &w);
                    return maybe<std::array<std::size_t,2> >
                        (std::array<std::size_t,2>({{ w.ws_col, w.ws_row }}));
#else
                    return maybe<std::array<std::size_t,2> >();
#endif
                }

                maybe<T> read (void)
                {
                    if (!input)
                    {
                        return maybe<T>();
                    }
                    else
                    {
                        char n;
                        input >> n;
                        queue.push_back (T(n));
                        return maybe<T>(T(n));
                    }
                }

                /**\brief Write string to target buffer
                 *
                 * Will write the given text at the coordinates specified by
                 * (column, line) with the specified colours to the target
                 * buffer.
                 *
                 * If you omit either the column or the line then this method
                 * will write to the internal cursor position. If you omit the
                 * colours then this method will not modify the colours at the
                 * specified coordinates.
                 *
                 * This function will break to the next line at the end of a
                 * line. If that line happens to be the last one on the screen,
                 * then the method will wrap around to (0,0).
                 *
                 * The internal cursor position is NOT updated after the write
                 * operation.
                 *
                 * \tparam S Character type of the string you pass to this
                 *           function; separate from T because you might well
                 *           try to write a regular string to a terminal<long>.
                 *
                 * \param[in] text   The string to write.
                 * \param[in] column Column coordinate of the target position.
                 * \param[in] line   Line coordinate of the target position.
                 * \param[in] foregroundColour The foreground colour to use.
                 * \param[in] backgroundColour The background colour to use.
                 *
                 * \returns 'true' if the operation succeeded, 'false'
                 *          otherwise. Since there is no way for this function
                 *          to fail right now, it'll always return 'true'.
                 */
                template<typename S>
                bool write
                    (const std::basic_string<S> &text,
                     const maybe<std::size_t> &column = maybe<std::size_t>(),
                     const maybe<std::size_t> &line = maybe<std::size_t>(),
                     const maybe<int> &foregroundColour = maybe<int>(),
                     const maybe<int> &backgroundColour = maybe<int>())
                {
                    std::size_t c = column ? std::size_t(column) : cursor[0];
                    std::size_t l = line   ? std::size_t(line)   : cursor[1];
                    std::array<std::size_t,2> si = size();

                    for (const S &character : text)
                    {
                        if (c >= si[0])
                        {
                            c = 0;
                            l++;
                        }
                        if (l >= si[1])
                        {
                            l = 0;
                        }
                        target[l][c].content = T(character);
                        if (foregroundColour)
                        {
                            target[l][c].foregroundColour = foregroundColour;
                        }
                        if (backgroundColour)
                        {
                            target[l][c].backgroundColour = backgroundColour;
                        }
                        c++;
                    }

                    return true;
                }

            protected:
                /**\brief Current cursor position
                 *
                 * This is the current device cursor position. Should be kept
                 * up to date by any terminal frontends that make use of this
                 * information - e.g. by the vt100 class.
                 */
                std::array<std::size_t,2> cursor;

            private:
#if defined(TCSANOW)
                bool didSetup;
                termios originalTerminalFlags;
#endif

        };
    };
};

#endif
