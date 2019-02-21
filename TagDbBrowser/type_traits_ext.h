#pragma once

#include <string>
#include <type_traits>

namespace sqlite_tag_db
{
    // Int type <= 32 bits
    template<class T>
    struct is_86_int : std::false_type {};

    template<>
    struct is_86_int<bool> : std::true_type {};

    template<>
    struct is_86_int<char> : std::true_type {};

    template<>
    struct is_86_int<wchar_t> : std::true_type {};

    template<>
    struct is_86_int<char16_t> : std::true_type {};

    template<>
    struct is_86_int<char32_t> : std::true_type {};

    template<>
    struct is_86_int<uint8_t> : std::true_type {};

    template<>
    struct is_86_int<int8_t> : std::true_type {};

    template<>
    struct is_86_int<uint16_t> : std::true_type {};

    template<>
    struct is_86_int<int16_t> : std::true_type {};

    template<>
    struct is_86_int<uint32_t> : std::true_type {};

    template<>
    struct is_86_int<int32_t> : std::true_type {};

    template <typename T>
    constexpr bool is_86_int_v = is_86_int<T>::value;

    // Int type > 32 bits
    template<class T>
    struct is_64_int : std::bool_constant<std::is_integral_v<T> && !is_86_int_v<T>> {};

    template <typename T>
    constexpr bool is_64_int_v = is_64_int<T>::value;

    template <typename T>
    struct is_string : public std::bool_constant<std::is_same_v<T, const char*> || std::is_same_v<T, char*> || std::is_same_v<T, std::string>> {};

    template <typename T>
    constexpr bool is_string_v = is_string<T>::value;

    template <typename T>
    struct is_wstring : public std::bool_constant<std::is_same_v<T, const wchar_t*> || std::is_same_v<T, wchar_t*> || std::is_same_v<T, std::wstring>> {};

    template <typename T>
    constexpr bool is_wstring_v = is_wstring<T>::value;

    template <typename T, template<typename> typename Test>
    struct test : public std::enable_if<Test<std::decay_t<T>>::value, T> {};

    template <typename T, template<typename> typename Test>
    using test_t = typename test<T, Test>::type;
}