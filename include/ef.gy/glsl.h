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
#include <regex>
#include <array>

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
            /**\brief Shader variable type
             *
             * Determines the type of a variable.
             */
            enum type
            {
                gv_attribute, /**< Input variable from vertex data. */
                gv_varying,   /**< Input or output varying variable. */
                gv_uniform    /**< Input uniform variable. */
            };

            /**\brief GLSL shader version
             *
             * The general layout of GLSL shaders varies significantly between
             * versions; it is thus necessary to specify which shader version to
             * produce.
             */
            enum version
            {
                ver_100, /**< Version 100 shaders; also used in WebGL and OpenGL
                          *   ES.
                          */
                ver_auto /**< Automatically determine the version to use; upon
                          *   Writing out a shader to a stream, the supported
                          *   shaders are queried from the current OpenGL
                          *   runtime to determine an appropriate type of shader
                          *   that is supported by OpenGL.
                          */
            };

            template <enum type T>
            class variable
            {
                public:
                    constexpr variable
                        (const char *pName, const char *pType = "float", const char *pPrecision = "")
                        : name(pName), type(pType), precision(pPrecision)
                        {}

                    const char *name;
                    const char *type;
                    const char *precision;
            };

            /*
             *
             * \tparam V GLSL shader version to produce.
             */
            template <enum version V = ver_auto>
            class shader
            {
                public:
                    shader(const std::string &pMain = "",
                           const std::vector<variable<gv_attribute>> &pAttribute = {},
                           const std::vector<variable<gv_varying>> &pVarying = {},
                           const std::vector<variable<gv_uniform>> &pUniform = {})
                        : main(pMain),
                          attribute(pAttribute),
                          varying(pVarying),
                          uniform(pUniform) {}

                    shader(const std::string &pMain,
                           const std::vector<variable<gv_varying>> &pVarying,
                           const std::vector<variable<gv_uniform>> &pUniform = {})
                        : main(pMain),
                          attribute(),
                          varying(pVarying),
                          uniform(pUniform) {}

                    template <enum version R>
                    shader(const shader<R> &s)
                        : attribute(s.attribute),
                          varying(s.varying),
                          uniform(s.uniform),
                          main(s.main) {}

                    std::vector<variable<gv_attribute>> attribute;
                    std::vector<variable<gv_varying>>   varying;
                    std::vector<variable<gv_uniform>>   uniform;

                    std::string main;
            };

            template <typename C>
            std::basic_ostream<C> &operator <<
                (std::basic_ostream<C> &out, const shader<ver_auto> &s)
            {
#if defined(GL_NUM_SHADING_LANGUAGE_VERSIONS) && 0
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
                    std::string sver = ver;
                    std::regex v100("1\\.0.*");

                    if (std::regex_match(sver, v100))
                    {
                        return out << shader<ver_100>(s);
                    }
                }
#endif
                return out << shader<ver_100>(s);
            }

            template <typename C>
            std::basic_ostream<C> &operator <<
                (std::basic_ostream<C> &out, const shader<ver_100> &s)
            {
                out << "#version 100\n";

                for (const variable<gv_attribute> &v : s.attribute)
                {
                    out << "attribute " << v.precision << " " << v.type << " " << v.name << ";\n";
                }
                for (const variable<gv_varying> &v : s.varying)
                {
                    out << "varying " << v.precision << " " << v.type << " " << v.name << ";\n";
                }
                for (const variable<gv_uniform> &v : s.uniform)
                {
                    out << "uniform " << v.precision << " " << v.type << " " << v.name << ";\n";
                }

                return out << "void main() {"
                           << s.main
                           << "}\n";
            }
        };
    };
};

#endif
