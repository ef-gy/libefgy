#if !defined(EF_GY_VERSION_H)
#define EF_GY_VERSION_H

/**\file
 * \brief Version information
 *
 * This header contains the library's version number and some additional
 * documentation that didn't seem to fit in anywhere else.
 */

/**\defgroup example-programmes Example Programmes
 * \brief Programmes designed to demonstrate the library's capabilities
 *
 * The library's repository contains a few programmes which aren't exactly part
 * of the library itself and which weren't quite fancy enough to warrant their
 * own repository, but they're kinda neat nontheless and they do show off some
 * of the libary's capabilities. You can find these example programmes in the
 * 'src/' directory, and they'll be built automatically if you ever do run a
 * plain 'make' in the top level of the repository.
 */

/**\dir include/ef.gy
 * \brief Library headers
 *
 * The library headers are all located at include/ef.gy in the repository.
 * Since this is a template libary these headers also contain the actual
 * implementation of the library, much like Pascal code tends to include both
 * the interface and the implementation in a single file.
 *
 * To "install" the library, all you really need to do is to copy this
 * directory to your own "include" directory - either globally or in your
 * project. Better yet, symlink this location to a checkout of the repository,
 * then you can update that with git every now and then and enjoy the latest
 * documentation and features of the library.
 */

/**\brief Base namespace
 *
 * This is the base namespace for all the headers contained within libefgy.
 * This namespace is used to prevent namespace clashes with your own code - if
 * you don't want to use fully qualified names in your code, use the 'using'
 * directive after including the headers, like so:
 *
 * \code
 * using namespace efgy;
 * \endcode
 */
namespace efgy {
/**\brief Library version
 *
 * This is the version of the header files you're including. You could test
 * this if you expect trouble with certain versions, or you know that your
 * code requires a very specific version of these headers.
 *
 * \note libefgy uses a very simple integer versioning scheme. There are no
 *       'major' and 'minor' versions and releases - every release gets the
 *       next available number, and that's it. This seems uncommon these
 *       days, but when you think about it, it makes a lot of sense:
 *       there's no such things as 'minor' changes, either you changed
 *       something or you haven't. Any change, no matter how minute, will
 *       ruin someone's day, so why not just stick with a plain ol' number
 *       that you can test for much more easily than a convoluted version
 *       string? ;)
 */
static const unsigned int version = 8;
}

#endif
