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
    
    //* Main event loop
    for (unsigned int event_idx{0}; event_idx < reader.get_nentries(); event_idx++)
    {       
        Event event = reader.get_event(event_idx);

        std::cout << event_idx << "/" << reader.get_nentries() << ": " << event << std::endl;

        Event new_event = event;

        // if (event.event_number != 11855459) continue;
        // if (event.event_number != 62725516) continue;
        
        std::vector<int> hit_indices_to_erase{};
        
        int hit_index{0};
        int n_wrongly_tagged_hits{0};
        for (const auto& hit: event.hits)
        {
            int layer_of_hit = hit.layer; 
            bool remove_hit = true;
            double closest_distance = 1e10;

            for (const auto& another_hit: event.hits)
            {
                if (another_hit.layer == layer_of_hit + 1 || another_hit.layer == layer_of_hit - 1)
                {   
                    if (pow( pow(hit.x - another_hit.x, 2) + pow(hit.y - another_hit.y, 2), 0.5) < 2){
                        // std::cout << hit << " " << another_hit << std::endl;
                        remove_hit = false;
                        break;
                    }                    
                }
            }
            
            
            //* if there is no hit is within an adjacent layer within 10mm then it is a ghost to erase
            if (remove_hit)
            {   
                if (hit.pdgc != 0){
                    n_wrongly_tagged_hits++;
                    // std::cout << "INFO: Removing non-ghost hit! " << hit << " nearest hit was " << closest_distance << " mm away" << std::endl;
                }
                hit_indices_to_erase.push_back(hit_index);
            }
            hit_index++;
        }

        std::cout << "Removing " << hit_indices_to_erase.size() << " hits" << " of which " << n_wrongly_tagged_hits << " were wrongly classified as ghosts" << std::endl;
        double misid_eff = static_cast<double>(n_wrongly_tagged_hits) / static_cast<double>(hit_indices_to_erase.size());
        std::cout << "Mis-ID efficiency is " << misid_eff  * 100 << " %" << std::endl;

        // std::vector<Hit> new_hits{};
        // int n_kept{0};
        // int n_removed{0};
        // for (unsigned int i{0}; i < event.hits.size(); i++)
        // {   
        //     // std::cout << i << " / " << event.hits.size() << std::endl;
        //     bool remove_this{false};
        //     for (const int& index : hit_indices_to_erase)
        //     {   
        //         // std::cout << i << " ----  " << index << " / " << hit_indices_to_erase.size() << std::endl;
        //         if (i == index)
        //         {   

        //             n_removed++;
        //             remove_this = true;
        //             break;
        //         }
        //     }

        //     if (!remove_this){
        //         n_kept++;
                
                
        //         // if (abs(new_event.hits[i].x) > 30)
        //         // {
        //         //     std::cout << "Keeping " << new_event.hits[i] << std::endl;    
        //         // }

        //         new_hits.emplace_back(new_event.hits[i]);
        //     }
        // }

        // std::cout << "Removed: " << n_removed << "   Kept: " << n_kept << std::endl;
        
        // new_event.hits.clear();
        // new_event.hits = new_hits;

        deleteElementsByIndices(new_event.hits, hit_indices_to_erase);
        
        writer.write_event(new_event);
        // return 0;
    }

    return 0;
}
