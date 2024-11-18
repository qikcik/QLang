#pragma once

#include <memory>

#include "LexScanner.hpp"
#include "AstNode.hpp"

class AstParser
{
public:
    explicit AstParser(LexScanner& inScanner) : scanner(inScanner) {};

    //<primary> ::= <integer> | <float> | '(' <expr> ')'
    std::unique_ptr<AstNode> primary()
    {
        if(const auto v = scanner.current<LexToken::Integer>())
        {
            scanner.next();
            return std::make_unique<AstInteger>( *v );
        }
        if(const auto v =scanner.current<LexToken::Float>())
        {
            scanner.next();
            return std::make_unique<AstFloat>( *v );
        }
        if(scanner.currentMath<LexToken::Separator>("("))
        {
            scanner.next();
            std::unique_ptr<AstNode> e = std::move(expr());
            if(scanner.currentMath<LexToken::Separator>(")"))
            {
                scanner.next();
            }
            else
            {
                throw std::runtime_error("Unbalanced parentheses");
            }

            return e;
        }
        throw std::runtime_error("unexpected token");
    }

    //<unary> ::= ('+'|'-'|'~') <unary> | <primary>
    std::unique_ptr<AstNode> unary()
    {
        if(scanner.currentMath<LexToken::Separator>("~") || scanner.currentMath<LexToken::Separator>("+") || scanner.currentMath<LexToken::Separator>("-"))
        {
            auto op = *scanner.current<LexToken::Separator>();
            scanner.next();

            auto inner = std::move(unary());
            return std::make_unique<AstUnaryOp>( op, std::move(inner) );
        }
        return primary();
    }

    //<multiplication> ::= <unary> ( ('+' | '-') <unary> )*
    std::unique_ptr<AstNode> multiplication()
    {
        auto e = unary();
        while (scanner.currentMath<LexToken::Separator>("*") || scanner.currentMath<LexToken::Separator>("/") )
        {

            auto op = *scanner.current<LexToken::Separator>();
            scanner.next();

            auto right = std::move(unary());
            e = std::make_unique<AstMathOp>(op, std::move(e), std::move(right));

        }
        return std::move(e);
    }

    //<addition> ::= <multiplication> ( ('+' | '-') <multiplication> )*
    std::unique_ptr<AstNode> addition()
    {
        auto e = multiplication();

        while (scanner.currentMath<LexToken::Separator>("+") || scanner.currentMath<LexToken::Separator>("-") )
        {
            auto op = *scanner.current<LexToken::Separator>();
            scanner.next();

            auto right = std::move(multiplication());
            e = std::make_unique<AstMathOp>(op, std::move(e), std::move(right));

        }
        return std::move(e);
    }

    //<expr> ::= <addition>
    std::unique_ptr<AstNode> expr()
    {
        return addition();
    }

protected:
    LexScanner& scanner;
};