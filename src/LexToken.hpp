#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <variant>

#include "CodeSource.hpp"

namespace  LexToken
{
    struct Source
    {
        std::shared_ptr<CodeSource> fromSource {};
        size_t atLine {};
        size_t startingCharacter {};

        std::string stringify() const;
        std::string printHint() const;
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

    std::string printHint(const Any& in);
};

std::ostream& operator<<(std::ostream& os, const LexToken::Source& in);
std::ostream& operator<<(std::ostream& os, const LexToken::Any& in);
std::ostream& operator<<(std::ostream& os, const std::optional<LexToken::Any>& in);
