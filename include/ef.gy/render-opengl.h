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
#include <ef.gy/opengl.h>
#include <map>
#include <functional>
#include <algorithm>

namespace efgy
{
    namespace render
    {
        static inline std::string getVertexShader (const bool fractalFlameColouring, const bool postProcess, const bool renderHistogram)
        {
            std::stringstream output ("");
            output << "#version 100\n";
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
                    "attribute highp float index;\n"
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
            output << "#version 100\n";
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
    };

    namespace opengl
    {
        /**\brief OpenGL render programme
         *
         * This class encapsulates a typical, single-pass OpenGL programme to
         * render things on the primary framebuffer.
         *
         * \tparam Q Base data type for calculations.
         */
        template<typename Q>
        class renderProgramme : public renderToFramebufferProgramme<Q>
        {
            public:
                /**\brief Default constructor
                 *
                 * Initialises, but does not compile, the render programme
                 * with the correct shaders.
                 */
                renderProgramme (void)
                    : initialised(false),
                      renderToFramebufferProgramme<Q>
                        (render::getVertexShader(false, false, false),
                         render::getFragmentShader(false, false, false))
                    {}
            
                /**\brief Load matrix uniforms
                 *
                 * Updates the projection and normal matrices of the OpenGL
                 * programme with the specified values.
                 *
                 * \param[in] combined     The combined (MVC) projective
                 *                         transformation.
                 * \param[in] normalMatrix The normal matrix to use for
                 *                         lighting.
                 *
                 * \return True if the uniforms were uploaded successfully,
                 *         false otherwise.
                 */
                bool matrices (const geometry::transformation::projective<Q,3> &combined, const math::matrix<Q,3,3> &normalMatrix)
                {
                    return renderToFramebufferProgramme<Q>::uniform(uniformProjectionMatrix, combined.transformationMatrix)
                        && renderToFramebufferProgramme<Q>::uniform(uniformNormalMatrix, normalMatrix);
                }

                using renderToFramebufferProgramme<Q>::uniform;
            
                /**\brief Render to current OpenGL context
                 *
                 * This function will perform a regular render of anything you
                 * specify in the draw function. Shader programmes are compiled
                 * and activated as necessary.
                 *
                 * \param[in] width  Width of the viewport to render to.
                 * \param[in] height Height of the viewport to render to.
                 * \param[in] draw   Function to do the actual rendering.
                 *                   You'll want to use one of those
                 *                   new-fangled lambdas here.
                 *
                 * \return True if rendering the scene succeeded.
                 */
                bool operator () (const GLuint &width, const GLuint &height, std::function<void()> draw)
                {
                    if (!initialised)
                    {
                        initialised = true;

                        renderToFramebufferProgramme<Q>::copy();
                    }

                    renderToFramebufferProgramme<Q>::use(width, height);
                    glDepthMask(GL_TRUE);
                    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    glEnable(GL_DEPTH_TEST);
                    glDepthFunc(GL_LEQUAL);

                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                    draw();

                    return true;
                }

            protected:
                /**\brief Has the object been initialised?
                 *
                 * The actual initialisation of the object is deferred until
                 * the first time it is rendered, so as to prevent issues when
                 * creating the object before an OpenGL context is available.
                 * This boolean is set to true or false, depending on whether
                 * this initialisation step has ever been processed.
                 */
                bool initialised;
        };

        /**\brief Fractal flame render programme
         *
         * This class encapsulates the OpenGL programmes used to implement the
         * fractal flame colouring algorithm.
         *
         * \tparam Q Base data type for calculations.
         *
         * \see http://flam3.com/flame_draves.pdf for a description of the
         *      colouring algorithm these render programmes are based on.
         */
        template<typename Q>
        class fractalFlameRenderProgramme
        {
            public:
                /**\brief Default constructor
                 *
                 * Initialises, but does not compile, the render programmes
                 * with the correct shaders.
                 */
                fractalFlameRenderProgramme (void)
                    : initialised(false),
                      colouring(render::getVertexShader(true, false, false), render::getFragmentShader(true, false, false)),
                      histogram(render::getVertexShader(true, false, true), render::getFragmentShader(true, false, true)),
                      postProcess(render::getVertexShader(true, true, false), render::getFragmentShader(true, true, false))
                    {}

                /**\brief Load matrix uniforms
                 *
                 * Updates the projection and normal matrices of the OpenGL
                 * programmes with the specified values.
                 *
                 * \param[in] combined     The combined (MVC) projective
                 *                         transformation.
                 * \param[in] normalMatrix The normal matrix to use for
                 *                         lighting.
                 *
                 * \return True if the uniforms were uploaded successfully,
                 *         false otherwise.
                 */
                bool matrices (const geometry::transformation::projective<Q,3> &combined, const math::matrix<Q,3,3> &normalMatrix)
                {
                    return histogram.uniform(uniformProjectionMatrix, combined.transformationMatrix)
                        && histogram.uniform(uniformNormalMatrix, normalMatrix)
                        && colouring.uniform(uniformProjectionMatrix, combined.transformationMatrix)
                        && colouring.uniform(uniformNormalMatrix, normalMatrix);
                }

                /**\brief Render to current OpenGL context
                 *
                 * This function will perform a two-pass render of anything you
                 * specify in the draw function. Shader programmes are compiled
                 * and activated as necessary.
                 *
                 * \param[in] width  Width of the viewport to render to.
                 * \param[in] height Height of the viewport to render to.
                 * \param[in] draw   Function to do the actual rendering.
                 *                   You'll want to use one of those
                 *                   new-fangled lambdas here.
                 *
                 * \return True if rendering the scene succeeded.
                 */
                bool operator () (const GLuint &width, const GLuint &height, std::function<void()> draw)
                {
                    if (!initialised)
                    {
                        initialised = true;

                        postProcess.copy();

                        static const GLfloat fullscreenQuadBufferData[] =
                        {
                            -1.0f, -1.0f,  0.0f,
                             1.0f, -1.0f,  0.0f,
                            -1.0f,  1.0f,  0.0f,
                            -1.0f,  1.0f,  0.0f,
                             1.0f, -1.0f,  0.0f,
                             1.0f,  1.0f,  0.0f
                        };
                        
                        vertexArray.use();
                        vertices.load(sizeof(fullscreenQuadBufferData), fullscreenQuadBufferData);
                        vertexArray.setup();
                        
                        setColourMap();
                    }

                    glClearColor(1,1,1,1);
                    
                    colouring.use(width, height, 0);
                    
                    glDepthMask(GL_TRUE);
                    glBlendFunc (GL_ONE, GL_ZERO);
                    glEnable(GL_DEPTH_TEST);
                    glDepthFunc(GL_LEQUAL);
                    
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    glBlendFunc (GL_SRC_ALPHA, GL_SRC_ALPHA);
                    
                    draw();
                    
                    histogram.use(width, height, 1);
                    
                    glDepthMask(GL_FALSE);
                    glDisable(GL_DEPTH_TEST);

//                        glClearColor(0,0,0,1);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    
//                        glBlendFunc (GL_ONE, GL_ONE);
                    glBlendFunc (GL_ZERO, GL_SRC_COLOR);
                    
                    draw();

                    postProcess.use(width, height);
                    
                    postProcess.uniform(efgy::opengl::uniformScreenFramebuffer, 0);
                    postProcess.uniform(efgy::opengl::uniformScreenHistogram, 1);
                    
                    glActiveTexture(GL_TEXTURE0 + 2);
                    colourMap.bind();
                    postProcess.uniform(efgy::opengl::uniformColourMap, 2);
                    
                    glBlendFunc (GL_ONE, GL_ZERO);
                    
                    vertexArray.use();
                    vertices.bind();
                    vertexArray.setup();
                    
                    glDrawArrays(GL_TRIANGLES, 0, 6);

                    return true;
                }

                /**\brief Set random colour map
                 *
                 * The fractal flame colouring algorithm uses a colour map to
                 * distinguish between different IFS indices; this creates a
                 * random colour map for this purpose.
                 */
                void setColourMap (void)
                {
                    std::vector<unsigned char> colours;

                    for (unsigned int i = 0; i < 8; i++)
                    {
                        colours.push_back(std::rand()%255);
                        colours.push_back(std::rand()%255);
                        colours.push_back(std::rand()%255);
                    }

                    colourMap.load(GLsizei(colours.size()/3), 1, &colours[0]);

                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                }

            protected:
                /**\brief Has the object been initialised?
                 *
                 * The actual initialisation of the object is deferred until
                 * the first time it is rendered, so as to prevent issues when
                 * creating the object before an OpenGL context is available.
                 * This boolean is set to true or false, depending on whether
                 * this initialisation step has ever been processed.
                 */
                bool initialised;

                /**\brief Colouring render pass programme
                 *
                 * Stores the OpenGL programme that creates the colour index
                 * texture for the flame algorithm. This colour information is
                 * used to sample the colour map.
                 */
                renderToTextureProgramme<Q> colouring;

                /**\brief Histogram render pass programme
                 *
                 * Stores the OpenGL programme that creates the histogram
                 * texture for the flame algorithm. This information is used to
                 * calculate the output intensity of individual fragments.
                 */
                renderToTextureProgramme<Q> histogram;

                /**\brief Merge pass programme
                 *
                 * Stores the OpenGL programme that combines the textures
                 * creates in the first two passes to output fragments for the
                 * output framebuffer.
                 */
                renderToFramebufferProgramme<Q> postProcess;

                /**\brief Vertex array for fullscreen quad
                 *
                 * Stores the vertex array for a fullscreen quad, which is used
                 * in the merge pass to emit fragments for each pixel of the
                 * output viewport.
                 */
                vertexArrayMinimal<Q> vertexArray;

                /**\brief Vertex buffer for fullscreen quad
                 *
                 * Stores the vertex data for a fullscreen quad, which is used
                 * in the merge pass to emit fragments for each pixel of the
                 * output viewport.
                 */
                vertexBuffer vertices;

                /**\brief Colour map texture
                 *
                 * This is the colour map created by setColourMap() and used in
                 * the final merge pass to turn th initial grayscale image into
                 * something much more interesting.
                 */
                texture2D colourMap;
        };
    };

    namespace render
    {
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
                constexpr opengl
                    (const geometry::transformation::affine<Q,d> &pTransformation,
                     const geometry::projection<Q,d> &pProjection,
                     opengl<Q,d-1> &pLowerRenderer)
                    : transformation(pTransformation), projection(pProjection), lowerRenderer(pLowerRenderer),
                      fractalFlameColouring(pLowerRenderer.fractalFlameColouring),
                      width(pLowerRenderer.width), height(pLowerRenderer.height),
                      prepared(pLowerRenderer.prepared)
                    {}

                void frameStart (void)
                {
                    combined = transformation * projection;
                    lowerRenderer.frameStart();
                };
                void frameEnd (void) const { lowerRenderer.frameEnd(); };

                template<std::size_t q>
                void drawFace
                    (const std::array<typename geometry::euclidian::space<Q,d>::vector,q> &pV, const Q &index = 0.5) const
                {
                    if (prepared) return;

                    std::array<typename geometry::euclidian::space<Q,d-1>::vector,q> V;

                    std::transform
                        (pV.begin(), pV.end(), V.begin(),
                         [&](const typename geometry::euclidian::space<Q,d>::vector &s)
                          -> typename geometry::euclidian::space<Q,d-1>::vector
                    {
                        return combined * s;
                    });

                    lowerRenderer.drawFace(V, index);
                }

                /**\brief Set colour
                 *
                 * Sets the colour for either the wireframe model or the faces.
                 * Which of the two is set depends on the pWireframe parameter.
                 *
                 * \note The renderers typically use the alpha component for
                 *       transparency.
                 *
                 * \param[in] pRed   The red component of the colour to set.
                 * \param[in] pGreen The green component of the colour to set.
                 * \param[in] pBlue  The blue component of the colour to set.
                 * \param[in] pAlpha The alpha component of the colour to set.
                 * \param[in] pWireframe 'True' if you want to set the colour
                 *                       for the model's lines, 'false' to set
                 *                       the colour for the faces.
                 *
                 * \returns True when the new colour was successfully set, false
                 *          otherwise.
                 */
                bool setColour (float pRed, float pGreen, float pBlue, float pAlpha, bool pWireframe)
                {
                    return lowerRenderer.setColour (pRed, pGreen, pBlue, pAlpha, pWireframe);
                }

                /**\brief Create random colour map
                 *
                 * Calling this method will create a random colour map texture
                 * for use by the fractal flame colouring code.
                 */
                void setColourMap (void)
                {
                    lowerRenderer.setColourMap();
                }

                /**\brief Use Fractal Flame Colouring?
                 *
                 * Set to true to use the fractal flame colouring algorithm
                 * instead of the 'normal' render programme.
                 */
                bool &fractalFlameColouring;

                /**\brief Has the scene been prepared?
                 *
                 * Set to true after uploading geometry data to the graphics
                 * card, right before actually telling OpenGL to render that
                 * data. Needs to be set to 'false' if you change any model
                 * parameters in order for that data to be prepared and uploaded
                 * to the graphics card again.
                 */
                bool &prepared;

                /**\brief Viewport width
                 *
                 * Width of the output viewport. The renderer assumes that the
                 * viewport is rectangular, going from (0,0) and extending to
                 * (width-1,height-1).
                 */
                GLuint &width;

                /**\brief Viewport height
                 *
                 * Height of the output viewport. The renderer assumes that the
                 * viewport is rectangular, going from (0,0) and extending to
                 * (width-1,height-1).
                 */
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
                constexpr opengl
                    (const geometry::transformation::affine<Q,3> &pTransformation,
                     const geometry::projection<Q,3> &pProjection,
                     opengl<Q,2> &pLowerRenderer)
                    : transformation(pTransformation), projection(pProjection),
                      fractalFlameColouring(pLowerRenderer.fractalFlameColouring),
                      prepared(pLowerRenderer.prepared),
                      width(pLowerRenderer.width), height(pLowerRenderer.height)
                    {
                    }

                void frameStart (void)
                {
                    const geometry::transformation::projective<Q,3> combined = transformation * projection;
                    const math::matrix<Q,3,3> normalMatrix = math::transpose(math::invert(math::transpose(math::matrix<Q,3,3>(transformation.transformationMatrix))));

                    if (fractalFlameColouring)
                    {
                        fractalFlame.matrices (combined, normalMatrix);
                    }
                    else
                    {
                        render.matrices (combined, normalMatrix);
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

                        // log errors
                        efgy::opengl::error<>();
                    }

                    if (fractalFlameColouring)
                    {
                        fractalFlame (width, height, [this] ()
                        {
                            pushFaces();
                        });
                    }
                    else
                    {
                        render (width, height, [this] ()
                        {
                            pushLines();
                            pushFaces();
                        });
                    }
                };

                template<std::size_t q>
                void drawFace
                    (const std::array<typename geometry::euclidian::space<Q,3>::vector,q> &pV, const Q &index = 0.5)
                {
                    if (prepared) return;

                    typename geometry::euclidian::space<Q,3>::vector R
                        = geometry::euclidian::normalise<Q,3>
                            (geometry::euclidian::crossProduct<Q>
                                (pV[1] - pV[0], pV[2] - pV[0]));

                    typename geometry::euclidian::space<Q,3>::vector RN
                        = geometry::euclidian::normalise<Q,3>
                            (geometry::euclidian::crossProduct<Q>
                        (pV[2] - pV[0], pV[1] - pV[0]));

                    triindices.push_back(addVertex(GLfloat(pV[0][0]), GLfloat(pV[0][1]), GLfloat(pV[0][2]),
                                                   GLfloat(R[0]), GLfloat(R[1]), GLfloat(R[2]), GLfloat(index)));
                    unsigned int nstartf = triindices.back();
                    lineindices.push_back(nstartf);
                    triindices.push_back(addVertex(GLfloat(pV[1][0]), GLfloat(pV[1][1]), GLfloat(pV[1][2]),
                                                   GLfloat(R[0]), GLfloat(R[1]), GLfloat(R[2]), GLfloat(index)));
                    lineindices.push_back(triindices.back());
                    lineindices.push_back(triindices.back());
                    triindices.push_back(addVertex(GLfloat(pV[2][0]), GLfloat(pV[2][1]), GLfloat(pV[2][2]),
                                                   GLfloat(R[0]), GLfloat(R[1]), GLfloat(R[2]), GLfloat(index)));
                    unsigned int nendf = triindices.back();
                    lineindices.push_back(nendf);

                    triindices.push_back(addVertex(GLfloat(pV[2][0]), GLfloat(pV[2][1]), GLfloat(pV[2][2]),
                                                   GLfloat(RN[0]), GLfloat(RN[1]), GLfloat(RN[2]), GLfloat(index)));
                    unsigned int nendb = triindices.back();
                    lineindices.push_back(nendb);
                    triindices.push_back(addVertex(GLfloat(pV[1][0]), GLfloat(pV[1][1]), GLfloat(pV[1][2]),
                                                   GLfloat(RN[0]), GLfloat(RN[1]), GLfloat(RN[2]), GLfloat(index)));
                    lineindices.push_back(triindices.back());
                    lineindices.push_back(triindices.back());
                    triindices.push_back(addVertex(GLfloat(pV[0][0]), GLfloat(pV[0][1]), GLfloat(pV[0][2]),
                                                   GLfloat(RN[0]), GLfloat(RN[1]), GLfloat(RN[2]), GLfloat(index)));
                    unsigned int nstartb = triindices.back();
                    lineindices.push_back(nstartb);

                    for (unsigned int j = 3; j < q; j++)
                    {
                        triindices.push_back(addVertex(GLfloat(pV[0][0]), GLfloat(pV[0][1]), GLfloat(pV[0][2]),
                                                       GLfloat(R[0]), GLfloat(R[1]), GLfloat(R[2]), GLfloat(index)));
                        triindices.push_back(addVertex(GLfloat(pV[(j-1)][0]), GLfloat(pV[(j-1)][1]), GLfloat(pV[(j-1)][2]),
                                                       GLfloat(R[0]), GLfloat(R[1]), GLfloat(R[2]), GLfloat(index)));
                        lineindices.push_back(triindices.back());
                        triindices.push_back(addVertex(GLfloat(pV[j][0]), GLfloat(pV[j][1]), GLfloat(pV[j][2]),
                                                       GLfloat(R[0]), GLfloat(R[1]), GLfloat(R[2]), GLfloat(index)));
                        lineindices.push_back(triindices.back());
                        nendf = triindices.back();

                        triindices.push_back(addVertex(GLfloat(pV[j][0]), GLfloat(pV[j][1]), GLfloat(pV[j][2]),
                                                       GLfloat(RN[0]), GLfloat(RN[1]), GLfloat(RN[2]), GLfloat(index)));
                        nendb = triindices.back();
                        lineindices.push_back(triindices.back());
                        triindices.push_back(addVertex(GLfloat(pV[(j-1)][0]), GLfloat(pV[(j-1)][1]), GLfloat(pV[(j-1)][2]),
                                                       GLfloat(RN[0]), GLfloat(RN[1]), GLfloat(RN[2]), GLfloat(index)));
                        lineindices.push_back(triindices.back());
                        triindices.push_back(addVertex(GLfloat(pV[0][0]), GLfloat(pV[0][1]), GLfloat(pV[0][2]),
                                                       GLfloat(RN[0]), GLfloat(RN[1]), GLfloat(RN[2]), GLfloat(index)));
                    }

                    lineindices.push_back(nendf);
                    lineindices.push_back(nstartf);
                    lineindices.push_back(nstartb);
                    lineindices.push_back(nendb);
                }

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

                /**\brief Set colour
                 *
                 * Sets the colour for either the wireframe model or the faces.
                 * Which of the two is set depends on the pWireframe parameter.
                 *
                 * \note The renderers typically use the alpha component for
                 *       transparency.
                 *
                 * \param[in] pRed   The red component of the colour to set.
                 * \param[in] pGreen The green component of the colour to set.
                 * \param[in] pBlue  The blue component of the colour to set.
                 * \param[in] pAlpha The alpha component of the colour to set.
                 * \param[in] pWireframe 'True' if you want to set the colour
                 *                       for the model's lines, 'false' to set
                 *                       the colour for the faces.
                 *
                 * \returns True when the new colour was successfully set, false
                 *          otherwise.
                 */
                bool setColour
                    (const float &pRed, const float &pGreen, const float &pBlue,
                     const float &pAlpha, const bool &pWireframe)
                {
                    if (pWireframe)
                    {
                        wireframeColour[0] = pRed;
                        wireframeColour[1] = pGreen;
                        wireframeColour[2] = pBlue;
                        wireframeColour[3] = pAlpha;
                    }
                    else
                    {
                        surfaceColour[0] = pRed;
                        surfaceColour[1] = pGreen;
                        surfaceColour[2] = pBlue;
                        surfaceColour[3] = pAlpha;
                    }
                    return true;
                }

                /**\brief Create random colour map
                 *
                 * Calling this method will create a random colour map texture
                 * for use by the fractal flame colouring code.
                 */
                void setColourMap (void)
                {
                    fractalFlame.setColourMap();
                }

                /**\brief Use Fractal Flame Colouring?
                 *
                 * Set to true to use the fractal flame colouring algorithm
                 * instead of the 'normal' render programme.
                 */
                bool &fractalFlameColouring;

                /**\brief Has the scene been prepared?
                 *
                 * Set to true after uploading geometry data to the graphics
                 * card, right before actually telling OpenGL to render that
                 * data. Needs to be set to 'false' if you change any model
                 * parameters in order for that data to be prepared and uploaded
                 * to the graphics card again.
                 */
                bool &prepared;

                /**\brief Viewport width
                 *
                 * Width of the output viewport. The renderer assumes that the
                 * viewport is rectangular, going from (0,0) and extending to
                 * (width-1,height-1).
                 */
                GLuint &width;

                /**\brief Viewport height
                 *
                 * Height of the output viewport. The renderer assumes that the
                 * viewport is rectangular, going from (0,0) and extending to
                 * (width-1,height-1).
                 */
                GLuint &height;

            protected:
                const geometry::transformation::affine<Q,3> &transformation;
                const geometry::projection<Q,3> &projection;
                std::vector<GLfloat> vertices;
                std::vector<unsigned int> triindices;
                std::vector<unsigned int> lineindices;

                /**\brief Number of vertex indices
                 *
                 * The number of vertices that will be uploaded to the graphics
                 * card; used when preparing the data to be uploaded, so it's
                 * only relevant when 'prepared' is 'false'.
                 */
                unsigned int indices;

                /**\brief Number of triangle indices
                 *
                 * The number of triangles that have been uploaded to the
                 * graphics card; used when rendering a model. Only valid when
                 * the member variable 'prepared' is 'true'.
                 */
                GLsizei tindices;
            
                /**\brief Number of line indices
                 *
                 * The number of lines that have been uploaded to the graphics
                 * card; used when rendering a model. Only valid when the member
                 * variable 'prepared' is 'true'.
                 */
                GLsizei lindices;

                efgy::opengl::renderProgramme<Q> render;
                efgy::opengl::fractalFlameRenderProgramme<Q> fractalFlame;

                efgy::opengl::vertexArrayExtended<Q> vertexArrayModel;

                efgy::opengl::vertexBuffer vertexbuffer;
                efgy::opengl::indexBuffer elementbuffer;
                efgy::opengl::indexBuffer linebuffer;

                GLfloat wireframeColour[4];
                GLfloat surfaceColour[4];

                void pushLines (void) const
                {
                    if (prepared && (wireframeColour[3] > 0.f) && !fractalFlameColouring)
                    {
                        render.uniform(efgy::opengl::uniformColour, wireframeColour);

                        glDepthMask ((wireframeColour[3] >= 1.f) ? GL_TRUE : GL_FALSE);

                        vertexArrayModel.use();
                        vertexbuffer.bind();
                        linebuffer.bind();
                        vertexArrayModel.setup();

                        glDrawElements(GL_LINES, lindices, GL_UNSIGNED_INT, 0);
                    }
                }
            
                void pushFaces (void) const
                {
                    if (prepared && ((surfaceColour[3] > 0.f) || fractalFlameColouring))
                    {
                        if (!fractalFlameColouring)
                        {
                            render.uniform(efgy::opengl::uniformColour, surfaceColour);
                        }

                        glDepthMask ((surfaceColour[3] >= 1.f) ? GL_TRUE : GL_FALSE);

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
                constexpr opengl (const geometry::transformation::affine<Q,2> &)
                    : fractalFlameColouring(false), prepared(false) {}

                /**\brief Use Fractal Flame Colouring?
                 *
                 * Set to true to use the fractal flame colouring algorithm
                 * instead of the 'normal' render programme.
                 */
                bool fractalFlameColouring;

                /**\brief Has the scene been prepared?
                 *
                 * Set to true after uploading geometry data to the graphics
                 * card, right before actually telling OpenGL to render that
                 * data. Needs to be set to 'false' if you change any model
                 * parameters in order for that data to be prepared and uploaded
                 * to the graphics card again.
                 */
                bool prepared;

                /**\brief Viewport width
                 *
                 * Width of the output viewport. The renderer assumes that the
                 * viewport is rectangular, going from (0,0) and extending to
                 * (width-1,height-1).
                 */
                GLuint width;

                /**\brief Viewport height
                 *
                 * Height of the output viewport. The renderer assumes that the
                 * viewport is rectangular, going from (0,0) and extending to
                 * (width-1,height-1).
                 */
                GLuint height;
        };
    };
};

#endif
