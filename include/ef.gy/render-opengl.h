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

                /**\brief Default uniforms
                 *
                 * Contains the indices of the default uniforms. This array is
                 * populated after linking the shaders.
                 */
                GLint uniforms[uniformMax];

            protected:
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

        /**\brief Framebuffer object
         *
         * Encapsulates an OpenGL texture object and the state associated
         * with it.
         *
         * \tparam Q Base data type for calculations.
         */
        template<typename Q>
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
                    : textureID(0), target(0) {}

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
                 * \param[in] target The texture target to bind to.
                 *
                 * \return True on success, false otherwise.
                 *
                 * \see http://www.opengl.org/sdk/docs/man/xhtml/glBindTexture.xml
                 *      for the possible values of the target parameter.
                 */
                bool bind (const GLenum &target = GL_TEXTURE_2D)
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
                 * \param[in] pFormat Format of the texture to load or create.
                 * \param[in] pType   Type of the texture to load.
                 * \param[in] data    Raw memory data of the texture to load;
                 *                    use zero if you only want to create a
                 *                    texture and not load one from memory.
                 * \param[in] pTarget The texture target to bind to.
                 *
                 * \return True on success, false otherwise.
                 *
                 * \see http://www.opengl.org/sdk/docs/man/xhtml/glTexImage2D.xml
                 *      for the possible values of the GLenum parameters.
                 */
                bool load (const GLuint &pWidth, const GLuint &pHeight,
                           const GLenum &pFormat = GL_RGB,
                           const GLenum &pType = GL_UNSIGNED_BYTE,
                           const void *data = 0,
                           const GLenum &pTarget = GL_TEXTURE_2D)
                {
                    if (bind(pTarget))
                    {
                        if (   (pWidth != width) || (pHeight != height)
                            || (format != pFormat) || (type != pType)
                            || (pTarget != target) || (data != 0))
                        {
                            width  = pWidth;
                            height = pHeight;
                            target = pTarget;
                            format = pFormat;
                            type   = pType;

                            glTexImage2D
                                (target, 0, format,
                                 width, height, 0, format, type, data);
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
                /**\brief Last texture target
                 *
                 * This is the texture target that was passed to the load()
                 * method the last time it was used.
                 *
                 * Used to figure out if the texture needs to be recreated if
                 * you use the load() method.
                 *
                 * \see http://www.opengl.org/sdk/docs/man/xhtml/glTexImage2D.xml
                 *      for possible values of this enum.
                 */
                GLenum target;

                /**\brief Current texture format
                 *
                 * This is the internal and external texture format that was
                 * passed to the load() method the last time it was used.
                 *
                 * Used to figure out if the texture needs to be recreated if
                 * you use the load() method.
                 *
                 * \see http://www.opengl.org/sdk/docs/man/xhtml/glTexImage2D.xml
                 *      for possible values of this enum.
                 */
                GLenum format;

                /**\brief Texture type
                 *
                 * The texture type that was used to load the last texture with
                 * load().
                 *
                 * Used to figure out if the texture needs to be recreated if
                 * you use the load() method.
                 *
                 * \see http://www.opengl.org/sdk/docs/man/xhtml/glTexImage2D.xml
                 *      for possible values of this enum.
                 */
                GLenum type;

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
                            ? "gl_FragColor = vec4(1,1,1,0.992);\n"
                            : "gl_FragColor = vec4(indexVarying,indexVarying,indexVarying,0.5);\n")
                      : "gl_FragColor = colorVarying;\n")
                <<  "}\n";
            }
            return output.str();
        }

        template<typename Q, unsigned int d>
        class opengl
        {
            public:
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

        template<typename Q>
        class opengl<Q,3>
        {
            public:
                opengl
                    (const geometry::transformation::affine<Q,3> &pTransformation,
                     const geometry::projection<Q,3> &pProjection,
                     const opengl<Q,2> &)
                    : transformation(pTransformation), projection(pProjection),
                      haveBuffers(false), prepared(false),
                      regular(getVertexShader(false, false, false), getFragmentShader(false, false, false)),
                      flameColouring(getVertexShader(true, false, false), getFragmentShader(true, false, false)),
                      flameHistogram(getVertexShader(true, false, true), getFragmentShader(true, false, true)),
                      flamePostProcess(getVertexShader(true, true, false), getFragmentShader(true, true, false))
                    {
                    }

                ~opengl (void)
                    {
                        if (haveBuffers)
                        {
                            glDeleteBuffers(1, &vertexbufferFullscreenQuad);
                            glDeleteBuffers(1, &vertexbuffer);
                            glDeleteBuffers(1, &elementbuffer);
                            glDeleteBuffers(1, &linebuffer);
                        }
                    }

                void frameStart (void)
                {
                    if(!haveBuffers)
                    {
                        haveBuffers = true;
                        
                        framebufferOriginal.copy();

                        framebufferFlameColouring.use();
                        framebufferFlameHistogram.use();

#if !defined(NOVAO)
                        glGenVertexArrays(1, &vertexArrayFullscreenQuad);
                        glBindVertexArray(vertexArrayFullscreenQuad);
#endif
                        glGenBuffers(1, &vertexbufferFullscreenQuad);
                        glBindBuffer(GL_ARRAY_BUFFER, vertexbufferFullscreenQuad);

                        static const GLfloat fullscreenQuadBufferData[] =
                        {
                            -1.0f, -1.0f,  0.0f,
                             1.0f, -1.0f,  0.0f,
                            -1.0f,  1.0f,  0.0f,
                            -1.0f,  1.0f,  0.0f,
                             1.0f, -1.0f,  0.0f,
                             1.0f,  1.0f,  0.0f
                        };
                        
                        glBufferData(GL_ARRAY_BUFFER, sizeof(fullscreenQuadBufferData), fullscreenQuadBufferData, GL_STATIC_DRAW);
#if !defined(NOVAO)
                        glEnableVertexAttribArray(efgy::opengl::attributePosition);
                        glVertexAttribPointer(efgy::opengl::attributePosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
#endif
                        glBindBuffer(GL_ARRAY_BUFFER, 0);

#if !defined(NOVAO)
                        glGenVertexArrays(1, &vertexArrayModel);
#endif
                        glGenBuffers(1, &vertexbuffer);
                        glGenBuffers(1, &elementbuffer);
                        glGenBuffers(1, &linebuffer);

                        setColourMap();
                    }

                    glViewport(0, 0, width, height);

                    const geometry::transformation::projective<Q,3> combined = transformation * projection;

                    GLfloat mat[16] =
                      { GLfloat(combined.transformationMatrix.data[0][0]),
                        GLfloat(combined.transformationMatrix.data[0][1]),
                        GLfloat(combined.transformationMatrix.data[0][2]),
                        GLfloat(combined.transformationMatrix.data[0][3]),
                        GLfloat(combined.transformationMatrix.data[1][0]),
                        GLfloat(combined.transformationMatrix.data[1][1]),
                        GLfloat(combined.transformationMatrix.data[1][2]),
                        GLfloat(combined.transformationMatrix.data[1][3]),
                        GLfloat(combined.transformationMatrix.data[2][0]),
                        GLfloat(combined.transformationMatrix.data[2][1]),
                        GLfloat(combined.transformationMatrix.data[2][2]),
                        GLfloat(combined.transformationMatrix.data[2][3]),
                        GLfloat(combined.transformationMatrix.data[3][0]),
                        GLfloat(combined.transformationMatrix.data[3][1]),
                        GLfloat(combined.transformationMatrix.data[3][2]),
                        GLfloat(combined.transformationMatrix.data[3][3]) };

                    math::matrix<Q,3,3> normalMatrix;

                    for (unsigned int i = 0; i < 3; i++)
                    {
                        for (unsigned int j = 0; j < 3; j++)
                        {
                            normalMatrix.data[i][j] = transformation.transformationMatrix.data[i][j];
                        }
                    }

                    normalMatrix = math::transpose(math::invert(math::transpose(normalMatrix)));

                    GLfloat matn[9] =
                      { GLfloat(normalMatrix.data[0][0]),
                        GLfloat(normalMatrix.data[0][1]),
                        GLfloat(normalMatrix.data[0][2]),
                        GLfloat(normalMatrix.data[1][0]),
                        GLfloat(normalMatrix.data[1][1]),
                        GLfloat(normalMatrix.data[1][2]),
                        GLfloat(normalMatrix.data[2][0]),
                        GLfloat(normalMatrix.data[2][1]),
                        GLfloat(normalMatrix.data[2][2]) };

                    if (fractalFlameColouring)
                    {
                        flameHistogram.use();
                        glUniformMatrix4fv(flameHistogram.uniforms[efgy::opengl::uniformProjectionMatrix], 1, 0, mat);
                        glUniformMatrix3fv(flameHistogram.uniforms[efgy::opengl::uniformNormalMatrix], 1, 0, matn);

                        flameColouring.use();
                        for (unsigned int i = 0; i < efgy::opengl::uniformMax; i++)
                        {
                            uniforms[i] = flameColouring.uniforms[i];
                        }
                    }
                    else
                    {
                        regular.use();
                        for (unsigned int i = 0; i < efgy::opengl::uniformMax; i++)
                        {
                            uniforms[i] = regular.uniforms[i];
                        }
                    }

                    glUniformMatrix4fv(uniforms[efgy::opengl::uniformProjectionMatrix], 1, 0, mat);
                    glUniformMatrix3fv(uniforms[efgy::opengl::uniformNormalMatrix], 1, 0, matn);
                };

                void frameEnd (void)
                {
                    if (!prepared)
                    {
                        prepared = true;

#if !defined(NOVAO)
                        glBindVertexArray(vertexArrayModel);
#endif
                        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
                        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
                        glBufferData(GL_ELEMENT_ARRAY_BUFFER, triindices.size() * sizeof(unsigned int), &triindices[0], GL_STATIC_DRAW);
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, linebuffer);
                        glBufferData(GL_ELEMENT_ARRAY_BUFFER, lineindices.size() * sizeof(unsigned int), &lineindices[0], GL_STATIC_DRAW);
#if !defined(NOVAO)
                        glEnableVertexAttribArray(efgy::opengl::attributePosition);
                        glVertexAttribPointer(efgy::opengl::attributePosition, 3, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), 0);
                        glEnableVertexAttribArray(efgy::opengl::attributeNormal);
                        glVertexAttribPointer(efgy::opengl::attributeNormal, 3, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
                        glEnableVertexAttribArray(efgy::opengl::attributeIndex);
                        glVertexAttribPointer(efgy::opengl::attributeIndex, 1, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), (void*)(6*sizeof(GLfloat)));
#endif

                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                        glBindBuffer(GL_ARRAY_BUFFER, 0);
#if !defined(NOVAO)
                        glBindVertexArray(0);
#endif

                        tindices = GLsizei(triindices.size());
                        lindices = GLsizei(lineindices.size());

                        vertices.clear();
                        triindices.clear();
                        lineindices.clear();
                        indices = 0;
                    }

                    if (fractalFlameColouring)
                    {
                        const unsigned int width = 2048, height = 2048;
                        glViewport(0, 0, width, height);

                        flameColouring.use();
                        framebufferFlameColouring.use();

                        glActiveTexture(GL_TEXTURE0 + 0);

                        textureFlameColouring.load(width, height);
                        
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

                        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureFlameColouring.textureID, 0);

                        glDepthMask(GL_FALSE);
                        glBlendFunc (GL_SRC_ALPHA, GL_SRC_ALPHA);
                        glDisable(GL_DEPTH_TEST);

                        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                        pushFaces();

                        flameHistogram.use();
                        framebufferFlameHistogram.use();

                        glActiveTexture(GL_TEXTURE0 + 1);

                        textureFlameHistogram.load(width, height);
                        
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

                        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureFlameHistogram.textureID, 0);
                        
                        glBlendFunc (GL_ZERO, GL_SRC_ALPHA);

                        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
                        pushFaces();

                        flamePostProcess.use();
                        glViewport(0, 0, this->width, this->height);

                        glUniform1i(flamePostProcess.uniforms[efgy::opengl::uniformScreenFramebuffer], 0);
                        glUniform1i(flamePostProcess.uniforms[efgy::opengl::uniformScreenHistogram], 1);

                        glActiveTexture(GL_TEXTURE0 + 2);
                        textureFlameColourMap.bind(GL_TEXTURE_2D);
                        glUniform1i(flamePostProcess.uniforms[efgy::opengl::uniformColourMap], 2);

                        glBlendFunc (GL_ONE, GL_ZERO);

                        framebufferOriginal.use();

#if !defined(NOVAO)
                        glBindVertexArray(vertexArrayFullscreenQuad);
#endif
                        glBindBuffer(GL_ARRAY_BUFFER, vertexbufferFullscreenQuad);

#if defined(NOVAO)
                        glEnableVertexAttribArray(attributePosition);
                        glVertexAttribPointer(attributePosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
#endif

                        glDrawArrays(GL_TRIANGLES, 0, 6);

                        glBlendFunc (GL_SRC_ALPHA, GL_SRC_ALPHA);
                    }
                    else
                    {
                        regular.use();
                        glDepthMask(GL_TRUE);
                        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                        glEnable(GL_DEPTH_TEST);
                        glDepthFunc(GL_LEQUAL);

                        framebufferOriginal.use();

                        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                        pushLines();
                        pushFaces();
                    }
                };

                template<unsigned int q>
                void drawFace
                    (const math::tuple<q, typename geometry::euclidian::space<Q,3>::vector> &pV, const Q &index = 0.5);

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
                        colours.push_back(255);
                    }

                    textureFlameColourMap.load(GLsizei(colours.size()/4), 1, GL_RGBA, GL_UNSIGNED_BYTE, &colours[0]);
                    
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                }

                bool fractalFlameColouring;
                GLuint width;
                GLuint height;

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
#if !defined(NOVAO)
                GLuint vertexArrayModel;
                GLuint vertexArrayFullscreenQuad;
#endif
                GLuint vertexbuffer;
                GLuint vertexbufferFullscreenQuad;
                GLuint elementbuffer;
                GLuint linebuffer;
                GLint uniforms[efgy::opengl::uniformMax];
                bool linesEnabled;
                bool facesEnabled;
                bool lineDepthMask;
                bool faceDepthMask;
                GLfloat wireframeColour[4];
                GLfloat surfaceColour[4];
                efgy::opengl::framebuffer<Q> framebufferFlameColouring;
                efgy::opengl::framebuffer<Q> framebufferFlameHistogram;
                efgy::opengl::framebuffer<Q> framebufferOriginal;
                efgy::opengl::texture<Q> textureFlameColouring;
                efgy::opengl::texture<Q> textureFlameHistogram;
                efgy::opengl::texture<Q> textureFlameColourMap;
                efgy::opengl::programme<Q> regular;
                efgy::opengl::programme<Q> flameColouring;
                efgy::opengl::programme<Q> flameHistogram;
                efgy::opengl::programme<Q> flamePostProcess;

                void pushLines (void) const
                {
                    if (prepared && linesEnabled && !fractalFlameColouring)
                    {
                        glUniform4f(uniforms[efgy::opengl::uniformColour], wireframeColour[0], wireframeColour[1], wireframeColour[2], wireframeColour[3]);

                        glDepthMask (lineDepthMask ? GL_TRUE : GL_FALSE);

#if !defined(NOVAO)
                        glBindVertexArray(vertexArrayModel);
#endif
                        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, linebuffer);

#if defined(NOVAO)
                        glEnableVertexAttribArray(attributePosition);
                        glVertexAttribPointer(attributePosition, 3, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), 0);
                        glEnableVertexAttribArray(attributeNormal);
                        glVertexAttribPointer(attributeNormal, 3, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
                        glEnableVertexAttribArray(attributeIndex);
                        glVertexAttribPointer(attributeIndex, 1, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), (void*)(6*sizeof(GLfloat)));
#endif

                        glDrawElements(GL_LINES, lindices, GL_UNSIGNED_INT, 0);
                        
                        glBindBuffer(GL_ARRAY_BUFFER, 0);
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#if !defined(NOVAO)
                        glBindVertexArray(0);
#endif
                    }
                }
            
                void pushFaces (void) const
                {
                    if (prepared && (facesEnabled || fractalFlameColouring))
                    {
                        glUniform4f(uniforms[efgy::opengl::uniformColour], surfaceColour[0], surfaceColour[1], surfaceColour[2], surfaceColour[3]);

                        glDepthMask (faceDepthMask ? GL_TRUE : GL_FALSE);

#if !defined(NOVAO)
                        glBindVertexArray(vertexArrayModel);
#endif
                        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

#if defined(NOVAO)
                        glEnableVertexAttribArray(attributePosition);
                        glVertexAttribPointer(attributePosition, 3, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), 0);
                        glEnableVertexAttribArray(attributeNormal);
                        glVertexAttribPointer(attributeNormal, 3, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
                        glEnableVertexAttribArray(attributeIndex);
                        glVertexAttribPointer(attributeIndex, 1, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), (void*)(6*sizeof(GLfloat)));
#endif

                        glDrawElements(GL_TRIANGLES, tindices, GL_UNSIGNED_INT, 0);
                        
                        glBindBuffer(GL_ARRAY_BUFFER, 0);
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#if !defined(NOVAO)
                        glBindVertexArray(0);
#endif
                    }
                }
        };

        template<typename Q>
        class opengl<Q,2>
        {
            public:
                opengl (const geometry::transformation::affine<Q,2> &) {}
        };

        template<typename Q>
        template<unsigned int q>
        void opengl<Q,3>::drawFace
            (const math::tuple<q, typename geometry::euclidian::space<Q,3>::vector> &pV, const Q &index)
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
    };
};

#endif
