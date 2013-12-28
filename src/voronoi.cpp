/**\file
 * \deprecated Will be removed in libefgy-5
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

#include <stdio.h>
#include <iostream>
#include <string>

#include <ef.gy/geometry.h>
#include <ef.gy/voronoi.h>
#include <ef.gy/svg.h>

#include <stdlib.h>
#include <time.h>

using namespace efgy;
using namespace efgy::math;
using namespace efgy::geometry;
using namespace efgy::geometry::euclidian;
using namespace efgy::render;

using namespace std;

voronoi<Q2> vor = voronoi<Q2>();

void addVoronoiPoint (int xn, int xd, int yn, int yd)
{
    Q2::scalar s[2];
    s[0] = Q2::scalar(Q(xn, xd));
    s[1] = Q2::scalar(Q(yn, yd));

    Q2::vector v = Q2::vector (s);

    colour::HSLA<Q>::value colour;

    colour.hue        = Q(rand()%360, 360);
    colour.saturation = Q(1, 2);
    colour.lightness  = Q(1, 2);
    colour.alpha      = Q(1, 2);

    vor = vor + voronoi<Q2>::cell(v, colour);
}

int main()
{
    cout << "<?xml version=\"1.0\" encoding=\"utf-8\" ?><svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" viewBox=\"0 0 1000 1000\" version=\"1.1\">";

    srand(time(0));

    for (unsigned int i = 0; i < 50; i++)
    {
        cerr << intToString(i);

        addVoronoiPoint(100+rand()%1000,1+rand()%10,100+rand()%1000,1+rand()%10);
    }

    cout << svg (vor);
    cout << "</svg>";

    return 0;
}
