#include "../include/Device.h"
#include <stdexcept>

// CONSTRUCTORS
// Used to create manual devices 
Device::Device(int id, std::string name, float energy) : ID(id), name(name), energy(energy){
    enTotal = 0; 
    active = false; 
    timeOn = nullptr; // Not initialized
    timer = nullptr; // Not initialized
}

// Used to create auto devices
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
// removeSchedule
void Device::removeSchedule(){
    delete timeOn; // Delete the timeOn
    timeOn = nullptr; 
}

// setSchedule 
void Device::setSchedule(Clock start){
    if(timeOn != nullptr){ // If the device is already scheduled, throw an exception
        throw std::invalid_argument("Dispositivo non disponibile"); 
    }
    timeOn = new Clock(start); // Set the timeOn 
}




// TURN ON THE DEVICE 
void Device::turnOn(Clock currTime){
    // If the device is already active, throw an exception
    if(active){
        throw std::invalid_argument("Dispositivo gia' acceso");
    }
    timeOn = new Clock(currTime); // Set turn on time 
    active = true; // Set the device as active
}

// TURN OFF THE DEVICE 
void Device::turnOff(Clock currTime){
    if(!active){
        throw std::invalid_argument("Dispositivo gia' spento");
    }
    Clock time = currTime - *timeOn; // Calcolo il tempo di accensione
    float hh = time.getHh() + time.getMm()/60; // Calcolo le ore di accensione   
    enTotal += energy * hh; // Calcolo il consumo energetico
    deactivate(); // Disattivo il dispositivo
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

