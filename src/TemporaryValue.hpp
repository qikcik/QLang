#pragma once
#include <memory>
#include <string>
#include <variant>

#include "AstNode.hpp"

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

    struct Func         final       : public WithContent<AstNode::OwnedNode>
    {
        Func(AstNode::OwnedNode node)
        {
            value = std::move(node);
        }
        Func(const Func& o)
        {
            value = o.value->copy();
        }

        void operator=(const Func& o)
        {
            value = o.value->copy();
        }

    };

    using Any = std::variant<Bool,Integer,Float,String,Func>;

    float getFloat(Any& in);
    int getInteger(Any& in);
    bool getBool(Any& in);
    std::string getString(Any& in);
}

std::ostream& operator<<(std::ostream& os, const TemporaryValue::Any& in);