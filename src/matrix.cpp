/**\file
 * \ingroup example-programmes
 * \brief Matrix-esque terminal animation
 *
 * This is a terminal programme that uses libefgy's vt100 code to render a text
 * version of the matrix 'scrolling streams of text' animation. It's really
 * fairly simple but also kinda nice to see how the vt100 output is performing.
 *
 * \image html matrix.png "Screenshot of the programme running in Terminal.app"
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

#include <iostream>

#include <ef.gy/vt100.h>
#include <ef.gy/random.h>
#include <chrono>
#include <csignal>
#include <cmath>
#include <sched.h>

using namespace efgy;
using namespace std::chrono;

/**\brief Data and functions of the matrix example programme
 *
 * Contains global variables and classes used by the 'matrix' demo animation.
 * This is a separate namespace to keep things all neat, clean and tidy.
 */
namespace thematrix
{
    /**\brief VT100 output buffer
     *
     * Encapsulates stdio to automatically generate the terminal escape
     * sequences to write things at specific positions with specific colours.
     */
    terminal::vt100<> output;

    /**\brief Random number generator
     *
     * A seeded instance of the mersenne twister RNG; used to position the
     * streams and to add and remove glyphs randomly.
     */
    random::mersenneTwister<> rng(1337);

    /**\brief Current time
     *
     * The point in time when processing of the current streams started; used
     * to determine when streams should be updated.
     */
    system_clock::time_point now;

    /**\brief A stream of data
     *
     * A 'stream' is what I dubbed the individual columns of text in the
     * animation. These are generated randomly and they mutate randomly as
     * well.
     */
    class stream
    {
        public:
            /**\brief A single glyph in a stream
             *
             * To create the animation, each of the glyphs in the output needs
             * to be tagges with the time it was created, in order to know
             * which colour to draw the glyph in.
             */
            class cell
            {
                public:
                    cell(const unsigned long &pCharacter)
                        : character(pCharacter), created(now) {}

                    unsigned long character;
                    system_clock::time_point created;
            };

            stream(const std::size_t &pLine, const std::size_t &pColumn)
                : line(pLine), column(pColumn), last(now), doDelete(false) {}

            std::size_t line;
            std::size_t column;
            std::vector<cell> data;
            system_clock::time_point last;
            bool doDelete;

            bool update (void)
            {
                if ((now - last) > std::chrono::milliseconds(10))
                {
                    last = now;
                    switch (rng() % 3)
                    {
                        case 0:
                            if (data.size() > 0)
                            {
                                data.erase(data.begin() + (rng() % data.size()));
                                break;
                            }
                        default:
                            data.push_back(cell(rng() % (1 << 7)));
                    }
                }
                std::array<std::size_t,2> s = output.size();
                if (data.size() > (s[1]/2))
                {
                    if (line < s[1])
                    {
                        if (rng() % 5 == 0)
                        {
                            output.target[line][column].content = ' ';
                            line++;
                        }
                    }
                    else
                    {
                        doDelete = true;
                    }
                    data.erase(data.begin());
                }
                int i = line;
                for (cell &d : data)
                {
                    if (i >= s[1])
                    {
                        break;
                    }
                    output.target[i][column].content = d.character;
                    output.target[i][column].foregroundColour = (now - d.created) > std::chrono::milliseconds(120) ? 2 : 7;
                    output.target[i][column].backgroundColour = 0;
                    i++;
                }
                return true;
            }
    };

    void handle_interrupt(int signal)
    {
        exit(1);
    }

    terminal::cell<long> postProcess
        (const terminal::terminal<long>&t,
         const std::size_t &l,
         const std::size_t &c)
    {
        terminal::cell<long> rv = t.target[l][c];
        rv.content = rv.content == 0 ? ' ' : rv.content;
        rv.foregroundColour = rv.foregroundColour == 7 ? 7 : 2;
        rv.backgroundColour = 0;
        return rv;
    }

    terminal::cell<long> postProcessPolar
        (const terminal::terminal<long>&t,
         const std::size_t &pl,
         const std::size_t &pc)
    {
        double l = pl;
        double c = pc;
        std::array<std::size_t,2> s = t.size();
        double hl = s[1]/2;
        double hc = s[0]/2;
        double loff = l - hl;
        double coff = c - hc;
        double r2 = loff*loff + coff*coff;
        double r = std::sqrt(r2);
        l = hl + loff + std::sin(r) * 1.0;
        c = hc + coff + std::cos(r) * 1.0;
        const std::size_t tl = (std::size_t)l < s[1] ? (std::size_t)l : (s[1] - 1);
        const std::size_t tc = (std::size_t)c < s[0] ? (std::size_t)c : (s[0] - 1);
        terminal::cell<long> rv = t.target[tl][tc];
        terminal::cell<long> cv = t.current[tl][tc];
        rv.content = rv.content == 0 ? (cv.content == 0 ? ' ' : cv.content) : rv.content;
        rv.foregroundColour = rv.foregroundColour == 7 ? 7 : 2;
        rv.backgroundColour = 0;
        return rv;
    }
};

using namespace thematrix;

int main (int argc, char **argv)
{
    output.resize(output.getOSDimensions());
    std::array<std::size_t,2> s = output.size();

    std::size_t i;

    std::vector<stream> streams;

    std::signal(SIGINT, handle_interrupt);

    while (1)
    {
        i++;
        now = system_clock::now();

        if ((streams.size() <= 100) && (i % 50 == 0))
        {
            std::size_t l = rng() % (s[1] / 3);
            std::size_t c = rng() %  s[0];
            streams.push_back(stream(l, c));
        }

        for (stream &s : streams)
        {
            s.update();
        }

        for (unsigned int i = 0; i < streams.size(); i++)
        {
            if (streams[i].doDelete)
            {
                streams.erase(streams.begin() + i);
            }
        }

        if (output.flush(postProcess) == 0)
        {
            sched_yield();
        }
    }

    while ((i = output.flush(postProcess)) > 0);

    return 0;
}
