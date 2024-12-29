#include "../include/Manual.h"

// CONSTRUCTORS
Manual::Manual(int id, std::string name, float energy) : Device(id, name, energy){}

// CHECK (Caso on-off-on e off-on-off gestisce house?)
bool Manual::check(Clock skipTime){
    if((Device::getTimeOn() <= skipTime && skipTime <= Device::getTimeOn() + Device::getTimer()) && !Device::isActive()){ // Device turn on
        Device::turnOn(skipTime);
        return true;
    }
    else if(Device::getTimeOn() + Device::getTimer() < skipTime && Device::isActive()){ // Device turn off
        Device::turnOff(skipTime);
        return true;
    }
    return false; // No action
}

// SET TIMER
void Manual::setTimer(Clock timer){
    if(Device::timer != nullptr){
        throw std::invalid_argument("Timer gia' impostato");
    }
    Device::timer = new Clock(timer);
}