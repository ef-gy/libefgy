/**\file
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

#if !defined(EF_GY_TRACER_H)
#define EF_GY_TRACER_H

#include <string>
#include <sstream>
#include <memory>

namespace efgy
{
    namespace math
    {
        /**\brief Numeric operation tracer
         *
         * Implements a template class that acts like a numeric data type, but
         * instead of performing any calculations it will log any operations
         * performed on it. This would allow you to, e.g. find out which
         * calculations a template function actually performs at runtime.
         */
        namespace tracer
        {
            template<typename T, typename S, char op, bool runtime, typename format>
            class tracer;

            /**\brief Output formatter base
             *
             * Generic output formatter base template. Only creates a typedef
             * for the result type.
             *
             * \tparam R The result type that the data needs to be formatted to.
             */
            template<typename R>
            class formatter
            {
                public:
                    /**\brief Result type
                     *
                     * Convenient typedef for the result type, which was set as
                     * a template parameter.
                     */
                    typedef R result;
            };

            /**\brief Standard string output formatter
             *
             * The default output formatter for the tracer; formats content as
             * a std::string for output or further processing.
             */
            template<>
            class formatter<std::string>
            {
                public:
                    /**\brief Result type
                     *
                     * Convenient typedef for the result type, which was set as
                     * a template parameter.
                     */
                    typedef std::string result;

                    /**\brief Format two-parameter operation
                     *
                     * Formats an operation with two parameters. The output is
                     * always fully-paranthesised to avoid ambiguities.
                     *
                     * \tparam T       Type of the first argument.
                     * \tparam S       Type of the second argument.
                     * \tparam op      The applicable operator.
                     * \tparam runtime Whether or not this is a runtime tracer.
                     *
                     * \param[in] t The operation to format.
                     */
                    template<typename T, typename S, char op, bool runtime>
                    static result format (const tracer<T,S,op,runtime,formatter> &t)
                    {
                        std::stringstream s("");
                        if (op)
                        {
                            s << "(" << t.value1 << " " << op << " " << t.value2 << ")";
                        }
                        else
                        {
                            s << "(" << t.value1 << " " << t.value2 << ")";
                        }
                        return s.str();
                    }

                    /**\brief Format single-parameter operation
                     *
                     * Formats an operation with one parameters. The output is
                     * always fully-paranthesised to avoid ambiguities.
                     *
                     * \tparam T       Type of the argument.
                     * \tparam op      The applicable operator.
                     * \tparam runtime Whether or not this is a runtime tracer.
                     *
                     * \param[in] t The operation to format.
                     */
                    template<typename T, char op, bool runtime>
                    static result format (const tracer<T,void,op,runtime,formatter> &t)
                    {
                        std::stringstream s("");
                        if (op)
                        {
                            s << "(" << op << t.value1 << ")";
                        }
                        else
                        {
                            s << t.value1;
                        }
                        return s.str();
                    }
            };

            /**\brief Tracer base class
             *
             * The base class for the numeric tracer. Sets up virtual attributes
             * when required by the tracer.
             *
             * \tparam runtime Whether a runtime tracer is needed.
             * \tparam format  The formatter for the tracer.
             */
            template<bool runtime = false, typename format = formatter<std::string>>
            class base
            {
                public:
                    /**\brief Virtual base destructor
                     *
                     * Empty, but defined so that derived classes can have their
                     * destructors called when necessary.
                     */
                    virtual ~base (void) {}

                    /**\brief Format the tracer result
                     *
                     * Cast the tracer result to the formatter's return type to
                     * format the result.
                     *
                     * \returns The formatted result.
                     */
                    virtual operator typename format::result (void) const
                    {
                        return "unknown";
                    }
            };

            /**\brief Non-runtime tracer base class
             *
             * Empty base class, used for a non-runtime tracer.
             *
             * \tparam format  The formatter for the tracer.
             */
            template<typename format>
            class base<false,format> {};

            /**\brief Runtime tracer type
             *
             * Convenient typedef for a runtime tracer with the standard output
             * formatter.
             */
            typedef std::shared_ptr<base<true>> runtime;

            template<typename T, typename S = void, char op = 0, bool runtime = false, typename format = formatter<std::string>>
            class tracer : public base<runtime,format>
            {
                public:
                    constexpr tracer (const T &p1, const S &p2)
                        : value1(p1), value2(p2) {}

                    operator typename format::result (void) const
                    {
                        return format::format(*this);
                    }

                    operator std::shared_ptr<tracer> (void)
                    {
                        return std::shared_ptr<tracer>(new tracer (*this));
                    }

                    const T value1;
                    const S value2;
            };

            template<typename T, char op, bool runtime, typename format>
            class tracer<T,void,op,runtime,format> : public base<runtime,format>
            {
                public:
                    constexpr tracer (const T &p1)
                        : value1(p1) {}

                    operator std::string (void) const
                    {
                        return format::format(*this);
                    }

                    operator std::shared_ptr<tracer> (void)
                    {
                        return std::shared_ptr<tracer>(new tracer (*this));
                    }

                    const T value1;
            };

            template<typename format>
            class tracer<void,void,0,true,format> : public base<true,format>
            {
                public:
                    tracer (std::string pName)
                        : name(pName) {}

                    operator std::string (void) const
                    {
                        return name;
                    }

                    operator std::shared_ptr<tracer> (void)
                    {
                        return std::shared_ptr<tracer>(new tracer (*this));
                    }

                    const std::string name;
            };

            template<typename format>
            class tracer<void,void,0,false,format> : public base<false,format>
            {
                public:
                    tracer (const char *pName)
                        : name(pName) {}

                    operator std::string (void) const
                    {
                        return std::string(name);
                    }

                    operator std::shared_ptr<tracer> (void)
                    {
                        return std::shared_ptr<tracer>(new tracer (*this));
                    }

                    const char *name;
            };

            template<typename T, typename S, char op>
            static inline std::ostream & operator << (std::ostream &s, const tracer<T,S,op,false> &t)
            {
                return (s << std::string(t));
            }

            template<typename T, typename S, char op>
            static inline std::ostream & operator << (std::ostream &s, const tracer<T,S,op,true> &t)
            {
                return (s << std::string(t));
            }

            template<typename T, typename S, char op>
            static inline std::ostream & operator << (std::ostream &s, const base<true> &t)
            {
                return (s << std::string(t));
            }

            template<typename T, typename S, char op>
            static inline std::ostream & operator << (std::ostream &s, const std::shared_ptr<tracer<T,S,op,true>> &t)
            {
                return t ? (s << *t) : (s << "0");
            }

            static inline std::ostream & operator << (std::ostream &s, const runtime &t)
            {
                return t ? (s << std::string(*t)) : (s << "0");
            }

            // add

            template<typename T, typename S, typename R, char op>
            constexpr static inline tracer<tracer<T,S,op,false>,R,'+',false> operator + (const tracer<T,S,op,false> &p1, const R &p2)
            {
                return tracer<tracer<T,S,op,false>,R,'+',false> (p1, p2);
            }

            template<typename T, typename S, typename R, char op>
            constexpr static inline tracer<R,tracer<T,S,op,false>,'+',false> operator + (const R &p1, const tracer<T,S,op,false> &p2)
            {
                return tracer<R,tracer<T,S,op,false>,'+',false> (p1, p2);
            }

            template<typename T, typename S, typename R, typename Q, char op1, char op2>
            constexpr static inline tracer<tracer<R,Q,op1,false>,tracer<T,S,op2,false>,'+',false> operator + (const tracer<R,Q,op1,false> &p1, const tracer<T,S,op2,false> &p2)
            {
                return tracer<tracer<R,Q,op1,false>,tracer<T,S,op2,false>,'+',false> (p1, p2);
            }

            template<typename R>
            static inline std::shared_ptr<tracer<runtime,R,'+',true>> operator + (const runtime &p1, const R &p2)
            {
                return std::shared_ptr<tracer<runtime,R,'+',true>> (new tracer<runtime,R,'+',true>(p1, p2));
            }

            template<typename R>
            static inline std::shared_ptr<tracer<R,runtime,'+',true>> operator + (const R &p1, const runtime &p2)
            {
                return std::shared_ptr<tracer<R,runtime,'+',true>> (new tracer<R,runtime,'+',true>(p1, p2));
            }

            static inline std::shared_ptr<tracer<runtime,runtime,'+',true>> operator + (const runtime &p1, const runtime &p2)
            {
                return std::shared_ptr<tracer<runtime,runtime,'+',true>> (new tracer<runtime,runtime,'+',true>(p1, p2));
            }

            // subtract

            template<typename T, typename S, typename R, char op>
            constexpr static inline tracer<tracer<T,S,op,false>,R,'-',false> operator - (const tracer<T,S,op,false> &p1, const R &p2)
            {
                return tracer<tracer<T,S,op,false>,R,'-',false> (p1, p2);
            }

            template<typename T, typename S, typename R, char op>
            constexpr static inline tracer<R,tracer<T,S,op,false>,'-',false> operator - (const R &p1, const tracer<T,S,op,false> &p2)
            {
                return tracer<R,tracer<T,S,op,false>,'-',false> (p1, p2);
            }

            template<typename T, typename S, typename R, typename Q, char op1, char op2>
            constexpr static inline tracer<tracer<R,Q,op1,false>,tracer<T,S,op2,false>,'-',false> operator - (const tracer<R,Q,op1,false> &p1, const tracer<T,S,op2,false> &p2)
            {
                return tracer<tracer<R,Q,op1,false>,tracer<T,S,op2,false>,'-',false> (p1, p2);
            }

            template<typename R>
            static inline std::shared_ptr<tracer<runtime,R,'-',true>> operator - (const runtime &p1, const R &p2)
            {
                return std::shared_ptr<tracer<runtime,R,'-',true>> (new tracer<runtime,R,'-',true>(p1, p2));
            }

            template<typename R>
            static inline std::shared_ptr<tracer<R,runtime,'-',true>> operator - (const R &p1, const runtime &p2)
            {
                return std::shared_ptr<tracer<R,runtime,'-',true>> (new tracer<R,runtime,'-',true>(p1, p2));
            }

            static inline std::shared_ptr<tracer<runtime,runtime,'-',true>> operator - (const runtime &p1, const runtime &p2)
            {
                return std::shared_ptr<tracer<runtime,runtime,'-',true>> (new tracer<runtime,runtime,'-',true>(p1, p2));
            }

            // multiply

            template<typename T, typename S, typename R, char op>
            constexpr static inline tracer<tracer<T,S,op,false>,R,'*',false> operator * (const tracer<T,S,op,false> &p1, const R &p2)
            {
                return tracer<tracer<T,S,op,false>,R,'*',false> (p1, p2);
            }

            template<typename T, typename S, typename R, char op>
            constexpr static inline tracer<R,tracer<T,S,op,false>,'*',false> operator * (const R &p1, const tracer<T,S,op,false> &p2)
            {
                return tracer<R,tracer<T,S,op,false>,'*',false> (p1, p2);
            }

            template<typename T, typename S, typename R, typename Q, char op1, char op2>
            constexpr static inline tracer<tracer<R,Q,op1,false>,tracer<T,S,op2,false>,'*',false> operator * (const tracer<R,Q,op1,false> &p1, const tracer<T,S,op2,false> &p2)
            {
                return tracer<tracer<R,Q,op1,false>,tracer<T,S,op2,false>,'*',false> (p1, p2);
            }

            template<typename R>
            static inline std::shared_ptr<tracer<runtime,R,'*',true>> operator * (const runtime &p1, const R &p2)
            {
                return std::shared_ptr<tracer<runtime,R,'*',true>> (new tracer<runtime,R,'*',true>(p1, p2));
            }

            template<typename R>
            static inline std::shared_ptr<tracer<R,runtime,'*',true>> operator * (const R &p1, const runtime &p2)
            {
                return std::shared_ptr<tracer<R,runtime,'*',true>> (new tracer<R,runtime,'*',true>(p1, p2));
            }

            static inline std::shared_ptr<tracer<runtime,runtime,'*',true>> operator * (const runtime &p1, const runtime &p2)
            {
                return std::shared_ptr<tracer<runtime,runtime,'*',true>> (new tracer<runtime,runtime,'*',true>(p1, p2));
            }

            // divide

            template<typename T, typename S, typename R, char op>
            constexpr static inline tracer<tracer<T,S,op,false>,R,'/',false> operator / (const tracer<T,S,op,false> &p1, const R &p2)
            {
                return tracer<tracer<T,S,op,false>,R,'/',false> (p1, p2);
            }

            template<typename T, typename S, typename R, char op>
            constexpr static inline tracer<R,tracer<T,S,op,false>,'/',false> operator / (const R &p1, const tracer<T,S,op,false> &p2)
            {
                return tracer<R,tracer<T,S,op,false>,'/',false> (p1, p2);
            }

            template<typename T, typename S, typename R, typename Q, char op1, char op2>
            constexpr static inline tracer<tracer<R,Q,op1,false>,tracer<T,S,op2,false>,'/',false> operator / (const tracer<R,Q,op1,false> &p1, const tracer<T,S,op2,false> &p2)
            {
                return tracer<tracer<R,Q,op1,false>,tracer<T,S,op2,false>,'/',false> (p1, p2);
            }

            template<typename R>
            static inline std::shared_ptr<tracer<runtime,R,'/',true>> operator / (const runtime &p1, const R &p2)
            {
                return std::shared_ptr<tracer<runtime,R,'/',true>> (new tracer<runtime,R,'/',true>(p1, p2));
            }

            template<typename R>
            static inline std::shared_ptr<tracer<R,runtime,'/',true>> operator / (const R &p1, const runtime &p2)
            {
                return std::shared_ptr<tracer<R,runtime,'/',true>> (new tracer<R,runtime,'/',true>(p1, p2));
            }

            static inline std::shared_ptr<tracer<runtime,runtime,'/',true>> operator / (const runtime &p1, const runtime &p2)
            {
                return std::shared_ptr<tracer<runtime,runtime,'/',true>> (new tracer<runtime,runtime,'/',true>(p1, p2));
            }

            // combined assignment operators

            static inline runtime operator += (runtime &p1, const runtime &p2)
            {
                return p1 = (p1 + p2);
            }

            static inline runtime operator -= (runtime &p1, const runtime &p2)
            {
                return p1 = (p1 - p2);
            }

            static inline runtime operator *= (runtime &p1, const runtime &p2)
            {
                return p1 = (p1 * p2);
            }

            static inline runtime operator /= (runtime &p1, const runtime &p2)
            {
                return p1 = (p1 / p2);
            }
        };
    };
};

#endif
