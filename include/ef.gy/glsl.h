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
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 */

#if !defined(EF_GY_GLSL_H)
#define EF_GY_GLSL_H

//#include <ef.gy/tracer.h>
#include <map>
#include <string>
#include <regex>

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

            /**\brief GLSL variable specification
             *
             * Contains all the data related to a GLSL variable so that it can
             * be written to a stream and used in a shader programme.
             */
            template <enum type T>
            class variable
            {
                public:
                    /**\brief Construct with data
                     *
                     * Initialises a new variable specification with the name,
                     * type and precision to use.
                     *
                     * \param[in] pName      Name of the variable.
                     * \param[in] pType      The GLSL type of the variable.
                     * \param[in] pPrecision The precision to use.
                     * \param[in] pElements  Number of array elements.
                     */
                    constexpr variable
                        (const std::string &pName, const char *pType = "float", const char *pPrecision = "", unsigned int pElements = 1)
                        : name(pName), type(pType), precision(pPrecision), elements(pElements)
                        {}

                    /**\brief Variable name
                     *
                     * Name of the variable, as used in the source code.
                     */
                    const std::string name;

                    /**\brief Variable type
                     *
                     * GLSL type string for the variable.
                     */
                    const char *type;

                    /**\brief Variable precision
                     *
                     * The floating-point precision for the variable.
                     */
                    const char *precision;

                    /**\brief Number of array elements
                     *
                     * For scalars, this is 1. For arrays, this is the number
                     * of elements in that array.
                     */
                    unsigned int elements;
            };

            /**\brief GLSL shader
             *
             * This is the base class for GLSL shaders; to specify a shader, you
             * should derive from this class and design the derived version to
             * only take a single version template parameter, and make sure the
             * constructor does not take any arguments. This is how the OpenGL
             * code uses them.
             *
             * \tparam V GLSL shader version to produce.
             */
            template <enum version V = ver_auto>
            class shader
            {
                public:
                    /**\brief Construct with shader data
                     *
                     * Initialises a new GLSL shader with all the details that
                     * it consists of specified separately.
                     *
                     * \param[in] pMain      GLSL main() function source.
                     * \param[in] pAttribute List of input vertex attributes.
                     * \param[in] pVarying   List of input/output varyings.
                     * \param[in] pUniform   List of input uniforms.
                     */
                    shader(const std::string &pMain = "",
                           const std::vector<variable<gv_attribute>> &pAttribute = {},
                           const std::vector<variable<gv_varying>> &pVarying = {},
                           const std::vector<variable<gv_uniform>> &pUniform = {})
                        : main(pMain),
                          attribute(pAttribute),
                          varying(pVarying),
                          uniform(pUniform) {}

                    /**\brief Construct without vertex attributes
                     *
                     * Initialises a new GLSL shader with all the details that
                     * it consists of specified separately, except for the list
                     * of vertex attributes. This is useful for specifying
                     * fragment shaders, which do not use vertex attributes.
                     *
                     * \param[in] pMain      GLSL main() function source.
                     * \param[in] pVarying   List of input/output varyings.
                     * \param[in] pUniform   List of input uniforms.
                     */
                    shader(const std::string &pMain,
                           const std::vector<variable<gv_varying>> &pVarying,
                           const std::vector<variable<gv_uniform>> &pUniform = {})
                        : main(pMain),
                          attribute(),
                          varying(pVarying),
                          uniform(pUniform) {}

                    /**\brief Template copy constructor
                     *
                     * Provides a copy constructor that can copy shaders of
                     * arbitrary versions to a new instance.
                     *
                     * \tparam R The version of the shader to copy.
                     *
                     * \param[in] s The shader to copy.
                     */
                    template <enum version R>
                    shader(const shader<R> &s)
                        : attribute(s.attribute),
                          varying(s.varying),
                          uniform(s.uniform),
                          main(s.main) {}

                    /**\brief Shader vertex attributes
                     *
                     * Contains all the vertex attributes that the shader uses.
                     */
                    std::vector<variable<gv_attribute>> attribute;

                    /**\brief Shader varying variables
                     *
                     * Contains all the shader's varying input/output variables.
                     */
                    std::vector<variable<gv_varying>> varying;

                    /**\brief Shader uniforms
                     *
                     * Contains all the shader's uniform input variables.
                     */
                    std::vector<variable<gv_uniform>> uniform;

                    /**\brief Main GLSL shader sources
                     *
                     * Contains the sources of the shader's main() function.
                     */
                    std::string main;
            };

            /**\brief Write shader to stream and automatically detect version
             *
             * Tries to guess the GLSL version to use by querying the supported
             * shader versions from the OpenGL runtime and producing output
             * accordingly.
             *
             * \tparam C The character type of the stream.
             *
             * \param[out] out The stream to write to.
             * \param[in]  s   The shader to write to the stream.
             *
             * \returns The 'out' stream after writing to it.
             */
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

            /**\brief Write version 100 shader to stream
             *
             * This will turn the given shader data into a version 100 GLSL
             * shader and write it to the given output stream.
             *
             * \tparam C The character type of the stream.
             *
             * \param[out] out The stream to write to.
             * \param[in]  s   The shader to write to the stream.
             *
             * \returns The 'out' stream after writing to it.
             */
            template <typename C>
            std::basic_ostream<C> &operator <<
                (std::basic_ostream<C> &out, const shader<ver_100> &s)
            {
                out << "#version 100\n";

                for (const variable<gv_attribute> &v : s.attribute)
                {
                    out << "attribute " << v.precision << " " << v.type << " " << v.name;
                    if (v.elements > 1)
                    {
                        out << "[" << v.elements << "]";
                    }
                    out << ";\n";
                }
                for (const variable<gv_varying> &v : s.varying)
                {
                    out << "varying " << v.precision << " " << v.type << " " << v.name;
                    if (v.elements > 1)
                    {
                        out << "[" << v.elements << "]";
                    }
                    out << ";\n";
                }
                for (const variable<gv_uniform> &v : s.uniform)
                {
                    out << "uniform " << v.precision << " " << v.type << " " << v.name;
                    if (v.elements > 1)
                    {
                        out << "[" << v.elements << "]";
                    }
                    out << ";\n";
                }

                return out << "void main() {"
                           << s.main
                           << "}\n";
            }
        };
    };
};

#endif
