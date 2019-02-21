#pragma once

#include "sqlite3.h"

#include "tag_dispatch.h"
#include "type_traits_ext.h"
#include "utility.h"
#include "query_packet.h"

using namespace std::string_literals;

namespace sqlite_tag_db
{
    namespace
    {
        struct sqlite3_column_chooser
        {
            /* TODO [4] Figure out some way to have a reasonable default
            template <typename T>
            sqlite3_value* operator()(sqlite3_stmt* ps, int i, T&& args, tags::p2)
            {
                return args = sqlite3_column_value(ps, i);
            }*/

            template <typename T>
            test_t<T, is_86_int> operator()(sqlite3_stmt* ps, int i, T&& args, tags::p1)
            {
                return args = sqlite3_column_int(ps, i);
            }

            template <typename T>
            test_t<T, is_64_int> operator()(sqlite3_stmt* ps, int i, T&& args, tags::p1)
            {
                return args = sqlite3_column_int64(ps, i);
            }

            template <typename T>
            test_t<T, std::is_floating_point> operator()(sqlite3_stmt* ps, int i, T&& args, tags::p1)
            {
                return args = sqlite3_column_double(ps, i);
            }

            template <typename T>
            // TODO [1] Get char* to work, maybe just return a new pointer?
            //test_t<T, is_string>
            std::enable_if_t<std::is_same_v<std::decay_t<T>, std::string>, T>
                operator()(sqlite3_stmt* ps, int i, T&& args, tags::p1)
            {
                return args = reinterpret_cast<const char*>(sqlite3_column_text(ps, i));
            }

            /* TODO [2] Get wide-strings to work
            template <typename T>
            test_t<T, is_wstring> operator()(sqlite3_stmt* ps, int i, T&& args, tags::p1)
            {
                return args = static_cast<const unsigned char16_t*>(sqlite3_column_text16(ps, i));
            }
            */
        };
    }

    template <typename Tuple>
    std::vector<Tuple> populate_data(sqlite3_stmt* ps)
    {
        std::vector<Tuple> rv;

        while (sqlite3_step(ps) == SQLITE_ROW)
        {
            Tuple tuple;
            int i = 0;
            auto l = [ps, &i](auto&&... args) { (..., sqlite3_column_chooser{}(ps, i++, args, tags::p1{})); };
            std::apply(l, tuple);
            rv.push_back(tuple);
        }

        return rv;
    }

    namespace
    {
        template <typename Tuple>
        std::vector<Tuple> run_select_(sqlite3* db, const std::string& select_query, const Tuple&)
        {
            std::vector<Tuple> rv;
            sqlite3_stmt* ps;
            const char* _1;
            sqlite3_prepare_v2(db, select_query.c_str(), select_query.size() + 1, &ps, &_1);
            rv = populate_data<Tuple>(ps);
            sqlite3_finalize(ps);
            return rv;
        }
    }

    template <typename FwdIt>
    void print_data(FwdIt first, FwdIt last)
    {
        auto print_tuple = [](auto tuple)
        {
            auto l = [](auto&&... args) { (..., (std::cout << args << " ")); };
            std::apply(l, tuple);
        };
        std::for_each(first, last, print_tuple);
    }

    template <typename Tuple>
    std::vector<Tuple> run_select(sqlite3* db, const select_packet& packet, const Tuple& arg_tuple)
    {
        std::stringstream ss;
        ss << "SELECT " << packet.select << " FROM " << packet.table;
        if (!packet.where.empty())
            ss << " WHERE " << packet.where;
        if (packet.limit > 0)
            ss << " LIMIT " << packet.limit;
        return run_select_(db, ss.str(), arg_tuple);
    }
}