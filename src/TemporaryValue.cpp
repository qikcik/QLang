
#include "TemporaryValue.hpp"
#include <ostream>
#include "vx.hpp"

std::ostream& operator<<(std::ostream& os, const TemporaryValue::Any& in)
{
    in |vx::match {
        [&os,&in](const TemporaryValue::Bool& v)       { os << "TemporaryValue::Bool{" << (v.value ? "true" : "false") << "}";},
        [&os,&in](const TemporaryValue::Integer& v)    { os << "TemporaryValue::Integer{" << v.value << "}";},
        [&os,&in](const TemporaryValue::Float& v)      { os << "TemporaryValue::Float{" << v.value << "}";},
        [&os,&in](const TemporaryValue::String& v)     { os << "TemporaryValue::String{" << v.value << "}";}
    };
    return os;
}

float TemporaryValue::getFloat(TemporaryValue::Any& in)
{
    if(in|vx::is<Integer>)
    {
        std::cout << "WRN: context required conversion to Float from " << in << "'\n";
        return static_cast<float>((in|vx::as<Integer>).value);
    }
    if(in|vx::is<Float>)
        return (in|vx::as<Float>).value;

    std::cout << "unsupported conversion to Float from:'" << in << "'\n";
    throw std::runtime_error("conversion error");
}

std::string TemporaryValue::getString(TemporaryValue::Any& in)
{
    if(in|vx::is<Integer>)
    {
        //std::cout << "WRN: context required conversion to String from " << in << "'\n";
        return std::to_string((in|vx::as<Integer>).value);
    }
    if(in|vx::is<Bool>)
    {
        //std::cout << "WRN: context required conversion to String from " << in << "'\n";
        return (in|vx::as<Bool>).value ? "true" : "false";
    }
    if(in|vx::is<Float>)
    {
        //std::cout << "WRN: context required conversion to String from " << in << "'\n";
        return std::to_string((in|vx::as<Float>).value);
    }
    if(in|vx::is<String>)
        return (in|vx::as<String>).value;

    std::cout << "unsupported conversion to Float from:'" << in << "'\n";
    throw std::runtime_error("conversion error");
}

int TemporaryValue::getInteger(TemporaryValue::Any& in)
{
    if(in|vx::is<Integer>)
        return static_cast<float>((in|vx::as<Integer>).value);
    if(in|vx::is<Float>)
    {
        std::cout << "WRN: context required conversion to Integer from " << in << "'\n";
        return static_cast<int>((in|vx::as<Float>).value);
    }

    std::cout << "unsupported conversion to Float from:'" << in << "'\n";
    throw std::runtime_error("conversion error");
}

bool TemporaryValue::getBool(TemporaryValue::Any& in)
{
    if(in|vx::is<Bool>)
        return (in|vx::as<Bool>).value;

    std::cout << "unsupported conversion to Bool from:'" << in << "'\n";
    throw std::runtime_error("conversion error");
}