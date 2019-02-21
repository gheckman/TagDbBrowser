#pragma once

#include <string>

struct select_packet
{
    select_packet() = default;
    select_packet(
        const std::string& in_table,
        const std::string& in_select,
        const std::string& in_where,
        int in_limit) :
        table(in_table),
        select(in_select),
        where(in_where),
        limit(in_limit)
    {}

    std::string table;
    std::string select;
    std::string where;
    int limit;
};