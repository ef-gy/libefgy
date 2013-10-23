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

#if !defined(EF_GY_RENDER_OPENGL_H)
#define EF_GY_RENDER_OPENGL_H

#include <ef.gy/euclidian.h>
#include <ef.gy/projection.h>
#include <vector>
#include <map>
#include <sstream>

namespace efgy
{
    namespace render
    {
        // Attribute index.
        enum openGLShaderAttribute
        {
            attributePosition,
            attributeNormal,
            attributeIndex
        };

        enum openGLUniforms
        {
            uniformProjectionMatrix,
            uniformNormalMatrix,
            uniformColour,
            uniformMax
        };

        static inline std::string getVertexShader (const bool fractalFlameColouring, const bool postProcess)
        {
            std::stringstream output ("#version 100\n");
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

        static inline std::string getFragmentShader (const bool fractalFlameColouring, const bool postProcess)
        {
            std::stringstream output ("#version 100\n");
            if (postProcess)
            {
                output
                <<  "varying lowp vec2 UV;\n"
                    "uniform sampler2D screenFramebuffer;\n"
                    "uniform sampler2D screenHistogram;\n"
                    "void main() {\n"
                        "gl_FragColor = texture2D(screenFramebuffer, UV);\n"
                    "}\n";
            }
            else
            {
                output
                <<  "varying lowp vec4 colorVarying;\n"
                    "varying lowp float indexVarying;\n"
                    "void main() {\n"
                << (fractalFlameColouring
                      ? "gl_FragColor = vec4(indexVarying,indexVarying,indexVarying,0.5);\n"
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
                      programRegular(0), programFlameColouring(0), programFlamePostProcess(0),
                      framebufferFlameColouring(0), framebufferFlameHistogram(0),
                      textureFlameColouring(0), textureFlameHistogram(0)
                    {
                    }

                ~opengl (void)
                    {
                        if (haveBuffers)
                        {
                            glDeleteBuffers(1, &vertexbuffer);
                            glDeleteBuffers(1, &elementbuffer);
                            glDeleteBuffers(1, &linebuffer);
                        }

                        if (programRegular) {
                            glDeleteProgram(programRegular);
                        }
                        if (programFlameColouring) {
                            glDeleteProgram(programFlameColouring);
                        }
                        if (programFlamePostProcess)
                        {
                            glDeleteProgram(programFlamePostProcess);
                        }
                        if (framebufferFlameHistogram)
                        {
                            glDeleteFramebuffers(1, &framebufferFlameHistogram);
                        }
                        if (framebufferFlameColouring)
                        {
                            glDeleteFramebuffers(1, &framebufferFlameColouring);
                        }
                        if (textureFlameColouring)
                        {
                            glDeleteTextures(1, &textureFlameColouring);
                        }
                        if (textureFlameHistogram)
                        {
                            glDeleteTextures(1, &textureFlameHistogram);
                        }
                    }

                void frameStart (void)
                {
                    if(!haveBuffers)
                    {
                        haveBuffers = true;

                        glGenFramebuffers(1, &framebufferFlameColouring);
                        glBindFramebuffer(GL_FRAMEBUFFER, framebufferFlameColouring);
                        glGenTextures(1, &textureFlameColouring);

                        glGenFramebuffers(1, &framebufferFlameHistogram);
                        glBindFramebuffer(GL_FRAMEBUFFER, framebufferFlameHistogram);
                        glGenTextures(1, &textureFlameHistogram);

                        loadShaders(programRegular, false, false);
                        loadShaders(programFlameColouring, true, false);
                        loadShaders(programFlamePostProcess, true, true);

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
                        glEnableVertexAttribArray(attributePosition);
                        glVertexAttribPointer(attributePosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
#endif
                        glBindBuffer(GL_ARRAY_BUFFER, 0);

#if !defined(NOVAO)
                        glGenVertexArrays(1, &vertexArrayModel);
#endif
                        glGenBuffers(1, &vertexbuffer);
                        glGenBuffers(1, &elementbuffer);
                        glGenBuffers(1, &linebuffer);
                    }

                    glViewport(0, 0, width, height);

                    if (fractalFlameColouring)
                    {
                        glUseProgram(programFlameColouring);
                        glDepthMask(GL_FALSE);
                        glBlendFunc (GL_SRC_ALPHA, GL_SRC_ALPHA);
                        glDisable(GL_DEPTH_TEST);

                        for (unsigned int i = 0; i < uniformMax; i++)
                        {
                            uniforms[i] = uniformsFlameColouring[i];
                        }

                        glBindFramebuffer(GL_FRAMEBUFFER, framebufferFlameHistogram);

                        glBindTexture(GL_TEXTURE_2D, textureFlameHistogram);
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                        
                        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureFlameHistogram, 0);
                        GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
                        glDrawBuffers(1, DrawBuffers);
                        
                        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                        {
                            throw std::runtime_error("framebuffer has not been initialised properly");
                        }

                        glBindFramebuffer(GL_FRAMEBUFFER, framebufferFlameColouring);
                        
                        glBindTexture(GL_TEXTURE_2D, textureFlameColouring);
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
                        
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                        
                        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureFlameColouring, 0);
                        glDrawBuffers(1, DrawBuffers);
                        
                        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                        {
                            throw std::runtime_error("framebuffer has not been initialised properly");
                        }
                    }
                    else
                    {
                        glUseProgram(programRegular);
                        glDepthMask(GL_TRUE);
                        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                        glEnable(GL_DEPTH_TEST);
                        glDepthFunc(GL_LEQUAL);

                        for (unsigned int i = 0; i < uniformMax; i++)
                        {
                            uniforms[i] = uniformsRegular[i];
                        }

                        glBindFramebuffer(GL_FRAMEBUFFER, 0);
                    }

                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

                    glUniformMatrix4fv(uniforms[efgy::render::uniformProjectionMatrix], 1, 0, mat);
                    glUniformMatrix3fv(uniforms[efgy::render::uniformNormalMatrix], 1, 0, matn);
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
                        glEnableVertexAttribArray(attributePosition);
                        glVertexAttribPointer(attributePosition, 3, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), 0);
                        glEnableVertexAttribArray(attributeNormal);
                        glVertexAttribPointer(attributeNormal, 3, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
                        glEnableVertexAttribArray(attributeIndex);
                        glVertexAttribPointer(attributeIndex, 1, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), (void*)(6*sizeof(GLfloat)));
#endif

                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                        glBindBuffer(GL_ARRAY_BUFFER, 0);
#if !defined(NOVAO)
                        glBindVertexArray(0);
#endif

                        tindices = GLsizei(triindices.size());
                        lindices = GLsizei(lineindices.size());

                        vertices.clear();
                        vertexmap.clear();
                        triindices.clear();
                        lineindices.clear();
                        indices = 0;
                    }

                    if (fractalFlameColouring)
                    {
                        pushFaces();

                        glBindFramebuffer(GL_FRAMEBUFFER, framebufferFlameHistogram);

                        pushFaces();

                        glBlendFunc (GL_ONE, GL_ZERO);

                        glUseProgram(programFlamePostProcess);

                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, textureFlameColouring);
                        glUniform1i(uniformScreenFramebuffer, 0);

                        glActiveTexture(GL_TEXTURE0+1);
                        glBindTexture(GL_TEXTURE_2D, textureFlameHistogram);
                        glUniform1i(uniformScreenHistogram, 1);

                        glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
                    std::vector<GLfloat> key (7);
                    key[0] = x;
                    key[1] = y;
                    key[2] = z;
                    key[3] = nx;
                    key[4] = ny;
                    key[5] = nz;
                    key[6] = index;

                    std::map<std::vector<GLfloat>,unsigned int>::iterator it = vertexmap.find(key);
                    if (it != vertexmap.end())
                    {
                        return it->second;
                    }

                    vertices.push_back(x);
                    vertices.push_back(y);
                    vertices.push_back(z);

                    vertices.push_back(nx);
                    vertices.push_back(ny);
                    vertices.push_back(nz);

                    vertices.push_back(index);

                    unsigned int rv = indices;

                    indices++;

                    vertexmap[key] = rv;

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

                bool fractalFlameColouring;
                GLuint width;
                GLuint height;

            protected:
                const geometry::transformation::affine<Q,3> &transformation;
                const geometry::projection<Q,3> &projection;
                std::vector<GLfloat> vertices;
                std::map<std::vector<GLfloat>,unsigned int> vertexmap;
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
                GLuint programRegular;
                GLuint programFlameColouring;
                GLuint programFlamePostProcess;
                GLint uniforms[uniformMax];
                GLint uniformsRegular[uniformMax];
                GLint uniformsFlameColouring[uniformMax];
                bool linesEnabled;
                bool facesEnabled;
                bool lineDepthMask;
                bool faceDepthMask;
                GLfloat wireframeColour[4];
                GLfloat surfaceColour[4];
                GLuint framebufferFlameColouring;
                GLuint framebufferFlameHistogram;
                GLuint textureFlameColouring;
                GLuint textureFlameHistogram;
                GLuint uniformScreenFramebuffer;
                GLuint uniformScreenHistogram;

                bool compileShader (GLuint &shader, GLenum type, const char *data)
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

                bool linkProgram (GLuint prog)
                {
                    GLint status;
                    glLinkProgram(prog);
                    
#if defined(DEBUG)
                    GLint logLength;
                    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
                    if (logLength > 0)
                    {
                        GLchar *log = new GLchar[logLength];
                        glGetProgramInfoLog(prog, logLength, &logLength, log);
                        std::cerr << "Program link log:\n" << log << "\n";
                        delete[] log;
                    }
#endif
                    
                    glGetProgramiv(prog, GL_LINK_STATUS, &status);
                    if (status == 0)
                    {
                        return false;
                    }
                    
                    return true;
                }

                bool loadShaders (GLuint &program, const bool fractalFlameColouring, const bool postProcess)
                {
                    GLuint vertShader, fragShader;
                    
                    program = glCreateProgram();
                    
                    if (!compileShader(vertShader, GL_VERTEX_SHADER, getVertexShader(fractalFlameColouring, postProcess).c_str()))
                    {
                        std::cerr << "Failed to compile vertex shader\n";
                        return false;
                    }
                    
                    if (!compileShader(fragShader, GL_FRAGMENT_SHADER, getFragmentShader(fractalFlameColouring, postProcess).c_str()))
                    {
                        std::cerr << "Failed to compile fragment shader\n";
                        return false;
                    }
                    
                    glAttachShader(program, vertShader);
                    
                    glAttachShader(program, fragShader);
                    
                    glBindAttribLocation(program, attributePosition, "position");
                    glBindAttribLocation(program, attributeNormal,   "normal");
                    glBindAttribLocation(program, attributeIndex,    "index");
                    
                    if (!linkProgram(program))
                    {
                        std::cerr << "Failed to link program: " << program << "\n";
                        
                        if (vertShader) {
                            glDeleteShader(vertShader);
                            vertShader = 0;
                        }
                        if (fragShader) {
                            glDeleteShader(fragShader);
                            fragShader = 0;
                        }
                        if (program) {
                            glDeleteProgram(program);
                            program = 0;
                        }
                        
                        return false;
                    }
                    
                    // Get uniform locations.
                    if (!postProcess)
                    {
                        if (fractalFlameColouring)
                        {
                            uniformsFlameColouring[uniformProjectionMatrix] = glGetUniformLocation(program, "modelViewProjectionMatrix");
                            uniformsFlameColouring[uniformNormalMatrix]     = glGetUniformLocation(program, "normalMatrix");
                            uniformsFlameColouring[uniformColour]           = glGetUniformLocation(program, "colour");
                        }
                        else
                        {
                            uniformsRegular[uniformProjectionMatrix] = glGetUniformLocation(program, "modelViewProjectionMatrix");
                            uniformsRegular[uniformNormalMatrix]     = glGetUniformLocation(program, "normalMatrix");
                            uniformsRegular[uniformColour]           = glGetUniformLocation(program, "colour");
                        }
                    }
                    else
                    {
                        uniformScreenFramebuffer = glGetUniformLocation(program, "screenFramebuffer");
                        uniformScreenHistogram   = glGetUniformLocation(program, "screenHistogram");
                    }
                    
                    // Release vertex and fragment shaders.
                    if (vertShader) {
                        glDetachShader(program, vertShader);
                        glDeleteShader(vertShader);
                    }
                    if (fragShader) {
                        glDetachShader(program, fragShader);
                        glDeleteShader(fragShader);
                    }
                    
                    return true;
                }

                void pushLines (void) const
                {
                    if (prepared && linesEnabled && !fractalFlameColouring)
                    {
                        glUniform4f(uniforms[uniformColour], wireframeColour[0], wireframeColour[1], wireframeColour[2], wireframeColour[3]);

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
                        glUniform4f(uniforms[uniformColour], surfaceColour[0], surfaceColour[1], surfaceColour[2], surfaceColour[3]);

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
