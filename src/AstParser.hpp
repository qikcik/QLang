#pragma once

#include <memory>

#include "LexScanner.hpp"
#include "AstNode.hpp"

class AstParser
{
public:
    explicit AstParser(LexScanner& inScanner) : scanner(inScanner) {};

    //<identifier> ::= <label> | <label> '(' <arg>? (',' <arg>)*
    AstNode::OwnedNode identifier()
    {

        if(const auto v = scanner.current<LexToken::Label>())
        {
            scanner.next();
            auto id = std::make_unique<AstNode::Identifier>(*v);


            if(const auto v = scanner.currentMath<LexToken::Separator>("("))
            {
                std::vector<AstNode::OwnedNode> args;
                scanner.next();

                if (auto el = scanner.currentMath<LexToken::Separator>(")"))
                {
                    scanner.next();
                    return std::make_unique<AstNode::FunctionCall>(*v,std::move(id),std::move(args));
                }
                else
                {
                    args.push_back(std::move(expr()));
                    while (scanner.currentMath<LexToken::Separator>(",") )
                    {
                        scanner.next();

                        args.push_back(std::move(expr()));
                    }

                    if (auto el = scanner.currentMath<LexToken::Separator>(")") ; !el )
                    {
                        std::cout << "\nCRITICAL PARSER ERROR: mising ')' in function declaration " << LexToken::printHint(*v) << "opened here" << std::endl;
                        throw std::runtime_error("");
                    }
                    scanner.next();

                    return std::make_unique<AstNode::FunctionCall>(*v,std::move(id),std::move(args));
                }
            }
            else
            {
                return std::move(id);
            }
        }

        std::cout << "\nCRITICAL PARSER ERROR: couldn't parse as identifier " << (scanner.current() ? LexToken::printHint(*scanner.current()) : "'in the end of file'") << " unexpected token" << std::endl;
        throw std::runtime_error("");
    }

    //<primary> ::= <identifier> | <integer> | <float> | <string> | <function> |  <bool> as ('true'|'false') | '(' <expr> ')'
    AstNode::OwnedNode primary()
    {
        if(const auto v = scanner.current<LexToken::Integer>())
        {
            scanner.next();
            return std::make_unique<AstNode::Integer>(*v);
        }
        if(const auto v =scanner.current<LexToken::Float>())
        {
            scanner.next();
            return std::make_unique<AstNode::Float>(*v);
        }
        if(const auto v =scanner.current<LexToken::String>())
        {
            scanner.next();
            return std::make_unique<AstNode::String>(*v);
        }

        if(scanner.currentMath<LexToken::Label>("true") || scanner.currentMath<LexToken::Label>("false"))
        {
            const auto v = *scanner.current<LexToken::Label>();
            scanner.next();
            return std::make_unique<AstNode::Bool>(v);
        }
        else if(scanner.current<LexToken::Label>())
        {
            return std::move(identifier());
        }
        if(auto open = scanner.currentMath<LexToken::Separator>("fn"))
        {
            return std::move(function());
        }
        if(auto open = scanner.currentMath<LexToken::Separator>("("))
        {
            scanner.next();
            std::unique_ptr<AstNode::Base> e = std::move(expr());
            if(scanner.currentMath<LexToken::Separator>(")"))
            {
                scanner.next();
            }
            else
            {
                std::cout << "\nCRITICAL PARSER ERROR: expected closing parentheses, opened " << LexToken::printHint(*open) << " not found closing ')'" << std::endl;
                throw std::runtime_error("");
            }

            return e;
        }
        std::cout << "\nCRITICAL PARSER ERROR: couldn't parse as primary " << (scanner.current() ? LexToken::printHint(*scanner.current()) : "'in the end of file'") << " unexpected token" << std::endl;
        throw std::runtime_error("");
    }

    //<unary> ::= ('+'|'-'|'!') <unary> | <primary>
    AstNode::OwnedNode unary()
    {
        if(scanner.currentMath<LexToken::Separator>("+") || scanner.currentMath<LexToken::Separator>("-") || scanner.currentMath<LexToken::Separator>("!"))
        {
            auto op = *scanner.current<LexToken::Separator>();
            scanner.next();

            auto inner = std::move(unary());
            return std::make_unique<AstNode::UnaryOp>(op, std::move(inner));
        }
        return primary();
    }

    //<exponent> ::= <unary> ( ('^') <exponent> )*
    AstNode::OwnedNode exponent()
    {
        auto e = unary();
        while (scanner.currentMath<LexToken::Separator>("^") )
        {

            auto op = *scanner.current<LexToken::Separator>();
            scanner.next();

            auto right = std::move(exponent());
            e = std::make_unique<AstNode::BinaryOp>(op, std::move(e), std::move(right));

        }
        return std::move(e);
    }

    //<multiplication> ::= <exponent> ( ('+' | '-') <exponent> )*
    AstNode::OwnedNode multiplication()
    {
        auto e = exponent();
        while (scanner.currentMath<LexToken::Separator>("*") || scanner.currentMath<LexToken::Separator>("/") || scanner.currentMath<LexToken::Separator>("%") )
        {

            auto op = *scanner.current<LexToken::Separator>();
            scanner.next();

            auto right = std::move(exponent());
            e = std::make_unique<AstNode::BinaryOp>(op, std::move(e), std::move(right));

        }
        return std::move(e);
    }

    //<addition> ::= <multiplication> ( ('+' | '-') <multiplication> )*
    AstNode::OwnedNode addition()
    {
        auto e = multiplication();

        while (scanner.currentMath<LexToken::Separator>("+") || scanner.currentMath<LexToken::Separator>("-") )
        {
            auto op = *scanner.current<LexToken::Separator>();
            scanner.next();

            auto right = std::move(multiplication());
            e = std::make_unique<AstNode::BinaryOp>(op, std::move(e), std::move(right));

        }
        return std::move(e);
    }

    //<comparison> ::= <addition> ( ('==' | '!=' | '<' | '>' | '<=' | '>=' ) <addition> )*
    AstNode::OwnedNode comparison()
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
            e = std::make_unique<AstNode::BinaryOp>(op, std::move(e), std::move(right));

        }
        return std::move(e);
    }

    //<logic> ::= <comparison> ( ('&&'|'||') <comparison> )*
    AstNode::OwnedNode logic()
    {
        auto e = comparison();

        while (scanner.currentMath<LexToken::Separator>("&&")
            || scanner.currentMath<LexToken::Separator>("||"))
        {
            auto op = *scanner.current<LexToken::Separator>();
            scanner.next();

            auto right = std::move(comparison());
            e = std::make_unique<AstNode::BinaryOp>(op, std::move(e), std::move(right));

        }
        return std::move(e);
    }


    //<expr> ::= <logic>
    AstNode::OwnedNode expr()
    {
        return logic();
    }

    //<assigment> ::= <expr> ':=' <expr> | <identifier> '(' | <function>
    AstNode::OwnedNode assigment()
    {
        if (auto t= scanner.currentMath<LexToken::Separator>("fn"))
        {
            return std::move(function());
        }
        else if (auto t= scanner.current<LexToken::Label>() ) // <assigment>
        {
            auto id = std::move(expr());

            auto el = scanner.currentMath<LexToken::Separator>(":=");
            if (!el)
            {
                return std::move(id);
            }
            scanner.next();

            auto ex = std::move(expr());

            return std::make_unique<AstNode::AssignStmt>(*el,std::move(id),std::move(ex));
        }
        std::cout << "\nCRITICAL PARSER ERROR: couldn't parse as assigment " << (scanner.current() ? LexToken::printHint(*scanner.current()) : "'in the end of file'") << " unexpected token" << std::endl;
        throw std::runtime_error("");
    }

    //<stmt> ::= 'print' <expr> | 'if' <expr> <stmt> ( 'else' <stmt> )? | 'while' <expr> <stmt> | 'for' '(' <assigment> ',' <expr> ',' <assigment> ')'  <stmt> | <identifier> := <expr> | '{' <stmt>* '}'
    AstNode::OwnedNode stmt()
    {
        if (auto t= scanner.currentMath<LexToken::Label>("print") )
        {
            scanner.next();
            auto e = std::move(expr());
            return std::make_unique<AstNode::PrintStmt>(*t,std::move(e));
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
                return std::make_unique<AstNode::IfStmt>(*t,std::move(ex),std::move(st),std::move(elSt));
            }
            return std::make_unique<AstNode::IfStmt>(*t,std::move(ex),std::move(st),nullptr);
        }
        else if (auto t= scanner.currentMath<LexToken::Label>("while") )
        {
            scanner.next();
            auto until = std::move(expr());
            auto loop = std::move(stmt());
            return std::make_unique<AstNode::WhileStmt>(*t,std::move(until),std::move(loop));
        }
        else if (auto t= scanner.currentMath<LexToken::Label>("for") )
        {
            scanner.next();

            if (auto el = scanner.currentMath<LexToken::Separator>("(") ; !el )
            {
                std::cout << "\nCRITICAL PARSER ERROR: expected '(' if for loop statement " << (scanner.current() ? LexToken::printHint(*scanner.current()) : "end of file") << std::endl;
                throw std::runtime_error("");
            }
            scanner.next();

            auto doOnce = std::move(assigment());

            if (auto el = scanner.currentMath<LexToken::Separator>(",") ; !el )
            {
                std::cout << "\nCRITICAL PARSER ERROR: expected ',' if for loop statement " << (scanner.current() ? LexToken::printHint(*scanner.current()) : "end of file") << std::endl;
                throw std::runtime_error("");
            }
            scanner.next();

            auto until = std::move(expr());

            if (auto el = scanner.currentMath<LexToken::Separator>(",") ; !el )
            {
                std::cout << "\nCRITICAL PARSER ERROR: expected ',' if for loop statement " << (scanner.current() ? LexToken::printHint(*scanner.current()) : "end of file") << std::endl;
                throw std::runtime_error("");
            }
            scanner.next();

            auto after = std::move(assigment());

            if (auto el = scanner.currentMath<LexToken::Separator>(")") ; !el )
            {
                std::cout << "\nCRITICAL PARSER ERROR: expected ')' if for loop statement " << (scanner.current() ? LexToken::printHint(*scanner.current()) : "end of file") << std::endl;
                throw std::runtime_error("");
            }
            scanner.next();

            auto st = std::move(stmt());

            return std::make_unique<AstNode::ForStmt>(*t,std::move(doOnce),std::move(until),std::move(after),std::move(st));
        }
        else if (auto t= scanner.currentMath<LexToken::Label>("ret") )
        {
            scanner.next();
            auto e = std::move(expr());
            return std::make_unique<AstNode::Return>(*t,std::move(e));
        }
        else if (auto t= scanner.current<LexToken::Label>() ) // <assigment>
        {
            return std::move(assigment());
        }

        if (auto t= scanner.currentMath<LexToken::Separator>("{") )
        {
            scanner.next();
            std::vector<AstNode::OwnedNode> statements;
            while (scanner.current() && !scanner.currentMath<LexToken::Separator>("}"))
            {
                statements.push_back(std::move(stmt()));
            }
            if (scanner.currentMath<LexToken::Separator>("}") )
            {
                scanner.next();
                return std::make_unique<AstNode::Block>(std::move(statements));
            }
            std::cout << "\nCRITICAL INTERPRETER ERROR: expected closing parentheses, opened " << LexToken::printHint(*t) << " not found closing '}'" << std::endl;
            throw std::runtime_error("");
        }

        std::cout << "\nCRITICAL PARSER ERROR: couldn't parse as stmt " << (scanner.current() ? LexToken::printHint(*scanner.current()) : "'in the end of file'") << " unexpected token" << std::endl;
        throw std::runtime_error("");
    }

    //<param> ::= <identifier>
    AstNode::OwnedNode param()
    {
        return std::move(identifier());
    }
    //<function> ::= 'fn' '(' <param>? (',' <param>)* ) <stmt>
    AstNode::OwnedNode function()
    {
        auto oP = scanner.currentMath<LexToken::Separator>("fn");
        if (!oP)
        {
            std::cout << "\nCRITICAL PARSER ERROR: expected 'fn' as begining of function declaration " << (scanner.current() ? LexToken::printHint(*scanner.current()) : "end of file") << std::endl;
            throw std::runtime_error("");
        }
        scanner.next();

        std::vector<AstNode::OwnedNode> params;
        if (auto el = scanner.currentMath<LexToken::Separator>("(") ; !el )
        {
            std::cout << "\nCRITICAL PARSER ERROR: expected '(' after function declaration " << (scanner.current() ? LexToken::printHint(*scanner.current()) : "end of file") << std::endl;
            throw std::runtime_error("");
        }
        scanner.next();

        if (auto el = scanner.currentMath<LexToken::Separator>(")"))
        {
            scanner.next();
        }
        else
        {
            params.push_back(std::move(param()));
            while (scanner.currentMath<LexToken::Separator>(",") )
            {
                scanner.next();

                params.push_back(std::move(param()));
            }

            if (auto el = scanner.currentMath<LexToken::Separator>(")") ; !el )
            {
                std::cout << "\nCRITICAL PARSER ERROR: mising ')' in function declaration " << LexToken::printHint(*oP) << "opened here" << std::endl;
                throw std::runtime_error("");
            }
            scanner.next();
        }
        auto st = std::move(stmt());

        return std::make_unique<AstNode::FunctionDecl>(*oP,std::move(params),std::move(st));
    }

    //<block> ::= <stmt>*
    AstNode::OwnedNode block()
    {
        std::vector<AstNode::OwnedNode> statements;

        while (scanner.current() && !scanner.currentMath<LexToken::Separator>("}"))
        {
            statements.push_back(std::move(stmt()));
        }

        return std::make_unique<AstNode::Block>(std::move(statements));
    }

protected:
    LexScanner& scanner;
};