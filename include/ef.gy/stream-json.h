/**\file
 * \brief JSON stream tag
 *
 * Contains the JSON stream tag used by the JSON renderer. Also contains output
 * functions for basic, atomic types and certain combined types.
 *
 * \copyright
 * Copyright (c) 2012-2015, ef.gy Project Members
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

#if !defined(EF_GY_STREAM_JSON_H)
#define EF_GY_STREAM_JSON_H

#include <ef.gy/json.h>
#include <string>
#include <ostream>
#include <sstream>
#include <array>
#include <vector>
#include <map>

namespace efgy
{
    namespace json
    {
        /**\brief std::ostream JSON tag
         *
         * Used to distinguish between a plain std::ostream, and one where the
         * output should be in JSON format.
         *
         * \tparam C Character type for the basic_ostream reference.
         */
        template <typename C>
        class ostream
        {
            public:
                /**\brief Construct with stream reference
                 *
                 * Initialises a new ostream JSON tag instance.
                 *
                 * \param[out] pStream The stream to write to.
                 */
                ostream (std::basic_ostream<C> &pStream)
                    : stream(pStream)
                    {}

                /**\brief Output stream reference
                 *
                 * This is the stream where the output is written to.
                 */
                std::basic_ostream<C> &stream;
        };

        /**\brief JSON tag
         *
         * Write this to an ostream to turn it into an json::ostream. Like this:
         *
         * \code{.cpp}
         * cout << json::tag();
         * \encode
         */
        class tag {};

        /**\brief Convert std::ostream to JSON
         *
         * Converts the given stream to a JSON stream so that write operations
         * after that will produce JSON instead of plain text.
         *
         * \tparam C Character type for the basic_ostream reference.
         *
         * \param[out] stream The stream to write to.
         */
        template <typename C>
        constexpr inline ostream<C> operator << (std::basic_ostream<C> &stream, const tag &)
        {
            return ostream<C>(stream);
        }

        /**\brief Write float to JSON stream
         *
         * Writes a JSON serialisation of a floating point number to a stream.
         *
         * \tparam C Character type for the basic_ostream reference.
         *
         * \param[out] stream The JSON stream to write to.
         * \param[in]  pValue The floating point value to serialise.
         *
         * \returns A new copy of the input stream.
         */
        template <typename C>
        static inline ostream<C> operator <<
            (ostream<C> stream,
             const float &pValue)
        {
            stream.stream << pValue;
            
            return stream;
        }

        /**\brief Write double to JSON stream
         *
         * Writes a JSON serialisation of a floating point number to a stream.
         *
         * \tparam C Character type for the basic_ostream reference.
         *
         * \param[out] stream The JSON stream to write to.
         * \param[in]  pValue The floating point value to serialise.
         *
         * \returns A new copy of the input stream.
         */
        template <typename C>
        static inline ostream<C> operator <<
            (ostream<C> stream,
             const double &pValue)
        {
            stream.stream << pValue;
            
            return stream;
        }

        /**\brief Write long double to JSON stream
         *
         * Writes a JSON serialisation of a floating point number to a stream.
         *
         * \tparam C Character type for the basic_ostream reference.
         *
         * \param[out] stream The JSON stream to write to.
         * \param[in]  pValue The floating point value to serialise.
         *
         * \returns A new copy of the input stream.
         */
        template <typename C>
        static inline ostream<C> operator <<
            (ostream<C> stream,
             const long double &pValue)
        {
            stream.stream << pValue;
            
            return stream;
        }

        /**\brief Write integer to JSON stream
         *
         * Writes a JSON serialisation of an integer to a stream.
         *
         * \tparam C Character type for the basic_ostream reference.
         *
         * \param[out] stream The JSON stream to write to.
         * \param[in]  pValue The integer value to serialise.
         *
         * \returns A new copy of the input stream.
         */
        template <typename C>
        static inline ostream<C> operator <<
            (ostream<C> stream,
             const int &pValue)
        {
            stream.stream << pValue;
            
            return stream;
        }

        /**\brief Write long integer to JSON stream
         *
         * Writes a JSON serialisation of a long integer to a stream.
         *
         * \tparam C Character type for the basic_ostream reference.
         *
         * \param[out] stream The JSON stream to write to.
         * \param[in]  pValue The long integer value to serialise.
         *
         * \returns A new copy of the input stream.
         */
        template <typename C>
        static inline ostream<C> operator <<
            (ostream<C> stream,
             const long &pValue)
        {
            stream.stream << pValue;
            
            return stream;
        }

        /**\brief Write long long integer to JSON stream
         *
         * Writes a JSON serialisation of a long long integer to a stream.
         *
         * \tparam C Character type for the basic_ostream reference.
         *
         * \param[out] stream The JSON stream to write to.
         * \param[in]  pValue The long long integer value to serialise.
         *
         * \returns A new copy of the input stream.
         */
        template <typename C>
        static inline ostream<C> operator <<
            (ostream<C> stream,
             const long long &pValue)
        {
            stream.stream << pValue;

            return stream;
        }

        /**\brief Write boolean to JSON stream
         *
         * Writes a JSON serialisation of a boolean to a stream.
         *
         * \tparam C Character type for the basic_ostream reference.
         *
         * \param[out] stream The JSON stream to write to.
         * \param[in]  pValue The boolean value to serialise.
         *
         * \returns A new copy of the input stream.
         */
        template <typename C>
        static inline ostream<C> operator <<
            (ostream<C> stream,
             const bool &pValue)
        {
            stream.stream << (pValue ? "true" : "false");
            
            return stream;
        }

        /**\brief Write string to JSON stream
         *
         * Writes a JSON serialisation of a character string to a stream.
         *
         * \tparam C Character type for the basic_ostream reference.
         *
         * \param[out] stream The JSON stream to write to.
         * \param[in]  pValue The stream to serialise.
         *
         * \returns A new copy of the input stream.
         */
        template <typename C>
        static inline ostream<C> operator <<
            (ostream<C> stream,
             const std::string &pValue)
        {
            std::string out;

            for (const C &c : pValue)
            {
                switch (c)
                {
                    case '\\':
                    case '"':
                    {
                        const char a[] = { '\\', c, 0 };
                        out.append(a, 2);
                    }
                        break;

                    default:
                        out.append(1, c);
                        break;
                }
            }

            stream.stream << "\"" << out << "\"";

            return stream;
        }

        /**\brief Write string to JSON stream
         *
         * Writes a JSON serialisation of a C-style character string to a
         * stream.
         *
         * \tparam C Character type for the basic_ostream reference.
         *
         * \param[out] stream The JSON stream to write to.
         * \param[in]  pValue The stream to serialise.
         *
         * \returns A new copy of the input stream.
         */
        template <typename C>
        static inline ostream<C> operator <<
            (ostream<C> stream,
             const char *pValue)
        {
            std::string out;
            
            for (; *pValue != 0; pValue++)
            {
                const C &c = *pValue;
                switch (c)
                {
                    case '\\':
                    case '"':
                    {
                        const char a[] = { '\\', c, 0 };
                        out.append(a, 2);
                    }
                        break;
                        
                    default:
                        out.append(1, c);
                        break;
                }
            }
            
            stream.stream << "\"" << out << "\"";
            
            return stream;
        }

        /**\brief Write array to JSON stream
         *
         * Writes a JSON serialisation of a std::array to a stream.
         *
         * \tparam C Character type for the basic_ostream reference.
         * \tparam E Element type of the array.
         * \tparam n The number of elements in the array.
         *
         * \param[out] stream The JSON stream to write to.
         * \param[in]  pValue The array to serialise.
         *
         * \returns A new copy of the input stream.
         */
        template <typename C, typename E, std::size_t n>
        static inline ostream<C> operator <<
            (ostream<C> stream,
             const std::array<E,n> &pValue)
        {
            bool notFirst = false;
            stream.stream << "[";

            for (const E &e : pValue)
            {
                if (notFirst)
                {
                    stream.stream << ",";
                }
                else
                {
                    notFirst = true;
                }
                stream << e;
            }

            stream.stream << "]";
            
            return stream;
        }

        /**\brief Write vector to JSON stream
         *
         * Writes a JSON serialisation of a std::vector to a stream.
         *
         * \tparam C Character type for the basic_ostream reference.
         * \tparam E Element type of the vector.
         *
         * \param[out] stream The JSON stream to write to.
         * \param[in]  pValue The vector to serialise.
         *
         * \returns A new copy of the input stream.
         */
        template <typename C, typename E>
        static inline ostream<C> operator <<
            (ostream<C> stream,
             const std::vector<E> &pValue)
        {
            bool notFirst = false;
            stream.stream << "[";
            
            for (const E &e : pValue)
            {
                if (notFirst)
                {
                    stream.stream << ",";
                }
                else
                {
                    notFirst = true;
                }
                stream << e;
            }
            
            stream.stream << "]";
            
            return stream;
        }

        /**\brief Write map to JSON stream
         *
         * Writes a JSON serialisation of a std::map to a stream.
         *
         * \tparam C Character type for the basic_ostream reference.
         * \tparam V Value type of the vector.
         *
         * \param[out] stream The JSON stream to write to.
         * \param[in]  pValue The map to serialise.
         *
         * \returns A new copy of the input stream.
         */
        template <typename C, typename V>
        static inline ostream<C> operator <<
            (ostream<C> stream,
             const std::map<std::string,V> &pValue)
        {
            bool notFirst = false;
            stream.stream << "{";
            
            for (const auto &e : pValue)
            {
                if (notFirst)
                {
                    stream.stream << ",";
                }
                else
                {
                    notFirst = true;
                }
                stream << e.first;
                stream.stream << ":";
                stream << e.second;
            }
            
            stream.stream << "}";
            
            return stream;
        }

        /**\brief Write map to JSON stream
         *
         * Writes a JSON serialisation of a std::map to a stream.
         *
         * \tparam C Character type for the basic_ostream reference.
         * \tparam V Value type of the vector.
         *
         * \param[out] stream The JSON stream to write to.
         * \param[in]  pValue The map to serialise.
         *
         * \returns A new copy of the input stream.
         */
        template <typename C, typename V>
        static inline ostream<C> operator <<
            (ostream<C> stream,
             const std::map<const char*,V> &pValue)
        {
            bool notFirst = false;
            stream.stream << "{";
            
            for (const auto &e : pValue)
            {
                if (notFirst)
                {
                    stream.stream << ",";
                }
                else
                {
                    notFirst = true;
                }
                stream << e.first;
                stream.stream << ":";
                stream << e.second;
            }
            
            stream.stream << "}";
            
            return stream;
        }

        /**\brief Write JSON value to JSON stream
         *
         * Writes a JSON serialisation of a JSON value to a stream.
         *
         * \tparam C Character type for the basic_ostream reference.
         * \tparam V Value numeric type.
         *
         * \param[out] stream The JSON stream to write to.
         * \param[in]  pValue The value to serialise.
         *
         * \returns A new copy of the input stream.
         */
        template <typename C, typename Q>
        static inline ostream<C> operator <<
            (ostream<C> stream,
             const json::value<Q> &pValue)
        {
            switch (pValue.type)
            {
                case json::value<Q>::object:
                    stream << pValue.asObject();
                    break;
                case json::value<Q>::array:
                    stream << pValue.asArray();
                    break;
                case json::value<Q>::string:
                    stream << pValue.asString();
                    break;
                case json::value<Q>::number:
                    stream << (Q)pValue;
                    break;
                case json::value<Q>::yes:
                    stream << true;
                    break;
                case json::value<Q>::no:
                    stream << false;
                    break;
                case json::value<Q>::null:
                    stream.stream << "null";
                    break;
                case json::value<Q>::comma:
                case json::value<Q>::colon:
                case json::value<Q>::error:
                case json::value<Q>::endObject:
                case json::value<Q>::endArray:
                    break;
            }

            return stream;
        }

        /**\brief Read JSON value from JSON string
         *
         * Reads a JSON serialisation from a JSON string.
         *
         * \tparam V Value numeric type.
         *
         * \param[out] stream The JSON string to read from.
         * \param[out] pValue The value to read to.
         *
         * \returns The unprocessed remainder of the JSON string.
         */
        template <typename Q>
        static inline std::string operator >>
            (std::string stream,
             json::value<Q> &pValue)
        {
            pValue = json::value<Q>();

            enum { scan,
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
                   read_nul } state = scan;

            std::stringstream ss("");
            std::string key;

            for (std::ptrdiff_t i = 0; i < stream.size(); i++)
            {
                const auto c = stream[i];
                switch (state)
                {
                    case scan:
                        switch (c)
                        {
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
                                ss.str("");
                                ss << c;
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
                                pValue.type = json::value<Q>::comma;
                                return stream.substr(i+1);
                            case ':':
                                pValue.type = json::value<Q>::colon;
                                return stream.substr(i+1);
                            case ']':
                                pValue.type = json::value<Q>::endArray;
                                return stream.substr(i+1);
                            case '}':
                                pValue.type = json::value<Q>::endObject;
                                return stream.substr(i+1);
                            case ' ':
                            case '\t':
                            case '\v':
                            case '\n':
                            case 0:
                                break;
                        }
                        break;
                    case read_object:
                    {
                        json::value<Q> v;
                        std::string nstream = stream.substr(i) >> v;
                        switch (v.type)
                        {
                            case json::value<Q>::endObject:
                                return nstream;
                            case json::value<Q>::string:
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
                    case read_object_colon:
                    {
                        json::value<Q> v;
                        std::string nstream = stream.substr(i) >> v;
                        switch (v.type)
                        {
                            case json::value<Q>::endObject:
                                return nstream;
                            case json::value<Q>::colon:
                                state = read_object_value;
                                break;
                            default:
                                break;
                        }
                        stream = nstream;
                    }
                        i = -1;
                        break;
                    case read_object_value:
                    {
                        json::value<Q> v;
                        std::string nstream = stream.substr(i) >> v;
                        switch (v.type)
                        {
                            case json::value<Q>::endObject:
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
                    case read_object_comma:
                    {
                        json::value<Q> v;
                        std::string nstream = stream.substr(i) >> v;
                        switch (v.type)
                        {
                            case json::value<Q>::comma:
                                state = read_object;
                                break;
                            case json::value<Q>::endObject:
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
                        json::value<Q> v;
                        std::string nstream = stream.substr(i) >> v;
                        if (v.type == json::value<Q>::endArray)
                        {
                            return nstream;
                        }
                        stream = nstream;
                        pValue.toArray().push_back(v);
                    }
                        i = -1;
                        break;
                    case read_array_comma:
                    {
                        json::value<Q> v;
                        std::string nstream = stream.substr(i) >> v;
                        switch (v.type)
                        {
                            case json::value<Q>::comma:
                                state = read_array;
                                break;
                            case json::value<Q>::endArray:
                                return nstream;
                            default:
                                break;
                        }
                        stream = nstream;
                    }
                        i = -1;
                        break;
                    case read_number:
                        switch (c)
                        {
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
                                ss << c;
                                break;
                            default:
                                ss >> pValue.toNumber();
                                return stream.substr(i);
                        }
                        break;
                    case read_string_escape:
                        switch (c)
                        {
                            case 't':
                                pValue.toString().append(1, '\t');
                                break;
                            case 'v':
                                pValue.toString().append(1, '\v');
                                break;
                            case 'n':
                                pValue.toString().append(1, '\n');
                                break;
                            default:
                                pValue.toString().append(1, c);
                                break;
                        }
                        state = read_string;
                        break;
                    case read_string:
                        switch (c)
                        {
                            case '"':
                                return stream.substr(i+1);
                                break;
                            case '\\':
                                state = read_string_escape;
                                break;
                            default:
                                pValue.toString().append(1, c);
                                break;
                        }
                        break;
                    case read_t:
                        if (c == 'r')
                        {
                            state = read_tr;
                            break;
                        }
                        else
                        {
                            return "";
                        }
                    case read_tr:
                        if (c == 'u')
                        {
                            state = read_tru;
                            break;
                        }
                        else
                        {
                            return "";
                        }
                    case read_tru:
                        if (c == 'e')
                        {
                            pValue.type = json::value<Q>::yes;
                            return stream.substr(i+1);
                        }
                        else
                        {
                            return "";
                        }
                    case read_f:
                        if (c == 'a')
                        {
                            state = read_fa;
                            break;
                        }
                        else
                        {
                            return "";
                        }
                    case read_fa:
                        if (c == 'l')
                        {
                            state = read_fal;
                            break;
                        }
                        else
                        {
                            return "";
                        }
                    case read_fal:
                        if (c == 's')
                        {
                            state = read_fals;
                            break;
                        }
                        else
                        {
                            return "";
                        }
                    case read_fals:
                        if (c == 'e')
                        {
                            pValue.type = json::value<Q>::no;
                            return stream.substr(i+1);
                        }
                        else
                        {
                            return "";
                        }
                    case read_n:
                        if (c == 'u')
                        {
                            state = read_nu;
                            break;
                        }
                        else
                        {
                            return "";
                        }
                    case read_nu:
                        if (c == 'l')
                        {
                            state = read_nul;
                            break;
                        }
                        else
                        {
                            return "";
                        }
                    case read_nul:
                        if (c == 'l')
                        {
                            pValue.type = json::value<Q>::null;
                            return stream.substr(i+1);
                        }
                        else
                        {
                            return "";
                        }
                }
            }

            switch (state)
            {
                case read_number:
                    ss >> pValue.toNumber();
                    break;
                default:
                    break;
            }

            return "";
        }
    };
};

#endif
