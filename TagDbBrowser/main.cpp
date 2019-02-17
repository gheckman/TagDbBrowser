#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <sstream>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "sqlite3.h"

#include "type_traits_ext.h"
#include "tag_dispatch.h"

using namespace std::string_literals;
using namespace sqlite_tag_db;

#undef UNICODE

void pause()
{
    std::cout << "Press Enter to continue...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

struct sqlite3_column_chooser
{
    template <typename T>
    const unsigned char* operator()(sqlite3_stmt* ps, int i, T&& args, tags::p2)
    {
        return args = reinterpret_cast<const char*>(sqlite3_column_text(ps, i));
    }

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
    test_t<T, is_string> operator()(sqlite3_stmt* ps, int i, T&& args, tags::p1)
    {
        return args = reinterpret_cast<const char*>(sqlite3_column_text(ps, i));
    }

    template <typename T>
    test_t<T, is_wstring> operator()(sqlite3_stmt* ps, int i, T&& args, tags::p1)
    {
        return args = static_cast<const unsigned char16_t*>(sqlite3_column_text16(ps, i));
    }
};

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

template <typename Tuple>
std::vector<Tuple> run_select_(sqlite3* db, const std::string& select_query, const Tuple&)
{
    sqlite3_stmt* ps;
    const char* _1;
    sqlite3_prepare_v2(db, select_query.c_str(), select_query.size() + 1, &ps, &_1);
    return populate_data<Tuple>(ps);
}

template <typename FwdIt, typename Tuple>
std::vector<Tuple> run_select(sqlite3* db, FwdIt first, FwdIt last, const std::string& table, const Tuple& arg_tuple)
{
    std::stringstream ss;
    ss << "SELECT ";
    auto select_list = std::copy(first, last, std::ostream_iterator<std::string>(ss, ", "));
    ss.seekp(-2, std::ios_base::end);
    ss << " FROM " << table;
    return run_select_(db, ss.str(), arg_tuple);
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

int main()
{
    std::string db_path = R"(C:\Users\Gary\Documents\QtTagDatabase\tagdb.db)";
    sqlite3* db;
    sqlite3_open(db_path.c_str(), &db);
    if (db)
    {
        auto select_list = { "width", "height", "path", "thumb_path", "tag_name" };
        auto data = run_select(db, select_list.begin(), select_list.end(), "all_tags", std::tuple<int, long, const char*, std::string, std::string>{});
        print_data(data.begin(), data.end());
    }
    sqlite3_close(db);

    pause();
}