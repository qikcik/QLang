#pragma once
#include <string>
#include <variant>

namespace TemporaryValue
{
    template<typename T>
    struct WithContent
    {
        using TContentType = T;
        TContentType value {};
    };

    struct Bool         final       : public WithContent<bool>        {};
    struct Integer      final       : public WithContent<int>         {};
    struct Float        final       : public WithContent<float>       {};
    struct String       final       : public WithContent<std::string> {};

    using Any = std::variant<Bool,Integer,Float,String>;

    float getFloat(Any& in);
    int getInteger(Any& in);
    bool getBool(Any& in);
    std::string getString(Any& in);
}

std::ostream& operator<<(std::ostream& os, const TemporaryValue::Any& in);