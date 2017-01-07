/**\file
 * \brief Basic primitives
 *
 * This file contains code to create the meshes for basic primitives and to
 * present the results with the renderers provided by this library.
 *
 * 'Basic' primitives include cubes and simplices.
 *
 * \see parametric.h, ifs.h and flame.h for more complicated shapes, some of
 *      which are based on these primitives.
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

#if !defined(EF_GY_POLYTOPE_H)
#define EF_GY_POLYTOPE_H

#include <ef.gy/euclidian.h>
#include <ef.gy/polar.h>
#include <vector>
#include <array>
#include <algorithm>
#include <ef.gy/range.h>
#include <ef.gy/exponential.h>
#include <type_traits>
#include <iterator>

namespace efgy {
namespace geometry {
/**\defgroup libefgy-geometric-primitives Geometric Primitives
 * \brief Class templates that define basic geometric primitives
 *
 * This group contains class templates that calculate meshes for basic
 * geometric primitives, such as simplices, cubes or spheres.
 */

/**\ingroup libefgy-geometric-primitives
 * \defgroup libefgy-extended-geometric-primitives Extended Geometric Primitives
 * \brief Class templates that define extended geometric primitives
 *
 * Contains "extended" primitives - either those that reuse "basic"
 * primitives, those that are very specialised or those that can get
 * very complex.
 */

/**\brief Geometry parameters
 *
 * Contains parameters used by the shape-creating templates. You need
 * to have at least one of these for each data type you intend to use.
 *
 * \tparam Q Base data type for calculations.
 */
template <typename Q> class parameters {
public:
  /**\brief Construct with default parameters
   *
   * Initialises the parameter object with sane defaults for the
   * individual parameters.
   */
  parameters(void)
      : radius(1), radius2(0.5), constant(0.9), precision(3), iterations(4),
        functions(3), seed(0), preRotate(1), postRotate(0),
        flameCoefficients(3), vertexLimit(1000000) {}

  /**\brief Radius
   *
   * The radius of the object to be created; used by cubes,
   * simplices, spheres, most parametric surfaces, planes and just
   * about any instance of the IFS code.
   */
  Q radius;

  /**\brief Secondary radius
   *
   * Some primitives, such as tori, require two radii, so this is
   * that second radius.
   */
  Q radius2;

  /**\brief Constant
   *
   * Some formulae, e.g. the one for klein bottles, employ an
   * additional, unnamed constant that determines part of the
   * geometry.
   */
  Q constant;

  /**\brief Precision
   *
   * The precision that an object should be calculated in. Mostly
   * used to determine the smoothness of round surfaces, such as
   * those of spheres or parametric functions.
   */
  Q precision;

  /**\brief Number of iterations
   *
   * The iterations that should be used to calculate an object;
   * used primarily by the IFS code to determine the target
   * fidelity of the rendered object.
   */
  unsigned int iterations;

  /**\brief Number of functions
   *
   * Contains the number of functions for random iterated function
   * systems or fractal flames.
   */
  unsigned int functions;

  /**\brief PRNG seed
   *
   * The seed for any PRNG that is employed when creating a mesh;
   * primarily used by the random iterated function systems and
   * the random fractal flames.
   */
  unsigned int seed;

  /**\brief Allow pre-transformation rotations
   *
   * Used when generating random iterated function systems to
   * allow for a random rotation to be applied before the random
   * translation.
   */
  bool preRotate;

  /**\brief Allow post-transformation rotations
   *
   * Used when generating random iterated function systems to
   * allow for a random rotation to be applied after the random
   * translation.
   */
  bool postRotate;

  /**\brief Distinct nonzero flame coefficients
   *
   * How many distinct, nonzero flame variation coefficients to
   * allow when generating random fractal flames.
   */
  unsigned int flameCoefficients;

  /**\brief Vertex limit
   *
   * How many vertices to allow at most when generating geometry.
   * Different models deal differently with the vertex limit, but
   * generally the precision or number of iterations is reduced
   * when creating geometry that would exceed this limit.
   */
  unsigned long long vertexLimit;
};

/**\brief Flags for geometry parameters.
 *
 * Used when specifying which parameters a model uses. The actual flags are
 * template parameters, to allow for easier type aliasing and so that there does
 * not need to be any subclassing or instantiating of this class.
 */
template<bool tRadius = false, bool tRadius2 = false, bool tConstant = false,
    bool tPrecision = false, bool tIterations = false, bool tFunctions = false,
    bool tSeed = false, bool tPreRotate = false, bool tPostRotate = false,
    bool tFlameCoefficients = false>
class parameterFlags {
public:
  static const bool radius = tRadius;
  static const bool radius2 = tRadius2;
  static const bool constant = tConstant;
  static const bool precision = tPrecision;
  static const bool iterations = tIterations;
  static const bool functions = tFunctions;
  static const bool seed = tSeed;
  static const bool preRotate = tPreRotate;
  static const bool postRotate = tPostRotate;
  static const bool flameCoefficients = tFlameCoefficients;
};

/**\brief Dimensional constraints
 *
 * This class is used to hold dimensional constraints, which are in turn
 * used by the geometric model factory to figure out which dimensions to
 * allow a model to be instantiated in.
 *
 * These constraints have to be expressed at compile time using template
 * parameters. Using '0' in modelMax and renderMax means that there are
 * no constraints on the maximum number of supported dimensions.
 *
 * \note These parameters commonly depend on the model or render depth
 *       of a model, so you will quite likely have to make these
 *       parameters dependant on the depth of the model or the target
 *       render depth. For example, if you had some kind of cube to be
 *       rendered with a render depth of 'n', then you would have to set
 *       the modelMax template argument to 'n', because you can't render
 *       an (n+1)-cube in only 'n' dimensions.
 *
 * \tparam modelMin  Minimum number of model dimensions needed.
 * \tparam modelMax  Maximum number of model dimensions supported.
 */
template <std::size_t modelMin = 2, std::size_t modelMax = 0> class dimensions {
public:
  /**\brief Minimum number of model dimensions needed
   *
   * Exports the 'modelMin' parameter, which is the number of
   * dimensions that the model needs to render properly.
   */
  static const std::size_t modelDimensionMinimum = modelMin;

  /**\brief Maximum number of model dimensions supported
   *
   * Exports the 'modelMax' parameter, which is the number of
   * dimensions that the model could be rendered in.
   */
  static const std::size_t modelDimensionMaximum = modelMax;
};

/**\brief Object base template
 *
 * Contains the base definitions for any polytope, i.e. code that
 * should be shared among most class templates that generate meshes to
 * be rendered.
 *
 * \tparam Q      Base type for calculations; should be a rational type
 * \tparam od     Model depth, e.g. '2' for a square or '3' for a cube
 * \tparam d      Number of dimensions of the vector space to use
 * \tparam f      Number of vertices for mesh faces
 * \tparam Format Vector coordinate format to work in, e.g.
 *                math::format::cartesian.
 */
template <typename Q, unsigned int od, unsigned int d, unsigned int f,
          typename Format>
class object {
public:
  /**\brief Vector format type
   *
   * Holds the vector format type that was passed as an argument
   * to the template.
   */
  using format = Format;

  /**\brief Construct with renderer and parameters
   *
   * Initialises an instance of the class using a renderer
   * instance and a set of parameters. The base template does not
   * generate an actual mesh, however the class does contain
   * everything needed to render an actual mesh, so derived
   * classes should easily be able to fill in the blanks.
   *
   * \param[in]  pParameter Polytope parameters to apply.
   * \param[in]  pFormat    Coordinate format tag instance.
   */
  object(const parameters<Q> &pParameter, const format &pFormat = format())
      : parameter(pParameter), tag(pFormat) {}

  /**\brief Number of face vertices
   *
   * An alias for the 'f' template parameter, which contains the
   * number of vertices that make up an individual 'face' of the
   * resulting mesh.
   */
  static constexpr const unsigned int faceVertices = f;

  /**\brief Query the model's depth
   *
   * This static method returns a model's "depth". The exact
   * meaning of this value depends on the model in question, but
   * generally it's the number of dimensions of the surface of a
   * mesh.
   *
   * For example, the depth of a 2-sphere is "2", because while
   * you do need 3 dimensions to render a 2-sphere, the surface
   * of such a sphere is 2 dimensional since you can describe any
   * point on the surface with its (u,v) coordinates.
   *
   * A 3-cube, on the other hand, is also renderable in 3
   * dimensions, but there's no natural way to express a point on
   * the surface of such a cube with only two coordinates - you
   * could of course unroll the sides onto a texture, but it
   * still isn't "natural".
   *
   * Yeah the definition is kind of foggy; as a rule of thumb:
   * have a look at how the models are described in other places,
   * if it's called an n-something, then this 'depth' here is
   * 'n'.
   *
   * \returns The model's depth.
   */
  static constexpr const unsigned int depth = od;

  /**\brief Query the model's render depth
   *
   * This static method returns a model's render depth, which is
   * basically the number of dimensions in the vector space in
   * which it was constructed.
   *
   * \returns The model's render depth.
   */
  static constexpr const unsigned int renderDepth = d;

  /**\brief Query model ID
   *
   * Every primitive has an ID that is used to identify the type
   * at runtime. This method produces that string.
   *
   * \returns The model's ID as a C-style string.
   */
  static constexpr const char *id(void);

  /**\brief A 2D surface
   *
   * A convenient typedef for a 2D surface, commonly called a
   * 'face.'
   */
  using face = std::array<math::vector<Q, d, format>, f>;

  /**\brief Parameter reference
   *
   * A reference to the parameters used to generate the model; Set
   * in the constructor.
   */
  const parameters<Q> &parameter;

  /**\brief Coordinate format tag
   *
   * A copy of the vector format tag for this model.
   */
  const format tag;

  using usedParameters = parameterFlags<>;

  // TODO: remove this
  void calculateObject(void) const {}
};

/**\brief Polytope base template
 *
 * Separate from geometry::object to allow for easier overloads in
 * renderers.
 *
 * \tparam Q      Base type for calculations; should be a rational type
 * \tparam od     Model depth, e.g. '2' for a square or '3' for a cube
 * \tparam d      Number of dimensions of the vector space to use
 * \tparam f      Number of vertices for mesh faces
 * \tparam format Vector coordinate format to work in, e.g.
 *                math::format::cartesian.
 */
template <typename Q, unsigned int od, unsigned int d, unsigned int f,
          typename format>
class polytope : public object<Q, od, d, f, format> {
public:
  using object<Q, od, d, f, format>::object;
  using typename object<Q, od, d, f, format>::face;

  using iterator = typename std::vector<face>::const_iterator;

  iterator begin(void) const {
    return faces.begin();
  }

  iterator end(void) const {
    return faces.end();
  }

  std::size_t size(void) const {
    return faces.size();
  }

protected:
  /**\brief The actual mesh data
   *
   * Contains all the faces that this polytope's mesh is composed
   * of. Set by deriving classes.
   */
  std::vector<face> faces;
};

template <class face, class iterator>
class adaptiveIterator : public std::iterator<std::forward_iterator_tag, face> {
public:
  adaptiveIterator(iterator pIT) : it(pIT) {}

  adaptiveIterator &operator++(void) {
    it++;
    return *this;
  }

  adaptiveIterator operator++(int) {
    adaptiveIterator c = *this;
    ++(*this);
    return c;
  }

  const face operator*(void) const {
    const auto &f = *it;
    face cf;
    for (unsigned int i = 0; (i < f.size()) && (i < cf.size()); i++) {
      for (unsigned int j = 0; (j < f[i].size()) && (j < cf[i].size()); j++) {
        cf[i][j] = f[i][j];
      }
    }
    return cf;
  }

  constexpr bool operator!=(const adaptiveIterator &b) const {
    return it != b.it;
  }

  constexpr bool operator==(const adaptiveIterator &b) const {
    return it == b.it;
  }

protected:
  iterator it;
};

template <typename Q, unsigned int d, class model, class format>
class adapt : public object<Q, model::depth, d, model::faceVertices, format> {
public:
  using parent = object<Q, model::depth, d, model::faceVertices, format>;
  using iterator =
      adaptiveIterator<typename parent::face, typename model::iterator>;

  adapt(const parameters<Q> &pParameter, const format &pFormat)
      : parent(pParameter, pFormat),
        object(pParameter, typename model::format()) {}

  void calculateObject(void) { object.calculateObject(); }

  static constexpr const char *id(void) { return model::id(); }

  iterator begin(void) const { return iterator(object.begin()); }
  iterator end(void) const { return iterator(object.end()); }
  std::size_t size(void) const { return object.size(); }

  using usedParameters = typename parent::usedParameters;

protected:
  model object;
};

template <typename Q, unsigned int d, class model, class format>
using autoAdapt = typename std::conditional<
    std::is_same<format, typename model::format>::value &&
        (d == model::renderDepth),
    model, efgy::geometry::adapt<Q, d, model, format>>::type;

/**\ingroup libefgy-geometric-primitives
 * \brief The hypercube
 *
 * This template calculates the mesh for a cube in any number of
 * dimensions. For example if you set the 'od' parameter to '3', then
 * you get a regular cube, but if you set the 'od' parameter to '2' the
 * resulting mesh is a 2D square. '4' will get you a so-called
 * "tesseract", which looks like this:
 *
 * \image html geometric-primitive-4-cube.svg "A tesseract, i.e. a 4-cube"
 *
 * Cubes are a fairly simple and common primitive, and they're easy to
 * generalise to higher dimensional spaces. They also make excellent
 * base primitives for iterated function systems, because they don't
 * produce all that much output while still covering a lot of space,
 * and deformations don't lose much information.
 *
 * In terms of parameters, this primitive only makes use of the
 * parameter::radius field, which i used to determine the size of
 * the mesh. Confusingly, this radius is actually used as the edge
 * length for historical reasons.
 *
 * \tparam Q      Base datatype for calculations
 * \tparam od     The 'depth' of the hypercube; e.g. '3' for a cube
 *
 * \see http://ef.gy/documentation/topologic - Topologic, the programme
 *      that was used to render the sample image.
 *
 * \see http://en.wikipedia.org/wiki/Hypercube for more information on
 *      hypercubes in general.
 *
 * \todo Use the parameter::radius field properly; this should
 *       probably be half the diagonal of the resulting model.
 */
template <typename Q, unsigned int od>
class cube : public polytope<Q, od, od, 4, math::format::cartesian> {
private:
  using parent = polytope<Q, od, od, 4, math::format::cartesian>;

public:
  using typename parent::format;

  cube(const parameters<Q> &pParameter, const format &pFormat = format())
      : parent(pParameter, pFormat) {
    calculateObject();
  }

  using parent::parameter;

  /**\brief Number of vertices
   *
   * This is the number of vertices that the hypercube has. This
   * may not be the same as the number of vertices that, say,
   * OpenGL would have to send to the graphics card, as this does
   * not account for vertex normals.
   *
   * \note The generel, closed formula for this is (n being the
   *       depth of the cube):
   *       2^n
   */
  static const std::size_t vertices =
      math::exponentiate::integral<long long, (long long)od>::raise(
          (long long)2);

  /**\brief Number of surfaces
   *
   * This is the number of 2D surfaces that the hypercube has. It
   * helps to know this when trying to impose a limit on the
   * number of vertices in certain derived classes.
   *
   * \note The general, closed formula for this is (n being the
   *       depth of the cube):
   *       (2^(n-4))*(n-2)*(n-1)
   */
  static const std::size_t surfaces =
      math::exponentiate::integral<long long, ((long long)od - 4)>::raise(
          (long long)2) *
      ((long long)od - 2) * ((long long)od - 1);

  typedef dimensions<2, 0> dimensions;

  /**\copydoc polytope::id() */
  static constexpr const char *id(void) { return "cube"; }

#if 0
  static constexpr std::size_t size(void) {
    return surfaces;
  }
#endif

  using usedParameters = parameterFlags<true>;

  void calculateObject(void) {
    Q diameter = parameter.radius * Q(0.5);

    std::vector<std::array<math::vector<Q, parent::renderDepth, format>, 2>>
        lines;
    faces.clear();

    std::vector<math::vector<Q, parent::renderDepth, format>> points;

    points.push_back(math::vector<Q, parent::renderDepth, format>());

    for (unsigned int i : range<int>(0, od, false)) {
      std::vector<math::vector<Q, parent::renderDepth, format>> newPoints;
      std::vector<std::array<math::vector<Q, parent::renderDepth, format>, 2>>
          newLines;
      std::vector<std::array<math::vector<Q, parent::renderDepth, format>, 4>>
          newFaces;

      for (std::array<math::vector<Q, parent::renderDepth, format>, 2> &line :
           lines) {
        line[0][i] = -diameter;
        line[1][i] = -diameter;

        std::array<math::vector<Q, parent::renderDepth, format>, 2> newLine =
            line;

        newLine[0][i] = diameter;
        newLine[1][i] = diameter;

        newLines.push_back(newLine);
        newFaces.push_back({{newLine[0], newLine[1], line[1], line[0]}});
      }

      for (typename parent::face &face : faces) {
        face[0][i] = -diameter;
        face[1][i] = -diameter;
        face[2][i] = -diameter;
        face[3][i] = -diameter;

        typename parent::face newFace = face;
        newFace[0][i] = diameter;
        newFace[1][i] = diameter;
        newFace[2][i] = diameter;
        newFace[3][i] = diameter;
        newFaces.push_back(newFace);
      }

      for (math::vector<Q, parent::renderDepth, format> &v : points) {
        v[i] = -diameter;

        std::array<math::vector<Q, parent::renderDepth, format>, 2> newLine{
            {v, v}};

        newLine[1][i] = diameter;

        newPoints.push_back(newLine[1]);

        lines.push_back(newLine);
      }

      points.insert(points.end(), newPoints.begin(), newPoints.end());
      lines.insert(lines.end(), newLines.begin(), newLines.end());
      faces.insert(faces.end(), newFaces.begin(), newFaces.end());
    }
  }

protected:
  using parent::faces;
};
}
}

#endif
