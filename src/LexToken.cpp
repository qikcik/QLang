#include "LexToken.hpp"
#include <ostream>
#include <vx.hpp>

std::ostream& operator<<(std::ostream& os, const LexToken::Source& in)
{
    os << in.fromSource << ":" << in.atLine << " at char " << in.startingCharacter;
    return os;
}

std::ostream& operator<<(std::ostream& os, const LexToken::Any& in)
{
    in |vx::match {
        [&os,&in](const LexToken::Separator& v)   { os << "LexToken::Separator{" << v.content << "} at " << v.source; },
        [&os,&in](const LexToken::Label& v)       { os << "LexToken::Label{" << v.content << "} at " << v.source; },
        [&os,&in](const LexToken::String& v)      { os << "LexToken::String{" << v.content << "} at " << v.source; },
        [&os,&in](const LexToken::Integer& v)     { os << "LexToken::Integer{" << v.content << "} at " << v.source; },
        [&os,&in](const LexToken::Float& v)       { os << "LexToken::Float{" << v.content << "} at " << v.source; },
    };

    return os;
}

std::ostream& operator<<(std::ostream& os, const std::optional<LexToken::Any>& in)
{
    if(in)
        os << *in;
    else
        os << "<nullopt>";
    return os;
}