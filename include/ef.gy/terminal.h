/**\file
 * \brief Terminal I/O helpers
 *
 * This header contains templates that should make it fairly easy to use
 * text-based terminals efficiently. The general idea is that this header
 * provides a class template, terminal::terminal, with two screen buffers, one
 * for the current state of the terminal and one for the target state of the
 * terminal. Terminal-specific classes can then derive from this base class and
 * implement efficient algorithms to transform the terminal state from the
 * current state to the target state.
 *
 * One such deriving class is terminal::vt100, see ef.gy/vt100.h for more
 * details on that. Also see src/matrix.cpp for some example code if you're
 * stuck with the documentation for some reason.
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
    /**\brief Terminal I/O helpers
     *
     * Contains classes and templates that should make it easy - or at least
     * easier - to produce snappy TUI interfaces, or other terminal
     * applications. See src/matrix.cpp for an example.
     */
    namespace terminal
    {
        /**\brief Terminal buffer cell
         *
         * Contains all the attributes associated with an individual cell in a
         * terminal buffer: the contained character, colours and any applicable
         * attributes.
         *
         * \tparam T Data type for an individual cell's contents. The default
         *           is a "long", which should be enough to cover all possible
         *           unicode codepoints, ever. Hopefully, anyway.
         */
        template<typename T = long>
        class cell
        {
            public:
                /**\brief The character in the cell
                 *
                 * Contains the current character in the given cell. Expect
                 * this to be a unicode codepoint.
                 */
                T content;

                /**\brief Foreground colour
                 *
                 * This is the colour of the glyph itself. The values in here
                 * do sort of depend on the terminal settings on the output
                 * driver, but expect xterm-256 codes to work as intended.
                 */
                int foregroundColour;

                /**\brief Background colour
                 *
                 * This is the colour of the glyph's background. The values in
                 * here do sort of depend on the terminal settings on the
                 * output driver, but expect xterm-256 codes to work as
                 * intended.
                 */
                int backgroundColour;

                /**\brief Text attributes
                 *
                 * \todo This really needs to be fleshed out properly.
                 */
                int attributes;

                /**\brief Compare two cells
                 *
                 * The frontend driver will need some way to figure out if two
                 * cells contain the same data, so as to figure out if a cell
                 * has been modified and needs to be modified. This operator
                 * provides that functionality.
                 *
                 * \param[in] b The cell to compare this one with.
                 *
                 * \returns 'true' if the two cells don't contain the same
                 *          data.
                 */
                constexpr bool operator != (const cell &b) const
                {
                    return (content          != b.content)
                        || (foregroundColour != b.foregroundColour)
                        || (backgroundColour != b.backgroundColour)
                        || (attributes       != b.attributes);
                }
        };

        /**\brief Screen buffer
         *
         * Contains the data stored in a single screen buffer - i.e. the
         * current contents of a terminal or the target contents.
         *
         * This buffer is derived from a nested std::vector using the 'cell'
         * template for individual cells. You should not modify this vector
         * manually, unless you really know what you're doing. Note that the
         * indices into this nested std::vector are reversed with respect to
         * the coordinate and dimension tuples used in other places.
         *
         * While odd, this was done on purpose - because typically, if you
         * iterate over the whole screen, you'd expect things on the same line
         * to be 'in sequence'; or at least you would if you read from left to
         * right, then top to bottom. However, it is customary to specify
         * terminal sizes as WidthXHeight, or rather ColumnsXLines, which
         * creates a bit of ambiguity in this case.
         *
         * \tparam T Data type to hold a single character of data. The default
         *         is 'long', which is easily able to handle all of the unicode
         *         code points.
         */
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
