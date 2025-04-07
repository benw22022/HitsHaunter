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
        outputFile = args["output"];
    }
    else
    {
        throw std::invalid_argument("ERROR: You must give an output file");
    }

    // Create the RootReader and RootWriter objects
    RootReader reader{inputFile.c_str(), "Hits"};
    RootWriter writer{outputFile.c_str(), "Hits"};
    
    //* Initialise SCT Modules
    double module_start_pos = 550 + 13.92/2;        // Starting z-position of SCT modules [mm]
    double module_offset = 13.92 + 7.08;//7.98;     // Offset between SCT modules [mm]
    std::vector<SCTModule> modules{};
    double width=63.56; 
    double length=128.05;
    int nlayers = 50;
    std::vector<std::pair<double, bool>> module_params{{0,false}, {M_PI/2,true}, {0, true}, {M_PI/2,true}};
    // std::vector<std::pair<double, double>> module_offsets{{0.5, 0.5}, {0.5, 1.5}, {0.5, 2.5}, {0.5, 3.5},
    //                                                       {0.5, -0.5}, {0.5, -1.5}, {0.5,-2.5}, {0.5, -3.5}, 
    //                                                       {-0.5, 0.5}, {-0.5, 1.5}, {-0.5, 2.5}, {-0.5, 3.5},
    //                                                       {-0.5, -0.5}, {-0.5, -1.5}, {-0.5,-2.5}, {-0.5, -3.5}};
    
    for (int i{0}; i < nlayers; i++)
    {   
        int index = i % module_params.size();
        double module_zpos = module_start_pos + module_offset * i;
        double rot = module_params[index].first;
        bool flip = module_params[index].second;
       
        //* Vertical modules 4 x 4 array
        if (rot == 0)
        {
            modules.push_back(SCTModule(0.5*width, 0.5*length, module_zpos, i, module_params[index].first, module_params[index].second));
            modules.push_back(SCTModule(0.5*width, 1.5*length, module_zpos, i, module_params[index].first, module_params[index].second));

            modules.push_back(SCTModule(0.5*width, -0.5*length, module_zpos, i, module_params[index].first, module_params[index].second));
            modules.push_back(SCTModule(0.5*width, -1.5*length, module_zpos, i, module_params[index].first, module_params[index].second));

            modules.push_back(SCTModule(-0.5*width, 0.5*length, module_zpos, i, module_params[index].first, module_params[index].second));
            modules.push_back(SCTModule(-0.5*width, 1.5*length, module_zpos, i, module_params[index].first, module_params[index].second));

            modules.push_back(SCTModule(-0.5*width, -0.5*length, module_zpos, i, module_params[index].first, module_params[index].second));
            modules.push_back(SCTModule(-0.5*width, -1.5*length, module_zpos, i, module_params[index].first, module_params[index].second));

            modules.push_back(SCTModule(1.5*width, 0.5*length, module_zpos, i, module_params[index].first, module_params[index].second));
            modules.push_back(SCTModule(1.5*width, 1.5*length, module_zpos, i, module_params[index].first, module_params[index].second));

            modules.push_back(SCTModule(1.5*width, -0.5*length, module_zpos, i, module_params[index].first, module_params[index].second));
            modules.push_back(SCTModule(1.5*width, -1.5*length, module_zpos, i, module_params[index].first, module_params[index].second));

            modules.push_back(SCTModule(-1.5*width, 0.5*length, module_zpos, i, module_params[index].first, module_params[index].second));
            modules.push_back(SCTModule(-1.5*width, 1.5*length, module_zpos, i, module_params[index].first, module_params[index].second));

            modules.push_back(SCTModule(-1.5*width, -0.5*length, module_zpos, i, module_params[index].first, module_params[index].second));
            modules.push_back(SCTModule(-1.5*width, -1.5*length, module_zpos, i, module_params[index].first, module_params[index].second));            
        }
        //* Horizontal modules 2 x 8 array
        if (rot != 0){           

            modules.push_back(SCTModule(0.5*length, 0.5*width, module_zpos, i, module_params[index].first, module_params[index].second));
            modules.push_back(SCTModule(0.5*length, 1.5*width, module_zpos, i, module_params[index].first, module_params[index].second));
            modules.push_back(SCTModule(0.5*length, 2.5*width, module_zpos, i, module_params[index].first, module_params[index].second));
            modules.push_back(SCTModule(0.5*length, 3.5*width, module_zpos, i, module_params[index].first, module_params[index].second));
            
            modules.push_back(SCTModule(-0.5*length, 0.5*width, module_zpos, i, module_params[index].first, module_params[index].second));
            modules.push_back(SCTModule(-0.5*length, 1.5*width, module_zpos, i, module_params[index].first, module_params[index].second));
            modules.push_back(SCTModule(-0.5*length, 2.5*width, module_zpos, i, module_params[index].first, module_params[index].second));
            modules.push_back(SCTModule(-0.5*length, 3.5*width, module_zpos, i, module_params[index].first, module_params[index].second));

            modules.push_back(SCTModule(0.5*length, -0.5*width, module_zpos, i, module_params[index].first, module_params[index].second));
            modules.push_back(SCTModule(0.5*length, -1.5*width, module_zpos, i, module_params[index].first, module_params[index].second));
            modules.push_back(SCTModule(0.5*length, -2.5*width, module_zpos, i, module_params[index].first, module_params[index].second));
            modules.push_back(SCTModule(0.5*length, -3.5*width, module_zpos, i, module_params[index].first, module_params[index].second));
            
            modules.push_back(SCTModule(-0.5*length, -0.5*width, module_zpos, i, module_params[index].first, module_params[index].second));
            modules.push_back(SCTModule(-0.5*length, -1.5*width, module_zpos, i, module_params[index].first, module_params[index].second));
            modules.push_back(SCTModule(-0.5*length, -2.5*width, module_zpos, i, module_params[index].first, module_params[index].second));
            modules.push_back(SCTModule(-0.5*length, -3.5*width, module_zpos, i, module_params[index].first, module_params[index].second));
        }


    }

    
    //* Main event loop
    for (unsigned int event_idx{0}; event_idx < reader.get_nentries(); event_idx++)
    {       
        if (event_idx > 100) break;

        Event event = reader.get_event(event_idx);

        std::cout << event_idx << "/" << reader.get_nentries() << ": " << event << std::endl;

        // if (abs(event.nu_pdgc) != 16) continue;

        // Centre hits
        // for (auto&  hit: event.hits)
        // {
        //     hit.x -= event.vertex_x;
        //     hit.y -= event.vertex_y;
        //     // hit.z -= event.vertex_z;
        // }

        // for (auto&  hit: event.true_hits)
        // {
        //     hit.x -= event.vertex_x;
        //     hit.y -= event.vertex_y;
        //     // hit.z -= event.vertex_z;
        // }

        Event new_event = event;
        std::vector<Hit> new_hits{};  //TODO: Write out truth and digitized hits to seperate branches
        for (const auto& module: modules)
        {
            std::vector<Hit> digit_hits = module.digitizeHits(event.hits);
            new_hits.insert(new_hits.end(), digit_hits.begin(), digit_hits.end());
        }

        new_event.hits = new_hits;
        new_event.true_hits = event.true_hits;
        
        writer.write_event(new_event);
    }

    return 0;
}
