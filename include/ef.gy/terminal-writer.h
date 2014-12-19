/**\file
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
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 */

#if !defined(EF_GY_TERMINAL_WRITER_H)
#define EF_GY_TERMINAL_WRITER_H

#include <ef.gy/terminal.h>
#include <string>

namespace efgy
{
    namespace terminal
    {
        template<typename T = long>
        class writer
        {
            public:
                writer(terminal<T> &pOutput)
                    : output(pOutput),
                      foreground(7),
                      background(0),
                      position({0,0})
                    {}

                int foreground;
                int background;
                std::array<ssize_t,2> position;

                writer &write (const T &ch)
                {
                    const auto dim = output.size();

                    position[0]  = (position[0] >= 0) ? position[0] : (dim[0] + position[0]);
                    position[1]  = (position[1] >= 0) ? position[1] : (dim[1] + position[1]);

                    position[0]  = (position[0] >= 0) ? position[0] : 0;
                    position[1]  = (position[1] >= 0) ? position[1] : 0;

                    position[1] += (position[0] >= dim[0]) ? 1 : 0;
                    position[0]  = (position[0] >= dim[0]) ? 0 : position[0];
                    position[1]  = (position[1] >= dim[1]) ? 0 : position[1];

                    auto &cell = output.target[position[1]][position[0]];
                    cell.content = ch;
                    cell.foregroundColour = foreground;
                    cell.backgroundColour = background;

                    position[0]++;
                    return *this;
                }

                template<typename C>
                writer &write (const std::basic_string<C> &str,
                               const std::size_t &width)
                {
                    for (std::size_t i = 0; i < width; i++)
                    {
                        write(i < str.size() ? str[i] : ' ');
                    }

                    return *this;
                }

                writer &bar(const ssize_t &min, const ssize_t &max,
                            const std::size_t &width,
                            const T &full = '#', const T &left = '[', const T &right = ']')
                {
                    const double perc = double(min)/double(max);
                    const std::size_t fullchars = perc > 0 ? (width-2) * perc : 0;

                    write(left);
                    for (std::size_t i = 0; i < (width - 2); i++)
                    {
                        write(i < fullchars ? full : ' ');
                    }
                    write(right);

                    return *this;
                }

                writer &to (const ssize_t &column, const ssize_t &line)
                {
                    position = {column, line};

                    return *this;
                }

                writer &x (const ssize_t &column)
                {
                    position[0] = column;

                    return *this;
                }

                writer &y (const ssize_t &line)
                {
                    position[1] = line;

                    return *this;
                }

            protected:
                terminal<T> &output;
        };
    }
}

#endif
