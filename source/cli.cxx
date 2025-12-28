#include <iostream>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <cli.hxx>
#include <config.hxx>
#include <parser.hxx>

void printHelp()
{
    std::cout << "TODO" << std::endl;
}

void printVersion()
{
    std::cout << "VSharp Compiler v" << VSHARP_VERSION << std::endl;
}

void compileFile(const std::string &filename, const std::vector<std::string> &flags)
{
    if (!std::filesystem::exists(filename))
    {
        std::cerr << "File does not exist: " << filename << std::endl;
    }

    std::ifstream file(filename, std::ios::binary);
    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    Lexer lexer(std::move(source), filename);
    Parser parser(lexer);

    try {
        ASTNodePtr ast = parser.parserProgram();
        
        if (std::find(flags.begin(), flags.end(), "--emit-ast") != flags.end()) {
            printAST(ast.get());
        }
    } catch (const std::exception &e) {
        std::cerr << "Parser Error: " << e.what() << std::endl;
        exit(1);
    }
}