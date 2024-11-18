#pragma once

#include <memory>

#include "LexScanner.hpp"

struct AstNode
{
    virtual std::string stringify()
    {
        return "<AstNode>{}";
    }
};
struct AstInteger : public AstNode
{
    explicit AstInteger(const int inValue) : value(inValue) {};
    int value;

    std::string stringify() override
    {
        return "<AstInteger>{"+std::to_string(value)+"}";
    }
};
struct AstFloat : public AstNode
{
    explicit AstFloat(const float inValue) : value(inValue) {};
    float value;

    std::string stringify() override
    {
        return "<AstFloat>{"+std::to_string(value)+"}";
    }
};

struct AstUnary : public AstNode
{
    AstUnary(std::string inUnaryOperator, std::unique_ptr<AstNode> inValue) : unaryOperator(std::move(inUnaryOperator)), value(std::move(inValue)) {};
    std::string unaryOperator;
    std::unique_ptr<AstNode> value;

    std::string stringify() override
    {
        return "<AstUnary>{'"+unaryOperator+"',"+value->stringify()+"}";
    }
};

struct AstMathOp : public AstNode
{
    AstMathOp(std::string inOperation, std::unique_ptr<AstNode> inLeft, std::unique_ptr<AstNode> inRight) : operation(std::move(inOperation)), left(std::move(inLeft)), right(std::move(inRight)) {};
    std::string operation;
    std::unique_ptr<AstNode> left;
    std::unique_ptr<AstNode> right;

    std::string stringify() override
    {
        return "<AstMathOp>{'"+operation+"',"+left->stringify()+","+right->stringify()+"}";
    }
};

class AstParser
{
public:
    explicit AstParser(LexScanner& inScanner) : scanner(inScanner) {};

    //<primary> ::= <integer> | <float> | '(' <expr> ')'
    std::unique_ptr<AstNode> primary();

    //<unary> ::= ('+'|'-'|'~') <unary> | <primary>
    std::unique_ptr<AstNode> unary();

    //<factor> ::= <unary>
    std::unique_ptr<AstNode> factor();

    //<term> ::= <factor> ( ('*' | '/') <factor> )*
    std::unique_ptr<AstNode> term();

    //<expr> ::= <term> ( ('+' | '-') <term> )*
    std::unique_ptr<AstNode> expr();

protected:
    LexScanner& scanner;
};