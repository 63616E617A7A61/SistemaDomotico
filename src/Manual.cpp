#include "../include/Manual.h"

// CONSTRUCTORS
Manual::Manual(int id, std::string name, float energy) : Device(id, name, energy){}

// SET TIMER
void Manual::setTimer(Clock time){
    if(timer != nullptr){
        throw std::invalid_argument("Timer gia' impostato");
    }
    timer = new Clock(time);
}
