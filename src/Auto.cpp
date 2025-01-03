#include "../include/Auto.h"

// CONSTRUCTORS
Auto::Auto(int id, std::string name, float energy, Clock timer) : Device(id, name, energy, timer){
    this->timer = new Clock(timer);
}

// CHECK 
bool Auto::check(Clock skipTime){
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

Auto::~Auto() {
    delete timer; 
}
