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

using efgy::math::numeric::bigIntegers;

voronoi<Q2> vor = voronoi<Q2>();

extern "C"
__attribute__((used))
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
/*
    addVoronoiPoint(200,1,200,1);
    addVoronoiPoint(800,1,200,1);
    addVoronoiPoint(200,1,800,1);
    addVoronoiPoint(800,1,800,1);
    addVoronoiPoint(500,1,500,1);
    addVoronoiPoint(501,1,501,1);
    addVoronoiPoint(501,1,502,1);
    addVoronoiPoint(501,1,503,1);
    addVoronoiPoint(501,1,506,1);
    addVoronoiPoint(501,1,507,1);
    addVoronoiPoint(501,1,508,1);
*/
/*
    for (unsigned int i = 0; i < 5; i++)
    {
        for (unsigned int j = 0; j < 5; j++)
        {
            addVoronoiPoint(i * 50, 1, 200 + j * 50, 1);
        }
    }

    addVoronoiPoint(50,1,225,1);
*/

/*
    addVoronoiPoint(200,1,200,1);
    addVoronoiPoint(800,1,200,1);
    addVoronoiPoint(500,1,700,1);
    addVoronoiPoint(700,1,700,1);
    addVoronoiPoint(300,1,500,1);
    addVoronoiPoint(100,1,100,1);
    addVoronoiPoint(100,2,100,2);
    addVoronoiPoint(100,3,100,3);
    addVoronoiPoint(3000,3,700,3);
    addVoronoiPoint(324,6,129,2);
    addVoronoiPoint(328,6,130,2);
*/
/*
    addVoronoiPoint(200,1,200,1);
    addVoronoiPoint(300,1,500,1);
    addVoronoiPoint(324,6,129,2);
    addVoronoiPoint(328,6,130,2);
*/

//    addVoronoiPoint(800,1,200,1);
//    addVoronoiPoint(324,6,129,2);
//    addVoronoiPoint(328,6,130,2);

/*
    addVoronoiPoint(200,1,200,1);
    addVoronoiPoint(800,1,200,1);
    addVoronoiPoint(324,6,129,2);
    addVoronoiPoint(328,6,130,2);
*/

#if 1
    for (unsigned int i = 0; i < 50; i++)
    {
        cerr << intToString(i);

        addVoronoiPoint(100+rand()%1000,1+rand()%10,100+rand()%1000,1+rand()%10);

        cout << "<g><rect x=\"-5000\" y=\"-5000\" width=\"10000\" height=\"10000\" fill=\"white\" />";
        cout << svg (vor);
        cout << "</g>";
    }
#endif
#if 0
    for (unsigned int i = 0; i < 50; i++)
    {
        addVoronoiPoint(100+rand()%1000,100000000,100+rand()%1000,100000000);
    }
#endif

/*
    for (unsigned int i = 0; i < 100; i++)
    {
        addVoronoiPoint(100+rand()%1000,1,100+rand()%1000,1);
    }
*/
/*
    polygon<Q2> poly;

    for (unsigned int i = 0; i < 10; i++)
    {
        polygon<Q2> a;
        Q2::scalar s[2];
        s[0] = Q2::scalar(Q(100+rand()%1200, 1+rand()%10));
        s[1] = Q2::scalar(Q(100+rand()%700, 1+rand()%10));

        Q2::vector v = Q2::vector (s);

        a = a + s;

        svg(v, buffer, bufferSize);
        puts (buffer);

        poly = poly + a;
    }

    svg(poly, buffer, bufferSize);
    puts (buffer);
*/

/*
    polygon<Q2> poly;
    polygon<Q2> a;

    Q2::scalar s[2];
    s[0] = Q2::scalar(Q(33739, 1));
    s[1] = Q2::scalar(Q(-800, 1));

    Q2::vector v = Q2::vector (s);

    a = a + s;

    svg(v, buffer, bufferSize);
    puts (buffer);
    poly = poly + a;

    s[0] = Q2::scalar(Q(-3841693, 502));
    s[1] = Q2::scalar(Q(282299, 502));

    v = Q2::vector (s);

    a = polygon<Q2>();
    a = a + s;

    svg(v, buffer, bufferSize);
    puts (buffer);
    poly = poly + a;

    s[0] = Q2::scalar(Q(1158895, 1));
    s[1] = Q2::scalar(Q(-800, 1));

    v = Q2::vector (s);

    a = polygon<Q2>();
    a = a + s;

    svg(v, buffer, bufferSize);
    puts (buffer);
    poly = poly + a;

    s[0] = Q2::scalar(Q(-106823, 484));
    s[1] = Q2::scalar(Q(245341, 484));

    v = Q2::vector (s);

    a = polygon<Q2>();
    a = a + s;

    svg(v, buffer, bufferSize);
    puts (buffer);
    poly = poly + a;

    svg(poly, buffer, bufferSize);
    puts (buffer);
*/

    cout << "</svg>";

/*
    bigIntegers<signed long long, unsigned long long, unsigned char, 8> q = 127;

    for (unsigned int i = 0; i < 256; i++)
    {
        q -= bigIntegers<signed long long, unsigned long long, unsigned char, 8>(3);
    }

    fprintf (stderr, "%lli\n", (signed long long)q);

    bigIntegers<signed long long, unsigned long long, unsigned char, 8> p = 127;

    for (unsigned int i = 0; i < 256; i++)
    {
        p -= bigIntegers<signed long long, unsigned long long, unsigned char, 8>(4);
    }

    fprintf (stderr, "%lli\n", (signed long long)p);

    fprintf (stderr, "%lli\n", (signed long long)(p + q));
    fprintf (stderr, "%lli\n", (signed long long)(p % q));
*/

#if 0
    bigIntegers<signed long long, unsigned long long, unsigned char, 8> p = 0;
    signed long long pn = 0;
    bool nuke = false;

    for (unsigned int i = 0; i < 200000; i++)
    {
        signed long long qn = 0;
        bigIntegers<signed long long, unsigned long long, unsigned char, 8> q;

        for (unsigned int j = 0; j < 50; j++)
        {
            signed long long qnt = (signed long long)(rand() % 16) + 1;
            bigIntegers<signed long long, unsigned long long, unsigned char, 8> qt = qnt;

            qn += qnt;
            q += qt;
        }

        if ((rand() % 2) == 0)
        {
            qn *= -1;
            q *= -1;
        }

        if (((long double)p != (long double)pn) ||
            ((signed long long)q != (signed long long)qn))
        {
            fprintf (stderr, "!! %Lf != %Lf || %lli != %lli\n", (long double)p, (long double)pn, (signed long long)q, (signed long long)qn);
            nuke = true;
        }

        if ((p > q) !=
            ((signed long long)pn > (signed long long)qn))
        {
            fprintf (stderr, "!! %lli > %lli != %lli > %lli\n", (signed long long)p, (signed long long)q, (signed long long)pn, (signed long long)qn);
            nuke = true;
        }

        if ((p < q) !=
            ((signed long long)pn < (signed long long)qn))
        {
            fprintf (stderr, "!! %lli < %lli != %lli < %lli\n", (signed long long)p, (signed long long)q, (signed long long)pn, (signed long long)qn);
            nuke = true;
        }

        if ((p == q) !=
            ((signed long long)pn == (signed long long)qn))
        {
            fprintf (stderr, "!! %lli == %lli != %lli == %lli\n", (signed long long)p, (signed long long)q, (signed long long)pn, (signed long long)qn);
            nuke = true;
        }

        if (nuke)
        {
            break;
        }

//        p = p + q;
//        pn = pn + qn;

        int sw = rand() % 5;

        if ((q != bigIntegers<signed long long, unsigned long long, unsigned char, 8>(0)) && (sw == 0))
        {
            fprintf (stderr, "test: %Lf mod %Lf = %Lf\n", (long double)p, (long double)q, (long double)(p%q));
            fprintf (stderr, "control: %lli mod %lli = %lli\n", pn, qn, (pn%qn));

            p %= q;
            pn %= qn;

//            p >>= 2;
//            pn >>= 2;
        }
        else if (!p.negative && (sw == 1))
        {
            fprintf (stderr, "test: %lli >> 5 = %lli\n", (signed long long)p, (signed long long)(p >>= 5));
            fprintf (stderr, "control: %lli >> 5 = %lli\n", pn, (pn >>= 5));
        }
        else if (!p.negative && (sw == 2))
        {
            fprintf (stderr, "test: %Lf << 9 = %Lf\n", (long double)p, (long double)(p <<= 9));
            fprintf (stderr, "control: %lli << 9 = %lli\n", pn, (pn <<= 9));
        }
        else if (sw == 3)
        {
            fprintf (stderr, "test: %Lf * %Lf = %Lf\n", (long double)p, (long double)q, (long double)(p*q));
            fprintf (stderr, "control: %lli * %lli = %lli\n", pn, qn, (pn*qn));

            p *= q;
            pn *= qn;
        }
        else
        {
            p += q;
            pn += qn;
        }

        if (!(i % 1000))
        {
            fprintf (stderr, " -- c (%i) : %lli, %lli\n", i, (signed long long)p, (signed long long)pn);
        }
    }
#endif

/*
    Q rq = bigIntegers<>(0) / bigIntegers<>(1);
    Q rq2 = bigIntegers<>(0) / bigIntegers<>(-2);
    Q re = rq / bigIntegers<>(-2);

    if (rq < rq2)
    {
        fprintf (stderr, "rq < rq2\n");
    }
    if (rq <= rq2)
    {
        fprintf (stderr, "rq <= rq2\n");
    }
    if (rq > rq2)
    {
        fprintf (stderr, "rq > rq2\n");
    }
    if (rq >= rq2)
    {
        fprintf (stderr, "rq >= rq2\n");
    }
    if (rq == rq2)
    {
        fprintf (stderr, "rq == rq2\n");
    }

    fprintf (stderr, "res = %g (%lli/%lli)\n", (double)re, (signed long long)re.numerator, (signed long long)re.denominator);
*/

/*
    bigIntegers<signed long long, unsigned long long, unsigned char, 8> q = 2;

    q -= q;

    fprintf (stderr, "c: %lli\n", (signed long long)q);
*/

/*
    {
        efgy::data::scratchPad<int> foo = efgy::data::scratchPad<int>(30);
        efgy::data::scratchPad<int> bar = foo;

        foo[20] = 42;

        const efgy::data::scratchPad<int> &baz = bar;
        
        fprintf (stderr, "%i:%i\n", foo[20], foo[2]);
        fprintf (stderr, "%i:%i\n", baz[20], baz[2]);
    }
*/

/*
    {
        Z a = Z(2);
        Z b = Z(4);

        fprintf (stderr, "  * %Lf\n", (long double)(a ^ b));
    }
*/

/*
    fprintf (stderr, "  * %lu, %lu(%lu), %lu\n", sizeof(efgy::data::scratchPad<unsigned short>), sizeof(efgy::data::referenceCountedArray<unsigned short>), efgy::data::referenceCountedArray<unsigned short>::spaceLeft, sizeof (efgy::data::referenceCountedArray<unsigned short>::content));
*/

    return 0;
}
