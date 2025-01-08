#include "../include/Auto.h"

/**
 * Constructor. 
 * @param id The ID of the auto device.
 * @param name The name of the auto device.
 * @param energy The energy consumption of the auto device.
 * @param timer The timer for the auto device.
 */
Auto::Auto(int id, std::string name, float energy, Clock timer) : Device(id, name, energy){
    this->timer = new Clock(timer);
}

