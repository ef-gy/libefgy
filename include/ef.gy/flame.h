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

#if !defined(EF_GY_FLAME_H)
#define EF_GY_FLAME_H

#include <ef.gy/ifs.h>

namespace efgy
{
    namespace geometry
    {
        namespace transformation
        {
            /* These transformations are based on the 'Fractal Flame Algorithm' paper by Scott Draves
               and Eric Reckase.
               See http://flam3.com/flame_draves.pdf for the original paper. */
            template <typename Q, unsigned int d>
            class flame : public affine<Q,d>
            {
                public:
                    flame (void) {}

                    using affine<Q,d>::transformationMatrix;

                    typename euclidian::space<Q,d>::vector operator *
                        (const typename euclidian::space<Q,d>::vector &pV) const
                    {
                        const typename euclidian::space<Q,d>::vector V = affine<Q,d>(*this) * pV;
                        typename euclidian::space<Q,d>::vector rv = V * coefficient[0];

                        for (unsigned int i = 1; i < coefficients; i++)
                        {
                            rv = rv + apply(i, V);
                        }

                        return rv;
                    }

                    static const unsigned int coefficients = 7;
                    Q coefficient[coefficients];

                protected:
                    typename euclidian::space<Q,d>::vector apply
                        (unsigned int f, const typename euclidian::space<Q,d>::vector &V) const
                    {
                        typename euclidian::space<Q,d>::vector rv;

                        if (coefficient[f] <= Q(0.))
                        {
                            return rv;
                        }

                        switch (f)
                        {
                            case 0:
                                rv = V;
                                break;
                            case 1:
                                for (unsigned int i = 0; i < d; i++)
                                {
                                    rv.data[i] = sin(V.data[i]);
                                }
                                break;
                            case 2:
                                rv = V * Q(1)/euclidian::lengthSquared<Q,d>(V);
                                break;
                            case 3:
                            {
                                const Q rsq = euclidian::lengthSquared<Q,d>(V);
                                const Q sinrsq = sin(rsq);
                                const Q cosrsq = cos(rsq);
                                for (unsigned int i = 0; i < d; i++)
                                {
                                    if ((i % 2 == 0) && (i < (d-1)))
                                    {
                                        rv.data[i] = rv.data[i] * sinrsq - rv.data[(i+1)] * cosrsq;
                                    }
                                    else
                                    {
                                        rv.data[i] = rv.data[(i-1)] * cosrsq + rv.data[i] * sinrsq;
                                    }
                                }
                            }
                                break;
                            case 4:
                                rv = V;
                                rv.data[0] = (V.data[0]-V.data[1])*(V.data[0]+V.data[1]);
                                rv.data[1] = Q(2) * V.data[0] * V.data[1];
                                rv = rv * Q(1)/sqrt(euclidian::lengthSquared<Q,d>(V));
                                break;
                            case 5:
                            {
                                const Q theta = atan(V.data[0]/V.data[1]);
                                rv = V;
                                rv.data[0] = theta/Q(M_PI);
                                rv.data[1] = sqrt(euclidian::lengthSquared<Q,d>(V)) - Q(1);
                            }
                                break;
                            case 6:
                            {
                                const Q theta = atan(V.data[0]/V.data[1]);
                                const Q r = sqrt(euclidian::lengthSquared<Q,d>(V)) - Q(1);
                                const Q sintpr = sin(theta + r);
                                const Q costmr = cos(theta - r);
                                for (unsigned int i = 0; i < d; i++)
                                {
                                    if (i % 2 == 0)
                                    {
                                        rv.data[i] = sintpr;
                                    }
                                    else
                                    {
                                        rv.data[i] = costmr;
                                    }
                                }
                                rv = rv * r;
                            }
                                break;
                        }

                        return rv * coefficient[f];
                    }
            };

            template <typename Q, unsigned int d, unsigned int od = d>
            class randomFlame : public flame<Q,d>
            {
                public:
                    randomFlame(const parameters<Q> &pParameter)
                        {
                            transformationMatrix = randomAffine<Q,d,od>(pParameter).transformationMatrix;

                            for (unsigned int i = 0; i < coefficients; i++)
                            {
                                coefficient[i] = Q(std::rand()%10000)/Q(10000);
                            }

                            for (unsigned int nonzero = pParameter.flameCoefficients + 1;
                                 nonzero > pParameter.flameCoefficients; )
                            {
                                nonzero = 0;
                                for (unsigned int i = 0; i < coefficients; i++)
                                {
                                    if (coefficient[i] > Q(0.))
                                    {
                                        nonzero++;
                                    }
                                    else if (coefficient[i] < Q(0.))
                                    {
                                        coefficient[i] = Q(0.);
                                    }
                                }
                                
                                if (nonzero > pParameter.flameCoefficients)
                                {
                                    coefficient[(std::rand()%coefficients)] = Q(0.);
                                }
                            }
                            
                            Q coefficientsum = coefficient[0];

                            for (unsigned int i = 1; i < coefficients; i++)
                            {
                                coefficientsum = coefficientsum + coefficient[i];
                            }
                            
                            for (unsigned int i = 0; i < coefficients; i++)
                            {
                                coefficient[i] = coefficient[i] / coefficientsum;
                            }
                        }

                    using flame<Q,d>::transformationMatrix;
                    using flame<Q,d>::coefficient;
                    using flame<Q,d>::coefficients;
            };
        };

        namespace flame
        {
            template <typename Q, unsigned int od, typename render, unsigned int d = od>
            class random : public ifs<Q,od,render,d,cube,2,transformation::flame>
            {
                public:
                    typedef ifs<Q,od,render,d,cube,2,transformation::flame> parent;

                    random(render &pRenderer, const parameters<Q> &pParameter, const Q &pMultiplier = 1)
                        : parent(pRenderer, pParameter, pMultiplier)
                        {
                            calculateObject();
                        }

                        void calculateObject (void)
                        {
                            functions.clear();

                            std::srand(parameter.seed);

                            const unsigned int nfunctions = parameter.functions;
                        
                            for (unsigned int i = 0; i < nfunctions; i++)
                            {
                                functions.push_back (transformation::randomFlame<Q,d,od>(parameter));
                            }
                        
                            parent::calculateObject();
                        }

                    using parent::parameter;
                    using parent::renderSolid;
                    using parent::renderer;
                    using parent::lines;
                    using parent::faces;

                    using parent::modelDimensionMinimum;
                    static const unsigned int modelDimensionMaximum = d;
                    using parent::renderDimensionMinimum;
                    using parent::renderDimensionMaximum;

                    using parent::faceVertices;

                    using parent::functions;
                    using parent::calculateObject;

                    static unsigned int depth (void) { return od; }
                    static unsigned int renderDepth (void) { return d; }
                    static const char *id (void) { return "random-flame"; }
            };
        };
    };
};

#endif
