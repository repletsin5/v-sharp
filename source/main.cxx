#include <map>
#include <iostream>
#include <functional>
#include <cli.hxx>
#include <server.hxx>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printHelp();
        return 1;
    }

    std::map<std::string, std::function<void(const std::vector<std::string> &)>> commands;

    commands["help"] = [](const auto &)
    {
        printHelp();
    };
    commands["version"] = [](const auto &)
    {
        printVersion();
    };
    commands["lsp"] = [](const auto &)
    {
        runLSP();
    };
    commands["compile"] = [](const auto &args)
    {
        if (args.empty())
        {
            std::cerr << "Error: No file provided." << std::endl;
            exit(1);
        }
        std::string file = args[0];
        std::vector<std::string> flags(args.begin() + 1, args.end());
        compileFile(file, flags);
    };

    std::string command = argv[1];
    std::vector<std::string> args(argv + 2, argv + argc);

    if (commands.find(command) != commands.end())
    {
        commands[command](args);
    }
    else
    {
        std::cerr << "Unknown command: " << command << std::endl;
        printHelp();
        return 1;
    }

    return 0;
}