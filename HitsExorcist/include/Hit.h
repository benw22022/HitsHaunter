#ifndef HIT_H
#define HIT_H

#include <iostream>

//TODO: This data structure is very inefficient, better to do a struct of vectors 
//TODO: rather than a vector of structs
class Hit {
public:
    double x, y, z;
    double charge, energy;
    int pdgc, layer, counter;

    Hit(double x = 0, double y = 0, double z = 0, double charge = 0, double energy = 0, int pdgc = 0, int layer = 0, int counter = 0)
        : x(x), y(y), z(z), charge(charge), energy(energy), pdgc(pdgc), layer(layer), counter(counter) {}

    bool operator==(const Hit& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator<(const Hit& other) const {
        return this->z < other.z;
    }

    friend std::ostream& operator<<(std::ostream& os, const Hit& hit);
};


#endif