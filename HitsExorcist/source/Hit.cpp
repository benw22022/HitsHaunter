#include "Hit.h"

std::ostream& operator<<(std::ostream& os, const Hit& hit) {
    os << "Hit(x=" << hit.x
       << ", y=" << hit.y
       << ", z=" << hit.z
       << ", charge=" << hit.charge
       << ", energy=" << hit.energy
       << ", pdgc=" << hit.pdgc
       << ", layer=" << hit.layer
       << ", counter=" << hit.counter
       << ")";
    return os;
}