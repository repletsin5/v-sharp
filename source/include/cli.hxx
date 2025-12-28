#pragma once

#include <string>
#include <vector>

void printHelp();

void printVersion();

void compileFile(const std::string &filename, const std::vector<std::string>& flags);
