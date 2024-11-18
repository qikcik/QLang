#pragma once

#include <optional>
#include <string>
#include <vector>
#include <variant>

namespace  LexToken
{
    struct Source
    {
        std::string fromSource {};
        size_t atLine {};
        size_t startingCharacter {};
    };

    template<typename T>
    struct WithValueType
    {
        using ValueType = T;

        Source source;
        ValueType value {};
    };

    struct Separator    final       : public WithValueType<std::string> {};
    struct Label        final       : public WithValueType<std::string> {};
    struct String       final       : public WithValueType<std::string> {};
    struct Integer      final       : public WithValueType<int>         {};
    struct Float        final       : public WithValueType<float>       {};

    using Any = std::variant<Separator,Label,String,Integer,Float>;
};

std::ostream& operator<<(std::ostream& os, const LexToken::Source& in);
std::ostream& operator<<(std::ostream& os, const LexToken::Any& in);
std::ostream& operator<<(std::ostream& os, const std::optional<LexToken::Any>& in);
