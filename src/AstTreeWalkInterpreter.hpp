#pragma once

#include <cmath>
#include <regex>
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
    bool getBool(Any& in);
    std::string getString(Any& in);
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

std::string TemporaryVariable::getString(TemporaryVariable::Any& in)
{
    if(in|vx::is<Integer>)
    {
        std::cout << "WRN: context required conversion to String from " << in << "'\n";
        return std::to_string((in|vx::as<Integer>).value);
    }
    if(in|vx::is<Bool>)
    {
        std::cout << "WRN: context required conversion to String from " << in << "'\n";
        return (in|vx::as<Bool>).value ? "true" : "false";
    }
    if(in|vx::is<Float>)
    {
        std::cout << "WRN: context required conversion to String from " << in << "'\n";
        return std::to_string((in|vx::as<Float>).value);
    }
    if(in|vx::is<String>)
        return (in|vx::as<String>).value;

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

bool TemporaryVariable::getBool(TemporaryVariable::Any& in)
{
    if(in|vx::is<Bool>)
        return (in|vx::as<Bool>).value;

    std::cout << "unsupported conversion to Bool from:'" << in << "'\n";
    throw std::runtime_error("conversion error");
}

template <typename T>
concept hasTokenValue = requires(T t)
{
    {t.tokenValue} -> std::convertible_to<LexToken::Any>;
};

TemporaryVariable::Any treeWallInterpret(AstNode::Any& in)
{
    if (in |vx::is<AstNode::Float>)
    {
        return TemporaryVariable::Float{(in|vx::as<AstNode::Float>).tokenValue.content};
    }
    else if (in |vx::is<AstNode::Integer>)
    {
        return TemporaryVariable::Integer{(in|vx::as<AstNode::Integer>).tokenValue.content};
    }
    else if (in |vx::is<AstNode::Bool>)
    {
        return TemporaryVariable::Bool{(in|vx::as<AstNode::Bool>).tokenValue.content == "true"};
    }
    else if (in |vx::is<AstNode::String>)
    {
        return TemporaryVariable::String{(in|vx::as<AstNode::String>).tokenValue.content};
    }
    else if (in |vx::is<AstNode::UnaryOp>)
    {
        auto& v = in|vx::as<AstNode::UnaryOp>;
        if(v.tokenValue.content == "-")
        {
            auto inner = treeWallInterpret(*v.inner);

            if (inner |vx::is<TemporaryVariable::Float>)
            {
                return TemporaryVariable::Float{-(inner|vx::as<TemporaryVariable::Float>).value };
            }
            else if (inner |vx::is<TemporaryVariable::Integer>)
            {
                return TemporaryVariable::Integer{-(inner|vx::as<TemporaryVariable::Integer>).value };
            }
        }

        if(v.tokenValue.content == "+")
        {
            auto inner = treeWallInterpret(*v.inner);

            if (inner |vx::is<TemporaryVariable::Float>)
            {
                return TemporaryVariable::Float{(inner|vx::as<TemporaryVariable::Float>).value };
            }
            else if (inner |vx::is<TemporaryVariable::Integer>)
            {
                return TemporaryVariable::Integer{(inner|vx::as<TemporaryVariable::Integer>).value };
            }
        }

        if(v.tokenValue.content == "!")
        {
            auto inner = treeWallInterpret(*v.inner);

            if (inner |vx::is<TemporaryVariable::Bool>)
            {
                return TemporaryVariable::Bool{!(inner|vx::as<TemporaryVariable::Bool>).value };
            }
        }
    }

    else if (in |vx::is<AstNode::BinaryOp>)
    {
        auto& v = in|vx::as<AstNode::BinaryOp>;

        auto left = treeWallInterpret(*v.left);

        if(left |vx::is<TemporaryVariable::Bool>)
        {
            if(v.tokenValue.content == "&&")
            {
                if(TemporaryVariable::getBool(left) == false)
                    return TemporaryVariable::Bool{false};

                auto right = treeWallInterpret(*v.right);
                return TemporaryVariable::Bool{TemporaryVariable::getBool(right)};

            }
            if(v.tokenValue.content == "||")
            {
                if(TemporaryVariable::getBool(left) == true)
                    return TemporaryVariable::Bool{true};

                auto right = treeWallInterpret(*v.right);
                return TemporaryVariable::Bool{TemporaryVariable::getBool(right)};
            }

            auto right = treeWallInterpret(*v.right);
            if(v.tokenValue.content == "==")
                return TemporaryVariable::Bool{TemporaryVariable::getBool(left) == TemporaryVariable::getBool(right)};
            if(v.tokenValue.content == "!=")
                return TemporaryVariable::Bool{TemporaryVariable::getBool(left) != TemporaryVariable::getBool(right)};
        }

        auto right = treeWallInterpret(*v.right);

        if(left |vx::is<TemporaryVariable::Integer> && right |vx::is<TemporaryVariable::Integer>)
        {
            if(v.tokenValue.content == "==")
                return TemporaryVariable::Bool{TemporaryVariable::getInteger(left) == TemporaryVariable::getInteger(right)};
            if(v.tokenValue.content == "!=")
                return TemporaryVariable::Bool{TemporaryVariable::getInteger(left) != TemporaryVariable::getInteger(right)};
            if(v.tokenValue.content == "<")
                return TemporaryVariable::Bool{TemporaryVariable::getInteger(left) < TemporaryVariable::getInteger(right)};
            if(v.tokenValue.content == ">")
                return TemporaryVariable::Bool{TemporaryVariable::getInteger(left) > TemporaryVariable::getInteger(right)};
            if(v.tokenValue.content == "<=")
                return TemporaryVariable::Bool{TemporaryVariable::getInteger(left) <= TemporaryVariable::getInteger(right)};
            if(v.tokenValue.content == ">=")
                return TemporaryVariable::Bool{TemporaryVariable::getInteger(left) >= TemporaryVariable::getInteger(right)};

            if(v.tokenValue.content == "+")
                return TemporaryVariable::Integer{TemporaryVariable::getInteger(left) + TemporaryVariable::getInteger(right)};
            if(v.tokenValue.content == "-")
                return TemporaryVariable::Integer{TemporaryVariable::getInteger(left) - TemporaryVariable::getInteger(right)};
            if(v.tokenValue.content == "*")
                return TemporaryVariable::Integer{TemporaryVariable::getInteger(left) * TemporaryVariable::getInteger(right)};
            if(v.tokenValue.content == "/")
                return TemporaryVariable::Integer{TemporaryVariable::getInteger(left) / TemporaryVariable::getInteger(right)};

            if(v.tokenValue.content == "^")
                return TemporaryVariable::Integer{static_cast<int>(
                    std::pow(TemporaryVariable::getInteger(left),TemporaryVariable::getInteger(right))
                )};
        }

        if((left |vx::is<TemporaryVariable::Integer> || left |vx::is<TemporaryVariable::Float>) && (right |vx::is<TemporaryVariable::Integer> || right |vx::is<TemporaryVariable::Float>)) // If any argument is float, promote to float
        {
            if(v.tokenValue.content == "==")
                return TemporaryVariable::Bool{TemporaryVariable::getFloat(left) == TemporaryVariable::getFloat(right)};
            if(v.tokenValue.content == "!=")
                return TemporaryVariable::Bool{TemporaryVariable::getFloat(left) != TemporaryVariable::getFloat(right)};
            if(v.tokenValue.content == "<")
                return TemporaryVariable::Bool{TemporaryVariable::getFloat(left) < TemporaryVariable::getFloat(right)};
            if(v.tokenValue.content == ">")
                return TemporaryVariable::Bool{TemporaryVariable::getFloat(left) > TemporaryVariable::getFloat(right)};
            if(v.tokenValue.content == "<=")
                return TemporaryVariable::Bool{TemporaryVariable::getFloat(left) <= TemporaryVariable::getFloat(right)};
            if(v.tokenValue.content == ">=")
                return TemporaryVariable::Bool{TemporaryVariable::getFloat(left) >= TemporaryVariable::getFloat(right)};


            if(v.tokenValue.content == "+")
                return TemporaryVariable::Float{TemporaryVariable::getFloat(left) + TemporaryVariable::getFloat(right)};
            if(v.tokenValue.content == "-")
                return TemporaryVariable::Float{TemporaryVariable::getFloat(left) - TemporaryVariable::getFloat(right)};
            if(v.tokenValue.content == "*")
                return TemporaryVariable::Float{TemporaryVariable::getFloat(left) * TemporaryVariable::getFloat(right)};
            if(v.tokenValue.content == "/")
                return TemporaryVariable::Float{TemporaryVariable::getFloat(left) / TemporaryVariable::getFloat(right)};

            if(v.tokenValue.content == "^")
                return TemporaryVariable::Float{std::pow(TemporaryVariable::getFloat(left),TemporaryVariable::getFloat(right))};
        }
        if((left |vx::is<TemporaryVariable::String> || right |vx::is<TemporaryVariable::String>) )
        {
            if(v.tokenValue.content == "==")
                return TemporaryVariable::Bool{TemporaryVariable::getString(left) == TemporaryVariable::getString(right)};
            if(v.tokenValue.content == "!=")
                return TemporaryVariable::Bool{TemporaryVariable::getString(left) != TemporaryVariable::getString(right)};

            if(v.tokenValue.content == "+")
                return TemporaryVariable::String{TemporaryVariable::getString(left) + TemporaryVariable::getString(right)};
        }

        std::cout << "unsupported operation:'" << v.tokenValue.content << "' between left:'" << left << "' and right:'" << right << "'\n";
        std::cout << v.tokenValue.source.printHint()  << "here \n";
        std::cout << "left: " << AstNode::stringify(*v.left) << '\n';
        std::cout << "right: " << AstNode::stringify(*v.right) << '\n';
        throw std::runtime_error("");
    }
    else if (in |vx::is<AstNode::Block>)
    {
        auto& v = in|vx::as<AstNode::Block>;
        if(v.statements.size() >= 1)
        {
            for(int i = 0; i != v.statements.size()-1; i++)
            {
                treeWallInterpret(*v.statements[i]);
            }
            return treeWallInterpret(*v.statements[v.statements.size()-1]);
        }
    }
    else if (in |vx::is<AstNode::PrintStmt>)
    {
        auto& v = in|vx::as<AstNode::PrintStmt>;
        auto inner = treeWallInterpret(*v.inner);
        inner |vx::match {
            [&in](const TemporaryVariable::Bool& v)       { std::cout << (v.value ? "true" : "false") ;},
            [&in](const TemporaryVariable::Integer& v)    { std::cout <<  v.value;},
            [&in](const TemporaryVariable::Float& v)      { std::cout <<  v.value;},
            [&in](const TemporaryVariable::String& v)
            {
                std::cout <<  std::regex_replace(v.value, std::regex(R"(\\n)"), "\n");
            }
        };
        return inner;
    }
    else if (in |vx::is<AstNode::IfStmt>)
    {
        auto& v = in|vx::as<AstNode::IfStmt>;
        auto when = treeWallInterpret(*v.when);

        if(TemporaryVariable::getBool(when))
        {
            return treeWallInterpret(*v.then);
        }
        if(v.elseThen)
        {
            return treeWallInterpret(*v.elseThen);
        }
        return {};
    }


    std::cout << "unable to execute '" << in << "'\n";
    std::visit([](auto& v)
    {
        if constexpr (hasTokenValue<decltype(v)>)
        {
            std::cout << v.tokenValue.source.printHint()  << "here \n";
        }
        else
        {
            std::cout << "couldn't find tokenValue \n";
        }
    },in);
    throw std::runtime_error("interpreting error");
}