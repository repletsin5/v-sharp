#include <iostream>
#include <parser.hxx>
#include <string.hxx>

void printAST(const ASTNode *node, int indent)
{
    std::string pad(indent, ' ');
    if (!node)
        return;

    switch (node->type)
    {
    case ASTNodeType::Block:
    {
        const BlockNode *blk = static_cast<const BlockNode *>(node);
        std::cout << pad << "Block\n";
        for (const auto &child : blk->children)
            printAST(child.get(), indent + 2);
        break;
    }
    case ASTNodeType::Literal:
    {
        const LiteralNode *lit = static_cast<const LiteralNode *>(node);
        std::cout << pad << "Literal(";
        if (std::holds_alternative<int64_t>(lit->value))
            std::cout << std::get<int64_t>(lit->value);
        else if (std::holds_alternative<double>(lit->value))
            std::cout << std::get<double>(lit->value);
        else if (std::holds_alternative<bool>(lit->value))
            std::cout << (std::get<bool>(lit->value) ? "true" : "false");
        else if (std::holds_alternative<std::string>(lit->value))
            std::cout << std::get<std::string>(lit->value);
        else if (std::holds_alternative<char>(lit->value))
        {
            char c = std::get<char>(lit->value);
            std::cout << '\'';
            switch (c)
            {
            case '\n':
                std::cout << "\\n";
                break;
            case '\t':
                std::cout << "\\t";
                break;
            case '\r':
                std::cout << "\\r";
                break;
            case '\\':
                std::cout << "\\\\";
                break;
            case '\'':
                std::cout << "\\'";
                break;
            default:
                std::cout << c;
                break;
            }
            std::cout << '\'';
        }
        std::cout << ")" << std::endl;
        break;
    }
    case ASTNodeType::Identifier:
    {
        const IdentifierNode *id = static_cast<const IdentifierNode *>(node);
        std::cout << pad << "Identifier(" << id->name << ")" << std::endl;
        break;
    }
    case ASTNodeType::BinaryExpr:
    {
        const BinaryExprNode *bin = static_cast<const BinaryExprNode *>(node);
        std::cout << pad << "BinaryExpr(" << bin->op << ")" << std::endl;
        printAST(bin->left.get(), indent + 2);
        printAST(bin->right.get(), indent + 2);
        break;
    }
    case ASTNodeType::FunctionDecl:
    {
        const FunctionDeclNode *fn = static_cast<const FunctionDeclNode *>(node);
        std::cout << pad << "FunctionDecl(" << fn->access << " " << fn->name << ") -> " << fn->returnType << std::endl;
        std::cout << pad << "  Params:" << std::endl;
        for (auto &p : fn->params)
            std::cout << pad << "    " << p.first << " " << p.second << std::endl;
        std::cout << pad << "  Body:" << std::endl;
        printAST(fn->body.get(), indent + 4);
        break;
    }
    case ASTNodeType::ReturnExpr:
    {
        const ReturnExprNode *ret = static_cast<const ReturnExprNode *>(node);
        std::cout << pad << "ReturnExpr" << std::endl;
        printAST(ret->expr.get(), indent + 2);
        break;
    }
    default:
        std::cout << pad << "Unknown AST Node" << std::endl;
    }
}