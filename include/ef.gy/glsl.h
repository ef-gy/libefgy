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
        /**\brief GLSL synthesiser
         *
         * Contains templates to generate GLSL shaders in different dialects,
         * depending on the GLSL version supported by the target graphics
         * hardware.
         */
        namespace glsl
        {
            enum type
            {
                gv_attribute,
                gv_output,
                gv_varying,
                gv_uniform
            };

            enum version
            {
                ver_100,
                ver_auto
            };

            template <enum type T>
            class variable
            {
                public:
                    std::string name;
                    std::string type;
                    std::string precision;
            };

            template <enum version V>
            class shader
            {
                public:
                    std::vector<variable<gv_attribute>> attribute;
                    std::vector<variable<gv_output>>    output;
                    std::vector<variable<gv_varying>>   varying;
                    std::vector<variable<gv_uniform>>   uniform;

                    std::string main;
            };

            template <typename C>
            std::basic_ostream<C> &operator <<
                (const std::basic_ostream<C> &out, const shader<ver_auto> &s)
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
                const char *ver = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
                if (ver)
                {
                }
#endif
            }

            template <typename C>
            std::basic_ostream<C> &operator <<
                (const std::basic_ostream<C> &out, const shader<ver_100> &s)
            {
                out << "#version 100\n";

                for (const variable<gv_attribute> &v : s.attribute)
                {
                    out << "attribute " << v.precision << " " << v.type << " " << v.name << "\n";
                }
                for (const variable<gv_varying> &v : s.varying)
                {
                    out << "varying " << v.precision << " " << v.type << " " << v.name << "\n";
                }
                for (const variable<gv_uniform> &v : s.uniform)
                {
                    out << "uniform " << v.precision << " " << v.type << " " << v.name << "\n";
                }

                return out << "void main() {"
                           << s.main
                           << "}\n";
            }
        };
    };
};

#endif
