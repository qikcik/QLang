#pragma once

#include <array>
#include <memory>
#include <string>

#include "LexToken.hpp"
#include "vx.hpp"

namespace AstNode
{
    struct Identifier;
    struct Integer; struct Float; struct String; struct Bool;
    struct UnaryOp; struct BinaryOp;
    struct Block; struct PrintStmt; struct IfStmt; struct AssignStmt; struct WhileStmt; struct ForStmt; struct FunctionDecl; struct FunctionCall; struct Return;

    using Any = std::variant<Identifier,Integer,Float,String,Bool,UnaryOp,BinaryOp,Block,PrintStmt,IfStmt,AssignStmt,WhileStmt,ForStmt, FunctionDecl,FunctionCall,Return>;

    struct Base
    {
        virtual ~Base() = default;
        virtual Any copy() const = 0;
    };

    struct Identifier final : public Base
    {
        explicit Identifier(const LexToken::Label& inValue) : tokenValue(inValue) {};
        LexToken::Label tokenValue;

        Any copy() const override;
    };

    struct Integer : public Base
    {
        explicit Integer(const LexToken::Integer& inValue) : tokenValue(inValue) {};
        LexToken::Integer tokenValue;

        Any copy() const override;;
    };

    struct Float final : public Base
    {
        explicit Float(const LexToken::Float& inValue) : tokenValue(inValue) {};
        LexToken::Float tokenValue;

        Any copy() const override;;
    };

    struct Bool final : public Base
    {
        explicit Bool(const LexToken::Label& inValue) : tokenValue(inValue) {};
        LexToken::Label tokenValue;

        Any copy() const override;
    };

    struct String final : public Base
    {
        explicit String(const LexToken::String& inValue) : tokenValue(inValue) {};
        LexToken::String tokenValue;

        Any copy() const override;
    };

    struct UnaryOp final : public Base
    {
        UnaryOp(const LexToken::Separator& inOp,std::unique_ptr<AstNode::Any> inInner) : tokenValue(inOp), inner(std::move(inInner)) {};
        LexToken::Separator tokenValue;
        std::unique_ptr<AstNode::Any> inner;

        Any copy() const override;

    };

    struct BinaryOp final : public Base
    {
        BinaryOp(const LexToken::Separator& inOp, std::unique_ptr<AstNode::Any>  inLeft, std::unique_ptr<AstNode::Any>  inRight) : tokenValue(inOp), left(std::move(inLeft)), right(std::move(inRight)) {};
        LexToken::Separator tokenValue;
        std::unique_ptr<AstNode::Any> left;
        std::unique_ptr<AstNode::Any> right;

        Any copy() const override;
    };

    struct Block final : public Base
    {
        explicit Block(std::vector<std::unique_ptr<AstNode::Any>> inStatements) : statements(std::move(inStatements)) {}
        std::vector<std::unique_ptr<AstNode::Any>> statements;

        Any copy() const override;
    };

    struct PrintStmt final : public Base
    {
        PrintStmt(const LexToken::Label& inOp,std::unique_ptr<AstNode::Any> inInner) : tokenValue(inOp), inner(std::move(inInner)) {};
        LexToken::Label tokenValue;
        std::unique_ptr<AstNode::Any> inner;

        Any copy() const override;
    };

    struct IfStmt final : public Base
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

        Any copy() const override;
    };

    struct AssignStmt final : public Base
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

        Any copy() const override;
    };

    struct WhileStmt final : public Base
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

        Any copy() const override;
    };

    struct ForStmt final : public Base
    {
        ForStmt(const LexToken::Label& inOp,
                std::unique_ptr<AstNode::Any> doOnce,
                std::unique_ptr<AstNode::Any> until,
                std::unique_ptr<AstNode::Any> afterIter,
                std::unique_ptr<AstNode::Any> loop)
        : tokenValue(inOp),
            doOnce(std::move(doOnce)),
            until(std::move(until)),
            afterIter(std::move(afterIter)),
            loop(std::move(loop)){};

        LexToken::Label tokenValue;
        std::unique_ptr<AstNode::Any> doOnce;
        std::unique_ptr<AstNode::Any> until;
        std::unique_ptr<AstNode::Any> afterIter;
        std::unique_ptr<AstNode::Any> loop;

        Any copy() const override;
    };

    struct FunctionDecl final : public Base
    {
        FunctionDecl(const LexToken::Separator& inOp,
                std::vector<AstNode::Identifier> params,
                std::unique_ptr<AstNode::Any> body)
        : tokenValue(inOp),
            params(std::move(params)),
            body(std::move(body)){};

        LexToken::Separator tokenValue;
        std::vector<AstNode::Identifier> params;
        std::unique_ptr<AstNode::Any> body;

        Any copy() const override;
    };

    struct FunctionCall final : public Base
    {
        FunctionCall(const LexToken::Separator& inOp,
                AstNode::Identifier name,
                std::vector<AstNode::Any> params)
        : tokenValue(inOp), params(std::move(params)), name(std::move(name)){};

        LexToken::Separator tokenValue;
        AstNode::Identifier name;
        std::vector<AstNode::Any> params;

        Any copy() const override;
    };

    struct Return final : public Base
    {
        Return(const LexToken::Label& inOp,std::unique_ptr<AstNode::Any> inInner) : tokenValue(inOp), inner(std::move(inInner)) {};
        LexToken::Label tokenValue;
        std::unique_ptr<AstNode::Any> inner;

        Any copy() const override;

    };

    std::string stringify(const AstNode::Any& in, int intend = 0);
}


inline std::ostream& operator<<(std::ostream& os, const AstNode::Any& in)
{
    return os << AstNode::stringify(in, 0);
}
