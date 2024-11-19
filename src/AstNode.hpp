#pragma once
#include <array>
#include <memory>
#include <string>

#include "LexToken.hpp"



namespace AstNode
{
    struct Literal {}; struct Integer; struct Float; struct String; struct Bool;
    struct Operation {}; struct UnaryOp; struct BinaryOp;

    using Any = std::variant<Integer,Float,String,Bool,UnaryOp,BinaryOp>;

    struct Integer : public Literal
    {
        explicit Integer(const LexToken::Integer& inValue) : tokenValue(inValue) {};
        LexToken::Integer tokenValue;
    };

    struct Float final : public Literal
    {
        explicit Float(const LexToken::Float& inValue) : tokenValue(inValue) {};
        LexToken::Float tokenValue;
    };

    struct Bool final : public Literal
    {
        explicit Bool(const LexToken::Label& inValue) : tokenValue(inValue) {};
        LexToken::Label tokenValue;
    };

    struct String final : public Literal
    {
        explicit String(const LexToken::String& inValue) : tokenValue(inValue) {};
        LexToken::String tokenValue;
    };


    struct UnaryOp final : public Operation
    {
        UnaryOp(const LexToken::Separator& inOp,std::unique_ptr<AstNode::Any> inInner) : tokenValue(inOp), inner(std::move(inInner)) {};
        LexToken::Separator tokenValue;
        std::unique_ptr<AstNode::Any> inner;

    };

    struct BinaryOp final : public Operation
    {
        BinaryOp(const LexToken::Separator& inOp, std::unique_ptr<AstNode::Any>  inLeft, std::unique_ptr<AstNode::Any>  inRight) : tokenValue(inOp), left(std::move(inLeft)), right(std::move(inRight)) {};
        LexToken::Separator tokenValue;
        std::unique_ptr<AstNode::Any> left;
        std::unique_ptr<AstNode::Any> right;
    };

    std::string stringify(const AstNode::Any& in, int intend = 0)
    {
        std::string result;
        for(int i=0;i!=intend;i++) result+="\t";

        result += in |vx::match {
            [&in](const AstNode::Integer& v)
                { return "Integer{"+std::to_string(v.tokenValue.content)+"} at "+v.tokenValue.source.stringify(); },
            [&in](const AstNode::Float& v)
                {return "Float{"+std::to_string(v.tokenValue.content)+"} at "+v.tokenValue.source.stringify(); },
            [&in](const AstNode::Bool& v)
                { return "Bool{"+v.tokenValue.content+"} at "+v.tokenValue.source.stringify(); },
            [&in](const AstNode::String& v)
                { return "String{"+v.tokenValue.content+"} at "+v.tokenValue.source.stringify(); },
            [&in,intend](const AstNode::UnaryOp& v)
            {
                std::string result = "UnaryOp{\n";

                for(int i=0;i!=intend+1;i++) result+="\t";
                result += v.tokenValue.content+" at "+v.tokenValue.source.stringify()+"\n";

                result += stringify(*v.inner, intend+1)+"\n";
                for(int i=0;i!=intend;i++) result+="\t";
                return result + "}";
            },
            [&in,intend](const AstNode::BinaryOp& v)
            {
                std::string result = "BinaryOp{\n";

                for(int i=0;i!=intend+1;i++) result+="\t";
                result += v.tokenValue.content+" at "+v.tokenValue.source.stringify()+"\n";

                result += stringify(*v.left, intend+1)+",\n";
                result += stringify(*v.right, intend+1)+"\n";
                for(int i=0;i!=intend;i++) result+="\t";
                return result + "}";
            },
        };

        return result;
    }
}


std::ostream& operator<<(std::ostream& os, const AstNode::Any& in)
{
    return os << AstNode::stringify(in, 0);
}
