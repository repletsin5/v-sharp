#pragma once

#include <vector>
#include <memory>
#include <variant>
#include <string>
#include <cstdint>

enum class Type
{
    Void,
    Boolean,
    Byte,
    String,
    Int8,
    Int16,
    Int32,
    Int64,
    Uint8,
    Uint16,
    Uint32,
    Uint64,
    Float32,
    Float64
};

enum class ASTNodeType
{
    Program,
    Block,

    VarDecl,
    ConstDelc,
    FunctionDecl,
    ReturnExpr,
    IfExpr,
    ForExpr,
    MatchExpr,
    BinaryExpr,
    UnaryExpr,
    Literal,
    Identifier,
    FunctionCall,
    AssignExpr,
};

struct ASTNode
{
    ASTNodeType type;
    ASTNode(ASTNodeType t) : type(t) {}
    virtual ~ASTNode() = default;
};

using ASTNodePtr = std::unique_ptr<ASTNode>;
using ASTNodeList = std::vector<ASTNodePtr>;

using LiteralValue = std::variant<int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t, float, double, bool, char, std::string>;

struct BlockNode : ASTNode
{
    ASTNodeList children;
    BlockNode() : ASTNode(ASTNodeType::Block) {}
};

struct LiteralNode : ASTNode
{
    Type literalType;
    LiteralValue value;
    LiteralNode(Type t, LiteralValue v)
        : ASTNode(ASTNodeType::Literal), literalType(t), value(std::move(v)) {}
};

struct TypeNode : ASTNode
{
    Type type;
    TypeNode(Type t) : ASTNode(ASTNodeType::Identifier), type(t) {}
};

struct IdentifierNode : ASTNode
{
    std::string name;
    IdentifierNode(std::string n) : ASTNode(ASTNodeType::Identifier), name(std::move(n)) {}
};

struct BinaryExprNode : ASTNode
{
    std::string op;
    ASTNodePtr left;
    ASTNodePtr right;

    BinaryExprNode(std::string o, ASTNodePtr l, ASTNodePtr r)
        : ASTNode(ASTNodeType::BinaryExpr), op(std::move(o)), left(std::move(l)), right(std::move(r)) {}
};

struct FunctionDeclNode : ASTNode
{
    std::string access;
    std::string name;
    std::vector<std::pair<Type, std::string>> params;
    Type returnType;
    ASTNodePtr body;

    FunctionDeclNode(std::string access, std::string name, std::vector<std::pair<Type, std::string>> params, Type returnType, ASTNodePtr body)
        : ASTNode(ASTNodeType::FunctionDecl), access(std::move(access)), name(std::move(name)), params(std::move(params)), returnType(returnType), body(std::move(body)) {}
};

struct ReturnExprNode : ASTNode
{
    ASTNodePtr expr;
    ReturnExprNode(ASTNodePtr e) : ASTNode(ASTNodeType::ReturnExpr), expr(std::move(e)) {}
};

struct VarDeclNode : ASTNode
{
    bool isConst;
    std::string name;
    Type varType;
    ASTNodePtr value;

    VarDeclNode(bool isConst, std::string n, Type t, ASTNodePtr v)
        : ASTNode(ASTNodeType::VarDecl), isConst(isConst), name(std::move(n)), varType(t), value(std::move(v)) {}
};

struct IfExprNode : ASTNode
{
    ASTNodePtr condition;
    ASTNodePtr thenBranch;
    ASTNodePtr elseBranch;

    IfExprNode(ASTNodePtr cond, ASTNodePtr thenB, ASTNodePtr elseB = nullptr)
        : ASTNode(ASTNodeType::IfExpr), condition(std::move(cond)), thenBranch(std::move(thenB)), elseBranch(std::move(elseB)) {}
};

struct AssignExprNode : ASTNode
{
    std::string name;
    ASTNodePtr value;

    AssignExprNode(std::string name, ASTNodePtr value)
        : ASTNode(ASTNodeType::AssignExpr), name(std::move(name)), value(std::move(value)) {}
};

void printAST(const ASTNode *node, int indent = 0);