/* JSON streams,
 *
 * Contains the JSON stream tag used by the JSON renderer. Also contains output
 * functions for basic, atomic types and certain combined types.
 *
 * See also:
 * * Project Documentation: https://ef.gy/documentation/libefgy
 * * Project Source Code: https://github.com/ef-gy/libefgy
 * * Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 *
 * @copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 */

#if !defined(EF_GY_STREAM_JSON_H)
#define EF_GY_STREAM_JSON_H

#include <array>
#include <iomanip>
#include <limits>
#include <map>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include <ef.gy/json.h>

namespace efgy {
namespace json {
/* std::ostream JSON tag
 *
 * Used to distinguish between a plain std::ostream, and one where the
 * output should be in JSON format.
 *
 * @C Character type for the basic_ostream reference.
 */
template <typename C>
class ostream {
 public:
  /* Construct with stream reference
   *
   * Initialises a new ostream JSON tag instance.
   *
   * @pStream The stream to write to.
   */
  ostream(std::basic_ostream<C> &pStream) : stream(pStream) {}

  /* Output stream reference
   *
   * This is the stream where the output is written to.
   */
  std::basic_ostream<C> &stream;
};

/* JSON tag
 *
 * Write this to an ostream to turn it into an json::ostream. Like this:
 *
 *     cout << json::tag();
 */
class tag {};

/* Convert std::ostream to JSON
 *
 * Converts the given stream to a JSON stream so that write operations
 * after that will produce JSON instead of plain text.
 *
 * @C Character type for the basic_ostream reference.
 *
 * @stream The stream to write to.
 */
template <typename C>
constexpr inline ostream<C> operator<<(std::basic_ostream<C> &stream,
                                       const tag &) {
  return ostream<C>(stream);
}

/* Write float to JSON stream
 *
 * Writes a JSON serialisation of a floating point number to a stream.
 *
 * @C Character type for the basic_ostream reference.
 *
 * @stream The JSON stream to write to.
 * @pValue The floating point value to serialise.
 *
 * @return A new copy of the input stream.
 */
template <typename C>
static inline ostream<C> operator<<(ostream<C> stream, const float &pValue) {
  stream.stream << pValue;

  return stream;
}

/* Write double to JSON stream
 *
 * Writes a JSON serialisation of a floating point number to a stream.
 *
 * @C Character type for the basic_ostream reference.
 *
 * @stream The JSON stream to write to.
 * @pValue The floating point value to serialise.
 *
 * @return A new copy of the input stream.
 */
template <typename C>
static inline ostream<C> operator<<(ostream<C> stream, const double &pValue) {
  stream.stream << std::setprecision(std::numeric_limits<double>::digits10)
                << pValue;

  return stream;
}

/* Write long double to JSON stream
 *
 * Writes a JSON serialisation of a floating point number to a stream.
 *
 * @C Character type for the basic_ostream reference.
 *
 * @stream The JSON stream to write to.
 * @pValue The floating point value to serialise.
 *
 * @return A new copy of the input stream.
 */
template <typename C>
static inline ostream<C> operator<<(ostream<C> stream,
                                    const long double &pValue) {
  stream.stream << std::setprecision(std::numeric_limits<long double>::digits10)
                << pValue;

  return stream;
}

/* Write integer to JSON stream
 *
 * Writes a JSON serialisation of an integer to a stream.
 *
 * @C Character type for the basic_ostream reference.
 *
 * @stream The JSON stream to write to.
 * @pValue The integer value to serialise.
 *
 * @return A new copy of the input stream.
 */
template <typename C>
static inline ostream<C> operator<<(ostream<C> stream, const int &pValue) {
  stream.stream << pValue;

  return stream;
}

/* Write long integer to JSON stream
 *
 * Writes a JSON serialisation of a long integer to a stream.
 *
 * @C Character type for the basic_ostream reference.
 *
 * @stream The JSON stream to write to.
 * @pValue The long integer value to serialise.
 *
 * @return A new copy of the input stream.
 */
template <typename C>
static inline ostream<C> operator<<(ostream<C> stream, const long &pValue) {
  stream.stream << pValue;

  return stream;
}

/* Write long long integer to JSON stream
 *
 * Writes a JSON serialisation of a long long integer to a stream.
 *
 * @C Character type for the basic_ostream reference.
 *
 * @stream The JSON stream to write to.
 * @pValue The long long integer value to serialise.
 *
 * @return A new copy of the input stream.
 */
template <typename C>
static inline ostream<C> operator<<(ostream<C> stream,
                                    const long long &pValue) {
  stream.stream << pValue;

  return stream;
}

/* Write boolean to JSON stream
 *
 * Writes a JSON serialisation of a boolean to a stream.
 *
 * @C Character type for the basic_ostream reference.
 *
 * @stream The JSON stream to write to.
 * @pValue The boolean value to serialise.
 *
 * @return A new copy of the input stream.
 */
template <typename C>
static inline ostream<C> operator<<(ostream<C> stream, const bool &pValue) {
  stream.stream << (pValue ? "true" : "false");

  return stream;
}

/* Write string to JSON stream
 *
 * Writes a JSON serialisation of a character string to a stream.
 *
 * @C Character type for the basic_ostream reference.
 *
 * @stream The JSON stream to write to.
 * @pValue The stream to serialise.
 *
 * @return A new copy of the input stream.
 */
template <typename C>
static inline ostream<C> operator<<(ostream<C> stream,
                                    const std::string &pValue) {
  std::string out;

  for (const C &c : pValue) {
    switch (c) {
      case '\\':
      case '"': {
        const char a[] = {'\\', c, 0};
        out.append(a, 2);
      } break;

      default:
        out.append(1, c);
        break;
    }
  }

  stream.stream << "\"" << out << "\"";

  return stream;
}

/* Write string to JSON stream
 *
 * Writes a JSON serialisation of a C-style character string to a
 * stream.
 *
 * @C Character type for the basic_ostream reference.
 *
 * @stream The JSON stream to write to.
 * @pValue The stream to serialise.
 *
 * @return A new copy of the input stream.
 */
template <typename C>
static inline ostream<C> operator<<(ostream<C> stream, const char *pValue) {
  std::string out;

  for (; *pValue != 0; pValue++) {
    const C &c = *pValue;
    switch (c) {
      case '\\':
      case '"': {
        const char a[] = {'\\', c, 0};
        out.append(a, 2);
      } break;

      default:
        out.append(1, c);
        break;
    }
  }

  stream.stream << "\"" << out << "\"";

  return stream;
}

/* Write array to JSON stream
 *
 * Writes a JSON serialisation of a std::array to a stream.
 *
 * @C Character type for the basic_ostream reference.
 * @E Element type of the array.
 * @n The number of elements in the array.
 *
 * @stream The JSON stream to write to.
 * @pValue The array to serialise.
 *
 * @return A new copy of the input stream.
 */
template <typename C, typename E, std::size_t n>
static inline ostream<C> operator<<(ostream<C> stream,
                                    const std::array<E, n> &pValue) {
  bool notFirst = false;
  stream.stream << "[";

  for (const E &e : pValue) {
    if (notFirst) {
      stream.stream << ",";
    } else {
      notFirst = true;
    }
    stream << e;
  }

  stream.stream << "]";

  return stream;
}

/* Write vector to JSON stream
 *
 * Writes a JSON serialisation of a std::vector to a stream.
 *
 * @C Character type for the basic_ostream reference.
 * @E Element type of the vector.
 *
 * @stream The JSON stream to write to.
 * @pValue The vector to serialise.
 *
 * @return A new copy of the input stream.
 */
template <typename C, typename E>
static inline ostream<C> operator<<(ostream<C> stream,
                                    const std::vector<E> &pValue) {
  bool notFirst = false;
  stream.stream << "[";

  for (const E &e : pValue) {
    if (notFirst) {
      stream.stream << ",";
    } else {
      notFirst = true;
    }
    stream << e;
  }

  stream.stream << "]";

  return stream;
}

/* Write map to JSON stream
 *
 * Writes a JSON serialisation of a std::map to a stream.
 *
 * @C Character type for the basic_ostream reference.
 * @V Value type of the vector.
 *
 * @stream The JSON stream to write to.
 * @pValue The map to serialise.
 *
 * @return A new copy of the input stream.
 */
template <typename C, typename V>
static inline ostream<C> operator<<(ostream<C> stream,
                                    const std::map<std::string, V> &pValue) {
  bool notFirst = false;
  stream.stream << "{";

  for (const auto &e : pValue) {
    if (notFirst) {
      stream.stream << ",";
    } else {
      notFirst = true;
    }
    stream << e.first;
    stream.stream << ":";
    stream << e.second;
  }

  stream.stream << "}";

  return stream;
}

/* Write map to JSON stream
 *
 * Writes a JSON serialisation of a std::map to a stream.
 *
 * @C Character type for the basic_ostream reference.
 * @V Value type of the vector.
 *
 * @stream The JSON stream to write to.
 * @pValue The map to serialise.
 *
 * @return A new copy of the input stream.
 */
template <typename C, typename V>
static inline ostream<C> operator<<(ostream<C> stream,
                                    const std::map<const char *, V> &pValue) {
  bool notFirst = false;
  stream.stream << "{";

  for (const auto &e : pValue) {
    if (notFirst) {
      stream.stream << ",";
    } else {
      notFirst = true;
    }
    stream << e.first;
    stream.stream << ":";
    stream << e.second;
  }

  stream.stream << "}";

  return stream;
}

/* Write JSON value to JSON stream
 *
 * Writes a JSON serialisation of a JSON value to a stream.
 *
 * @C Character type for the basic_ostream reference.
 * @V Value numeric type.
 *
 * @stream The JSON stream to write to.
 * @pValue The value to serialise.
 *
 * @return A new copy of the input stream.
 */
template <typename C, typename Q>
static inline ostream<C> operator<<(ostream<C> stream,
                                    const efgy::json::value<Q> &pValue) {
  switch (pValue.type) {
    case object:
      stream << pValue.asObject();
      break;
    case array:
      stream << pValue.asArray();
      break;
    case string:
      stream << pValue.asString();
      break;
    case number:
      stream << (Q)pValue;
      break;
    case yes:
      stream << true;
      break;
    case no:
      stream << false;
      break;
    case null:
      stream.stream << "null";
      break;
    case comma:
    case colon:
    case error:
    case endObject:
    case endArray:
      break;
  }

  return stream;
}
}
}

#endif
