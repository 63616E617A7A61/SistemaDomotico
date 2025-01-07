#include "../include/Device.h"
#include <stdexcept>

Device::Device(int id, std::string name, float energy) : ID(id), name(name), energy(energy){
    enTotal = 0; 
    active = false; 
    timeOn = nullptr; 
    timer = nullptr; 
    essential = false;
}

bool Device::isEssential(){
    return essential;
}

void Device::setEssential(bool val){
    essential = val;
}

std::string Device::getName(){
    return name;
}

float Device::getEnTotal(){
    return enTotal;
}

float Device::getEnergy(){
    return energy;
}

int Device::getId(){
    return ID;
}

/*
tipo di ritorno modificato cosi si puo' controllare da classi esterne se getTimeOn() != nullptr
*/
Clock Device::getTimeOn(){
    return *timeOn;
}

Clock Device::getTimer(){
    return *timer;
}

bool Device::timerExist(){
    if (timer != nullptr){
        return true;
    }
    return false;
}

void Device::setEnTotal(float en){
    enTotal = en;
}




// removeSchedule (in order to remove the schedule remove the timeOn)
void Device::removeSchedule(){
    if(timeOn != nullptr){
        delete timeOn;
        timeOn = nullptr; 
    }
}

void Device::setSchedule(Clock start){
    // if the device is scheduled to an other time 
    if(timeOn != nullptr){
        throw std::invalid_argument("Dispositivo non disponibile"); 
    }
    timeOn = new Clock(start);
}




void Device::turnOn(Clock currTime){
    // If the device is already on 
    // if(active == true){
    //     throw std::invalid_argument("Dispositivo gia' acceso");
    // }
    active = true;
    timeOn = new Clock(currTime);
}

void Device::turnOff(Clock currTime){
    // If the device is already off 
    // if(active == false){
    //     throw std::invalid_argument("Dispositivo gia' spento");
    // }
    enTotal += energy * ((currTime - *timeOn).getHh() +(currTime - *timeOn).getMm()/60.0);

    deactivate(); 
}

bool Device::check(Clock skipTime, Clock currTime){
    if(timeOn != nullptr && (*timeOn <= skipTime && *timeOn > currTime)){ // Device turn on | qua bisognerebbe controlloare se timeOn è maggiore di currTime (forse si puo gestire da house)
        return true;
    }
    else if(timeOn != nullptr && timer != nullptr && (*timeOn + *timer) <= skipTime){ // Device turn off
        return true;
    }
    return false; // No action
}


float Device::show(Clock currentTime){
    float tmpEn = enTotal + (active ? energy * ((currentTime - *timeOn).getHh() +(currentTime - *timeOn).getMm()/60.0) : 0);
    return tmpEn;
}

void Device::deactivate(){
    active = false;
    delete timeOn;
    timeOn = nullptr;
}

bool Device::isActive(){
    return active;
}

void Device::removeTimer(){
    delete timer;
    timer = nullptr;
}

Device::~Device(){
    delete timer;
    delete timeOn;
    timeOn = nullptr;
    timer = nullptr;
}
