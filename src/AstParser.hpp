#pragma once

#include <memory>

#include "LexScanner.hpp"
#include "AstNode.hpp"

class AstParser
{
public:
    explicit AstParser(LexScanner& inScanner) : scanner(inScanner) {};

    //<primary> ::= <integer> | <float> | <string> |  <bool> as ('true'|'false') | '(' <expr> ')'
    std::unique_ptr<AstNode::Any> primary()
    {
        if(const auto v = scanner.current<LexToken::Integer>())
        {
            scanner.next();
            return std::make_unique<AstNode::Any>( AstNode::Integer{*v} );
        }
        if(const auto v =scanner.current<LexToken::Float>())
        {
            scanner.next();
            return std::make_unique<AstNode::Any>( AstNode::Float{*v} );
        }
        if(const auto v =scanner.current<LexToken::String>())
        {
            scanner.next();
            return std::make_unique<AstNode::Any>( AstNode::String{*v} );
        }
        if(scanner.currentMath<LexToken::Label>("true") || scanner.currentMath<LexToken::Label>("false"))
        {
            const auto v = *scanner.current<LexToken::Label>();
            scanner.next();
            return std::make_unique<AstNode::Any>( AstNode::Bool{v} );
        }
        if(auto open = scanner.currentMath<LexToken::Separator>("("))
        {
            scanner.next();
            std::unique_ptr<AstNode::Any> e = std::move(expr());
            if(scanner.currentMath<LexToken::Separator>(")"))
            {
                scanner.next();
            }
            else
            {
                std::cout << "\nCRITICAL PARSER ERROR: expected closing parentheses, opened " << LexToken::printHint(*open) << "not found closing ')'" << std::endl;
                throw std::runtime_error("");
            }

            return e;
        }
        std::cout << "\nCRITICAL PARSER ERROR: couldn't parse as primary " << (scanner.current() ? LexToken::printHint(*scanner.current()) : "'in the end of file'") << "unexpected token" << std::endl;
        throw std::runtime_error("");
    }

    //<unary> ::= ('+'|'-'|'!') <unary> | <primary>
    std::unique_ptr<AstNode::Any> unary()
    {
        if(scanner.currentMath<LexToken::Separator>("+") || scanner.currentMath<LexToken::Separator>("-") || scanner.currentMath<LexToken::Separator>("!"))
        {
            auto op = *scanner.current<LexToken::Separator>();
            scanner.next();

            auto inner = std::move(unary());
            return std::make_unique<AstNode::Any>( AstNode::UnaryOp{op, std::move(inner)} );
        }
        return primary();
    }

    //<exponent> ::= <unary> ( ('^') <exponent> )*
    std::unique_ptr<AstNode::Any> exponent()
    {
        auto e = unary();
        while (scanner.currentMath<LexToken::Separator>("^") )
        {

            auto op = *scanner.current<LexToken::Separator>();
            scanner.next();

            auto right = std::move(exponent());
            e = std::make_unique<AstNode::Any>( AstNode::BinaryOp{op, std::move(e), std::move(right)} );

        }
        return std::move(e);
    }

    //<multiplication> ::= <exponent> ( ('+' | '-') <exponent> )*
    std::unique_ptr<AstNode::Any> multiplication()
    {
        auto e = exponent();
        while (scanner.currentMath<LexToken::Separator>("*") || scanner.currentMath<LexToken::Separator>("/") )
        {

            auto op = *scanner.current<LexToken::Separator>();
            scanner.next();

            auto right = std::move(exponent());
            e = std::make_unique<AstNode::Any>(AstNode::BinaryOp{op, std::move(e), std::move(right)});

        }
        return std::move(e);
    }

    //<addition> ::= <multiplication> ( ('+' | '-') <multiplication> )*
    std::unique_ptr<AstNode::Any> addition()
    {
        auto e = multiplication();

        while (scanner.currentMath<LexToken::Separator>("+") || scanner.currentMath<LexToken::Separator>("-") )
        {
            auto op = *scanner.current<LexToken::Separator>();
            scanner.next();

            auto right = std::move(multiplication());
            e = std::make_unique<AstNode::Any>(AstNode::BinaryOp{op, std::move(e), std::move(right)});

        }
        return std::move(e);
    }

    //<comparison> ::= <addition> ( ('==' | '!=' | '<' | '>' | '<=' | '>=' ) <addition> )*
    std::unique_ptr<AstNode::Any> comparison()
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
            e = std::make_unique<AstNode::Any>(AstNode::BinaryOp{op, std::move(e), std::move(right)});

        }
        return std::move(e);
    }

    //<logic> ::= <comparison> ( ('&&'|'||') <comparison> )*
    std::unique_ptr<AstNode::Any> logic()
    {
        auto e = comparison();

        while (scanner.currentMath<LexToken::Separator>("&&")
            || scanner.currentMath<LexToken::Separator>("||"))
        {
            auto op = *scanner.current<LexToken::Separator>();
            scanner.next();

            auto right = std::move(comparison());
            e = std::make_unique<AstNode::Any>(AstNode::BinaryOp{op, std::move(e), std::move(right)});

        }
        return std::move(e);
    }


    //<expr> ::= <logic>
    std::unique_ptr<AstNode::Any> expr()
    {
        return logic();
    }

protected:
    LexScanner& scanner;
};