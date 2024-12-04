#include <stdexcept>
#include <map>
#include <set>
#include <map>
#include <unordered_map>

#include "RootReader.h"
#include "RootWriter.h"
// #include <Eigen/Dense>
// #include <Math/Minimizer.h>
// #include <Math/Functor.h>
// #include <Math/Factory.h>


RootWriter::RootWriter(const char* filename, const char* treename) :
    m_filename{filename}, m_treename{treename}
{
    // Open the ROOT file 
    m_file = TFile::Open(filename, "RECREATE"); 
    if (!m_file || m_file->IsZombie())
     { 
        std::cerr << "Error opening file: " << filename << std::endl; 
        throw std::invalid_argument("Error opening file: " + std::string(filename));
     } 
     
    // Create the TTree 
    m_tree = new TTree(treename, "Hits tree decorated with clusters");

    // Get number of entries
    m_nEntries = m_tree->GetEntries();
    m_currentEntry = 0;

    // Set branch addresses 
    m_tree->Branch("fEvent",        &m_event_number, "fEvent/I"); 
    m_tree->Branch("vertex_x",      &m_vertex_x, "vertex_x/D");
    m_tree->Branch("vertex_y",      &m_vertex_y, "vertex_y/D");
    m_tree->Branch("vertex_z",      &m_vertex_z, "vertex_z/D");
    m_tree->Branch("nu_E",          &m_nu_E, "nu_E/D");
    m_tree->Branch("nu_px",         &m_nu_px, "nu_px/D");
    m_tree->Branch("nu_py",         &m_nu_py, "nu_py/D");
    m_tree->Branch("nu_pz",         &m_nu_pz, "nu_pz/D");
    m_tree->Branch("nu_pdgc",       &m_nu_pdgc, "nu_pdgc/I");
    m_tree->Branch("target_pdgc",   &m_target_pdgc, "target_pdgc/I");
    m_tree->Branch("isCC",          &m_isCC, "isCC/I");
    m_tree->Branch("cclepton_pdgc", &m_cclepton_pdgc, "cclepton_pdgc/I");
    m_tree->Branch("cclepton_E",    &m_cclepton_E, "cclepton_E/D");
    m_tree->Branch("cclepton_px",   &m_cclepton_px, "cclepton_px/D");
    m_tree->Branch("cclepton_py",   &m_cclepton_py, "cclepton_py/D");
    m_tree->Branch("cclepton_pz",   &m_cclepton_pz, "cclepton_pz/D");
    m_tree->Branch("x",             &m_hits_x); 
    m_tree->Branch("y",             &m_hits_y); 
    m_tree->Branch("z",             &m_hits_z); 
    m_tree->Branch("E",             &m_hits_E); 
    m_tree->Branch("pdgc",          &m_hits_pdgc); 
    m_tree->Branch("charge",        &m_hits_charge); 
    m_tree->Branch("layer",         &m_hits_layernum); 
}

RootWriter::~RootWriter()
{   
    m_tree->Write();
    m_file->Close();
}

void RootWriter::write_event(RootReader& reader)
{
    m_currentEntry++;

    // Clear these vectors - else they'll just keep growing
    m_hits_x->clear();
    m_hits_y->clear();
    m_hits_z->clear();
    m_hits_E->clear();
    m_hits_pdgc->clear();
    m_hits_charge->clear();
    m_hits_layernum->clear();
    

    // Move data from reader into writer
    m_event_number = reader.m_event_number;
    m_vertex_x = reader.m_vertex_x;    
    m_vertex_y = reader.m_vertex_y;    
    m_vertex_z = reader.m_vertex_z;    
    m_nu_E =  reader.m_nu_E;        
    m_nu_px = reader.m_nu_px;       
    m_nu_py = reader.m_nu_py;       
    m_nu_pz = reader.m_nu_pz;       
    m_nu_pdgc = reader.m_nu_pdgc;     
    m_target_pdgc  = reader.m_target_pdgc; 
    m_isCC = reader.m_isCC;        
    m_cclepton_pdgc  = reader.m_cclepton_pdgc;
    m_cclepton_E  = reader.m_cclepton_E;
    m_cclepton_px = reader.m_cclepton_px;
    m_cclepton_py = reader.m_cclepton_py;
    m_cclepton_pz = reader.m_cclepton_pz;
    m_hits_x        = std::move(reader.m_hits_x);
    m_hits_y        = std::move(reader.m_hits_y);
    m_hits_z        = std::move(reader.m_hits_z);
    m_hits_E        = std::move(reader.m_hits_E);
    m_hits_pdgc     = std::move(reader.m_hits_pdgc);
    m_hits_charge   = std::move(reader.m_hits_charge);
    m_hits_layernum = std::move(reader.m_hits_layernum);

    // Set nullptrs for safety 
    reader.m_hits_x = nullptr;
    reader.m_hits_y = nullptr;
    reader.m_hits_z = nullptr;
    reader.m_hits_E = nullptr;
    reader.m_hits_pdgc = nullptr;
    reader.m_hits_charge = nullptr;
    reader.m_hits_layernum = nullptr;

    // sHIT HAPPENS HERE
    

    // std::cout << "Filling tree" << std::endl;
    m_tree->Fill();
}


