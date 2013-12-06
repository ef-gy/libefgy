/**\file
 * \brief Matrix-esque terminal animation
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
#include <unistd.h>
#include <chrono>

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
            : line(pLine), column(pColumn), last(now) {}

        std::size_t line;
        std::size_t column;
        std::vector<cell> data;
        system_clock::time_point last;

        bool update (void)
        {
            if ((now - last) > std::chrono::microseconds(40))
            {
                last = now;
                switch (rng() % 4)
                {
                    case 0:
                        if (data.size() > 0)
                        {
                            data.erase(data.begin() + (rng() % data.size()));
                        }
                        else
                        {
                            data.push_back(cell(rng() % (1 << 7)));
                        }
                        break;
                    default:
                        data.push_back(cell(rng() % (1 << 7)));
                }
            }
            std::array<std::size_t,2> s = output.size();
            int i = line;
            for (cell &d : data)
            {
                if (i >= s[1])
                {
                    break;
                }
                output.target[i][column].content = d.character;
                output.target[i][column].foregroundColour = (now - d.created) > std::chrono::microseconds(120) ? 2 : 7;
                output.target[i][column].backgroundColour = 0;
                i++;
            }
            return true;
        }
};

int main (int argc, char **argv)
{
    output.resize(output.getOSDimensions());
    std::array<std::size_t,2> s = output.size();

    int i;

    std::vector<matrixStream> streams;

    while (1)
    {
        i++;
        now = system_clock::now();

        if (i % 100 == 0)
        {
            std::size_t l = rng() % s[1];
            std::size_t c = rng() % s[0];
            streams.push_back(matrixStream(l, c));
        }
        if (streams.size() > 200)
        {
            streams.erase(streams.begin());
        }

        for (matrixStream &s : streams)
        {
            if (rng() % 10 == 0)
            {
                s.update();
            }
        }

        std::cout << output.flush();
        usleep(10);
    }

    for (std::string s = output.flush(); s != ""; s = output.flush())
    {
        std::cout << s;
        i++;
    }

    std::cout << "\e[39;49;0m\n";

    return 0;
}
