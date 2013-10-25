/*
 * This file is part of the ef.gy project.
 * See the appropriate repository at http://ef.gy/.git for exact file
 * modification records.
*/

/*
 * Copyright (c) 2012-2013, ef.gy Project Members
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

#if !defined(EF_GY_GEOMETRY_H)
#define EF_GY_GEOMETRY_H

#include <ef.gy/maybe.h>
#include <ef.gy/set.h>
#include <ef.gy/math.h>
#include <ef.gy/euclidian.h>
#include <ef.gy/colour-space-hsl.h>

namespace efgy
{
    namespace geometry
    {
        template <typename S>
        class polygon;

        template <typename S, unsigned int n>
        class ngon : public math::tuple<n, typename S::vector>
        {
            public:
                typedef typename S::base base;
                typedef typename S::vector vector;
                typedef typename S::scalar scalar;

                using math::tuple<n, vector>::data;

                ngon ()
                    : math::tuple<n, vector>()
                    {}

                ngon (const vector pV[n])
                    : math::tuple<n, vector>(pV)
                    {}

                vector midpoint (void) const
                {
                    vector v = data[0];

                    for (unsigned int i = 1; i < n; i++)
                    {
                        v += data[i];
                    }

                    return v / scalar(n);
                }

                bool operator == (const ngon &b) const
                {
                    for (unsigned int i = 0; i < n; i++)
                    {
                        if (data[i] != b.data[i])
                        {
                            return false;
                        }
                    }

                    return true;
                }

                bool operator != (const ngon &b) const
                {
                    return !((*this) == b);
                }

                operator polygon<S> (void)
                {
                    polygon<S> p = polygon<S>();

                    p.data.resize (n);

                    for (unsigned int i = 0; i < n; i++)
                    {
                        p.data[i] = data[i];
                    }

                    p.colour = colour;

                    return p;
                }

                typename colour::HSLA<base>::value colour;
        };

        template <typename S>
        class line : public ngon<S,2>
        {
            public:
                typedef typename S::base base;
                typedef typename S::vector vector;
                typedef typename S::scalar scalar;

                using ngon<S,2>::data;

                line ()
                    : ngon<S,2>(), A(data[0]), B(data[1])
                    {}

                line (const vector pV[2])
                    : ngon<S,2>(pV), A(data[0]), B(data[1])
                    {}

                line (const vector &pA, const vector &pB)
                    : A(data[0]), B(data[1])
                    {
                        data[0] = pA;
                        data[1] = pB;
                    }

                line (const line &pL)
                    : A(data[0]), B(data[1])
                    {
                        data[0] = pL.A;
                        data[1] = pL.B;
                    }

                line (const ngon<S,2> &pL)
                    : A(data[0]), B(data[1])
                    {
                        data[0] = pL.data[0];
                        data[1] = pL.data[1];
                    }

                line &operator = (const line &pL)
                    {
                        data[0] = pL.data[0];
                        data[1] = pL.data[1];
                        return *this;
                    }

                line &operator = (const ngon<S,2> &pL)
                    {
                        data[0] = pL.data[0];
                        data[1] = pL.data[1];
                        return *this;
                    }

                vector &A;
                vector &B;
        };

        template <typename S>
        class triangle : public ngon<S,3>
        {
            public:
                typedef typename S::base base;
                typedef typename S::vector vector;
                typedef typename S::scalar scalar;

                using ngon<S,3>::data;
                using ngon<S,3>::colour;

                triangle ()
                    : ngon<S,3>(), A(data[0]), B(data[1]), C(data[2])
                    {}

                triangle (const vector pV[3])
                    : ngon<S,3>(pV), A(data[0]), B(data[1]), C(data[2])
                    {}

                triangle (const vector &pA, const vector &pB, const vector &pC)
                    : A(data[0]), B(data[1]), C(data[2])
                    {
                        data[0] = pA;
                        data[1] = pB;
                        data[2] = pC;
                    }

                triangle (const triangle &pL)
                    : A(data[0]), B(data[1]), C(data[2])
                    {
                        data[0] = pL.A;
                        data[1] = pL.B;
                        data[2] = pL.C;
                        colour = pL.colour;
                    }

                triangle (const ngon<S,3> &pL)
                    : A(data[0]), B(data[1]), C(data[2])
                    {
                        data[0] = pL.data[0];
                        data[1] = pL.data[1];
                        data[2] = pL.data[2];
                        colour = pL.colour;
                    }

                triangle &operator = (const triangle &pL)
                    {
                        data[0] = pL.data[0];
                        data[1] = pL.data[1];
                        data[2] = pL.data[2];
                        colour = pL.colour;
                        return *this;
                    }

                triangle &operator = (const ngon<S,3> &pL)
                    {
                        data[0] = pL.data[0];
                        data[1] = pL.data[1];
                        data[2] = pL.data[2];
                        colour = pL.colour;
                        return *this;
                    }

                vector &A;
                vector &B;
                vector &C;
        };

        template <typename S>
        class polygon : public math::set<typename S::vector>
        {
            public: 
                typedef typename S::base base;
                typedef typename S::vector vector;
                typedef typename S::scalar scalar;

                using math::set<vector>::data;

                using math::set<vector>::operator =;
                using math::set<vector>::operator +;
                using math::set<vector>::operator -;
                using math::set<vector>::operator ==;

                polygon (void)
                    : math::set<vector>()
                    {}

                polygon (const polygon &pB)
                    : math::set<vector>(pB), colour(pB.colour)
                    {}

                vector midpoint (void) const
                {
                    vector v = data[0];

                    for (unsigned int i = 1; i < data.size(); i++)
                    {
                        v += data[i];
                    }

                    return v / scalar(data.size());
                }

                polygon operator + (const polygon &b)
                {
                    if (data.size() == 0)
                    {
                        return b;
                    }

                    polygon rv;
                    polygon p;
                    vector l = data[0];
                    vector m;
                    line<S> h;
                    vector start;

                    p = p + l;

                    for (unsigned int i = 1; i < data.size(); i++)
                    {
                        const vector &u = data[i];

                        p = p + u;

                        if ((u.data[0] < l.data[0]) ||
                            ((u.data[0] == l.data[0]) &&
                             (u.data[1] < l.data[1])))
                        {
                            l = u;
                        }
                    }
                    for (unsigned int i = 0; i < b.data.size(); i++)
                    {
                        const vector &u = b.data[i];

                        p = p + u;

                        if ((u.data[0] < l.data[0]) ||
                            ((u.data[0] == l.data[0]) &&
                             (u.data[1] < l.data[1])))
                        {
                            l = u;
                        }
                    }

                    if (p.data.size() <= 2)
                    {
                        return p;
                    }

                    rv = rv + l;

                    start = l;

/*
                    for (unsigned int i = 0; i < p.data.size(); i++)
                    {
                        const vector &u = p.data[i];

                        fprintf (stderr, "x=(%lli/%lli), y=(%lli/%lli)\n", (signed long long)u.data[0].numerator, (signed long long)u.data[1].denominator, (signed long long)u.data[1].numerator, (signed long long)u.data[1].denominator);
                    }

                    fprintf (stderr, "start=(%f,%f)\n", (float)start.data[0], (float)start.data[1]);
*/

                    do
                    {
                        m = start;
                        h = line<S>(l, m);

                        for (unsigned int i = 0; i < p.data.size(); i++)
                        {
                            const vector &u = p.data[i];

                            bool doAdd = false;

                            if (u && h)
                            {
                                scalar dh = euclidian::lengthSquared<base,2>(vector(l - m));
                                scalar du = euclidian::lengthSquared<base,2>(vector(l - u));

                                if (du > dh)
                                {
                                    doAdd = true;
                                }
                            }
                            else if (u < h)
                            {
                                doAdd = true;
                            }

                            if (doAdd)
                            {
                                m = u;
                                h = line<S>(l, u);
                            }
                        }

                        rv = rv + m;
                        l = m;
                    }
                    while (m != start);

                    rv.colour = colour;

                    return rv;
                }

                typename colour::HSLA<base>::value colour;
        };

        template <typename S>
        class object : public math::set<polygon<S> >
        {
            public: 
                typedef typename S::vector vector;
                typedef typename S::scalar scalar;

                using math::set<polygon<S> >::operator =;
                using math::set<polygon<S> >::operator +;
                using math::set<polygon<S> >::operator -;
                using math::set<polygon<S> >::operator ==;

                object ()
                    : math::set<polygon<S> >()
                    {}

                object (const scalar &boundingBoxSize)
                    : math::set<polygon<S> >()
                    {}

                object (const object &pB)
                    : math::set<polygon<S> >(pB)
                    {}
        };

        /* test if a vector is inside a given ngon */
        template<typename Q, unsigned int n>
        bool operator && (const ngon<euclidian::space<Q,2>,n> &a, const typename euclidian::space<Q,2>::vector &b)
        {
            /* for this test, we simply test if the vertex is right of all the lines.
             * this assumes a convex ngon. */

            bool rv = b >= line<euclidian::space<Q,2> >(a.data[(n-1)], a.data[0]);

            for (unsigned int i = 1; rv && (i < n); i++)
            {
                rv = b >= line<euclidian::space<Q,2> >(a.data[(i-1)], a.data[i]);
            }

            return rv;
        }

        /* test if a vector is inside a given polygon */
        template<typename Q>
        bool operator && (const polygon<euclidian::space<Q,2> > &a, const typename euclidian::space<Q,2>::vector &b)
        {
            /* for this test, we simply test if the vertex is right of all the lines.
             * this assumes a convex polygon. */

            bool rv = b >= line<euclidian::space<Q,2> >(a.data[(a.data.size()-1)], a.data[0]);

            for (unsigned int i = 1; rv && (i < a.data.size()); i++)
            {
                rv = b >= line<euclidian::space<Q,2> >(a.data[(i-1)], a.data[i]);
            }

            return rv;
        }

        /* test if a vector is "left" of a given line */
        template<typename Q>
        bool operator < (const typename euclidian::space<Q,2>::vector &a, const ngon<euclidian::space<Q,2>,2> &b)
        {
            return !(a >= b);
        }

        /* test if a vector is "right" of a given line */
        template<typename Q>
        bool operator > (const typename euclidian::space<Q,2>::vector &a, const ngon<euclidian::space<Q,2>,2> &b)
        {
            return !(a <= b);
        }

        /* test if a vector is "left of" or on a given line */
        template<typename Q>
        bool operator <= (const typename euclidian::space<Q,2>::vector &a, const ngon<euclidian::space<Q,2>,2> &b)
        {
            Q r = (b.data[1].data[0]-b.data[0].data[0])*(a.data[1]-b.data[0].data[1])
                - (b.data[1].data[1]-b.data[0].data[1])*(a.data[0]-b.data[0].data[0]);

            return r <= math::numeric::zero();
        }

        /* test if a vector is "right of" or on a given line */
        template<typename Q>
        bool operator >= (const typename euclidian::space<Q,2>::vector &a, const ngon<euclidian::space<Q,2>,2> &b)
        {
            Q r = (b.data[1].data[0]-b.data[0].data[0])*(a.data[1]-b.data[0].data[1])
                - (b.data[1].data[1]-b.data[0].data[1])*(a.data[0]-b.data[0].data[0]);

            return r >= math::numeric::zero();
        }

        /* test if a vector is on (i.e. neither left or right of) a given line */
        template<typename Q>
        bool operator && (const typename euclidian::space<Q,2>::vector &a, const ngon<euclidian::space<Q,2>,2> &b)
        {
            Q r = (b.data[1].data[0]-b.data[0].data[0])*(a.data[1]-b.data[0].data[1])
                - (b.data[1].data[1]-b.data[0].data[1])*(a.data[0]-b.data[0].data[0]);

            return r == math::numeric::zero();
        }

        /* intersect two lines, i.e. find the point P where P is on
         * both line a and line b */
        template<typename Q>
        maybe<typename euclidian::space<Q,2>::vector> operator & (const ngon<euclidian::space<Q,2>,2> &a, const ngon<euclidian::space<Q,2>,2> &b)
        {
            typename euclidian::space<Q,2>::scalar d
                = (a.data[1].data[0] - a.data[0].data[0])
                * (b.data[1].data[1] - b.data[0].data[1])
                - (a.data[1].data[1] - a.data[0].data[1])
                * (b.data[1].data[0] - b.data[0].data[0]);

            if (d != typename euclidian::space<Q,2>::scalar(0))
            {
                typename euclidian::space<Q,2>::scalar p
                    = ((a.data[0].data[1] - b.data[0].data[1])
                     * (b.data[1].data[0] - b.data[0].data[0])
                     - (a.data[0].data[0] - b.data[0].data[0])
                     * (b.data[1].data[1] - b.data[0].data[1]))
                    / d;
/*
                typename euclidian::space<Q,2>::scalar q
                    = ((a.data[0].data[1] - b.data[0].data[1])
                     * (a.data[1].data[0] - a.data[0].data[0])
                     - (a.data[0].data[0] - b.data[0].data[0])
                     * (a.data[1].data[1] - a.data[0].data[1]))
                    / d;
*/

                return maybe<typename euclidian::space<Q,2>::vector>
                    (a.data[0] + p*(a.data[1] - a.data[0]));
            }

            return maybe<typename euclidian::space<Q,2>::vector>();
        }

        /* split a line in two parts given a second line.
         * the second line is considered "infinite", i.e.
         * not a line segment. */
        template<typename Q>
        typename math::tuple<2,maybe<ngon<euclidian::space<Q,2>,2> > > operator / (const ngon<euclidian::space<Q,2>,2> &a, const ngon<euclidian::space<Q,2>,2> &b)
        {
            maybe<ngon<euclidian::space<Q,2>,2> > r[2];

            typename euclidian::space<Q,2>::scalar d
                = (a.data[1].data[0] - a.data[0].data[0])
                * (b.data[1].data[1] - b.data[0].data[1])
                - (a.data[1].data[1] - a.data[0].data[1])
                * (b.data[1].data[0] - b.data[0].data[0]);

            if (d != typename euclidian::space<Q,2>::scalar(0))
            {
                typename euclidian::space<Q,2>::scalar p
                    = ((a.data[0].data[1] - b.data[0].data[1])
                     * (b.data[1].data[0] - b.data[0].data[0])
                     - (a.data[0].data[0] - b.data[0].data[0])
                     * (b.data[1].data[1] - b.data[0].data[1]))
                    / d;

                /* ignore intersection if it's not on the first line segment */
                if ((p >= typename euclidian::space<Q,2>::scalar(0)) &&
                    (p <= typename euclidian::space<Q,2>::scalar(1)))
                {
                    typename euclidian::space<Q,2>::vector v
                        = a.data[0] + p*(a.data[1] - a.data[0]);
                    ngon<euclidian::space<Q,2>,2> A = a;
                    ngon<euclidian::space<Q,2>,2> B = a;

                    A.data[1] = v;
                    B.data[0] = v;

                    r[0] = A;
                    r[1] = B;

                    return math::tuple<2,maybe<ngon<euclidian::space<Q,2>,2> > > (r);
                }
            }

            r[0] = a;

            return math::tuple<2,maybe<ngon<euclidian::space<Q,2>,2> > > (r);
        }

        /* split a polygon in two parts given a line to intersect with.
         * the line is considered "infinite", i.e.
         * not a line segment.
         *
         * The result is a triple of polygons. The first two polygons are the
         * resulting polygons after the cut. The third polygon is actually just
         * a collection of points where the given line intersected with the
         * polygon's edges.
         *
         * NOTE: there is no analogous function for ngons, because this
         * operation may well change the vertex data.size().
         */
        template<typename Q>
        typename math::tuple<3,maybe<polygon<euclidian::space<Q,2> > > > operator / (const polygon<euclidian::space<Q,2> > &a, const ngon<euclidian::space<Q,2>,2> &b)
        {
            polygon<euclidian::space<Q,2> > p3;

            typename math::tuple<3,maybe<polygon<euclidian::space<Q,2> > > > rv;

            maybe<typename euclidian::space<Q,2>::vector> lastPoint;

            for (unsigned int i = 0; i < a.data.size(); i++)
            {
                unsigned int r = ((i == 0) ? a.data.size() : i) - 1;

                typename math::tuple<2,maybe<ngon<euclidian::space<Q,2>,2> > > s =
                    line<euclidian::space<Q,2> >(a.data[r], a.data[i]) / b;

                if (s.data[0] && s.data[1])
                {
                    ngon<euclidian::space<Q,2>,2> A = s.data[0];
                    ngon<euclidian::space<Q,2>,2> B = s.data[1];

                    p3 = p3 + A.data[1];
                }
            }

            switch (p3.data.size())
            {
                case 0: /* no intersections */
                case 1: /* this ought to be impossible, unless it intersects a vertex */
                    rv.data[0] = maybe<polygon<euclidian::space<Q,2> > >(a);
                    rv.data[1] = maybe<polygon<euclidian::space<Q,2> > >();
                    rv.data[2] = maybe<polygon<euclidian::space<Q,2> > >();
                    break;
                default: /* more intersections (probably two of them, more's kinda bad) */
                    {
                        line<euclidian::space<Q,2> > dv =
                            line<euclidian::space<Q,2> >(p3.data[0], p3.data[1]);

                        polygon<euclidian::space<Q,2> > p1;
                        polygon<euclidian::space<Q,2> > p2;

                        for (unsigned int i = 0; i < a.data.size(); i++)
                        {
                            const typename euclidian::space<Q,2>::vector &u = a.data[i];

                            if (u <= dv)
                            {
                                p1 = p1 + u;
                            }

                            if (u >= dv)
                            {
                                p2 = p2 + u;
                            }
                        }

                        rv.data[0] = maybe<polygon<euclidian::space<Q,2> > >(p3 + p1);
                        rv.data[1] = maybe<polygon<euclidian::space<Q,2> > >(p3 + p2);
                        rv.data[2] = p3;
                    }
                    break;
            }

            return rv;
        }
    };
};

#endif
