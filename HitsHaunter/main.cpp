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

    //TODO: Don't hardcode this!
    std::string inputFile =  "FASERvSi_baseline-charmTrench-680fb.0.HITS.root";
    std::string outputFile = "test.root";

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

        if (abs(event.nu_pdgc) != 16) continue;


        // Centre hits
        for (auto&  hit: event.hits)
        {
            hit.x -= event.vertex_x;
            hit.y -= event.vertex_y;
            // hit.z -= event.vertex_z;
        }

        Event new_event = event;
        std::vector<Hit> new_hits{};  //TODO: Write out truth and digitized hits to seperate branches
        for (const auto& module: modules)
        {
            std::vector<Hit> digit_hits = module.digitizeHits(event.hits);

            if (event.event_number == 11855459)
            {   
                std::string figname = "11855459_hits_module_";
                figname = figname + std::to_string(module.getLayerNum()) + ".png";
                module.drawHitsOnModule(event.hits, 1, 1, figname);
            }


            new_hits.insert(new_hits.end(), digit_hits.begin(), digit_hits.end());
        }

        new_event.hits = new_hits;
        
        writer.write_event(new_event);
    }

    return 0;
}
