#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "RootReader.h"
#include "RootWriter.h"

// Function to parse command-line arguments
std::unordered_map<std::string, std::string> parseArgs(int argc, char* argv[]) {
    std::unordered_map<std::string, std::string> args;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.substr(0, 2) == "--") {
            std::string key = arg.substr(2);
            std::string value = "";
            if ((i + 1) < argc && argv[i + 1][0] != '-') {
                value = argv[++i];
            }
            args[key] = value;
        }
    }
    return args;
}

int main(int argc, char* argv[]) {
    // Parse the command-line arguments
    std::unordered_map<std::string, std::string> args = parseArgs(argc, argv);

    // Access and print the arguments
    for (const auto& [key, value] : args) {
        std::cout << "Argument: --" << key << ", Value: " << value << std::endl;
    }

    std::string inputFile;
    std::string outputFile;
    if (args.find("input") != args.end()) {
        inputFile = args["input"];
    }
    else
    {
        throw std::invalid_argument("ERROR: You must give an input file");
    }

    if (args.find("output") != args.end()) {
        std::string outputFile = args["output"];
    }
    else
    {
        throw std::invalid_argument("ERROR: You must give an output file");
    }

    RootReader reader{inputFile.c_str(), "Hits"};
    RootWriter writer{outputFile.c_str(), "Hits"};

    for (unsigned int event_idx{0}; event_idx < reader.get_nentries(); event_idx++)
    {
        
    }
    return 0;
}
