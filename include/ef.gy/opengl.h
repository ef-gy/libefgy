/**\file
 * \brief OpenGL helpers
 *
 * Helpers that encapsulate OpenGL state and objects.
 *
 * \copyright
 * Copyright (c) 2012-2013, ef.gy Project Members
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

#if !defined(EF_GY_OPENGL_H)
#define EF_GY_OPENGL_H

#include <vector>
#include <sstream>
#include <exception>

namespace efgy
{
    /**\brief OpenGL helpers
     *
     * Templates to handle basic OpenGL tasks, such as compiling and linking
     * shader programmes.
     */
    namespace opengl
    {
        /**\brief OpenGL error wrapper
         *
         * A functor to handle OpenGL errors in a generic way; should be used
         * every now and then to make sure the graphics library is still in the
         * intended state.
         *
         * \tparam doThrow Whether to throw a runtime exception instead of
         *                 logging an error to stderr.
         *
         * \see http://www.opengl.org/sdk/docs/man/xhtml/glGetError.xml for a
         *      description of the flags that this functor might return.
         */
        template<const bool doThrow = false>
        class error
        {
            public:
                /**\brief Process OpenGL errors
                 *
                 * Queries OpenGL for any recent errors and return any actual
                 * errors as a vector.
                 *
                 * \return A vector with any error flags that may have occurred
                 *         since the last time glGetError was used. If no errors
                 *         have occurred, then the vector will be empty.
                 */
                std::vector<GLenum> operator () (void)
                {
                    std::vector<GLenum> errors;

                    for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
                    {
                        std::cerr << "glGetError() = 0x" << std::hex << flag << "\n";

                        errors.push_back(flag);
                    }

                    return errors;
                }
        };

        /**\brief OpenGL error wrapper; throws exceptions on errors
         *
         * A functor to handle OpenGL errors in a generic way; should be used
         * every now and then to make sure the graphics library is still in the
         * intended state.
         */
        template<>
        class error<true>
        {
            public:
                /**\brief Process OpenGL errors
                 *
                 * Queries OpenGL for any recent errors; will throw an exception
                 * if any errors have occurred and do nothing otherwise.
                 *
                 * \return Either an empty vector or nothing at all - because
                 *         any error flags that would be returned will instead
                 *         be thrown as an exception, meaning the function will
                 *         not return in such cases.
                 */
                std::vector<GLenum> operator () (void)
                {
                    std::vector<GLenum> errors;
                    
                    for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
                    {
                        std::stringstream s("");

                        s << "glGetError() = 0x" << std::hex << flag << "\n";

                        throw std::runtime_error(s.str());

                        errors.push_back(flag);
                    }

                    return errors;
                }
        };

        /**\brief Rounds up to the nearest power of two
         *
         * This function uses one of the infamous Bit Twiddling Hacks to round a
         * number up to the nearest power of two. We do this because graphics
         * cards prefer their textures to be powers of two.
         *
         * \see http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
         *      for this and other bit hacks to do neat things rather swiftly.
         *      This particular hack was devised by Sean Anderson, so kudos to
         *      him.
         */
        static inline GLuint roundToPowerOf2 (GLuint value)
        {
            value--;
            value |= value >> 1;
            value |= value >> 2;
            value |= value >> 4;
            value |= value >> 8;
            value |= value >> 16;
            value++;
            
            return value;
        }

        /**\brief Shader attributes
         *
         * Contains a list of default vertex attributes passed to vertex
         * shaders.
         */
        enum shaderAttribute
        {
            attributePosition,
            attributeNormal,
            attributeIndex
        };
        
        /**\brief Uniforms
         *
         * Contains a list of default uniforms for the shaders.
         */
        enum uniforms
        {
            uniformProjectionMatrix,
            uniformNormalMatrix,
            uniformColour,
            uniformScreenFramebuffer,
            uniformScreenHistogram,
            uniformColourMap,
            uniformMax
        };

        /**\brief Shader programme
         *
         * Encapsulates an OpenGL shader programme and the state associated with
         * it.
         *
         * \tparam Q Base data type for calculations.
         */
        template<typename Q>
        class programme
        {
            public:
                /**\brief Construct with shaders
                 *
                 * Initialises an instance of this class with a copy of a vertex
                 * and a fragment shader. The programme is not compiled and
                 * linked before it's used for the first time, so it's OK to
                 * initialise the class before you have an active OpenGL
                 * context.
                 *
                 * \param[in] pVertexShader   The vertex shader to compile and
                 *                            link to the programme.
                 * \param[in] pFragmentShader The fragment shader to compile and
                 *                            link to the programme.
                 */
                programme (const std::string &pVertexShader, const std::string &pFragmentShader)
                    : programmeID(0), vertexShader(pVertexShader), fragmentShader(pFragmentShader) {}
            
                /**\brief Destructor
                 *
                 * Erases the programme if it has been compiled and linked; does
                 * nothing otherwise.
                 */
                ~programme (void)
                    {
                        if (programmeID)
                        {
                            glDeleteProgram(programmeID);
                        }
                    }
            
                /**\brief Use programme
                 *
                 * This is a wrapper for OpenGL's useProgram() that activates
                 * the programme; compiles the programme before doing so if it's
                 * not yet compiled and linked properly.
                 *
                 * \return True if the programme was bound correctly, false
                 *         otherwise.
                 */
                bool use (void)
                {
                    if (programmeID)
                    {
                        glUseProgram(programmeID);
                        return true;
                    }
                    else if (compile() && programmeID)
                    {
                        glUseProgram(programmeID);
                        return true;
                    }

                    return false;
                }

                /**\brief Use but do not compile programme
                 *
                 * This is a wrapper for OpenGL's useProgram() that activates
                 * the programme; if the programme has not been compiled yet
                 * then this method simply fails.
                 *
                 * \return True if the programme was bound correctly, false
                 *         otherwise.
                 */
                bool use (void) const
                {
                    if (programmeID)
                    {
                        glUseProgram(programmeID);
                        return true;
                    }

                    return false;
                }

                /**\brief Load uniform 4x4 matrix
                 *
                 * Activate the programme and upload a 4x4 uniform variable to
                 * the specified uniform index. The index array that is used to
                 * look up the actual uniform variable ID is obtained during the
                 * compilation process of the shader porgramme.
                 *
                 * \param[in] index  The index into the uniform ID array to use.
                 * \param[in] matrix The 4x4 matrix to load. The contents of
                 *                   this matrix are turned into a GLfloat array
                 *                   before handing the data to OpenGL.
                 *
                 * \return True if the programme was bound correctly and the
                 *         matrix has been handed off to OpenGL, false
                 *         otherwise.
                 */
                bool uniform (const enum uniforms &index, const math::matrix<Q,4,4> &matrix)
                {
                    if (use())
                    {
                        GLfloat mat[16] =
                          { GLfloat(matrix[0][0]),
                            GLfloat(matrix[0][1]),
                            GLfloat(matrix[0][2]),
                            GLfloat(matrix[0][3]),
                            GLfloat(matrix[1][0]),
                            GLfloat(matrix[1][1]),
                            GLfloat(matrix[1][2]),
                            GLfloat(matrix[1][3]),
                            GLfloat(matrix[2][0]),
                            GLfloat(matrix[2][1]),
                            GLfloat(matrix[2][2]),
                            GLfloat(matrix[2][3]),
                            GLfloat(matrix[3][0]),
                            GLfloat(matrix[3][1]),
                            GLfloat(matrix[3][2]),
                            GLfloat(matrix[3][3]) };

                        glUniformMatrix4fv(uniforms[index], 1, GL_FALSE, mat);
                        return true;
                    }

                    return false;
                }

                /**\brief Load uniform 3x3 matrix
                 *
                 * Activate the programme and upload a 3x3 uniform variable to
                 * the specified uniform index. The index array that is used to
                 * look up the actual uniform variable ID is obtained during the
                 * compilation process of the shader porgramme.
                 *
                 * \param[in] index  The index into the uniform ID array to use.
                 * \param[in] matrix The 3x3 matrix to load. The contents of
                 *                   this matrix are turned into a GLfloat array
                 *                   before handing the data to OpenGL.
                 *
                 * \return True if the programme was bound correctly and the
                 *         matrix has been handed off to OpenGL, false
                 *         otherwise.
                 */
                bool uniform (const enum uniforms &index, const math::matrix<Q,3,3> &matrix)
                {
                    if (use())
                    {
                        GLfloat mat[9] =
                          { GLfloat(matrix[0][0]),
                            GLfloat(matrix[0][1]),
                            GLfloat(matrix[0][2]),
                            GLfloat(matrix[1][0]),
                            GLfloat(matrix[1][1]),
                            GLfloat(matrix[1][2]),
                            GLfloat(matrix[2][0]),
                            GLfloat(matrix[2][1]),
                            GLfloat(matrix[2][2]) };

                        glUniformMatrix3fv(uniforms[index], 1, GL_FALSE, mat);
                        return true;
                    }

                    return false;
                }

                /**\brief Load uniform 2x2 matrix
                 *
                 * Activate the programme and upload a 2x2 uniform variable to
                 * the specified uniform index. The index array that is used to
                 * look up the actual uniform variable ID is obtained during the
                 * compilation process of the shader porgramme.
                 *
                 * \param[in] index  The index into the uniform ID array to use.
                 * \param[in] matrix The 2x2 matrix to load. The contents of
                 *                   this matrix are turned into a GLfloat array
                 *                   before handing the data to OpenGL.
                 *
                 * \return True if the programme was bound correctly and the
                 *         matrix has been handed off to OpenGL, false
                 *         otherwise.
                 */
                bool uniform (const enum uniforms &index, const math::matrix<Q,2,2> &matrix)
                {
                    if (use())
                    {
                        GLfloat mat[4] =
                          { GLfloat(matrix[0][0]),
                            GLfloat(matrix[0][1]),
                            GLfloat(matrix[1][0]),
                            GLfloat(matrix[1][1]) };

                        glUniformMatrix2fv(uniforms[index], 1, GL_FALSE, mat);
                        return true;
                    }

                    return false;
                }

                /**\brief Load integer uniform
                 *
                 * Activate the programme and upload an integer uniform variable
                 * to the specified uniform index. The index array that is used
                 * to look up the actual uniform variable ID is obtained during
                 * the compilation process of the shader porgramme.
                 *
                 * \param[in] index The index into the uniform ID array to use.
                 * \param[in] value The integer value to load.
                 *
                 * \return True if the programme was bound correctly and the
                 *         matrix has been handed off to OpenGL, false
                 *         otherwise.
                 */
                bool uniform (const enum uniforms &index, const GLint &value)
                {
                    if (use())
                    {
                        glUniform1i(uniforms[index], value);
                        return true;
                    }

                    return false;
                }

                /**\brief Load uniform floating point vector
                 *
                 * Activate the programme and upload a uniform float vector
                 * to the specified uniform index. The index array that is used
                 * to look up the actual uniform variable ID is obtained during
                 * the compilation process of the shader porgramme.
                 *
                 * \param[in] index The index into the uniform ID array to use.
                 * \param[in] value The floating point vector to load.
                 *
                 * \return True if the programme was bound correctly and the
                 *         matrix has been handed off to OpenGL, false
                 *         otherwise.
                 */
                bool uniform (const enum uniforms &index, const GLfloat value[4])
                {
                    if (use())
                    {
                        glUniform4f(uniforms[index], value[0], value[1], value[2], value[3]);
                        return true;
                    }

                    return false;
                }

                /**\brief Load uniform 4x4 matrix
                 *
                 * Activate the programme and upload a 4x4 uniform variable to
                 * the specified uniform index. The index array that is used to
                 * look up the actual uniform variable ID is obtained during the
                 * compilation process of the shader porgramme.
                 *
                 * \param[in] index  The index into the uniform ID array to use.
                 * \param[in] matrix The 4x4 matrix to load. The contents of
                 *                   this matrix are turned into a GLfloat array
                 *                   before handing the data to OpenGL.
                 *
                 * \return True if the programme was bound correctly and the
                 *         matrix has been handed off to OpenGL, false
                 *         otherwise.
                 */
                bool uniform (const enum uniforms &index, const math::matrix<Q,4,4> &matrix) const
                {
                    if (use())
                    {
                        GLfloat mat[16] =
                          { GLfloat(matrix[0][0]),
                            GLfloat(matrix[0][1]),
                            GLfloat(matrix[0][2]),
                            GLfloat(matrix[0][3]),
                            GLfloat(matrix[1][0]),
                            GLfloat(matrix[1][1]),
                            GLfloat(matrix[1][2]),
                            GLfloat(matrix[1][3]),
                            GLfloat(matrix[2][0]),
                            GLfloat(matrix[2][1]),
                            GLfloat(matrix[2][2]),
                            GLfloat(matrix[2][3]),
                            GLfloat(matrix[3][0]),
                            GLfloat(matrix[3][1]),
                            GLfloat(matrix[3][2]),
                            GLfloat(matrix[3][3]) };

                        glUniformMatrix4fv(uniforms[index], 1, GL_FALSE, mat);
                        return true;
                    }

                    return false;
                }

                /**\brief Load uniform 3x3 matrix
                 *
                 * Activate the programme and upload a 3x3 uniform variable to
                 * the specified uniform index. The index array that is used to
                 * look up the actual uniform variable ID is obtained during the
                 * compilation process of the shader porgramme.
                 *
                 * \param[in] index  The index into the uniform ID array to use.
                 * \param[in] matrix The 3x3 matrix to load. The contents of
                 *                   this matrix are turned into a GLfloat array
                 *                   before handing the data to OpenGL.
                 *
                 * \return True if the programme was bound correctly and the
                 *         matrix has been handed off to OpenGL, false
                 *         otherwise.
                 */
                bool uniform (const enum uniforms &index, const math::matrix<Q,3,3> &matrix) const
                {
                    if (use())
                    {
                        GLfloat mat[9] =
                          { GLfloat(matrix[0][0]),
                            GLfloat(matrix[0][1]),
                            GLfloat(matrix[0][2]),
                            GLfloat(matrix[1][0]),
                            GLfloat(matrix[1][1]),
                            GLfloat(matrix[1][2]),
                            GLfloat(matrix[2][0]),
                            GLfloat(matrix[2][1]),
                            GLfloat(matrix[2][2]) };

                        glUniformMatrix3fv(uniforms[index], 1, GL_FALSE, mat);
                        return true;
                    }

                    return false;
                }

                /**\brief Load uniform 2x2 matrix
                 *
                 * Activate the programme and upload a 2x2 uniform variable to
                 * the specified uniform index. The index array that is used to
                 * look up the actual uniform variable ID is obtained during the
                 * compilation process of the shader porgramme.
                 *
                 * \param[in] index  The index into the uniform ID array to use.
                 * \param[in] matrix The 2x2 matrix to load. The contents of
                 *                   this matrix are turned into a GLfloat array
                 *                   before handing the data to OpenGL.
                 *
                 * \return True if the programme was bound correctly and the
                 *         matrix has been handed off to OpenGL, false
                 *         otherwise.
                 */
                bool uniform (const enum uniforms &index, const math::matrix<Q,2,2> &matrix) const
                {
                    if (use())
                    {
                        GLfloat mat[4] =
                          { GLfloat(matrix[0][0]),
                            GLfloat(matrix[0][1]),
                            GLfloat(matrix[1][0]),
                            GLfloat(matrix[1][1]) };

                        glUniformMatrix2fv(uniforms[index], 1, GL_FALSE, mat);
                        return true;
                    }

                    return false;
                }

                /**\brief Load integer uniform
                 *
                 * Activate the programme and upload an integer uniform variable
                 * to the specified uniform index. The index array that is used
                 * to look up the actual uniform variable ID is obtained during
                 * the compilation process of the shader porgramme.
                 *
                 * \param[in] index The index into the uniform ID array to use.
                 * \param[in] value The integer value to load.
                 *
                 * \return True if the programme was bound correctly and the
                 *         matrix has been handed off to OpenGL, false
                 *         otherwise.
                 */
                bool uniform (const enum uniforms &index, const GLint &value) const
                {
                    if (use())
                    {
                        glUniform1i(uniforms[index], value);
                        return true;
                    }

                    return false;
                }

                /**\brief Load uniform floating point vector
                 *
                 * Activate the programme and upload a uniform float vector
                 * to the specified uniform index. The index array that is used
                 * to look up the actual uniform variable ID is obtained during
                 * the compilation process of the shader porgramme.
                 *
                 * \param[in] index The index into the uniform ID array to use.
                 * \param[in] value The floating point vector to load.
                 *
                 * \return True if the programme was bound correctly and the
                 *         matrix has been handed off to OpenGL, false
                 *         otherwise.
                 */
                bool uniform (const enum uniforms &index, const GLfloat value[4]) const
                {
                    if (use())
                    {
                        glUniform4f(uniforms[index], value[0], value[1], value[2], value[3]);
                        return true;
                    }

                    return false;
                }

            protected:
                /**\brief Default uniforms
                 *
                 * Contains the indices of the default uniforms. This array is
                 * populated after linking the shaders.
                 */
                GLint uniforms[uniformMax];

                /**\brief Programme ID
                 *
                 * The programme ID as returned by OpenGL; set to zero as long
                 * as the programme has not been compiled, nonzero afterwards.
                 */
                GLuint programmeID;
            
                /**\brief Vertex shader programme
                 *
                 * This is a copy of the vertex shader's source code; kept
                 * around so it can be compiled when needed.
                 */
                const std::string vertexShader;

                /**\brief Fragment shader programme
                 *
                 * This is a copy of the fragment shader's source code; kept
                 * around so it can be compiled when needed.
                 */
                const std::string fragmentShader;

                /**\brief Compile shader programme
                 *
                 * Compiles and links the shader programmes together and sets
                 * the programmeID and loads the default uniform indices.
                 *
                 * \return True if the programme linked and compiled correctly,
                 *         false otherwise.
                 */
                bool compile (void)
                {
                    GLuint vertShader, fragShader;
                    
                    programmeID = glCreateProgram();
                    
                    if (!compile(vertShader, GL_VERTEX_SHADER, vertexShader.c_str()))
                    {
                        std::cerr << "Failed to compile vertex shader\n";
                        return false;
                    }
                    
                    if (!compile(fragShader, GL_FRAGMENT_SHADER, fragmentShader.c_str()))
                    {
                        std::cerr << "Failed to compile fragment shader\n";
                        return false;
                    }
                    
                    glAttachShader(programmeID, vertShader);
                    glAttachShader(programmeID, fragShader);
                    
                    glBindAttribLocation(programmeID, attributePosition, "position");
                    glBindAttribLocation(programmeID, attributeNormal,   "normal");
                    glBindAttribLocation(programmeID, attributeIndex,    "index");
                    
                    if (!link())
                    {
                        std::cerr << "Failed to link program: " << programmeID << "\n";
                        
                        if (vertShader) {
                            glDeleteShader(vertShader);
                            vertShader = 0;
                        }
                        if (fragShader) {
                            glDeleteShader(fragShader);
                            fragShader = 0;
                        }
                        if (programmeID) {
                            glDeleteProgram(programmeID);
                            programmeID = 0;
                        }
                        
                        return false;
                    }
                    
                    // Get uniform locations.
                    uniforms[uniformProjectionMatrix]  = glGetUniformLocation(programmeID, "modelViewProjectionMatrix");
                    uniforms[uniformNormalMatrix]      = glGetUniformLocation(programmeID, "normalMatrix");
                    uniforms[uniformColour]            = glGetUniformLocation(programmeID, "colour");
                    uniforms[uniformScreenFramebuffer] = glGetUniformLocation(programmeID, "screenFramebuffer");
                    uniforms[uniformScreenHistogram]   = glGetUniformLocation(programmeID, "screenHistogram");
                    uniforms[uniformColourMap]         = glGetUniformLocation(programmeID, "colourMap");

                    // Release vertex and fragment shaders.
                    if (vertShader) {
                        glDetachShader(programmeID, vertShader);
                        glDeleteShader(vertShader);
                    }
                    if (fragShader) {
                        glDetachShader(programmeID, fragShader);
                        glDeleteShader(fragShader);
                    }
                    
                    return true;
                }

                /**\brief Compile a shader
                 *
                 * Compiles a shader so it can be linked into a proper OpenGL
                 * programme.
                 *
                 * \param[out] shader The shader ID produced by the compiler.
                 * \param[in]  type   The type of shader you want to compile.
                 * \param[in]  data   Source code of the shader to compile.
                 *
                 * \return True if the shader compiled correctly, false
                 *         otherwise.
                 */
                bool compile (GLuint &shader, GLenum type, const char *data) const
                {
                    GLint status;
                    const GLchar *source = (const GLchar *)data;
                    
                    shader = glCreateShader(type);
                    glShaderSource(shader, 1, &source, NULL);
                    glCompileShader(shader);
                    
#if defined(DEBUG)
                    GLint logLength;
                    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
                    if (logLength > 0)
                    {
                        GLchar *log = new GLchar[logLength];
                        glGetShaderInfoLog(shader, logLength, &logLength, log);
                        std::cerr << "Shader compile log:\n" << log << "\n";
                        delete[] log;
                    }
#endif
                    
                    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
                    if (status == 0) {
                        glDeleteShader(shader);
                        return false;
                    }
                    
                    return true;
                }
            
                /**\brief Link shader programmes
                 *
                 * Links a programme with the currently attached shaders so that
                 * it can be used by OpenGL.
                 *
                 * \return True if the programme was linked successfully, false
                 *         otherwise.
                 */
                bool link () const
                {
                    GLint status;
                    glLinkProgram(programmeID);
                    
#if defined(DEBUG)
                    GLint logLength;
                    glGetProgramiv(programmeID, GL_INFO_LOG_LENGTH, &logLength);
                    if (logLength > 0)
                    {
                        GLchar *log = new GLchar[logLength];
                        glGetProgramInfoLog(programmeID, logLength, &logLength, log);
                        std::cerr << "Program link log:\n" << log << "\n";
                        delete[] log;
                    }
#endif
                    
                    glGetProgramiv(programmeID, GL_LINK_STATUS, &status);
                    if (status == 0)
                    {
                        return false;
                    }
                    
                    return true;
                }
            };

        /**\brief Framebuffer object
         *
         * Encapsulates an OpenGL framebuffer object and the state associated
         * with it.
         *
         * \tparam Q Base data type for calculations.
         */
        template<typename Q>
        class framebuffer
        {
            public:
                /**\brief Default constructor
                 *
                 * Initialises the object; this does not yet create a
                 * framebuffer object, the use() method does that the first
                 * time you call it.
                 */
                framebuffer (void)
                    : framebufferID(0), framebufferIDcopied(false) {}

                /**\brief Destructor
                 *
                 * Erases the framebuffer object if it has been created; does
                 * nothing otherwise.
                 */
                ~framebuffer (void)
                    {
                        if (!framebufferIDcopied && framebufferID)
                        {
                            glDeleteFramebuffers(1, &framebufferID);
                        }
                    }

                /**\brief Use framebuffer
                 *
                 * Enables the framebuffer for this object; if the framebuffer
                 * object has not been created yet, it will be created when you
                 * call this method.
                 *
                 * \param[in] target The type of framebuffer to set.
                 *
                 * \return True on success, false otherwise.
                 *
                 * \see http://www.opengl.org/sdk/docs/man/xhtml/glBindFramebuffer.xml
                 *      for the possible values of the target parameter.
                 */
                bool use (const GLenum &target = GL_FRAMEBUFFER)
                {
                    if (!framebufferID && !framebufferIDcopied)
                    {
                        glGenFramebuffers(1, &framebufferID);
                    }

                    if (framebufferID || framebufferIDcopied)
                    {
                        glBindFramebuffer(target, framebufferID);
                        return true;
                    }

                    return false;
                }

                /**\brief Copy framebuffer ID
                 *
                 * Assign the internal framebuffer ID the currently bound
                 * framebuffer.
                 *
                 * \param[in] target The type of the framebuffer whose binding
                 *                   should be used.
                 *
                 * \return True on success, false otherwise.
                 */
                bool copy (const GLenum &target = GL_FRAMEBUFFER_BINDING)
                {
                    glGetIntegerv(target, (GLint*)&framebufferID);
                    framebufferIDcopied = true;

                    return true;
                }

            protected:
                /**\brief Framebuffer ID
                 *
                 * The framebuffer ID as returned by OpenGL; set to zero as long
                 * as the framebuffer has not been created, nonzero afterwards.
                 */
                GLuint framebufferID;

                /**\brief Did we copy the framebuffer ID?
                 *
                 * Set to true after copying a framebuffer ID; this is kept
                 * track of separately so that we don't delete someone else's
                 * framebuffer ID if we just copied it with copy().
                 */
                bool framebufferIDcopied;
        };

        /**\brief Texture object
         *
         * Encapsulates an OpenGL texture object and the state associated
         * with it.
         *
         * \tparam target The texture target to bind to.
         * \tparam format Format of the texture to load or create.
         * \tparam baseFormat External texture format.
         * \tparam type   Type of the texture to load.
         *
         * \see http://www.opengl.org/sdk/docs/man/xhtml/glBindTexture.xml and
         *      http://www.opengl.org/sdk/docs/man/xhtml/glTexImage2D.xml for
         *      the possible values of the target, format and type parameters.
         */
        template<GLenum target, GLenum format = GL_RGB, GLenum baseFormat = format, GLenum type = GL_UNSIGNED_BYTE>
        class texture
        {
            public:
                /**\brief Default constructor
                 *
                 * Initialises the object; this does not yet create a
                 * texture object, the use() method does that the first
                 * time you call it.
                 */
                texture (void)
                    : textureID(0) {}

                /**\brief Destructor
                 *
                 * Erases the texture object if it has been created; does
                 * nothing otherwise.
                 */
                ~texture (void)
                    {
                        if (textureID)
                        {
                            glDeleteTextures(1, &textureID);
                        }
                    }

                /**\brief Bind texture
                 *
                 * Binds the texture for this object; if the texture object has
                 * not been created yet, it will be created when you call this
                 * method.
                 *
                 * \return True on success, false otherwise.
                 */
                bool bind (void)
                {
                    if (!textureID)
                    {
                        glGenTextures(1, &textureID);
                        width = 0;
                        height = 0;
                    }

                    if (textureID)
                    {
                        glBindTexture(target, textureID);
                        return true;
                    }
                    
                    return false;
                }

                /**\brief Bind and load texture
                 *
                 * Binds a texture and then uses glTexImage2D to set its
                 * parameters and - optionally - load its data as specified.
                 *
                 * The texture is only loaded or created explicity if any of the
                 * parameters is different from what the class thinks the
                 * current parameters ought to be.
                 *
                 * \param[in] pWidth  Width of the texture to load or create.
                 * \param[in] pHeight Height of the texture to load or create.
                 * \param[in] data    Raw memory data of the texture to load;
                 *                    use zero if you only want to create a
                 *                    texture and not load one from memory.
                 *
                 * \return True on success, false otherwise.
                 */
                bool load (const GLuint &pWidth, const GLuint &pHeight,
                           const void *data = 0)
                {
                    if (bind())
                    {
                        if (   (pWidth != width) || (pHeight != height)
                            || (data != 0))
                        {
                            width  = roundToPowerOf2(pWidth);
                            height = roundToPowerOf2(pHeight);

                            glTexImage2D
                                (target, 0, format,
                                 width, height, 0, baseFormat, type, data);
                        }

                        return true;
                    }

                    return false;
                }

                /**\brief Texture ID
                 *
                 * The texture ID as returned by OpenGL; set to zero as long
                 * as the texture has not been created, nonzero afterwards.
                 */
                GLuint textureID;

            protected:
                /**\brief Texture width
                 *
                 * The width of the texture that was loaded or created with the
                 * load() method; zero if the texture has been created with
                 * glGenTextures() but has not had any of its settings set with
                 * the load() method.
                 *
                 * Used to figure out if the texture needs to be recreated if
                 * you use the load() method.
                 */
                GLuint width;

                /**\brief Texture height
                 *
                 * The height of the texture that was loaded or created with the
                 * load() method; zero if the texture has been created with
                 * glGenTextures() but has not had any of its settings set with
                 * the load() method.
                 *
                 * Used to figure out if the texture needs to be recreated if
                 * you use the load() method.
                 */
                GLuint height;
        };

        /**\brief 2D texture object
         *
         * Specialisation of the texture template with the GL_TEXTURE_2D target.
         * This is probably the most common texture target, thus the typedef.
         */
        typedef texture<GL_TEXTURE_2D> texture2D;

        /**\brief Renderbuffer object
         *
         * Encapsulates an OpenGL renderbuffer object and the state associated
         * with it.
         *
         * \note The format and target parameters are reversed compared to the
         *       texture template, because you shouldn't ever need to change the
         *       renderbuffer target.
         *
         * \tparam format Texture format to use.
         * \tparam target Renderbuffer target to bind to.
         *
         * \see http://www.khronos.org/opengles/sdk/docs/man/xhtml/glRenderbufferStorage.xml
         *      for the possible values of the format and target parameters.
         */
        template<GLenum format, GLenum target = GL_RENDERBUFFER>
        class renderbuffer
        {
            public:
                /**\brief Default constructor
                 *
                 * Initialises the object; this does not yet create a
                 * renderbuffer object, the use() method does that the first
                 * time you call it.
                 */
                renderbuffer (void)
                    : renderbufferID(0) {}
                
                /**\brief Destructor
                 *
                 * Erases the renderbuffer object if it has been created; does
                 * nothing otherwise.
                 */
                ~renderbuffer (void)
                {
                    if (renderbufferID)
                    {
                        glDeleteTextures(1, &renderbufferID);
                    }
                }

                /**\brief Bind renderbuffer
                 *
                 * Binds the renderbuffer for this object; if the renderbuffer
                 * object has not been created yet, it will be created when you
                 * call this method.
                 *
                 * \return True on success, false otherwise.
                 */
                bool bind (void)
                {
                    if (!renderbufferID)
                    {
                        glGenRenderbuffers(1, &renderbufferID);
                        width = 0;
                        height = 0;
                    }
                    
                    if (renderbufferID)
                    {
                        glBindRenderbuffer(target, renderbufferID);
                        return true;
                    }
                    
                    return false;
                }

                /**\brief Bind and load renderbuffer
                 *
                 * Binds a renderbuffer and then sets its width and height if
                 * necessary.
                 *
                 * The renderbuffer is created if it doesn't exist yet.
                 *
                 * \param[in] pWidth  Width of the renderbuffer to bind.
                 * \param[in] pHeight Height of the renderbuffer to bind.
                 *
                 * \return True on success, false otherwise.
                 */
                bool load (const GLuint &pWidth, const GLuint &pHeight)
                {
                    if (bind())
                    {
                        if ((pWidth != width) || (pHeight != height))
                        {
                            width  = roundToPowerOf2(pWidth);
                            height = roundToPowerOf2(pHeight);

                            glRenderbufferStorage(target, format, width, height);
                        }
                        
                        return true;
                    }
                    
                    return false;
                }

                /**\brief Renderbuffer ID
                 *
                 * The renderbuffer ID as returned by OpenGL; set to zero as
                 * long as the texture has not been created, nonzero afterwards.
                 */
                GLuint renderbufferID;
                
            protected:
                /**\brief Renderbuffer width
                 *
                 * The width of the buffer that was created with the use()
                 * method; zero if the buffer has been created with
                 * glGenRenderbuffers() but has not had any storage attached
                 * with the use() method.
                 *
                 * Used to figure out if the buffer needs to be recreated if
                 * you use the use() method.
                 */
                GLuint width;

                /**\brief Renderbuffer height
                 *
                 * The height of the buffer that was created with the use()
                 * method; zero if the buffer has been created with
                 * glGenRenderbuffers() but has not had any storage attached
                 * with the use() method.
                 *
                 * Used to figure out if the buffer needs to be recreated if
                 * you use the use() method.
                 */
                GLuint height;
        };

        /**\brief Framebuffer with associated texture
         *
         * Associates a texture with a framebuffer so the two are always used
         * together.
         *
         * \tparam Q Base data type for calculations.
         * \tparam format Format of the texture to load or create.
         * \tparam baseFormat External texture format.
         * \tparam type   Type of the texture to load.
         * \tparam target The texture target to bind to.
         *
         * \see http://www.opengl.org/sdk/docs/man/xhtml/glBindTexture.xml and
         *      http://www.opengl.org/sdk/docs/man/xhtml/glTexImage2D.xml for
         *      the possible values of the target, format and type parameters.
         */
        template<typename Q, GLenum format = GL_RGB, GLenum baseFormat = format, GLenum type = GL_UNSIGNED_BYTE, GLenum target = GL_TEXTURE_2D>
        class framebufferTexture : public framebuffer<Q>, public texture<target, format, baseFormat, type>
        {
            public:
                /**\brief Bind framebuffer and texture
                 *
                 * Binds the framebuffer and texture objects for this instance.
                 * The texture is bound to the specified target. Creates the
                 * framebuffer or the texture if they don't exist yet.
                 *
                 * \param[in] width  Width of the texture to load or create.
                 * \param[in] height Height of the texture to load or create.
                 *
                 * \return True on success, false otherwise.
                 */
                bool use (const GLuint &width, const GLuint &height)
                {
                    if (framebuffer<Q>::use() && texture<target,format,baseFormat,type>::load(width, height))
                    {
                        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

                        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, texture<target,format,baseFormat,type>::textureID, 0);

                        return true;
                    }

                    return false;
                }
        };

        /**\brief Framebuffer with associated texture and depth buffer
         *
         * Associates a texture with a framebuffer so the two are always used
         * together. Also adds a renderbuffer used as a depth buffer.
         *
         * \tparam Q Base data type for calculations.
         * \tparam format Format of the texture to load or create.
         * \tparam baseFormat External texture format.
         * \tparam type   Type of the texture to load.
         * \tparam target The texture target to bind to.
         * \tparam depthFormat Internal texture format of the depth buffer.
         *
         * \see http://www.opengl.org/sdk/docs/man/xhtml/glBindTexture.xml and
         *      http://www.opengl.org/sdk/docs/man/xhtml/glTexImage2D.xml for
         *      the possible values of the target, format and type parameters.
         * \see http://www.khronos.org/opengles/sdk/docs/man/xhtml/glRenderbufferStorage.xml
         *      for possible values of the depthFormat parameter.
         */
        template<typename Q, GLenum format = GL_RGB, GLenum baseFormat = format, GLenum type = GL_UNSIGNED_BYTE, GLenum target = GL_TEXTURE_2D, GLenum depthFormat = GL_DEPTH_COMPONENT16>
        class framebufferTextureDepth : public framebufferTexture<Q, format, baseFormat, type, target>, public renderbuffer<depthFormat>
        {
            public:
                /**\brief Bind framebuffer, texture and depth renderbuffer
                 *
                 * Binds the framebuffer, texture object and depth renderbuffer
                 * for this instance. The texture is bound to the specified
                 * target. Creates the framebuffer, texture or renderbuffer if
                 * they don't exist yet.
                 *
                 * \param[in] width  Width of the texture to load or create.
                 * \param[in] height Height of the texture to load or create.
                 *
                 * \return True on success, false otherwise.
                 */
                bool use (const GLuint &width, const GLuint &height)
                {
                    if (   framebufferTexture<Q, format, baseFormat, type, target>::load(width, height)
                        && renderbuffer<depthFormat>::load(width, height))
                    {
                        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer<depthFormat>::renderbufferID);

                        return true;
                    }

                    return false;
                }
        };

        /**\brief OpenGL programme to render to a texture
         *
         * Associates an OpenGL programme with a framebuffer and a texture to
         * easily render grab the output of a render pass.
         *
         * \tparam Q Base data type for calculations.
         * \tparam format Format of the texture to load or create.
         * \tparam baseFormat External texture format.
         * \tparam type   Type of the texture to load.
         * \tparam target The texture target to bind to.
         *
         * \see http://www.opengl.org/sdk/docs/man/xhtml/glBindTexture.xml and
         *      http://www.opengl.org/sdk/docs/man/xhtml/glTexImage2D.xml for
         *      the possible values of the target, format and type parameters.
         */
        template<typename Q, GLenum format = GL_RGB, GLenum baseFormat = format, GLenum type = GL_UNSIGNED_BYTE, GLenum target = GL_TEXTURE_2D>
        class renderToTextureProgramme : public programme<Q>, public framebufferTexture<Q,format,baseFormat,type,target>
        {
            public:
                /**\brief Construct with shaders
                 *
                 * Initialises an instance of this class with a copy of a vertex
                 * and a fragment shader. The programme is not compiled and
                 * linked before it's used for the first time, so it's OK to
                 * initialise the class before you have an active OpenGL
                 * context.
                 *
                 * The framebuffer and texture objects are similarly initialised
                 * with their default constructor; they're not created or bound
                 * automatically either, only when the use() method is called.
                 *
                 * \param[in] pVertexShader   The vertex shader to compile and
                 *                            link to the programme.
                 * \param[in] pFragmentShader The fragment shader to compile and
                 *                            link to the programme.
                 */
                renderToTextureProgramme (const std::string &pVertexShader, const std::string &pFragmentShader)
                    : programme<Q>(pVertexShader, pFragmentShader), framebufferTexture<Q,format,baseFormat,type,target>() {}

                /**\brief Use programme and render to associated texture
                 *
                 * Enables or compiles the programme described in the shaders
                 * passed to the constructor, then binds or creates a
                 * framebuffer and texture as needed.
                 *
                 * If compiling the programme and binding the framebuffer and
                 * texture succeed then the viewport is set to write to the
                 * whole texture.
                 *
                 * \param[in] width       Width of the texture to load or
                 *                        create.
                 * \param[in] height      Height of the texture to load or
                 *                        create.
                 * \param[in] textureUnit The texture unit to bind to.
                 *
                 * \return True on success, false otherwise.
                 */
                bool use (const GLuint &width, const GLuint &height, const int &textureUnit = -1)
                {
                    if (textureUnit >= 0)
                    {
                        glActiveTexture (GL_TEXTURE0 + textureUnit);
                    }

                    if (programme<Q>::use() && framebufferTexture<Q,format,baseFormat,type,target>::use(width, height))
                    {
                        glViewport (0, 0, roundToPowerOf2(width), roundToPowerOf2(height));

                        return true;
                    }

                    return false;
                }

                using programme<Q>::use;
        };

        /**\brief OpenGL programme to render to a texture with depth buffer
         *
         * Associates an OpenGL programme with a framebuffer, a texture and a
         * renderbuffer for the depth component to easily render grab the output
         * of a render pass.
         *
         * \tparam Q Base data type for calculations.
         * \tparam format Format of the texture to load or create.
         * \tparam baseFormat External texture format.
         * \tparam type   Type of the texture to load.
         * \tparam target The texture target to bind to.
         * \tparam depthFormat Internal texture format of the depth buffer.
         *
         * \see http://www.opengl.org/sdk/docs/man/xhtml/glBindTexture.xml and
         *      http://www.opengl.org/sdk/docs/man/xhtml/glTexImage2D.xml for
         *      the possible values of the target, format and type parameters.
         * \see http://www.khronos.org/opengles/sdk/docs/man/xhtml/glRenderbufferStorage.xml
         *      for possible values of the depthFormat parameter.
         */
        template<typename Q, GLenum format = GL_RGB, GLenum baseFormat = format, GLenum type = GL_UNSIGNED_BYTE, GLenum target = GL_TEXTURE_2D, GLenum depthFormat = GL_DEPTH_COMPONENT16>
        class renderToTextureDepthProgramme : public programme<Q>, public framebufferTextureDepth<Q,format,baseFormat,type,target,depthFormat>
        {
            public:
                /**\brief Construct with shaders
                 *
                 * Initialises an instance of this class with a copy of a vertex
                 * and a fragment shader. The programme is not compiled and
                 * linked before it's used for the first time, so it's OK to
                 * initialise the class before you have an active OpenGL
                 * context.
                 *
                 * The framebuffer, texture and renderbuffer objects are
                 * similarly initialised with their default constructor; they're
                 * not created or bound automatically either, only when the
                 * use() method is called.
                 *
                 * \param[in] pVertexShader   The vertex shader to compile and
                 *                            link to the programme.
                 * \param[in] pFragmentShader The fragment shader to compile and
                 *                            link to the programme.
                 */
                renderToTextureDepthProgramme (const std::string &pVertexShader, const std::string &pFragmentShader)
                    : programme<Q>(pVertexShader, pFragmentShader), framebufferTextureDepth<Q,format,baseFormat,type,target,depthFormat>() {}
                
                /**\brief Use programme and render to associated texture
                 *
                 * Enables or compiles the programme described in the shaders
                 * passed to the constructor, then binds or creates a
                 * framebuffer, texture and depth buffer as needed.
                 *
                 * If compiling the programme and binding the framebuffer,
                 * texture and depth buffer succeed then the viewport is set to
                 * write to the whole texture.
                 *
                 * \param[in] width       Width of the texture to load or
                 *                        create.
                 * \param[in] height      Height of the texture to load or
                 *                        create.
                 * \param[in] textureUnit The texture unit to bind to.
                 *
                 * \return True on success, false otherwise.
                 */
                bool use (const GLuint &width, const GLuint &height, const int &textureUnit = -1)
                {
                    const GLuint swidth  = roundToPowerOf2(width);
                    const GLuint sheight = roundToPowerOf2(height);

                    if (textureUnit >= 0)
                    {
                        glActiveTexture (GL_TEXTURE0 + textureUnit);
                    }
                    
                    if (programme<Q>::use() && framebufferTextureDepth<Q,format,baseFormat,type,target>::use(swidth, sheight))
                    {
                        glViewport (0, 0, swidth, sheight);
                        
                        return true;
                    }
                    
                    return false;
                }

                using programme<Q>::use;
        };

        /**\brief OpenGL programme to render to a generic framebuffer
         *
         * Associates an OpenGL programme with a framebuffer but not with a
         * texture; this makes it easy to render things and have the correct
         * framebuffer and viewport selected.
         *
         * \tparam Q Base data type for calculations.
         */
        template<typename Q>
        class renderToFramebufferProgramme : public programme<Q>, public framebuffer<Q>
        {
            public:
                /**\brief Construct with shaders
                 *
                 * Initialises an instance of this class with a copy of a vertex
                 * and a fragment shader. The programme is not compiled and
                 * linked before it's used for the first time, so it's OK to
                 * initialise the class before you have an active OpenGL
                 * context.
                 *
                 * The framebuffer object is similarly initialised with its
                 * default constructor; it's not created or bound automatically
                 * either, only when the use() method is called.
                 *
                 * \param[in] pVertexShader   The vertex shader to compile and
                 *                            link to the programme.
                 * \param[in] pFragmentShader The fragment shader to compile and
                 *                            link to the programme.
                 */
                renderToFramebufferProgramme (const std::string &pVertexShader, const std::string &pFragmentShader)
                    : programme<Q>(pVertexShader, pFragmentShader),  framebuffer<Q>() {}

                /**\brief Use programme and render to associated framebuffer
                 *
                 * Enables or compiles the programme described in the shaders
                 * passed to the constructor, then binds or creates a
                 * framebuffer as needed.
                 *
                 * If compiling the programme and binding the framebuffer
                 * succeed then the viewport is set to write to the size passed
                 * as parameters.
                 *
                 * \param[in] width  Width of the viewport to set.
                 * \param[in] height Height of the viewport to set.
                 *
                 * \return True on success, false otherwise.
                 */
                bool use (const GLuint &width, const GLuint &height)
                {
                    if (programme<Q>::use() && framebuffer<Q>::use())
                    {
                        glViewport (0, 0, width, height);

                        return true;
                    }

                    return false;
                }

                using programme<Q>::use;
                using framebuffer<Q>::copy;
        };

        /**\brief Buffer object
         *
         * An OpenGL buffer object that encapsulates a specific type of buffer
         * used by OpenGL.
         *
         * \tparam target The type of buffer object to encapsulate.
         *
         * \see http://www.opengl.org/sdk/docs/man/xhtml/glBindBuffer.xml for
         *      possible values to use for the target parameter.
         */
        template<GLenum target>
        class buffer
        {
            public:
                /**\brief Default constructor
                 *
                 * Initialises an empty instance of this class. The OpenGL
                 * buffer object is not created right away, but rather when
                 * using the bind() function for the first time.
                 */
                buffer (void)
                    : bufferID(0) {}

                /**\brief Clean up created buffer
                 *
                 * Deletes the buffer this object is encapsulating, but only if
                 * it has been created in the first place.
                 */
                ~buffer (void)
                    {
                        if (bufferID)
                        {
                            glDeleteBuffers (1, &bufferID);
                        }
                    }

                /**\brief Bind buffer object
                 *
                 * Binds the buffer object in the current OpenGL context to the
                 * target specified as a template argument. If the object has
                 * not been created yet, then this function will do so before
                 * trying to bind the buffer.
                 *
                 * \return True on success, false otherwise.
                 */
                bool bind (void)
                {
                    if (!bufferID)
                    {
                        glGenBuffers (1, &bufferID);
                    }

                    if (bufferID)
                    {
                        glBindBuffer(target, bufferID);
                        return true;
                    }

                    return false;
                }

                /**\brief Bind but do not create buffer object
                 *
                 * Binds the buffer object in the current OpenGL context to the
                 * target specified as a template argument. If the object has
                 * not been created yet, then this function fails.
                 *
                 * This is in contrast to the non-const version of this
                 * function, which would create an OpenGL buffer object instead.
                 *
                 * \return True on success, false otherwise.
                 */
                bool bind (void) const
                {
                    if (bufferID)
                    {
                        glBindBuffer(target, bufferID);
                        return true;
                    }
                    
                    return false;
                }

                /**\brief Bind and load data
                 *
                 * This function binds and loads data to a buffer object. If the
                 * buffer object does not exist yet then it is created first.
                 *
                 * \param[in] size Number of bytes to copy from data into the
                 *                 buffer object.
                 * \param[in] data Pointer to the first byte to copy.
                 *
                 * \return True on success, false otherwise.
                 *
                 * \see http://www.opengl.org/sdk/docs/man/xhtml/glBufferData.xml
                 *      for possible values of the usage parameter.
                 */
                template<GLenum usage = GL_STATIC_DRAW>
                bool load (const GLsizeiptr &size, const GLvoid *data)
                {
                    if (bind())
                    {
                        glBufferData(target, size, data, usage);
                        return true;
                    }

                    return false;
                }

            protected:
                /**\brief OpenGL buffer ID
                 *
                 * This is the buffer ID as assigned by OpenGL when binding the
                 * buffer for the first time.
                 */
                GLuint bufferID;
        };

        /**\brief Vertex buffer object
         *
         * A vertex buffer in OpenGL is used to store vertex data, such as
         * positions, normals or colours of vertices. As such it is a simple
         * specialisation of the regular buffer objects with the GL_ARRAY_BUFFER
         * target.
         *
         * Having this as a separate typedef helps keeping the code readable.
         */
        typedef buffer<GL_ARRAY_BUFFER> vertexBuffer;

        /**\brief Index buffer object
         *
         * When drawing primitives in OpenGL it is often best to reuse vertices
         * when possible, as vertex data can get rather big compared to simple
         * indices. When drawing objects it is thus possible to provide two
         * buffers: a GL_ARRAY_BUFFER with vertex data, and a
         * GL_ELEMENT_ARRAY_BUFFER with indices into this 'vertex buffer'.
         *
         * This typedef helps keeping the code legible in spite of the
         * misleading OpenGL naming scheme by calling this type of buffer what
         * it is: an index buffer.
         */
        typedef buffer<GL_ELEMENT_ARRAY_BUFFER> indexBuffer;

        /**\brief Vertex array object
         *
         * Wrapper for OpenGL vertex array objects and the associated state.
         *
         * \tparam Q Base data type for calculations.
         */
        template<typename Q>
        class vertexArray
        {
            public:
                /**\brief Default constructor
                 *
                 * Initialise an empty instance of this class. The actual OpenGL
                 * vertex array object is not created yet, this is done with the
                 * use() method.
                 */
                vertexArray(void)
                    : vertexArrayID(0) {}

#if !defined(NOVAO)
                /**\brief Destructor
                 *
                 * Deletes the OpenGL vertex array object if it has been
                 * created; does nothing otherwise.
                 */
                ~vertexArray (void)
                {
                    if (vertexArrayID)
                    {
                        glDeleteVertexArrays(1, &vertexArrayID);
                    }
                }
#endif

                /**\brief Use vertex array object
                 *
                 * Binds the vertex array object in the current OpenGL context.
                 * If the vertex array object does not exist yet then it is
                 * created, first.
                 *
                 * \return True on success, false otherwise.
                 */
                bool use (void)
                {
#if !defined(NOVAO)
                    if (!vertexArrayID)
                    {
                        glGenVertexArrays(1, &vertexArrayID);
                    }

                    if (vertexArrayID)
                    {
                        glBindVertexArray(vertexArrayID);
                        return true;
                    }
#endif

                    return false;
                }

                /**\brief Use but do not create vertex array object
                 *
                 * Binds the vertex array object in the current OpenGL context.
                 * If the vertex array object does not exist yet then it is not
                 * created either, the function simply fails.
                 *
                 * \return True on success, false otherwise.
                 */
                bool use (void) const
                {
#if !defined(NOVAO)
                    if (vertexArrayID)
                    {
                        glBindVertexArray(vertexArrayID);
                        return true;
                    }
#endif

                    return false;
                }

            protected:
                /**\brief Vertex array ID
                 *
                 * This is the vertex array ID as assigned by OpenGL. If the
                 * vertex array object has not been created yet then this
                 * variable is set to zero.
                 */
                GLuint vertexArrayID;
        };

        /**\brief Minimal vertex array object
         *
         * Wrapper for a minimal vertex array object. 'Minimal' in this case
         * means that the vertex data only contains spatial information in 3D.
         *
         * \tparam Q Base data type for calculations.
         */
        template<typename Q>
        class vertexArrayMinimal : public vertexArray<Q>
        {
            public:
                /**\brief Use vertex array object
                 *
                 * Uses the vertexArray<Q>::use() method to bind or create a
                 * vertex array object. If the object could not be created - for
                 * example because the NOVAO macro is set - or if the object has
                 * been bound for the first time, it will modify the internal
                 * state so that a call to setup() will enable all the
                 * appropriate vertex array attributes as necessary.
                 */
                bool use (void)
                {
                    hadID = vertexArray<Q>::vertexArrayID != 0;
                    hasBound = vertexArray<Q>::use();

                    return hasBound;
                }

                /**\brief Setup vertex array attributes
                 *
                 * Enables the necessary vertex array attribute and pointer data
                 * for this vertex array object.
                 */
                bool setup (void) const
                {
                    if (!hasBound || !hadID)
                    {
                        glEnableVertexAttribArray(attributePosition);
                        glVertexAttribPointer(attributePosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
                        glDisableVertexAttribArray(attributeNormal);
                        glDisableVertexAttribArray(attributeIndex);
                    }

                    return true;
                }

            protected:
                /**\brief Whether the vertexArrayID existed before calling use()
                 *
                 * The use() method checks whether an ID existed before calling
                 * the vertexArray<Q>::use() method. The setup() method needs to
                 * know this in case binding the vertex array object succeeded
                 * for the first time, so it can set up the vertex array buffer
                 * as necessary.
                 */
                bool hadID;
                
                /**\brief Whether the parent use() method succeeded
                 *
                 * The use() method sets this flag to false in case the parent's
                 * use() method was unable to bind a vertex array object. In
                 * that case the setup() method will always set up the vertex
                 * attributes, regardless of whether the vertex array object
                 * was bound for the first time.
                 */
                bool hasBound;
        };

        /**\brief Extended vertex array object
         *
         * Wrapper for an extended vertex array object; i.e. one with 3D spatial
         * and normal data and an IFS function index.
         *
         * \tparam Q Base data type for calculations.
         */
        template<typename Q>
        class vertexArrayExtended : public vertexArray<Q>
        {
            public:
                /**\brief Use vertex array object
                 *
                 * Uses the vertexArray<Q>::use() method to bind or create a
                 * vertex array object. If the object could not be created - for
                 * example because the NOVAO macro is set - or if the object has
                 * been bound for the first time, it will modify the internal
                 * state so that a call to setup() will enable all the
                 * appropriate vertex array attributes as necessary.
                 */
                bool use (void)
                {
                    hadID = vertexArray<Q>::vertexArrayID != 0;
                    hasBound = vertexArray<Q>::use();
                    
                    return hasBound;
                }

                using vertexArray<Q>::use;

                /**\brief Setup vertex array attributes
                 *
                 * Enables the necessary vertex array attribute and pointer data
                 * for this vertex array object.
                 */
                bool setup (void)
                {
                    if (!hasBound || !hadID)
                    {
                        
                        glEnableVertexAttribArray(attributePosition);
                        glVertexAttribPointer(attributePosition, 3, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), 0);
                        glEnableVertexAttribArray(attributeNormal);
                        glVertexAttribPointer(attributeNormal, 3, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
                        glEnableVertexAttribArray(attributeIndex);
                        glVertexAttribPointer(attributeIndex, 1, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), (void*)(6*sizeof(GLfloat)));

                        hasBound = true;
                        hadID = false;
                    }

                    return true;
                }

                /**\brief Setup vertex array attributes
                 *
                 * Enables the necessary vertex array attribute and pointer data
                 * for this vertex array object.
                 */
                bool setup (void) const
                {
                    if (!hasBound || !hadID)
                    {
                        glEnableVertexAttribArray(attributePosition);
                        glVertexAttribPointer(attributePosition, 3, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), 0);
                        glEnableVertexAttribArray(attributeNormal);
                        glVertexAttribPointer(attributeNormal, 3, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
                        glEnableVertexAttribArray(attributeIndex);
                        glVertexAttribPointer(attributeIndex, 1, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), (void*)(6*sizeof(GLfloat)));
                    }
                    
                    return true;
                }

            protected:
                /**\brief Whether the vertexArrayID existed before calling use()
                 * 
                 * The use() method checks whether an ID existed before calling
                 * the vertexArray<Q>::use() method. The setup() method needs to
                 * know this in case binding the vertex array object succeeded
                 * for the first time, so it can set up the vertex array buffer
                 * as necessary.
                 */
                bool hadID;

                /**\brief Whether the parent use() method succeeded
                 *
                 * The use() method sets this flag to false in case the parent's
                 * use() method was unable to bind a vertex array object. In
                 * that case the setup() method will always set up the vertex
                 * attributes, regardless of whether the vertex array object
                 * was bound for the first time.
                 */
                bool hasBound;
        };
    };
};

#endif