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
}
}

#endif
