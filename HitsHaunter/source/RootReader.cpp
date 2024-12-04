#include "RootReader.h"
#include <stdexcept>


RootReader::RootReader(const char* filename, const char* treename) :
    m_filename{filename}, m_treename{treename}
{
    // Open the ROOT file 
    m_file = TFile::Open(filename, "READ"); 
    if (!m_file || m_file->IsZombie())
     { 
        std::cerr << "Error opening file: " << filename << std::endl; 
        throw std::invalid_argument("Error opening file: " + std::string(filename));
     } 
     
    // Access the TTree 
    m_tree = nullptr; 
    m_file->GetObject(treename, m_tree); 
    if (!m_tree) 
    { 
        std::cerr << "Error: TTree not found in file: " << filename << std::endl; 
        m_file->Close();
        throw std::invalid_argument("Error reading TTree: " + std::string(treename));
    return; 
    }

    // Get number of entries
    m_nEntries = m_tree->GetEntries();
    m_currentEntry = 0;

    // Set branch addresses 
    m_tree->SetBranchAddress("fEvent",        &m_event_number); 
    m_tree->SetBranchAddress("vertex_x",      &m_vertex_x);
    m_tree->SetBranchAddress("vertex_y",      &m_vertex_y);
    m_tree->SetBranchAddress("vertex_z",      &m_vertex_z);
    m_tree->SetBranchAddress("nu_E",          &m_nu_E);
    m_tree->SetBranchAddress("nu_px",         &m_nu_px);
    m_tree->SetBranchAddress("nu_py",         &m_nu_py);
    m_tree->SetBranchAddress("nu_pz",         &m_nu_pz);
    m_tree->SetBranchAddress("nu_pdgc",       &m_nu_pdgc);
    m_tree->SetBranchAddress("target_pdgc",   &m_target_pdgc);
    m_tree->SetBranchAddress("isCC",          &m_isCC);
    m_tree->SetBranchAddress("cclepton_pdgc", &m_cclepton_pdgc);
    m_tree->SetBranchAddress("cclepton_E",    &m_cclepton_E);
    m_tree->SetBranchAddress("cclepton_px",   &m_cclepton_px);
    m_tree->SetBranchAddress("cclepton_py",   &m_cclepton_py);
    m_tree->SetBranchAddress("cclepton_pz",   &m_cclepton_pz);
    m_tree->SetBranchAddress("x",             &m_hits_x);
    m_tree->SetBranchAddress("y",             &m_hits_y);
    m_tree->SetBranchAddress("z",             &m_hits_z);
    m_tree->SetBranchAddress("E",             &m_hits_E);
    m_tree->SetBranchAddress("pdgc",          &m_hits_pdgc);
    m_tree->SetBranchAddress("charge",        &m_hits_charge);
    m_tree->SetBranchAddress("layer",         &m_hits_layernum);
}

RootReader::~RootReader()
{
    m_file->Close();
}


void RootReader::get_entry(Long64_t entry_idx)
{
    if (entry_idx >= m_nEntries)
    {
        std::cerr << "ERROR: Index " << entry_idx << " is out of bound for tree of size " << m_nEntries << std::endl;
        throw std::invalid_argument("ERROR: Index out of bounds");
    }
    
    m_tree->GetEntry(entry_idx);
}


Long64_t RootReader::get_nentries() const
{
    return m_nEntries;
}