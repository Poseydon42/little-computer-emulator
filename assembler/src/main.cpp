#include <fstream>
#include <iostream>

#include <cxxopts.hpp>  

#include "CodeGenerator.h"
#include "ErrorReporting.h"
#include "Lexer.h"
#include "Parser.h"

using namespace lce::Assembler;

std::string ReadFile(const std::string& File)
{
    std::ifstream Input;
    Input.open(File);

    if (!Input.is_open())
    {
        lce::Common::ReportError(lce::Common::ErrorSeverity::Fatal, {}, "Cannot open input file {} for reading", File.c_str());
        return "";
    }

    std::stringstream Buffer;
    Buffer << Input.rdbuf();

    return Buffer.str();
}

void WriteFile(const std::string& File, std::span<const uint8_t> Bytes)
{
    std::ofstream Output;
    Output.open(File);

    if (!Output.is_open())
    {
        lce::Common::ReportError(lce::Common::ErrorSeverity::Fatal, {}, "Cannot open output file {} for writing", File.c_str());
        return;
    }

    Output.write(reinterpret_cast<const char*>(Bytes.data()), Bytes.size_bytes());
}

int main(int ArgumentCount, char** Arguments)
{
    cxxopts::Options Options("Little Computer Assembler");
    Options.add_options()
        ("file", "The file to assemble", cxxopts::value<std::string>());
    Options.parse_positional("file");
    auto Result = Options.parse(ArgumentCount, Arguments);

    std::string InputFileName;
    try
    {
        InputFileName = Result["file"].as<std::string>();
    }
    catch (...)
    {
        std::cout << "No input file provided" << std::endl;
        Options.show_positional_help();
        return 1;
    }

    auto SourceCode = ReadFile(InputFileName);
    if (SourceCode.empty())
        return 0;

    auto Lexer = lce::Assembler::Lexer(SourceCode, InputFileName);
    std::vector<Instruction> Instructions;
    if (!Parse(Lexer, Instructions))
        return 0;
    auto Bytes = GenerateMachineCode(Instructions);

    WriteFile("123.bin", Bytes);
}
