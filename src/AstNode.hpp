#pragma once
#include <memory>
#include <string>

#include "LexToken.hpp"

struct AstNode
{
    virtual std::string stringify() const
    {
        return "<AstNode>{}";
    }
};
struct AstInteger : public AstNode
{
    explicit AstInteger(const LexToken::Integer& inValue) : tokenValue(inValue) {};
    LexToken::Integer tokenValue;

    std::string stringify() const override
    {
        return "<AstInteger>{"+std::to_string(tokenValue.content)+"}";
    }
};
struct AstFloat : public AstNode
{
    explicit AstFloat(const LexToken::Float& inValue) : tokenValue(inValue) {};
    LexToken::Float tokenValue;

    std::string stringify() const override
    {
        return "<AstFloat>{"+std::to_string(tokenValue.content)+"}";
    }
};

struct AstUnaryOp : public AstNode
{
    AstUnaryOp(const LexToken::Separator& inOp, std::unique_ptr<AstNode> inInner) : operation(inOp), inner(std::move(inInner)) {};
    LexToken::Separator operation;
    std::unique_ptr<AstNode> inner;

    std::string stringify() const override
    {
        return "<AstUnary>{'"+operation.content+"',"+inner->stringify()+"}";
    }
};

struct AstMathOp : public AstNode
{
    AstMathOp(const LexToken::Separator& inOp, std::unique_ptr<AstNode> inLeft, std::unique_ptr<AstNode> inRight) : operation(inOp), left(std::move(inLeft)), right(std::move(inRight)) {};
    LexToken::Separator operation;
    std::unique_ptr<AstNode> left;
    std::unique_ptr<AstNode> right;

    std::string stringify() const override
    {
        return "<AstMathOp>{"+left->stringify()+",'"+operation.content+"',"+right->stringify()+"}";
    }
};