#include "../include/Device.h"

/**
 * Constructor
 * @param id The ID of the device.
 * @param name The name of the device.
 * @param energy The energy consumption of the device.
 */
Device::Device(int id, std::string name, float energy) : ID(id), name(name), energy(energy){
    enTotal = 0; 
    active = false; 
    timeOn = nullptr; 
    timer = nullptr; 
    essential = false;
}

/**
 * Checks if the device is essential.
 * @return True if the device is essential, false otherwise.
 */
bool Device::isEssential() const{
    return essential;
}

/**
 * Sets the essential status of the device.
 * @param val The new essential status.
 */
void Device::setEssential(bool val){
    essential = val;
}

/**
 * Gets the name of the device.
 * @return The name of the device.
 */
std::string Device::getName() const{
    return name;
}

/**
 * Gets the total energy consumed by the device.
 * @return The total energy consumed.
 */
float Device::getEnTotal() const{
    return enTotal;
}

/**
 * Gets the energy consumption of the device.
 * @return The energy consumption.
 */
float Device::getEnergy() const{
    return energy;
}

/**
 * Gets the ID of the device.
 * @return The ID of the device.
 */
int Device::getId() const{
    return ID;
}

/**
 * Gets the time the device was turned on.
 * @return The time the device was turned on.
 */
Clock Device::getTimeOn() const{
    return *timeOn;
}

/**
 * Gets the timer of the device.
 * @return The timer of the device.
 */
Clock Device::getTimer() const{
    return *timer;
}

/**
 * Checks if the timer exists.
 * @return True if the timer exists, false otherwise.
 */
bool Device::timerExist() const{
    if (timer != nullptr){
        return true;
    }
    return false;
}

/**
 * Sets the total energy consumed by the device.
 * @param en The new total energy consumed.
 */
void Device::setEnTotal(float en){
    enTotal = en;
}

/**
 * Removes the schedule of the device.
 */
void Device::removeSchedule(){
    if(timeOn != nullptr){
        delete timeOn;
        timeOn = nullptr; 
    }
}

/**
 * Sets the schedule of the device.
 * @param start The start time of the schedule.
 * @throw std::invalid_argument if the device is already scheduled.
 */
void Device::setSchedule(Clock& start){
    if(timeOn != nullptr){
        throw std::invalid_argument("Device not available"); 
    }
    timeOn = new Clock(start);
}

/**
 * Turns on the device.
 * @param currTime The current time.
 */
void Device::turnOn(Clock& currTime){
    active = true;
    timeOn = new Clock(currTime);
}

/**
 * Turns off the device.
 * @param currTime The current time.
 */
void Device::turnOff(Clock& currTime){
    enTotal += energy * ((currTime - *timeOn).getHh() +(currTime - *timeOn).getMm()/60.0);
    deactivate(); 
}

/**
 * Checks the status of the device.
 * @param skipTime The time to skip to.
 * @param currTime The current time.
 * @return True if the device turned on or off, false otherwise.
 */
bool Device::check(Clock& skipTime, Clock& currTime){
    if(timeOn != nullptr && (*timeOn <= skipTime && *timeOn > currTime)){
        return true;
    }
    else if(timeOn != nullptr && timer != nullptr && (*timeOn + *timer) <= skipTime){
        return true;
    }
    return false;
}

/**
 * Shows the energy consumed by the device.
 * @param currentTime The current time.
 * @return The energy consumed.
 */
float Device::show(Clock& currentTime) const{
    float tmpEn = enTotal + (active ? energy * ((currentTime - *timeOn).getHh() +(currentTime - *timeOn).getMm()/60.0) : 0);
    return tmpEn;
}

/**
 * Deactivates the device.
 */
void Device::deactivate(){
    active = false;
    delete timeOn;
    timeOn = nullptr;
}

/**
 * Checks if the device is active.
 * @return True if the device is active, false otherwise.
 */
bool Device::isActive() const{
    return active;
}

/**
 * Removes the timer of the device.
 */
void Device::removeTimer(){
    delete timer;
    timer = nullptr;
}

/**
 * Destructor for the Device class.
 */
Device::~Device(){
    delete timer;
    delete timeOn;
    timeOn = nullptr;
    timer = nullptr;
}
