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
 * \see Project Source Code: http://git.becquerel.org/jyujin/libefgy.git
 */

#if !defined(EF_GY_SQLITE_H)
#define EF_GY_SQLITE_H

#include <sqlite3.h>
#include <string>
#include <iostream>
#include <stdexcept>

namespace efgy
{
    namespace database
    {
        class sqlite
        {
            public:
                sqlite(const std::string &databaseFile)
                    : database(0)
                    {
                        if (sqlite3_open(databaseFile.c_str(), &database) != SQLITE_OK)
                        {
                            throw sqlite::exception(std::string("could not open database ") + databaseFile);
                        }
                    }

                sqlite(const std::string &databaseFile, const char *reference)
                    : database(0)
                    {
                        if (sqlite3_open_v2(databaseFile.c_str(), &database, SQLITE_OPEN_READWRITE, 0) != SQLITE_OK)
                        {
                            std::cerr << "database '" << databaseFile << "' could not be opened: creating with reference data\n";

                            if (sqlite3_open(databaseFile.c_str(), &database) != SQLITE_OK)
                            {
                                throw sqlite::exception(std::string("could not open database ") + databaseFile);
                            }

                            import(reference);
                        }
                    }

                ~sqlite (void)
                    {
                        if (database && (sqlite3_close(database) != SQLITE_OK))
                        {
                            throw sqlite::exception("could not close database", *this);
                        }
                    }

                class exception : public std::runtime_error
                {
                    public:
                        exception(const std::string &pString)
                            : std::runtime_error(pString) {}

                        exception(const std::string &pString, const sqlite &pSQL)
                            : std::runtime_error(pSQL.database ? pString + ": " + sqlite3_errmsg(pSQL.database)
                                                               : pString + ": no valid database context")
                            {}
                };

                class statement
                {
                    public:
                        statement(const std::string &pStatement, sqlite &pSQL)
                            : sql(pSQL), row(false)
                        {
                            if (sqlite3_prepare_v2
                                    (sql.database,
                                     pStatement.c_str(),
                                     -1, &stmt, 0) != SQLITE_OK)
                            {
                                throw sqlite::exception("sqlite3_prepare_v2", sql);
                            }
                        }

                        ~statement(void)
                        {
                            // ignore error codes: these refer to errors encountered in
                            // sqlite3_step, which are thrown separately
                            sqlite3_finalize (stmt);
                        }

                        bool step (void)
                        {
                            int cRetries = 0;
                        retry:
                            switch (sqlite3_step (stmt))
                            {
                                case SQLITE_ROW:
                                    row = true;
                                    return true;
                                case SQLITE_OK:
                                case SQLITE_DONE:
                                    row = false;
                                    return true;
                                case SQLITE_BUSY:
                                    if (cRetries < retries)
                                    {
                                        cRetries++;
                                        sleep(1);
                                        goto retry;
                                    }
                                default:
                                    throw sqlite::exception("sqlite3_step", sql);
                            }

                            return false;
                        }

                        bool reset (void)
                        {
                            if (sqlite3_reset (stmt) != SQLITE_OK)
                            {
                                throw sqlite::exception("sqlite3_reset", sql);
                                return false;
                            }

                            row = false;

                            return true;
                        }

                        bool stepReset (void)
                        {
                            return step() && reset();
                        }

                        template <typename T>
                        bool bind (int i, const T &value)
                        {
                            if (sqlite3_bind_null (stmt, i) != SQLITE_OK)
                            {
                                throw sqlite::exception("sqlite3_bind_int64", sql);
                                return false;
                            }

                            return true;
                        }

                        template <typename T>
                        bool get (int i, T &value)
                        {
                            if (!row)
                            {
                                return false;
                            }

                            return sqlite3_column_type (stmt, i) == SQLITE_NULL;
                        }

                        int getColumnCount (void) const
                        {
                            return sqlite3_column_count(stmt);
                        }

                        operator sqlite3_stmt * (void)
                        {
                            return stmt;
                        }

                        bool row;

                    protected:
                        sqlite &sql;
                        sqlite3_stmt *stmt;
                        static const int retries = 5;
                };

                statement prepare (const std::string &pStatement)
                {
                    return statement (pStatement, *this);
                }

                bool execute (const std::string &pStatement)
                {
                    statement statement(pStatement, *this);
                    return statement.stepReset();
                }

                operator sqlite3 * (void)
                {
                    return database;
                }

                bool import (const char *data)
                {
                    size_t pos = 0;
                    const char *tail = data;

                    do
                    {
                        const char *ntail;
                        sqlite3_stmt *stmt = 0;
                        if (sqlite3_prepare_v2 (database, tail, -1, &stmt, &ntail) == SQLITE_OK)
                        {
                            switch (sqlite3_step(stmt))
                            {
                                case SQLITE_ROW:
                                case SQLITE_OK:
                                case SQLITE_DONE:
                                    break;
                                default:
                                    throw exception("import: sqlite3_step", *this);
                            }
                            sqlite3_finalize(stmt);
                            tail = ntail;
                        }
                        else
                        {
                            throw exception("import: sqlite3_prepare_v2", *this);
                        }
                    } while ((tail != 0) && (*tail != (char)0));

                    return true;
                }

            protected:
                sqlite3 *database;
        };

        template <>
        bool sqlite::statement::bind (int i, const long long &value)
        {
            sqlite3_int64 v = value;

            if (sqlite3_bind_int64 (stmt, i, v) != SQLITE_OK)
            {
                throw sqlite::exception("sqlite3_bind_int64", sql);
                return false;
            }

            return true;
        }

        template <>
        bool sqlite::statement::bind (int i, const int &value)
        {
            if (sqlite3_bind_int (stmt, i, value) != SQLITE_OK)
            {
                throw sqlite::exception("sqlite3_bind_int", sql);
                return false;
            }

            return true;
        }

        template <>
        bool sqlite::statement::bind (int i, const std::string &value)
        {
            if (sqlite3_bind_text (stmt, i, value.data(), value.size(), SQLITE_TRANSIENT) != SQLITE_OK)
            {
                throw sqlite::exception("sqlite3_bind_text", sql);
                return false;
            }

            return true;
        }

        template <>
        bool sqlite::statement::bind (int i, const double &value)
        {
            if (sqlite3_bind_double (stmt, i, value) != SQLITE_OK)
            {
                throw sqlite::exception("sqlite3_bind_double", sql);
                return false;
            }

            return true;
        }

        template <>
        bool sqlite::statement::get (int i, long long &value)
        {
            if (!row)
            {
                return false;
            }

            sqlite3_int64 v = sqlite3_column_int64 (stmt, i);

            value = (long long)v;

            return true;
        }

        template <>
        bool sqlite::statement::get (int i, int &value)
        {
            if (!row)
            {
                return false;
            }

            value = sqlite3_column_int (stmt, i);

            return true;
        }

        template <>
        bool sqlite::statement::get (int i, std::string &value)
        {
            if (!row)
            {
                return false;
            }

            const char *v = (const char*)sqlite3_column_text (stmt, i);

            value = v ? v : "";

            return v != 0;
        }

        template <>
        bool sqlite::statement::get (int i, double &value)
        {
            if (!row)
            {
                return false;
            }

            value = sqlite3_column_double (stmt, i);

            return true;
        }
    };
};

#endif
