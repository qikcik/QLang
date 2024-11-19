#pragma once

#include <cmath>
#include <sstream>

#include "AstNode.hpp"

namespace TemporaryVariable
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
}

std::ostream& operator<<(std::ostream& os, const TemporaryVariable::Any& in)
{
    in |vx::match {
        [&os,&in](const TemporaryVariable::Bool& v)       { os << "TemporaryVariable::Bool{" << (v.value ? "true" : "false") << "}";},
        [&os,&in](const TemporaryVariable::Integer& v)    { os << "TemporaryVariable::Integer{" << v.value << "}";},
        [&os,&in](const TemporaryVariable::Float& v)      { os << "TemporaryVariable::Float{" << v.value << "}";},
        [&os,&in](const TemporaryVariable::String& v)     { os << "TemporaryVariable::String{" << v.value << "}";}
    };
    return os;
}

float TemporaryVariable::getFloat(TemporaryVariable::Any& in)
{
    if(in|vx::is<Integer>)
    {
        std::cout << "WRN: context required conversion to Float from " << in << "'\n";
        return static_cast<float>((in|vx::as<Integer>).value);
    }
    if(in|vx::is<Float>)
        return (in|vx::as<Float>).value;

    std::cout << "unsupported conversion to Float from:'" << in << "'\n";
    throw std::runtime_error("conversion error");
}

int TemporaryVariable::getInteger(TemporaryVariable::Any& in)
{
    if(in|vx::is<Integer>)
        return static_cast<float>((in|vx::as<Integer>).value);
    if(in|vx::is<Float>)
    {
        std::cout << "WRN: context required conversion to Integer from " << in << "'\n";
        return static_cast<int>((in|vx::as<Float>).value);
    }

    std::cout << "unsupported conversion to Float from:'" << in << "'\n";
    throw std::runtime_error("conversion error");
}


TemporaryVariable::Any treeWallInterpret(AstNode* in)
{
    if (auto* v = dynamic_cast<AstFloat*>(in))
    {
        return TemporaryVariable::Float{v->tokenValue.content};
    }
    else if (auto* v = dynamic_cast<AstInteger*>(in))
    {
        return TemporaryVariable::Integer{v->tokenValue.content};
    }
    else if (auto* v = dynamic_cast<AstBool*>(in))
    {
        return TemporaryVariable::Bool{v->tokenValue.content == "true"};
    }
    else if (auto* v = dynamic_cast<AstUnaryOp*>(in))
    {
        if(v->operation.content == "-")
        {
            auto inner = treeWallInterpret(v->inner.get());

            if (inner |vx::is<TemporaryVariable::Float>)
            {
                return TemporaryVariable::Float{-(inner|vx::as<TemporaryVariable::Float>).value };
            }
            else if (inner |vx::is<TemporaryVariable::Integer>)
            {
                return TemporaryVariable::Integer{-(inner|vx::as<TemporaryVariable::Integer>).value };
            }
        }

        if(v->operation.content == "+")
        {
            auto inner = treeWallInterpret(v->inner.get());

            if (inner |vx::is<TemporaryVariable::Float>)
            {
                return TemporaryVariable::Float{(inner|vx::as<TemporaryVariable::Float>).value };
            }
            else if (inner |vx::is<TemporaryVariable::Integer>)
            {
                return TemporaryVariable::Integer{(inner|vx::as<TemporaryVariable::Integer>).value };
            }
        }

        if(v->operation.content == "!")
        {
            auto inner = treeWallInterpret(v->inner.get());

            if (inner |vx::is<TemporaryVariable::Bool>)
            {
                return TemporaryVariable::Bool{!(inner|vx::as<TemporaryVariable::Bool>).value };
            }
        }
    }

    else if (auto* v = dynamic_cast<AstBinaryOp*>(in))
    {
        auto left = treeWallInterpret(v->left.get());
        auto right = treeWallInterpret(v->right.get());


        if(left |vx::is<TemporaryVariable::Integer> && right |vx::is<TemporaryVariable::Integer>)
        {
            if(v->operation.content == "==")
                return TemporaryVariable::Bool{TemporaryVariable::getInteger(left) == TemporaryVariable::getInteger(right)};
            if(v->operation.content == "!=")
                return TemporaryVariable::Bool{TemporaryVariable::getInteger(left) != TemporaryVariable::getInteger(right)};
            if(v->operation.content == "<")
                return TemporaryVariable::Bool{TemporaryVariable::getInteger(left) < TemporaryVariable::getInteger(right)};
            if(v->operation.content == ">")
                return TemporaryVariable::Bool{TemporaryVariable::getInteger(left) > TemporaryVariable::getInteger(right)};
            if(v->operation.content == "<=")
                return TemporaryVariable::Bool{TemporaryVariable::getInteger(left) <= TemporaryVariable::getInteger(right)};
            if(v->operation.content == ">=")
                return TemporaryVariable::Bool{TemporaryVariable::getInteger(left) >= TemporaryVariable::getInteger(right)};

            if(v->operation.content == "+")
                return TemporaryVariable::Integer{TemporaryVariable::getInteger(left) + TemporaryVariable::getInteger(right)};
            if(v->operation.content == "-")
                return TemporaryVariable::Integer{TemporaryVariable::getInteger(left) - TemporaryVariable::getInteger(right)};
            if(v->operation.content == "*")
                return TemporaryVariable::Integer{TemporaryVariable::getInteger(left) * TemporaryVariable::getInteger(right)};
            if(v->operation.content == "/")
                return TemporaryVariable::Integer{TemporaryVariable::getInteger(left) / TemporaryVariable::getInteger(right)};

            if(v->operation.content == "^")
                return TemporaryVariable::Integer{static_cast<int>(
                    std::pow(TemporaryVariable::getInteger(left),TemporaryVariable::getInteger(right))
                )};
        }

        if((left |vx::is<TemporaryVariable::Integer> || left |vx::is<TemporaryVariable::Float>) && (right |vx::is<TemporaryVariable::Integer> || right |vx::is<TemporaryVariable::Float>)) // If any argument is float, promote to float
        {
            if(v->operation.content == "==")
                return TemporaryVariable::Bool{TemporaryVariable::getFloat(left) == TemporaryVariable::getFloat(right)};
            if(v->operation.content == "!=")
                return TemporaryVariable::Bool{TemporaryVariable::getFloat(left) != TemporaryVariable::getFloat(right)};
            if(v->operation.content == "<")
                return TemporaryVariable::Bool{TemporaryVariable::getFloat(left) < TemporaryVariable::getFloat(right)};
            if(v->operation.content == ">")
                return TemporaryVariable::Bool{TemporaryVariable::getFloat(left) > TemporaryVariable::getFloat(right)};
            if(v->operation.content == "<=")
                return TemporaryVariable::Bool{TemporaryVariable::getFloat(left) <= TemporaryVariable::getFloat(right)};
            if(v->operation.content == ">=")
                return TemporaryVariable::Bool{TemporaryVariable::getFloat(left) >= TemporaryVariable::getFloat(right)};


            if(v->operation.content == "+")
                return TemporaryVariable::Float{TemporaryVariable::getFloat(left) + TemporaryVariable::getFloat(right)};
            if(v->operation.content == "-")
                return TemporaryVariable::Float{TemporaryVariable::getFloat(left) - TemporaryVariable::getFloat(right)};
            if(v->operation.content == "*")
                return TemporaryVariable::Float{TemporaryVariable::getFloat(left) * TemporaryVariable::getFloat(right)};
            if(v->operation.content == "/")
                return TemporaryVariable::Float{TemporaryVariable::getFloat(left) / TemporaryVariable::getFloat(right)};

            if(v->operation.content == "^")
                return TemporaryVariable::Float{std::pow(TemporaryVariable::getFloat(left),TemporaryVariable::getFloat(right))};
        }

        std::cout << "unsupported operation:'" << v->operation.content << "' between left:'" << left << "' and right:'" << right << "'\n";
        throw std::runtime_error("interpreting error");
    }

    std::cout << "unhandled error at AST node:'" << in->stringify() << "'\n";
    throw std::runtime_error("interpreting error");
}