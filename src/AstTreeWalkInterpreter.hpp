#pragma once

#include <cmath>
#include <regex>
#include <sstream>

#include "AstNode.hpp"
#include "RuntimeScope.hpp"
#include "TemporaryValue.hpp"

const int MAX_LOOP_ITERATION = 1000;

template <typename T>
concept hasTokenValue = requires(T t)
{
    {t.tokenValue} -> std::convertible_to<LexToken::Any>;
};

struct FuncReturn
{
    TemporaryValue::Any result;
};

TemporaryValue::Any treeWallInterpret(const AstNode::OwnedNode& in,RuntimeScope& globalScope,RuntimeScope& localScope,bool preventNewScopeFromBlock = false);
struct InterpreterVisitor : public AstNode::IVisitor
{

    TemporaryValue::Any& result;
    RuntimeScope& globalScope;
    RuntimeScope& localScope;
    bool preventNewScopeFromBlock;


    InterpreterVisitor(TemporaryValue::Any& result, RuntimeScope& global_scope, RuntimeScope& local_scope,
        bool prevent_new_scope_from_block)
        : result(result),
          globalScope(global_scope),
          localScope(local_scope),
          preventNewScopeFromBlock(prevent_new_scope_from_block)
    {
    }

    void operator()(const AstNode::Identifier& v) override
    {
        auto e = localScope.getVariable(v.tokenValue.content);
        if(e) result = TemporaryValue::Any{*e};
    }

    void operator()(const AstNode::Integer& v) override
    {
        result = TemporaryValue::Integer{v.tokenValue.content};
    }
    void operator()(const AstNode::Float& v) override
    {
        result = TemporaryValue::Float{v.tokenValue.content};
    }
    void operator()(const AstNode::String& v) override
    {
        result = TemporaryValue::String{v.tokenValue.content};
    }

    void operator()(const AstNode::Bool& v) override
    {
        result = TemporaryValue::Bool{v.tokenValue.content == "true"};
    }
    void operator()(const AstNode::UnaryOp& v) override
    {
        if(v.tokenValue.content == "-")
        {
            auto inner = treeWallInterpret(v.inner,globalScope,localScope);

            if (inner |vx::is<TemporaryValue::Float>)
            {
                result = TemporaryValue::Float{-(inner|vx::as<TemporaryValue::Float>).value };
            }
            else if (inner |vx::is<TemporaryValue::Integer>)
            {
                result = TemporaryValue::Integer{-(inner|vx::as<TemporaryValue::Integer>).value };
            }
        }

        if(v.tokenValue.content == "+")
        {
            auto inner = treeWallInterpret(v.inner,globalScope,localScope);

            if (inner |vx::is<TemporaryValue::Float>)
            {
                result = TemporaryValue::Float{(inner|vx::as<TemporaryValue::Float>).value };
            }
            else if (inner |vx::is<TemporaryValue::Integer>)
            {
                result = TemporaryValue::Integer{(inner|vx::as<TemporaryValue::Integer>).value };
            }
        }

        if(v.tokenValue.content == "!")
        {
            auto inner = treeWallInterpret(v.inner,globalScope,localScope);

            if (inner |vx::is<TemporaryValue::Bool>)
            {
                result = TemporaryValue::Bool{!(inner|vx::as<TemporaryValue::Bool>).value };
            }
        }
    }
    void operator()(const AstNode::BinaryOp& v) override
    {
       auto left = treeWallInterpret(v.left,globalScope,localScope);

        if(left |vx::is<TemporaryValue::Bool>)
        {
            if(v.tokenValue.content == "&&")
            {
                if(TemporaryValue::getBool(left) == false)
                {
                    result = TemporaryValue::Bool{false};
                    return;
                }

                auto right = treeWallInterpret(v.right,globalScope,localScope);
                result = TemporaryValue::Bool{TemporaryValue::getBool(right)};
                return;
            }
            if(v.tokenValue.content == "||")
            {
                if(TemporaryValue::getBool(left) == true)
                {
                    result = TemporaryValue::Bool{true};
                    return;
                }

                auto right = treeWallInterpret(v.right,globalScope,localScope);
                result = TemporaryValue::Bool{TemporaryValue::getBool(right)};
                return;
            }

            auto right = treeWallInterpret(v.right,globalScope,localScope);
            if(v.tokenValue.content == "==")
                result = TemporaryValue::Bool{TemporaryValue::getBool(left) == TemporaryValue::getBool(right)};
            if(v.tokenValue.content == "!=")
                result = TemporaryValue::Bool{TemporaryValue::getBool(left) != TemporaryValue::getBool(right)};
            return;
        }

        auto right = treeWallInterpret(v.right,globalScope,localScope);

        if(left |vx::is<TemporaryValue::Integer> && right |vx::is<TemporaryValue::Integer>)
        {
            if(v.tokenValue.content == "==")
                {result = TemporaryValue::Bool{TemporaryValue::getInteger(left) == TemporaryValue::getInteger(right)};return;}
            if(v.tokenValue.content == "!=")
                {result = TemporaryValue::Bool{TemporaryValue::getInteger(left) != TemporaryValue::getInteger(right)};return;}
            if(v.tokenValue.content == "<")
                {result = TemporaryValue::Bool{TemporaryValue::getInteger(left) < TemporaryValue::getInteger(right)};return;}
            if(v.tokenValue.content == ">")
                {result = TemporaryValue::Bool{TemporaryValue::getInteger(left) > TemporaryValue::getInteger(right)};return;}
            if(v.tokenValue.content == "<=")
                {result = TemporaryValue::Bool{TemporaryValue::getInteger(left) <= TemporaryValue::getInteger(right)};return;}
            if(v.tokenValue.content == ">=")
                {result = TemporaryValue::Bool{TemporaryValue::getInteger(left) >= TemporaryValue::getInteger(right)};return;}

            if(v.tokenValue.content == "+")
                {result = TemporaryValue::Integer{TemporaryValue::getInteger(left) + TemporaryValue::getInteger(right)};return;}
            if(v.tokenValue.content == "-")
                {result = TemporaryValue::Integer{TemporaryValue::getInteger(left) - TemporaryValue::getInteger(right)};return;}
            if(v.tokenValue.content == "*")
                {result = TemporaryValue::Integer{TemporaryValue::getInteger(left) * TemporaryValue::getInteger(right)};return;}
            if(v.tokenValue.content == "/")
                {result = TemporaryValue::Integer{TemporaryValue::getInteger(left) / TemporaryValue::getInteger(right)};return;}
            if(v.tokenValue.content == "%")
                {result = TemporaryValue::Integer{TemporaryValue::getInteger(left) % TemporaryValue::getInteger(right)};return;}

            if(v.tokenValue.content == "^")
                {result = TemporaryValue::Integer{static_cast<int>(
                    std::pow(TemporaryValue::getInteger(left),TemporaryValue::getInteger(right))
                )};return;}
        }

        if((left |vx::is<TemporaryValue::Integer> || left |vx::is<TemporaryValue::Float>) && (right |vx::is<TemporaryValue::Integer> || right |vx::is<TemporaryValue::Float>)) // If any argument is float, promote to float
        {
            if(v.tokenValue.content == "==")
                {result = TemporaryValue::Bool{TemporaryValue::getFloat(left) == TemporaryValue::getFloat(right)};return;}
            if(v.tokenValue.content == "!=")
                {result = TemporaryValue::Bool{TemporaryValue::getFloat(left) != TemporaryValue::getFloat(right)};return;}
            if(v.tokenValue.content == "<")
                {result = TemporaryValue::Bool{TemporaryValue::getFloat(left) < TemporaryValue::getFloat(right)};return;}
            if(v.tokenValue.content == ">")
                {result = TemporaryValue::Bool{TemporaryValue::getFloat(left) > TemporaryValue::getFloat(right)};return;}
            if(v.tokenValue.content == "<=")
                {result = TemporaryValue::Bool{TemporaryValue::getFloat(left) <= TemporaryValue::getFloat(right)};return;}
            if(v.tokenValue.content == ">=")
                {result = TemporaryValue::Bool{TemporaryValue::getFloat(left) >= TemporaryValue::getFloat(right)};return;}


            if(v.tokenValue.content == "+")
                {result = TemporaryValue::Float{TemporaryValue::getFloat(left) + TemporaryValue::getFloat(right)};return;}
            if(v.tokenValue.content == "-")
                {result = TemporaryValue::Float{TemporaryValue::getFloat(left) - TemporaryValue::getFloat(right)};return;}
            if(v.tokenValue.content == "*")
                {result = TemporaryValue::Float{TemporaryValue::getFloat(left) * TemporaryValue::getFloat(right)};return;}
            if(v.tokenValue.content == "/")
                {result = TemporaryValue::Float{TemporaryValue::getFloat(left) / TemporaryValue::getFloat(right)};return;}

            if(v.tokenValue.content == "^")
                {result = TemporaryValue::Float{std::pow(TemporaryValue::getFloat(left),TemporaryValue::getFloat(right))};return;}
        }
        if((left |vx::is<TemporaryValue::String> || right |vx::is<TemporaryValue::String>) )
        {
            if(v.tokenValue.content == "==")
                {result = TemporaryValue::Bool{TemporaryValue::getString(left) == TemporaryValue::getString(right)};return;}
            if(v.tokenValue.content == "!=")
                {result = TemporaryValue::Bool{TemporaryValue::getString(left) != TemporaryValue::getString(right)};return;}

            if(v.tokenValue.content == "+")
                {result = TemporaryValue::String{TemporaryValue::getString(left) + TemporaryValue::getString(right)};return;}
        }

        std::cout << "unsupported operation:'" << v.tokenValue.content << "' between left:'" << left << "' and right:'" << right << "'\n";
        std::cout << v.tokenValue.source.printHint()  << "here \n";
        std::cout << "left: " << AstNode::stringify(*v.left) << '\n';
        std::cout << "right: " << AstNode::stringify(*v.right) << '\n';
        throw std::runtime_error("");

    }
    void operator()(const AstNode::Block& v) override
    {
        if(preventNewScopeFromBlock)
        {
            if(v.statements.size() >= 1)
            {
                for(int i = 0; i != v.statements.size()-1; i++)
                {
                    treeWallInterpret(v.statements[i],globalScope,localScope);
                }
                result =  treeWallInterpret(v.statements[v.statements.size()-1],globalScope,localScope);
                return;
            }
        }
        else
        {
            auto blockScope = RuntimeScope(localScope);
            if(v.statements.size() >= 1)
            {
                for(int i = 0; i != v.statements.size()-1; i++)
                {
                    treeWallInterpret(v.statements[i],globalScope,blockScope);
                }
                result = treeWallInterpret(v.statements[v.statements.size()-1],globalScope,blockScope);
                return;
            }
        }
    }
    void operator()(const AstNode::PrintStmt& v) override
    {
        result = treeWallInterpret(v.inner,globalScope,localScope);
        result |vx::match {
            [](const TemporaryValue::Bool& v)       { std::cout << (v.value ? "true" : "false") ;},
            [](const TemporaryValue::Integer& v)    { std::cout <<  v.value;},
            [](const TemporaryValue::Float& v)      { std::cout <<  v.value;},
            [](const TemporaryValue::String& v)
            {
                std::cout <<  std::regex_replace(v.value, std::regex(R"(\\n)"), "\n");
            },
            [](const TemporaryValue::Func& v)
            {
                std::cout << "<func>";
            }
        };
        return;
    }
    void operator()(const AstNode::IfStmt& v) override
    {
        auto when = treeWallInterpret(v.when,globalScope,localScope);

        if(TemporaryValue::getBool(when))
        {
            auto blockScope = RuntimeScope(localScope);
            result = treeWallInterpret(v.then,globalScope,blockScope,true);
            return;
        }
        if(v.elseThen)
        {
            auto blockScope = std::make_unique<RuntimeScope>(localScope);
            result = treeWallInterpret(v.elseThen,globalScope,localScope,true);
            return;
        }
    }
    void operator()(const AstNode::AssignStmt& v) override
    {
        auto asId = dynamic_cast<AstNode::Identifier*>(v.identifier.get());
        if(!asId)
        {
            std::cout << v.tokenValue.source.printHint()  << "here \n";
            throw std::runtime_error("");
        }
        auto varName = asId->tokenValue.content;
        auto value = treeWallInterpret(v.value,globalScope,localScope);

        if(auto var = localScope.getVariable(varName))
        {
            if(var->index() == value.index())
            {
                *var = std::move(value);
                result = TemporaryValue::Any{*var};
                return;
            }

            std::cout << "forbitted redefintion variable:'" << varName << "' old value:'" << *var << "' new value:'" << value << "'\n";
            std::cout << v.tokenValue.source.printHint()  << "here \n";
            throw std::runtime_error("");
        }
        localScope.variables[varName] = std::move(value);

        result = TemporaryValue::Any{localScope.variables[varName]};
    }
    void operator()(const AstNode::WhileStmt& v) override
    {
        auto blockScope = RuntimeScope(localScope);

        result = TemporaryValue::Bool{false};

        for(int i = 0; i!= MAX_LOOP_ITERATION; i++) //max iteration
        {
            auto until = treeWallInterpret(v.until,globalScope,blockScope);
            if(TemporaryValue::getBool(until))
            {
                result = treeWallInterpret(v.loop,globalScope,blockScope,true);
                continue;
            }
            break;
        }
    }
    void operator()(const AstNode::ForStmt& v) override
    {
        auto blockScope = RuntimeScope(localScope);

        treeWallInterpret(v.doOnce,globalScope,blockScope,true);

        result = TemporaryValue::Bool{false};

        for(int i = 0; i!= MAX_LOOP_ITERATION; i++) //max iteration
        {
            auto until = treeWallInterpret(v.until,globalScope,blockScope);
            if(TemporaryValue::getBool(until))
            {
                result = treeWallInterpret(v.loop,globalScope,blockScope,true);
                treeWallInterpret(v.afterIter,globalScope,blockScope,true);
                continue;
            }
            break;
        }

    }
    void operator()(const AstNode::FunctionDecl& v) override
    {
        result = TemporaryValue::Func{v.copy()};
    }
    void operator()(const AstNode::FunctionCall& v) override
    {
        /*
        auto asId = dynamic_cast<AstNode::Identifier*>(v.name.get());
        if(!asId)
        {
            std::cout << v.tokenValue.source.printHint()  << "here \n";
            throw std::runtime_error("");
        }
        if(!localScope.getVariable(asId->tokenValue.content))
        {
            std::cout << "Undefined function\n";
            std::cout << v.tokenValue.source.printHint()  << "here \n";
            throw std::runtime_error("");
        }

        if(!((*localScope.getVariable(v.name.tokenValue.content))|vx::is<TemporaryValue::Func>))
        {
            std::cout << "that is not a function\n";
            std::cout << v.tokenValue.source.printHint()  << "here \n";
            throw std::runtime_error("");
        }

        auto& fn = (*localScope.getVariable(v.name.tokenValue.content))|vx::as<TemporaryValue::Func>;



        if(fn.value.params.size() != v.params.size())
        {
            std::cout << "not matching number of arguments\n";
            std::cout << v.tokenValue.source.printHint()  << "here \n";
            throw std::runtime_error("");
        }
        auto fnScope = std::make_unique<RuntimeScope>(globalScope);
        for(int i = 0; i!= v.params.size(); ++i)
        {
            fnScope->variables[fn.value.params[i].tokenValue.content] = treeWallInterpret(v.params[i],globalScope,localScope);
        }

        try
        {
            auto res = treeWallInterpret(*fn.value.body,globalScope,fnScope.get(),true);
            return std::move(res);
        }
        catch(FuncReturn& ret)
        {
            return std::move(ret.result);
        }*/
    }
    void operator()(const AstNode::Return& v) override
    {
        throw FuncReturn{treeWallInterpret(v.inner,globalScope,localScope)};
    }
};


TemporaryValue::Any treeWallInterpret(const AstNode::OwnedNode& in,RuntimeScope& globalScope,RuntimeScope& localScope,bool preventNewScopeFromBlock)
{
    TemporaryValue::Any result;
    in->accept(InterpreterVisitor(result,globalScope,localScope,preventNewScopeFromBlock));
    return result;

    std::cout << "unable to execute '" << in << "'\n";
    throw std::runtime_error("interpreting error");
}