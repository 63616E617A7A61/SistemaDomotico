// @author Nicola Zillio

#ifndef MANUAL
#define MANUAL

#include "Device.h"

class Manual : public Device{
private:
    
public:
    Manual(int id, std::string name, float energy);
    void setTimer(Clock timer);
};

#endif