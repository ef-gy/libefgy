/**\file
 * \brief OpenGL renderer
 *
 * Code to render geometric primitives to an OpenGL 3.2/OpenGL ES/WebGL context.
 * Higher-dimensional primitives will be reduced to 3D and are then passed to
 * the graphics card to render.
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

#if !defined(EF_GY_RENDER_OPENGL_H)
#define EF_GY_RENDER_OPENGL_H

#include <ef.gy/euclidian.h>
#include <ef.gy/projection.h>
#include <vector>
#include <map>
#include <sstream>

namespace efgy
{
    /**\brief OpenGL helpers
     *
     * Templates to handle basic OpenGL tasks, such as compiling and linking
     * shader programmes.
     */
    namespace opengl
    {
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
                          { GLfloat(matrix.data[0][0]),
                            GLfloat(matrix.data[0][1]),
                            GLfloat(matrix.data[0][2]),
                            GLfloat(matrix.data[0][3]),
                            GLfloat(matrix.data[1][0]),
                            GLfloat(matrix.data[1][1]),
                            GLfloat(matrix.data[1][2]),
                            GLfloat(matrix.data[1][3]),
                            GLfloat(matrix.data[2][0]),
                            GLfloat(matrix.data[2][1]),
                            GLfloat(matrix.data[2][2]),
                            GLfloat(matrix.data[2][3]),
                            GLfloat(matrix.data[3][0]),
                            GLfloat(matrix.data[3][1]),
                            GLfloat(matrix.data[3][2]),
                            GLfloat(matrix.data[3][3]) };

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
                          { GLfloat(matrix.data[0][0]),
                            GLfloat(matrix.data[0][1]),
                            GLfloat(matrix.data[0][2]),
                            GLfloat(matrix.data[1][0]),
                            GLfloat(matrix.data[1][1]),
                            GLfloat(matrix.data[1][2]),
                            GLfloat(matrix.data[2][0]),
                            GLfloat(matrix.data[2][1]),
                            GLfloat(matrix.data[2][2]) };

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
                          { GLfloat(matrix.data[0][0]),
                            GLfloat(matrix.data[0][1]),
                            GLfloat(matrix.data[1][0]),
                            GLfloat(matrix.data[1][1]) };

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
                          { GLfloat(matrix.data[0][0]),
                            GLfloat(matrix.data[0][1]),
                            GLfloat(matrix.data[0][2]),
                            GLfloat(matrix.data[0][3]),
                            GLfloat(matrix.data[1][0]),
                            GLfloat(matrix.data[1][1]),
                            GLfloat(matrix.data[1][2]),
                            GLfloat(matrix.data[1][3]),
                            GLfloat(matrix.data[2][0]),
                            GLfloat(matrix.data[2][1]),
                            GLfloat(matrix.data[2][2]),
                            GLfloat(matrix.data[2][3]),
                            GLfloat(matrix.data[3][0]),
                            GLfloat(matrix.data[3][1]),
                            GLfloat(matrix.data[3][2]),
                            GLfloat(matrix.data[3][3]) };

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
                          { GLfloat(matrix.data[0][0]),
                            GLfloat(matrix.data[0][1]),
                            GLfloat(matrix.data[0][2]),
                            GLfloat(matrix.data[1][0]),
                            GLfloat(matrix.data[1][1]),
                            GLfloat(matrix.data[1][2]),
                            GLfloat(matrix.data[2][0]),
                            GLfloat(matrix.data[2][1]),
                            GLfloat(matrix.data[2][2]) };

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
                          { GLfloat(matrix.data[0][0]),
                            GLfloat(matrix.data[0][1]),
                            GLfloat(matrix.data[1][0]),
                            GLfloat(matrix.data[1][1]) };

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

    namespace render
    {
        static inline std::string getVertexShader (const bool fractalFlameColouring, const bool postProcess, const bool renderHistogram)
        {
            std::stringstream output ("");
            output
                << "#version 100\n";
            if (postProcess)
            {
                output
                <<  "attribute vec4 position;\n"
                    "varying lowp vec2 UV;\n"
                    "void main() {\n"
                        "gl_Position = position;\n"
                        "UV = (position.xy+vec2(1.0,1.0))/2.0;\n"
                    "}\n";
            }
            else
            {
                output
                <<  "attribute vec4 position;\n"
                    "attribute vec3 normal;\n"
                    "attribute float index;\n"
                    "varying lowp vec4 colorVarying;\n"
                    "varying lowp float indexVarying;\n"
                    "uniform mat4 modelViewProjectionMatrix;\n"
                    "uniform mat3 normalMatrix;\n"
                    "uniform vec4 colour;\n"
                    "void main() {\n"
                << (fractalFlameColouring
                      ? "indexVarying = index;\n"
                      : "vec3 eyeNormal = normalize(normalMatrix * normal);\n"
                        "vec3 lightPosition = vec3(0.0, 0.0, 1.0);\n"
                        "float nDotVP = max(0.0, dot(eyeNormal, normalize(lightPosition)));\n"
                        "colorVarying = colour * nDotVP;\n")
                <<      "gl_Position = modelViewProjectionMatrix * position;\n"
                    "}";
            }
            return output.str();
        }

        static inline std::string getFragmentShader (const bool fractalFlameColouring, const bool postProcess, const bool renderHistogram)
        {
            std::stringstream output ("");
            output
                << "#version 100\n";
            if (postProcess)
            {
                output
                <<  "varying lowp vec2 UV;\n"
                    "uniform sampler2D screenFramebuffer;\n"
                    "uniform sampler2D screenHistogram;\n"
                    "uniform sampler2D colourMap;\n"
                    "void main() {\n"
                        "highp float intensity = 1.0 - texture2D(screenHistogram, UV).x;\n"
                        "highp float index     = texture2D(screenFramebuffer, UV).x;\n"
                        "gl_FragColor = texture2D(colourMap, vec2(index,0.0)) * intensity;\n"
                    "}\n";
            }
            else
            {
                output
                <<  "varying lowp vec4 colorVarying;\n"
                    "varying lowp float indexVarying;\n"
                    "void main() {\n"
                << (fractalFlameColouring
                      ? (renderHistogram
                            ? "gl_FragColor = vec4(0.995,0.995,0.995,0.995);\n"
                            : "gl_FragColor = vec4(indexVarying,indexVarying,indexVarying,0.5);\n")
                      : "gl_FragColor = colorVarying;\n")
                <<  "}\n";
            }
            return output.str();
        }

        /**\brief OpenGL renderer
         *
         * Transforms object to a format that OpenGL can understand and then
         * renders it in an active OpenGL context. Constructs in more than 3
         * dimensions are projected into 3-space and then processed by a
         * dedicated 3D renderer.
         *
         * \tparam Q The base numeric type you intend to use.
         * \tparam d The number of dimensions for vectors.
         */
        template<typename Q, unsigned int d>
        class opengl
        {
            public:
                /**\brief Construct with matrices
                 *
                 * Constructs an OpenGL renderer with references to a
                 * transformation matrix, a projection matrix and an additional
                 * lower-dimensional renderer that values to project are passed
                 * to.
                 *
                 * \param[in]  pTransformation An affine transformation matrix
                 *    to pre-multiply vectors with when feeding them to OpenGL.
                 * \param[in]  pProjection A projective transformation that is
                 *    used to reduce the number of dimensions so the vector can
                 *    be passed to pLowerRenderer.
                 * \param[out] pLowerRenderer An instance of this template with
                 *    one spatial dimension less - used in a chain to get from
                 *    an arbitrary number of dimensions to 3 dimensions, as
                 *    OpenGL natively supports 3D graphics.
                 */
                opengl
                    (const geometry::transformation::affine<Q,d> &pTransformation,
                     const geometry::projection<Q,d> &pProjection,
                     opengl<Q,d-1> &pLowerRenderer)
                    : transformation(pTransformation), projection(pProjection), lowerRenderer(pLowerRenderer),
                      fractalFlameColouring(pLowerRenderer.fractalFlameColouring),
                      width(pLowerRenderer.width), height(pLowerRenderer.height)
                    {}

                void frameStart (void)
                {
                    combined = transformation * projection;
                    lowerRenderer.frameStart();
                };
                void frameEnd (void) const { lowerRenderer.frameEnd(); };

                template<unsigned int q>
                void drawFace
                    (const math::tuple<q, typename geometry::euclidian::space<Q,d>::vector> &pV, const Q &index = 0.5) const
                {
                    if (isPrepared()) return;
                    
                    math::tuple<q, typename geometry::euclidian::space<Q,d-1>::vector> V;
                    
                    for (unsigned int i = 0; i < q; i++)
                    {
                        V.data[i] = combined * pV.data[i];
                    }
                    
                    lowerRenderer.drawFace(V, index);
                }

                void reset (void) const { lowerRenderer.reset(); }
                const bool isPrepared (void) const { return lowerRenderer.isPrepared(); }
                bool setColour (float red, float green, float blue, float alpha, bool wireframe)
                {
                    return lowerRenderer.setColour(red,green,blue,alpha,wireframe);
                }

                void setColourMap (void)
                {
                    lowerRenderer.setColourMap();
                }

                bool &fractalFlameColouring;
                GLuint &width;
                GLuint &height;

            protected:
                const geometry::transformation::affine<Q,d> &transformation;
                const geometry::projection<Q,d> &projection;
                geometry::transformation::projective<Q,d> combined;
                opengl<Q,d-1> &lowerRenderer;

                void pushLines (void) const { lowerRenderer.pushLines(); };
                void pushFaces (void) const { lowerRenderer.pushFaces(); };
        };

        /**\brief OpenGL renderer (3D fix point)
         *
         * This is the 3D fix point for the OpenGL renderer. 3D is the natural
         * render depth for OpenGL, so whenever things are rendered in 3D the
         * rendering can be passed directly to OpenGL, which is what this class
         * does.
         *
         * \tparam Q The data type to use for calculations.
         */
        template<typename Q>
        class opengl<Q,3>
        {
            public:
                /**\brief Construct with matrices
                 *
                 * Constructs an OpenGL renderer with references to a
                 * transformation matrix, a projection matrix and an additional
                 * lower-dimensional renderer that values to project are passed
                 * to.
                 *
                 * \param[in]  pTransformation An affine transformation matrix
                 *    to pre-multiply vectors with when feeding them to OpenGL.
                 * \param[in]  pProjection A projective transformation that is
                 *    used to reduce the number of dimensions so the vector can
                 *    be passed to pLowerRenderer.
                 * \param[out] pLowerRenderer An instance of this template with
                 *    one spatial dimension less - used in a chain to get from
                 *    an arbitrary number of dimensions to 3 dimensions, as
                 *    OpenGL natively supports 3D graphics.
                 */
                opengl
                    (const geometry::transformation::affine<Q,3> &pTransformation,
                     const geometry::projection<Q,3> &pProjection,
                     opengl<Q,2> &pLowerRenderer)
                    : transformation(pTransformation), projection(pProjection),
                      haveBuffers(false), prepared(false),
                      regular(getVertexShader(false, false, false), getFragmentShader(false, false, false)),
                      flameColouring(getVertexShader(true, false, false), getFragmentShader(true, false, false)),
                      flameHistogram(getVertexShader(true, false, true), getFragmentShader(true, false, true)),
                      flamePostProcess(getVertexShader(true, true, false), getFragmentShader(true, true, false)),
                      width(pLowerRenderer.width), height(pLowerRenderer.height)
                    {
                    }

                void frameStart (void)
                {
                    if(!haveBuffers)
                    {
                        haveBuffers = true;
                        
                        regular.copy();
                        flamePostProcess.copy();

                        static const GLfloat fullscreenQuadBufferData[] =
                        {
                            -1.0f, -1.0f,  0.0f,
                             1.0f, -1.0f,  0.0f,
                            -1.0f,  1.0f,  0.0f,
                            -1.0f,  1.0f,  0.0f,
                             1.0f, -1.0f,  0.0f,
                             1.0f,  1.0f,  0.0f
                        };

                        vertexArrayFullscreenQuad.use();
                        vertexbufferFullscreenQuad.load(sizeof(fullscreenQuadBufferData), fullscreenQuadBufferData);
                        vertexArrayFullscreenQuad.setup();

                        setColourMap();
                    }

                    const geometry::transformation::projective<Q,3> combined = transformation * projection;
                    const math::matrix<Q,3,3> normalMatrix = math::transpose(math::invert(math::transpose(math::matrix<Q,3,3>(transformation.transformationMatrix))));

                    if (fractalFlameColouring)
                    {
                        flameHistogram.uniform(efgy::opengl::uniformProjectionMatrix, combined.transformationMatrix);
                        flameHistogram.uniform(efgy::opengl::uniformNormalMatrix, normalMatrix);

                        flameColouring.uniform(efgy::opengl::uniformProjectionMatrix, combined.transformationMatrix);
                        flameColouring.uniform(efgy::opengl::uniformNormalMatrix, normalMatrix);
                    }
                    else
                    {
                        regular.uniform(efgy::opengl::uniformProjectionMatrix, combined.transformationMatrix);
                        regular.uniform(efgy::opengl::uniformNormalMatrix, normalMatrix);
                    }
                };

                void frameEnd (void)
                {
                    if (!prepared)
                    {
                        prepared = true;

                        vertexArrayModel.use();
                        vertexbuffer.load(vertices.size() * sizeof(GLfloat), &vertices[0]);
                        elementbuffer.load(triindices.size() * sizeof(unsigned int), &triindices[0]);
                        linebuffer.load(lineindices.size() * sizeof(unsigned int), &lineindices[0]);
                        vertexArrayModel.setup();

                        tindices = GLsizei(triindices.size());
                        lindices = GLsizei(lineindices.size());

                        vertices.clear();
                        triindices.clear();
                        lineindices.clear();
                        indices = 0;
                    }

                    if (fractalFlameColouring)
                    {
                        glClearColor(1,1,1,1);

                        flameColouring.use(width, height, 0);

                        glDepthMask(GL_FALSE);
                        glBlendFunc (GL_SRC_ALPHA, GL_SRC_ALPHA);
                        glDisable(GL_DEPTH_TEST);

                        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                        pushFaces();

                        flameHistogram.use(width, height, 1);

//                        glClearColor(0,0,0,1);
                        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//                        glBlendFunc (GL_ONE, GL_ONE);
                        glBlendFunc (GL_ZERO, GL_SRC_COLOR);
 
                        pushFaces();

                        flamePostProcess.use(width, height);

                        flamePostProcess.uniform(efgy::opengl::uniformScreenFramebuffer, 0);
                        flamePostProcess.uniform(efgy::opengl::uniformScreenHistogram, 1);

                        glActiveTexture(GL_TEXTURE0 + 2);
                        textureFlameColourMap.bind();
                        flamePostProcess.uniform(efgy::opengl::uniformColourMap, 2);

                        glBlendFunc (GL_ONE, GL_ZERO);

                        vertexArrayFullscreenQuad.use();
                        vertexbufferFullscreenQuad.bind();
                        vertexArrayFullscreenQuad.setup();

                        glDrawArrays(GL_TRIANGLES, 0, 6);
                    }
                    else
                    {
                        regular.use(width, height);
                        glDepthMask(GL_TRUE);
                        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                        glEnable(GL_DEPTH_TEST);
                        glDepthFunc(GL_LEQUAL);

                        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                        pushLines();
                        pushFaces();
                    }
                };

                template<unsigned int q>
                void drawFace
                    (const math::tuple<q, typename geometry::euclidian::space<Q,3>::vector> &pV, const Q &index = 0.5)
                {
                    if (isPrepared()) return;

                    typename geometry::euclidian::space<Q,3>::vector R
                        = geometry::euclidian::normalise<Q,3>
                            (geometry::euclidian::crossProduct<Q>
                                (pV.data[1] - pV.data[0], pV.data[2] - pV.data[0]));

                    typename geometry::euclidian::space<Q,3>::vector RN
                        = geometry::euclidian::normalise<Q,3>
                            (geometry::euclidian::crossProduct<Q>
                        (pV.data[2] - pV.data[0], pV.data[1] - pV.data[0]));

                    triindices.push_back(addVertex(GLfloat(pV.data[0].data[0]), GLfloat(pV.data[0].data[1]), GLfloat(pV.data[0].data[2]),
                                                   GLfloat(R.data[0]), GLfloat(R.data[1]), GLfloat(R.data[2]), GLfloat(index)));
                    unsigned int nstartf = triindices.back();
                    lineindices.push_back(nstartf);
                    triindices.push_back(addVertex(GLfloat(pV.data[1].data[0]), GLfloat(pV.data[1].data[1]), GLfloat(pV.data[1].data[2]),
                                                   GLfloat(R.data[0]), GLfloat(R.data[1]), GLfloat(R.data[2]), GLfloat(index)));
                    lineindices.push_back(triindices.back());
                    lineindices.push_back(triindices.back());
                    triindices.push_back(addVertex(GLfloat(pV.data[2].data[0]), GLfloat(pV.data[2].data[1]), GLfloat(pV.data[2].data[2]),
                                                   GLfloat(R.data[0]), GLfloat(R.data[1]), GLfloat(R.data[2]), GLfloat(index)));
                    unsigned int nendf = triindices.back();
                    lineindices.push_back(nendf);

                    triindices.push_back(addVertex(GLfloat(pV.data[2].data[0]), GLfloat(pV.data[2].data[1]), GLfloat(pV.data[2].data[2]),
                                                   GLfloat(RN.data[0]), GLfloat(RN.data[1]), GLfloat(RN.data[2]), GLfloat(index)));
                    unsigned int nendb = triindices.back();
                    lineindices.push_back(nendb);
                    triindices.push_back(addVertex(GLfloat(pV.data[1].data[0]), GLfloat(pV.data[1].data[1]), GLfloat(pV.data[1].data[2]),
                                                   GLfloat(RN.data[0]), GLfloat(RN.data[1]), GLfloat(RN.data[2]), GLfloat(index)));
                    lineindices.push_back(triindices.back());
                    lineindices.push_back(triindices.back());
                    triindices.push_back(addVertex(GLfloat(pV.data[0].data[0]), GLfloat(pV.data[0].data[1]), GLfloat(pV.data[0].data[2]),
                                                   GLfloat(RN.data[0]), GLfloat(RN.data[1]), GLfloat(RN.data[2]), GLfloat(index)));
                    unsigned int nstartb = triindices.back();
                    lineindices.push_back(nstartb);

                    for (unsigned int j = 3; j < q; j++)
                    {
                        triindices.push_back(addVertex(GLfloat(pV.data[0].data[0]), GLfloat(pV.data[0].data[1]), GLfloat(pV.data[0].data[2]),
                                                       GLfloat(R.data[0]), GLfloat(R.data[1]), GLfloat(R.data[2]), GLfloat(index)));
                        triindices.push_back(addVertex(GLfloat(pV.data[(j-1)].data[0]), GLfloat(pV.data[(j-1)].data[1]), GLfloat(pV.data[(j-1)].data[2]),
                                                       GLfloat(R.data[0]), GLfloat(R.data[1]), GLfloat(R.data[2]), GLfloat(index)));
                        lineindices.push_back(triindices.back());
                        triindices.push_back(addVertex(GLfloat(pV.data[j].data[0]), GLfloat(pV.data[j].data[1]), GLfloat(pV.data[j].data[2]),
                                                       GLfloat(R.data[0]), GLfloat(R.data[1]), GLfloat(R.data[2]), GLfloat(index)));
                        lineindices.push_back(triindices.back());
                        nendf = triindices.back();

                        triindices.push_back(addVertex(GLfloat(pV.data[j].data[0]), GLfloat(pV.data[j].data[1]), GLfloat(pV.data[j].data[2]),
                                                       GLfloat(RN.data[0]), GLfloat(RN.data[1]), GLfloat(RN.data[2]), GLfloat(index)));
                        nendb = triindices.back();
                        lineindices.push_back(triindices.back());
                        triindices.push_back(addVertex(GLfloat(pV.data[(j-1)].data[0]), GLfloat(pV.data[(j-1)].data[1]), GLfloat(pV.data[(j-1)].data[2]),
                                                       GLfloat(RN.data[0]), GLfloat(RN.data[1]), GLfloat(RN.data[2]), GLfloat(index)));
                        lineindices.push_back(triindices.back());
                        triindices.push_back(addVertex(GLfloat(pV.data[0].data[0]), GLfloat(pV.data[0].data[1]), GLfloat(pV.data[0].data[2]),
                                                       GLfloat(RN.data[0]), GLfloat(RN.data[1]), GLfloat(RN.data[2]), GLfloat(index)));
                    }

                    lineindices.push_back(nendf);
                    lineindices.push_back(nstartf);
                    lineindices.push_back(nstartb);
                    lineindices.push_back(nendb);
                }

                void reset (void) { prepared = false; }
                const bool isPrepared (void) const { return prepared; }

                unsigned int addVertex
                    (const GLfloat &x, const GLfloat &y, const GLfloat &z,
                     const GLfloat &nx, const GLfloat &ny, const GLfloat &nz,
                     const GLfloat &index)
                {
                    vertices.push_back(x);
                    vertices.push_back(y);
                    vertices.push_back(z);

                    vertices.push_back(nx);
                    vertices.push_back(ny);
                    vertices.push_back(nz);

                    vertices.push_back(index);

                    unsigned int rv = indices;

                    indices++;

                    return rv;
                }

                bool setColour (float pRed, float pGreen, float pBlue, float pAlpha, bool pWireframe)
                {
                    if (pWireframe)
                    {
                        linesEnabled = pAlpha > 0.f;
                        lineDepthMask = true;
                        wireframeColour[0] = pRed;
                        wireframeColour[1] = pGreen;
                        wireframeColour[2] = pBlue;
                        wireframeColour[3] = pAlpha;
                    }
                    else
                    {
                        facesEnabled = pAlpha > 0.f;
                        faceDepthMask = pAlpha >= 1.f;
                        surfaceColour[0] = pRed;
                        surfaceColour[1] = pGreen;
                        surfaceColour[2] = pBlue;
                        surfaceColour[3] = pAlpha;
                    }
                    return true;
                }

                void setColourMap (void)
                {
                    std::vector<unsigned char> colours;
                    
                    for (unsigned int i = 0; i < 8; i++)
                    {
                        colours.push_back(std::rand()%255);
                        colours.push_back(std::rand()%255);
                        colours.push_back(std::rand()%255);
                    }

                    textureFlameColourMap.load(GLsizei(colours.size()/3), 1, &colours[0]);
                    
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                }

                bool fractalFlameColouring;
                GLuint &width;
                GLuint &height;

            protected:
                const geometry::transformation::affine<Q,3> &transformation;
                const geometry::projection<Q,3> &projection;
                std::vector<GLfloat> vertices;
                std::vector<unsigned int> triindices;
                std::vector<unsigned int> lineindices;
                unsigned int indices;
                GLsizei tindices;
                GLsizei lindices;
                bool haveBuffers;
                bool prepared;

                efgy::opengl::vertexArrayExtended<Q> vertexArrayModel;
                efgy::opengl::vertexArrayMinimal<Q> vertexArrayFullscreenQuad;

                efgy::opengl::vertexBuffer vertexbuffer;
                efgy::opengl::vertexBuffer vertexbufferFullscreenQuad;
                efgy::opengl::indexBuffer elementbuffer;
                efgy::opengl::indexBuffer linebuffer;

                bool linesEnabled;
                bool facesEnabled;
                bool lineDepthMask;
                bool faceDepthMask;
                GLfloat wireframeColour[4];
                GLfloat surfaceColour[4];
                efgy::opengl::texture2D textureFlameColourMap;
                efgy::opengl::renderToFramebufferProgramme<Q> regular;
                efgy::opengl::renderToTextureProgramme<Q> flameColouring;
                efgy::opengl::renderToTextureProgramme<Q> flameHistogram;
                efgy::opengl::renderToFramebufferProgramme<Q> flamePostProcess;

                void pushLines (void) const
                {
                    if (prepared && linesEnabled && !fractalFlameColouring)
                    {
                        regular.uniform(efgy::opengl::uniformColour, wireframeColour);

                        glDepthMask (lineDepthMask ? GL_TRUE : GL_FALSE);

                        vertexArrayModel.use();
                        vertexbuffer.bind();
                        linebuffer.bind();
                        vertexArrayModel.setup();

                        glDrawElements(GL_LINES, lindices, GL_UNSIGNED_INT, 0);
                    }
                }
            
                void pushFaces (void) const
                {
                    if (prepared && (facesEnabled || fractalFlameColouring))
                    {
                        if (!fractalFlameColouring)
                        {
                            regular.uniform(efgy::opengl::uniformColour, surfaceColour);
                        }

                        glDepthMask (faceDepthMask ? GL_TRUE : GL_FALSE);

                        vertexArrayModel.use();
                        vertexbuffer.bind();
                        elementbuffer.bind();
                        vertexArrayModel.setup();

                        glDrawElements(GL_TRIANGLES, tindices, GL_UNSIGNED_INT, 0);
                    }
                }
        };

        /**\brief OpenGL renderer (2D fix point)
         *
         * This is the 2D fix point for the OpenGL. Having this template makes
         * the code a bit more symmetric and easier to use, because the other
         * renderers all have a 2D fix point as well. OpenGL would have a 3D
         * fix point instead, which would require separate 2D and 3D handlers
         * depending on what type of output you're trying to produce.
         *
         * \tparam Q The data type to use for calculations.
         */
        template<typename Q>
        class opengl<Q,2>
        {
            public:
                /**\brief Construct with affine transformation
                 *
                 * Initialise an object instance with an affine transformation
                 * matrix. This matrix is actually ignored, as the 'real' fix
                 * point for the OpenGL renderer is in 3D, not 2D.
                 */
                opengl (const geometry::transformation::affine<Q,2> &) {}

                GLuint width;
                GLuint height;
        };
    };
};

#endif
