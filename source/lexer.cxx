#include "include/token.hxx"

char Lexer::peek(size_t offset) const
{
    size_t p = Position + offset;
    return p < Source.size() ? Source[p] : '\0';
}

char Lexer::advance()
{
    if (Position >= Source.size())
        return '\0';

    char c = Source[Position++];
    if (c == '\n')
    {
        Line++;
        Column = 1;
    }
    else
        Column++;
    return c;
}

void Lexer::skipWhitespace()
{
    while (std::isspace(peek()))
        advance();
}

Token Lexer::makeToken(TokenType type, size_t start, size_t end, size_t line, size_t column) const
{
    return Token{type, std::string_view(Source.data() + start, end - start), File, line, column};
}

TokenType Lexer::lookupIdentifier(std::string_view s) const
{
    for (auto &kw : keywords)
    {
        if (kw.name == s)
            return kw.type;
    }
    return TokenType::Identifier;
}

Token Lexer::scanIdentifier(size_t line, size_t column)
{
    size_t start = Position;
    while (std::isalnum(peek()) || peek() == '_' || peek() == '\'')
        advance();
    size_t end = Position;
    std::string_view ident(Source.data() + start, end - start);
    TokenType type = lookupIdentifier(ident);
    return makeToken(type, start, end, line, column);
}

Token Lexer::scanString(size_t line, size_t column)
{
    size_t start = Position;
    advance();
    while (peek() != '\0')
    {
        if (peek() == '"')
        {
            advance();
            break;
        }
        if (peek() == '\\')
        {
            advance();
            if (peek() != '\0')
                advance();
            continue;
        }
        advance();
    }
    return makeToken(TokenType::String, start, Position, line, column);
}

Token Lexer::scanByte(size_t line, size_t column)
{
    size_t start = Position;
    advance();

    if (peek() == '\\')
    {
        advance();
        if (peek() != '\0')
            advance();
    }
    else if (peek() != '\0')
        advance();

    if (peek() == '\'')
        advance();

    return makeToken(TokenType::Byte, start, Position, line, column);
}

Token Lexer::scanNumber(size_t line, size_t column)
{
    size_t start = Position;
    bool isFloat = false;
    while (std::isdigit(peek()) || peek() == '.')
    {
        if (peek() == '.')
        {
            if (isFloat)
                break;
            isFloat = true;
        }
        advance();
    }
    if (peek() == 'u')
    {
        advance();
        return makeToken(TokenType::Unsigned, start, Position, line, column);
    }
    return makeToken(isFloat ? TokenType::Float : TokenType::Integer, start, Position, line, column);
}

Token Lexer::scanLineComment(size_t line, size_t column)
{
    size_t start = Position;
    advance();
    advance();
    while (peek() != '\0' && peek() != '\n')
        advance();
    size_t end = Position;
    while (end > start && std::isspace(static_cast<unsigned char>(Source[end - 1])))
        --end;
    return makeToken(TokenType::Comment, start, end, line, column);
}

Token Lexer::next()
{
    skipWhitespace();
    size_t line = Line;
    size_t column = Column;
    char c = peek();

    if (c == '\0')
        return makeToken(TokenType::EndOfFile, Position, Position, line, column);
    if (std::isalpha(c) || c == '_')
        return scanIdentifier(line, column);
    if (c == '"')
        return scanString(line, column);
    if (c == '\'')
        return scanByte(line, column);
    if (std::isdigit(c))
        return scanNumber(line, column);
    if (c == '/' && peek(1) == '/')
        return scanLineComment(line, column);

    advance();

    switch (c)
    {
    case '=':
        return peek() == '=' ? (advance(), makeToken(TokenType::Equal, Position - 2, Position, line, column))
                             : makeToken(TokenType::Assign, Position - 1, Position, line, column);
    case '!':
        return peek() == '=' ? (advance(), makeToken(TokenType::NotEqual, Position - 2, Position, line, column))
                             : makeToken(TokenType::Not, Position - 1, Position, line, column);
    case '<':
        return peek() == '=' ? (advance(), makeToken(TokenType::LessEqual, Position - 2, Position, line, column))
                             : makeToken(TokenType::LessThan, Position - 1, Position, line, column);
    case '>':
        return peek() == '=' ? (advance(), makeToken(TokenType::GreaterEqual, Position - 2, Position, line, column))
                             : makeToken(TokenType::GreaterThan, Position - 1, Position, line, column);
    case '&':
        return peek() == '&' ? (advance(), makeToken(TokenType::And, Position - 2, Position, line, column))
                             : makeToken(TokenType::Illegal, Position - 1, Position, line, column);
    case '|':
        return peek() == '|' ? (advance(), makeToken(TokenType::Or, Position - 2, Position, line, column))
                             : makeToken(TokenType::Vbar, Position - 1, Position, line, column);
    case '+':
        return makeToken(TokenType::Plus, Position - 1, Position, line, column);
    case '-':
        return makeToken(TokenType::Minus, Position - 1, Position, line, column);
    case '*':
        return makeToken(TokenType::Asterisk, Position - 1, Position, line, column);
    case '/':
        return makeToken(TokenType::Slash, Position - 1, Position, line, column);
    case '%':
        return makeToken(TokenType::Percent, Position - 1, Position, line, column);
    case '(':
        return makeToken(TokenType::LeftParen, Position - 1, Position, line, column);
    case ')':
        return makeToken(TokenType::RightParen, Position - 1, Position, line, column);
    case '{':
        return makeToken(TokenType::LeftBrace, Position - 1, Position, line, column);
    case '}':
        return makeToken(TokenType::RightBrace, Position - 1, Position, line, column);
    case '[':
        return makeToken(TokenType::LeftBracket, Position - 1, Position, line, column);
    case ']':
        return makeToken(TokenType::RightBracket, Position - 1, Position, line, column);
    case ',':
        return makeToken(TokenType::Comma, Position - 1, Position, line, column);
    case ';':
        return makeToken(TokenType::Semicolon, Position - 1, Position, line, column);
    case ':':
        return makeToken(TokenType::Colon, Position - 1, Position, line, column);
    case '.':
        return makeToken(TokenType::Dot, Position - 1, Position, line, column);
    default:
        return makeToken(TokenType::Illegal, Position - 1, Position, line, column);
    }
}