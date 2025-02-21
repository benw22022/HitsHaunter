#include "Event.h"

std::ostream& operator<<(std::ostream& os, const Event& event) {
    os << "Event(nu_pdgc=" << event.nu_pdgc
       << ", nu_E=" << event.nu_E
       << ", isCC=" << event.isCC
       << ", nHits=" << event.hits.size()
       << ")";
    return os;
}