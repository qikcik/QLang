#pragma once

#include <array>
#include <memory>
#include <regex>
#include <string>

#include "LexToken.hpp"
#include "vx.hpp"

namespace AstNode
{
    struct Base;
    struct Identifier;
    struct Integer; struct Float; struct String; struct Bool;
    struct UnaryOp; struct BinaryOp;
    struct Block; struct PrintStmt; struct IfStmt; struct AssignStmt; struct WhileStmt; struct ForStmt; struct FunctionDecl; struct FunctionCall; struct Return;

    using OwnedNode = std::unique_ptr<Base>;


    struct IVisitor
    {
        virtual ~IVisitor() = default;
        virtual void operator()(const Identifier&) =0;
        virtual void operator()(const Integer&) =0;
        virtual void operator()(const Float&) =0;
        virtual void operator()(const String&) =0;
        virtual void operator()(const Bool&) =0;
        virtual void operator()(const UnaryOp&) =0;
        virtual void operator()(const BinaryOp&) =0;
        virtual void operator()(const Block&) =0;
        virtual void operator()(const PrintStmt&) =0;
        virtual void operator()(const IfStmt&) =0;
        virtual void operator()(const AssignStmt&) =0;
        virtual void operator()(const WhileStmt&) =0;
        virtual void operator()(const ForStmt&) =0;
        virtual void operator()(const FunctionDecl&) =0;
        virtual void operator()(const FunctionCall&) =0;
        virtual void operator()(const Return&) =0;
    };

    struct Base
    {
        Base() = default;
        virtual ~Base() = default;

        virtual OwnedNode copy() const = 0;
        virtual void accept(IVisitor&& visitor) const = 0;
    };

    template <typename TDerivered>
    struct BaseImpl : public Base
    {
        BaseImpl(): Base() {} ;
        ~BaseImpl() override = default;

        void accept(IVisitor&& visitor) const override
        {
            visitor.operator()(static_cast<const TDerivered&>(*this));
        }
    };

    struct Identifier final : public BaseImpl<Identifier>
    {
        explicit Identifier(const LexToken::Label& inValue) : BaseImpl<Identifier>(), tokenValue(inValue)
        {
        };
        LexToken::Label tokenValue;

        OwnedNode copy() const override;
    };

    struct Integer : public BaseImpl<Integer>
    {
        explicit Integer(const LexToken::Integer& inValue) : BaseImpl<Integer>(), tokenValue(inValue)
        {
        };
        LexToken::Integer tokenValue;

        OwnedNode copy() const override;;
    };

    struct Float final : public BaseImpl<Float>
    {
        explicit Float(const LexToken::Float& inValue) : BaseImpl<Float>(), tokenValue(inValue)
        {
        };
        LexToken::Float tokenValue;

        OwnedNode copy() const override;
    };

    struct Bool final : public BaseImpl<Bool>
    {
        explicit Bool(const LexToken::Label& inValue) : BaseImpl<Bool>(), tokenValue(inValue)
        {
        };
        LexToken::Label tokenValue;

        OwnedNode copy() const override;
    };

    struct String final : public BaseImpl<String>
    {
        explicit String(const LexToken::String& inValue) : BaseImpl<String>(), tokenValue(inValue)
        {
        };
        LexToken::String tokenValue;

        OwnedNode copy() const override;
    };

    struct UnaryOp final : public BaseImpl<UnaryOp>
    {
        UnaryOp(const LexToken::Separator& inOp,OwnedNode inInner) : BaseImpl<UnaryOp>(),
            tokenValue(inOp), inner(std::move(inInner))
        {
        };
        LexToken::Separator tokenValue;
        OwnedNode inner;

        OwnedNode copy() const override;

    };

    struct BinaryOp final : public BaseImpl<BinaryOp>
    {
        BinaryOp(const LexToken::Separator& inOp, OwnedNode inLeft, OwnedNode inRight) :
            BaseImpl<BinaryOp>(), tokenValue(inOp), left(std::move(inLeft)), right(std::move(inRight))
        {
        };
        LexToken::Separator tokenValue;
        OwnedNode left;
        OwnedNode right;

        OwnedNode copy() const override;
    };

    struct Block final : public BaseImpl<Block>
    {
        explicit Block(std::vector<OwnedNode> inStatements) : BaseImpl<Block>(),
            statements(std::move(inStatements))
        {
        }

        std::vector<OwnedNode> statements;

        OwnedNode copy() const override;
    };

    struct PrintStmt final : public BaseImpl<PrintStmt>
    {
        PrintStmt(const LexToken::Label& inOp,OwnedNode inInner) : BaseImpl<PrintStmt>(),
            tokenValue(inOp), inner(std::move(inInner))
        {
        };
        LexToken::Label tokenValue;
        OwnedNode inner;

        OwnedNode copy() const override;
    };

    struct IfStmt final : public BaseImpl<IfStmt>
    {
        IfStmt(const LexToken::Label& inOp,
                OwnedNode when,
                OwnedNode then,
                OwnedNode elseThen)
            : BaseImpl<IfStmt>(), tokenValue(inOp),
              when(std::move(when)),
              then(std::move(then)),
              elseThen(std::move(elseThen))
        {
        };

        LexToken::Label tokenValue;
        OwnedNode when;
        OwnedNode then;
        OwnedNode elseThen;

        OwnedNode copy() const override;
    };

    struct AssignStmt final : public BaseImpl<AssignStmt>
    {
        AssignStmt(const LexToken::Separator& inOp,
                OwnedNode inIdentifier,
                OwnedNode inValue)
            : BaseImpl<AssignStmt>(), tokenValue(inOp),
              identifier(std::move(inIdentifier)),
              value(std::move(inValue))
        {
        };

        LexToken::Separator tokenValue;
        OwnedNode identifier;
        OwnedNode value;

        OwnedNode copy() const override;
    };

    struct WhileStmt final : public BaseImpl<WhileStmt>
    {
        WhileStmt(const LexToken::Label& inOp,
                OwnedNode until,
                OwnedNode loop)
            : BaseImpl<WhileStmt>(), tokenValue(inOp),
              until(std::move(until)),
              loop(std::move(loop))
        {
        };

        LexToken::Label tokenValue;
        OwnedNode until;
        OwnedNode loop;

        OwnedNode copy() const override;
    };

    struct ForStmt final : public BaseImpl<ForStmt>
    {
        ForStmt(const LexToken::Label& inOp,
                OwnedNode doOnce,
                OwnedNode until,
                OwnedNode afterIter,
                OwnedNode loop)
            : BaseImpl<ForStmt>(), tokenValue(inOp),
              doOnce(std::move(doOnce)),
              until(std::move(until)),
              afterIter(std::move(afterIter)),
              loop(std::move(loop))
        {
        };

        LexToken::Label tokenValue;
        OwnedNode doOnce;
        OwnedNode until;
        OwnedNode afterIter;
        OwnedNode loop;

        OwnedNode copy() const override;
    };

    struct FunctionDecl final : public BaseImpl<FunctionDecl>
    {
        FunctionDecl(const LexToken::Separator& inOp,
                std::vector<AstNode::OwnedNode> params,
                OwnedNode body)
            : BaseImpl<FunctionDecl>(), tokenValue(inOp),
              params(std::move(params)),
              body(std::move(body))
        {
        };

        LexToken::Separator tokenValue;
        std::vector<AstNode::OwnedNode> params;
        OwnedNode body;

        OwnedNode copy() const override;
    };

    struct FunctionCall final : public BaseImpl<FunctionCall>
    {
        FunctionCall(const LexToken::Separator& inOp,
                OwnedNode name,
                std::vector<OwnedNode> args)
            : BaseImpl<FunctionCall>(), tokenValue(inOp), args(std::move(args)), name(std::move(name))
        {
        };

        LexToken::Separator tokenValue;
        OwnedNode name;
        std::vector<OwnedNode> args;

        OwnedNode copy() const override;
    };

    struct Return final : public BaseImpl<Return>
    {
        Return(const LexToken::Label& inOp,OwnedNode inInner) : BaseImpl<Return>(),
            tokenValue(inOp), inner(std::move(inInner))
        {
        };
        LexToken::Label tokenValue;
        OwnedNode inner;

        OwnedNode copy() const override;

    };

    std::string stringify(const Base& in, int intend = 0);
}


inline std::ostream& operator<<(std::ostream& os, const AstNode::Base& in)
{
    return os << AstNode::stringify(in, 0);
}
