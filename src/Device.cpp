// TODO: gestione con smartpointer, controllo metodi 

#include "../include/Device.h"
#include <stdexcept>

// CONSTRUCTORS
// Used to create manual devices (manual devices don't have a default timer)
Device::Device(int id, std::string name, float energy) : ID(id), name(name), energy(energy){
    enTotal = 0; 
    active = false; 
    timeOn = nullptr; +
    timer = nullptr; 
}

// Used to create auto devices (auto devices have a default timer)
Device::Device(int id, std::string name, float energy, Clock timer) : ID(id), name(name), energy(energy), timer(new Clock(timer)){
    enTotal = 0; 
    active = false; 
    timeOn = nullptr; // Not initialized
}

// GETTERS
// getName
std::string Device::getName(){
    return name;
}

// getEnTotal
float Device::getEnTotal(){
    return enTotal;
}

// getEnergy
float Device::getEnergy(){
    return energy;
}

// getId
int Device::getId(){
    return ID;
}

// getTimeOn
Clock Device::getTimeOn(){
    return *timeOn;
}

// getTimer
Clock Device::getTimer(){
    return *timer;
}




// SETTERS
// setEnTotal
void Device::setEnTotal(float en){
    enTotal = en;
}




// SCHEDULE HANDLERS 
// removeSchedule (in order to remove the schedule remove the timeOn)
void Device::removeSchedule(){
    if(timeOn != nullptr){
        delete timeOn;
        timeOn = nullptr; 
    }
}

// setSchedule 
void Device::setSchedule(Clock start){
    // if the device is scheduled to an other time throw an exception
    if(timeOn != nullptr){
        throw std::invalid_argument("Dispositivo non disponibile"); 
    }
    timeOn = new Clock(start);
}




// TURN ON THE DEVICE 
void Device::turnOn(Clock currTime){
    // If the device is already on throw an exception
    if(active){
        throw std::invalid_argument("Dispositivo gia' acceso");
    }
    timeOn = new Clock(currTime);
    active = true;
}

// TURN OFF THE DEVICE 
void Device::turnOff(Clock currTime){
    // If the device is already off throw an exception
    if(!active){
        throw std::invalid_argument("Dispositivo gia' spento");
    }
    // Calculate the time of power on (in hours)
    Clock time = currTime - *timeOn; 
    float hh = time.getHh() + time.getMm()/60; 
    // Calculate the energy consumption
    enTotal += energy * hh;
    
    deactivate(); 
}




// PRINT THE DEVICE PRODUCTION/CONSUMPTION
std::string Device::show(){
    // If  enTotal > 0 the device has produced energy
    if(enTotal > 0){
        return "Il dispositivo " + name + " ha prodotto " + std::to_string(enTotal) + " Wh";
    }
    return "Il dispositivo " + name + " ha consumato " + std::to_string(enTotal) + " Wh";
}




// DEACTIVATE THE DEVICE
void Device::deactivate(){
    active = false;
    delete timeOn;
    timeOn = nullptr;
}




// ISACTIVE 
bool Device::isActive(){
    return active;
}




// REMOVE TIMER
void Device::removeTimer(){
    delete timer;
    timer = nullptr;
}



// DESTRUCTOR
Device::~Device(){
    delete timer;
    delete timeOn;
}



