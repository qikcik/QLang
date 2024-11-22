#include "AstNode.hpp"

AstNode::Any AstNode::Identifier::copy() const
{return Identifier(tokenValue); }

AstNode::Any AstNode::Integer::copy() const
{return Integer(tokenValue); }

AstNode::Any AstNode::Float::copy() const
{return Float(tokenValue); }

AstNode::Any AstNode::Bool::copy() const
{return Bool(tokenValue); }

AstNode::Any AstNode::String::copy() const
{return String(tokenValue); }

AstNode::Any AstNode::UnaryOp::copy() const
{return UnaryOp(tokenValue,std::make_unique<Any>(std::visit([](auto& v){ return v.copy();},*inner))); }

AstNode::Any AstNode::BinaryOp::copy() const
{return BinaryOp(tokenValue,
                 std::make_unique<Any>(std::visit([](auto& v){ return v.copy();},*left)),
                 std::make_unique<Any>(std::visit([](auto& v){ return v.copy();},*right))
); }

AstNode::Any AstNode::Block::copy() const
{
    auto block = Block({});
    for (const auto& statement: statements)
    {
        block.statements.push_back(std::make_unique<Any>(std::visit([](auto& v){ return v.copy();},*statement)));
    }
    return block;
}

AstNode::Any AstNode::PrintStmt::copy() const
{return PrintStmt(tokenValue,
                  std::make_unique<Any>(std::visit([](auto& v){ return v.copy();},*inner))
); }

AstNode::Any AstNode::IfStmt::copy() const
{return IfStmt(tokenValue,
               std::make_unique<Any>(std::visit([](auto& v){ return v.copy();},*when)),
               std::make_unique<Any>(std::visit([](auto& v){ return v.copy();},*then)),
               elseThen ? std::make_unique<Any>(std::visit([](auto& v){ return v.copy();},*elseThen)) : nullptr
); }

AstNode::Any AstNode::AssignStmt::copy() const
{return AssignStmt(tokenValue,
                   identifier,
                   std::make_unique<Any>(std::visit([](auto& v){ return v.copy();},*value))
); }

AstNode::Any AstNode::WhileStmt::copy() const
{return WhileStmt(tokenValue,
                  std::make_unique<Any>(std::visit([](auto& v){ return v.copy();},*until)),
                  std::make_unique<Any>(std::visit([](auto& v){ return v.copy();},*loop))
); }

AstNode::Any AstNode::ForStmt::copy() const
{return ForStmt(tokenValue,
                std::make_unique<Any>(std::visit([](auto& v){ return v.copy();},*doOnce)),
                std::make_unique<Any>(std::visit([](auto& v){ return v.copy();},*until)),
                std::make_unique<Any>(std::visit([](auto& v){ return v.copy();},*afterIter)),
                std::make_unique<Any>(std::visit([](auto& v){ return v.copy();},*loop))
); }

AstNode::Any AstNode::FunctionDecl::copy() const
{
    auto ret =  FunctionDecl(tokenValue,{},std::make_unique<Any>(std::visit([](auto& v){ return v.copy();},*body)));
    for (const auto& p: params)
    {
        ret.params.push_back(p);
    }
    return ret;
}

AstNode::Any AstNode::FunctionCall::copy() const
{
    auto ret =  FunctionCall(tokenValue,name,{});
    for (const auto& p: params)
    {
        ret.params.push_back(std::visit([](auto& v){ return v.copy();},p));
    }
    return ret;
}

std::string AstNode::stringify(const AstNode::Any& in, int intend)
{
    std::string result;
    for(int i=0;i!=intend;i++) result+="\t";

    result += in |vx::match {
        [&in](const AstNode::Identifier& v)
        { return "Identifier{"+v.tokenValue.content+"} at "+v.tokenValue.source.stringify(); },
        [&in](const AstNode::Integer& v)
        { return "Integer{"+std::to_string(v.tokenValue.content)+"} at "+v.tokenValue.source.stringify(); },
        [&in](const AstNode::Float& v)
        {return "Float{"+std::to_string(v.tokenValue.content)+"} at "+v.tokenValue.source.stringify(); },
        [&in](const AstNode::Bool& v)
        { return "Bool{"+v.tokenValue.content+"} at "+v.tokenValue.source.stringify(); },
        [&in](const AstNode::String& v)
        { return "String{"+v.tokenValue.content+"} at "+v.tokenValue.source.stringify(); },
        [&in,intend](const AstNode::UnaryOp& v)
        {
            std::string result = "UnaryOp{\n";

            for(int i=0;i!=intend+1;i++) result+="\t";
            result += v.tokenValue.content+" at "+v.tokenValue.source.stringify()+"\n";

            result += stringify(*v.inner, intend+1)+"\n";
            for(int i=0;i!=intend;i++) result+="\t";
            return result + "}";
        },
        [&in,intend](const AstNode::BinaryOp& v)
        {
            std::string result = "BinaryOp{\n";

            for(int i=0;i!=intend+1;i++) result+="\t";
            result += v.tokenValue.content+" at "+v.tokenValue.source.stringify()+"\n";

            result += stringify(*v.left, intend+1)+",\n";
            result += stringify(*v.right, intend+1)+"\n";
            for(int i=0;i!=intend;i++) result+="\t";
            return result + "}";
        },
        [&in,intend](const AstNode::Block& v)
        {
            std::string result = "Block{\n";
            for(auto& i : v.statements)
            {
                result += stringify(*i, intend+1)+",\n";
            }
            for(int i=0;i!=intend;i++) result+="\t";
            return result + "}";
        },
        [&in,intend](const AstNode::PrintStmt& v)
        {
            std::string result = "PrintStmt{\n";

            for(int i=0;i!=intend+1;i++) result+="\t";
            result += v.tokenValue.content+" at "+v.tokenValue.source.stringify()+"\n";

            result += stringify(*v.inner, intend+1)+"\n";
            for(int i=0;i!=intend;i++) result+="\t";
            return result + "}";
        },
        [&in,intend](const AstNode::IfStmt& v)
        {
            std::string result = "IfStmt{\n";

            for(int i=0;i!=intend+1;i++) result+="\t";
            result += v.tokenValue.content+" at "+v.tokenValue.source.stringify()+"\n";

            result += stringify(*v.when, intend+1)+",\n";
            result += stringify(*v.then, intend+1)+"\n";
            if(v.elseThen)
                result += stringify(*v.elseThen, intend+1)+"\n";
            for(int i=0;i!=intend;i++) result+="\t";
            return result + "}";
        },
        [&in,intend](const AstNode::AssignStmt& v)
        {
            std::string result = "AssignStmt{\n";

            for(int i=0;i!=intend+1;i++) result+="\t";
            result += v.tokenValue.content+" at "+v.tokenValue.source.stringify()+"\n";

            result += stringify(v.identifier, intend+1)+",\n";
            result += stringify(*v.value, intend+1)+"\n";
            for(int i=0;i!=intend;i++) result+="\t";
            return result + "}";
        },
        [&in,intend](const AstNode::WhileStmt& v)
        {
            std::string result = "WhileStmt{\n";

            for(int i=0;i!=intend+1;i++) result+="\t";
            result += v.tokenValue.content+" at "+v.tokenValue.source.stringify()+"\n";

            result += stringify(*v.until, intend+1)+",\n";
            result += stringify(*v.loop, intend+1)+"\n";
            for(int i=0;i!=intend;i++) result+="\t";
            return result + "}";
        },
        [&in,intend](const AstNode::ForStmt& v)
        {
            std::string result = "ForStmt{\n";

            for(int i=0;i!=intend+1;i++) result+="\t";
            result += v.tokenValue.content+" at "+v.tokenValue.source.stringify()+"\n";

            result += stringify(*v.doOnce, intend+1)+",\n";
            result += stringify(*v.until, intend+1)+"\n";
            result += stringify(*v.afterIter, intend+1)+"\n";
            result += stringify(*v.loop, intend+1)+"\n";
            for(int i=0;i!=intend;i++) result+="\t";
            return result + "}";
        },
        [&in,intend](const AstNode::FunctionDecl& v)
        {
            std::string result = "FunctionDecl{\n";

            for(int i=0;i!=intend+1;i++) result+="\t";
            result += v.tokenValue.content+" at "+v.tokenValue.source.stringify()+"\n";

            for(int i=0;i!=intend+1;i++) result+="\t"; result += "params: [";
            for(auto& it : v.params) result += it.tokenValue.content + ",";
            result += "]\n";

            result += stringify(*v.body, intend+1)+",\n";
            for(int i=0;i!=intend;i++) result+="\t";
            return result + "}";
        },
        [&in,intend](const AstNode::FunctionCall& v)
        {
            std::string result = "FunctionCall{\n";

            for(int i=0;i!=intend+1;i++) result+="\t";
            result += v.tokenValue.content+" at "+v.tokenValue.source.stringify()+"\n";

            for(auto& it : v.params) result += stringify(it);

            for(int i=0;i!=intend;i++) result+="\t";
            return result + "}";
        },
    };

    return result;
}
