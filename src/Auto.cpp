#include "../include/Auto.h"

// CONSTRUCTORS
Auto::Auto(int id, std::string name, float energy, Clock timer) : Device(id, name, energy){
    this->timer = new Clock(timer);
}

