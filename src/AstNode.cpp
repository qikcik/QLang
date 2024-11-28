#include "AstNode.hpp"

#include "vx.hpp"

AstNode::OwnedNode AstNode::Identifier::copy() const
{return std::make_unique<Identifier>(tokenValue);  }

AstNode::OwnedNode AstNode::Integer::copy() const
{return std::make_unique<Integer>(tokenValue); }

AstNode::OwnedNode AstNode::Float::copy() const
{return std::make_unique<Float>(tokenValue); }

AstNode::OwnedNode AstNode::Bool::copy() const
{return std::make_unique<Bool>(tokenValue); }

AstNode::OwnedNode AstNode::String::copy() const
{return std::make_unique<String>(tokenValue); }

AstNode::OwnedNode AstNode::UnaryOp::copy() const
{return std::make_unique<UnaryOp>(tokenValue,inner->copy()); }

AstNode::OwnedNode AstNode::BinaryOp::copy() const
{return std::make_unique<BinaryOp>(tokenValue,left->copy(),right->copy());}

AstNode::OwnedNode AstNode::Block::copy() const
{
    auto block = std::make_unique<Block>(std::vector<AstNode::OwnedNode>{});
    for (const auto& statement: statements)
    {
        block->statements.push_back(statement->copy());
    }
    return block;
}

AstNode::OwnedNode AstNode::PrintStmt::copy() const
{return std::make_unique<PrintStmt>(tokenValue,inner->copy()); }

AstNode::OwnedNode AstNode::IfStmt::copy() const
{return std::make_unique<IfStmt>(tokenValue,when->copy(),then->copy(),elseThen->copy());}

AstNode::OwnedNode AstNode::AssignStmt::copy() const
{return std::make_unique<AssignStmt>(tokenValue,identifier->copy(),value->copy()); }

AstNode::OwnedNode AstNode::WhileStmt::copy() const
{return std::make_unique<WhileStmt>(tokenValue,until->copy(),loop->copy());}

AstNode::OwnedNode AstNode::ForStmt::copy() const
{return std::make_unique<ForStmt>(tokenValue,doOnce->copy(),until->copy(),afterIter->copy(),loop->copy());}

AstNode::OwnedNode AstNode::FunctionDecl::copy() const
{
    auto ret = std::make_unique<FunctionDecl>(tokenValue,std::vector<OwnedNode>{},body->copy());
    for (const auto& p: params)
    {
        ret->params.push_back(p->copy());
    }
    return ret;
}

AstNode::OwnedNode AstNode::FunctionCall::copy() const
{
    auto ret = std::make_unique<FunctionCall>(tokenValue,name->copy(),std::vector<OwnedNode>{});
    for (const auto& p: args)
    {
        ret->args.push_back(p->copy());
    }
    return ret;
}

AstNode::OwnedNode AstNode::Return::copy() const
{return std::make_unique<Return>(tokenValue,inner->copy()); }

struct PrinterVisitor : public AstNode::IVisitor
{

    std::string& result;
    int intend {};

    PrinterVisitor(std::string& result, int intend)
        : result(result),
          intend(intend)
    {
    }

    void operator()(const AstNode::Identifier& v) override
    {
        result = "Identifier{"+v.tokenValue.content+"} at "+v.tokenValue.source.stringify();
    }

    void operator()(const AstNode::Integer& v) override
    {
        result = "Integer{"+std::to_string(v.tokenValue.content)+"} at "+v.tokenValue.source.stringify();
    }
    void operator()(const AstNode::Float& v) override
    {
        result = "Float{"+std::to_string(v.tokenValue.content)+"} at "+v.tokenValue.source.stringify();
    }
    void operator()(const AstNode::String& v) override
    {
        result  ="String{"+v.tokenValue.content+"} at "+v.tokenValue.source.stringify();
    }

    void operator()(const AstNode::Bool& v) override
    {
        result = "Bool{"+v.tokenValue.content+"} at "+v.tokenValue.source.stringify();
    }
    void operator()(const AstNode::UnaryOp& v) override
    {
        result = "UnaryOp{\n";

        for(int i=0;i!=intend+1;i++) result+="\t";
        result += v.tokenValue.content+" at "+v.tokenValue.source.stringify()+"\n";

        result += stringify(*v.inner, intend+1)+"\n";
        for(int i=0;i!=intend;i++) result+="\t";
        result += "}";
    }
    void operator()(const AstNode::BinaryOp& v) override
    {
        result = "BinaryOp{\n";

        for(int i=0;i!=intend+1;i++) result+="\t";
        result += v.tokenValue.content+" at "+v.tokenValue.source.stringify()+"\n";

        result += stringify(*v.left, intend+1)+",\n";
        result += stringify(*v.right, intend+1)+"\n";
        for(int i=0;i!=intend;i++) result+="\t";
        result += "}";
    }
    void operator()(const AstNode::Block& v) override
    {
        result = "Block{\n";
        for(auto& i : v.statements)
        {
            result += stringify(*i, intend+1)+",\n";
        }
        for(int i=0;i!=intend;i++) result+="\t";
        result += "}";
    }
    void operator()(const AstNode::PrintStmt& v) override
    {
        result = "PrintStmt{\n";

        for(int i=0;i!=intend+1;i++) result+="\t";
        result += v.tokenValue.content+" at "+v.tokenValue.source.stringify()+"\n";

        result += stringify(*v.inner, intend+1)+"\n";
        for(int i=0;i!=intend;i++) result+="\t";
        result += "}";
    }
    void operator()(const AstNode::IfStmt& v) override
    {
        result = "IfStmt{\n";

        for(int i=0;i!=intend+1;i++) result+="\t";
        result += v.tokenValue.content+" at "+v.tokenValue.source.stringify()+"\n";

        result += stringify(*v.when, intend+1)+",\n";
        result += stringify(*v.then, intend+1)+"\n";
        if(v.elseThen)
            result += stringify(*v.elseThen, intend+1)+"\n";
        for(int i=0;i!=intend;i++) result+="\t";
        result += "}";
    }
    void operator()(const AstNode::AssignStmt& v) override
    {
        result = "AssignStmt{\n";

        for(int i=0;i!=intend+1;i++) result+="\t";
        result += v.tokenValue.content+" at "+v.tokenValue.source.stringify()+"\n";

        result += stringify(*v.identifier, intend+1)+",\n";
        result += stringify(*v.value, intend+1)+"\n";
        for(int i=0;i!=intend;i++) result+="\t";
        result += "}";
    }
    void operator()(const AstNode::WhileStmt& v) override
    {
        result = "WhileStmt{\n";

        for(int i=0;i!=intend+1;i++) result+="\t";
        result += v.tokenValue.content+" at "+v.tokenValue.source.stringify()+"\n";

        result += stringify(*v.until, intend+1)+",\n";
        result += stringify(*v.loop, intend+1)+"\n";
        for(int i=0;i!=intend;i++) result+="\t";
        result += "}";
    }
    void operator()(const AstNode::ForStmt& v) override
    {
        result = "ForStmt{\n";

        for(int i=0;i!=intend+1;i++) result+="\t";
        result += v.tokenValue.content+" at "+v.tokenValue.source.stringify()+"\n";

        result += stringify(*v.doOnce, intend+1)+",\n";
        result += stringify(*v.until, intend+1)+"\n";
        result += stringify(*v.afterIter, intend+1)+"\n";
        result += stringify(*v.loop, intend+1)+"\n";
        for(int i=0;i!=intend;i++) result+="\t";
        result += "}";
    }
    void operator()(const AstNode::FunctionDecl& v) override
    {
        result = "FunctionDecl{\n";

        for(int i=0;i!=intend+1;i++) result+="\t";
        result += v.tokenValue.content+" at "+v.tokenValue.source.stringify()+"\n";

        for(int i=0;i!=intend+1;i++) result+="\t"; result += "params: [";
        //for(auto& it : v.params) result += it.tokenValue.content + ",";
        result += "]\n";

        result += stringify(*v.body, intend+1)+",\n";
        for(int i=0;i!=intend;i++) result+="\t";
        result += "}";
    }
    void operator()(const AstNode::FunctionCall& v) override
    {
        result = "FunctionCall{\n";

        for(int i=0;i!=intend+1;i++) result+="\t";
        result += v.tokenValue.content+" at "+v.tokenValue.source.stringify()+"\n";

        for(auto& it : v.args) result += stringify(*it);

        for(int i=0;i!=intend;i++) result+="\t";
        result += "}";
    }
    void operator()(const AstNode::Return& v) override
    {
        result = "Return{\n";

        for(int i=0;i!=intend+1;i++) result+="\t";
        result += v.tokenValue.content+" at "+v.tokenValue.source.stringify()+"\n";

        result += stringify(*v.inner, intend+1)+"\n";
        for(int i=0;i!=intend;i++) result+="\t";
        result += "}";
    }
};

std::string AstNode::stringify(const Base& in, int intend)
{
    auto T = overload {
        [](auto& test) { return "unknown type"; },
    };

    std::string result;
    in.accept(PrinterVisitor(result,intend));
    return result;
}
