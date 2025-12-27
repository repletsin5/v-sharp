#include <iostream>
#include <fstream>
#include "include/token.hxx"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <file>\n";
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary);
    if (!file)
    {
        std::cerr << "Cannot open file\n";
        return 1;
    }

    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    Lexer lexer(std::move(source), argv[1]);

    while (true)
    {
        Token tok = lexer.next();
        std::cout << int(tok.Type) << " : " << tok.Lexeme << '\n';
        if (tok.Type == TokenType::EndOfFile)
            break;
    }

    return 0;
}