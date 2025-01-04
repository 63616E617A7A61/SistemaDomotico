#include "../include/Manual.h"

// CONSTRUCTORS
Manual::Manual(int id, std::string name, float energy) : Device(id, name, energy){}

// SET TIMER
void Manual::setTimer(Clock timer){
    if(Device::timer != nullptr){
        throw std::invalid_argument("Timer gia' impostato");
    }
    Device::timer = new Clock(timer);
}
