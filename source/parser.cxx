#include <stdexcept>
#include <parser.hxx>

void Parser::expect(TokenType type)
{
    if (current.Type != type)
        throw std::runtime_error("Unexpected token: '" + std::string(current.Lexeme) +
                                 "' at line " + std::to_string(current.Line));
    advance();
}

ASTNodePtr Parser::parseBody(TokenType endcase, ASTNode* parent, bool shouldAdvance) {
    ASTNodeList expressions;
    while (current.Type != endcase && current.Type != TokenType::EndOfFile)
    {
        ASTNodePtr node;

        bool hasAccess = false;
        if (current.Type == TokenType::KwPublic ||
            current.Type == TokenType::KwPrivate /* ||
            current.Type == TokenType::KwVirtual ||
            current.Type == TokenType::KwOverride ||
            current.Type == TokenType::KwStatic*/
            )
        {
            hasAccess = true;
        }
        else if (current.Type == TokenType::Identifier)
        {
            Token next = peekToken();
            if (next.Type == TokenType::LeftParen) {

                node = parseFunction();
				node.get()->parent = parent;
            }
        }

        if (hasAccess) {
            Token next = peekToken();
            if (next.Type == TokenType::KwClass) {
                node = parseClassDecl();
            }
            else {
                if(next.Type == TokenType::KwVar ||
                    next.Type == TokenType::KwConst)
                    node = parseVarDecl(parent);
				else
                    node = parseFunction();
            }
            node.get()->parent = parent;

        }
        else if(node.get() == nullptr) {
            if (current.Type == TokenType::KwClass) {
                node = parseClassDecl();
                node.get()->parent = parent;
            }
            else if(current.Type == TokenType::KwVar 
                || current.Type == TokenType::KwConst
                )
            {
				node = parseVarDecl(parent);
                node.get()->parent = parent;
            }
            else {
                Token next = peekToken();
                 if (next.Type == TokenType::LeftParen) {
                    node = parseFunction(parent);
                 }
                 else if (next.Type == TokenType::KwVar || next.Type == TokenType::KwConst) {
				    node = parseVarDecl(parent);
                    }
                 else {
                    if (parent == nullptr)
                        node = parseExpression();
                    else   
                        throw std::runtime_error("Unexpected token in class body at line " + std::to_string(current.Line));
                 }

                
            }
        }
        expressions.push_back(std::move(node));

        if (current.Type == TokenType::Semicolon)
            advance();
    }

    if(shouldAdvance)
		advance();
    auto block = std::make_unique<BlockNode>();
    block->children = std::move(expressions);
    return block;
}

ASTNodePtr Parser::parserProgram()
{
    auto block = parseBody(TokenType::EndOfFile, nullptr, false);
    return block;
}

ASTNodePtr Parser::parsePrimary()
{
    switch (current.Type)
    {
    case TokenType::KwIf:
        return parseIfExpr();
    case TokenType::KwVar:
    case TokenType::KwConst:
        return parseVarDecl();
    case TokenType::KwReturn:
    {
        advance();
        return std::make_unique<ReturnExprNode>(parseExpression());
    }
    case TokenType::Integer:
    {
        int64_t value = std::stoi(std::string(current.Lexeme));
        advance();
        return std::make_unique<LiteralNode>(Type::Int64, value);
    }
    case TokenType::Float:
    {
        double value = std::stod(std::string(current.Lexeme));
        advance();
        return std::make_unique<LiteralNode>(Type::Float64, value);
    }
    case TokenType::Unsigned:
    {
        uint64_t value = std::stoul(std::string(current.Lexeme));
        advance();
        return std::make_unique<LiteralNode>(Type::Uint64, value);
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
        advance();
        return std::make_unique<LiteralNode>(Type::Byte, value);
    }
    case TokenType::String:
    {
        std::string value = std::string(current.Lexeme);
        advance();
        return std::make_unique<LiteralNode>(Type::String, value);
    }
    case TokenType::Boolean:
    {
        bool value = (current.Lexeme == "true");
        advance();
        return std::make_unique<LiteralNode>(Type::Boolean, value);
    }
    case TokenType::Identifier:
    {
        std::string name(current.Lexeme);
        advance();
        return std::make_unique<IdentifierNode>(name);
    }
    case TokenType::LeftParen:
    {
        advance();
        ASTNodePtr expr = parseExpression();
        expect(TokenType::RightParen);
        return expr;
    }
    default:
        throw std::runtime_error("Unexpected token in expression at line " + std::to_string(current.Line));
    }
}

ASTNodePtr Parser::parseExpression(int minPrec)
{
    if (current.Type == TokenType::Identifier)
    {
        Token ident = current;
        Token next = peekToken();

        if (next.Type == TokenType::Assign)
        {
            advance();
            advance();
            ASTNodePtr value = parseExpression();
            return std::make_unique<AssignExprNode>(
                std::string(ident.Lexeme),
                std::move(value));
        }
    }

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

ASTNodePtr Parser::parseFunction(ASTNode* parent)
{

	AccessType access = parseAccessModifier();
    if (access == AccessType::Default && parent != nullptr) {
        access = AccessType::Private;
    }
    else if (access == AccessType::Default && parent == nullptr)
    {
        access = AccessType::Public;
    }

    ModifierType modifier = parseModifiers();

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
        ASTNodeList expressions;
        while (current.Type != TokenType::RightBrace && current.Type != TokenType::EndOfFile)
        {
            expressions.push_back(parseExpression());
        }
        expect(TokenType::RightBrace);
        static_cast<BlockNode *>(body.get())->children = std::move(expressions);
    }


    auto node = std::make_unique<FunctionDeclNode>(modifier, name, params, retType, std::move(body), access);
	node.get()->parent = parent;
    return node;
}

Type Parser::parseType()
{
    switch (current.Type)
    {
    case TokenType::KwInt8:
        advance();
        return Type::Int8;
    case TokenType::KwInt16:
        advance();
        return Type::Int16;
    case TokenType::KwInt32:
        advance();
        return Type::Int32;
    case TokenType::KwInt64:
        advance();
        return Type::Int64;
    case TokenType::KwUInt8:
        advance();
        return Type::Uint8;
    case TokenType::KwUInt16:
        advance();
        return Type::Uint16;
    case TokenType::KwUInt32:
        advance();
        return Type::Uint32;
    case TokenType::KwUInt64:
        advance();
        return Type::Uint64;
    case TokenType::KwFloat32:
        advance();
        return Type::Float32;
    case TokenType::KwFloat64:
        advance();
        return Type::Float64;
    case TokenType::KwBoolean:
        advance();
        return Type::Boolean;
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

ASTNodePtr Parser::parseVarDecl(ASTNode* parent)
{
	AccessType access = parseAccessModifier();
    if (access == AccessType::Default && parent != nullptr) {
		access == AccessType::Private;
    }
    else if(access == AccessType::Default && parent == nullptr)
    {
		access = AccessType::Public;
    }
    ModifierType modifier = parseModifiers();

    bool isConst = false;
    if (current.Type == TokenType::KwConst)
        isConst = true;
    advance();

    if (current.Type != TokenType::Identifier)
        throw std::runtime_error("Expected variable name at line " + std::to_string(current.Line));
    std::string name(current.Lexeme);
    advance();

    expect(TokenType::Colon);

    Type varType = parseType();

    ASTNodePtr value = nullptr;
    if (current.Type == TokenType::Assign)
    {
        advance();
        value = parseExpression();
    }
    auto node = std::make_unique<VarDeclNode>(isConst, name, varType, std::move(value), modifier);
	node.get()->parent = parent;
    return node;
}

ASTNodePtr Parser::parseIfExpr()
{
    expect(TokenType::KwIf);

    ASTNodePtr condition = parseExpression();
    expect(TokenType::LeftBrace);

    ASTNodeList thenExpressions;
    while (current.Type != TokenType::RightBrace && current.Type != TokenType::EndOfFile)
    {
        thenExpressions.push_back(parseExpression());
    }
    expect(TokenType::RightBrace);
    ASTNodePtr thenBlock = std::make_unique<BlockNode>();
    static_cast<BlockNode *>(thenBlock.get())->children = std::move(thenExpressions);

    ASTNodePtr elseBranch = nullptr;
    if (current.Type == TokenType::KwElse)
    {
        advance();
        if (current.Type == TokenType::KwIf)
        {
            elseBranch = parseIfExpr();
        }
        else if (current.Type == TokenType::LeftBrace)
        {
            advance();
            ASTNodeList elseExpressions;
            while (current.Type != TokenType::RightBrace && current.Type != TokenType::EndOfFile)
            {
                elseExpressions.push_back(parseExpression());
            }
            expect(TokenType::RightBrace);
            elseBranch = std::make_unique<BlockNode>();
            static_cast<BlockNode *>(elseBranch.get())->children = std::move(elseExpressions);
        }
        else
        {
            throw std::runtime_error("Expected '{' or 'if' after 'else' at line " + std::to_string(current.Line));
        }
    }
    return std::make_unique<IfExprNode>(std::move(condition), std::move(thenBlock), std::move(elseBranch));
}

ASTNodePtr Parser::parseClassDecl()
{
	auto access = parseAccessModifier();
    if (access == AccessType::Default) {
		access = AccessType::Public;
    }
	expect(TokenType::KwClass);
    if (current.Type != TokenType::Identifier) {
        throw std::runtime_error("Expected class name at line " + std::to_string(current.Line));
    }
    std::string name(current.Lexeme);
    
	advance();
    ASTNodePtr clazz = std::make_unique<ClassDeclNode>(name, access, nullptr);
    
    ASTNodePtr body = std::make_unique<BlockNode>();
    if (current.Type == TokenType::LeftBrace)
    {
        advance();
        ASTNodeList expressions;

        body = parseBody(TokenType::RightBrace, clazz.get());

    }
    else {
		throw std::runtime_error("Expected '{' after class name at line " + std::to_string(current.Line));
    }
	((ClassDeclNode*)clazz.get())->body = std::move(body);
    return clazz;
}

AccessType Parser::parseAccessModifier()
{   
    if (current.Type == TokenType::KwPublic)
    {   
        advance();
		return AccessType::Public;
    }
    else if (current.Type == TokenType::KwPrivate)
    { 
		advance();
		return AccessType::Private;
    }
	return AccessType::Default;
    
}

ModifierType Parser::parseModifiers()
{

    if (current.Type == TokenType::KwOverride)
    {
        advance();
        return ModifierType::Override;
    }
    else if (current.Type == TokenType::KwStatic)
    {
        advance();
        return ModifierType::Static;
    }
    else if(current.Type == TokenType::KwVirtual) {
        advance();
        return ModifierType::Virtual;
    }

	return ModifierType::None;
}
