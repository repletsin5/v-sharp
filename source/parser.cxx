#include "include/parser.hxx"
#include <stdexcept>

void Parser::expect(TokenType type)
{
    if (current.Type != type)
        throw std::runtime_error("Unexpected token: '" + std::string(current.Lexeme) +
                                 "' at line " + std::to_string(current.Line));
    advance();
}

ASTNodePtr Parser::parserProgram()
{
    ASTNodeList expressions;
    while (current.Type != TokenType::EndOfFile)
    {
        ASTNodePtr node;

        bool maybeFunction = false;
        if (current.Type == TokenType::KwPublic ||
            current.Type == TokenType::KwPrivate ||
            current.Type == TokenType::KwVirtual ||
            current.Type == TokenType::KwOverride ||
            current.Type == TokenType::KwStatic)
        {
            maybeFunction = true;
        }
        else if (current.Type == TokenType::Identifier)
        {
            Token next = peekToken();
            if (next.Type == TokenType::LeftParen)
                maybeFunction = true;
        }

        if (maybeFunction)
            node = parseFunction();
        else
            node = parseExpression();

        expressions.push_back(std::move(node));

        if (current.Type == TokenType::Semicolon)
            advance();
    }

    auto block = std::make_unique<BlockNode>();
    block->children = std::move(expressions);
    return block;
}

ASTNodePtr Parser::parsePrimary()
{
    switch (current.Type)
    {
    case TokenType::KwReturn:
    {
        advance();
        ASTNodePtr expr = parseExpression();
        return std::make_unique<ReturnExprNode>(std::move(expr));
    }
    case TokenType::Integer:
    {
        int64_t value = std::stoi(std::string(current.Lexeme));
        ASTNodePtr node = std::make_unique<LiteralNode>(Type::I64, value);
        advance();
        return node;
    }
    case TokenType::Float:
    {
        double value = std::stod(std::string(current.Lexeme));
        ASTNodePtr node = std::make_unique<LiteralNode>(Type::F64, value);
        advance();
        return node;
    }
    case TokenType::Unsigned:
    {
        uint64_t value = std::stoul(std::string(current.Lexeme));
        ASTNodePtr node = std::make_unique<LiteralNode>(Type::U64, value);
        advance();
        return node;
    }
    case TokenType::Byte:
    {
        std::string_view lex = current.Lexeme;
        if (lex.size() < 3 || lex.front() != '\'' || lex.back() != '\'')
            throw std::runtime_error("Invalid byte literal at line " + std::to_string(current.Line));
        char value = lex[1];
        if (value == '\\')
        {
            if (lex.size() < 4)
                throw std::runtime_error("Invalid escape sequence in byte literal");

            switch (lex[2])
            {
            case 'n':
                value = '\n';
                break;
            case 't':
                value = '\t';
                break;
            case 'r':
                value = '\r';
                break;
            case '\\':
                value = '\\';
                break;
            case '\'':
                value = '\'';
                break;
            case '"':
                value = '"';
                break;
            default:
                value = lex[2];
                break;
            }
        }
        ASTNodePtr node = std::make_unique<LiteralNode>(Type::Byte, value);
        advance();
        return node;
    }
    case TokenType::String:
    {
        std::string value = std::string(current.Lexeme);
        ASTNodePtr node = std::make_unique<LiteralNode>(Type::String, value);
        advance();
        return node;
    }
    case TokenType::Boolean:
    {
        bool value = (current.Lexeme == "true");
        ASTNodePtr node = std::make_unique<LiteralNode>(Type::Bool, value);
        advance();
        return node;
    }
    case TokenType::Identifier:
    {
        ASTNodePtr node = std::make_unique<IdentifierNode>(std::string(current.Lexeme));
        advance();
        return node;
    }
    case TokenType::LeftParen:
    {
        advance();
        ASTNodePtr expr = parseExpression();
        expect(TokenType::RightParen);
        return expr;
    }
    default:
        throw std::runtime_error("Unexpected token in primary expression at line " + std::to_string(current.Line));
    }
}

ASTNodePtr Parser::parseExpression(int minPrec)
{
    ASTNodePtr left = parsePrimary();

    while (true)
    {
        int prec = getPrecedence();
        if (prec < minPrec)
            break;

        Token op = current;
        advance();

        ASTNodePtr right = parseExpression(prec + 1);

        left = std::make_unique<BinaryExprNode>(std::string(op.Lexeme), std::move(left), std::move(right));
    }

    return left;
}

ASTNodePtr Parser::parseFunction()
{
    std::vector<std::string> modifiers(3);
	uint8_t maxModifiers = 0;
    while (true && maxModifiers < 3)
    {
        
        switch (current.Type)
        {
            case TokenType::KwPublic:
                modifiers.emplace_back("public");
                advance();
                break;
            case TokenType::KwPrivate:
                modifiers.emplace_back("private");
                advance();
                break;
            case TokenType::KwVirtual:
                modifiers.emplace_back("virtual");
                advance();
                break;
            case TokenType::KwOverride:
                modifiers.emplace_back("override");
                advance();
                break;
            case TokenType::KwStatic:
                modifiers.emplace_back("static");
                advance();
                break;
            default: {
                break; break; //replet - cursed i hate this.
            }
        }
        maxModifiers++;
    }

    if (current.Type != TokenType::Identifier)
        throw std::runtime_error("Expected function name at line " + std::to_string(current.Line));
    std::string name(current.Lexeme);
    advance();

    expect(TokenType::LeftParen);

    std::vector<std::pair<Type, std::string>> params;
    while (current.Type != TokenType::RightParen)
    {
        Type paramType = parseType();
        if (current.Type == TokenType::LeftBracket)
        {
            advance();
            while (true)
            {
                if (current.Type != TokenType::Identifier)
                    throw std::runtime_error("Expected parameter name inside brackets at line " + std::to_string(current.Line));
                std::string paramName(current.Lexeme);
                params.emplace_back(paramType, paramName);
                advance();

                if (current.Type == TokenType::Comma)
                    advance();
                else if (current.Type == TokenType::RightBracket)
                {
                    advance();
                    break;
                }
                else
                    throw std::runtime_error("Expected ',' or ']' in parameter list at line " + std::to_string(current.Line));
            }
        }
        else
        {
            if (current.Type != TokenType::Identifier)
                throw std::runtime_error("Expected parameter name at line " + std::to_string(current.Line));
            std::string paramName(current.Lexeme);
            params.emplace_back(paramType, paramName);
            advance();
        }

        if (current.Type == TokenType::Comma)
            advance();
    }

    expect(TokenType::RightParen);

    Type retType = Type::Void;
    if (current.Type != TokenType::LeftBrace)
        retType = parseType();

    ASTNodePtr body = std::make_unique<BlockNode>();
    if (current.Type == TokenType::LeftBrace)
    {
        advance();
        ASTNodeList statements;
        while (current.Type != TokenType::RightBrace && current.Type != TokenType::EndOfFile)
        {
            statements.push_back(parseExpression());
            if (current.Type == TokenType::Semicolon)
                advance();
        }
        expect(TokenType::RightBrace);
        static_cast<BlockNode *>(body.get())->children = std::move(statements);
    }

    std::string access;
    for (auto &mod : modifiers)
    {
        if (!access.empty())
            access += " ";
        access += mod;
    }

    return std::make_unique<FunctionDeclNode>(access, name, params, retType, std::move(body));
}

Type Parser::parseType()
{
    switch (current.Type)
    {
    case TokenType::KwInt8:
        advance();
        return Type::I8;
    case TokenType::KwInt16:
        advance();
        return Type::I16;
    case TokenType::KwInt32:
        advance();
        return Type::I32;
    case TokenType::KwInt64:
        advance();
        return Type::I64;
    case TokenType::KwUInt8:
        advance();
        return Type::U8;
    case TokenType::KwUInt16:
        advance();
        return Type::U16;
    case TokenType::KwUInt32:
        advance();
        return Type::U32;
    case TokenType::KwUInt64:
        advance();
        return Type::U64;
    case TokenType::KwFloat32:
        advance();
        return Type::F32;
    case TokenType::KwFloat64:
        advance();
        return Type::F64;
    case TokenType::KwBoolean:
        advance();
        return Type::Bool;
    case TokenType::KwByte:
        advance();
        return Type::Byte;
    case TokenType::KwString:
        advance();
        return Type::String;
    case TokenType::KwVoid:
        advance();
        return Type::Void;
    default:
        throw std::runtime_error("Expected type at line " + std::to_string(current.Line));
    }
}