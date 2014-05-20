/**\file
 * \brief JSON stream tag
 *
 * Contains the JSON stream tag used by the JSON renderer. Also contains output
 * functions for basic, atomic types and certain combined types.
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
 * \see Project Source Code: https://github.com/ef-gy/libefgy.git
 */

#if !defined(EF_GY_STREAM_JSON_H)
#define EF_GY_STREAM_JSON_H

#include <ef.gy/json.h>
#include <string>
#include <ostream>
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

        /**\brief Write boolea  to JSON stream
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
                stream << e.first << ":" << e.second;
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
    };
};

#endif
