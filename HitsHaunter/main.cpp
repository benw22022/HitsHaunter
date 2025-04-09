#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Hit.h"
#include "StripModule.h"

#include <ROOT/RDataFrame.hxx>
#include <ROOT/RDFHelpers.hxx>


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


std::vector<Hit> processOneEvent(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z, const std::vector<double>& charge, const std::vector<double>& energy, const std::vector<int>& pdgc, const std::vector<int>& layer, const std::vector<SCTModule>& modules) {
    std::vector<Hit> new_hits{};
    std::vector<Hit> hits = {};

    for (unsigned int i{0}; i < x.size(); i++)
    {   
        Hit hit;
        hit.x = x.at(i);
        hit.y = y.at(i);
        hit.z = z.at(i);
        hit.pdgc = pdgc.at(i);
        hit.energy = energy.at(i);
        hit.charge = charge.at(i);
        hit.layer = layer.at(i);

        hits.push_back(hit);
    }

    for (const auto& module: modules)
    {   
        std::vector<Hit> digit_hits = module.digitizeHits(hits);
        new_hits.insert(new_hits.end(), digit_hits.begin(), digit_hits.end());
    }
    return new_hits;
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
    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df{"Hits", inputFile.c_str()};
    ROOT::RDF::Experimental::AddProgressBar(df);

    // auto df = df1.Range(100);
    
    
    //* Initialise SCT Modules
    double module_start_pos = 550 + 13.92/2;        // Starting z-position of SCT modules [mm]
    double module_offset = 13.92 + 7.08;//7.98;     // Offset between SCT modules [mm]
    std::vector<SCTModule> modules{};
    double width=63.56; 
    double length=128.05;
    int nlayers = 50;
    std::vector<std::pair<double, bool>> module_params{{0,false}, {M_PI/2,true}, {0, true}, {M_PI/2,true}};
    
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

    //* Digitize the hits
    auto resultDf = df.Define("processed_hits", [&](const std::vector<double>& x, const std::vector<double>& y,
        const std::vector<double>& z, const std::vector<double>& charge,
        const std::vector<double>& energy, const std::vector<int>& pdgc,
        const std::vector<int>& layer) 
        {
            return processOneEvent(x, y, z, charge, energy, pdgc, layer, modules);
        }, 
        {"x", "y", "z", "charge", "E", "pdgc", "layer"});
    
    
    // resultDf.Foreach([](const std::vector<Hit>& hits) {
    //     // std::cout << "Processed " << hits.size() << " hits.\n";
    // }, {"processed_hits"});


    resultDf = resultDf.Define("true_x", "x");
    resultDf = resultDf.Define("true_y", "y");
    resultDf = resultDf.Define("true_z", "z");
    resultDf = resultDf.Define("true_E", "E");
    resultDf = resultDf.Define("true_pdgc", "pdgc");
    resultDf = resultDf.Define("true_charge", "charge");
    resultDf = resultDf.Define("true_layer", "layer");
    

    resultDf = resultDf.Define("interaction_type", [](int isCC, int nu_pdgc) {
        
        std::vector<int> interaction_type{0, 0, 0, 0};
        
        if(isCC && nu_pdgc == 12) {
            interaction_type[0] = 1; // CC electron
        }
        else if(isCC && nu_pdgc == 14) {
            interaction_type[1] = 1; // CC muon
        }
        else if(isCC && nu_pdgc == 16) {
            interaction_type[2] = 1; // CC tau
        }
        else if(!isCC) {
            interaction_type[3] = 1; // NC 
        }
        return interaction_type;
    }, {"isCC", "nu_pdgc"});



    resultDf = resultDf.Define("digit_x", [](const std::vector<Hit>& hits) {
        std::vector<double> x;
        for (const auto& hit : hits) {
            x.push_back(hit.x);
        }
        return x;
    }, {"processed_hits"});
    resultDf = resultDf.Define("digit_y", [](const std::vector<Hit>& hits) {
        std::vector<double> y;
        for (const auto& hit : hits) {
            y.push_back(hit.y);
        }
        return y;
    }, {"processed_hits"});
    resultDf = resultDf.Define("digit_z", [](const std::vector<Hit>& hits) {
        std::vector<double> z;
        for (const auto& hit : hits) {
            z.push_back(hit.z);
        }
        return z;
    }, {"processed_hits"});
    resultDf = resultDf.Define("digit_pdgc", [](const std::vector<Hit>& hits) {
        std::vector<int> pdgc;
        for (const auto& hit : hits) {
            pdgc.push_back(hit.pdgc);
        }
        return pdgc;
    }, {"processed_hits"});
    resultDf = resultDf.Define("digit_E", [](const std::vector<Hit>& hits) {
        std::vector<double> E;
        for (const auto& hit : hits) {
            E.push_back(hit.energy);
        }
        return E;
    }, {"processed_hits"});
    resultDf = resultDf.Define("digit_charge", [](const std::vector<Hit>& hits) {
        std::vector<double> charge;
        for (const auto& hit : hits) {
            charge.push_back(hit.charge);
        }
        return charge;
    }, {"processed_hits"});
    resultDf = resultDf.Define("digit_layer", [](const std::vector<Hit>& hits) {
        std::vector<int> layer;
        for (const auto& hit : hits) {
            layer.push_back(hit.layer);
        }
        return layer;
    }, {"processed_hits"});
    resultDf = resultDf.Define("digit_counter", [](const std::vector<Hit>& hits) {
        std::vector<int> layer;
        for (const auto& hit : hits) {
            layer.push_back(hit.counter);
        }
        return layer;
    }, {"processed_hits"});
    

    resultDf.Snapshot("Hits", outputFile.c_str(), 
    {"fEvent",       
    "vertex_x",     
    "vertex_y",     
    "vertex_z",     
    "nu_E",         
    "nu_px",        
    "nu_py",        
    "nu_pz",        
    "nu_pdgc",      
    "target_pdgc",  
    "isCC",         
    "cclepton_pdgc",
    "cclepton_E",   
    "cclepton_px",  
    "cclepton_py",  
    "cclepton_pz",  
    "digit_x",            
    "digit_y",            
    "digit_z",            
    "digit_E",            
    "digit_pdgc",         
    "digit_charge",       
    "digit_layer",        
    "digit_counter",      
    "true_x",       
    "true_y",       
    "true_z",       
    "true_E",       
    "true_pdgc",    
    "true_charge",  
    "true_layer"}
    );

    return 0;
}
