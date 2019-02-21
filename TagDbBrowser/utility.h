#pragma once

#include <algorithm>
#include <iterator>
#include <locale>
#include <string>
#include <vector>

namespace sqlite_tag_db
{
    namespace
    {
        bool tilde_sort(const std::string& a, const std::string& b)
        {
            if (a == b || a.empty())
                return false;

            if (b.empty())
                return true;

            return a[0] == '~' ?
                (b[0] == '~' ? a < b : true) :
                (b[0] == '~' ? false : a < b);
        };
    }

    template <typename FwdIt>
    std::string join(FwdIt first, FwdIt last, const std::string& delimiter = ", ")
    {
        using value_type = typename std::iterator_traits<FwdIt>::value_type;

        std::ostringstream ss;

        std::copy(first, last, std::ostream_iterator<value_type>(ss, delimiter.c_str()));
        auto rv = ss.str();
        if (!rv.empty())
            rv = rv.substr(0, rv.size() - delimiter.size());

        return rv;
    }

    template <typename Container>
    std::string join(Container c, const std::string& delimiter = ", ")
    {
        return join(std::begin(c), std::end(c), delimiter);
    }

    template <typename StringLike>
    std::string join(std::initializer_list<StringLike> c, const std::string& delimiter = ", ")
    {
        return join(std::begin(c), std::end(c), delimiter);
    }

    std::string tags_to_where(std::string tags)
    {
        if (tags.empty())
            return "";

        using is_it = std::istream_iterator<std::string>;
        std::istringstream iss(tags);
        std::vector<std::string> tokens;
        std::copy(is_it(iss), is_it{}, back_inserter(tokens));
        std::sort(tokens.begin(), tokens.end(), tilde_sort);
        auto first_not_tilde = std::find_if(tokens.begin(), tokens.end(), [](const std::string& t) { return t[0] != '~'; });

        std::ostringstream oss;
        oss << join(tokens.begin(), first_not_tilde) << " | " << join(first_not_tilde, tokens.end());
        return oss.str();
    }
}