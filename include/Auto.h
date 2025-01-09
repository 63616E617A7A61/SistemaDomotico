// @author Nicola Zillio

#ifndef AUTO
#define AUTO

#include "Device.h"

class Auto : public Device{
    
public:
    Auto(int id, std::string name, float energy, Clock timer);
};
    
#endif