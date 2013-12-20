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
        namespace numeric
        {
            template<bool runtime = false>
            class tracerBase
            {
                public:
                    virtual ~tracerBase (void) {}

                    virtual operator std::string (void) const
                    {
                        return "unknown";
                    }

                    virtual operator char (void) const
                    {
                        return 0;
                    }
            };

            template<>
            class tracerBase<false> {};

            template<typename T, typename S = void, char op = 0, bool runtime = false>
            class tracer : public tracerBase<runtime>
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

                    operator char (void) const
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
            class tracer<T,void,op,runtime> : public tracerBase<runtime>
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

                    operator char (void) const
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
            class tracer<void,void,0,true> : public tracerBase<true>
            {
                public:
                    tracer (std::string pName)
                        : name(pName) {}

                    operator std::string (void) const
                    {
                        return name;
                    }

                    operator char (void) const
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
            class tracer<void,void,0,false> : public tracerBase<false>
            {
                public:
                    tracer (const char *pName)
                        : name(pName) {}

                    operator std::string (void) const
                    {
                        return std::string(name);
                    }

                    operator char (void) const
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
            std::ostream & operator << (std::ostream &s, const tracerBase<true> &t)
            {
                return (s << std::string(t));
            }

            template<typename T, typename S, char op>
            std::ostream & operator << (std::ostream &s, const std::shared_ptr<tracer<T,S,op,true>> &t)
            {
                return (s << *t);
            }

            std::ostream & operator << (std::ostream &s, const std::shared_ptr<tracerBase<true>> &t)
            {
                return (s << *t);
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

            template<typename T, typename S, typename R, char op>
            std::shared_ptr<tracer<std::shared_ptr<tracer<T,S,op,true>>,R,'+',true>> operator + (const std::shared_ptr<tracer<T,S,op,true>> &p1, const R &p2)
            {
                return std::shared_ptr<tracer<std::shared_ptr<tracer<T,S,op,true>>,R,'+',true>>
                        (new tracer<std::shared_ptr<tracer<T,S,op,true>>,R,'+',true>(p1, p2));
            }

            template<typename R>
            std::shared_ptr<tracer<std::shared_ptr<tracerBase<true>>,R,'+',true>> operator + (const std::shared_ptr<tracerBase<true>> &p1, const R &p2)
            {
                return std::shared_ptr<tracer<std::shared_ptr<tracerBase<true>>,R,'+',true>>
                        (new tracer<std::shared_ptr<tracerBase<true>>,R,'+',true>(p1, p2));
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
        };
    };
};

#endif
