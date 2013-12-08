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

using namespace efgy;
using namespace std::chrono;

terminal::vt100<> output;
random::mersenneTwister<> rng(1337);
system_clock::time_point now;

class matrixStream
{
    public:
        class cell
        {
            public:
                cell(const unsigned long &pCharacter)
                    : character(pCharacter), created(now) {}

                unsigned long character;
                system_clock::time_point created;
        };

        matrixStream(const std::size_t &pLine, const std::size_t &pColumn)
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
     const std::size_t&l,
     const std::size_t&c)
{
    terminal::cell<long> rv = t.target[l][c];
    rv.content = rv.content == 0 ? ' ' : rv.content;
    rv.foregroundColour = rv.foregroundColour == 7 ? 7 : 2;
    rv.backgroundColour = 0;
    return rv;
}

int main (int argc, char **argv)
{
    output.resize(output.getOSDimensions());
    std::array<std::size_t,2> s = output.size();

    std::size_t i;

    std::vector<matrixStream> streams;

    std::signal(SIGINT, handle_interrupt);

    while (1)
    {
        i++;
        now = system_clock::now();

        if ((streams.size() <= 100) && (i % 50 == 0))
        {
            std::size_t l = rng() % (s[1] / 3);
            std::size_t c = rng() %  s[0];
            streams.push_back(matrixStream(l, c));
        }

        for (matrixStream &s : streams)
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
        }
    }

    while ((i = output.flush(postProcess)) > 0);

    return 0;
}
