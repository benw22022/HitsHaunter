#ifndef ROOTREADER_H
#define ROOTREADER_H

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <iostream>
#include <vector>

#include "RootWriter.h"
#include "StripModule.h"
// #include "Event.h"

class Event;

class RootReader {

    public:
        RootReader(const char* filename, const char* treename);
        ~RootReader();
        
        Long64_t get_nentries() const;
        Event get_event(Long64_t entry_idx);

        friend class RootWriter;
        friend class Event;

    private:
        void get_entry(Long64_t entry_idx);

        const char* m_filename;
        const char* m_treename;

        TFile* m_file;
        TTree* m_tree;

        Long64_t m_nEntries;
        Long64_t m_currentEntry{0};

        std::vector<int> m_neutral_particle_hit_idxs{};
        int m_nhits{0};

        // Event info        
        int    m_event_number = 0;
        double m_vertex_x = 0.0;    
        double m_vertex_y = 0.0;    
        double m_vertex_z = 0.0;    
        double m_nu_E =  0.0;        
        double m_nu_px = 0.0;       
        double m_nu_py = 0.0;       
        double m_nu_pz = 0.0;       
        int    m_nu_pdgc = 0;     
        int    m_target_pdgc  = 0; 
        int    m_isCC = 0;        
        int    m_cclepton_pdgc  = 0;
        double m_cclepton_E  = 0.0;
        double m_cclepton_px = 0.0;
        double m_cclepton_py = 0.0;
        double m_cclepton_pz = 0.0;
        std::vector<double>* m_hits_x = nullptr;
        std::vector<double>* m_hits_y = nullptr;
        std::vector<double>* m_hits_z = nullptr;
        std::vector<double>* m_hits_E = nullptr;
        std::vector<int>*    m_hits_pdgc = nullptr;
        std::vector<double>* m_hits_charge = nullptr;
        std::vector<int>*    m_hits_layernum = nullptr;

        double m_vx;

};

#endif