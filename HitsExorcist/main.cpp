#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include<algorithm>

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


void deleteElementsByIndices(std::vector<Hit>& vec, const std::vector<int>& indices) {
    // Create a copy of indices and sort it in descending order
    std::vector<int> sortedIndices = indices;
    std::sort(sortedIndices.rbegin(), sortedIndices.rend());

    // Iterate over the sorted indices and remove the elements from the vector
    for (int index : sortedIndices) {
        if (index >= 0 && index < vec.size()) {
            vec.erase(vec.begin() + index);
        }
    }
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

    //TODO: Don't hardcode this!
    std::string inputFile =  "../build/test.root";
    std::string outputFile = "test-exorcised.root";

    RootReader reader{inputFile.c_str(), "Hits"};
    RootWriter writer{outputFile.c_str(), "Hits"};
    
    //* Initialise SCT Modules
    double module_start_pos = 550 + 0.9/2;                // Starting z-position of SCT modules [mm]
    double module_offset = 7.98;                  // Offset between SCT modules [mm]
    std::vector<SCTModule> modules{};
    std::vector<std::pair<double, bool>> module_params{{0,false}, {M_PI/2,true}, {0, true}, {M_PI/2,true}};
    
    for (int i{0}; i < 132; i++)
    {   
        double rotation{0};
        bool flip_module{false};
        int index = i % module_params.size();
        double module_zpos = module_start_pos + module_offset * i;

        SCTModule module{0, 0, module_zpos, i, module_params[index].first, module_params[index].second};
        modules.push_back(module);
    }

    
    //* Main event loop
    for (unsigned int event_idx{0}; event_idx < reader.get_nentries(); event_idx++)
    {       
        Event event = reader.get_event(event_idx);

        std::cout << event_idx << "/" << reader.get_nentries() << ": " << event << std::endl;

        Event new_event = event;
        
        std::vector<int> hit_indices_to_erase{};
        int hit_index{0};
        for (const auto& hit: event.hits)
        {
            int layer_of_hit = hit.layer; 

            for (const auto& another_hit: event.hits)
            {
                if (another_hit.layer != layer_of_hit + 1 && another_hit.layer != layer_of_hit - 1) continue;
                if (std::hypot(hit.x - another_hit.x, hit.y - another_hit.y) < 10) continue;
                
                //* if there is no hit is within an adjacent layer within 10mm then it is a ghost to erase
                
                hit_indices_to_erase.push_back(hit_index);
    
            }
        }

         deleteElementsByIndices(new_event.hits, hit_indices_to_erase);
        
        writer.write_event(new_event);
    }

    return 0;
}
