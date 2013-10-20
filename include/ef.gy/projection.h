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

                transformation operator *
                    (const transformation &pB) const
                {
                    transformation t;
                    t.transformationMatrix = this->transformationMatrix * pB.transformationMatrix;
                    return t;
                }

                typename euclidian::space<Q,(d-1)>::vector project
                    (const typename euclidian::space<Q,d>::vector &pP) const
                {
                    typename euclidian::space<Q,(d-1)>::vector result;
                
                    typename euclidian::space<Q,d>::vector R = transformation<Q,d>(*this) * pP;
                
                    Q S = 1 / R.data[(d-1)];
                
                    for (unsigned int i = 0; i < (d-1); i++)
                    {
                        result.data[i] = S * R.data[i];
                    }
                
                    return result;
                }
            
                math::matrix<Q,d+1,d+1> transformationMatrix;
        };

        template <typename Q, unsigned int d>
        class scale : public transformation<Q,d>
        {
            public:
                scale(const Q &pScale)
                    : targetScale(pScale)
                    {
                        updateMatrix();
                    }

                void updateMatrix (void)
                {
                    for (unsigned int i = 0; i <= d; i++)
                    {
                        for (unsigned int j = 0; j <= d; j++)
                        {
                            if (i == j)
                            {
                                transformationMatrix.data[i][j] = i == d ? Q(1) / targetScale : Q(1);
                            }
                            else
                            {
                                transformationMatrix.data[i][j] = 0;
                            }
                        }
                    }
                }

                using transformation<Q,d>::transformationMatrix;

            protected:
                const Q &targetScale;
        };

        template <typename Q, unsigned int d>
        class rotation : public transformation<Q,d>
        {
            public:
                rotation(const Q &pAngle, const unsigned int &pAxis1, const unsigned int &pAxis2)
                    : angle(pAngle), axis1(pAxis1), axis2(pAxis2)
                    {
                        updateMatrix();
                    }
            
                void updateMatrix (void)
                {
                    for (unsigned int i = 0; i <= d; i++)
                    {
                        for (unsigned int j = 0; j <= d; j++)
                        {
                            if ((i == axis1) && (j == axis1))
                            {
                                transformationMatrix.data[i][j] =  cos(angle);
                            }
                            else if ((i == axis1) && (j == axis2))
                            {
                                transformationMatrix.data[i][j] = -sin(angle);
                            }
                            else if ((i == axis2) && (j == axis2))
                            {
                                transformationMatrix.data[i][j] =  cos(angle);
                            }
                            else if ((i == axis2) && (j == axis1))
                            {
                                transformationMatrix.data[i][j] =  sin(angle);
                            }
                            else if (i == j)
                            {
                                transformationMatrix.data[i][j] = Q(1);
                            }
                            else
                            {
                                transformationMatrix.data[i][j] = 0;
                            }
                        }
                    }
                    
                    if ((axis1 + axis2) % 2 == 1)
                    {
                        transformationMatrix = math::transpose(transformationMatrix);
                    }
                }

                using transformation<Q,d>::transformationMatrix;

            protected:
                const Q &angle;
                const unsigned int &axis1;
                const unsigned int &axis2;
        };

        template <typename Q, unsigned int d>
        class translation : public transformation<Q,d>
        {
            public:
                translation(const typename euclidian::space<Q,d>::vector &pFrom)
                    : from(pFrom)
                    {
                        updateMatrix();
                    }

                /* note: this formula was obtained by generalising over the 3d translation matrix.
                 */
                void updateMatrix (void)
                {
                    if (d == 3)
                    {
                        transformationMatrix.data[3][0] = from.data[0];
                        transformationMatrix.data[3][1] = from.data[1];
                        transformationMatrix.data[3][2] = from.data[2];
                    }
                    else for (unsigned int i = 0; i <= d; i++)
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

                using transformation<Q,d>::transformationMatrix;

            protected:
                typename euclidian::space<Q,d>::vector from;
        };

        template <typename Q, unsigned int d>
        class lookAt : public transformation<Q,d>
        {
            public:
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
                void updateMatrix (void)
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

                using transformation<Q,d>::transformationMatrix;

            protected:
                typename euclidian::space<Q,d>::vector from;
                typename euclidian::space<Q,d>::vector to;

                math::tuple<d-2, typename euclidian::space<Q,d>::vector> orthogonalVector;
        };

        template <typename Q, unsigned int d>
        class perspective : public transformation<Q,d>
        {
            public:
                perspective(Q pEyeAngle = M_PI_4, const Q &pAspect = 1.8, const Q &pNear = -0.1, const Q &pFar = -100)
                    : eyeAngle(pEyeAngle),
                      aspect(pAspect),
                      near(pNear),
                      far(pFar)
                    {
                        updateMatrix();
                    }
            
                void updateMatrix (void)
                {
                    Q f = Q(1) / tan(eyeAngle / Q(2));

                    if (d == 3)
                    {
                        transformationMatrix.data[0][0] = f/aspect;
                        transformationMatrix.data[1][1] = f;
                        transformationMatrix.data[2][2] = (far+near)/(near-far);
                        transformationMatrix.data[3][2] = Q(-2)*far*near/(near-far);
                        transformationMatrix.data[2][3] = Q(-1);
                        transformationMatrix.data[3][3] = Q(0);
                    }
                    else for (unsigned int i = 0; i <= d; i++)
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

                using transformation<Q,d>::transformationMatrix;

            protected:
                Q near;
                Q far;
                Q aspect;
                Q eyeAngle;
        };

        template <typename Q, unsigned int d>
        class projection : public transformation<Q,d>
        {
            public:
                projection(typename euclidian::space<Q,d>::vector pFrom, typename euclidian::space<Q,d>::vector pTo, const Q &pEyeAngle = M_PI_4, const Q &pAspect = 1.8, const bool &initialiseMatrix = true)
                    : from(pFrom), to(pTo), eyeAngle(pEyeAngle), aspect(pAspect)
                    {
                        if (initialiseMatrix)
                        {
                            updateMatrix();
                        }
                    }

                void updateMatrix (void)
                {
                    lookAt<Q,d> lookAtTransformation(from * Q(-1), to);
                    translation<Q,d> translationTransformation(from * Q(-1));
                    perspective<Q,d> perspectiveTransformation(eyeAngle, aspect);
                        
                    this->transformationMatrix =
                      ( translationTransformation
                      * lookAtTransformation
                      * perspectiveTransformation ).transformationMatrix;
                }

                typename euclidian::space<Q,d>::vector from;
                typename euclidian::space<Q,d>::vector to;
                const Q eyeAngle;
                Q aspect;

                using transformation<Q,d>::transformationMatrix;
        };
    };
};

#endif
