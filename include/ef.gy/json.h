/* JSON values.
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

#if !defined(EF_GY_JSON_H)
#define EF_GY_JSON_H

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace efgy {
namespace json {
/* JSON value type.
 *
 * A JSON value has one of a very limited number of types: object, array,
 * string, number, boolean or null.
 *
 * There's also a few special type-ish tags in this list, which are needed by
 * the parser to keep track of certain bits of state but which should not be
 * used.
 */
enum type {
  object,
  array,
  string,
  number,
  yes,
  no,
  null,
  comma,
  colon,
  error,
  endArray,
  endObject
};

/* JSON value.
 * @tNumeric The numeric base data type for your JSON data.
 *
 * This class is used to represent a JSON value, as defined at
 * http://www.json.org/ - the class basically holds a type tag and a pointer.
 *
 * See http://www.json.org/ for more details on the format.
 */
template <typename tNumeric>
class value {
 public:
  /* JSON object type.
   *
   * Objects are essentially dictionaries that map from strings to JSON values.
   */
  using objectType = std::map<std::string, value>;

  /* JSON array type.
   *
   * Arrays are a list of JSON values.
   */
  using arrayType = std::vector<value>;

  /* JSON string type.
   *
   * We really just use standard C++ strings.
   */
  using stringType = std::string;

  /* Numeric base type.
   *
   * The precision that we can in floating point numbers depends on this type.
   * The default is 'long double' and should only be widened from there.
   */
  using numberType = tNumeric;

  value(void) : type(null), payload(0) {}
  value(const bool &pValue) : type(pValue ? yes : no), payload(0) {}
  value(const value &b) : type(null), payload(0) { *this = b; }
  value(const char *b) : type(null), payload(0) { toString() = b; }
  value(const stringType &b) : type(null), payload(0) { toString() = b; }
  value(const numberType &b) : type(null), payload(0) { toNumber() = b; }
  value(const arrayType &b) : type(null), payload(0) { toArray() = b; }
  value(const objectType &b) : type(null), payload(0) { toObject() = b; }

  ~value(void) { clear(); }

  enum type type;

  value &operator=(const value &b) {
    clear();
    switch (b.type) {
      case object:
        toObject() = b.asObject();
        break;
      case array:
        toArray() = b.asArray();
        break;
      case string:
        toString() = b.asString();
        break;
      case number:
        toNumber() = b.asNumber();
        break;
      case yes:
      case no:
      case null:
      case comma:
      case colon:
      case error:
      case endObject:
      case endArray:
        type = b.type;
        break;
    }
    return *this;
  }

  /* Returns the number of elements this value represents.
   *
   * The size of a number, string or other value is 1, the size of an array or
   * object is the length of that array or object.
   *
   * @returns The number of elements in this value.
   */
  const std::size_t size(void) const {
    if (isObject()) {
      const auto &a = *((const objectType *)payload);
      return a.size();
    } else if (isArray()) {
      const auto &a = *((const arrayType *)payload);
      return a.size();
    }
    return 1;
  }

  const value operator()(const stringType &i) const {
    if (isObject()) {
      return (*((const objectType *)payload))[i];
    }
    return value();
  }

  value &operator()(const stringType &i) { return toObject()[i]; }

  constexpr const bool isObject(void) const { return type == object; }

  const objectType asObject(void) const {
    if (!isObject()) {
      return objectType();
    }
    return *((const objectType *)payload);
  }

  objectType &toObject(void) {
    if (!isObject()) {
      clear();
      type = object;
      payload = new objectType();
    }
    return *((objectType *)payload);
  }

  value &push(const value &v) {
    toArray().push_back(v);
    return *this;
  }

  const value operator[](const std::size_t &i) const {
    if (isArray()) {
      const auto &a = *((const arrayType *)payload);
      if (i < a.size()) {
        return a[i];
      }
    }
    return value();
  }

  value &operator[](const std::size_t &i) { return toArray()[i]; }

  constexpr const bool isArray(void) const { return type == array; }

  const arrayType asArray(void) const {
    if (!isArray()) {
      return arrayType();
    }
    return *((const arrayType *)payload);
  }

  arrayType &toArray(void) {
    if (!isArray()) {
      clear();
      type = array;
      payload = new arrayType();
    }
    return *((arrayType *)payload);
  }

  constexpr const bool isString(void) const { return type == string; }

  const stringType asString(void) const {
    if (!isString()) {
      return "";
    }
    return *((const stringType *)payload);
  }

  stringType &toString(void) {
    if (!isString()) {
      clear();
      type = string;
      payload = new stringType();
    }
    return *((stringType *)payload);
  }

  constexpr operator const numberType(void) const { return asNumber(); }

  constexpr explicit operator bool(void) const { return type == yes; }

  constexpr const bool isNumber(void) const { return type == number; }

  const numberType asNumber(void) const {
    if (!isNumber()) {
      return 0;
    }
    return *((numberType *)payload);
  }

  numberType &toNumber(void) {
    if (!isNumber()) {
      clear();
      type = number;
      payload = new numberType();
    }
    return *((numberType *)payload);
  }

 protected:
  void clear(void) {
    if (payload) {
      switch (type) {
        case object:
          delete ((objectType *)payload);
          break;
        case array:
          delete ((arrayType *)payload);
          break;
        case string:
          delete ((stringType *)payload);
          break;
        case number:
          delete ((numberType *)payload);
          break;
        case yes:
        case no:
        case null:
        case comma:
        case colon:
        case error:
        case endObject:
        case endArray:
          break;
      }
      payload = 0;
    }
    type = null;
  }

  void *payload;
};

/* Default JSON value type.
 *
 * Usually you'd want to use a 'long double' for numbers, and you don't want to
 * put empty angle brackets everywhere, so type alias is to help both of these
 * needs.
 */
using json = value<long double>;

/* Read JSON value from JSON string
 *
 * Reads a JSON serialisation from a JSON string.
 *
 * @V Value numeric type.
 *
 * @stream The JSON string to read from.
 * @pValue The value to read to.
 *
 * @return The unprocessed remainder of the JSON string.
 */
template <typename Q>
static inline std::string parse(std::string stream,
                                efgy::json::value<Q> &pValue) {
  pValue = value<Q>();

  enum {
    scan,
    read_object,
    read_object_colon,
    read_object_value,
    read_object_comma,
    read_array,
    read_array_comma,
    read_string,
    read_string_escape,
    read_number,
    read_t,
    read_tr,
    read_tru,
    read_f,
    read_fa,
    read_fal,
    read_fals,
    read_n,
    read_nu,
    read_nul
  } state = scan;

  std::string ss;
  std::string key;

  for (std::ptrdiff_t i = 0; i < stream.size(); i++) {
    const auto c = stream[i];
    if (state >= read_t) {
      if (state == read_t && c == 'r') {
        state = read_tr;
      } else if (state == read_tr && c == 'u') {
        state = read_tru;
      } else if (state == read_tru && c == 'e') {
        pValue.type = yes;
        return stream.substr(i + 1);
      } else if (state == read_f && c == 'a') {
        state = read_fa;
      } else if (state == read_fa && c == 'l') {
        state = read_fal;
      } else if (state == read_fal && c == 's') {
        state = read_fals;
      } else if (state == read_fals && c == 'e') {
        pValue.type = no;
        return stream.substr(i + 1);
      } else if (state == read_n && c == 'u') {
        state = read_nu;
      } else if (state == read_nu && c == 'l') {
        state = read_nul;
      } else if (state == read_nul && c == 'l') {
        pValue.type = null;
        return stream.substr(i + 1);
      } else {
        // this should raise a parse errorr
        return "";
      }
    } else
      switch (state) {
        case scan:
          switch (c) {
            case '{':
              state = read_object;
              pValue.toObject();
              break;
            case '[':
              state = read_array;
              pValue.toArray();
              break;
            case '"':
              state = read_string;
              pValue.toString();
              break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '+':
            case '-':
              state = read_number;
              pValue.toNumber();
              ss.clear();
              ss.push_back(c);
              break;
            case 't':
              state = read_t;
              break;
            case 'f':
              state = read_f;
              break;
            case 'n':
              state = read_n;
              break;
            case ',':
              pValue.type = comma;
              return stream.substr(i + 1);
            case ':':
              pValue.type = colon;
              return stream.substr(i + 1);
            case ']':
              pValue.type = endArray;
              return stream.substr(i + 1);
            case '}':
              pValue.type = endObject;
              return stream.substr(i + 1);
            case ' ':
            case '\t':
            case '\v':
            case '\n':
            case 0:
              break;
          }
          break;
        case read_object: {
          efgy::json::value<Q> v;
          std::string nstream = parse(stream.substr(i), v);
          switch (v.type) {
            case endObject:
              return nstream;
            case string:
              key = v.asString();
              state = read_object_colon;
              break;
            default:
              break;
          }
          stream = nstream;
        }
          i = -1;
          break;
        case read_object_colon: {
          efgy::json::value<Q> v;
          std::string nstream = parse(stream.substr(i), v);
          switch (v.type) {
            case endObject:
              return nstream;
            case colon:
              state = read_object_value;
              break;
            default:
              break;
          }
          stream = nstream;
        }
          i = -1;
          break;
        case read_object_value: {
          efgy::json::value<Q> v;
          std::string nstream = parse(stream.substr(i), v);
          switch (v.type) {
            case endObject:
              return nstream;
            default:
              pValue.toObject()[key] = v;
              state = read_object_comma;
              break;
          }
          stream = nstream;
        }
          i = -1;
          break;
        case read_object_comma: {
          efgy::json::value<Q> v;
          std::string nstream = parse(stream.substr(i), v);
          switch (v.type) {
            case comma:
              state = read_object;
              break;
            case endObject:
              return nstream;
            default:
              break;
          }
          stream = nstream;
        }
          i = -1;
          break;
        case read_array:
          state = read_array_comma;
          {
            efgy::json::value<Q> v;
            std::string nstream = parse(stream.substr(i), v);
            if (v.type == endArray) {
              return nstream;
            }
            stream = nstream;
            pValue.toArray().push_back(v);
          }
          i = -1;
          break;
        case read_array_comma: {
          efgy::json::value<Q> v;
          std::string nstream = parse(stream.substr(i), v);
          switch (v.type) {
            case comma:
              state = read_array;
              break;
            case endArray:
              return nstream;
            default:
              break;
          }
          stream = nstream;
        }
          i = -1;
          break;
        case read_number:
          switch (c) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case 'e':
            case 'E':
            case '.':
            case '+':
            case '-':
              ss.push_back(c);
              break;
            default:
              pValue.toNumber() = std::stold(ss);
              return stream.substr(i);
          }
          break;
        case read_string_escape:
          switch (c) {
            case 'b':
              pValue.toString().push_back('\b');
              break;
            case 'f':
              pValue.toString().push_back('\f');
              break;
            case 'n':
              pValue.toString().push_back('\n');
              break;
            case 'r':
              pValue.toString().push_back('\r');
              break;
            case 't':
              pValue.toString().push_back('\t');
              break;
            default:
              pValue.toString().push_back(c);
              break;
          }
          state = read_string;
          break;
        case read_string:
          switch (c) {
            case '"':
              return stream.substr(i + 1);
              break;
            case '\\':
              state = read_string_escape;
              break;
            default:
              pValue.toString().push_back(c);
              break;
          }
        default:
          break;
      }
  }

  if (state == read_number) {
    pValue.toNumber() = std::stold(ss);
  }

  return "";
}

/* Parse JSON value.
 * @input What to parse.
 *
 * Parses the given input under the assumption that it's a valid JSON string.
 * This only works for the default JSON value type, because there's no way to
 * infer the correct numeric type.
 *
 * @return The parsed version of input.
 */
static inline json parse(const std::string &input) {
  json rv;
  parse(input, rv);
  return rv;
}

/* Stringify a JSON value.
 * @Q Numeric type for the JSON value.
 * @v The value to stringify.
 *
 * Turns a JSON value into a JSON string.
 *
 * @return A string representation of v.
 */
template <typename Q>
static inline std::string to_string(const value<Q> &v) {
  switch (v.type) {
    case object: {
      bool first = true;
      std::string rv = "{";

      for (const auto &e : v.asObject()) {
        rv += (first ? "" : ",") + to_string(json(e.first)) + ":" +
              to_string(e.second);
        first = false;
      }

      return rv + "}";
    }
    case array: {
      bool first = true;
      std::string rv = "[";

      for (const auto &e : v.asArray()) {
        rv += (first ? "" : ",") + to_string(e);
        first = false;
      }

      return rv + "]";
    }
    case string: {
      std::string rv = "\"";
      for (const auto &c : v.asString()) {
        switch (c) {
          case '\\':
          case '"':
            rv.push_back('\\');
            rv.push_back(c);
            break;
          case '\b':
            rv.push_back('\\');
            rv.push_back('b');
            break;
          case '\f':
            rv.push_back('\\');
            rv.push_back('f');
            break;
          case '\n':
            rv.push_back('\\');
            rv.push_back('n');
            break;
          case '\r':
            rv.push_back('\\');
            rv.push_back('r');
            break;
          case '\t':
            rv.push_back('\\');
            rv.push_back('t');
            break;
          default:
            rv.push_back(c);
            break;
        }
      }
      return rv + "\"";
    }
    case number: {
      std::string n = std::to_string(v.asNumber());
      while (n[(n.size() - 1)] == '0') {
        n.pop_back();
      }
      if (n[(n.size() - 1)] == '.') {
        n.pop_back();
      }
      return n;
    }
    case yes:
      return "true";
    case no:
      return "false";
    case null:
      return "null";
    default:
      // don't do anything for other types.
      return "";
  }
}
}  // namespace json
}  // namespace efgy

#endif
