#pragma once

#include <string>
#include <string_view>
#include <vector>

/**
 * @brief Enumeration of all token types in the language.
 */
enum class TokenType
{
    Illegal, /**< Represents an invalid or unrecognized token */
    Comment, /**< Represents a comment */

    // Literals
    Identifier, /**< Identifier (variable or function name) */
    Integer,    /**< All integer literals */
    Float,      /**< All floating-point literals */
    Unsigned,   /**< Unsigned integer literals */
    Int8,
    Int16,
    Int32,
    Int64, /**< Specific integer types */
    UInt8,
    UInt16,
    UInt32,
    UInt64, /**< Specific unsigned integer types */
    Float32,
    Float64, /**< Floating-point types */
    Boolean, /**< Boolean literal */
    String,  /**< String literal */
    Byte,    /**< Byte literal */
    Void,    /**< Void type */

    // Operators
    Plus,     /**< + */
    Minus,    /**< - */
    Asterisk, /**< * */
    Slash,    /**< / */
    Percent,  /**< % */

    // Delimiters
    LeftParen,    /**< ( */
    RightParen,   /**< ) */
    LeftBrace,    /**< { */
    RightBrace,   /**< } */
    LeftBracket,  /**< [ */
    RightBracket, /**< ] */
    Comma,        /**< , */
    Semicolon,    /**< ; */
    Colon,        /**< : */
    Dot,          /**< . */
    Vbar,         /**< | */

    // Assignment and comparison
    Assign,       /**< = */
    Equal,        /**< == */
    NotEqual,     /**< != */
    LessThan,     /**< < */
    GreaterThan,  /**< > */
    LessEqual,    /**< <= */
    GreaterEqual, /**< >= */

    // Logical
    Not, /**< ! */
    And, /**< && */
    Or,  /**< || */

    // Keywords
    KwPublic,
    KwPrivate,
    KwVirtual,
    KwOverride,
    KwStatic,
    KwConst,
    KwVar,
    KwIf,
    KwElse,
    KwMatch,
    KwFor,
    KwReturn,
    KwStructure,
    KwEnumeration,
    KwDefine,
    KwTypedef,
    KwClass,
    KwInt8,
    KwInt16,
    KwInt32,
    KwInt64,
    KwUInt8,
    KwUInt16,
    KwUInt32,
    KwUInt64,
    KwFloat32,
    KwFloat64,
    KwBoolean,
    KwString,
    KwByte,
    KwVoid,
    EndOfFile /**< End of file marker */
};

/**
 * @brief Represents a lexical token.
 */
struct Token
{
    TokenType Type;          /**< Type of the token */
    std::string_view Lexeme; /**< Raw text of the token */
    std::string File;        /**< Source file name */
    size_t Line, Column;     /**< Line and column of the token in the source */
};

/**
 * @brief Represents a keyword mapping to its token type.
 */
struct Keyword
{
    std::string name; /**< Keyword text */
    TokenType type;   /**< Corresponding token type */
};

/** @brief List of all keywords in the language */
inline const std::vector<Keyword> keywords = {
    {"public", TokenType::KwPublic},
    {"private", TokenType::KwPrivate},
    {"virtual", TokenType::KwVirtual},
    {"override", TokenType::KwOverride},
    {"static", TokenType::KwStatic},
    {"const", TokenType::KwConst},
    {"var", TokenType::KwVar},
    {"if", TokenType::KwIf},
    {"else", TokenType::KwElse},
    {"match", TokenType::KwMatch},
    {"for", TokenType::KwFor},
    {"return", TokenType::KwReturn},
    {"structure", TokenType::KwStructure},
    {"enumeration", TokenType::KwEnumeration},
    {"define", TokenType::KwDefine},
    {"typedef", TokenType::KwTypedef},
    {"class", TokenType::KwClass},
    {"int8", TokenType::KwInt8},
    {"int16", TokenType::KwInt16},
    {"int32", TokenType::KwInt32},
    {"int64", TokenType::KwInt64},
    {"uint8", TokenType::KwUInt8},
    {"uint16", TokenType::KwUInt16},
    {"uint32", TokenType::KwUInt32},
    {"uint64", TokenType::KwUInt64},
    {"float32", TokenType::KwFloat32},
    {"float64", TokenType::KwFloat64},
    {"boolean", TokenType::KwBoolean},
    {"string", TokenType::KwString},
    {"byte", TokenType::KwByte},
    {"void", TokenType::KwVoid},
};

/**
 * @brief Lexical analyzer that converts source code into tokens.
 */
struct Lexer
{
    std::string Source; /**< Full source code */
    size_t Position;    /**< Current position in the source */
    size_t Line;        /**< Current line number */
    size_t Column;      /**< Current column number */
    std::string File;   /**< Source file name */

    /**
     * @brief Construct a new Lexer.
     * @param src Source code string
     * @param file Source file name
     */
    Lexer(std::string src, std::string file)
        : Source(std::move(src)), Position(0), Line(1), Column(1), File(std::move(file)) {}

    /**
     * @brief Get the next token from the source code.
     * @return Token The next token.
     */
    Token next();

private:
    /**
     * @brief Peek at a character in the source code without advancing.
     * @param offset Offset from the current position.
     * @return char Character at the given offset.
     */
    char peek(size_t offset = 0) const;

    /**
     * @brief Advance the current position by one character.
     * @return char The character at the previous position.
     */
    char advance();

    /**
     * @brief Skip whitespace and update line/column counters.
     */
    void skipWhitespace();

    /**
     * @brief Create a token from the source range.
     * @param type Token type
     * @param start Start index in the source
     * @param end End index in the source
     * @param line Line number
     * @param column Column number
     * @return Token Constructed token
     */
    Token makeToken(TokenType type, size_t start, size_t end, size_t line, size_t column) const;

    /**
     * @brief Lookup an identifier to see if it is a keyword.
     * @param s Identifier string
     * @return TokenType Keyword token type or Identifier
     */
    TokenType lookupIdentifier(std::string_view s) const;

    /**
     * @brief Scan an identifier or keyword token.
     * @param line Starting line
     * @param column Starting column
     * @return Token The identifier or keyword token
     */
    Token scanIdentifier(size_t line, size_t column);

    /**
     * @brief Scan a string literal token.
     * @param line Starting line
     * @param column Starting column
     * @return Token String literal token
     */
    Token scanString(size_t line, size_t column);

    /**
     * @brief Scan a byte literal token.
     * @param line Starting line
     * @param column Starting column
     * @return Token Byte literal token
     */
    Token scanByte(size_t line, size_t column);

    /**
     * @brief Scan a numeric literal token (integer or float).
     * @param line Starting line
     * @param column Starting column
     * @return Token Numeric token
     */
    Token scanNumber(size_t line, size_t column);

    /**
     * @brief Scan a single-line comment token.
     * @param line Starting line
     * @param column Starting column
     * @return Token Comment token
     */
    Token scanLineComment(size_t line, size_t column);
};
