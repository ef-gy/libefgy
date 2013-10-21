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
        };

        namespace flame
        {
            template <typename Q, unsigned int od, typename render, unsigned int d = od>
            class random : public ifs<Q,od,render,d,cube,2,transformation::affine>
            {
                public:
                    typedef ifs<Q,od,render,d,cube,2,transformation::affine> parent;

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
                            }
                        
                            parent::calculateObject();
                        }

                    using parent::parameter;
                    using parent::renderWireframe;
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
