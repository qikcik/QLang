#pragma once
#include <array>
#include <memory>
#include <string>

#include "LexToken.hpp"



namespace AstNode
{
    struct Identifier;
    struct Literal {}; struct Integer; struct Float; struct String; struct Bool;
    struct Operation {}; struct UnaryOp; struct BinaryOp;
    struct Stmt {}; struct Block; struct PrintStmt; struct IfStmt; struct AssignStmt; struct WhileStmt;

    using Any = std::variant<Identifier,Integer,Float,String,Bool,UnaryOp,BinaryOp,Block,PrintStmt,IfStmt,AssignStmt,WhileStmt>;

    struct Identifier final
    {
        explicit Identifier(const LexToken::Label& inValue) : tokenValue(inValue) {};
        LexToken::Label tokenValue;
    };

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

    struct Block final : public Stmt
    {
        explicit Block(std::vector<std::unique_ptr<AstNode::Any>> inStatements) : statements(std::move(inStatements)) {}
        std::vector<std::unique_ptr<AstNode::Any>> statements;
    };

    struct PrintStmt final : public Stmt
    {
        PrintStmt(const LexToken::Label& inOp,std::unique_ptr<AstNode::Any> inInner) : tokenValue(inOp), inner(std::move(inInner)) {};
        LexToken::Label tokenValue;
        std::unique_ptr<AstNode::Any> inner;
    };

    struct IfStmt final : public Stmt
    {
        IfStmt(const LexToken::Label& inOp,
                std::unique_ptr<AstNode::Any> when,
                std::unique_ptr<AstNode::Any> then,
                std::unique_ptr<AstNode::Any> elseThen)
        : tokenValue(inOp),
            when(std::move(when)),
            then(std::move(then)),
            elseThen(std::move(elseThen)) {};

        LexToken::Label tokenValue;
        std::unique_ptr<AstNode::Any> when;
        std::unique_ptr<AstNode::Any> then;
        std::unique_ptr<AstNode::Any> elseThen;
    };

    struct AssignStmt final : public Stmt
    {
        AssignStmt(const LexToken::Separator& inOp,
                AstNode::Identifier inIdentifier,
                std::unique_ptr<AstNode::Any> inValue)
        : tokenValue(inOp),
            identifier(std::move(inIdentifier)),
            value(std::move(inValue)) {};

        LexToken::Separator tokenValue;
        AstNode::Identifier identifier;
        std::unique_ptr<AstNode::Any> value;
    };

    struct WhileStmt final : public Stmt
    {
        WhileStmt(const LexToken::Label& inOp,
                std::unique_ptr<AstNode::Any> until,
                std::unique_ptr<AstNode::Any> loop)
        : tokenValue(inOp),
            until(std::move(until)),
            loop(std::move(loop)) {};

        LexToken::Label tokenValue;
        std::unique_ptr<AstNode::Any> until;
        std::unique_ptr<AstNode::Any> loop;
    };

    std::string stringify(const AstNode::Any& in, int intend = 0)
    {
        std::string result;
        for(int i=0;i!=intend;i++) result+="\t";

        result += in |vx::match {
            [&in](const AstNode::Identifier& v)
                { return "Identifier{"+v.tokenValue.content+"} at "+v.tokenValue.source.stringify(); },
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
            [&in,intend](const AstNode::Block& v)
            {
                std::string result = "Block{\n";
                for(auto& i : v.statements)
                {
                    result += stringify(*i, intend+1)+",\n";
                }
                for(int i=0;i!=intend;i++) result+="\t";
                return result + "}";
            },
            [&in,intend](const AstNode::PrintStmt& v)
            {
                std::string result = "PrintStmt{\n";

                for(int i=0;i!=intend+1;i++) result+="\t";
                result += v.tokenValue.content+" at "+v.tokenValue.source.stringify()+"\n";

                result += stringify(*v.inner, intend+1)+"\n";
                for(int i=0;i!=intend;i++) result+="\t";
                return result + "}";
            },
            [&in,intend](const AstNode::IfStmt& v)
            {
                std::string result = "IfStmt{\n";

                for(int i=0;i!=intend+1;i++) result+="\t";
                result += v.tokenValue.content+" at "+v.tokenValue.source.stringify()+"\n";

                result += stringify(*v.when, intend+1)+",\n";
                result += stringify(*v.then, intend+1)+"\n";
                if(v.elseThen)
                    result += stringify(*v.elseThen, intend+1)+"\n";
                for(int i=0;i!=intend;i++) result+="\t";
                return result + "}";
            },
            [&in,intend](const AstNode::AssignStmt& v)
            {
                std::string result = "AssignStmt{\n";

                for(int i=0;i!=intend+1;i++) result+="\t";
                result += v.tokenValue.content+" at "+v.tokenValue.source.stringify()+"\n";

                result += stringify(v.identifier, intend+1)+",\n";
                result += stringify(*v.value, intend+1)+"\n";
                for(int i=0;i!=intend;i++) result+="\t";
                return result + "}";
            },
            [&in,intend](const AstNode::WhileStmt& v)
            {
                std::string result = "WhileStmt{\n";

                for(int i=0;i!=intend+1;i++) result+="\t";
                result += v.tokenValue.content+" at "+v.tokenValue.source.stringify()+"\n";

                result += stringify(*v.until, intend+1)+",\n";
                result += stringify(*v.loop, intend+1)+"\n";
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
