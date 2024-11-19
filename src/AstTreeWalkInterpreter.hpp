#pragma once
#include "AstNode.hpp"


std::unique_ptr<AstNode> treeWallInterpret(AstNode* in)
{
    if (auto* v = dynamic_cast<AstFloat*>(in))
    {
        return std::make_unique<AstFloat>(*v);
    }
    else if (auto* v = dynamic_cast<AstInteger*>(in))
    {
        return std::make_unique<AstInteger>(*v);
    }
    else if (auto* v = dynamic_cast<AstUnaryOp*>(in))
    {
        if(v->operation.content == "-")
        {
            if(auto interpreted = treeWallInterpret(v->inner.get()))
            {
                if (auto* i = dynamic_cast<AstFloat*>(interpreted.get()))
                {
                    return std::make_unique<AstFloat>( LexToken::Float{i->tokenValue.source,-i->tokenValue.content} );
                }
                else if (auto* i = dynamic_cast<AstInteger*>(interpreted.get()))
                {
                    return std::make_unique<AstInteger>( LexToken::Integer{i->tokenValue.source,-i->tokenValue.content} );
                }
            }
        }
    }

    else if (auto* v = dynamic_cast<AstMathOp*>(in))
    {
        auto left = treeWallInterpret(v->left.get());
        auto right = treeWallInterpret(v->right.get());

        if (auto* l = dynamic_cast<AstInteger*>(left.get()))
        {
            if (auto* r = dynamic_cast<AstInteger*>(right.get()))
            {
                if(v->operation.content == "+")
                    return std::make_unique<AstInteger>( LexToken::Integer{l->tokenValue.source,l->tokenValue.content+r->tokenValue.content} );
                if(v->operation.content == "-")
                    return std::make_unique<AstInteger>( LexToken::Integer{l->tokenValue.source,l->tokenValue.content-r->tokenValue.content} );
                if(v->operation.content == "*")
                    return std::make_unique<AstInteger>( LexToken::Integer{l->tokenValue.source,l->tokenValue.content*r->tokenValue.content} );
                if(v->operation.content == "/")
                    return std::make_unique<AstInteger>( LexToken::Integer{l->tokenValue.source,l->tokenValue.content/r->tokenValue.content} );
            }
            if (auto* r = dynamic_cast<AstFloat*>(right.get()))
            {
                if(v->operation.content == "+")
                    return std::make_unique<AstFloat>( LexToken::Float{l->tokenValue.source,l->tokenValue.content+r->tokenValue.content} );
                if(v->operation.content == "-")
                    return std::make_unique<AstFloat>( LexToken::Float{l->tokenValue.source,l->tokenValue.content-r->tokenValue.content} );
                if(v->operation.content == "*")
                    return std::make_unique<AstFloat>( LexToken::Float{l->tokenValue.source,l->tokenValue.content*r->tokenValue.content} );
                if(v->operation.content == "/")
                    return std::make_unique<AstFloat>( LexToken::Float{l->tokenValue.source,l->tokenValue.content/r->tokenValue.content} );
            }
        }
        if (auto* l = dynamic_cast<AstFloat*>(left.get()))
        {
            if (auto* r = dynamic_cast<AstInteger*>(right.get()))
            {
                if(v->operation.content == "+")
                    return std::make_unique<AstFloat>( LexToken::Float{l->tokenValue.source,l->tokenValue.content+r->tokenValue.content} );
                if(v->operation.content == "-")
                    return std::make_unique<AstFloat>( LexToken::Float{l->tokenValue.source,l->tokenValue.content-r->tokenValue.content} );
                if(v->operation.content == "*")
                    return std::make_unique<AstFloat>( LexToken::Float{l->tokenValue.source,l->tokenValue.content*r->tokenValue.content} );
                if(v->operation.content == "/")
                    return std::make_unique<AstFloat>( LexToken::Float{l->tokenValue.source,l->tokenValue.content/r->tokenValue.content} );
            }
            if (auto* r = dynamic_cast<AstFloat*>(right.get()))
            {
                if(v->operation.content == "+")
                    return std::make_unique<AstFloat>( LexToken::Float{l->tokenValue.source,l->tokenValue.content+r->tokenValue.content} );
                if(v->operation.content == "-")
                    return std::make_unique<AstFloat>( LexToken::Float{l->tokenValue.source,l->tokenValue.content-r->tokenValue.content} );
                if(v->operation.content == "*")
                    return std::make_unique<AstFloat>( LexToken::Float{l->tokenValue.source,l->tokenValue.content*r->tokenValue.content} );
                if(v->operation.content == "/")
                    return std::make_unique<AstFloat>( LexToken::Float{l->tokenValue.source,l->tokenValue.content/r->tokenValue.content} );
            }
        }
    }

    throw std::runtime_error("invalid op");
}
