#pragma once

#include <cmath>
#include <regex>
#include <sstream>

#include "AstNode.hpp"
#include "RuntimeScope.hpp"
#include "TemporaryValue.hpp"

template <typename T>
concept hasTokenValue = requires(T t)
{
    {t.tokenValue} -> std::convertible_to<LexToken::Any>;
};

TemporaryValue::Any treeWallInterpret(AstNode::Any& in,std::unique_ptr<RuntimeScope>& scope,bool preventNewScopeFromBlock = false)
{
    const int MAX_LOOP_ITERATION = 100000;

    if (in |vx::is<AstNode::Float>)
    {
        return TemporaryValue::Float{(in|vx::as<AstNode::Float>).tokenValue.content};
    }
    else if (in |vx::is<AstNode::Integer>)
    {
        return TemporaryValue::Integer{(in|vx::as<AstNode::Integer>).tokenValue.content};
    }
    else if (in |vx::is<AstNode::Bool>)
    {
        return TemporaryValue::Bool{(in|vx::as<AstNode::Bool>).tokenValue.content == "true"};
    }
    else if (in |vx::is<AstNode::String>)
    {
        return TemporaryValue::String{(in|vx::as<AstNode::String>).tokenValue.content};
    }
    else if (in |vx::is<AstNode::Identifier>)
    {
        //TODO: make it better;
        auto e = scope->getVariable((in|vx::as<AstNode::Identifier>).tokenValue.content);
        if(e)
            return *e;
    }
    else if (in |vx::is<AstNode::UnaryOp>)
    {
        auto& v = in|vx::as<AstNode::UnaryOp>;
        if(v.tokenValue.content == "-")
        {
            auto inner = treeWallInterpret(*v.inner,scope);

            if (inner |vx::is<TemporaryValue::Float>)
            {
                return TemporaryValue::Float{-(inner|vx::as<TemporaryValue::Float>).value };
            }
            else if (inner |vx::is<TemporaryValue::Integer>)
            {
                return TemporaryValue::Integer{-(inner|vx::as<TemporaryValue::Integer>).value };
            }
        }

        if(v.tokenValue.content == "+")
        {
            auto inner = treeWallInterpret(*v.inner,scope);

            if (inner |vx::is<TemporaryValue::Float>)
            {
                return TemporaryValue::Float{(inner|vx::as<TemporaryValue::Float>).value };
            }
            else if (inner |vx::is<TemporaryValue::Integer>)
            {
                return TemporaryValue::Integer{(inner|vx::as<TemporaryValue::Integer>).value };
            }
        }

        if(v.tokenValue.content == "!")
        {
            auto inner = treeWallInterpret(*v.inner,scope);

            if (inner |vx::is<TemporaryValue::Bool>)
            {
                return TemporaryValue::Bool{!(inner|vx::as<TemporaryValue::Bool>).value };
            }
        }
    }

    else if (in |vx::is<AstNode::BinaryOp>)
    {
        auto& v = in|vx::as<AstNode::BinaryOp>;

        auto left = treeWallInterpret(*v.left,scope);

        if(left |vx::is<TemporaryValue::Bool>)
        {
            if(v.tokenValue.content == "&&")
            {
                if(TemporaryValue::getBool(left) == false)
                    return TemporaryValue::Bool{false};

                auto right = treeWallInterpret(*v.right,scope);
                return TemporaryValue::Bool{TemporaryValue::getBool(right)};

            }
            if(v.tokenValue.content == "||")
            {
                if(TemporaryValue::getBool(left) == true)
                    return TemporaryValue::Bool{true};

                auto right = treeWallInterpret(*v.right,scope);
                return TemporaryValue::Bool{TemporaryValue::getBool(right)};
            }

            auto right = treeWallInterpret(*v.right,scope);
            if(v.tokenValue.content == "==")
                return TemporaryValue::Bool{TemporaryValue::getBool(left) == TemporaryValue::getBool(right)};
            if(v.tokenValue.content == "!=")
                return TemporaryValue::Bool{TemporaryValue::getBool(left) != TemporaryValue::getBool(right)};
        }

        auto right = treeWallInterpret(*v.right,scope);

        if(left |vx::is<TemporaryValue::Integer> && right |vx::is<TemporaryValue::Integer>)
        {
            if(v.tokenValue.content == "==")
                return TemporaryValue::Bool{TemporaryValue::getInteger(left) == TemporaryValue::getInteger(right)};
            if(v.tokenValue.content == "!=")
                return TemporaryValue::Bool{TemporaryValue::getInteger(left) != TemporaryValue::getInteger(right)};
            if(v.tokenValue.content == "<")
                return TemporaryValue::Bool{TemporaryValue::getInteger(left) < TemporaryValue::getInteger(right)};
            if(v.tokenValue.content == ">")
                return TemporaryValue::Bool{TemporaryValue::getInteger(left) > TemporaryValue::getInteger(right)};
            if(v.tokenValue.content == "<=")
                return TemporaryValue::Bool{TemporaryValue::getInteger(left) <= TemporaryValue::getInteger(right)};
            if(v.tokenValue.content == ">=")
                return TemporaryValue::Bool{TemporaryValue::getInteger(left) >= TemporaryValue::getInteger(right)};

            if(v.tokenValue.content == "+")
                return TemporaryValue::Integer{TemporaryValue::getInteger(left) + TemporaryValue::getInteger(right)};
            if(v.tokenValue.content == "-")
                return TemporaryValue::Integer{TemporaryValue::getInteger(left) - TemporaryValue::getInteger(right)};
            if(v.tokenValue.content == "*")
                return TemporaryValue::Integer{TemporaryValue::getInteger(left) * TemporaryValue::getInteger(right)};
            if(v.tokenValue.content == "/")
                return TemporaryValue::Integer{TemporaryValue::getInteger(left) / TemporaryValue::getInteger(right)};

            if(v.tokenValue.content == "^")
                return TemporaryValue::Integer{static_cast<int>(
                    std::pow(TemporaryValue::getInteger(left),TemporaryValue::getInteger(right))
                )};
        }

        if((left |vx::is<TemporaryValue::Integer> || left |vx::is<TemporaryValue::Float>) && (right |vx::is<TemporaryValue::Integer> || right |vx::is<TemporaryValue::Float>)) // If any argument is float, promote to float
        {
            if(v.tokenValue.content == "==")
                return TemporaryValue::Bool{TemporaryValue::getFloat(left) == TemporaryValue::getFloat(right)};
            if(v.tokenValue.content == "!=")
                return TemporaryValue::Bool{TemporaryValue::getFloat(left) != TemporaryValue::getFloat(right)};
            if(v.tokenValue.content == "<")
                return TemporaryValue::Bool{TemporaryValue::getFloat(left) < TemporaryValue::getFloat(right)};
            if(v.tokenValue.content == ">")
                return TemporaryValue::Bool{TemporaryValue::getFloat(left) > TemporaryValue::getFloat(right)};
            if(v.tokenValue.content == "<=")
                return TemporaryValue::Bool{TemporaryValue::getFloat(left) <= TemporaryValue::getFloat(right)};
            if(v.tokenValue.content == ">=")
                return TemporaryValue::Bool{TemporaryValue::getFloat(left) >= TemporaryValue::getFloat(right)};


            if(v.tokenValue.content == "+")
                return TemporaryValue::Float{TemporaryValue::getFloat(left) + TemporaryValue::getFloat(right)};
            if(v.tokenValue.content == "-")
                return TemporaryValue::Float{TemporaryValue::getFloat(left) - TemporaryValue::getFloat(right)};
            if(v.tokenValue.content == "*")
                return TemporaryValue::Float{TemporaryValue::getFloat(left) * TemporaryValue::getFloat(right)};
            if(v.tokenValue.content == "/")
                return TemporaryValue::Float{TemporaryValue::getFloat(left) / TemporaryValue::getFloat(right)};

            if(v.tokenValue.content == "^")
                return TemporaryValue::Float{std::pow(TemporaryValue::getFloat(left),TemporaryValue::getFloat(right))};
        }
        if((left |vx::is<TemporaryValue::String> || right |vx::is<TemporaryValue::String>) )
        {
            if(v.tokenValue.content == "==")
                return TemporaryValue::Bool{TemporaryValue::getString(left) == TemporaryValue::getString(right)};
            if(v.tokenValue.content == "!=")
                return TemporaryValue::Bool{TemporaryValue::getString(left) != TemporaryValue::getString(right)};

            if(v.tokenValue.content == "+")
                return TemporaryValue::String{TemporaryValue::getString(left) + TemporaryValue::getString(right)};
        }

        std::cout << "unsupported operation:'" << v.tokenValue.content << "' between left:'" << left << "' and right:'" << right << "'\n";
        std::cout << v.tokenValue.source.printHint()  << "here \n";
        std::cout << "left: " << AstNode::stringify(*v.left) << '\n';
        std::cout << "right: " << AstNode::stringify(*v.right) << '\n';
        throw std::runtime_error("");
    }
    else if (in |vx::is<AstNode::Block>)
    {
        if(!preventNewScopeFromBlock) RuntimeScope::addNew(&scope);
        auto& v = in|vx::as<AstNode::Block>;
        if(v.statements.size() >= 1)
        {
            for(int i = 0; i != v.statements.size()-1; i++)
            {
                treeWallInterpret(*v.statements[i],scope);
            }
            auto ret =  treeWallInterpret(*v.statements[v.statements.size()-1],scope);
            if(!preventNewScopeFromBlock) RuntimeScope::removeLast(&scope);
            return ret;
        }
    }
    else if (in |vx::is<AstNode::PrintStmt>)
    {
        auto& v = in|vx::as<AstNode::PrintStmt>;
        auto inner = treeWallInterpret(*v.inner,scope);
        inner |vx::match {
            [&in](const TemporaryValue::Bool& v)       { std::cout << (v.value ? "true" : "false") ;},
            [&in](const TemporaryValue::Integer& v)    { std::cout <<  v.value;},
            [&in](const TemporaryValue::Float& v)      { std::cout <<  v.value;},
            [&in](const TemporaryValue::String& v)
            {
                std::cout <<  std::regex_replace(v.value, std::regex(R"(\\n)"), "\n");
            }
        };
        return inner;
    }
    else if (in |vx::is<AstNode::IfStmt>)
    {
        auto& v = in|vx::as<AstNode::IfStmt>;
        auto when = treeWallInterpret(*v.when,scope);

        if(TemporaryValue::getBool(when))
        {
            RuntimeScope::addNew(&scope);
            auto ret = treeWallInterpret(*v.then,scope,true);
            RuntimeScope::removeLast(&scope);
            return ret;
        }
        if(v.elseThen)
        {
            RuntimeScope::addNew(&scope);
            auto ret = treeWallInterpret(*v.elseThen,scope,true);
            RuntimeScope::removeLast(&scope);
            return ret;
        }
        return {};
    }

    else if (in |vx::is<AstNode::AssignStmt>)
    {
        auto& v = in|vx::as<AstNode::AssignStmt>;
        auto varName = v.identifier.tokenValue.content;
        auto value = treeWallInterpret(*v.value,scope);

        if(auto var = scope->getVariable(varName))
        {
            if(var->index() == value.index())
            {
                *var = value;
                return value;
            }

            std::cout << "forbitted redefintion variable:'" << varName << "' old value:'" << *var << "' new value:'" << value << "'\n";
            std::cout << v.tokenValue.source.printHint()  << "here \n";
            throw std::runtime_error("");
        }
        scope->variables[varName] = value;

        return value;
    }
    else if (in |vx::is<AstNode::WhileStmt>)
    {
        auto& v = in|vx::as<AstNode::WhileStmt>;
        RuntimeScope::addNew(&scope);

        TemporaryValue::Any ret = TemporaryValue::Bool{false};

        for(int i = 0; i!= MAX_LOOP_ITERATION; i++) //max iteration
        {
            auto until = treeWallInterpret(*v.until,scope);
            if(TemporaryValue::getBool(until))
            {
                ret = treeWallInterpret(*v.loop,scope,true);
                continue;
            }
            break;
        }

        RuntimeScope::removeLast(&scope);
        return ret;
    }
    else if (in |vx::is<AstNode::ForStmt>)
    {
        auto& v = in|vx::as<AstNode::ForStmt>;
        RuntimeScope::addNew(&scope);

        treeWallInterpret(*v.doOnce,scope,true);

        TemporaryValue::Any ret = TemporaryValue::Bool{false};

        for(int i = 0; i!= MAX_LOOP_ITERATION; i++) //max iteration
        {
            auto until = treeWallInterpret(*v.until,scope);
            if(TemporaryValue::getBool(until))
            {
                ret = treeWallInterpret(*v.loop,scope,true);
                treeWallInterpret(*v.afterIter,scope,true);
                continue;
            }
            break;
        }

        RuntimeScope::removeLast(&scope);
        return ret;
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