#include "RuntimeScope.hpp"

void RuntimeScope::addNew(std::unique_ptr<RuntimeScope>* in)
{
    *in = std::make_unique<RuntimeScope>(std::move(*in));
}

void RuntimeScope::removeLast(std::unique_ptr<RuntimeScope>* in)
{
    if(*in==nullptr)
        return;

    auto p = std::move((*in)->parent);
    *in = std::move(p);
}

