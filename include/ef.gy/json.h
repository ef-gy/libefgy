/**\file
 * \brief JSON helpers
 *
 * Contains helper functionality for JSON output.
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

#if !defined(EF_GY_JSON_H)
#define EF_GY_JSON_H

#include <map>
#include <vector>
#include <string>
#include <memory>

namespace efgy
{
    /**\brief JSON helpers
     *
     * Namespace for helper classes and functions used to generate CSS
     * serialisations.
     */
    namespace json
    {
        /**\brief JSON value
         *
         * This class is used to represent a JSON value, as defined at
         * http://www.json.org/ - the class basically holds a type tag and a
         * pointer.
         *
         * \see http://www.json.org/
         */
        template <typename numeric = long double>
        class value
        {
            public:
                value (void) : type(null), payload(0) {}
                value (const bool &pValue) : type(pValue ? yes : no), payload(0) {}
                value (const value &b) : type(null), payload(0)
                    {
                        *this = b;
                    }
                value (const std::string &b) : type(null), payload(0)
                    {
                        toString();
                        getString() = b;
                    }
                value (const numeric &b) : type(null), payload(0)
                    {
                        toNumber();
                        getNumber() = b;
                    }

                ~value (void)
                {
                    clear();
                }

                enum
                {
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
                } type;

                void *payload;

                value &operator = (const value &b)
                {
                    clear();
                    switch (b.type)
                    {
                        case object:
                            toObject();
                            getObject() = b.getObject();
                            break;
                        case array:
                            toArray();
                            getArray() = b.getArray();
                            break;
                        case string:
                            toString();
                            getString() = b.getString();
                            break;
                        case number:
                            toNumber();
                            getNumber() = b.getNumber();
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

                const std::map<std::string,value<numeric>> &getObject (void) const
                {
                    return *((const std::map<std::string,value<numeric>>*)payload);
                }

                const std::vector<value<numeric>> &getArray (void) const
                {
                    return *((const std::vector<value<numeric>>*)payload);
                }

                const std::string &getString (void) const
                {
                    return *((const std::string*)payload);
                }

                const numeric &getNumber (void) const
                {
                    return *((const numeric*)payload);
                }

                std::map<std::string,value<numeric>> &getObject (void)
                {
                    return *((std::map<std::string,value<numeric>>*)payload);
                }

                std::vector<value<numeric>> &getArray (void)
                {
                    return *((std::vector<value<numeric>>*)payload);
                }

                std::string &getString (void)
                {
                    return *((std::string*)payload);
                }

                numeric &getNumber (void)
                {
                    return *((numeric*)payload);
                }

                bool getBoolean (void) const
                {
                    return type == yes;
                }

                void toObject (void)
                {
                    clear();
                    type = object;
                    payload = new std::map<std::string,value<numeric>>();
                }

                void toArray (void)
                {
                    clear();
                    type = array;
                    payload = new std::vector<value<numeric>>();
                }

                void toString (void)
                {
                    clear();
                    type = string;
                    payload = new std::string();
                }
            
                void toNumber (void)
                {
                    clear();
                    type = number;
                    payload = new numeric();
                }

            protected:
                void clear (void)
                {
                    if (payload)
                    {
                        switch (type)
                        {
                            case object:
                                delete ((std::map<std::string,value<numeric>>*)payload);
                                break;
                            case array:
                                delete ((std::vector<value<numeric>>*)payload);
                                break;
                            case string:
                                delete ((std::string*)payload);
                                break;
                            case number:
                                delete ((numeric*)payload);
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
        };
    };
};

#endif
