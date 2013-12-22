/**\file
 *
 * \copyright
 * Copyright (c) 2012-2013, ef.gy Project Members
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
 * \see Project Source Code: http://git.becquerel.org/jyujin/libefgy.git
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
            template<typename T, typename S>
            class coalesce;

            template<typename T>
            class coalesce<T,T>
            {
                public:
                    typedef T type;
            };

            template<typename T, typename S, char op, bool runtime>
            class tracer;

            template<typename R>
            class formatter
            {
                public:
                    typedef R result;

                    template<typename T, typename S, char op, bool runtime>
                    static R format (const tracer<T,S,op,runtime> &);
            };

            template<bool runtime = false>
            class base
            {
                public:
                    virtual ~base (void) {}

                    virtual operator std::string (void) const
                    {
                        return "unknown";
                    }

                    virtual char getOperator (void) const
                    {
                        return 0;
                    }

                    virtual std::string trace (void) const
                    {
                        return "";
                    }

                    virtual int value (void) const
                    {
                        return 0;
                    }
            };

            template<>
            class base<false> {};

            typedef std::shared_ptr<base<true>> runtime;

            template<typename T, typename S = void, char op = 0, bool runtime = false>
            class tracer : public base<runtime>
            {
                public:
                    constexpr tracer (const T &p1, const S &p2)
                        : value1(p1), value2(p2) {}

                    operator std::string (void) const
                    {
                        std::stringstream s("");
                        if (op)
                        {
                            s << "(" << value1 << " " << op << " " << value2 << ")";
                        }
                        else
                        {
                            s << "(" << value1 << " " << value2 << ")";
                        }
                        return s.str();
                    }

                    char getOperator (void) const
                    {
                        return op;
                    }

                    operator std::shared_ptr<tracer> (void)
                    {
                        return std::shared_ptr<tracer>(new tracer (*this));
                    }

                    const T value1;
                    const S value2;
            };

            template<typename T, char op, bool runtime>
            class tracer<T,void,op,runtime> : public base<runtime>
            {
                public:
                    constexpr tracer (const T &p1)
                        : value1(p1) {}

                    operator std::string (void) const
                    {
                        std::stringstream s("");
                        if (op)
                        {
                            s << "(" << op << value1 << ")";
                        }
                        else
                        {
                            s << value1;
                        }
                        return s.str();
                    }

                    char getOperator (void) const
                    {
                        return op;
                    }

                    operator std::shared_ptr<tracer> (void)
                    {
                        return std::shared_ptr<tracer>(new tracer (*this));
                    }

                    const T value1;
            };

            template<>
            class tracer<void,void,0,true> : public base<true>
            {
                public:
                    tracer (std::string pName)
                        : name(pName) {}

                    operator std::string (void) const
                    {
                        return name;
                    }

                    char getOperator (void) const
                    {
                        return 0;
                    }

                    operator std::shared_ptr<tracer> (void)
                    {
                        return std::shared_ptr<tracer>(new tracer (*this));
                    }

                    const std::string name;
            };

            template<>
            class tracer<void,void,0,false> : public base<false>
            {
                public:
                    tracer (const char *pName)
                        : name(pName) {}

                    operator std::string (void) const
                    {
                        return std::string(name);
                    }

                    char getOperator (void) const
                    {
                        return 0;
                    }

                    operator std::shared_ptr<tracer> (void)
                    {
                        return std::shared_ptr<tracer>(new tracer (*this));
                    }

                    const char *name;
            };

            template<typename T, typename S, char op>
            std::ostream & operator << (std::ostream &s, const tracer<T,S,op,false> &t)
            {
                return (s << std::string(t));
            }

            template<typename T, typename S, char op>
            std::ostream & operator << (std::ostream &s, const tracer<T,S,op,true> &t)
            {
                return (s << std::string(t));
            }

            template<typename T, typename S, char op>
            std::ostream & operator << (std::ostream &s, const base<true> &t)
            {
                return (s << std::string(t));
            }

            template<typename T, typename S, char op>
            std::ostream & operator << (std::ostream &s, const std::shared_ptr<tracer<T,S,op,true>> &t)
            {
                return t ? (s << *t) : (s << "0");
            }

            std::ostream & operator << (std::ostream &s, const runtime &t)
            {
                return t ? (s << std::string(*t)) : (s << "0");
            }

            // add

            template<typename T, typename S, typename R, char op>
            constexpr tracer<tracer<T,S,op,false>,R,'+',false> operator + (const tracer<T,S,op,false> &p1, const R &p2)
            {
                return tracer<tracer<T,S,op,false>,R,'+',false> (p1, p2);
            }

            template<typename T, typename S, typename R, char op>
            constexpr tracer<R,tracer<T,S,op,false>,'+',false> operator + (const R &p1, const tracer<T,S,op,false> &p2)
            {
                return tracer<R,tracer<T,S,op,false>,'+',false> (p1, p2);
            }

            template<typename T, typename S, typename R, typename Q, char op1, char op2>
            constexpr tracer<tracer<R,Q,op1,false>,tracer<T,S,op2,false>,'+',false> operator + (const tracer<R,Q,op1,false> &p1, const tracer<T,S,op2,false> &p2)
            {
                return tracer<tracer<R,Q,op1,false>,tracer<T,S,op2,false>,'+',false> (p1, p2);
            }

            template<typename R>
            std::shared_ptr<tracer<runtime,R,'+',true>> operator + (const runtime &p1, const R &p2)
            {
                return std::shared_ptr<tracer<runtime,R,'+',true>> (new tracer<runtime,R,'+',true>(p1, p2));
            }

            template<typename R>
            std::shared_ptr<tracer<R,runtime,'+',true>> operator + (const R &p1, const runtime &p2)
            {
                return std::shared_ptr<tracer<R,runtime,'+',true>> (new tracer<R,runtime,'+',true>(p1, p2));
            }

            std::shared_ptr<tracer<runtime,runtime,'+',true>> operator + (const runtime &p1, const runtime &p2)
            {
                return std::shared_ptr<tracer<runtime,runtime,'+',true>> (new tracer<runtime,runtime,'+',true>(p1, p2));
            }

            // subtract

            template<typename T, typename S, typename R, char op>
            constexpr tracer<tracer<T,S,op,false>,R,'-',false> operator - (const tracer<T,S,op,false> &p1, const R &p2)
            {
                return tracer<tracer<T,S,op,false>,R,'-',false> (p1, p2);
            }

            template<typename T, typename S, typename R, char op>
            constexpr tracer<R,tracer<T,S,op,false>,'-',false> operator - (const R &p1, const tracer<T,S,op,false> &p2)
            {
                return tracer<R,tracer<T,S,op,false>,'-',false> (p1, p2);
            }

            template<typename T, typename S, typename R, typename Q, char op1, char op2>
            constexpr tracer<tracer<R,Q,op1,false>,tracer<T,S,op2,false>,'-',false> operator - (const tracer<R,Q,op1,false> &p1, const tracer<T,S,op2,false> &p2)
            {
                return tracer<tracer<R,Q,op1,false>,tracer<T,S,op2,false>,'-',false> (p1, p2);
            }

            template<typename R>
            std::shared_ptr<tracer<runtime,R,'-',true>> operator - (const runtime &p1, const R &p2)
            {
                return std::shared_ptr<tracer<runtime,R,'-',true>> (new tracer<runtime,R,'-',true>(p1, p2));
            }

            template<typename R>
            std::shared_ptr<tracer<R,runtime,'-',true>> operator - (const R &p1, const runtime &p2)
            {
                return std::shared_ptr<tracer<R,runtime,'-',true>> (new tracer<R,runtime,'-',true>(p1, p2));
            }

            std::shared_ptr<tracer<runtime,runtime,'-',true>> operator - (const runtime &p1, const runtime &p2)
            {
                return std::shared_ptr<tracer<runtime,runtime,'-',true>> (new tracer<runtime,runtime,'-',true>(p1, p2));
            }

            // multiply

            template<typename T, typename S, typename R, char op>
            constexpr tracer<tracer<T,S,op,false>,R,'*',false> operator * (const tracer<T,S,op,false> &p1, const R &p2)
            {
                return tracer<tracer<T,S,op,false>,R,'*',false> (p1, p2);
            }

            template<typename T, typename S, typename R, char op>
            constexpr tracer<R,tracer<T,S,op,false>,'*',false> operator * (const R &p1, const tracer<T,S,op,false> &p2)
            {
                return tracer<R,tracer<T,S,op,false>,'*',false> (p1, p2);
            }

            template<typename T, typename S, typename R, typename Q, char op1, char op2>
            constexpr tracer<tracer<R,Q,op1,false>,tracer<T,S,op2,false>,'*',false> operator * (const tracer<R,Q,op1,false> &p1, const tracer<T,S,op2,false> &p2)
            {
                return tracer<tracer<R,Q,op1,false>,tracer<T,S,op2,false>,'*',false> (p1, p2);
            }

            template<typename R>
            std::shared_ptr<tracer<runtime,R,'*',true>> operator * (const runtime &p1, const R &p2)
            {
                return std::shared_ptr<tracer<runtime,R,'*',true>> (new tracer<runtime,R,'*',true>(p1, p2));
            }

            template<typename R>
            std::shared_ptr<tracer<R,runtime,'*',true>> operator * (const R &p1, const runtime &p2)
            {
                return std::shared_ptr<tracer<R,runtime,'*',true>> (new tracer<R,runtime,'*',true>(p1, p2));
            }

            std::shared_ptr<tracer<runtime,runtime,'*',true>> operator * (const runtime &p1, const runtime &p2)
            {
                return std::shared_ptr<tracer<runtime,runtime,'*',true>> (new tracer<runtime,runtime,'*',true>(p1, p2));
            }

            // divide

            template<typename T, typename S, typename R, char op>
            constexpr tracer<tracer<T,S,op,false>,R,'/',false> operator / (const tracer<T,S,op,false> &p1, const R &p2)
            {
                return tracer<tracer<T,S,op,false>,R,'/',false> (p1, p2);
            }

            template<typename T, typename S, typename R, char op>
            constexpr tracer<R,tracer<T,S,op,false>,'/',false> operator / (const R &p1, const tracer<T,S,op,false> &p2)
            {
                return tracer<R,tracer<T,S,op,false>,'/',false> (p1, p2);
            }

            template<typename T, typename S, typename R, typename Q, char op1, char op2>
            constexpr tracer<tracer<R,Q,op1,false>,tracer<T,S,op2,false>,'/',false> operator / (const tracer<R,Q,op1,false> &p1, const tracer<T,S,op2,false> &p2)
            {
                return tracer<tracer<R,Q,op1,false>,tracer<T,S,op2,false>,'/',false> (p1, p2);
            }

            template<typename R>
            std::shared_ptr<tracer<runtime,R,'/',true>> operator / (const runtime &p1, const R &p2)
            {
                return std::shared_ptr<tracer<runtime,R,'/',true>> (new tracer<runtime,R,'/',true>(p1, p2));
            }

            template<typename R>
            std::shared_ptr<tracer<R,runtime,'/',true>> operator / (const R &p1, const runtime &p2)
            {
                return std::shared_ptr<tracer<R,runtime,'/',true>> (new tracer<R,runtime,'/',true>(p1, p2));
            }

            std::shared_ptr<tracer<runtime,runtime,'/',true>> operator / (const runtime &p1, const runtime &p2)
            {
                return std::shared_ptr<tracer<runtime,runtime,'/',true>> (new tracer<runtime,runtime,'/',true>(p1, p2));
            }

            // combined assignment operators

            runtime operator += (runtime &p1, const runtime &p2)
            {
                return p1 = (p1 + p2);
            }

            runtime operator -= (runtime &p1, const runtime &p2)
            {
                return p1 = (p1 - p2);
            }

            runtime operator *= (runtime &p1, const runtime &p2)
            {
                return p1 = (p1 * p2);
            }

            runtime operator /= (runtime &p1, const runtime &p2)
            {
                return p1 = (p1 / p2);
            }
        };
    };
};

#endif
