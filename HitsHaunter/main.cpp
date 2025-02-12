#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "RootReader.h"
#include "RootWriter.h"
#include "Event.h"

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
    // std::unordered_map<std::string, std::string> args = parseArgs(argc, argv);

    // // Access and print the arguments
    // for (const auto& [key, value] : args) {
    //     std::cout << "Argument: --" << key << ", Value: " << value << std::endl;
    // }

    // std::string inputFile;
    // std::string outputFile;
    // if (args.find("input") != args.end()) {
    //     inputFile = args["input"];
    // }
    // else
    // {
    //     throw std::invalid_argument("ERROR: You must give an input file");
    // }

    // if (args.find("output") != args.end()) {
    //     std::string outputFile = args["output"];
    // }
    // else
    // {
    //     throw std::invalid_argument("ERROR: You must give an output file");
    // }

    // std::cout << "Opening input file: " << inputFile << std::endl;
    // std::cout << "Creating output file: " << outputFile << std::endl;

    std::string inputFile =  "FASERvSi_baseline-charmTrench-680fb.0.HITS.root";
    std::string outputFile = "test.root";

    RootReader reader{inputFile.c_str(), "Hits"};
    RootWriter writer{outputFile.c_str(), "Hits"};
    
    Event event = reader.get_event(54);

    std::cout << event << std::endl;

    for (auto&  hit: event.hits)
    {
        hit.x -= event.vertex_x;
        hit.y -= event.vertex_y;
        hit.z -= event.vertex_z;
    }

    SCTModule module(0., 0., 0., 78, M_PI/2, false);
    std::cout << module << std::endl;

    module.drawHitsOnModule(event.hits);


    // for (unsigned int event_idx{0}; event_idx < reader.get_nentries(); event_idx++)
    // {   
        
    //     Event event = reader.get_event(event_idx);
    //     SCTModule module(0., 0., 0., 0);

    //     module.drawHitsOnModule(event.hits);


    //     return 0;
    // }
    return 0;
}
