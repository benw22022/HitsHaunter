
#ifndef EVENT_H
#define EVENT_H

#include "RootReader.h"
#include "StripModule.h"

// class RootReader;

class Event {

    public:
        int    event_number = 0;
        double vertex_x = 0.0;    
        double vertex_y = 0.0;    
        double vertex_z = 0.0;    
        double nu_E =  0.0;        
        double nu_px = 0.0;       
        double nu_py = 0.0;       
        double nu_pz = 0.0;       
        int    nu_pdgc = 0;     
        int    target_pdgc  = 0; 
        int    isCC = 0;        
        int    cclepton_pdgc  = 0;
        double cclepton_E  = 0.0;
        double cclepton_px = 0.0;
        double cclepton_py = 0.0;
        double cclepton_pz = 0.0;
        std::vector<Hit> hits;

        Event(const RootReader& reader)
        {
            event_number = reader.m_event_number;
            vertex_x = reader.m_vertex_x;    
            vertex_y = reader.m_vertex_y;    
            vertex_z = reader.m_vertex_z;    
            nu_E =  reader.m_nu_E;   
            nu_px = reader.m_nu_px;   
            nu_py = reader.m_nu_py;   
            nu_pz = reader.m_nu_pz;   
            nu_pdgc = reader.m_nu_pdgc;     
            target_pdgc = reader.m_target_pdgc; 
            isCC = reader.m_isCC;        
            cclepton_pdgc  = reader.m_cclepton_pdgc;
            cclepton_E  = reader.m_cclepton_E;
            cclepton_px = reader.m_cclepton_px;
            cclepton_py = reader.m_cclepton_py;
            cclepton_pz = reader.m_cclepton_pz;
            
            for (unsigned int i{0}; i < reader.m_hits_x->size(); i++)
            {
                Hit hit;
                hit.x = reader.m_hits_x->at(i);
                hit.y = reader.m_hits_y->at(i);
                hit.z = reader.m_hits_z->at(i);
                hit.pdgc = reader.m_hits_pdgc->at(i);
                hit.energy = reader.m_hits_E->at(i);
                hit.charge = reader.m_hits_charge->at(i);
                hit.layer = reader.m_hits_layernum->at(i);
                hits.push_back(hit);
            }
        }

        Event(const Event& other) {
            event_number = other.event_number;
            vertex_x = other.vertex_x;
            vertex_y = other.vertex_y;
            vertex_z = other.vertex_z;
            nu_E = other.nu_E;
            nu_px = other.nu_px;
            nu_py = other.nu_py;
            nu_pz = other.nu_pz;
            nu_pdgc = other.nu_pdgc;
            target_pdgc = other.target_pdgc;
            isCC = other.isCC;
            cclepton_pdgc = other.cclepton_pdgc;
            cclepton_E = other.cclepton_E;
            cclepton_px = other.cclepton_px;
            cclepton_py = other.cclepton_py;
            cclepton_pz = other.cclepton_pz;
            hits = other.hits;
        }

        template <typename T, typename MemberType>
        std::vector<MemberType> getHitValues(MemberType Hit::* member) const {
            std::vector<MemberType> values;
            for (const auto& hit : hits) {
                values.push_back(hit.*member);
            }
            return values;
        }




        friend std::ostream& operator<<(std::ostream& os, const Event& event);
};

#endif