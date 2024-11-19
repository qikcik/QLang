#pragma once

#include <memory>

#include "LexScanner.hpp"
#include "AstNode.hpp"

class AstParser
{
public:
    explicit AstParser(LexScanner& inScanner) : scanner(inScanner) {};

    //<primary> ::= <integer> | <float> | <string> |  <bool> as ('true'|'false') | '(' <expr> ')'
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
        if(const auto v =scanner.current<LexToken::String>())
        {
            scanner.next();
            return std::make_unique<AstString>( *v );
        }
        if(scanner.currentMath<LexToken::Label>("true") || scanner.currentMath<LexToken::Label>("false"))
        {
            const auto v = *scanner.current<LexToken::Label>();
            scanner.next();
            return std::make_unique<AstBool>( v );
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
        std::cout << "unexpected token:" << scanner.current() << std::endl;
        throw std::runtime_error("unexpected token");
    }

    //<unary> ::= ('+'|'-'|'!') <unary> | <primary>
    std::unique_ptr<AstNode> unary()
    {
        if(scanner.currentMath<LexToken::Separator>("+") || scanner.currentMath<LexToken::Separator>("-") || scanner.currentMath<LexToken::Separator>("!"))
        {
            auto op = *scanner.current<LexToken::Separator>();
            scanner.next();

            auto inner = std::move(unary());
            return std::make_unique<AstUnaryOp>( op, std::move(inner) );
        }
        return primary();
    }

    //<exponent> ::= <unary> ( ('^') <exponent> )*
    std::unique_ptr<AstNode> exponent()
    {
        auto e = unary();
        while (scanner.currentMath<LexToken::Separator>("^") )
        {

            auto op = *scanner.current<LexToken::Separator>();
            scanner.next();

            auto right = std::move(exponent());
            e = std::make_unique<AstBinaryOp>(op, std::move(e), std::move(right));

        }
        return std::move(e);
    }

    //<multiplication> ::= <exponent> ( ('+' | '-') <exponent> )*
    std::unique_ptr<AstNode> multiplication()
    {
        auto e = exponent();
        while (scanner.currentMath<LexToken::Separator>("*") || scanner.currentMath<LexToken::Separator>("/") )
        {

            auto op = *scanner.current<LexToken::Separator>();
            scanner.next();

            auto right = std::move(exponent());
            e = std::make_unique<AstBinaryOp>(op, std::move(e), std::move(right));

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
            e = std::make_unique<AstBinaryOp>(op, std::move(e), std::move(right));

        }
        return std::move(e);
    }

    //<comparison> ::= <addition> ( ('==' | '!=' | '<' | '>' | '<=' | '>=' ) <addition> )*
    std::unique_ptr<AstNode> comparison()
    {
        auto e = addition();

        while (scanner.currentMath<LexToken::Separator>("==")
            || scanner.currentMath<LexToken::Separator>("!=")
            || scanner.currentMath<LexToken::Separator>("<")
            || scanner.currentMath<LexToken::Separator>(">")
            || scanner.currentMath<LexToken::Separator>("<=")
            || scanner.currentMath<LexToken::Separator>(">="))
        {
            auto op = *scanner.current<LexToken::Separator>();
            scanner.next();

            auto right = std::move(addition());
            e = std::make_unique<AstBinaryOp>(op, std::move(e), std::move(right));

        }
        return std::move(e);
    }

    //<logic> ::= <comparison> ( ('&&'|'||') <comparison> )*
    std::unique_ptr<AstNode> logic()
    {
        auto e = comparison();

        while (scanner.currentMath<LexToken::Separator>("&&")
            || scanner.currentMath<LexToken::Separator>("||"))
        {
            auto op = *scanner.current<LexToken::Separator>();
            scanner.next();

            auto right = std::move(comparison());
            e = std::make_unique<AstBinaryOp>(op, std::move(e), std::move(right));

        }
        return std::move(e);
    }


    //<expr> ::= <logic>
    std::unique_ptr<AstNode> expr()
    {
        return logic();
    }

protected:
    LexScanner& scanner;
};