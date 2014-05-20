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

#if !defined(EF_GY_PROJECTION_H)
#define EF_GY_PROJECTION_H

#include <ef.gy/transformation.h>
#include <array>
#include <cmath>

namespace efgy
{
    namespace geometry
    {
        template <typename Q, unsigned int d>
        class lookAt : public transformation::affine<Q,d>
        {
            public:
                lookAt(math::vector<Q,d> pFrom, math::vector<Q,d> pTo)
                    : from(pFrom), to(pTo)
                    {
                        for (unsigned int i = 0; i < (d-2); i++)
                        {
                            for (unsigned int j = 0; j < d; j++)
                            {
                                orthogonalVector[i][j] = (((i+1) == j) ? 1 : 0);
                            }
                        }
                        updateMatrix();
                    }

                /* note: this formula was obtained by generalising over the 3d-to-2d and 4d-to-3d
                 *       lookat matrices.
                 */
                void updateMatrix (void)
                {
                    columns[(d-1)] = to - from;
                    columns[(d-1)] = math::normalise(columns[(d-1)]);

                    for (int i = 0; i < (d-1); i++)
                    {
                        std::array<math::vector<Q,d>,d-1> crossVectors;

                        for (int j = i - (d - 2), c = 0; c < (d-1); j++, c++)
                        {
                            if (j < 0)
                            {
                                crossVectors[c] = orthogonalVector[(j + (d - 2))];
                            }
                            else if (j == 0)
                            {
                                crossVectors[c] = columns[(d-1)];
                            }
                            else
                            {
                                crossVectors[c] = columns[(j-1)];
                            }
                        }

                        columns[i] = math::normal(crossVectors);

                        if (i != (d-2))
                        {
                            columns[i] = math::normalise(columns[i]);
                        }
                    }

                    for (unsigned int i = 0; i <= d; i++)
                    {
                        for (unsigned int j = 0; j <= d; j++)
                        {
                            if ((i < d) && (j < d))
                            {
                                transformationMatrix[i][j] = columns[j][i];
                            }
                            else if (i == j)
                            {
                                transformationMatrix[i][j] = 1;
                            }
                            else
                            {
                                transformationMatrix[i][j] = 0;
                            }
                        }
                    }
                }

                std::array<math::vector<Q,d>,d> columns;

                using transformation::affine<Q,d>::transformationMatrix;

            protected:
                math::vector<Q,d> from;
                math::vector<Q,d> to;

                std::array<math::vector<Q,d>,d-2> orthogonalVector;
        };

        template <typename Q, unsigned int d>
        class perspective : public transformation::affine<Q,d>
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
                        transformationMatrix[0][0] = f/aspect;
                        transformationMatrix[1][1] = f;
                        transformationMatrix[2][2] = (far+near)/(near-far);
                        transformationMatrix[3][2] = Q(-2)*far*near/(near-far);
                        transformationMatrix[2][3] = Q(-1);
                        transformationMatrix[3][3] = Q(0);
                    }
                    else for (unsigned int i = 0; i <= d; i++)
                    {
                        for (unsigned int j = 0; j <= d; j++)
                        {
                            if (i == j)
                            {
                                transformationMatrix[i][j] = ((i >= (d-1)) ? Q(1) : f);
                            }
                            else
                            {
                                transformationMatrix[i][j] = 0;
                            }
                        }
                    }
                }

                using transformation::affine<Q,d>::transformationMatrix;

            protected:
                Q near;
                Q far;
                Q aspect;
                Q eyeAngle;
        };

        template <typename Q, unsigned int d>
        class projection : public transformation::projective<Q,d>
        {
            public:
                projection(math::vector<Q,d> pFrom, math::vector<Q,d> pTo, const Q &pEyeAngle = M_PI_4, const Q &pAspect = 1.8, const bool &initialiseMatrix = true)
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
                    transformation::translation<Q,d> translationTransformation(from * Q(-1));
                    perspective<Q,d> perspectiveTransformation(eyeAngle, aspect);

                    this->transformationMatrix =
                      ( translationTransformation
                      * lookAtTransformation
                      * perspectiveTransformation ).transformationMatrix;
                }

                math::vector<Q,d> from;
                math::vector<Q,d> to;
                const Q eyeAngle;
                Q aspect;

                using transformation::affine<Q,d>::transformationMatrix;
        };
    };
};

#endif
