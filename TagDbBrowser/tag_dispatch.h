#pragma once

namespace sqlite_tag_db
{
    namespace tags
    {
        struct p1 {};
        struct p2 { p2(p1) {}; };
    }
}