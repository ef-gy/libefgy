/**\file
 * \brief GLSL synthesiser
 *
 * Contains code to synthesise GLSL source code to feed to the graphics card.
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

#if !defined(EF_GY_GLSL_H)
#define EF_GY_GLSL_H

//#include <ef.gy/tracer.h>
#include <ef.gy/opengl.h>
#include <map>
#include <string>

namespace efgy
{
    namespace opengl
    {
        namespace glsl
        {
            enum type
            {
                gv_input,
                gv_output,
                gv_varying,
                gv_uniform
            };

            template <enum type T>
            class variable
            {
                public:
                    static const enum type type = T;
                    std::string name;
            };

            class shader
            {
                public:
                    shader(void)
                    {
#if defined(GL_NUM_SHADING_LANGUAGE_VERSIONS)
                        GLint versions;
                        glGetIntegerv(GL_NUM_SHADING_LANGUAGE_VERSIONS, &versions);

                        for (unsigned int i = 0; i < versions; i++)
                        {
                            const char *ver = (const char *)glGetStringi(GL_SHADING_LANGUAGE_VERSION, i);
                            
                            std::cerr << ver;
                        }
#else
                        version = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
#endif
                        std::cerr << version << "\n";
                    }

                    std::string version;
            };
        };
    };
};

#endif
