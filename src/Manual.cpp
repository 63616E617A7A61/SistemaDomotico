// @author Nicola Zillio

#include "../include/Manual.h"

/**
 * Constructor.
 * @param id The ID of the manual device.
 * @param name The name of the manual device.
 * @param energy The energy consumption of the manual device.
 */
Manual::Manual(int id, std::string name, float energy) : Device(id, name, energy){}

/**
 * Sets the timer for the manual device.
 * @param time The time to set the timer to.
 * @throw std::invalid_argument if the timer is already set.
 */
void Manual::setTimer(Clock time){
    if(timer != nullptr){
        throw std::invalid_argument("Timer already set");
    }
    timer = new Clock(time);
}
