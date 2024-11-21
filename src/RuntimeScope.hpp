#pragma once
#include <map>
#include <memory>
#include <vector>

#include "TemporaryValue.hpp"

struct RuntimeScope
{
    explicit RuntimeScope(std::unique_ptr<RuntimeScope> inParent) : parent(std::move(inParent)) {};

    TemporaryValue::Any* getVariable(std::string inName)
    {
        if(variables.find(inName) != variables.end())
        {
            return &variables[inName];
        }
        if(parent)
        {
            return parent->getVariable(inName);
        }
        return nullptr;
    }

    std::map<std::string,TemporaryValue::Any> variables {};
    std::unique_ptr<RuntimeScope> parent;
};
