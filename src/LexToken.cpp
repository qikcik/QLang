#include "LexToken.hpp"
#include <ostream>
#include <vx.hpp>

std::string LexToken::Source::stringify() const
{
    return fromSource->name + ":" + std::to_string(atLine) + ":" + std::to_string(startingCharacter);
}

std::string LexToken::Source::printHint() const
{
    return fromSource->printHint(atLine, startingCharacter);
}

std::string LexToken::printHint(const Any& in)
{
    return std::visit([](auto& v)
    {
        return v.source.printHint();
    },in);
}

std::ostream& operator<<(std::ostream& os, const LexToken::Source& in)
{
    os << in.stringify();
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