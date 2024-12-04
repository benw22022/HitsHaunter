// ROOT Dictionary magic
#ifdef __MAKECINT__
#pragma link C++ class std::vector < std::vector<int> >+;   
#pragma link C++ class std::vector < std::vector<double> >+;   
// #pragma link C++ class std::map<int , std::vector<Point>>+;   
#pragma link C++ class std::map<int , std::vector<int>>+;   
#endif

#ifndef ROOTWRITER_H
#define ROOTWRITER_H

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <iostream>
#include <vector>

#include "RootWriter.h"
// #include <Eigen/Dense> 


class RootReader;

class RootWriter {

    public:
        RootWriter(const char* filename, const char* treename);
        ~RootWriter();

        void write_event(RootReader& reader);
        
    private:

        const char* m_filename;
        const char* m_treename;

        TFile* m_file;
        TTree* m_tree;

        Long64_t m_nEntries;
        Long64_t m_currentEntry{0};

        
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
        
};

#endif