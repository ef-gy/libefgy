/**\file
 * \brief OpenGL renderer
 *
 * Code to render geometric primitives to an OpenGL 3.2/OpenGL ES/WebGL context.
 * Higher-dimensional primitives will be reduced to 3D and are then passed to
 * the graphics card to render.
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

#if !defined(EF_GY_RENDER_OPENGL_H)
#define EF_GY_RENDER_OPENGL_H

#include <ef.gy/euclidian.h>
#include <ef.gy/projection.h>
#include <ef.gy/opengl.h>
#include <ef.gy/colour-space-rgb.h>
#include <ef.gy/polytope.h>
#include <ef.gy/tracer.h>
#include <map>
#include <functional>
#include <algorithm>
#include <array>
#include <ostream>

namespace efgy
{
    namespace render
    {
        /**\brief GLSL shaders
         *
         * Contains the shaders used by the OpenGL renderer; the vertex and
         * fragment shaders each received their own namespace to keep things
         * somewhat tidy.
         */
        namespace glsl
        {
            template<unsigned int d>
            static inline void name
                (const std::string &name,
                 math::vector<math::tracer::runtime,d> &pVector)
            {
                for (unsigned int i = 0; i < d; i++)
                {
                    std::stringstream s("");
                    s << i;
                    pVector[i] = std::shared_ptr<math::tracer::tracer<void,void,0>>
                        (new math::tracer::tracer<void,void,0> (name + "[" + s.str() + "]"));
                }
            }
            
            template<unsigned int d>
            static inline void name
                (const std::string &name,
                 math::matrix<math::tracer::runtime,d,d> &pMatrix)
            {
                unsigned int k = 0;
                for (unsigned int i = 0; i < d; i++)
                {
                    for (unsigned int j = 0; j < d; j++)
                    {
                        std::stringstream s("");
                        s << "[" << k << "]";
                        k++;
                        pMatrix[i][j] = std::shared_ptr<math::tracer::tracer<void,void,0>>
                            (new math::tracer::tracer<void,void,0> (name + s.str()));
                    }
                }
            }

            template<unsigned int d>
            static inline void transform
                (std::vector<opengl::glsl::variable<opengl::glsl::gv_uniform>> &uniform,
                 math::vector<math::tracer::runtime,4> &gl_Position,
                 math::vector<math::tracer::runtime,d+1> &position)
            {
                std::stringstream vn("");
                vn << "mvp" << d;

                uniform.push_back
                    (opengl::glsl::variable<opengl::glsl::gv_uniform>
                     (vn.str(), "float", "", (d+1)*(d+1)));

                math::matrix<math::tracer::runtime,(d+1),(d+1)> m1;
                name(vn.str(), m1);

                geometry::transformation::projective<math::tracer::runtime,d> combined(m1);

                transform<d-1>(uniform,gl_Position,combined * position);
            }
            
            template<>
            void transform<3>
                (std::vector<opengl::glsl::variable<opengl::glsl::gv_uniform>> &uniform,
                 math::vector<math::tracer::runtime,4> &gl_Position,
                 math::vector<math::tracer::runtime,4> &position)
            {
                uniform.push_back
                    (opengl::glsl::variable<opengl::glsl::gv_uniform>
                     ("mvp3", "float", "", 16));

                math::matrix<math::tracer::runtime,4,4> m1;
                name("mvp3", m1);

                geometry::transformation::linear<math::tracer::runtime,4> l1(m1);

                gl_Position = l1 * position;
            }

            /**\brief GLSL vertex shaders
             *
             * Contains the vertex shaders used by the default OpenGL renderer.
             */
            namespace vertex
            {
                /*
                 *
                 * \tparam V GLSL shader version to produce.
                 */
                template <unsigned int d = 3, enum opengl::glsl::version V = opengl::glsl::ver_auto>
                class fractalFlame : public opengl::glsl::shader<V>
                {
                    public:
                        fractalFlame(void)
                            : opengl::glsl::shader<V>
                                ("indexVarying = index;\n",
                                 { opengl::glsl::variable<opengl::glsl::gv_attribute>("position", "vec4"),
                                   opengl::glsl::variable<opengl::glsl::gv_attribute>("index", "float", "highp") },
                                 { opengl::glsl::variable<opengl::glsl::gv_varying>("indexVarying", "float", "lowp") } )
                            {
                                math::vector<math::tracer::runtime,d+1> P;
                                name("position", P);

                                transform<d>(opengl::glsl::shader<V>::uniform, gl_Position, P);
                                opengl::glsl::shader<V>::main += position();
                            }

                        std::string position (void)
                        {
                            std::stringstream m("");

                            for (unsigned int i = 0; i < (d+1); i++)
                            {
                                m << "gl_Position[" << i << "] = "
                                << gl_Position[i] << ";\n";
                            }

                            return m.str();
                        }

                        math::vector<math::tracer::runtime,4> gl_Position;
                };

                /*
                 *
                 * \tparam V GLSL shader version to produce.
                 */
                template <enum opengl::glsl::version V>
                class fractalFlame<2,V> : public opengl::glsl::shader<V>
                {
                    // mvp3 should be mvp2
                    public:
                        fractalFlame(void)
                            : opengl::glsl::shader<V>
                                ("indexVarying = index;\n"
                                 "gl_Position = vec4(mvp3 * position,1);\n",
                                 { opengl::glsl::variable<opengl::glsl::gv_attribute>("position", "vec3"),
                                   opengl::glsl::variable<opengl::glsl::gv_attribute>("index", "float", "highp") },
                                 { opengl::glsl::variable<opengl::glsl::gv_varying>("indexVarying", "float", "lowp") },
                                 { opengl::glsl::variable<opengl::glsl::gv_uniform>("mvp3", "mat3") } )
                            {}
                };

                /*
                 *
                 * \tparam V GLSL shader version to produce.
                 */
                template <unsigned int d = 3, enum opengl::glsl::version V = opengl::glsl::ver_auto>
                class regular : public opengl::glsl::shader<V>
                {
                    public:
                        regular(void)
                            : opengl::glsl::shader<V>
                                ("vec3 eyeNormal = normalize(normalMatrix * normal);\n"
                                 "vec3 lightPosition = vec3(0.0, 1.0, 1.0);\n"
                                 "float nDotVP = max(0.0, dot(eyeNormal, normalize(lightPosition)));\n"
                                 "colorVarying = colour * nDotVP;\n"
                                 "colorVarying = vec4(colorVarying.xyz, colour.w);\n"
                                 ,
                                 { opengl::glsl::variable<opengl::glsl::gv_attribute>("position", "vec4"),
                                   opengl::glsl::variable<opengl::glsl::gv_attribute>("normal", "vec3") },
                                 { opengl::glsl::variable<opengl::glsl::gv_varying>("colorVarying", "vec4") },
                                 { opengl::glsl::variable<opengl::glsl::gv_uniform>("normalMatrix", "mat3"),
                                   opengl::glsl::variable<opengl::glsl::gv_uniform>("colour", "vec4") } )
                            {
                                math::vector<math::tracer::runtime,d+1> P;
                                name("position", P);

                                transform<d>(opengl::glsl::shader<V>::uniform, gl_Position, P);
                                opengl::glsl::shader<V>::main += position();
                            }

                        std::string position (void)
                        {
                            std::stringstream m("");

                            for (unsigned int i = 0; i < (d+1); i++)
                            {
                                m << "gl_Position[" << i << "] = "
                                  << gl_Position[i] << ";\n";
                            }
                            
                            return m.str();
                        }

                        math::vector<math::tracer::runtime,4> gl_Position;
                };

                /*
                 *
                 * \tparam V GLSL shader version to produce.
                 */
                template <enum opengl::glsl::version V>
                class regular<2,V> : public opengl::glsl::shader<V>
                {
                    // mvp3 should be mvp2
                    public:
                        regular(void)
                            : opengl::glsl::shader<V>
                                ("gl_Position = vec4(mvp3 * position,1);\n"
                                 "colorVarying = colour;\n",
                                 { opengl::glsl::variable<opengl::glsl::gv_attribute>("position", "vec3") },
                                 { opengl::glsl::variable<opengl::glsl::gv_varying>("colorVarying", "vec4") },
                                 { opengl::glsl::variable<opengl::glsl::gv_uniform>("mvp3", "mat3"),
                                   opengl::glsl::variable<opengl::glsl::gv_uniform>("colour", "vec4") } )
                            {}
                };

                /*
                 *
                 * \tparam V GLSL shader version to produce.
                 */
                template <enum opengl::glsl::version V = opengl::glsl::ver_auto>
                class postProcess : public opengl::glsl::shader<V>
                {
                    public:
                        postProcess(void)
                            : opengl::glsl::shader<V>
                                ("gl_Position = position;\n"
                                 "UV = (position.xy+vec2(1.0,1.0))/2.0;\n",
                                 { opengl::glsl::variable<opengl::glsl::gv_attribute>("position", "vec4") },
                                 { opengl::glsl::variable<opengl::glsl::gv_varying>("UV", "vec2", "lowp") } )
                            {}
                };
            };

            /**\brief GLSL fragment shaders
             *
             * Contains the fragment shaders used by the default OpenGL
             * renderer.
             */
            namespace fragment
            {
                /*
                 *
                 * \tparam V GLSL shader version to produce.
                 */
                template <enum opengl::glsl::version V = opengl::glsl::ver_auto>
                class fractalFlame : public opengl::glsl::shader<V>
                {
                    public:
                        fractalFlame(void)
                            : opengl::glsl::shader<V>
                                ("gl_FragColor = vec4(indexVarying,indexVarying,indexVarying,0.5);\n",
                                 { opengl::glsl::variable<opengl::glsl::gv_varying>("indexVarying", "float", "lowp") })
                            {}
                };

                /*
                 *
                 * \tparam V GLSL shader version to produce.
                 */
                template <enum opengl::glsl::version V = opengl::glsl::ver_auto>
                class histogram : public opengl::glsl::shader<V>
                {
                    public:
                        histogram(void)
                            : opengl::glsl::shader<V>
                                ("gl_FragColor = vec4(0.995,0.995,0.995,0.995);\n")
                            {}
                };

                /*
                 *
                 * \tparam V GLSL shader version to produce.
                 */
                template <enum opengl::glsl::version V = opengl::glsl::ver_auto>
                class regular : public opengl::glsl::shader<V>
                {
                    public:
                        regular(void)
                            : opengl::glsl::shader<V>
                                ("gl_FragColor = colorVarying;\n",
                                 { opengl::glsl::variable<opengl::glsl::gv_varying>("colorVarying", "vec4", "lowp") })
                            {}
                };

                /*
                 *
                 * \tparam V GLSL shader version to produce.
                 */
                template <enum opengl::glsl::version V = opengl::glsl::ver_auto>
                class postProcess : public opengl::glsl::shader<V>
                {
                    public:
                        postProcess(void)
                            : opengl::glsl::shader<V>
                                ("highp float intensity = 1.0 - texture2D(screenHistogram, UV).x;\n"
                                 "highp float index     = texture2D(screenFramebuffer, UV).x;\n"
                                 "gl_FragColor = texture2D(colourMap, vec2(index,0.0)) * intensity;\n",
                                 { opengl::glsl::variable<opengl::glsl::gv_varying>("UV", "vec2", "lowp") },
                                 { opengl::glsl::variable<opengl::glsl::gv_uniform>("screenFramebuffer", "sampler2D"),
                                   opengl::glsl::variable<opengl::glsl::gv_uniform>("screenHistogram", "sampler2D"),
                                   opengl::glsl::variable<opengl::glsl::gv_uniform>("colourMap", "sampler2D") })
                            {}
                };
            };
        };
    };

    namespace opengl
    {
        /**\brief OpenGL render programme
         *
         * This class encapsulates a typical, single-pass OpenGL programme to
         * render things on the primary framebuffer.
         *
         * \tparam Q Base data type for calculations.
         * \tparam V GLSL shader version to produce.
         */
        template<typename Q, unsigned int d, enum glsl::version V = glsl::ver_auto>
        class renderProgramme
        {
            public:
                /**\brief Default constructor
                 *
                 * Initialises, but does not compile, the render programme
                 * with the correct shaders.
                 */
                renderProgramme (void)
                    : initialised(false), programme(),
                      clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
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
                bool matrices (const geometry::transformation::affine<Q,d> &combined,
                               const math::matrix<Q,d,d> &normalMatrix)
                {
                    return programme.uniform("mvp3", combined.transformationMatrix, d > 2)
                        && programme.uniform("normalMatrix", normalMatrix, false);
                }

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

                        programme.copy();
                    }

                    programme.use(width, height);
                    glDepthMask(GL_TRUE);
                    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    glEnable(GL_DEPTH_TEST);
                    glDepthFunc(GL_LEQUAL);

                    glClear(clear);

                    draw();

                    return true;
                }

                /**\brief Clear flags
                 *
                 * These are the flags to glClear() that are used every time you
                 * try to render a scene with this render programme. Set to 0 to
                 * not clear anything.
                 *
                 * \see https://www.opengl.org/sdk/docs/man2/xhtml/glClear.xml
                 *      for a description of the possible flags.
                 */
                GLint clear;

                /**\brief Vertex render programme
                 *
                 * A shorthand type template for the standard render programme
                 * at this depth. The type alias template is required, because
                 * the framebuffer render programme only takes templates without
                 * a depth specifier, so we need to cut that out of the type.
                 *
                 * \tparam Ve The GLSL version to generate a programme for.
                 */
                template<enum opengl::glsl::version Ve>
                using vertex = render::glsl::vertex::regular<d,Ve>;

                /**\brief Framebuffer render programme
                 *
                 * This is the framebuffer render programme that is used to
                 * render the scene with.
                 */
                renderToFramebufferProgramme
                    <Q,V,vertex,render::glsl::fragment::regular> programme;

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
         * \tparam V GLSL shader version to produce.
         *
         * \see http://flam3.com/flame_draves.pdf for a description of the
         *      colouring algorithm these render programmes are based on.
         */
        template<typename Q, unsigned int d, enum glsl::version V = glsl::ver_auto>
        class fractalFlameRenderProgramme
        {
            public:
                /**\brief Default constructor
                 *
                 * Initialises, but does not compile, the render programmes
                 * with the correct shaders.
                 */
                fractalFlameRenderProgramme (void)
                    : initialised(false), colouring(), histogram(), postProcess()
                    {}

                /**\brief Load matrix uniforms
                 *
                 * Updates the projection and normal matrices of the OpenGL
                 * programmes with the specified values.
                 *
                 * \param[in] combined     The combined (MVC) projective
                 *                         transformation.
                 *
                 * \return True if the uniforms were uploaded successfully,
                 *         false otherwise.
                 */
                bool matrices (const geometry::transformation::affine<Q,d> &combined)
                {
                    return histogram.uniform("mvp3", combined.transformationMatrix, d > 2)
                        && colouring.uniform("mvp3", combined.transformationMatrix, d > 2);
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
                            -1.0f, -1.0f,
                             1.0f, -1.0f,
                            -1.0f,  1.0f,
                            -1.0f,  1.0f,
                             1.0f, -1.0f,
                             1.0f,  1.0f
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
                    
                    postProcess.uniform("screenFramebuffer", 0);
                    postProcess.uniform("screenHistogram", 1);
                    
                    glActiveTexture(GL_TEXTURE0 + 2);
                    colourMap.bind();
                    postProcess.uniform("colourMap", 2);
                    
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

                /**\brief Vertex render programme
                 *
                 * A shorthand type template for the fractal flame render
                 * programme at this depth. The type alias template is required,
                 * because the framebuffer render programme only takes templates
                 * without a depth specifier, so we need to cut that out of the
                 * type.
                 *
                 * \tparam Ve The GLSL version to generate a programme for.
                 */
                template<enum opengl::glsl::version Ve>
                using vertex = render::glsl::vertex::fractalFlame<d,Ve>;

                /**\brief Colouring render pass programme
                 *
                 * Stores the OpenGL programme that creates the colour index
                 * texture for the flame algorithm. This colour information is
                 * used to sample the colour map.
                 */
                renderToTextureProgramme<Q,V,vertex,render::glsl::fragment::fractalFlame> colouring;

                /**\brief Histogram render pass programme
                 *
                 * Stores the OpenGL programme that creates the histogram
                 * texture for the flame algorithm. This information is used to
                 * calculate the output intensity of individual fragments.
                 */
                renderToTextureProgramme<Q,V,vertex,render::glsl::fragment::histogram> histogram;

                /**\brief Merge pass programme
                 *
                 * Stores the OpenGL programme that combines the textures
                 * creates in the first two passes to output fragments for the
                 * output framebuffer.
                 */
                renderToFramebufferProgramme<Q,V,render::glsl::vertex::postProcess,render::glsl::fragment::postProcess> postProcess;

                /**\brief Vertex array for fullscreen quad
                 *
                 * Stores the vertex array for a fullscreen quad, which is used
                 * in the merge pass to emit fragments for each pixel of the
                 * output viewport.
                 */
                vertexArrayMinimal<Q,2> vertexArray;

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
        template<typename Q, unsigned int d>
        class opengl;

        /**\brief Common OpenGL context data
         *
         * This specialisation of the opengl class template contains all the
         * common data that applies to all depths of a render context, such as
         * geometry buffers or the dimensions of the output.
         *
         * \tparam Q The base numeric type you intend to use.
         */
        template<typename Q>
        class opengl<Q,0>
        {
            public:
                /**\brief Default constructor
                 *
                 * Initialise an object with sane defaults.
                 */
                opengl (void)
                    : prepared(false),
                      fractalFlameColouring(false)
                    {}

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

                /**\brief Wireframe colour
                 *
                 * The colour that wireframes are drawn in. The array elements
                 * are the red, green, blue and alpha components, in that order.
                 */
                math::vector<GLfloat,4,math::format::RGB> wireframeColour;

                /**\brief Surface colour
                 *
                 * The colour that surfaces are drawn in. The array elements are
                 * the red, green, blue and alpha components, in that order.
                 */
                math::vector<GLfloat,4,math::format::RGB> surfaceColour;

                /**\brief Vertex buffer elements
                 *
                 * Temporary storage for the vertex elements. This is committed
                 * to the vertexbuffer after all lines and tris have been drawn
                 * to the scene.
                 */
                std::vector<GLfloat> vertices;

                /**\brief Triangle indices
                 *
                 * Temporary storage for the triangle data used to draw surfaces
                 * to the OpenGL context. This is committed to the elementbuffer
                 * once all mesh elements have been drawn.
                 */
                std::vector<unsigned int> triindices;

                /**\brief Line indices
                 *
                 * Temporary storage for the index data used to draw lines to
                 * the OpenGL context. This is committed to the linebuffer once
                 * all mesh elements have been drawn.
                 */
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

                /**\brief Vertex buffer
                 *
                 * Contains vertex data for the mesh to be rendered.
                 */
                efgy::opengl::vertexBuffer vertexbuffer;

                /**\brief Index buffer for surfaces
                 *
                 * Contains indices into the vertex buffer that make up the
                 * surfaces of models to be rendered.
                 */
                efgy::opengl::indexBuffer elementbuffer;

                /**\brief Index buffer for lines
                 *
                 * Contains indices into the vertex buffer that make up the
                 * outlines of models to be rendered.
                 */
                efgy::opengl::indexBuffer linebuffer;

                /**\brief Use Fractal Flame Colouring?
                 *
                 * Set to true to use the fractal flame colouring algorithm
                 * instead of the 'normal' render programme.
                 */
                bool fractalFlameColouring;

                /**\brief Add vertex to vertex buffer
                 *
                 * Appends the given vertex data to the vertex buffer. The
                 * buffer is extended as necessary.
                 *
                 * \param[in] c     coordinates of the vertex.
                 * \param[in] n     coordinates of the vertex's normal.
                 * \param[in] index IFS index of the vertex.
                 *
                 * \tparam e The depth of the vertex.
                 *
                 * \returns The index of the vertex that was just added.
                 */
                template<unsigned int e>
                unsigned int add
                    (math::vector<GLfloat,e> c, math::vector<GLfloat,e> n,
                     const GLfloat &index)
                {
                    vertices.insert (vertices.end(), c.begin(), c.end());
                    vertices.insert (vertices.end(), n.begin(), n.end());
                    
                    vertices.push_back(index);
                    
                    unsigned int rv = indices;
                    
                    indices++;
                    
                    return rv;
                }

                /**\brief Add polygon to buffers
                 *
                 * Draw a polygon with q vertices. The Polygon should be
                 * convex; if it isn't then you'll get rather strange results.
                 *
                 * \tparam q The number of vertices that define the polygon.
                 * \tparam e The depth of the vertices.
                 *
                 * \param[in] pV    The vertices that define the polygon.
                 * \param[in] index Source of the polygon; used when rendering
                 *                  IFSs to simulate fractal flame colouring,
                 *                  but currently ignored by the SVG renderer.
                 * \param[in] R     Normal of the polygon.
                 * \param[in] RN    Reverse normal of the polygon.
                 */
                template<unsigned int e, std::size_t q>
                void add
                    (const std::array<math::vector<Q,e>,q> &pV,
                     const math::vector<Q,e> &R,
                     const math::vector<Q,e> &RN,
                     const Q &index)
                {
                    if (prepared) return;
                    
                    triindices.push_back(add(pV[0], R, index));
                    unsigned int nstartf = triindices.back();
                    lineindices.push_back(nstartf);
                    triindices.push_back(add(pV[1], R, index));
                    lineindices.push_back(triindices.back());
                    lineindices.push_back(triindices.back());
                    triindices.push_back(add(pV[2], R, index));
                    unsigned int nendf = triindices.back();
                    lineindices.push_back(nendf);
                    
                    triindices.push_back(add(pV[2], RN, index));
                    unsigned int nendb = triindices.back();
                    lineindices.push_back(nendb);
                    triindices.push_back(add(pV[1], RN, index));
                    lineindices.push_back(triindices.back());
                    lineindices.push_back(triindices.back());
                    triindices.push_back(add(pV[0], RN, index));
                    unsigned int nstartb = triindices.back();
                    lineindices.push_back(nstartb);
                    
                    for (unsigned int j = 3; j < q; j++)
                    {
                        triindices.push_back(add(pV[0], R, index));
                        triindices.push_back(add(pV[(j-1)], R, index));
                        lineindices.push_back(triindices.back());
                        triindices.push_back(add(pV[j], R, index));
                        lineindices.push_back(triindices.back());
                        nendf = triindices.back();
                        
                        triindices.push_back(add(pV[j], RN, index));
                        nendb = triindices.back();
                        lineindices.push_back(triindices.back());
                        triindices.push_back(add(pV[(j-1)], RN, index));
                        lineindices.push_back(triindices.back());
                        triindices.push_back(add(pV[0], RN, index));
                    }
                    
                    lineindices.push_back(nendf);
                    lineindices.push_back(nstartf);
                    lineindices.push_back(nstartb);
                    lineindices.push_back(nendb);
                }

                /**\brief Upload vertex data
                 *
                 * Uploads the vertex data that has been prepared so far to the
                 * graphics card. This will also flush the vertices, triindices
                 * and lineindeces members, and reset the indices counter.
                 *
                 * \tparam e The depth of the vertex.
                 *
                 * \param[in] vertexArrayModel Vertex array model to use for the
                 *                             upload.
                 */
                template<unsigned int e>
                void upload (efgy::opengl::vertexArrayExtended<Q,e> &vertexArrayModel)
                {
                    if (!prepared)
                    {
                        prepared = true;

                        vertexArrayModel.use();
                        vertexbuffer.load(vertices.size() * sizeof(GLfloat), vertices.data());
                        elementbuffer.load(triindices.size() * sizeof(unsigned int), triindices.data());
                        linebuffer.load(lineindices.size() * sizeof(unsigned int), lineindices.data());
                        vertexArrayModel.setup();

                        tindices = GLsizei(triindices.size());
                        lindices = GLsizei(lineindices.size());

                        vertices.clear();
                        triindices.clear();
                        lineindices.clear();
                        indices = 0;

                        // log errors
                        efgy::opengl::error();
                    }
                }

                /**\brief Render lines
                 *
                 * Draws all currently prepared lines to the current OpenGL
                 * context. Used internally; the function does not modify any
                 * programme state, so everything has to be prepared in advance.
                 *
                 * \tparam e The depth of the vertex.
                 *
                 * \param[in] vertexArrayModel Vertex array model to use for the
                 *                             upload.
                 */
                template<unsigned int e>
                void pushLines (efgy::opengl::vertexArrayExtended<Q,e> &vertexArrayModel) const
                {
                    if (prepared && (wireframeColour[3] > 0.f) && !fractalFlameColouring)
                    {
                        glDepthMask ((wireframeColour[3] >= 1.f) ? GL_TRUE : GL_FALSE);

                        vertexArrayModel.use();
                        vertexbuffer.bind();
                        linebuffer.bind();
                        vertexArrayModel.setup();

                        glDrawElements(GL_LINES, lindices, GL_UNSIGNED_INT, 0);
                    }
                }

                /**\brief Render faces
                 *
                 * Draws all currently prepared faces to the current OpenGL
                 * context. Used internally; the function does not modify any
                 * programme state, so everything has to be prepared in advance.
                 *
                 * \tparam e The depth of the vertex.
                 *
                 * \param[in] vertexArrayModel Vertex array model to use for the
                 *                             upload.
                 */
                template<unsigned int e>
                void pushFaces (efgy::opengl::vertexArrayExtended<Q,e> &vertexArrayModel) const
                {
                    if (prepared && ((surfaceColour[3] > 0.f) || fractalFlameColouring))
                    {
                        glDepthMask ((surfaceColour[3] >= 1.f) ? GL_TRUE : GL_FALSE);

                        vertexArrayModel.use();
                        vertexbuffer.bind();
                        elementbuffer.bind();
                        vertexArrayModel.setup();

                        glDrawElements(GL_TRIANGLES, tindices, GL_UNSIGNED_INT, 0);
                    }
                }
        };

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
                    : transformation(pTransformation), projection(pProjection),
                      lowerRenderer(pLowerRenderer),
                      context(pLowerRenderer.context)
                    {}

                /**\brief Start new frame
                 *
                 * Calculates the combined transformation and projection matrix
                 * and the normal matrix, then prepares the OpenGL state for a
                 * new frame.
                 */
                void frameStart (void)
                {
                    combined = transformation * projection;
                    lowerRenderer.frameStart();
                };

                /**\brief End frame
                 *
                 * Indicates that the currently rendered frame is now complete
                 * and that it can be uploaded to the graphics card. The frame
                 * is then rendered with the currently active renderer.
                 */
                void frameEnd (void) const
                {
                    lowerRenderer.frameEnd();
                };

                /**\brief Clear framebuffer
                 *
                 * Calls the lower rendere's clear() method to clear the
                 * framebuffer. The bits argument decides which parts to clear.
                 *
                 * \param[in] bits A bitmask of what to clear.
                 *
                 * \see https://www.opengl.org/sdk/docs/man2/xhtml/glClear.xml
                 *      for a description of the possible flags for bits.
                 */
                void clear (GLint bits)
                {
                    lowerRenderer.clear(bits);
                }

                /**\brief Draw polygon
                 *
                 * Draw a polygon with q vertices. The Polygon should be
                 * convex; if it isn't then you'll get rather strange results.
                 *
                 * \tparam q The number of vertices that define the polygon.
                 *
                 * \param[in] pV    The vertices that define the polygon.
                 * \param[in] index Source of the polygon; used when rendering
                 *                  IFSs to simulate fractal flame colouring,
                 *                  but currently ignored by the SVG renderer.
                 */
                template<std::size_t q>
                void draw
                    (const std::array<math::vector<Q,d>,q> &pV, const Q &index = 0.5) const
                {
                    if (context.prepared) return;

                    std::array<math::vector<Q,d-1>,q> V;

                    std::transform
                        (pV.begin(), pV.end(), V.begin(),
                         [&](const math::vector<Q,d> &s)
                          -> math::vector<Q,d-1>
                    {
                        return combined * s;
                    });

                    lowerRenderer.draw(V, index);
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

                /**\copydoc opengl<Q,2>::context */
                opengl<Q,0> &context;

            protected:
                /**\copydoc opengl<Q,2>::render */
                efgy::opengl::renderProgramme<Q,d> render;

                /**\copydoc opengl<Q,2>::fractalFlame */
                efgy::opengl::fractalFlameRenderProgramme<Q,d> fractalFlame;

                /**\brief Affine transformation matrix
                 *
                 * This is a reference to the affine transformation matrix that
                 * any vectors are pre-multiplied with.
                 */
                const geometry::transformation::affine<Q,d> &transformation;

                /**\brief Projective transformation
                 *
                 * This is a reference to the projective transformation that is
                 * used to decrease any vector's number of dimensions by one,
                 * so that the lower renderer can do its job.
                 */
                const geometry::projection<Q,d> &projection;

                /**\brief Combined projective transformation
                 *
                 * Calculated by frameStart() to speed up rendering by merging
                 * the affine pre-transformation and the projective matrices.
                 */
                geometry::transformation::projective<Q,d> combined;

                /**\brief Lower renderer
                 *
                 * A reference to the renderer that drawing commands are passed
                 * to; OpenGL renderers are chained together so that you can
                 * render objects with arbitrarily large numbers of spatial
                 * dimensions, even though OpenGL natively operates in 3D.
                 */
                opengl<Q,d-1> &lowerRenderer;
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
                      context(pLowerRenderer.context),
                      lowerRenderer(pLowerRenderer)
                    {}

                /**\copydoc opengl<Q,2>::frameStart */
                void frameStart (void)
                {
                    const geometry::transformation::projective<Q,3> combined = transformation * projection;

                    if (context.fractalFlameColouring)
                    {
                        fractalFlame.matrices (combined);
                    }
                    else
                    {
                        const math::matrix<Q,3,3> normalMatrix = math::transpose(math::invert(math::transpose(math::matrix<Q,3,3>(transformation.transformationMatrix))));
                        render.matrices (combined, normalMatrix);
                    }
                };

                /**\copydoc opengl<Q,2>::frameEnd */
                void frameEnd (void)
                {
                    context.upload(vertexArrayModel);

                    if (context.fractalFlameColouring)
                    {
                        fractalFlame (context.width, context.height, [this] ()
                                      {
                                          context.pushFaces(vertexArrayModel);
                                      });
                    }
                    else
                    {
                        render (context.width, context.height, [this] ()
                                {
                                    render.programme.uniform("colour", context.wireframeColour);
                                    context.pushLines(vertexArrayModel);
                                    render.programme.uniform("colour", context.surfaceColour);
                                    context.pushFaces(vertexArrayModel);
                                });
                    }
                };

                /**\copydoc opengl<Q,2>::clear */
                void clear (GLint bits)
                {
                    render.clear = bits;
                }

                /**\copydoc opengl<Q,2>::draw */
                template<std::size_t q>
                void draw
                    (const std::array<math::vector<Q,3>,q> &pV, const Q &index = 0.5)
                {
                    if (context.prepared) return;

                    math::vector<Q,3> R
                        = math::normalise
                            (crossProduct(pV[1] - pV[0], pV[2] - pV[0]));

                    math::vector<Q,3> RN
                        = math::normalise
                            (crossProduct(pV[2] - pV[0], pV[1] - pV[0]));

                    context.add(pV, R, RN, index);
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

                /**\copydoc opengl<Q,2>::context */
                opengl<Q,0> &context;

                /**\copydoc opengl<Q,2>::vertexArrayModel */
                efgy::opengl::vertexArrayExtended<Q,3> vertexArrayModel;

            protected:
                /**\brief 3D affine transformation
                 *
                 * Contains the affine transformation matrix that vertices are
                 * multiplied with before they're projected.
                 */
                const geometry::transformation::affine<Q,3> &transformation;

                /**\brief 3D projection
                 *
                 * Contains the projection matrix used to project 3D vertices to
                 * 2-space to output to the OpenGL context. This is uploaded to
                 * the graphics card so the actual projection takes place there.
                 */
                const geometry::projection<Q,3> &projection;

                /**\copydoc opengl<Q,2>::render */
                efgy::opengl::renderProgramme<Q,3> render;

                /**\copydoc opengl<Q,2>::fractalFlame */
                efgy::opengl::fractalFlameRenderProgramme<Q,3> fractalFlame;

                /**\brief Lower renderer
                 *
                 * A reference to the renderer that drawing commands are passed
                 * to; OpenGL renderers are chained together so that you can
                 * render objects with arbitrarily large numbers of spatial
                 * dimensions, even though OpenGL natively operates in 3D.
                 */
                opengl<Q,2> &lowerRenderer;
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
                constexpr opengl (const geometry::transformation::affine<Q,2> &pTransformation,
                                  const geometry::projection<Q,2> &,
                                  const opengl<Q,1> &)
                    : transformation(pTransformation)
                    {}

                /**\brief Start new frame
                 *
                 * Calculates the combined transformation and projection matrix
                 * and the normal matrix, then prepares the OpenGL state for a
                 * new frame.
                 */
                void frameStart (void)
                {
                    geometry::transformation::affine<Q,2> scale
                        = geometry::transformation::identity<Q,2>();

                    scale.transformationMatrix[1][1] = Q(context.width)/Q(context.height);

                    const geometry::transformation::affine<Q,2> combined
                        = transformation * scale;

                    if (context.fractalFlameColouring)
                    {
                        fractalFlame.matrices (combined);
                    }
                    else
                    {
                        render.matrices (combined, math::matrix<Q,2,2>());
                    }
                }

                /**\brief End frame
                 *
                 * Indicates that the currently rendered frame is now complete
                 * and that it can be uploaded to the graphics card. The frame
                 * is then rendered with the currently active renderer.
                 */
                void frameEnd (void)
                {
                    context.upload(vertexArrayModel);
                    
                    if (context.fractalFlameColouring)
                    {
                        fractalFlame (context.width, context.height, [this] ()
                                      {
                                          context.pushFaces(vertexArrayModel);
                                      });
                    }
                    else
                    {
                        render (context.width, context.height, [this] ()
                                {
                                    render.programme.uniform("colour", context.wireframeColour);
                                    context.pushLines(vertexArrayModel);
                                    render.programme.uniform("colour", context.surfaceColour);
                                    context.pushFaces(vertexArrayModel);
                                });
                    }
                };

                /**\brief Clear framebuffer
                 *
                 * Sets a flag that will cause glClear() to be called on the
                 * next render.
                 *
                 * \param[in] bits A bitmask of what to clear.
                 *
                 * \see https://www.opengl.org/sdk/docs/man2/xhtml/glClear.xml
                 *      for a description of the possible flags for bits.
                 */
                void clear (GLint bits)
                {
                    render.clear = bits;
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

                /**\brief Draw polygon
                 *
                 * Draw a polygon with q vertices. The Polygon should be
                 * convex; if it isn't then you'll get rather strange results.
                 *
                 * \tparam q The number of vertices that define the polygon.
                 *
                 * \param[in] pV    The vertices that define the polygon.
                 * \param[in] index Source of the polygon; used when rendering
                 *                  IFSs to simulate fractal flame colouring,
                 *                  but currently ignored by the SVG renderer.
                 */
                template<std::size_t q>
                void draw
                    (const std::array<math::vector<Q,2>,q> &pV, const Q &index = 0.5)
                {
                    if (context.prepared) return;
                    
                    math::vector<Q,2> R  {{ Q(1),  Q(0) }};
                    math::vector<Q,2> RN {{ Q(-1), Q(0) }};

                    context.add(pV, R, RN, index);
                }

                /**\brief Vertex array
                 *
                 * Contains vertex buffer metadata for the individual vertices
                 * in the vertex buffer.
                 */
                efgy::opengl::vertexArrayExtended<Q,2> vertexArrayModel;

                opengl<Q,0> context;

            protected:
                /**\brief 2D affine transformation
                 *
                 * Contains the affine transformation matrix that vertices are
                 * multiplied with before they're projected.
                 */
                const geometry::transformation::affine<Q,2> &transformation;

                /**\brief Regular rendering programme
                 *
                 * The "normal" rendering programme, which uses the normal
                 * surface and wireframe colours.
                 */
                efgy::opengl::renderProgramme<Q,2> render;

                /**\brief Fractal flame colouring programme
                 *
                 * Implements the fractal flame colouring algorithm as described
                 * in the class's description. Used when the
                 * fractalFlameColouring boolean is set to true.
                 */
                efgy::opengl::fractalFlameRenderProgramme<Q,2> fractalFlame;
        };

        template<typename Q> class opengl<Q,1> {};

        /**\brief std::ostream OpenGL tag
         *
         * Used to distinguish between a plain std::ostream, and one where
         * drawing instructions actually go to OpenGL and the stream is used for
         * error and debug output.
         *
         * \tparam C Character type for the basic_ostream reference.
         * \tparam Q The base numeric type you intend to use.
         * \tparam d The number of dimensions for vectors.
         */
        template <typename C, typename Q, unsigned int d>
        class oglstream
        {
            public:
                /**\brief Construct with stream reference
                 *
                 * Initialises a new ostream OpenGL tag instance.
                 *
                 * \param[out] pStream The stream to write to.
                 * \param[out] pRender The renderer instance to use.
                 */
                oglstream (std::basic_ostream<C> &pStream,
                           opengl<Q,d> &pRender)
                    : stream(pStream),
                      render(pRender)
                {}
                
                /**\brief Output stream reference
                 *
                 * This is the stream where error and debug output is written
                 * to.
                 */
                std::basic_ostream<C> &stream;
                
                /**\brief OpenGL renderer reference
                 *
                 * This is the OpenGL renderer instance that things will be
                 * rendered with, if the input requires that.
                 */
                opengl<Q,d> &render;
        };

        /**\brief Convert std::ostream to OpenGL
         *
         * Converts the given stream to an OpenGL stream so that write
         * operations after that will render to an OpenGL context and write
         * error an debug logs to the stream.
         *
         * \tparam C Character type for the basic_ostream reference.
         * \tparam Q The base numeric type you intend to use.
         * \tparam d The number of dimensions for vectors.
         *
         * \param[out] stream The stream to write to.
         * \param[out] render The OpenGL renderer to project with.
         *
         * \returns A new osvgstream with the given parameters.
         */
        template <typename C, typename Q, unsigned int d>
        constexpr inline oglstream<C,Q,d> operator << (std::basic_ostream<C> &stream, opengl<Q,d> &render)
        {
            return oglstream<C,Q,d>(stream, render);
        }

        /**\brief Draw polytope with OpenGL
         *
         * Iterates through all of the polytope's faces and then writes a 3D
         * projection of them to the OpenGL context.
         *
         * \tparam C      Character type for the basic_ostream reference.
         * \tparam Q      Base type for calculations; should be a rational type
         * \tparam od     Model depth, e.g. '2' for a square or '3' for a cube
         * \tparam d      Number of dimensions of the vector space to use
         * \tparam f      Number of vertices for mesh faces
         * \tparam format Vector coordinate format to work in, e.g.
         *                math::format::cartesian.
         *
         * \param[out] stream The stream to write to.
         * \param[in]  poly   The polytope to render with OpenGL.
         *
         * \returns A new copy of the stream that was passed in.
         */
        template <typename C, typename Q, unsigned int d,
                  unsigned int od, unsigned int f, typename format>
        static inline oglstream<C,Q,d> operator <<
            (oglstream<C,Q,d> stream,
             const geometry::polytope<Q,od,d,f,format> &poly)
        {
            if (poly.faces.size() != poly.indices.size())
            {
                for (const auto &p : poly.faces)
                {
                    std::array<math::vector<Q,d>,geometry::polytope<Q,od,d,f,format>::faceVertices> q;
                    
                    for (std::size_t i = 0; i < poly.faceVertices; i++)
                    {
                        q[i] = p[i];
                    }
                    
                    stream.render.draw(q);
                }
            }
            else
            {
                auto itIndex = poly.indices.begin();
                for (const auto &p : poly.faces)
                {
                    std::array<math::vector<Q,d>,geometry::polytope<Q,od,d,f,format>::faceVertices> q;
                    
                    for (std::size_t i = 0; i < poly.faceVertices; i++)
                    {
                        q[i] = p[i];
                    }
                    
                    stream.render.draw(q, *itIndex);
                    itIndex++;
                }
            }
            
            return stream;
        }
    };
};

#endif
