#include <iostream>

#include <cxxopts.hpp>

#include "CodeGenerator.h"
#include "Lexer.h"
#include "Parser.h"

int main(int ArgumentCount, char** Arguments)
{
    cxxopts::Options Options("Little Computer Assembler");
    Options.parse_positional("file");
    auto Result = Options.parse(ArgumentCount, Arguments);

    try
    {
        auto InputFileName = Result["file"].as<std::string>();
        std::cout << InputFileName << std::endl;
    }
    catch (const cxxopts::option_not_present_exception&)
    {
        std::cerr << "No input file provided" << std::endl;
        Options.show_positional_help();
    }
}
