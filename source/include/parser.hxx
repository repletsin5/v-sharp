#pragma once

#include <ast.hxx>
#include <token.hxx>

struct Parser
{
    Lexer &lexer;
    Token current, nextToken;

    Parser(Lexer &lexer) : lexer(lexer)
    {
        current = lexer.next();
        nextToken = lexer.next();
    }
    void advance()
    {
        current = nextToken;
        nextToken = lexer.next();
    }
    void expect(TokenType type);
    Token peekToken() const { return nextToken; }
    ASTNodePtr parserProgram();
    ASTNodePtr parseExpression(int minPrec = 1);
    ASTNodePtr parsePrimary();
    ASTNodePtr parseFunction(ASTNode* parent = nullptr);
    Type parseType();
    ASTNodePtr parseVarDecl(ASTNode* parent = nullptr);
    ASTNodePtr parseIfExpr();
    ASTNodePtr parseClassDecl();
    AccessType parseAccessModifier();
    ModifierType parseModifiers();
    ASTNodePtr parseBody(TokenType endCase =  TokenType::LeftBrace, ASTNode* pc = nullptr,bool shouldAdvance = true);


private:
    int getPrecedence() const { return lexer.precedence(current.Type); }
};