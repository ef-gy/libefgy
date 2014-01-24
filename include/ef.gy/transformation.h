/**\file
 * \brief Handles vector transformations.
 * 
 * Contains templates for transformations of vector space elements.
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
 * \see Project Source Code: http://git.becquerel.org/jyujin/libefgy.git
 */

#if !defined(EF_GY_TRANSFORMATION_H)
#define EF_GY_TRANSFORMATION_H

#include <ef.gy/euclidian.h>
#include <ef.gy/matrix.h>

namespace efgy
{
    namespace geometry
    {
        /**\brief Contains vector transformation templates
         *
         * This namespace groups all templates that transform vector space
         * elements to other vector space elements. Or, in non-mathemese:
         * things that turn 3D coordinates to 2D coordinates, or things that
         * move stuff around in 3D space. Except it's not just 3D and 2D
         * vectors that we'd want to work with.
         */
        namespace transformation
        {
            template <typename Q, unsigned int d> class projective;
            
            /**\brief Template for linear maps on the vector space Q^d
             *
             * Handles linear maps, or endomorphisms, on Q^d. Maps are
             * described via their transformation matrices, using the
             * standard basis on Q^d.
             *
             * \tparam Q The data type that describes the underlying field of 
             * the vector space.
             *
             * \tparam d The dimension of the vector space.
             */
            template <typename Q, unsigned int d>
            class linear
            {
                public:
                    /**\brief Constructor for the identity  map
                     *
                     * Constructs a transformation whose matrix is 
                     * the identity matrix.
                     */
                    linear ()
                        {
                            for (unsigned int i = 0; i < d; i++)
                            {
                                for (unsigned int j = 0; j < d; j++)
                                {
                                    matrix[i][j] = (i == j) ? 1 : 0;
                                }
                            }
                        }
                    /**\brief Applies a transformation to a vector.
                     *
                     * Applies a transformation to a vector by 
                     * multiplying the transformation matrix to it.
                     *
                     * \param vector a vector from Q^d
                     *
                     * \returns The transformed vector, which is also
                     * in Q^d.
                     */
                    math::vector<Q,d> operator *
                        (const math::vector<Q,d> &pV) const
                    {
                        math::vector<Q,d> rv;

                        math::matrix<Q,1,d> vectorMatrix;

                        for (unsigned int i = 0; i < d; i++)
                        {
                            vectorMatrix[0][i] = pV.data[i];
                        }

                        vectorMatrix
                            = vectorMatrix
                            * matrix;

                        for (unsigned int i = 0; i < d; i++)
                        {
                            rv.data[i] = vectorMatrix[0][i];
                        }

                        return rv;
                    }
                    /*\brief Composes two linear maps.
                     *
                     * Composes two linear maps on Q^d by multiplying 
                     * their transformation matrices.
                     *
                     * \param pB The linear map object that will be composed
                     * with the current object.
                     *
                     * \returns A linear map that is the composition of 
                     * the current object and pB.
                     */
                    linear operator *
                        (const linear &pB) const
                    {
                        linear t;
                        t.matrix = this->matrix * pB.matrix;
                        return t;
                    }
                    
                    /*\brief Composes a linear and a projective map.
                     *
                     * Composes a linear and a projective transformation.
                     *
                     * \param pB a projective transformation.
                     *
                     * \returns A projective transformation that is the 
                     * composition of pB and the current object.
                     */
                    projective<Q,d> operator *
                        (const projective<Q,d> &pB) const
                    {
                        return pB * (*this);
                    }

                    /*\brief The transformation matrix.
                     *
                     * The transformation matrix of a linear map.
                     */
                    math::matrix<Q,d,d> matrix;
            };

            /* \brief The identity map on Q^d.
             *
             * Wrapper class for the identity map on Q^d.
             *
             * \tparam Q The underlying field of the vector space.
             *
             * \tparam d The dimension of the vector space
             *
             * \bug Assignments to the transformation matrix of objects
             * of this class are possible, so an object to this class
             * can have a non-identity transformation matrix (and thus
             * not behave like an identity map). 
             */ 
            template <typename Q, unsigned int d>
            class identity : public linear<Q,d>
            {
                public:
                    identity()
                        : linear<Q,d>()
                        {}
            };


            template <typename Q, unsigned int d>
            class affine
            {
                public:
                    affine ()
                        {
                            for (unsigned int i = 0; i <= d; i++)
                            {
                                for (unsigned int j = 0; j <= d; j++)
                                {
                                    transformationMatrix[i][j] = (i == j) ? 1 : 0;
                                }
                            }
                        }

                    affine (const linear<Q,d> &L)
                        {
                            for (unsigned int i = 0; i < d; i++)
                            {
                                for (unsigned int j = 0; j < d; j++)
                                {
                                    transformationMatrix[i][j] = L.data[i][j];
                                }
                            }
                            for (unsigned int i = 0; i < d; i++)
                            {
                                transformationMatrix[i][d] = Q(0);
                                transformationMatrix[d][i] = Q(0);
                            }
                            transformationMatrix[d][d] = Q(1);
                        }

                    math::vector<Q,d> operator *
                        (const math::vector<Q,d> &pV) const
                    {
                        math::vector<Q,d> rv;

                        math::matrix<Q,1,d+1> vectorMatrix;

                        for (unsigned int i = 0; i < d; i++)
                        {
                            vectorMatrix[0][i] = pV[i];
                        }

                        vectorMatrix[0][d] = 1;

                        vectorMatrix
                            = vectorMatrix
                            * transformationMatrix;

                        for (unsigned int i = 0; i < d; i++)
                        {
                            rv[i] = vectorMatrix[0][i] / vectorMatrix[0][d];
                        }

                        return rv;
                    }

                    affine operator *
                        (const affine &pB) const
                    {
                        affine t;
                        t.transformationMatrix = this->transformationMatrix * pB.transformationMatrix;
                        return t;
                    }

                    projective<Q,d> operator *
                        (const projective<Q,d> &pB) const
                    {
                        projective<Q,d> t;
                        t.transformationMatrix = this->transformationMatrix * pB.transformationMatrix;
                        return t;
                    }

                    math::matrix<Q,d+1,d+1> transformationMatrix;
            };

            template <typename Q, unsigned int d>
            class projective : public affine<Q,d>
            {
                public:
                    projective ()
                        : affine<Q,d>()
                        {}

                    math::vector<Q,d-1> operator *
                        (const math::vector<Q,d> &pP) const
                    {
                        math::vector<Q,d-1> result;
                    
                        math::vector<Q,d> R = affine<Q,d>(*this) * pP;

                        for (unsigned int i = 0; i < (d-1); i++)
                        {
                            result[i] = R[i] / R[(d-1)];
                        }
                    
                        return result;
                    }

                    projective operator *
                        (const affine<Q,d> &pB) const
                    {
                        projective<Q,d> t;
                        t.transformationMatrix = this->transformationMatrix * pB.transformationMatrix;
                        return t;
                    }

                    using affine<Q,d>::transformationMatrix;
            };

            template <typename Q, unsigned int d>
            class scale : public affine<Q,d>
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
                                    transformationMatrix[i][j] = i == d ? Q(1) / targetScale : Q(1);
                                }
                                else
                                {
                                    transformationMatrix[i][j] = 0;
                                }
                            }
                        }
                    }

                    using affine<Q,d>::transformationMatrix;

                protected:
                    const Q &targetScale;
            };

            template <typename Q, unsigned int d>
            class rotation : public affine<Q,d>
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
                                    transformationMatrix[i][j] =  cos(angle);
                                }
                                else if ((i == axis1) && (j == axis2))
                                {
                                    transformationMatrix[i][j] = -sin(angle);
                                }
                                else if ((i == axis2) && (j == axis2))
                                {
                                    transformationMatrix[i][j] =  cos(angle);
                                }
                                else if ((i == axis2) && (j == axis1))
                                {
                                    transformationMatrix[i][j] =  sin(angle);
                                }
                                else if (i == j)
                                {
                                    transformationMatrix[i][j] = Q(1);
                                }
                                else
                                {
                                    transformationMatrix[i][j] = 0;
                                }
                            }
                        }
                        
                        if ((axis1 + axis2 + d + 1) % 2 == 1)
                        {
                            transformationMatrix = math::transpose(transformationMatrix);
                        }
                    }

                    using affine<Q,d>::transformationMatrix;

                protected:
                    const Q &angle;
                    const unsigned int &axis1;
                    const unsigned int &axis2;
            };

            template <typename Q, unsigned int d>
            class translation : public affine<Q,d>
            {
                public:
                    translation(const math::vector<Q,d> &pFrom)
                        : from(pFrom)
                        {
                            updateMatrix();
                        }

                    void updateMatrix (void)
                    {
                        if (d == 3)
                        {
                            transformationMatrix[3][0] = from[0];
                            transformationMatrix[3][1] = from[1];
                            transformationMatrix[3][2] = from[2];
                        }
                        else for (unsigned int i = 0; i <= d; i++)
                        {
                            for (unsigned int j = 0; j <= d; j++)
                            {
                                if ((i == d) && (j < d))
                                {
                                    transformationMatrix[i][j] = from[j];
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

                    using affine<Q,d>::transformationMatrix;

                protected:
                    math::vector<Q,d> from;
            };
        };
    };
};

#endif
