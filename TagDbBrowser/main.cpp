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
#include "query_processor.h"
#include "utility.h"

using namespace std::string_literals;
using namespace sqlite_tag_db;

#undef UNICODE

void pause()
{
    std::cout << "Press Enter to continue...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

int main()
{
    std::string db_path = R"(C:\Users\Gary\Documents\QtTagDatabase\tagdb.db)";
    sqlite3* db;
    sqlite3_open(db_path.c_str(), &db);
    if (db)
    {
        auto select = join({ "width", "height", "path", "thumb_path", "tag_name" });
        auto data = run_select(db, select_packet{ "all_tags", select, "", 5 }, std::tuple<int, long, std::string, std::string, std::string>{});
        print_data(data.begin(), data.end());
    }
    sqlite3_close(db);

    pause();
}