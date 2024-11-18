#include "AstParser.hpp"

std::unique_ptr<AstNode> AstParser::primary()
{
    if(const auto v = scanner.current<LexToken::Integer>())
    {
        scanner.next();
        return std::make_unique<AstInteger>( v->value );
    }
    if(const auto v =scanner.current<LexToken::Float>())
    {
        scanner.next();
        return std::make_unique<AstInteger>( v->value );
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
    return {};
}

std::unique_ptr<AstNode> AstParser::unary()
{
    if(scanner.currentMath<LexToken::Separator>("~") || scanner.currentMath<LexToken::Separator>("+") || scanner.currentMath<LexToken::Separator>("-"))
    {
        auto op = scanner.current<LexToken::Separator>()->value;
        scanner.next();

        auto inner = std::move(unary());
        return std::make_unique<AstUnary>( op, std::move(inner) );
    }
    return primary();
}

std::unique_ptr<AstNode> AstParser::factor()
{
    auto e = unary();
    while (scanner.currentMath<LexToken::Separator>("*") || scanner.currentMath<LexToken::Separator>("/") )
    {

        auto op = scanner.current<LexToken::Separator>()->value;
        scanner.next();

        auto right = std::move(unary());
        e = std::make_unique<AstMathOp>(op, std::move(e), std::move(right));

    }
    return std::move(e);
}

std::unique_ptr<AstNode> AstParser::term()
{
    auto e = factor();

    while (scanner.currentMath<LexToken::Separator>("+") || scanner.currentMath<LexToken::Separator>("-") )
    {
        auto op = scanner.current<LexToken::Separator>()->value;
        scanner.next();

        auto right = std::move(factor());
        e = std::make_unique<AstMathOp>(op, std::move(e), std::move(right));

    }
    return std::move(e);
}

std::unique_ptr<AstNode> AstParser::expr()
{
    return term();
}
