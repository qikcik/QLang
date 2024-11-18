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
    struct WithContent
    {
        using TContentType = T;

        Source source;
        TContentType content {};
    };

    struct Separator    final       : public WithContent<std::string> {};
    struct Label        final       : public WithContent<std::string> {};
    struct String       final       : public WithContent<std::string> {};
    struct Integer      final       : public WithContent<int>         {};
    struct Float        final       : public WithContent<float>       {};

    using Any = std::variant<Separator,Label,String,Integer,Float>;
};

std::ostream& operator<<(std::ostream& os, const LexToken::Source& in);
std::ostream& operator<<(std::ostream& os, const LexToken::Any& in);
std::ostream& operator<<(std::ostream& os, const std::optional<LexToken::Any>& in);
