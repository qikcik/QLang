#pragma once

#include <memory>

#include "LexScanner.hpp"
#include "AstNode.hpp"

class AstParser
{
public:
    explicit AstParser(LexScanner& inScanner) : scanner(inScanner) {};

    //<identifier>
    std::unique_ptr<AstNode::Any> identifier()
    {
        if(const auto v = scanner.current<LexToken::Label>())
        {
            scanner.next();
            return std::make_unique<AstNode::Any>( AstNode::Identifier{*v} );
        }

        std::cout << "\nCRITICAL INTERPRETER ERROR: couldn't parse as identifier " << (scanner.current() ? LexToken::printHint(*scanner.current()) : "'in the end of file'") << " unexpected token" << std::endl;
        throw std::runtime_error("");
    }

    //<primary> ::= <identifier> | <integer> | <float> | <string> |  <bool> as ('true'|'false') | '(' <expr> ')'
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
        else if(scanner.current<LexToken::Label>())
        {
            return std::move(identifier());
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
                std::cout << "\nCRITICAL INTERPRETER ERROR: expected closing parentheses, opened " << LexToken::printHint(*open) << " not found closing ')'" << std::endl;
                throw std::runtime_error("");
            }

            return e;
        }
        std::cout << "\nCRITICAL INTERPRETER ERROR: couldn't parse as primary " << (scanner.current() ? LexToken::printHint(*scanner.current()) : "'in the end of file'") << " unexpected token" << std::endl;
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

    //<assigment> ::= <identifier> ':=' <expr>
    std::unique_ptr<AstNode::Any> assigment()
    {
        if (auto t= scanner.current<LexToken::Label>() ) // <assigment>
        {
            auto id = std::move(identifier());

            auto el = scanner.currentMath<LexToken::Separator>(":=");
            if (!el)
            {
                std::cout << "\nCRITICAL INTERPRETER ERROR: expected assing operator " << (scanner.current() ? LexToken::printHint(*scanner.current()) : "end of file") << " not found ':='" << std::endl;
                throw std::runtime_error("");
            }
            scanner.next();

            auto ex = std::move(expr());

            return std::make_unique<AstNode::Any>(AstNode::AssignStmt{*el,*id|vx::as<AstNode::Identifier>,std::move(ex)});
        }
        std::cout << "\nCRITICAL INTERPRETER ERROR: couldn't parse as assigment " << (scanner.current() ? LexToken::printHint(*scanner.current()) : "'in the end of file'") << " unexpected token" << std::endl;
        throw std::runtime_error("");
    }

    //<stmt> ::= 'print' <expr> | 'if' <expr> <stmt> ( 'else' <stmt> )? | 'while' <expr> <stmt> | 'for' '(' <assigment> ',' <expr> ',' <assigment> ')'  <stmt> | <identifier> := <expr> | '{' <stmt>* '}'
    std::unique_ptr<AstNode::Any> stmt()
    {
        if (auto t= scanner.currentMath<LexToken::Label>("print") )
        {
            scanner.next();
            auto e = std::move(expr());
            return std::make_unique<AstNode::Any>(AstNode::PrintStmt{*t,std::move(e)});
        }
        else if (auto t= scanner.currentMath<LexToken::Label>("if") )
        {
            scanner.next();
            auto ex = std::move(expr());
            auto st = std::move(stmt());

            if (auto el = scanner.currentMath<LexToken::Label>("else") )
            {
                scanner.next();
                auto elSt = std::move(stmt());
                return std::make_unique<AstNode::Any>(AstNode::IfStmt{*t,std::move(ex),std::move(st),std::move(elSt)});
            }
            return std::make_unique<AstNode::Any>(AstNode::IfStmt{*t,std::move(ex),std::move(st),nullptr});
        }
        else if (auto t= scanner.currentMath<LexToken::Label>("while") )
        {
            scanner.next();
            auto until = std::move(expr());
            auto loop = std::move(stmt());
            return std::make_unique<AstNode::Any>(AstNode::WhileStmt{*t,std::move(until),std::move(loop)});
        }
        else if (auto t= scanner.currentMath<LexToken::Label>("for") )
        {
            scanner.next();

            if (auto el = scanner.currentMath<LexToken::Separator>("(") ; !el )
            {
                std::cout << "\nCRITICAL INTERPRETER ERROR: expected '(' if for loop statement " << (scanner.current() ? LexToken::printHint(*scanner.current()) : "end of file") << std::endl;
                throw std::runtime_error("");
            }
            scanner.next();

            auto doOnce = std::move(assigment());

            if (auto el = scanner.currentMath<LexToken::Separator>(",") ; !el )
            {
                std::cout << "\nCRITICAL INTERPRETER ERROR: expected ',' if for loop statement " << (scanner.current() ? LexToken::printHint(*scanner.current()) : "end of file") << std::endl;
                throw std::runtime_error("");
            }
            scanner.next();

            auto until = std::move(expr());

            if (auto el = scanner.currentMath<LexToken::Separator>(",") ; !el )
            {
                std::cout << "\nCRITICAL INTERPRETER ERROR: expected ',' if for loop statement " << (scanner.current() ? LexToken::printHint(*scanner.current()) : "end of file") << std::endl;
                throw std::runtime_error("");
            }
            scanner.next();

            auto after = std::move(assigment());

            if (auto el = scanner.currentMath<LexToken::Separator>(")") ; !el )
            {
                std::cout << "\nCRITICAL INTERPRETER ERROR: expected ')' if for loop statement " << (scanner.current() ? LexToken::printHint(*scanner.current()) : "end of file") << std::endl;
                throw std::runtime_error("");
            }
            scanner.next();

            auto st = std::move(stmt());

            return std::make_unique<AstNode::Any>(AstNode::ForStmt{*t,std::move(doOnce),std::move(until),std::move(after),std::move(st)});
        }
        else if (auto t= scanner.current<LexToken::Label>() ) // <assigment>
        {
            return std::move(assigment());
        }

        if (auto t= scanner.currentMath<LexToken::Separator>("{") )
        {
            scanner.next();
            std::vector<std::unique_ptr<AstNode::Any>> statements;
            while (scanner.current() && !scanner.currentMath<LexToken::Separator>("}"))
            {
                statements.push_back(std::move(stmt()));
            }
            if (scanner.currentMath<LexToken::Separator>("}") )
            {
                scanner.next();
                return std::make_unique<AstNode::Any>(AstNode::Block{std::move(statements)});
            }
            std::cout << "\nCRITICAL INTERPRETER ERROR: expected closing parentheses, opened " << LexToken::printHint(*t) << " not found closing '}'" << std::endl;
            throw std::runtime_error("");
        }

        std::cout << "\nCRITICAL INTERPRET ERROR: couldn't parse as stmt " << (scanner.current() ? LexToken::printHint(*scanner.current()) : "'in the end of file'") << " unexpected token" << std::endl;
        throw std::runtime_error("");
    }

    //<block> ::= <stmt>*
    std::unique_ptr<AstNode::Any> block()
    {
        std::vector<std::unique_ptr<AstNode::Any>> statements;

        while (scanner.current() && !scanner.currentMath<LexToken::Separator>("}"))
        {
            statements.push_back(std::move(stmt()));
        }

        return std::make_unique<AstNode::Any>(AstNode::Block{std::move(statements)});
    }

protected:
    LexScanner& scanner;
};