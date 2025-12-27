#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include "../source/include/token.hxx"

// Minimal test helper
static void fail(size_t i, const std::string &msg)
{
    std::cerr << "[FAIL] test[" << i << "] " << msg << "\n";
    std::exit(1);
}

static void expect(bool cond, size_t i, const std::string &msg)
{
    if (!cond)
        fail(i, msg);
}

static void expectToken(size_t i, const Token &tok, TokenType type, const std::string &lexeme)
{
    expect(tok.Type == type, i,
           "token type wrong. expected=" + std::to_string((int)type) +
               ", got=" + std::to_string((int)tok.Type));
    expect(tok.Lexeme == lexeme, i,
           "lexeme wrong. expected=\"" + lexeme +
               "\", got=\"" + std::string(tok.Lexeme) + "\"");
}

// ---------------------------------------------------------
// Test Cases
// ---------------------------------------------------------

static void TestLexerBasicToken()
{
    std::string input = R"(var x: int16; 
const y: float32;
// This is a comment	
// Another comment
private add(int32 a, int32 b) int32 {
    return a + b;
}

class MyClass {
    private field1: int32;
    public field2: float64;
    public get() int32 {
        return field1;
    }
}
"Hello, World!"
'b')";

    struct Expected
    {
        TokenType type;
        std::string lexeme;
    };
    std::vector<Expected> tests = {
        {TokenType::KwVar, "var"},
        {TokenType::Identifier, "x"},
        {TokenType::Colon, ":"},
        {TokenType::KwInt16, "int16"},
        {TokenType::Semicolon, ";"},
        {TokenType::KwConst, "const"},
        {TokenType::Identifier, "y"},
        {TokenType::Colon, ":"},
        {TokenType::KwFloat32, "float32"},
        {TokenType::Semicolon, ";"},
        {TokenType::Comment, "// This is a comment"},
        {TokenType::Comment, "// Another comment"},
        {TokenType::KwPrivate, "private"},
        {TokenType::Identifier, "add"},
        {TokenType::LeftParen, "("},
        {TokenType::KwInt32, "int32"},
        {TokenType::Identifier, "a"},
        {TokenType::Comma, ","},
        {TokenType::KwInt32, "int32"},
        {TokenType::Identifier, "b"},
        {TokenType::RightParen, ")"},
        {TokenType::KwInt32, "int32"},
        {TokenType::LeftBrace, "{"},
        {TokenType::KwReturn, "return"},
        {TokenType::Identifier, "a"},
        {TokenType::Plus, "+"},
        {TokenType::Identifier, "b"},
        {TokenType::Semicolon, ";"},
        {TokenType::RightBrace, "}"},
        {TokenType::KwClass, "class"},
        {TokenType::Identifier, "MyClass"},
        {TokenType::LeftBrace, "{"},
        {TokenType::KwPrivate, "private"},
        {TokenType::Identifier, "field1"},
        {TokenType::Colon, ":"},
        {TokenType::KwInt32, "int32"},
        {TokenType::Semicolon, ";"},
        {TokenType::KwPublic, "public"},
        {TokenType::Identifier, "field2"},
        {TokenType::Colon, ":"},
        {TokenType::KwFloat64, "float64"},
        {TokenType::Semicolon, ";"},
        {TokenType::KwPublic, "public"},
        {TokenType::Identifier, "get"},
        {TokenType::LeftParen, "("},
        {TokenType::RightParen, ")"},
        {TokenType::KwInt32, "int32"},
        {TokenType::LeftBrace, "{"},
        {TokenType::KwReturn, "return"},
        {TokenType::Identifier, "field1"},
        {TokenType::Semicolon, ";"},
        {TokenType::RightBrace, "}"},
        {TokenType::RightBrace, "}"},
        {TokenType::String, "\"Hello, World!\""},
        {TokenType::Byte, "'b'"},
        {TokenType::EndOfFile, ""},
    };

    Lexer lexer(input, "test.vs");
    for (size_t i = 0; i < tests.size(); ++i)
        expectToken(i, lexer.next(), tests[i].type, tests[i].lexeme);

    std::cout << "[PASS] TestLexerBasicToken\n";
}

static void TestIdentifier()
{
    std::string input = "foo bar x y z' tail";
    std::vector<std::string> expected = {"foo", "bar", "x", "y", "z'", "tail"};
    Lexer lexer(input, "test.vs");
    for (size_t i = 0; i < expected.size(); ++i)
    {
        Token tok = lexer.next();
        expect(tok.Type == TokenType::Identifier, i, "expected Identifier");
        expect(tok.Lexeme == expected[i], i, "lexeme mismatch");
    }
    std::cout << "[PASS] TestIdentifier\n";
}

static void TestString()
{
    std::string input = R"("Hello, World!" "Line1\nLine2" "Quote: \"" "Backslash: \\")";
    std::vector<std::string> expected = {
        "\"Hello, World!\"",
        "\"Line1\\nLine2\"",
        "\"Quote: \\\"\"",
        "\"Backslash: \\\\\""};
    Lexer lexer(input, "test.vs");
    for (size_t i = 0; i < expected.size(); ++i)
    {
        Token tok = lexer.next();
        expect(tok.Type == TokenType::String, i, "expected String");
        expect(tok.Lexeme == expected[i], i, "lexeme mismatch");
    }
    std::cout << "[PASS] TestString\n";
}

static void TestByte()
{
    std::string input = R"('a' '\n' '\'' '\\')";
    std::vector<std::string> expected = {"'a'", "'\\n'", "'\\''", "'\\\\'"};
    Lexer lexer(input, "test.vs");
    for (size_t i = 0; i < expected.size(); ++i)
    {
        Token tok = lexer.next();
        expect(tok.Type == TokenType::Byte, i, "expected Byte");
        expect(tok.Lexeme == expected[i], i, "lexeme mismatch");
    }
    std::cout << "[PASS] TestByte\n";
}

static void TestComments()
{
    std::string input = "// First comment\n// Second comment\n// Third comment";
    std::vector<std::string> expected = {
        "// First comment", "// Second comment", "// Third comment"};
    Lexer lexer(input, "test.vs");
    for (size_t i = 0; i < expected.size(); ++i)
    {
        Token tok = lexer.next();
        expect(tok.Type == TokenType::Comment, i, "expected Comment");
        expect(tok.Lexeme == expected[i], i, "lexeme mismatch");
    }
    std::cout << "[PASS] TestComments\n";
}

static void TestIllegalToken()
{
    Lexer lexer("@", "test.vs");
    Token tok = lexer.next();
    expect(tok.Type == TokenType::Illegal, 0, "expected Illegal");
    std::cout << "[PASS] TestIllegalToken\n";
}

static void TestKeyword()
{
    std::string input = "var const private public class return if else for";
    std::vector<TokenType> expected = {TokenType::KwVar, TokenType::KwConst, TokenType::KwPrivate, TokenType::KwPublic, TokenType::KwClass, TokenType::KwReturn, TokenType::KwIf, TokenType::KwElse, TokenType::KwFor};
    Lexer lexer(input, "test.vs");
    for (size_t i = 0; i < expected.size(); ++i)
    {
        Token tok = lexer.next();
        expect(tok.Type == expected[i], i, "keyword mismatch");
    }
    std::cout << "[PASS] TestKeyword\n";
}

static void TestOperators()
{
    std::string input = "+ - * / % = == != < <= > >= ! && ||";
    std::vector<TokenType> expected = {TokenType::Plus, TokenType::Minus, TokenType::Asterisk, TokenType::Slash, TokenType::Percent, TokenType::Assign, TokenType::Equal, TokenType::NotEqual, TokenType::LessThan, TokenType::LessEqual, TokenType::GreaterThan, TokenType::GreaterEqual, TokenType::Not, TokenType::And, TokenType::Or};
    Lexer lexer(input, "test.vs");
    for (size_t i = 0; i < expected.size(); ++i)
    {
        Token tok = lexer.next();
        expect(tok.Type == expected[i], i, "operator mismatch");
    }
    std::cout << "[PASS] TestOperators\n";
}

static void TestDelimiters()
{
    std::string input = "( ) { } [ ] , ; :";
    std::vector<TokenType> expected = {TokenType::LeftParen, TokenType::RightParen, TokenType::LeftBrace, TokenType::RightBrace, TokenType::LeftBracket, TokenType::RightBracket, TokenType::Comma, TokenType::Semicolon, TokenType::Colon};
    Lexer lexer(input, "test.vs");
    for (size_t i = 0; i < expected.size(); ++i)
    {
        Token tok = lexer.next();
        expect(tok.Type == expected[i], i, "delimiter mismatch");
    }
    std::cout << "[PASS] TestDelimiters\n";
}

static void TestNumericLiterals()
{
    std::string input = "123 45.67 0u 255u 32767 2147483647 9223372036854775807 3.4028235 1.7976931348623157";
    struct Expected
    {
        TokenType type;
        std::string lexeme;
    };
    std::vector<Expected> expected = {
        {TokenType::Integer, "123"}, {TokenType::Float, "45.67"}, {TokenType::Unsigned, "0u"}, {TokenType::Unsigned, "255u"}, {TokenType::Integer, "32767"}, {TokenType::Integer, "2147483647"}, {TokenType::Integer, "9223372036854775807"}, {TokenType::Float, "3.4028235"}, {TokenType::Float, "1.7976931348623157"}};
    Lexer lexer(input, "test.vs");
    for (size_t i = 0; i < expected.size(); ++i)
    {
        Token tok = lexer.next();
        expect(tok.Type == expected[i].type, i, "numeric literal type mismatch");
        expect(tok.Lexeme == expected[i].lexeme, i, "numeric literal lexeme mismatch");
    }
    std::cout << "[PASS] TestNumericLiterals\n";
}

static void TestEOF()
{
    Lexer lexer("", "test.vs");
    Token tok = lexer.next();
    expect(tok.Type == TokenType::EndOfFile, 0, "expected EOF");
    std::cout << "[PASS] TestEOF\n";
}

static void TestWhitespaceHandling()
{
    std::string input = "   \n\t  var   \n\t x  ;  ";
    struct Expected
    {
        TokenType type;
        std::string lexeme;
    };
    std::vector<Expected> expected = {{TokenType::KwVar, "var"}, {TokenType::Identifier, "x"}, {TokenType::Semicolon, ";"}};
    Lexer lexer(input, "test.vs");
    for (size_t i = 0; i < expected.size(); ++i)
    {
        Token tok = lexer.next();
        expect(tok.Type == expected[i].type, i, "whitespace handling type mismatch");
        expect(tok.Lexeme == expected[i].lexeme, i, "whitespace handling lexeme mismatch");
    }
    std::cout << "[PASS] TestWhitespaceHandling\n";
}

static void TestComplexInput()
{
    std::string input = R"(
class Test {
    private value: int32;

    public setValue(int32 v) {
        value = v;
    }

    public getValue() int32 {
        return value;
    }
}

var t: Test;
t = Test();
t.setValue(42);
println("Value: " + t.getValue());
)";
    Lexer lexer(input, "test.vs");
    while (true)
    {
        Token tok = lexer.next();
        if (tok.Type == TokenType::EndOfFile)
            break;
        expect(tok.Type != TokenType::Illegal, 0, "Illegal token encountered: " + std::string(tok.Lexeme));
    }
    std::cout << "[PASS] TestComplexInput\n";
}

static void TestIdentifierWithApostrophe()
{
    std::string input = "data' value' test'";
    std::vector<std::string> expected = {"data'", "value'", "test'"};
    Lexer lexer(input, "test.vs");
    for (size_t i = 0; i < expected.size(); ++i)
    {
        Token tok = lexer.next();
        expect(tok.Type == TokenType::Identifier, i, "expected Identifier");
        expect(tok.Lexeme == expected[i], i, "lexeme mismatch");
    }
    std::cout << "[PASS] TestIdentifierWithApostrophe\n";
}

static void TestMultipleComments()
{
    std::string input = "// First comment\n// Second comment\n// Third comment";
    std::vector<std::string> expected = {"// First comment", "// Second comment", "// Third comment"};
    Lexer lexer(input, "test.vs");
    for (size_t i = 0; i < expected.size(); ++i)
    {
        Token tok = lexer.next();
        expect(tok.Type == TokenType::Comment, i, "expected Comment");
        expect(tok.Lexeme == expected[i], i, "lexeme mismatch");
    }
    std::cout << "[PASS] TestMultipleComments\n";
}

static void TestStringWithEscapes()
{
    std::string input = R"("Line1\nLine2\tTabbed\"Quote\"\\Backslash")";
    Lexer lexer(input, "test.vs");
    Token tok = lexer.next();
    expect(tok.Type == TokenType::String, 0, "expected String");
    expect(tok.Lexeme == input, 0, "lexeme mismatch");
    std::cout << "[PASS] TestStringWithEscapes\n";
}

static void TestByteWithEscapes()
{
    std::string input = R"('\n' '\t' '\'' '\\')";
    std::vector<std::string> expected = {R"('\n')", R"('\t')", R"('\'')", R"('\\')"};
    Lexer lexer(input, "test.vs");
    for (size_t i = 0; i < expected.size(); ++i)
    {
        Token tok = lexer.next();
        expect(tok.Type == TokenType::Byte, i, "expected Byte");
        expect(tok.Lexeme == expected[i], i, "lexeme mismatch");
    }
    std::cout << "[PASS] TestByteWithEscapes\n";
}

static void TestMixedInput()
{
    std::string input = R"(var count: int32 = 10; // Initialize count
count = count + 1;
println("Count is: " + count);)";
    Lexer lexer(input, "test.vs");
    while (true)
    {
        Token tok = lexer.next();
        if (tok.Type == TokenType::EndOfFile)
            break;
        expect(tok.Type != TokenType::Illegal, 0, "Illegal token encountered: " + std::string(tok.Lexeme));
    }
    std::cout << "[PASS] TestMixedInput\n";
}

static void TestWhitespaceOnly()
{
    std::string input = "   \n\t  \r\n   ";
    Lexer lexer(input, "test.vs");
    Token tok = lexer.next();
    expect(tok.Type == TokenType::EndOfFile, 0, "expected EOF");
    std::cout << "[PASS] TestWhitespaceOnly\n";
}

static void TestNoInput()
{
    Lexer lexer("", "test.vs");
    Token tok = lexer.next();
    expect(tok.Type == TokenType::EndOfFile, 0, "expected EOF");
    std::cout << "[PASS] TestNoInput\n";
}

static void TestAdjacentOperators()
{
    std::string input = "a+++b--*c";
    struct Expected
    {
        TokenType type;
        std::string lexeme;
    };
    std::vector<Expected> expected = {
        {TokenType::Identifier, "a"}, {TokenType::Plus, "+"}, {TokenType::Plus, "+"}, {TokenType::Plus, "+"}, {TokenType::Identifier, "b"}, {TokenType::Minus, "-"}, {TokenType::Minus, "-"}, {TokenType::Asterisk, "*"}, {TokenType::Identifier, "c"}};
    Lexer lexer(input, "test.vs");
    for (size_t i = 0; i < expected.size(); ++i)
    {
        Token tok = lexer.next();
        expect(tok.Type == expected[i].type && tok.Lexeme == expected[i].lexeme, i, "operator mismatch");
    }
    std::cout << "[PASS] TestAdjacentOperators\n";
}

static void TestNumbersWithLeadingZeros()
{
    std::string input = "00123 00045.67";
    struct Expected
    {
        TokenType type;
        std::string lexeme;
    };
    std::vector<Expected> expected = {
        {TokenType::Integer, "00123"},
        {TokenType::Float, "00045.67"}};
    Lexer lexer(input, "test.vs");
    for (size_t i = 0; i < expected.size(); ++i)
    {
        Token tok = lexer.next();
        expect(tok.Type == expected[i].type && tok.Lexeme == expected[i].lexeme, i, "number mismatch");
    }
    std::cout << "[PASS] TestNumbersWithLeadingZeros\n";
}

static void TestIdentifierStartingWithKeyword()
{
    std::string input = "varName constVal privateMethod";
    std::vector<std::string> expected = {"varName", "constVal", "privateMethod"};
    Lexer lexer(input, "test.vs");
    for (size_t i = 0; i < expected.size(); ++i)
    {
        Token tok = lexer.next();
        expect(tok.Type == TokenType::Identifier, i, "expected Identifier");
        expect(tok.Lexeme == expected[i], i, "lexeme mismatch");
    }
    std::cout << "[PASS] TestIdentifierStartingWithKeyword\n";
}

static void TestCommentWithSpecialChars()
{
    std::string input = "// comment with symbols !@#$%^&*()_+{}:\"<>?";
    Lexer lexer(input, "test.vs");
    Token tok = lexer.next();
    expect(tok.Type == TokenType::Comment, 0, "expected Comment");
    expect(tok.Lexeme == input, 0, "lexeme mismatch");
    std::cout << "[PASS] TestCommentWithSpecialChars\n";
}

static void TestMixedUnusualWhitespace()
{
    std::string input = "var\t x \n= 10 ;";
    struct Expected
    {
        TokenType type;
        std::string lexeme;
    };
    std::vector<Expected> expected = {
        {TokenType::KwVar, "var"}, {TokenType::Identifier, "x"}, {TokenType::Assign, "="}, {TokenType::Integer, "10"}, {TokenType::Semicolon, ";"}};
    Lexer lexer(input, "test.vs");
    for (size_t i = 0; i < expected.size(); ++i)
    {
        Token tok = lexer.next();
        expect(tok.Type == expected[i].type && tok.Lexeme == expected[i].lexeme, i, "whitespace/mixed mismatch");
    }
    std::cout << "[PASS] TestMixedUnusualWhitespace\n";
}

static void TestStringWithLineBreaks()
{
    std::string input = R"("This is a \n multi-line \n string")";
    Lexer lexer(input, "test.vs");
    Token tok = lexer.next();
    expect(tok.Type == TokenType::String, 0, "expected String");
    expect(tok.Lexeme == input, 0, "lexeme mismatch");
    std::cout << "[PASS] TestStringWithLineBreaks\n";
}

static void TestNumberEdgeCases()
{
    std::string input = "0 0.0 0u 0.0001";
    struct Expected
    {
        TokenType type;
        std::string lexeme;
    };
    std::vector<Expected> expected = {
        {TokenType::Integer, "0"}, {TokenType::Float, "0.0"}, {TokenType::Unsigned, "0u"}, {TokenType::Float, "0.0001"}};
    Lexer lexer(input, "test.vs");
    for (size_t i = 0; i < expected.size(); ++i)
    {
        Token tok = lexer.next();
        expect(tok.Type == expected[i].type && tok.Lexeme == expected[i].lexeme, i, "number edge case mismatch");
    }
    std::cout << "[PASS] TestNumberEdgeCases\n";
}

static void TestStringWithOnlyEscapes()
{
    std::string input = R"("\\n\\t\\\"\\'")";
    Lexer lexer(input, "test.vs");
    Token tok = lexer.next();
    expect(tok.Type == TokenType::String, 0, "expected String");
    expect(tok.Lexeme == input, 0, "lexeme mismatch");
    std::cout << "[PASS] TestStringWithOnlyEscapes\n";
}

int main()
{
    TestLexerBasicToken();
    TestIdentifier();
    TestString();
    TestByte();
    TestComments();
    TestIllegalToken();
    TestKeyword();
    TestOperators();
    TestDelimiters();
    TestNumericLiterals();
    TestEOF();
    TestWhitespaceHandling();
    TestComplexInput();
    TestIdentifierWithApostrophe();
    TestMultipleComments();
    TestStringWithEscapes();
    TestByteWithEscapes();
    TestMixedInput();
    TestWhitespaceOnly();
    TestNoInput();
    TestAdjacentOperators();
    TestNumbersWithLeadingZeros();
    TestIdentifierStartingWithKeyword();
    TestCommentWithSpecialChars();
    TestMixedUnusualWhitespace();
    TestStringWithLineBreaks();
    TestNumberEdgeCases();
    TestStringWithOnlyEscapes();
    std::cout << "\nALL TESTS PASSED\n";
    return 0;
}
