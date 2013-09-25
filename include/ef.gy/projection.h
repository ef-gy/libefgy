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

#if !defined(EF_GY_PROJECTION_H)
#define EF_GY_PROJECTION_H

#include <ef.gy/euclidian.h>
#include <ef.gy/matrix.h>
#include <ef.gy/tuple.h>

namespace efgy
{
    namespace geometry
    {
        template <typename Q, unsigned int d>
        class transformation
        {
            public:
                transformation ()
                    {
                        for (unsigned int i = 0; i <= d; i++)
                        {
                            for (unsigned int j = 0; j <= d; j++)
                            {
                                transformationMatrix.data[i][j] = (i == j) ? 1 : 0;
                            }
                        }
                    }

                typename euclidian::space<Q,d>::vector operator *
                    (const typename euclidian::space<Q,d>::vector &pV) const
                {
                    typename euclidian::space<Q,d>::vector rv;

                    math::matrix<Q,1,d+1> vectorMatrix;

                    for (unsigned int i = 0; i < d; i++)
                    {
                        vectorMatrix.data[0][i] = pV.data[i];
                    }

                    vectorMatrix.data[0][d] = 1;

                    vectorMatrix
                        = vectorMatrix
                        * transformationMatrix;

                    for (unsigned int i = 0; i < d; i++)
                    {
                        rv.data[i] = vectorMatrix.data[0][i] / vectorMatrix.data[0][d];
                    }

                    return rv;
                }

                math::matrix<Q,d+1,d+1> transformationMatrix;
        };

        template <typename Q, unsigned int d>
        class translation : public transformation<Q,d>
        {
            public:
                using transformation<Q,d>::transformationMatrix;

                translation(const typename euclidian::space<Q,d>::vector &pFrom)
                    : from(pFrom)
                    {
                        updateMatrix();
                    }

                /* note: this formula was obtained by generalising over the 3d translation matrix.
                 */
                void updateMatrix()
                {
                    for (unsigned int i = 0; i <= d; i++)
                    {
                        for (unsigned int j = 0; j <= d; j++)
                        {
                            if ((i == d) && (j < d))
                            {
                                transformationMatrix.data[i][j] = from.data[j];
                            }
                            else if (i == j)
                            {
                                transformationMatrix.data[i][j] = 1;
                            }
                            else
                            {
                                transformationMatrix.data[i][j] = 0;
                            }
                        }
                    }
                }

            protected:
                typename euclidian::space<Q,d>::vector from;
        };

        template <typename Q, unsigned int d>
        class lookAt : public transformation<Q,d>
        {
            public:
                using transformation<Q,d>::transformationMatrix;

                lookAt(typename euclidian::space<Q,d>::vector pFrom, typename euclidian::space<Q,d>::vector pTo)
                    : from(pFrom), to(pTo)
                    {
                        for (unsigned int i = 0; i < (d-2); i++)
                        {
                            for (unsigned int j = 0; j < d; j++)
                            {
                                orthogonalVector.data[i].data[j] = (((i+1) == j) ? 1 : 0);
                            }
                        }
                        updateMatrix();
                    }

                /* note: this formula was obtained by generalising over the 3d-to-2d and 4d-to-3d
                 *       lookat matrices.
                 */
                void updateMatrix()
                {
                    columns.data[(d-1)] = to - from;
                    columns.data[(d-1)] = euclidian::normalise<Q,d>(columns.data[(d-1)]);

                    for (int i = 0; i < (d-1); i++)
                    {
                        math::tuple<d-1,typename euclidian::space<Q,d>::vector> crossVectors;

                        for (int j = i - (d - 2), c = 0; c < (d-1); j++, c++)
                        {
                            if (j < 0)
                            {
                                crossVectors.data[c] = orthogonalVector.data[(j + (d - 2))];
                            }
                            else if (j == 0)
                            {
                                crossVectors.data[c] = columns.data[(d-1)];
                            }
                            else
                            {
                                crossVectors.data[c] = columns.data[(j-1)];
                            }
                        }

                        columns.data[i] = euclidian::getNormal<Q,d>(crossVectors);

                        if (i != (d-2))
                        {
                            columns.data[i] = euclidian::normalise<Q,d>(columns.data[i]);
                        }
                    }

                    for (unsigned int i = 0; i <= d; i++)
                    {
                        for (unsigned int j = 0; j <= d; j++)
                        {
                            if ((i < d) && (j < d))
                            {
                                transformationMatrix.data[i][j] = columns.data[j].data[i];
                            }
                            else if (i == j)
                            {
                                transformationMatrix.data[i][j] = 1;
                            }
                            else
                            {
                                transformationMatrix.data[i][j] = 0;
                            }
                        }
                    }
                }

                math::tuple<d,typename euclidian::space<Q,d>::vector> columns;

            protected:
                typename euclidian::space<Q,d>::vector from;
                typename euclidian::space<Q,d>::vector to;

                math::tuple<d-2, typename euclidian::space<Q,d>::vector> orthogonalVector;
        };

        template <typename Q, unsigned int d>
        class perspective : public transformation<Q,d>
        {
            public:
                using transformation<Q,d>::transformationMatrix;
            
                perspective(Q pEyeAngle = M_PI_4)
                    : eyeAngle(pEyeAngle)
                    {
                        updateMatrix();
                    }
            
                void updateMatrix()
                {
                    Q f = 1 / tan(eyeAngle / Q(2));

                    for (unsigned int i = 0; i <= d; i++)
                    {
                        for (unsigned int j = 0; j <= d; j++)
                        {
                            if (i == j)
                            {
                                transformationMatrix.data[i][j] = ((i >= (d-1)) ? Q(1) : f);
                            }
                            else
                            {
                                transformationMatrix.data[i][j] = 0;
                            }
                        }
                    }
                }
            
            protected:
                Q near;
                Q far;
                Q aspect;
                Q eyeAngle;
        };

        template <typename Q, unsigned int d>
        class perspectiveProjection
        {
            public:
                perspectiveProjection(typename euclidian::space<Q,d>::vector pFrom, typename euclidian::space<Q,d>::vector pTo, Q pEyeAngle = M_PI_4)
                    : from(pFrom), to(pTo), eyeAngle(pEyeAngle)
                    {
                        updateMatrix();
                    }

                void updateMatrix()
                {
                    lookAt<Q,d> lookAtTransformation(from, to);
                    translation<Q,d> translationTransformation(from * Q(-1));
                    perspective<Q,d> perspectiveTransformation(eyeAngle);

                    worldTransformation.transformationMatrix
                        = translationTransformation.transformationMatrix
                        * lookAtTransformation.transformationMatrix
                        * perspectiveTransformation.transformationMatrix
                    ;
                }

                typename euclidian::space<Q,(d-1)>::vector project
                    (const typename euclidian::space<Q,d>::vector &pP) const
                {
                    typename euclidian::space<Q,(d-1)>::vector result;

                    typename euclidian::space<Q,d>::vector R = worldTransformation * pP;

                    Q S = 1 / R.data[(d-1)];

                    for (unsigned int i = 0; i < (d-1); i++)
                    {
                        result.data[i] = S * R.data[i];
                    }

                    return result;
                }

                typename euclidian::space<Q,d>::vector from;
                typename euclidian::space<Q,d>::vector to;

            protected:
                transformation<Q,d> worldTransformation;

                const Q eyeAngle;
            };
    };
};

#endif
